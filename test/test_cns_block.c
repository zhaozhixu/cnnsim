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
	block = cns_block_create(size, CNS_INT8, 8);
	input = (int8_t *)cns_alloc(sizeof(int8_t) * size);
	output = (int8_t *)cns_alloc(sizeof(int8_t) * size);
	weight = (int8_t *)cns_alloc(sizeof(int8_t) * size);
	for (i = 0; i < size; i++) {
		input[i] = -i;
		output[i] = i;
		weight[i] = 1;
		cns_block_set_data(block, i, &input[i], &weight[i], &output[i]);
		cns_block_set_dtype(block, i, CNS_INT8);
		cns_block_set_width(block, i, 8);
		cns_block_set_op(block, i, &cns_cell_op_mul_int8);
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

/* START_TEST(test_block_link) */
/* { */
/* 	cns_block b; */
/* 	size_t b_size; */
/* 	size_t i; */

/* 	b_size = 9 + 9 + 9 + 1 + 1; */
/* 	b = cns_block_create(b_size, CNS_INT8, 8); */
/* 	for (i = 0; i < 9; i++) { /\* 9 multipliers *\/ */
/* 		cns_block_link_io(b, i, CNS_INPUT); */
/* 		cns_block_link_io(b, i, CNS_WEIGHT); */
/* 		cns_block_set_op(b, i, cns_cell_op_mul_int8); */
/* 	} */
/* 	for (i = 9; i < 18; i++) { /\* 9 adders *\/ */
/* 		cns_block_link(b, i, CNS_INPUT, i-9, CNS_OUTPUT); */
/* 		cns_block_link(b, i, CNS_WEIGHT, i, CNS_OUTPUT); */
/* 		cns_block_set_op(b, i, cns_cell_op_add_int8); */
/* 	} */
/* 	for (i = 18; i < 27; i++) { /\* 9 relus *\/ */
/* 		cns_block_link(b, i, CNS_INPUT, i-9, CNS_OUTPUT); */
/* 		cns_block_set_op(b, i, cns_cell_op_relu_int8); */
/* 	} */
/* } */
/* END_TEST */

/* /\* */
/*  * This is a test block for 3x3 and 1x1 convolution. */
/*  *\/ */
/* START_TEST(test_block_conv) */
/* { */
/* 	cns_block *block; */
/* 	void *buf; */
/* 	void *buf_mi; */
/* 	void *buf_mw; */
/* 	void *buf_mo; */
/* 	void *buf_ao; */
/* 	void *buf_ro; */
/* 	void *buf_eao; */
/* 	void *buf_eaw; */
/* 	void *buf_ero; */
/* 	size_t block_size; */
/* 	size_t nbuf; */
/* 	size_t i; */

/* 	/\* 9 multipliers, 9 adders, 9 relus, 1 extra adder, 1 extra relu *\/ */
/* 	block_size = 9 + 9 + 9 + 1 + 1; */
/* 	block = cns_block_create(block_size, CNS_INT8, 8); */

/* 	/\* 9 multiplier inputs, 9 multiplier weights, */
/* 	   9 multiplier outputs (adder inputs), */
/* 	   9 adder outputs (relu inputs), 9 relu outputs, */
/* 	   1 extra adder weight, */
/* 	   1 extra adder output (extra relu input), */
/* 	   1 extra relu output *\/ */
/* 	nbuf = 9 + 9 + 9 + 9 + 9 + 1 + 1 + 1; */
/* 	buf = cns_block_alloc_buf(block, nbuf, CNS_INT8); */
/* 	buf_mi = buf; */
/* 	buf_mw = buf_mi + 9; */
/* 	buf_mo = buf_mw + 9; */
/* 	buf_ao = buf_mo + 9; */
/* 	buf_ro = buf_ao + 9; */
/* 	buf_eaw = buf_ro + 9; */
/* 	buf_eao = buf_eaw + 1; */
/* 	buf_ero = buf_eao + 1; */

/* 	*(int8_t *)buf_eaw = 9;		/\* for cns_cell_op_add_many_int8 *\/ */
/* 	for (i = 0; i < block->size; i++) { */
/* 		cns_block_set_dtype(block, i, CNS_INT8); */
/* 		if (i >= 0 && i < 9) { */
/* 			cns_block_set_data(block, i, */
/* 					buf_mi+i, buf_mw+i, buf_mo+i); */
/* 			cns_block_set_op(block, i, cns_cell_op_mul_int8); */
/* 		} */
/* 		if (i >= 9 && i < 18) { */
/* 			cns_block_set_data(block, i, */
/* 					buf_mo+i-9, buf_ao+i-9, buf_ao+i-9); */
/* 			cns_block_set_op(block, i, cns_cell_op_add_int8); */
/* 		} */
/* 		if (i >= 18 && i < 27) { */
/* 			cns_block_set_data(block, i, */
/* 					buf_ao+i-18, NULL, buf_ro+i-18); */
/* 			cns_block_set_op(block, i, cns_cell_op_relu_int8); */
/* 		} */
/* 		if (i >= 27 && i < 28) { */
/* 			cns_block_set_data(block, i, buf_ao, buf_eaw, buf_eao); */
/* 			cns_block_set_op(block, i, cns_cell_op_add_many_int8); */
/* 		} */
/* 		if (i >= 28 && i < 29) { */
/* 			cns_block_set_data(block, i, buf_eao, NULL, buf_ero); */
/* 			cns_block_set_op(block, i, cns_cell_op_relu_int8); */
/* 		} */
/* 	} */

/* 	cns_block_free(block); */
/* } */
/* END_TEST */

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
