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
		/* cns_block_set_data(block, i, &input[i], &weight[i], &output[i]); */
		/* cns_block_set_dtype(block, i, CNS_INT8); */
		/* cns_block_set_width(block, i, 8); */
		/* cns_block_set_op(block, i, &cns_cell_op_mul_int8); */
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

START_TEST(test_block_link_io)
{
	cns_block_link_io(block, 0, CNS_INPUT);
	cns_block_link_io(block, 0, CNS_WEIGHT);
	cns_block_link_io(block, 1, CNS_INPUT);
	cns_block_link_io(block, 1, CNS_WEIGHT);
	cns_block_link_io(block, 2, CNS_OUTPUT);
	ck_assert_ptr_eq(block->cells[0].data.input, block->ibuf->buf);
	ck_assert_ptr_eq(block->cells[0].data.weight, block->wbuf->buf);
	ck_assert_ptr_eq(block->cells[1].data.input, (int8_t *)block->ibuf->buf+1);
	ck_assert_ptr_eq(block->cells[1].data.weight, (int8_t *)block->wbuf->buf+1);
	ck_assert_ptr_eq(block->cells[2].data.output, (int8_t *)block->obuf->buf);
	ck_assert_int_eq((ssize_t)block->cells[0].deps->data, -1);
	ck_assert_ptr_eq(block->cells[0].deps->next, NULL);
	ck_assert_int_eq((ssize_t)block->cells[1].deps->data, -1);
	ck_assert_ptr_eq(block->cells[1].deps->next, NULL);
	ck_assert_ptr_eq(block->cells[2].deps, NULL);
}
END_TEST

START_TEST(test_block_link)
{
	cns_block_link(block, 0, CNS_OUTPUT, 1, CNS_INPUT);
	ck_assert_ptr_eq(block->cells[0].data.output, block->cbuf->buf);
	ck_assert_ptr_eq(block->cells[1].data.input, block->cbuf->buf);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[0]->data)->idx, 0);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[0]->data)->itft, CNS_OUTPUT);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[0]->next->data)->idx, 1);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[0]->next->data)->itft, CNS_INPUT);
	ck_assert_ptr_eq(block->cbuf->iis[0]->next->next, NULL);
	ck_assert_ptr_eq(block->cells[0].deps, NULL);
	ck_assert_int_eq((ssize_t)block->cells[1].deps->data, 0);
	ck_assert_ptr_eq(block->cells[1].deps->next, NULL);

	cns_block_link(block, 0, CNS_OUTPUT, 1, CNS_WEIGHT);
	ck_assert_ptr_eq(block->cells[0].data.output, block->cbuf->buf);
	ck_assert_ptr_eq(block->cells[1].data.weight, block->cbuf->buf);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[0]->next->next->data)->idx, 1);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[0]->next->next->data)->itft, CNS_WEIGHT);
	ck_assert_ptr_eq(block->cbuf->iis[0]->next->next->next, NULL);
	ck_assert_ptr_eq(block->cells[0].deps, NULL);
	ck_assert_int_eq((ssize_t)block->cells[1].deps->data, 0);
	ck_assert_ptr_eq(block->cells[1].deps->next, NULL);

	cns_block_link(block, 1, CNS_OUTPUT, 2, CNS_WEIGHT);
	cns_block_link(block, 3, CNS_INPUT, 1, CNS_OUTPUT);
	ck_assert_ptr_eq(block->cells[1].data.output, (int8_t *)block->cbuf->buf+1);
	ck_assert_ptr_eq(block->cells[2].data.weight, (int8_t *)block->cbuf->buf+1);
	ck_assert_ptr_eq(block->cells[3].data.input, (int8_t *)block->cbuf->buf+1);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[1]->data)->idx, 1);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[1]->data)->itft, CNS_OUTPUT);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[1]->next->data)->idx, 2);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[1]->next->data)->itft, CNS_WEIGHT);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[1]->next->next->data)->idx, 3);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[1]->next->next->data)->itft, CNS_INPUT);
	ck_assert_ptr_eq(block->cbuf->iis[1]->next->next->next, NULL);
	ck_assert_ptr_eq(block->cells[0].deps, NULL);
	ck_assert_int_eq((ssize_t)block->cells[1].deps->data, 0);
	ck_assert_ptr_eq(block->cells[1].deps->next, NULL);
	ck_assert_int_eq((ssize_t)block->cells[2].deps->data, 1);
	ck_assert_ptr_eq(block->cells[2].deps->next, NULL);
	ck_assert_int_eq((ssize_t)block->cells[3].deps->data, 1);
	ck_assert_ptr_eq(block->cells[3].deps->next, NULL);

	cns_block_link(block, 3, CNS_OUTPUT, 3, CNS_WEIGHT);
	ck_assert_ptr_eq(block->cells[3].data.output, (int8_t *)block->cbuf->buf+2);
	ck_assert_ptr_eq(block->cells[3].data.weight, (int8_t *)block->cbuf->buf+2);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[2]->data)->idx, 3);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[2]->data)->itft, CNS_OUTPUT);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[2]->next->data)->idx, 3);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[2]->next->data)->itft, CNS_WEIGHT);
	ck_assert_ptr_eq(block->cbuf->iis[2]->next->next, NULL);
	ck_assert_ptr_eq(block->cells[0].deps, NULL);
	ck_assert_int_eq((ssize_t)block->cells[1].deps->data, 0);
	ck_assert_ptr_eq(block->cells[1].deps->next, NULL);
	ck_assert_int_eq((ssize_t)block->cells[2].deps->data, 1);
	ck_assert_ptr_eq(block->cells[2].deps->next, NULL);
	ck_assert_int_eq((ssize_t)block->cells[3].deps->data, 1);
	ck_assert_ptr_eq(block->cells[3].deps->next, NULL);

	/* Shouldn't enter this branch in practice,
	   causing CNS_OUTPUT of cell 3 dangling. */
	cns_block_link(block, 3, CNS_WEIGHT, 1, CNS_OUTPUT);
	ck_assert_ptr_eq(block->cells[3].data.output, (int8_t *)block->cbuf->buf+2);
	ck_assert_ptr_eq(block->cells[3].data.weight, (int8_t *)block->cbuf->buf+1);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[2]->data)->idx, 3);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[2]->data)->itft, CNS_OUTPUT);
	ck_assert_ptr_eq(block->cbuf->iis[2]->next, NULL);
	ck_assert_ptr_eq(block->cells[0].deps, NULL);
	ck_assert_int_eq((ssize_t)block->cells[1].deps->data, 0);
	ck_assert_ptr_eq(block->cells[1].deps->next, NULL);
	ck_assert_int_eq((ssize_t)block->cells[2].deps->data, 1);
	ck_assert_ptr_eq(block->cells[2].deps->next, NULL);
	ck_assert_int_eq((ssize_t)block->cells[3].deps->data, 1);
	ck_assert_ptr_eq(block->cells[3].deps->next, NULL);
}
END_TEST

/* START_TEST(test_block_conv) */
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

Suite *make_block_suite(void)
{
	Suite *s;
	s = suite_create("block");

	TCase *tc_block;
	tc_block = tcase_create("block");
	tcase_add_checked_fixture(tc_block, setup, teardown);
	tcase_add_test(tc_block, test_block_dep_graph);
	tcase_add_test(tc_block, test_block_link_io);
	tcase_add_test(tc_block, test_block_link);
	suite_add_tcase(s, tc_block);

	return s;
}
