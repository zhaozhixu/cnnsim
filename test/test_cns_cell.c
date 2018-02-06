#include "../src/cns_cell_op.h"
#include "../src/cns_block.h"
#include "test_cnnsim.h"

static cns_block *block;
static int8_t *input;
static int8_t *output;
static int8_t *weight;
static size_t size;

static void setup(void)
{
	size_t i;

	size = 5;
	block = cns_block_create(size);
	input = (int8_t *)cns_alloc(sizeof(int8_t) * size);
	output = (int8_t *)cns_alloc(sizeof(int8_t) * size);
	weight = (int8_t *)cns_alloc(sizeof(int8_t) * size);
	for (i = 0; i < size; i++) {
		input[i] = -i;
		output[i] = i;
		weight[i] = 1;
		cns_block_set_data(block, i, &input[i], &output[i], &weight[i]);
		cns_block_set_dtype(block, i, CNS_INT8);
		cns_block_set_width(block, i, 8);
		cns_block_set_op(block, i, &cns_cell_mul_int8);
	}
}

static void teardown(void)
{
	cns_free(input);
	cns_free(output);
	cns_free(weight);
	cns_block_free(block);
}

START_TEST(test_block_dep_graph)
{
	int i;
	cns_graph *dep_g;
	cns_list *res;
	cns_list *l, *sub_list;
	int res_num;

	cns_block_add_dep(block, 0, -1);
	cns_block_add_dep(block, 1, -1);
	cns_block_add_dep(block, 2, 0);
	cns_block_add_dep(block, 2, 1);
	cns_block_add_dep(block, 3, 0);
	cns_block_add_dep(block, 3, 2);
	cns_block_add_dep(block, 4, 3);

	dep_g = cns_block_dep_graph(block);
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

Suite *make_block_suite(void)
{
	Suite *s;
	s = suite_create("block");

	TCase *tc_block;
	tc_block = tcase_create("block");
	tcase_add_checked_fixture(tc_block, setup, teardown);
	tcase_add_test(tc_block, test_block_dep_graph);
	suite_add_tcase(s, tc_block);

	return s;
}
