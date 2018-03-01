#include <assert.h>
#include <string.h>
#include "cns_block.h"

cns_block *cns_block_create(size_t length, cns_dtype dtype, uint8_t width)
{
	assert(length > 0 && length <= CNS_MAX_CELLS);
	cns_cell *cells;
	cns_block *block;
	size_t i;

	block = (cns_block *)cns_alloc(sizeof(cns_block));
	block->ibuf = cns_buf_create(length, dtype);
	block->obuf = cns_buf_create(length, dtype);
	block->wbuf = cns_buf_create(length, dtype);
	block->cbuf = cns_buf_create(length, dtype);
	cells = (cns_cell *)cns_alloc(sizeof(cns_cell) * length);
	block->cells = cells;
	block->length = length;
	block->dtype = dtype;
	block->width = width;

	for (i = 0; i < length; i++) {
		cells[i].data.width = width;
		cells[i].data.dtype = dtype;
		cells[i].data.input = NULL;
		cells[i].data.weight = NULL;
		cells[i].data.output = NULL;
		cells[i].index = i;
		cells[i].deps = NULL;
		cells[i].op = NULL;
		cells[i].en = CNS_TRUE;
	}

	return block;
}

void cns_block_free(cns_block *block)
{
	assert(block);
	cns_buf_free(block->ibuf);
	cns_buf_free(block->obuf);
	cns_buf_free(block->wbuf);
	cns_buf_free(block->cbuf);
	cns_free(block->cells);
	cns_free(block);
}

void cns_block_run(cns_block *block) /* TODO: dep infomation */
{
	assert(block && block->cells);
	size_t i;		/* TODO: need to be parallized */
	for (i = 0; i < block->length; i++)
		cns_cell_run(&block->cells[i]);
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

	g = cns_graph_create();
	cns_graph_add(g, (void *)-1);
	for (i = 0; i < block->length; i++)
		cns_graph_add(g, (void *)i);
	for (i = 0; i < block->length; i++) {
		for (l = block->cells[i].deps; l; l = l->next)
			cns_graph_link(g, (void *)l->data, (void *)i);
	}

	return g;
}

void **cns_block_find_itfp(cns_block *block, size_t idx, int itft)
{
	void **itfp;

	if (idx >= block->length) {
		fprintf(stderr,
			"ERROR: cns_block_find_itfp: cell array out of bound\n");
		exit(EXIT_FAILURE);
	}

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
		fprintf(stderr,
			"ERROR: cns_block_find_itfp: unknown cns_interface_type %d\n",
			itft);
		exit(EXIT_FAILURE);
	}

	return itfp;
}

void cns_block_link(cns_block *block, size_t idx1, int itft1, size_t idx2, int itft2)
{
	void **itfp1;
	void **itfp2;
	int buf_idx, buf_idx1, buf_idx2;

	itfp1 = cns_block_find_itfp(block, idx1, itft1);
	itfp2 = cns_block_find_itfp(block, idx2, itft2);

	if (itft1 == CNS_OUTPUT && (itft2 == CNS_INPUT || itft2 == CNS_WEIGHT)
		&& idx1 != idx2)
		cns_block_add_dep(block, idx2, idx1);
	if (itft2 == CNS_OUTPUT && (itft1 == CNS_INPUT || itft1 == CNS_WEIGHT)
		&& idx1 != idx2)
		cns_block_add_dep(block, idx1, idx2);

	if (*itfp1 && !*itfp2) {
		buf_idx = cns_buf_index(block->cbuf, *itfp1);
		*itfp2 = cns_buf_attach(block->cbuf, buf_idx, idx2, itft2);
		return;
	}
	if (!*itfp1 && *itfp2) {
		buf_idx = cns_buf_index(block->cbuf, *itfp2);
		*itfp1 = cns_buf_attach(block->cbuf, buf_idx, idx1, itft1);
		return;
	}
	/* Shouldn't enter this branch in practice,
	   causing an dangling interface easily. */
	if (*itfp1 && *itfp2) {
		fprintf(stderr,
			"WARNING: cns_block_link: linking two attached interfaces\n");
		if (*itfp1 == *itfp2)
			return;
		buf_idx1 = cns_buf_index(block->cbuf, *itfp1);
		buf_idx2 = cns_buf_index(block->cbuf, *itfp2);
		if (buf_idx1 < buf_idx2) {
			*itfp2 = cns_buf_attach(block->cbuf, buf_idx1, idx2, itft2);
			cns_buf_detach(block->cbuf, buf_idx2, idx2, itft2);
		} else {
			*itfp1 = cns_buf_attach(block->cbuf, buf_idx2, idx1, itft1);
			cns_buf_detach(block->cbuf, buf_idx1, idx1, itft1);
		}
		return;
	}
	*itfp1 = cns_buf_append(block->cbuf, idx1, itft1);
	*itfp2 = cns_buf_attach(block->cbuf, block->cbuf->head-1, idx2, itft2);
}

void cns_block_link_io(cns_block *block, size_t idx, int itft)
{
	switch (itft) {
	case CNS_INPUT:
		block->cells[idx].data.input = cns_buf_append(block->ibuf, idx, itft);
		cns_block_add_dep(block, idx, -1);
		break;
	case CNS_OUTPUT:
		block->cells[idx].data.output = cns_buf_append(block->obuf, idx, itft);
		break;
	case CNS_WEIGHT:
		block->cells[idx].data.weight = cns_buf_append(block->wbuf, idx, itft);
		cns_block_add_dep(block, idx, -1);
		break;
	default:
		fprintf(stderr,
			"ERROR: cns_block_link_io: unknown cns_interface_type %d\n",
			itft);
		exit(EXIT_FAILURE);
	}
}

cns_block *cns_block_expand(cns_block *block, uint32_t multiple)
{
	cns_buf_ii *ii;
	cns_block *new_block;
	cns_list *iis;
	cns_list *deps;
	ssize_t dep;
	size_t new_length;
	size_t new_cell_idx;
	size_t new_ibuf_idx, new_wbuf_idx, new_obuf_idx, new_cbuf_idx;
	size_t idx;
	uint32_t mul;
	void *p;
	void **itfp;

	if (multiple == 0) {
		fprintf(stderr, "ERROR: cns_block_expand: multiple cannot be zero\n");
		exit(EXIT_FAILURE);
	}
	new_length = block->length * multiple;
	if (new_length > CNS_MAX_CELLS) {
		fprintf(stderr, "ERROR: cns_block_expand: exceeded maximum CNS_MAX_CELLS\n");
		exit(EXIT_FAILURE);
	}
	new_block = cns_block_create(new_length, block->dtype, block->width);

	/* iterate over new cells and new block bufs */
	for (mul = 0; mul < multiple; mul++) {
		for (idx = 0; idx < block->length; idx++) {
			new_cell_idx = mul * block->length + idx;
			cns_block_set_op(new_block, new_cell_idx, block->cells[idx].op);
			cns_block_set_en(new_block, new_cell_idx, block->cells[idx].en);
			for (deps = block->cells[idx].deps; deps; deps = deps->next) {
				dep = (ssize_t)deps->data;
				if (dep == -1) {
					cns_block_add_dep(new_block, new_cell_idx, -1);
					continue;
				}
				cns_block_add_dep(new_block, new_cell_idx,
						mul * block->length + dep);
			}

			new_ibuf_idx = new_block->ibuf->head;
			if (block->ibuf->iis[idx])
				new_block->ibuf->head++;
			for (iis = block->ibuf->iis[idx]; iis; iis = iis->next) {
				ii = (cns_buf_ii *)iis->data;
				new_cell_idx = mul * block->length + ii->idx;
				p = cns_buf_attach(new_block->ibuf,
					new_ibuf_idx, new_cell_idx, ii->itft);
				itfp = cns_block_find_itfp(new_block, new_cell_idx, ii->itft);
				*itfp = p;
			}
			new_wbuf_idx = new_block->wbuf->head;
			if (block->wbuf->iis[idx])
				new_block->wbuf->head++;
			for (iis = block->wbuf->iis[idx]; iis; iis = iis->next) {
				ii = (cns_buf_ii *)iis->data;
				new_cell_idx = mul * block->length + ii->idx;
				p = cns_buf_attach(new_block->wbuf,
						new_wbuf_idx, new_cell_idx, ii->itft);
				itfp = cns_block_find_itfp(new_block, new_cell_idx, ii->itft);
				*itfp = p;
			}
			new_obuf_idx = new_block->obuf->head;
			if (block->obuf->iis[idx])
				new_block->obuf->head++;
			for (iis = block->obuf->iis[idx]; iis; iis = iis->next) {
				ii = (cns_buf_ii *)iis->data;
				new_cell_idx = mul * block->length + ii->idx;
				p = cns_buf_attach(new_block->obuf,
						new_obuf_idx, new_cell_idx, ii->itft);
				itfp = cns_block_find_itfp(new_block, new_cell_idx, ii->itft);
				*itfp = p;
			}
			new_cbuf_idx = new_block->cbuf->head;
			if (block->cbuf->iis[idx])
				new_block->cbuf->head++;
			for (iis = block->cbuf->iis[idx]; iis; iis = iis->next) {
				ii = (cns_buf_ii *)iis->data;
				new_cell_idx = mul * block->length + ii->idx;
				p = cns_buf_attach(new_block->cbuf,
						new_cbuf_idx, new_cell_idx, ii->itft);
				itfp = cns_block_find_itfp(new_block, new_cell_idx, ii->itft);
				*itfp = p;
			}
		}
	}

	cns_block_free(block);
	return new_block;
}

size_t cns_block_size(cns_block *block)
{
	return block->length * cns_size_of(block->dtype);
}

void cns_block_fill(cns_block *block, int itft, void *src, size_t n)
{
	void *dst;

	if (cns_block_size(block) < n) {
		fprintf(stderr, "ERROR: cns_block_fill: buffer overflow\n");
		exit(EXIT_FAILURE);
	}

	switch (itft) {
	case CNS_INPUT:
		dst = block->ibuf->buf;
		break;
	case CNS_WEIGHT:
		dst = block->wbuf->buf;
		break;
	case CNS_OUTPUT:
		dst = block->obuf->buf;
		break;
	default:
		fprintf(stderr,
			"ERROR: cns_block_fill: unknown cns_interface_type %d\n",
			itft);
		exit(EXIT_FAILURE);
	}

	memmove(dst, src, n);
}

void cns_block_dump(cns_block *block, int itft, void *dst, size_t n)
{
	void *src;

	if (cns_block_size(block) < n) {
		fprintf(stderr,
			"WARNING: cns_block_dump: requested size is larger than buffer size\n");
		n = cns_block_size(block);
	}

	switch (itft) {
	case CNS_INPUT:
		src = block->ibuf->buf;
		break;
	case CNS_WEIGHT:
		src = block->wbuf->buf;
		break;
	case CNS_OUTPUT:
		src = block->obuf->buf;
		break;
	default:
		fprintf(stderr,
			"ERROR: cns_block_dump: unknown cns_interface_type %d\n",
			itft);
		exit(EXIT_FAILURE);
	}

	memmove(dst, src, n);
}
