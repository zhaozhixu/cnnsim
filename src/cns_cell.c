#include <assert.h>

#include "cns_util.h"
#include "cns_cell.h"

void cns_cell_run(cns_cell *cell)
{
	assert(cell);
	cell->op(&cell->data);
}

void cns_cell_set_cell_data(cns_cell *cell, cns_cell_data *cell_data)
{
	assert(cell && cell_data);
	cell->data.input = cell_data->input;
	cell->data.output = cell_data->output;
	cell->data.weight = cell_data->weight;
	cell->data.width = cell_data->width;
	cell->data.dtype = cell_data->dtype;
}

void cns_cell_set_data(cns_cell *cell, void *input, void *weight, void *output)
{
	assert(cell && input && output && weight);
	cell->data.input = input;
	cell->data.output = output;
	cell->data.weight = weight;
}

void cns_cell_set_dtype(cns_cell *cell, cns_dtype dtype)
{
	assert(cell);
	cell->data.dtype = dtype;
}

void cns_cell_set_width(cns_cell *cell, uint8_t width)
{
	assert(cell);
	cell->data.width = width;
}

void cns_cell_set_op(cns_cell *cell, cns_cell_op op)
{
	assert(cell);
	cell->op = op;
}

void cns_cell_add_dep(cns_cell *cell, ssize_t dep)
{
	assert(cell);
	cell->deps = cns_list_append(cell->deps, (void *)dep);
}

void cns_cell_fprint_data(FILE *fp, cns_cell *cell)
{
	assert(cell);
	fprintf(fp,
		"input = %p\n"
		"output = %p\n"
		"weight = %p\n"
		"width = %u\n"
		"dtype = %d\n",
		cell->data.input,
		cell->data.output,
		cell->data.weight,
		cell->data.width,
		cell->data.dtype);
}

void cns_cell_print_data(cns_cell *cell)
{
	cns_cell_fprint_data(stdout, cell);
}
