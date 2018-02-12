#include <assert.h>
#include <string.h>
#include "cns_block.h"

struct idx_itft {
	size_t idx;		/* index */
	int    itft;		/* interface type */
};

static int idx_itft_cmp(void *data1, void *data2)
{
	struct idx_itft *ii1, *ii2;

	ii1 = (struct idx_itft *)data1;
	ii2 = (struct idx_itft *)data2;
	if (ii1->idx == ii2->idx && ii1->itft = ii2->itft)
		return 0;
	return -1;
}

cns_block *cns_block_create(size_t size, cns_dtype dtype, uint8_t width)
{
	assert(size > 0 && size <= CNS_MAX_CELLS);
	cns_cell *cells;
	cns_block *block;
	size_t i, dsize;

	block = (cns_block *)cns_alloc(sizeof(cns_block));
	dsize = cns_size_of(dtype) * size;
	block->buf_dtype = dtype;
	block->ibuf = cns_alloc(dsize);
	block->obuf = cns_alloc(dsize);
	block->wbuf = cns_alloc(dsize);
	block->cbuf = cns_alloc(dsize);
	memset(block->ibuf, 0, dsize);
	memset(block->obuf, 0, dsize);
	memset(block->wbuf, 0, dsize);
	memset(block->cbuf, 0, dsize);
	block->ibuf_size = dsize;
	block->obuf_size = dsize;
	block->wbuf_size = dsize;
	block->cbuf_size = dsize;
	block->ibuf_mark = 0;
	block->obuf_mark = 0;
	block->wbuf_mark = 0;
	block->cbuf_mark = 0;
	block->cbuf_itfs = (cns_list **)cns_alloc(sizeof(cns_list *) * size);
	cells = (cns_cell *)cns_alloc(sizeof(cns_cell) * size);
	block->cells = cells;
	block->size = size;

	for (i = 0; i < size; i++) {
		block->cbuf_itfs[i] = NULL;
		cells[i].data.width = width;
		cells[i].data.dtype = dtype;
		cells[i].data.input = NULL;
		cells[i].data.weight = NULL;
		cells[i].data.output = NULL;
		cells[i].index = i;
		cells[i].deps = NULL;
		cells[i].op = NULL;
	}

	return block;
}

void cns_block_free(cns_block *block)
{
	assert(block);
	size_t i;

	for (i = 0; i < block->size; i++)
		cns_list_free_deep(block->cbuf_itfs[i]);
	cns_free(block->cells);
	cns_free(block->ibuf);
	cns_free(block->obuf);
	cns_free(block->wbuf);
	cns_free(block->cbuf);
	cns_free(block);
}

void cns_block_run(cns_block *block)
{
	assert(block && block->cells);
	size_t i;		/* TODO: need to be parallized */
	for (i = 0; i < block->size; i++)
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
	for (i = 0; i < block->size; i++)
		cns_graph_add(g, (void *)i);
	for (i = 0; i < block->size; i++) {
		for (l = block->cells[i].deps; l; l = l->next)
			cns_graph_link(g, (void *)l->data, (void *)i);
	}

	return g;
}

void cns_block_link(cns_block *block, size_t idx1, int itft1,
		size_t idx2, int itft2)
{
	void **itfp1;
	void **itfp2;

	switch (itft1) {
	case CNS_INPUT:
		itfp1 = &block->cells[idx1].data.input;
		break;
	case CNS_OUTPUT:
		itfp1 = &block->cells[idx1].data.output;
		break;
	case CNS_WEIGHT:
		itfp1 = &block->cells[idx1].data.weight;
		break;
	default:
		fprintf(stderr,
			"ERROR: cns_block_link: unknown cns_interface_type %d\n", itft1);
		exit(EXIT_FAILURE);
	}
	switch (itft2) {
	case CNS_INPUT:
		itfp2 = &block->cells[idx2].data.input;
		break;
	case CNS_OUTPUT:
		itfp2 = &block->cells[idx2].data.output;
		break;
	case CNS_WEIGHT:
		itfp2 = &block->cells[idx2].data.weight;
		break;
	default:
		fprintf(stderr,
			"ERROR: cns_block_link: unknown cns_interface_type %d\n", itft2);
		exit(EXIT_FAILURE);
	}

	if (*itfp1 && !*itfp2) {
		int buf_idx;
		struct idx_itft *ii2;
		*itfp2 = *itfp1;
		ii2 = (struct idx_itft *)cns_alloc(sizeof(struct idx_itft));
		ii2->idx = idx2;
		ii2->itft = itft2;
		buf_idx = cns_pointer_sub(*itfp1, block->cbuf, block->buf_dtype);
		block->cbuf_itfs[buf_idx] =
			cns_list_append(block->cbuf_itfs[buf_idx], ii2);
		return;
	}
	if (!*itfp1 && *itfp2) {
		int buf_idx;
		struct idx_itft *ii1;
		*itfp1 = *itfp2;
		ii1 = (struct idx_itft *)cns_alloc(sizeof(struct idx_itft));
		ii1->idx = idx1;
		ii1->itft = itft1;
		buf_idx = cns_pointer_sub(*itfp2, block->cbuf, block->buf_dtype);
		block->cbuf_itfs[buf_idx] =
			cns_list_append(block->cbuf_itfs[buf_idx], ii1);
		return;
	}
	if (*itfp1 && *itfp2) {
		if (*itfp1 == *itfp2)
			return;
		int buf_idx1, buf_idx2, find_idx;
		struct idx_itft *ii;
		buf_idx1 = cns_pointer_sub(*itfp1, block->cbuf, block->buf_dtype);
		buf_idx2 = cns_pointer_sub(*itfp2, block->cbuf, block->buf_dtype);
		ii = (struct idx_itft *)cns_alloc(sizeof(struct idx_itft));
		if (buf_idx1 < buf_idx2) {
			*itfp2 = *itfp1;
			ii->idx = idx2;
			ii->itft = itft2;
			find_idx = cns_list_index_custom(block->cbuf_itfs[idx2],
							ii, idx_itft_cmp);
			block->cbuf_itfs[buf_idx1] =
				cns_list_append(block->cbuf_itfs[buf_idx1], ii);
			block->cbuf_itfs[buf_idx2] =
				cns_list_remove_nth(block->cbuf_itfs[buf_idx2],
						find_idx);
		}
		else {
			*itfp1 = *itfp2;
			ii->idx = idx1;
			ii->itft = itft1;
			find_idx = cns_list_index_custom(block->cbuf_itfs[idx1],
							ii, idx_itft_cmp);
			block->cbuf_itfs[buf_idx2] =
				cns_list_append(block->cbuf_itfs[buf_idx2], ii);
			block->cbuf_itfs[buf_idx1] =
				cns_list_remove_nth(block->cbuf_itfs[buf_idx1],
						find_idx);
		}
		return;
	}

	if (block->cbuf_mark >= block->cbuf_size) {
		fprintf(stderr,
			"ERROR: cns_block_link: chore buffer overflow\n");
		exit(EXIT_FAILURE);
	}

	struct idx_itft *ii1;
	struct idx_itft *ii2;
	*itfp1 = *itfp2 = cns_pointer_add(block->cbuf, block->cbuf_mark,
					block->buf_dtype);
	ii1 = (struct idx_itft *)cns_alloc(sizeof(struct idx_itft));
	ii2 = (struct idx_itft *)cns_alloc(sizeof(struct idx_itft));
	ii1->idx = idx1;
	ii1->itft = itft1;
	ii2->idx = idx2;
	ii2->itft = itft2;
	block->cbuf_itfs[block->cbuf_mark] =
		cns_list_append(block->cbuf_itfs[block->cbuf_mark], ii1);
	block->cbuf_itfs[block->cbuf_mark] =
		cns_list_append(block->cbuf_itfs[block->cbuf_mark], ii2);
	block->cbuf_mark++;
}

void cns_block_link_io(cns_block *block, size_t idx, int itft)
{
	if (block->ibuf_mark >= block->ibuf_size) {
		fprintf(stderr,
			"ERROR: cns_block_link_io: input buffer overflow\n");
		exit(EXIT_FAILURE);
	}
	if (block->obuf_mark >= block->obuf_size) {
		fprintf(stderr,
			"ERROR: cns_block_link_io: output buffer overflow\n");
		exit(EXIT_FAILURE);
	}
	if (block->wbuf_mark >= block->wbuf_size) {
		fprintf(stderr,
			"ERROR: cns_block_link_io: weight buffer overflow\n");
		exit(EXIT_FAILURE);
	}

	switch (itft) {
	case CNS_INPUT:
		block->cells[idx].data.input = cns_pointer_add(block->ibuf,
							block->ibuf_mark++,
							block->buf_dtype);
		break;
	case CNS_OUTPUT:
		block->cells[idx].data.output = cns_pointer_add(block->obuf,
							block->obuf_mark++,
							block->buf_dtype);
		break;
	case CNS_WEIGHT:
		block->cells[idx].data.weight = cns_pointer_add(block->wbuf,
							block->wbuf_mark++,
							block->buf_dtype);
		break;
	default:
		fprintf(stderr,
			"ERROR: cns_block_link_io: unknown cns_interface_type %d\n", itft);
		exit(EXIT_FAILURE);
	}
}
