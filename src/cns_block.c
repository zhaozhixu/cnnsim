#include <assert.h>
#include "cns_block.h"

cns_block *cns_block_create(size_t size)
{
	assert(size > 0 && size <= CNS_MAX_CELLS);
	cns_cell *cells;
	cns_block *block;
	size_t i;

	cells = (cns_cell *)cns_alloc(sizeof(cns_cell) * size);
	block = (cns_block *)cns_alloc(sizeof(cns_block));
	block->cells = cells;
	block->size = size;

	for (i = 0; i < size; i++) {
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
			void *input, void *output, void *weight)
{
	cns_cell_set_data(&block->cells[index], input, output, weight);
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
