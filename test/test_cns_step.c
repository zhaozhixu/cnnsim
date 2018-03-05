#include <string.h>
#include "../src/cns_step.h"
#include "../src/cns_list.h"
#include "../src/cns_cell_op.h"
#include "test_cnnsim.h"

static cns_step *step;

struct input_weight {
	size_t input_len;
	size_t weight_len;
	int8_t *input;
	int8_t *weight;
};

struct output {
	size_t output_len;
	int8_t *output;
};

static void setup(void)
{
	step = NULL;
}

static void teardown(void)
{
	cns_step_free(step);
}

static void op_pre(cns_block *block, void *data)
{
	struct input_weight *iw;

	iw = (struct input_weight *)data;
	memmove(block->ibuf->buf, iw->input, sizeof(int8_t)*iw->input_len);
	memmove(block->wbuf->buf, iw->weight, sizeof(int8_t)*iw->weight_len);
}

static void op_post(cns_block *block, void *data)
{
	struct output *o;

	o = (struct output *)data;
	memmove(o->output, block->obuf->buf, sizeof(int8_t)*o->output_len);
}

START_TEST(test_step_run)
{
	cns_block *block;
	size_t b_size;
	size_t *en_array;
	cns_list *ens;
	size_t i;
	struct input_weight iw_buf;
	struct output o_buf;

	b_size = 9 + 9 + 9;
	block = cns_block_create(b_size, CNS_INT8, 8);
	for (i = 0; i < 9; i++) { /* 9 multipliers */
		cns_block_link_io(block, i, CNS_INPUT);
		cns_block_link_io(block, i, CNS_WEIGHT);
		cns_block_set_op(block, i, cns_cell_op_mul_int8);
	}
	for (i = 9; i < 18; i++) { /* 9 adders */
		cns_block_link(block, i, CNS_INPUT, i-9, CNS_OUTPUT);
		cns_block_link(block, i, CNS_WEIGHT, i, CNS_OUTPUT);
		cns_block_set_op(block, i, cns_cell_op_add_int8);
	}
	for (i = 18; i < 27; i++) { /* 9 outputs */
		cns_block_link(block, i, CNS_INPUT, i-9, CNS_OUTPUT);
		cns_block_link_io(block, i, CNS_OUTPUT);
		cns_block_set_op(block, i, cns_cell_op_assign_int8);
	}

	en_array = (size_t *)cns_alloc(sizeof(size_t) * b_size);
	for (i = 0; i < b_size; i++)
		en_array[i] = i;
	ens = cns_list_from_array_size_t(en_array, b_size);

	iw_buf.input = cns_alloc(sizeof(int8_t) * 9);
	iw_buf.weight = cns_alloc(sizeof(int8_t) * 9);
	o_buf.output = cns_alloc(sizeof(int8_t) * 9);
	iw_buf.input_len = 9;
	iw_buf.weight_len = 9;
	o_buf.output_len = 9;
	for (i = 0; i < 9; i++) {
		iw_buf.input[i] = i;
		iw_buf.weight[i] = i;
	}

	step = cns_step_create(block, ens, op_pre, &iw_buf, op_post, &o_buf);
	cns_step_run(step);

	for (i = 0; i < 9; i++) {
		ck_assert_int_eq(o_buf.output[i], i*i);
	}

	cns_block_free(block);
	cns_list_free(ens);
	cns_free(en_array);
	cns_free(iw_buf.input);
	cns_free(iw_buf.weight);
	cns_free(o_buf.output);
}
END_TEST

Suite *make_step_suite(void)
{
	Suite *s;
	TCase *tc_step;

	s = suite_create("step");
	tc_step = tcase_create("step");
	tcase_add_checked_fixture(tc_step, setup, teardown);
	tcase_add_test(tc_step, test_step_run);
	suite_add_tcase(s, tc_step);

	return s;
}
