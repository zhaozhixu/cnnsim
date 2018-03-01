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

START_TEST(test_block_set_op)
{
	cns_block_set_op(block, 0, cns_cell_op_mul_int8);
	cns_block_set_op(block, 1, cns_cell_op_add_int8);
	ck_assert_ptr_eq(block->cells[0].op, cns_cell_op_mul_int8);
	ck_assert_ptr_eq(block->cells[1].op, cns_cell_op_add_int8);
}
END_TEST

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

START_TEST(test_block_find_itfp)
{
	ck_assert_ptr_eq(cns_block_find_itfp(block, 0, CNS_INPUT), &block->cells[0].data.input);
	ck_assert_ptr_eq(cns_block_find_itfp(block, 1, CNS_WEIGHT), &block->cells[1].data.weight);
}
END_TEST

START_TEST(test_block_expand)
{
	cns_block_link_io(block, 0, CNS_INPUT);
	cns_block_link_io(block, 0, CNS_WEIGHT);
	cns_block_set_op(block, 0, cns_cell_op_mul_int8);
	cns_block_link_io(block, 1, CNS_INPUT);
	cns_block_link_io(block, 1, CNS_WEIGHT);
	cns_block_set_op(block, 1, cns_cell_op_mul_int8);
	cns_block_link(block, 2, CNS_INPUT, 0, CNS_OUTPUT);
	cns_block_link(block, 2, CNS_WEIGHT, 1, CNS_OUTPUT);
	cns_block_set_op(block, 2, cns_cell_op_add_int8);
	cns_block_link(block, 3, CNS_INPUT, 2, CNS_OUTPUT);
	cns_block_link(block, 3, CNS_WEIGHT, 3, CNS_OUTPUT);
	cns_block_set_op(block, 3, cns_cell_op_relu_int8);
	cns_block_link(block, 4, CNS_INPUT, 3, CNS_OUTPUT);
	cns_block_link_io(block, 4, CNS_OUTPUT);
	cns_block_set_op(block, 4, cns_cell_op_assign_int8);

	block = cns_block_expand(block, 1);
	ck_assert_int_eq(block->width, 8);
	ck_assert_int_eq(block->dtype, CNS_INT8);
	ck_assert_int_eq(block->length, 5);
	ck_assert_int_eq(block->ibuf->dtype, CNS_INT8);
	ck_assert_int_eq(block->ibuf->length, 5);
	ck_assert_int_eq(block->ibuf->head, 2);
	ck_assert_int_eq(block->wbuf->dtype, CNS_INT8);
	ck_assert_int_eq(block->wbuf->length, 5);
	ck_assert_int_eq(block->wbuf->head, 2);
	ck_assert_int_eq(block->obuf->dtype, CNS_INT8);
	ck_assert_int_eq(block->obuf->length, 5);
	ck_assert_int_eq(block->obuf->head, 1);
	ck_assert_int_eq(block->cbuf->dtype, CNS_INT8);
	ck_assert_int_eq(block->cbuf->length, 5);
	ck_assert_int_eq(block->cbuf->head, 4);
	ck_assert_int_eq(((cns_buf_ii *)block->ibuf->iis[0]->data)->idx, 0);
	ck_assert_int_eq(((cns_buf_ii *)block->ibuf->iis[0]->data)->itft, CNS_INPUT);
	ck_assert_ptr_eq(block->ibuf->iis[0]->next, NULL);
	ck_assert_int_eq(((cns_buf_ii *)block->ibuf->iis[1]->data)->idx, 1);
	ck_assert_int_eq(((cns_buf_ii *)block->ibuf->iis[1]->data)->itft, CNS_INPUT);
	ck_assert_ptr_eq(block->ibuf->iis[1]->next, NULL);
	ck_assert_int_eq(((cns_buf_ii *)block->wbuf->iis[0]->data)->idx, 0);
	ck_assert_int_eq(((cns_buf_ii *)block->wbuf->iis[0]->data)->itft, CNS_WEIGHT);
	ck_assert_ptr_eq(block->wbuf->iis[0]->next, NULL);
	ck_assert_int_eq(((cns_buf_ii *)block->wbuf->iis[1]->data)->idx, 1);
	ck_assert_int_eq(((cns_buf_ii *)block->wbuf->iis[1]->data)->itft, CNS_WEIGHT);
	ck_assert_ptr_eq(block->wbuf->iis[1]->next, NULL);
	ck_assert_int_eq(((cns_buf_ii *)block->obuf->iis[0]->data)->idx, 4);
	ck_assert_int_eq(((cns_buf_ii *)block->obuf->iis[0]->data)->itft, CNS_OUTPUT);
	ck_assert_ptr_eq(block->obuf->iis[0]->next, NULL);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[0]->data)->idx, 2);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[0]->data)->itft, CNS_INPUT);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[0]->next->data)->idx, 0);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[0]->next->data)->itft, CNS_OUTPUT);
	ck_assert_ptr_eq(block->cbuf->iis[0]->next->next, NULL);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[1]->data)->idx, 2);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[1]->data)->itft, CNS_WEIGHT);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[1]->next->data)->idx, 1);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[1]->next->data)->itft, CNS_OUTPUT);
	ck_assert_ptr_eq(block->cbuf->iis[1]->next->next, NULL);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[2]->data)->idx, 3);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[2]->data)->itft, CNS_INPUT);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[2]->next->data)->idx, 2);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[2]->next->data)->itft, CNS_OUTPUT);
	ck_assert_ptr_eq(block->cbuf->iis[2]->next->next, NULL);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[3]->data)->idx, 3);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[3]->data)->itft, CNS_WEIGHT);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[3]->next->data)->idx, 3);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[3]->next->data)->itft, CNS_OUTPUT);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[3]->next->next->data)->idx, 4);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[3]->next->next->data)->itft, CNS_INPUT);
	ck_assert_ptr_eq(block->cbuf->iis[3]->next->next->next, NULL);
	ck_assert_int_eq((ssize_t)block->cells[0].deps->data, -1);
	ck_assert_ptr_eq(block->cells[0].deps->next, NULL);
	ck_assert_int_eq((ssize_t)block->cells[1].deps->data, -1);
	ck_assert_ptr_eq(block->cells[1].deps->next, NULL);
	ck_assert_int_eq((ssize_t)block->cells[2].deps->data, 0);
	ck_assert_int_eq((ssize_t)block->cells[2].deps->next->data, 1);
	ck_assert_ptr_eq(block->cells[2].deps->next->next, NULL);
	ck_assert_int_eq((ssize_t)block->cells[3].deps->data, 2);
	ck_assert_ptr_eq(block->cells[3].deps->next, NULL);
	ck_assert_int_eq((ssize_t)block->cells[4].deps->data, 3);
	ck_assert_ptr_eq(block->cells[4].deps->next, NULL);
	ck_assert_ptr_eq(block->cells[0].data.input, block->ibuf->buf);
	ck_assert_ptr_eq(block->cells[0].data.weight, block->wbuf->buf);
	ck_assert_ptr_eq(block->cells[0].data.output, block->cbuf->buf);
	ck_assert_ptr_eq(block->cells[1].data.input, block->ibuf->buf+1);
	ck_assert_ptr_eq(block->cells[1].data.weight, block->wbuf->buf+1);
	ck_assert_ptr_eq(block->cells[1].data.output, block->cbuf->buf+1);
	ck_assert_ptr_eq(block->cells[2].data.input, block->cbuf->buf);
	ck_assert_ptr_eq(block->cells[2].data.weight, block->cbuf->buf+1);
	ck_assert_ptr_eq(block->cells[2].data.output, block->cbuf->buf+2);
	ck_assert_ptr_eq(block->cells[3].data.input, block->cbuf->buf+2);
	ck_assert_ptr_eq(block->cells[3].data.weight, block->cbuf->buf+3);
	ck_assert_ptr_eq(block->cells[3].data.output, block->cbuf->buf+3);
	ck_assert_ptr_eq(block->cells[4].data.input, block->cbuf->buf+3);
	ck_assert_ptr_eq(block->cells[4].data.weight, NULL);
	ck_assert_ptr_eq(block->cells[4].data.output, block->obuf->buf);
	ck_assert_int_eq(block->cells[0].data.width, 8);
	ck_assert_int_eq(block->cells[1].data.width, 8);
	ck_assert_int_eq(block->cells[2].data.width, 8);
	ck_assert_int_eq(block->cells[3].data.width, 8);
	ck_assert_int_eq(block->cells[4].data.width, 8);
	ck_assert_int_eq(block->cells[0].data.dtype, CNS_INT8);
	ck_assert_int_eq(block->cells[1].data.dtype, CNS_INT8);
	ck_assert_int_eq(block->cells[2].data.dtype, CNS_INT8);
	ck_assert_int_eq(block->cells[3].data.dtype, CNS_INT8);
	ck_assert_int_eq(block->cells[4].data.dtype, CNS_INT8);
	ck_assert_int_eq(block->cells[0].index, 0);
	ck_assert_int_eq(block->cells[1].index, 1);
	ck_assert_int_eq(block->cells[2].index, 2);
	ck_assert_int_eq(block->cells[3].index, 3);
	ck_assert_int_eq(block->cells[4].index, 4);
	ck_assert_int_eq(block->cells[0].en, CNS_TRUE);
	ck_assert_int_eq(block->cells[1].en, CNS_TRUE);
	ck_assert_int_eq(block->cells[2].en, CNS_TRUE);
	ck_assert_int_eq(block->cells[3].en, CNS_TRUE);
	ck_assert_int_eq(block->cells[4].en, CNS_TRUE);
	ck_assert_ptr_eq(block->cells[0].op, cns_cell_op_mul_int8);
	ck_assert_ptr_eq(block->cells[1].op, cns_cell_op_mul_int8);
	ck_assert_ptr_eq(block->cells[2].op, cns_cell_op_add_int8);
	ck_assert_ptr_eq(block->cells[3].op, cns_cell_op_relu_int8);
	ck_assert_ptr_eq(block->cells[4].op, cns_cell_op_assign_int8);

	block = cns_block_expand(block, 2);
	ck_assert_int_eq(block->width, 8);
	ck_assert_int_eq(block->dtype, CNS_INT8);
	ck_assert_int_eq(block->length, 10);
	ck_assert_int_eq(block->ibuf->dtype, CNS_INT8);
	ck_assert_int_eq(block->ibuf->length, 10);
	ck_assert_int_eq(block->ibuf->head, 4);
	ck_assert_int_eq(block->wbuf->dtype, CNS_INT8);
	ck_assert_int_eq(block->wbuf->length, 10);
	ck_assert_int_eq(block->wbuf->head, 4);
	ck_assert_int_eq(block->obuf->dtype, CNS_INT8);
	ck_assert_int_eq(block->obuf->length, 10);
	ck_assert_int_eq(block->obuf->head, 2);
	ck_assert_int_eq(block->cbuf->dtype, CNS_INT8);
	ck_assert_int_eq(block->cbuf->length, 10);
	ck_assert_int_eq(block->cbuf->head, 8);
	ck_assert_int_eq(((cns_buf_ii *)block->ibuf->iis[0]->data)->idx, 0);
	ck_assert_int_eq(((cns_buf_ii *)block->ibuf->iis[0]->data)->itft, CNS_INPUT);
	ck_assert_ptr_eq(block->ibuf->iis[0]->next, NULL);
	ck_assert_int_eq(((cns_buf_ii *)block->ibuf->iis[1]->data)->idx, 1);
	ck_assert_int_eq(((cns_buf_ii *)block->ibuf->iis[1]->data)->itft, CNS_INPUT);
	ck_assert_ptr_eq(block->ibuf->iis[1]->next, NULL);
	ck_assert_int_eq(((cns_buf_ii *)block->wbuf->iis[0]->data)->idx, 0);
	ck_assert_int_eq(((cns_buf_ii *)block->wbuf->iis[0]->data)->itft, CNS_WEIGHT);
	ck_assert_ptr_eq(block->wbuf->iis[0]->next, NULL);
	ck_assert_int_eq(((cns_buf_ii *)block->wbuf->iis[1]->data)->idx, 1);
	ck_assert_int_eq(((cns_buf_ii *)block->wbuf->iis[1]->data)->itft, CNS_WEIGHT);
	ck_assert_ptr_eq(block->wbuf->iis[1]->next, NULL);
	ck_assert_int_eq(((cns_buf_ii *)block->obuf->iis[0]->data)->idx, 4);
	ck_assert_int_eq(((cns_buf_ii *)block->obuf->iis[0]->data)->itft, CNS_OUTPUT);
	ck_assert_ptr_eq(block->obuf->iis[0]->next, NULL);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[0]->data)->idx, 2);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[0]->data)->itft, CNS_INPUT);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[0]->next->data)->idx, 0);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[0]->next->data)->itft, CNS_OUTPUT);
	ck_assert_ptr_eq(block->cbuf->iis[0]->next->next, NULL);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[1]->data)->idx, 2);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[1]->data)->itft, CNS_WEIGHT);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[1]->next->data)->idx, 1);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[1]->next->data)->itft, CNS_OUTPUT);
	ck_assert_ptr_eq(block->cbuf->iis[1]->next->next, NULL);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[2]->data)->idx, 3);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[2]->data)->itft, CNS_INPUT);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[2]->next->data)->idx, 2);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[2]->next->data)->itft, CNS_OUTPUT);
	ck_assert_ptr_eq(block->cbuf->iis[2]->next->next, NULL);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[3]->data)->idx, 3);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[3]->data)->itft, CNS_WEIGHT);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[3]->next->data)->idx, 3);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[3]->next->data)->itft, CNS_OUTPUT);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[3]->next->next->data)->idx, 4);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[3]->next->next->data)->itft, CNS_INPUT);
	ck_assert_ptr_eq(block->cbuf->iis[3]->next->next->next, NULL);
	ck_assert_int_eq((ssize_t)block->cells[0].deps->data, -1);
	ck_assert_ptr_eq(block->cells[0].deps->next, NULL);
	ck_assert_int_eq((ssize_t)block->cells[1].deps->data, -1);
	ck_assert_ptr_eq(block->cells[1].deps->next, NULL);
	ck_assert_int_eq((ssize_t)block->cells[2].deps->data, 0);
	ck_assert_int_eq((ssize_t)block->cells[2].deps->next->data, 1);
	ck_assert_ptr_eq(block->cells[2].deps->next->next, NULL);
	ck_assert_int_eq((ssize_t)block->cells[3].deps->data, 2);
	ck_assert_ptr_eq(block->cells[3].deps->next, NULL);
	ck_assert_int_eq((ssize_t)block->cells[4].deps->data, 3);
	ck_assert_ptr_eq(block->cells[4].deps->next, NULL);
	ck_assert_ptr_eq(block->cells[0].data.input, block->ibuf->buf);
	ck_assert_ptr_eq(block->cells[0].data.weight, block->wbuf->buf);
	ck_assert_ptr_eq(block->cells[0].data.output, block->cbuf->buf);
	ck_assert_ptr_eq(block->cells[1].data.input, block->ibuf->buf+1);
	ck_assert_ptr_eq(block->cells[1].data.weight, block->wbuf->buf+1);
	ck_assert_ptr_eq(block->cells[1].data.output, block->cbuf->buf+1);
	ck_assert_ptr_eq(block->cells[2].data.input, block->cbuf->buf);
	ck_assert_ptr_eq(block->cells[2].data.weight, block->cbuf->buf+1);
	ck_assert_ptr_eq(block->cells[2].data.output, block->cbuf->buf+2);
	ck_assert_ptr_eq(block->cells[3].data.input, block->cbuf->buf+2);
	ck_assert_ptr_eq(block->cells[3].data.weight, block->cbuf->buf+3);
	ck_assert_ptr_eq(block->cells[3].data.output, block->cbuf->buf+3);
	ck_assert_ptr_eq(block->cells[4].data.input, block->cbuf->buf+3);
	ck_assert_ptr_eq(block->cells[4].data.weight, NULL);
	ck_assert_ptr_eq(block->cells[4].data.output, block->obuf->buf);
	ck_assert_int_eq(block->cells[0].data.width, 8);
	ck_assert_int_eq(block->cells[1].data.width, 8);
	ck_assert_int_eq(block->cells[2].data.width, 8);
	ck_assert_int_eq(block->cells[3].data.width, 8);
	ck_assert_int_eq(block->cells[4].data.width, 8);
	ck_assert_int_eq(block->cells[0].data.dtype, CNS_INT8);
	ck_assert_int_eq(block->cells[1].data.dtype, CNS_INT8);
	ck_assert_int_eq(block->cells[2].data.dtype, CNS_INT8);
	ck_assert_int_eq(block->cells[3].data.dtype, CNS_INT8);
	ck_assert_int_eq(block->cells[4].data.dtype, CNS_INT8);
	ck_assert_int_eq(block->cells[0].index, 0);
	ck_assert_int_eq(block->cells[1].index, 1);
	ck_assert_int_eq(block->cells[2].index, 2);
	ck_assert_int_eq(block->cells[3].index, 3);
	ck_assert_int_eq(block->cells[4].index, 4);
	ck_assert_int_eq(block->cells[0].en, CNS_TRUE);
	ck_assert_int_eq(block->cells[1].en, CNS_TRUE);
	ck_assert_int_eq(block->cells[2].en, CNS_TRUE);
	ck_assert_int_eq(block->cells[3].en, CNS_TRUE);
	ck_assert_int_eq(block->cells[4].en, CNS_TRUE);
	ck_assert_ptr_eq(block->cells[0].op, cns_cell_op_mul_int8);
	ck_assert_ptr_eq(block->cells[1].op, cns_cell_op_mul_int8);
	ck_assert_ptr_eq(block->cells[2].op, cns_cell_op_add_int8);
	ck_assert_ptr_eq(block->cells[3].op, cns_cell_op_relu_int8);
	ck_assert_ptr_eq(block->cells[4].op, cns_cell_op_assign_int8);

	ck_assert_int_eq(((cns_buf_ii *)block->ibuf->iis[2]->data)->idx, 5);
	ck_assert_int_eq(((cns_buf_ii *)block->ibuf->iis[2]->data)->itft, CNS_INPUT);
	ck_assert_ptr_eq(block->ibuf->iis[2]->next, NULL);
	ck_assert_int_eq(((cns_buf_ii *)block->ibuf->iis[3]->data)->idx, 6);
	ck_assert_int_eq(((cns_buf_ii *)block->ibuf->iis[3]->data)->itft, CNS_INPUT);
	ck_assert_ptr_eq(block->ibuf->iis[3]->next, NULL);
	ck_assert_int_eq(((cns_buf_ii *)block->wbuf->iis[2]->data)->idx, 5);
	ck_assert_int_eq(((cns_buf_ii *)block->wbuf->iis[2]->data)->itft, CNS_WEIGHT);
	ck_assert_ptr_eq(block->wbuf->iis[2]->next, NULL);
	ck_assert_int_eq(((cns_buf_ii *)block->wbuf->iis[3]->data)->idx, 6);
	ck_assert_int_eq(((cns_buf_ii *)block->wbuf->iis[3]->data)->itft, CNS_WEIGHT);
	ck_assert_ptr_eq(block->wbuf->iis[3]->next, NULL);
	ck_assert_int_eq(((cns_buf_ii *)block->obuf->iis[1]->data)->idx, 9);
	ck_assert_int_eq(((cns_buf_ii *)block->obuf->iis[1]->data)->itft, CNS_OUTPUT);
	ck_assert_ptr_eq(block->obuf->iis[1]->next, NULL);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[4]->data)->idx, 7);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[4]->data)->itft, CNS_INPUT);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[4]->next->data)->idx, 5);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[4]->next->data)->itft, CNS_OUTPUT);
	ck_assert_ptr_eq(block->cbuf->iis[4]->next->next, NULL);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[5]->data)->idx, 7);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[5]->data)->itft, CNS_WEIGHT);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[5]->next->data)->idx, 6);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[5]->next->data)->itft, CNS_OUTPUT);
	ck_assert_ptr_eq(block->cbuf->iis[5]->next->next, NULL);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[6]->data)->idx, 8);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[6]->data)->itft, CNS_INPUT);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[6]->next->data)->idx, 7);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[6]->next->data)->itft, CNS_OUTPUT);
	ck_assert_ptr_eq(block->cbuf->iis[6]->next->next, NULL);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[7]->data)->idx, 8);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[7]->data)->itft, CNS_WEIGHT);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[7]->next->data)->idx, 8);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[7]->next->data)->itft, CNS_OUTPUT);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[7]->next->next->data)->idx, 9);
	ck_assert_int_eq(((cns_buf_ii *)block->cbuf->iis[7]->next->next->data)->itft, CNS_INPUT);
	ck_assert_ptr_eq(block->cbuf->iis[7]->next->next->next, NULL);
	ck_assert_int_eq((ssize_t)block->cells[5].deps->data, -1);
	ck_assert_ptr_eq(block->cells[5].deps->next, NULL);
	ck_assert_int_eq((ssize_t)block->cells[6].deps->data, -1);
	ck_assert_ptr_eq(block->cells[6].deps->next, NULL);
	ck_assert_int_eq((ssize_t)block->cells[7].deps->data, 5);
	ck_assert_int_eq((ssize_t)block->cells[7].deps->next->data, 6);
	ck_assert_ptr_eq(block->cells[7].deps->next->next, NULL);
	ck_assert_int_eq((ssize_t)block->cells[8].deps->data, 7);
	ck_assert_ptr_eq(block->cells[8].deps->next, NULL);
	ck_assert_int_eq((ssize_t)block->cells[9].deps->data, 8);
	ck_assert_ptr_eq(block->cells[9].deps->next, NULL);
	ck_assert_ptr_eq(block->cells[5].data.input, block->ibuf->buf+2);
	ck_assert_ptr_eq(block->cells[5].data.weight, block->wbuf->buf+2);
	ck_assert_ptr_eq(block->cells[5].data.output, block->cbuf->buf+4);
	ck_assert_ptr_eq(block->cells[6].data.input, block->ibuf->buf+3);
	ck_assert_ptr_eq(block->cells[6].data.weight, block->wbuf->buf+3);
	ck_assert_ptr_eq(block->cells[6].data.output, block->cbuf->buf+5);
	ck_assert_ptr_eq(block->cells[7].data.input, block->cbuf->buf+4);
	ck_assert_ptr_eq(block->cells[7].data.weight, block->cbuf->buf+5);
	ck_assert_ptr_eq(block->cells[7].data.output, block->cbuf->buf+6);
	ck_assert_ptr_eq(block->cells[8].data.input, block->cbuf->buf+6);
	ck_assert_ptr_eq(block->cells[8].data.weight, block->cbuf->buf+7);
	ck_assert_ptr_eq(block->cells[8].data.output, block->cbuf->buf+7);
	ck_assert_ptr_eq(block->cells[9].data.input, block->cbuf->buf+7);
	ck_assert_ptr_eq(block->cells[9].data.weight, NULL);
	ck_assert_ptr_eq(block->cells[9].data.output, block->obuf->buf+1);
	ck_assert_int_eq(block->cells[5].data.width, 8);
	ck_assert_int_eq(block->cells[6].data.width, 8);
	ck_assert_int_eq(block->cells[7].data.width, 8);
	ck_assert_int_eq(block->cells[8].data.width, 8);
	ck_assert_int_eq(block->cells[9].data.width, 8);
	ck_assert_int_eq(block->cells[5].data.dtype, CNS_INT8);
	ck_assert_int_eq(block->cells[6].data.dtype, CNS_INT8);
	ck_assert_int_eq(block->cells[7].data.dtype, CNS_INT8);
	ck_assert_int_eq(block->cells[8].data.dtype, CNS_INT8);
	ck_assert_int_eq(block->cells[9].data.dtype, CNS_INT8);
	ck_assert_int_eq(block->cells[5].index, 5);
	ck_assert_int_eq(block->cells[6].index, 6);
	ck_assert_int_eq(block->cells[7].index, 7);
	ck_assert_int_eq(block->cells[8].index, 8);
	ck_assert_int_eq(block->cells[9].index, 9);
	ck_assert_int_eq(block->cells[5].en, CNS_TRUE);
	ck_assert_int_eq(block->cells[6].en, CNS_TRUE);
	ck_assert_int_eq(block->cells[7].en, CNS_TRUE);
	ck_assert_int_eq(block->cells[8].en, CNS_TRUE);
	ck_assert_int_eq(block->cells[9].en, CNS_TRUE);
	ck_assert_ptr_eq(block->cells[5].op, cns_cell_op_mul_int8);
	ck_assert_ptr_eq(block->cells[6].op, cns_cell_op_mul_int8);
	ck_assert_ptr_eq(block->cells[7].op, cns_cell_op_add_int8);
	ck_assert_ptr_eq(block->cells[8].op, cns_cell_op_relu_int8);
	ck_assert_ptr_eq(block->cells[9].op, cns_cell_op_assign_int8);
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
	tcase_add_test(tc_block, test_block_set_op);
	tcase_add_test(tc_block, test_block_dep_graph);
	tcase_add_test(tc_block, test_block_link_io);
	tcase_add_test(tc_block, test_block_link);
	tcase_add_test(tc_block, test_block_find_itfp);
	tcase_add_test(tc_block, test_block_expand);
	suite_add_tcase(s, tc_block);

	return s;
}
