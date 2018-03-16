#include <string.h>
#include "../src/cns_util.h"
#include "../src/cns_sch.h"
#include "../src/cns_cell_op.h"
#include "test_cnnsim.h"

static int8_t *input;
static int8_t *weight;
static int8_t *output;

static void setup(void)
{
	input = (int8_t *)cns_alloc(sizeof(int8_t) * 9 * 9);
	weight = (int8_t *)cns_alloc(sizeof(int8_t) * 9 * 9);
	output = (int8_t *)cns_alloc(sizeof(int8_t));
}

static void teardown(void)
{
}

static cns_block *build_block()
{
	cns_block *block;
	size_t b_size;
	size_t *en_array;
	cns_list *ens;
	size_t i;

	b_size = 9 + 9 + 9 + 1 + 1 + 1;
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
	for (i = 18; i < 27; i++) { /* 9 relus for 1x1 conv */
		cns_block_link(block, i, CNS_INPUT, i-9, CNS_OUTPUT);
		cns_block_link_c(block, i, CNS_OUTPUT);
		cns_block_set_op(block, i, cns_cell_op_relu_int8);
	}
	/* max pooling for 1x1 conv */
	cns_block_link(block, 27, CNS_INPUT, 18, CNS_OUTPUT);
	cns_block_link_io(block, 27, CNS_OUTPUT);
	cns_block_set_op(block, 27, cns_cell_op_poolmax_9_int8);
	/* adder for 3x3 conv */
	cns_block_link(block, 28, CNS_INPUT, 9, CNS_OUTPUT);
	cns_block_set_op(block, 28, cns_cell_op_add_9_int8);
	/* relu for 3x3 conv */
	cns_block_link(block, 29, CNS_INPUT, 28, CNS_OUTPUT);
	cns_block_set_op(block, 29, cns_cell_op_relu_int8);

	block = cns_block_expand(block, 9, 1);
	/* max pooling for 3x3 conv */
	for (i = 0; i < 9; i++)
		cns_block_link_c(block, i * 30 + 29, CNS_OUTPUT);
	cns_block_link(block, 90, CNS_INPUT, 29, CNS_OUTPUT);
	cns_buf_seek(block->obuf, 0);
	cns_block_link_io(block, 90, CNS_OUTPUT);
	cns_block_set_op(block, 90, cns_cell_op_poolmax_9_int8);

	return block;
}

static cns_step *build_step_muladd(cns_block *block)
{
	cns_step *step;
	cns_list *ens;
	cns_step_iwbuf *iw_buf;

	iw_buf = cns_step_iwbuf_create(9*9, 9*9, CNS_INT8);
}

START_TEST(test_sch_run)
{
	size_t i;
	cns_sch *sch;
	cns_block *block;

	block = build_block();

	sch = cns_sch_create();

	cns_sch_free(sch);
	cns_block_free(block);
}
END_TEST
/* end of tests */

Suite *make_sch_suite(void)
{
	Suite *s;
	TCase *tc_sch;

	s = suite_create("sch");
	tc_sch = tcase_create("sch");
	tcase_add_checked_fixture(tc_sch, setup, teardown);

	tcase_add_test(tc_sch, test_sch_run);
	/* end of adding tests */

	suite_add_tcase(s, tc_sch);

	return s;
}
