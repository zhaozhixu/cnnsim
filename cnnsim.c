#include <stdio.h>

#include "cns_cell.h"
#include "cns_cell_op.h"

int main(int argc, char *argv[])
{
	int8_t input[8];
	int8_t output[8];
	int8_t weight[8];

	int i;
	cns_cell *cells = cns_cells_create(8);
	for (i = 0; i < 8; i++) {
		input[i] = -2;
		output[i] = 0;
		weight[i] = i;
		cns_cell_set_data(&cells[i], &input[i], &output[i], &weight[i]);
		cns_cell_set_op(&cells[i], &cns_cell_mul_int8);
	}

	print_cell_data(&cells[0]);
	for (i = 0; i < 8; i++) {
		printf("%d: %d\n", i, output[i]);
	}

	cns_cells_run(cells, 8);
	print_cell_data(&cells[0]);
	for (i = 0; i < 8; i++) {
		printf("%d: %d\n", i, output[i]);
	}

	cns_cells_free(cells);
}
