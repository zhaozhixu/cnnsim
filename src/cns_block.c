#include <assert.h>
#include <string.h>
#include "cns_block.h"

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
	cells = (cns_cell *)cns_alloc(sizeof(cns_cell) * size);
	block->cells = cells;
	block->size = size;

	for (i = 0; i < size; i++) {
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

/* void *cns_block_alloc_buf(cns_block *block, size_t n, cns_dtype dtype) */
/* { */
/* 	size_t size; */

/* 	size = cns_size_of(dtype) * n; */
/* 	block->buf = cns_alloc(size); */
/* 	memset(block->buf, 0, size); */
/* 	block->buf_size = size; */
/* 	block->buf_dtype = dtype; */
/* 	return block->buf; */
/* } */

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

void cns_block_link(cns_block *block, size_t idx1, int itf1, size_t idx2, int itf2)
{
	if (block->cbuf_mark >= block->cbuf_size) {
		fprintf(stderr,
			"ERROR: cns_block_link: chore buffer overflow\n");
		exit(EXIT_FAILURE);
	}

	switch (itf1) {
	case CNS_INPUT:
		block->cells[idx1].data.input = &block->cbuf[block->cbuf_mark++];
	}
}

void cns_block_link_io(cns_block *block, size_t idx, int itf)
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

	switch (itf) {
	case CNS_INPUT:
		block->cells[idx].data.input = &block->ibuf[block->ibuf_mark++];
		break;
	case CNS_OUTPUT:
		block->cells[idx].data.output = &block->obuf[block->obuf_mark++];
		break;
	case CNS_WEIGHT:
		block->cells[idx].data.weight = &block->wbuf[block->wbuf_mark++];
		break;
	default:
		fprintf(stderr,
			"ERROR: cns_block_link_io: unknown cns_interface_type %d\n", itf);
		exit(EXIT_FAILURE);
	}
}
