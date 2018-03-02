#include "test_cnnsim.h"
#include "../src/cns_tensor.h"
#include "../src/cns_tensor_op.h"

START_TEST(test_tensor)
{
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
	/* cns_tensor_print(tensor, NULL); */
	cns_tensor_free(tensor, CNS_FALSE);
}
END_TEST

Suite *make_tensor_suite(void)
{
	Suite *s;
	s = suite_create("tensor");

	TCase *tc_tensor;
	tc_tensor = tcase_create("tensor");
	tcase_add_test(tc_tensor, test_tensor);
	suite_add_tcase(s, tc_tensor);

	return s;
}
