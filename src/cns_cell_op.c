#include "cns_cell_op.h"

void cns_cell_mul_int8(cns_cell_data *data)
{
	*(int8_t *)data->output = *(int8_t *)data->input * *(int8_t *)data->weight;
}

void cns_cell_add_int8(cns_cell_data *data)
{
	*(int8_t *)data->output = *(int8_t *)data->input + *(int8_t *)data->weight;
}
