#include "test_cnnsim.h"
#include "../src/cns_util.h"
#include "../src/cns_reg.h"

static size_t len = 5;

static void setup(void)
{
}

static void teardown(void)
{
}

START_TEST(test_cns_reg_create)
{
	cns_reg *reg;
	cns_list *iis;
	cns_ii *ii;

	iis = NULL;
	iis = cns_iis_append(iis, 0, CNS_INPUT);
	iis = cns_iis_append(iis, 1, CNS_INPUT);
	iis = cns_iis_append(iis, 2, CNS_WEIGHT);

	reg = cns_reg_create(0, iis);
	ck_assert_int_eq(reg->ori, 0);
	ck_assert_int_eq(reg->len, 3);

	ii = cns_list_nth_data(reg->iis, 0);
	ck_assert_int_eq(ii->idx, 0);
	ck_assert_int_eq(ii->itft, CNS_INPUT);

	ii = cns_list_nth_data(reg->iis, 1);
	ck_assert_int_eq(ii->idx, 1);
	ck_assert_int_eq(ii->itft, CNS_INPUT);

	ii = cns_list_nth_data(reg->iis, 2);
	ck_assert_int_eq(ii->idx, 2);
	ck_assert_int_eq(ii->itft, CNS_WEIGHT);

	cns_list_free_deep(iis);
	cns_reg_free(reg);
}
END_TEST

START_TEST(test_cns_reg_buf_create)
{
	cns_reg_buf *buf;
	size_t i;

	buf = cns_reg_buf_create(len, CNS_INT8);
	ck_assert_int_eq(buf->dtype, CNS_INT8);
	ck_assert_int_eq(buf->len, len);
	ck_assert_int_eq(buf->head, 0);
	ck_assert_ptr_eq(buf->regs, NULL);
	for (i = 0; i < len; i++)
		ck_assert_int_eq(((int8_t *)buf->buf)[i], 0);

	cns_reg_buf_free(buf);
}
END_TEST

START_TEST(test_cns_reg_buf_link)
{
	cns_reg_buf *buf;
	cns_list *iis;
	void *p;

	buf = cns_reg_buf_create(len, CNS_INT8);

	iis = NULL;
	iis = cns_iis_append(iis, 0, CNS_INPUT);
	iis = cns_iis_append(iis, 1, CNS_INPUT);
	iis = cns_iis_append(iis, 2, CNS_WEIGHT);
	p = cns_reg_buf_link(buf, 0, iis);

	ck_assert_int_eq(buf->head, 3);
	ck_assert_ptr_eq(p, buf->buf);

	iis = cns_iis_append(iis, 3, CNS_WEIGHT);
	p = cns_reg_buf_link(buf, 0, iis);
	ck_assert_int_eq(buf->head, 4);
	ck_assert_ptr_eq(p, buf->buf);

	cns_list_free_deep(iis);
	cns_reg_buf_free(buf);
}
END_TEST
/* end of tests */

Suite *make_reg_suite(void)
{
	Suite *s;
	TCase *tc_reg;

	s = suite_create("reg");
	tc_reg = tcase_create("reg");
	tcase_add_checked_fixture(tc_reg, setup, teardown);

	tcase_add_test(tc_reg, test_cns_reg_create);
	tcase_add_test(tc_reg, test_cns_reg_buf_create);
	tcase_add_test(tc_reg, test_cns_reg_buf_link);
	/* end of adding tests */

	suite_add_tcase(s, tc_reg);

	return s;
}
