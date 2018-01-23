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
	cell->data.input_width = cell_data->input_width;
	cell->data.output_width = cell_data->output_width;
	cell->data.weight_width = cell_data->weight_width;
	cell->data.input_dtype = cell_data->input_dtype;
	cell->data.output_dtype = cell_data->output_dtype;
	cell->data.weight_dtype = cell_data->weight_dtype;
}

void cns_cell_set_data(cns_cell *cell, void *input, void *output, void *weight)
{
	assert(cell && input && output && weight);
	cell->data.input = input;
	cell->data.output = output;
	cell->data.weight = weight;
}

void cns_cell_set_dtype(cns_cell *cell, int input_dtype,
			int output_dtype, int weight_dtype)
{
	assert(cell);
	cell->data.input_dtype = input_dtype;
	cell->data.output_dtype = output_dtype;
	cell->data.weight_dtype = weight_dtype;
}

void cns_cell_set_width(cns_cell *cell, uint8_t input_width,
			uint8_t output_width, uint8_t weight_width)
{
	assert(cell);
	cell->data.input_width = input_width;
	cell->data.output_width = output_width;
	cell->data.weight_width = weight_width;
}

void cns_cell_set_op(cns_cell *cell, cns_cell_op op)
{
	assert(cell);
	cell->op = op;
}

void cns_cell_set_dep(cns_cell *cell, ssize_t *dep)
{
	assert(cell);
	cell->dep = dep;
}

void cns_cell_fprint_data(FILE *fp, cns_cell *cell)
{
	assert(cell);
	fprintf(fp,
		"input = %p\n"
		"output = %p\n"
		"weight = %p\n"
		"input_width = %u\n"
		"output_width = %u\n"
		"weight_width = %u\n"
		"input_dtype = %d\n"
		"output_dtype = %d\n"
		"weight_dtype = %d\n",
		cell->data.input,
		cell->data.output,
		cell->data.weight,
		cell->data.input_width,
		cell->data.output_width,
		cell->data.weight_width,
		cell->data.input_dtype,
		cell->data.output_dtype,
		cell->data.weight_dtype);
}

void cns_cell_print_data(cns_cell *cell)
{
	cns_cell_fprint_data(stdout, cell);
}

cns_cell_array *cns_cell_array_create(size_t size)
{
	assert(size > 0 && size <= CNS_MAX_CELLS);
	cns_cell *cells;
	cns_cell_array *cell_array;
	size_t i;

	cells = (cns_cell *)cns_alloc(sizeof(cns_cell) * size);
	cell_array = (cns_cell_array *)cns_alloc(sizeof(cns_cell_array));
	cell_array->cells = cells;
	cell_array->size = size;

	for (i = 0; i < size; i++)
		cells[i].index = i;

	return cell_array;
}

void cns_cell_array_free(cns_cell_array *cell_array)
{
	assert(cell_array);
	cns_free(cell_array->cells);
	cns_free(cell_array);
}

void cns_cell_array_run(cns_cell_array *cell_array)
{
	assert(cell_array && cell_array->cells);
	size_t i;		/* TODO: need to be parallized */
	for (i = 0; i < cell_array->size; i++)
		cns_cell_run(&cell_array->cells[i]);
}
