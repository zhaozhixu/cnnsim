#include "../src/cns_cell.h"
#include "../src/cns_cell_op.h"
#include "test_cnnsim.h"

START_TEST(test_cell)
{
	int8_t input[8];
	int8_t output[8];
	int8_t weight[8];

	int i;
	cns_cell_array *cell_array = cns_cell_array_create(8);
	for (i = 0; i < 8; i++) {
		input[i] = -2;
		output[i] = 0;
		weight[i] = i;
		cns_cell_array_set_data(cell_array, i, &input[i], &output[i], &weight[i]);
		cns_cell_array_set_op(cell_array, i, &cns_cell_mul_int8);
	}

	cns_cell_array_run(cell_array);
	/* cns_cell_print_data(&cell_array->cells[0]); */
	/* for (i = 0; i < 8; i++) { */
	/* 	printf("%d: %d\n", i, output[i]); */
	/* } */

	cns_cell_array_free(cell_array);
}
END_TEST

Suite *make_cell_suite(void)
{
	Suite *s;
	s = suite_create("cell");

	TCase *tc_cell;
	tc_cell = tcase_create("cell");
	tcase_add_test(tc_cell, test_cell);
	suite_add_tcase(s, tc_cell);

	return s;
}
