#include <stdio.h>

#include "cns_cell.h"
#include "cns_cell_op.h"
#include "cns_tensor.h"
#include "cns_util.h"

int main(int argc, char *argv[])
{
	/* int8_t input[8]; */
	/* int8_t output[8]; */
	/* int8_t weight[8]; */

	/* int i; */
	/* cns_cell *cells = cns_cells_create(8); */
	/* for (i = 0; i < 8; i++) { */
	/* 	input[i] = -2; */
	/* 	output[i] = 0; */
	/* 	weight[i] = i; */
	/* 	cns_cell_set_data(&cells[i], &input[i], &output[i], &weight[i]); */
	/* 	cns_cell_set_op(&cells[i], &cns_cell_mul_int8); */
	/* } */

	/* print_cell_data(&cells[0]); */
	/* for (i = 0; i < 8; i++) { */
	/* 	printf("%d: %d\n", i, output[i]); */
	/* } */

	/* cns_cells_run(cells, 8); */
	/* print_cell_data(&cells[0]); */
	/* for (i = 0; i < 8; i++) { */
	/* 	printf("%d: %d\n", i, output[i]); */
	/* } */

	/* cns_cells_free(cells); */
	uint32_t dims[] = {3, 4, 2, 3};
	int8_t data[72];
	/* cns_bool_t data[72]; */
	int i;
	for (i = 0; i < 72; i++) {
		/* if (i % 2) { */
		/* 	data[i] = -i; */
		/* 	continue; */
		/* } */
		data[i] = 128;
	}
	cns_tensor *tensor = cns_tensor_create(4, dims, data, CNS_INT8);
	cns_tensor_print(tensor, NULL);
	cns_tensor_free(tensor, CNS_FLASE);
}
