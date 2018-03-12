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

	iis = NULL;
	iis = cns_iis_append(iis, 0, CNS_INPUT);
	iis = cns_iis_append(iis, 1, CNS_INPUT);
	iis = cns_iis_append(iis, 2, CNS_WEIGHT);

	reg = cns_reg_create(0, iis);
	ck_assert_int_eq(reg->ori, 0);

	cns_reg_free(reg);
}
END_TEST

START_TEST(test_cns_reg_buf_create)
{
}
END_TEST

START_TEST(test_cns_reg_buf_link)
{
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
