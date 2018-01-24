#include "test_cnnsim.h"
#include "../src/cns_util.h"
#include "../src/cns_list.h"

static int *data;
static size_t data_len;
static cns_list *list;

static void setup(void)
{
	int i;
	list = NULL;
	data_len = 5;
	data = cns_alloc(sizeof(int) * data_len);
	for (i = 0; i < 5; i++) {
		data[i] = i;
		list = cns_list_append(list, &data[i]);
	}
}

static void teardown(void)
{
	cns_free(data);
	cns_list_free(list);
}

START_TEST(test_list_append_nth)
{
	cns_list *l;

	l = cns_list_append(NULL, &data[0]);
	l = cns_list_append(l, &data[1]);
	ck_assert_int_eq(*(int *)cns_list_nth_data(l, 0), 0);
	ck_assert_int_eq(*(int *)cns_list_nth_data(l, 1), 1);
	cns_list_free(l);
}
END_TEST

START_TEST(test_list_remove_insert_nth)
{
	cns_list_remove_nth(list, 2);
	ck_assert_int_eq(*(int *)cns_list_nth_data(list, 2), 3);
	ck_assert_int_eq(*(int *)cns_list_nth_data(list, 1), 1);

	cns_list_insert_nth(list, &data[2], 2);
	ck_assert_int_eq(*(int *)cns_list_nth_data(list, 2), 2);
	ck_assert_int_eq(*(int *)cns_list_nth_data(list, 3), 3);
	ck_assert_int_eq(*(int *)cns_list_nth_data(list, 1), 1);
}
END_TEST

Suite *make_list_suite(void)
{
	Suite *s;
	s = suite_create("list");

	TCase *tc_list;
	tc_list = tcase_create("tc_list");
	tcase_add_checked_fixture(tc_list, setup, teardown);
	tcase_add_test(tc_list, test_list_append_nth);
	tcase_add_test(tc_list, test_list_remove_insert_nth);
	suite_add_tcase(s, tc_list);

	return s;
}
