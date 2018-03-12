#include <assert.h>
#include <string.h>
#include "cns_block.h"

cns_block *cns_block_create(size_t len, cns_dtype dtype, uint8_t width)
{
	assert(len > 0 && len <= CNS_MAX_CELLS);
	cns_cell *cells;
	cns_block *block;
	size_t i;

	block = (cns_block *)cns_alloc(sizeof(cns_block));
	block->rbuf_i = cns_reg_buf_create(len, dtype);
	block->rbuf_o = cns_reg_buf_create(len, dtype);
	block->rbuf_w = cns_reg_buf_create(len, dtype);
	block->rbuf_c = cns_reg_buf_create(len, dtype);
	block->wbuf_c = cns_wire_buf_create(len, dtype);
	cells = (cns_cell *)cns_alloc(sizeof(cns_cell) * len);
	block->cells = cells;
	block->len = len;
	block->dtype = dtype;
	block->width = width;

	for (i = 0; i < len; i++) {
		cells[i].data.width = width;
		cells[i].data.dtype = dtype;
		cells[i].data.input = NULL;
		cells[i].data.weight = NULL;
		cells[i].data.output = NULL;
		cells[i].index = i;
		cells[i].deps = NULL;
		cells[i].op = NULL;
		cells[i].en = CNS_FALSE;
	}

	return block;
}

void cns_block_free(cns_block *block)
{
	assert(block);
	cns_reg_buf_free(block->rbuf_i);
	cns_reg_buf_free(block->rbuf_o);
	cns_reg_buf_free(block->rbuf_w);
	cns_reg_buf_free(block->rbuf_c);
	cns_wire_buf_free(block->wbuf_c);
	cns_free(block->cells);
	cns_free(block);
}

void cns_block_run(cns_block *block, cns_list *run_list)
{
	assert(block && block->cells && run_list);
	ssize_t idx;
	cns_list *rl, *sub_rl;

	memset(block->rbuf_c->buf, 0, cns_size_of(block->dtype)*block->len);
	memset(block->wbuf_c->buf, 0, cns_size_of(block->dtype)*block->len);
	for (rl = run_list; rl; rl = rl->next) {
		sub_rl = (cns_list *)rl->data;
		/* TODO: need to be parallized */
		for (; sub_rl; sub_rl = sub_rl->next) {
			idx = (ssize_t)sub_rl->data;
			if (idx < 0)
				continue;
			cns_cell_run(&block->cells[idx]);
		}
	}
}

void cns_block_set_data(cns_block *block, size_t index,
			void *input, void *weight, void *output)
{
	cns_cell_set_data(&block->cells[index], input, weight, output);
}

void cns_block_set_width(cns_block *block, size_t index, uint8_t width)
{
	cns_cell_set_width(&block->cells[index], width);
}

void cns_block_set_dtype(cns_block *block, size_t index, int dtype)
{
	cns_cell_set_dtype(&block->cells[index], dtype);
}

void cns_block_set_op(cns_block *block, size_t index, cns_cell_op op)
{
	cns_cell_set_op(&block->cells[index], op);
}

void cns_block_set_en(cns_block *block, size_t index, cns_bool_t en)
{
	cns_cell_set_en(&block->cells[index], en);
}

void cns_block_add_dep(cns_block *block, size_t index, ssize_t dep)
{
	cns_cell_add_dep(&block->cells[index], dep);
}

cns_graph *cns_block_dep_graph(cns_block *block)
{
	cns_graph *g;
	cns_list *l;
	size_t i;
	ssize_t dep;

	g = cns_graph_create();
	cns_graph_add(g, (void *)-1);
	for (i = 0; i < block->len; i++) {
		if (!block->cells[i].en)
			continue;
		cns_graph_add(g, (void *)i);
	}
	for (i = 0; i < block->len; i++) {
		if (!block->cells[i].en)
			continue;
		for (l = block->cells[i].deps; l; l = l->next) {
			dep = (ssize_t)l->data;
			if (dep == -1 || block->cells[dep].en)
				cns_graph_link(g, (void *)dep, (void *)i);
		}
	}

	return g;
}

void **cns_block_find_itfp(cns_block *block, size_t idx, int itft)
{
	void **itfp;

	if (idx >= block->len)
		cns_err_quit("ERROR: cns_block_find_itfp: cell array out of bound\n");

	switch (itft) {
	case CNS_INPUT:
		itfp = &block->cells[idx].data.input;
		break;
	case CNS_OUTPUT:
		itfp = &block->cells[idx].data.output;
		break;
	case CNS_WEIGHT:
		itfp = &block->cells[idx].data.weight;
		break;
	default:
		cns_err_quit("ERROR: cns_block_find_itfp: unknown cns_interface_type %d\n",
			itft);
	}

	return itfp;
}

void cns_block_link(cns_block *block, size_t idx1, int itft1, size_t idx2, int itft2)
{
	void **itfp1;
	void **itfp2;
	int b_idx, b_idx1, b_idx2;

	itfp1 = cns_block_find_itfp(block, idx1, itft1);
	itfp2 = cns_block_find_itfp(block, idx2, itft2);

	if (itft1 == CNS_OUTPUT && (itft2 == CNS_INPUT || itft2 == CNS_WEIGHT)
		&& idx1 != idx2)
		cns_block_add_dep(block, idx2, idx1);
	if (itft2 == CNS_OUTPUT && (itft1 == CNS_INPUT || itft1 == CNS_WEIGHT)
		&& idx1 != idx2)
		cns_block_add_dep(block, idx1, idx2);

	if (*itfp1 && !*itfp2) {
		b_idx = cns_wire_buf_index(block->wbuf_c, *itfp1);
		*itfp2 = cns_wire_buf_link(block->wbuf_c, b_idx, idx2, itft2);
		return;
	}
	if (!*itfp1 && *itfp2) {
		b_idx = cns_wire_buf_index(block->wbuf_c, *itfp2);
		*itfp1 = cns_wire_buf_link(block->wbuf_c, b_idx, idx1, itft1);
		return;
	}
	/* Shouldn't enter this branch in practice,
	   causing an dangling interface easily. */
	if (*itfp1 && *itfp2) {
		cns_err_msg("WARNING: cns_block_link: linking two attached interfaces\n");
		fprintf(stderr,
			);
		if (*itfp1 == *itfp2)
			return;
		b_idx1 = cns_wire_buf_index(block->wbuf_c, *itfp1);
		b_idx2 = cns_wire_buf_index(block->wbuf_c, *itfp2);
		if (b_idx1 < b_idx2) {
			*itfp2 = cns_wire_buf_link(block->wbuf_c, b_idx1, idx2, itft2);
			cns_wire_buf_unlink(block->wbuf_c, b_idx2, idx2, itft2);
		} else {
			*itfp1 = cns_wire_buf_link(block->wbuf_c, b_idx2, idx1, itft1);
			cns_wire_buf_unlink(block->wbuf_c, b_idx1, idx1, itft1);
		}
		return;
	}
	*itfp1 = cns_wire_buf_append(block->wbuf_c, idx1, itft1);
	*itfp2 = cns_wire_buf_link(block->wbuf_c, block->wbuf_c->head-1, idx2, itft2);
}

void cns_block_link_io(cns_block *block, size_t ori, cns_list *iis, int itft)
{
	int i;
	cns_list *l;
	cns_ii *ii;
	void *buf;
	void **itfp;

	switch (itft) {
	case CNS_INPUT:
		buf = cns_reg_buf_link(block->rbuf_i, ori, iis);
		break;
	case CNS_OUTPUT:
		buf = cns_reg_buf_link(block->rbuf_o, ori, iis);
		break;
	case CNS_WEIGHT:
		buf = cns_reg_buf_link(block->rbuf_w, ori, iis);
		break;
	default:
		cns_err_quit("ERROR: cns_block_link_io: unknown cns_interface_type %d\n",
			itft);
	}

	for (l = iis, i = 0; l; l = l->next, i++) {
		ii = (cns_ii *)l->data;
		itfp = cns_block_find_itfp(block, ii->idx, ii->itft);
		*itfp = cns_pointer_add(buf, i, block->dtype);
		if (itft == CNS_INPUT || itft == CNS_WEIGHT)
			cns_block_add_dep(block, ii->idx, -1);
	}
}

void cns_block_link_c(cns_block *block, size_t ori, cns_list *iis)
{
	void *buf;
	cns_list *l;
	cns_ii *ii;
	int i;

	buf = cns_reg_buf_link(block->rbuf_c, ori, iis);
	for (l = iis, i = 0; l; l = l->next) {
		ii = (cns_ii *)l->data;
		itfp = cns_block_find_itfp(block, ii->idx, ii->itft);
		*itfp = cns_pointer_add(buf, i, block->dtype);
	}
}

static void expand_reg(cns_reg_buf *rbuf, cns_reg_buf *new_rbuf, int multiple)
{
	cns_regs *reg;
	cns_list *regs;
	cns_ii *ii;
	cns_list *iis;
	cns_list *new_iis;
	int mul;

	for (regs = rbuf->regs; regs; regs = regs->next) {
		reg = (cns_reg *)regs->data;
		for (mul = 0; mul < multiple; mul++) {
			for (iis = reg->iis; iis; iis = iis->next) {
				ii = (cns_ii *)iis->data;
				new_iis = cns_iis_append(new_iis,
							mul*rbuf->len+ii->idx,
							ii->itft);
			}
		}
		cns_reg_buf_link(new_rbuf, mul*rbuf->len+reg->ori, new_iis);
	}
}

cns_block *cns_block_expand(cns_block *block, int multiple, int extra)
{
	cns_ii *ii;
	cns_block *new_block;
	cns_list *iis;
	cns_list *deps;
	ssize_t dep;
	size_t new_len;
	size_t new_cell_idx;
	size_t new_wbc_idx;
	size_t idx;
	int mul;
	void *p;
	void **itfp;

	if (multiple <= 0)
		cns_err_quit("ERROR: cns_block_expand: multiple must be positive\n");
	if (extra < 0)
		cns_err_quit("ERROR: cns_block_expand: extra can't be negative\n");
	new_len = block->len * multiple + extra;
	if (new_len > CNS_MAX_CELLS)
		cns_err_quit("ERROR: cns_block_expand: exceed maximum CNS_MAX_CELLS\n");
	new_block = cns_block_create(new_len, block->dtype, block->width);

	for (mul = 0; mul < multiple; mul++) {
		for (idx = 0; idx < block->len; idx++) {
			/* expand cells */
			new_cell_idx = mul * block->len + idx;
			cns_block_set_op(new_block, new_cell_idx, block->cells[idx].op);
			cns_block_set_en(new_block, new_cell_idx, block->cells[idx].en);
			for (deps = block->cells[idx].deps; deps; deps = deps->next) {
				dep = (ssize_t)deps->data;
				if (dep == -1) {
					cns_block_add_dep(new_block, new_cell_idx, -1);
					continue;
				}
				cns_block_add_dep(new_block, new_cell_idx,
						mul * block->len + dep);
			}

			/* expand wire buf */
			new_wbc_idx = mul * block->len + idx;
			for (iis = block->wbuf_c->iis[idx]; iis; iis = iis->next) {
				ii = (cns_ii *)iis->data;
				new_cell_idx = mul * block->len + ii->idx;
				p = cns_wire_buf_link(new_block->wbuf_c,
						new_wbc_idx, new_cell_idx, ii->itft);
				itfp = cns_block_find_itfp(new_block, new_cell_idx, ii->itft);
				*itfp = p;
			}
		}
	}

	expand_reg(block->rbuf_c, new_block->rbuf_c, multiple);
	expand_reg(block->rbuf_i, new_block->rbuf_i, multiple);
	expand_reg(block->rbuf_o, new_block->rbuf_o, multiple);
	expand_reg(block->rbuf_w, new_block->rbuf_w, multiple);

	cns_block_free(block);
	return new_block;
}

size_t cns_block_size(cns_block *block)
{
	return block->len * cns_size_of(block->dtype);
}

void cns_block_fill(cns_block *block, int itft, void *src, size_t n)
{
	void *dst;

	if (cns_block_size(block) < n)
		cns_err_quit("ERROR: cns_block_fill: buffer overflow\n");

	switch (itft) {
	case CNS_INPUT:
		dst = block->rbuf_i->buf;
		break;
	case CNS_WEIGHT:
		dst = block->rbuf_w->buf;
		break;
	case CNS_OUTPUT:
		dst = block->rbuf_o->buf;
		break;
	default:
		cns_err_quit("ERROR: cns_block_fill: unknown cns_interface_type %d\n",
			itft);
	}

	memmove(dst, src, n);
}

void cns_block_dump(cns_block *block, int itft, void *dst, size_t n)
{
	void *src;

	if (cns_block_size(block) < n)
		cns_err_msg("WARNING: cns_block_dump: requested size is larger than buffer size\n");

	switch (itft) {
	case CNS_INPUT:
		src = block->rbuf_i->buf;
		break;
	case CNS_WEIGHT:
		src = block->rbuf_w->buf;
		break;
	case CNS_OUTPUT:
		src = block->rbuf_o->buf;
		break;
	default:
		cns_err_quit("ERROR: cns_block_dump: unknown cns_interface_type %d\n",
			itft);
	}

	memmove(dst, src, n);
}

cns_list *cns_block_en_expand(cns_block *block, cns_list *ens, int base,
			int multiple, cns_list *extras)
{
	cns_list *new_ens;
	size_t new_len;
	size_t len;
	size_t new_idx;
	size_t idx;
	cns_list *l;
	int m;

	if (base <= 0)
		cns_err_quit("ERROR: cns_block_en_expand: base must be positive\n");
	if (multiple <= 0)
		cns_err_quit("ERROR: cns_block_en_expand: multiple must be positive\n");
	len = cns_list_length(ens);
	if (len > base)
		cns_err_quit("ERROR: cns_block_en_expand: ens length larger than base\n");
	new_len = len * multiple + cns_list_length(extras);
	if (new_len > block->len)
		cns_err_quit("ERROR: cns_block_en_expand: exceed block length\n");

	new_ens = NULL;
	for (m = 0; m < multiple; m++) {
		for (l = ens; l; l = l->next) {
			idx = (size_t)l->data;
			new_idx = m * base + idx;
			new_ens = cns_list_append(new_ens, (void *)new_idx);
		}
	}
	for (l = extras; l; l = l->next)
		new_ens = cns_list_append(new_ens, l->data);

	return new_ens;
}
