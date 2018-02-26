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

void cns_block_link(cns_block *block, size_t idx1, int itft1, size_t idx2, int itft2)
{
	void **itfp1;
	void **itfp2;
	int buf_idx, buf_idx1, buf_idx2;

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
	if (*itfp1 && *itfp2) {
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
		break;
	case CNS_OUTPUT:
		block->cells[idx].data.output = cns_buf_append(block->obuf, idx, itft);
		break;
	case CNS_WEIGHT:
		block->cells[idx].data.weight = cns_buf_append(block->wbuf, idx, itft);
		break;
	default:
		fprintf(stderr,
			"ERROR: cns_block_link_io: unknown cns_interface_type %d\n", itft);
		exit(EXIT_FAILURE);
	}
}
