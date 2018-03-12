#include "test_cnnsim.h"
#include "../src/cns_util.h"
#include "../src/cns_ii.h"

static void setup(void)
{
}

static void teardown(void)
{
}

START_TEST(test_cns_ii_create)
{
	cns_ii *ii;

	ii = cns_ii_create(0, CNS_INPUT);
	ck_assert_int_eq(ii->idx, 0);
	ck_assert_int_eq(ii->itft, CNS_INPUT);

	cns_ii_free(ii);
}
END_TEST

START_TEST(test_cns_ii_cmp)
{
	cns_ii *ii1, *ii2, *ii3, *ii4;

	ii1 = cns_ii_create(0, CNS_INPUT);
	ii2 = cns_ii_create(0, CNS_INPUT);
	ck_assert_int_eq(cns_ii_cmp(ii1, ii2), 0);

	ii3 = cns_ii_create(0, CNS_WEIGHT);
	ck_assert_int_eq(cns_ii_cmp(ii1, ii3), -1);

	ii4 = cns_ii_create(1, CNS_INPUT);
	ck_assert_int_eq(cns_ii_cmp(ii1, ii4), -1);

	cns_ii_free(ii1);
	cns_ii_free(ii2);
	cns_ii_free(ii3);
	cns_ii_free(ii4);
}
END_TEST

START_TEST(test_cns_iis_append)
{
	cns_list *iis;
	cns_ii *ii;

	iis = NULL;
	iis = cns_iis_append(iis, 0, CNS_INPUT);
	iis = cns_iis_append(iis, 1, CNS_INPUT);

	ii = cns_list_nth_data(iis, 0);
	ck_assert_int_eq(ii->idx, 0);
	ck_assert_int_eq(ii->itft, CNS_INPUT);
	ii = cns_list_nth_data(iis, 1);
	ck_assert_int_eq(ii->idx, 1);
	ck_assert_int_eq(ii->itft, CNS_INPUT);
	ii = cns_list_nth_data(iis, 2);
	ck_assert_ptr_eq(ii, NULL);

	cns_list_free_deep(iis);
}
END_TEST

START_TEST(test_cns_iis_copy)
{
	cns_list *iis, *iis_cpy;
	cns_ii *ii;

	iis = NULL;
	iis = cns_iis_append(iis, 0, CNS_INPUT);
	iis = cns_iis_append(iis, 1, CNS_INPUT);
	iis_cpy = cns_iis_copy(iis);

	ii = cns_list_nth_data(iis_cpy, 0);
	ck_assert_int_eq(ii->idx, 0);
	ck_assert_int_eq(ii->itft, CNS_INPUT);
	ii = cns_list_nth_data(iis_cpy, 1);
	ck_assert_int_eq(ii->idx, 1);
	ck_assert_int_eq(ii->itft, CNS_INPUT);
	ii = cns_list_nth_data(iis_cpy, 2);
	ck_assert_ptr_eq(ii, NULL);

	cns_list_free_deep(iis);
	cns_list_free_deep(iis_cpy);
}
END_TEST
/* end of tests */

Suite *make_ii_suite(void)
{
	Suite *s;
	TCase *tc_ii;

	s = suite_create("ii");
	tc_ii = tcase_create("ii");
	tcase_add_checked_fixture(tc_ii, setup, teardown);

	tcase_add_test(tc_ii, test_cns_ii_create);
	tcase_add_test(tc_ii, test_cns_ii_cmp);
	tcase_add_test(tc_ii, test_cns_iis_append);
	tcase_add_test(tc_ii, test_cns_iis_copy);
	/* end of adding tests */

	suite_add_tcase(s, tc_ii);

	return s;
}
