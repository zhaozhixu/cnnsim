#include "../src/cns_cell.h"
#include "../src/cns_cell_op.h"
#include "test_cnnsim.h"

static cns_cell_array *array;
static int8_t *input;
static int8_t *output;
static int8_t *weight;
static size_t size;

static void setup(void)
{
	size_t i;

	size = 5;
	array = cns_cell_array_create(size);
	input = (int8_t *)cns_alloc(sizeof(int8_t) * size);
	output = (int8_t *)cns_alloc(sizeof(int8_t) * size);
	weight = (int8_t *)cns_alloc(sizeof(int8_t) * size);
	for (i = 0; i < size; i++) {
		input[i] = -i;
		output[i] = i;
		weight[i] = 1;
		cns_cell_array_set_data(array, i, &input[i], &output[i], &weight[i]);
		cns_cell_array_set_dtype(array, i, CNS_INT8, CNS_INT8, CNS_INT8);
		cns_cell_array_set_width(array, i, 8, 8, 8);
		cns_cell_array_set_op(array, i, &cns_cell_mul_int8);
	}
}

static void teardown(void)
{
	cns_free(input);
	cns_free(output);
	cns_free(weight);
	cns_cell_array_free(array);
}

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

START_TEST(test_cell_array_dep_graph)
{
	int i;
	cns_graph *dep_g;
	cns_list *res;
	cns_list *l, *sub_list;
	int res_num;

	cns_cell_array_add_dep(array, 0, -1);
	cns_cell_array_add_dep(array, 1, -1);
	cns_cell_array_add_dep(array, 2, 0);
	cns_cell_array_add_dep(array, 2, 1);
	cns_cell_array_add_dep(array, 3, 0);
	cns_cell_array_add_dep(array, 3, 2);
	cns_cell_array_add_dep(array, 4, 3);

	dep_g = cns_cell_array_dep_graph(array);
	res_num = cns_graph_topsort(dep_g, &res);
	ck_assert_int_eq(res_num, 5);
	for (l = res, i = 0; i < res_num; l = l->next, i++) {
		if (i == 0) {
			sub_list = l->data;
			ck_assert_ptr_eq(sub_list->data, (void *)-1);
			ck_assert_ptr_eq(sub_list->next, NULL);
		}
		if (i == 1) {
			sub_list = l->data;
			ck_assert_ptr_eq(sub_list->data, (void *)0);
			ck_assert_ptr_eq(sub_list->next->data, (void *)1);
			ck_assert_ptr_eq(sub_list->next->next, NULL);
		}
		if (i == 2) {
			sub_list = l->data;
			ck_assert_ptr_eq(sub_list->data, (void *)2);
			ck_assert_ptr_eq(sub_list->next, NULL);
		}
		if (i == 3) {
			sub_list = l->data;
			ck_assert_ptr_eq(sub_list->data, (void *)3);
			ck_assert_ptr_eq(sub_list->next, NULL);
		}
		if (i == 4) {
			sub_list = l->data;
			ck_assert_ptr_eq(sub_list->data, (void *)4);
			ck_assert_ptr_eq(sub_list->next, NULL);
		}
	}
	cns_graph_free_topsortlist(res);
	cns_graph_free(dep_g);
}
END_TEST

Suite *make_cell_suite(void)
{
	Suite *s;
	s = suite_create("cell");

	TCase *tc_cell;
	tc_cell = tcase_create("cell");
	tcase_add_checked_fixture(tc_cell, setup, teardown);
	tcase_add_test(tc_cell, test_cell);
	tcase_add_test(tc_cell, test_cell_array_dep_graph);
	suite_add_tcase(s, tc_cell);

	return s;
}
