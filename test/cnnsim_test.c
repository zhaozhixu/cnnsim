#include <stdlib.h>
#include <stdio.h>
#include <check.h>

#include "../src/cns_cell.h"
#include "../src/cns_cell_op.h"

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
	cns_cell_print_data(&cell_array->cells[0]);
	for (i = 0; i < 8; i++) {
		printf("%d: %d\n", i, output[i]);
	}

	cns_cell_array_free(cell_array);
}
END_TEST

Suite *cns_suite_create(void)
{
	Suite *s;
	TCase *tc_cell;

	s = suite_create("cnnsim");

	tc_cell = tcase_create("cell");
	tcase_add_test(tc_cell, test_cell);
	suite_add_tcase(s, tc_cell);

	return s;
}

int main(int argc, char *argv[])
{
	int number_failed;
	int status;
	Suite *s;
	SRunner *sr;

	s = cns_suite_create();
	sr = srunner_create(s);

	srunner_set_xml (sr, "result/test.xml");
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	status = system("sed -i 's,http://check.sourceforge.net/xml/check_unittest.xslt,check_unittest.xslt,g' result/test.xml");
	if (status < 0)
		fprintf(stderr, "system() error\n");

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
