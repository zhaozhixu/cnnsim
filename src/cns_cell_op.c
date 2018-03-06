#include "cns_cell_op.h"

void cns_cell_op_mul_int8(cns_cell_data *data)
{
	*(int8_t *)data->output = *(int8_t *)data->input
		* *(int8_t *)data->weight;
}

void cns_cell_op_add_int8(cns_cell_data *data)
{
	*(int8_t *)data->output = *(int8_t *)data->input
		+ *(int8_t *)data->weight;
}

void cns_cell_op_relu_int8(cns_cell_data *data)
{
	*(int8_t *)data->output = *(int8_t *)data->input > 0 ?
		*(int8_t *)data->input : 0;
}

void cns_cell_op_add_many_int8(cns_cell_data *data)
{
	int8_t i;

	for (i = 0; i < *(int8_t *)data->weight; i++)
		*(int8_t *)data->output += ((int8_t *)data->input)[i];
}

void cns_cell_op_add_9_int8(cns_cell_data *data)
{
	int8_t i;

	for (i = 0; i < 9; i++)
		*(int8_t *)data->output += ((int8_t *)data->input)[i];
}

void cns_cell_op_poolmax_9_int8(cns_cell_data *data)
{
	int8_t i, max;

	for (i = 1, max = *(int8_t *)data->input; i < 9; i++) {
		if (((int8_t *)data->input)[i] > max)
			max = ((int8_t *)data->input)[i];
	}
	*(int8_t *)data->output = max;
}

void cns_cell_op_assign_int8(cns_cell_data *data)
{
	*(int8_t *)data->output = *(int8_t *)data->input;
}
