#include "test_cnnsim.h"
#include "../src/cns_util.h"
#include "../src/cns_wire.h"
#include "../src/cns_list.h"

static size_t len = 5;

static void setup(void)
{
}

static void teardown(void)
{
}

START_TEST(test_cns_wire_buf_create)
{
	cns_wire_buf *buf;
	int i;

	buf = cns_wire_buf_create(len, CNS_INT8);
	ck_assert_int_eq(buf->dtype, CNS_INT8);
	ck_assert_int_eq(buf->len, len);
	ck_assert_int_eq(buf->head, 0);
	for (i = 0; i < buf->len; i++) {
		ck_assert_int_eq((int8_t)buf->buf[i], 0);
		ck_assert_ptr_eq(buf->iis[i], NULL);
	}

	cns_wire_buf_free(buf);
}
END_TEST

START_TEST(test_cns_wire_buf_index)
{
	cns_wire_buf *buf;
	int idx;

	buf = cns_wire_buf_create(len, CNS_INT8);
	idx = cns_wire_buf_index(buf, (int8_t *)buf->buf+3);
	ck_assert_int_eq(idx, 3);

	cns_wire_buf_free(buf);
}
END_TEST

START_TEST(test_cns_wire_buf_addr)
{
	cns_wire_buf *buf;
	void *addr;

	buf = cns_wire_buf_create(len, CNS_INT8);
	addr = cns_wire_buf_addr(buf, 3);
	ck_assert_ptr_eq(addr, (int8_t *)buf->buf+3);

	cns_wire_buf_free(buf);
}
END_TEST

START_TEST(test_cns_wire_buf_link)
{
	cns_wire_buf *buf;
	cns_ii *ii;

	buf = cns_wire_buf_create(len, CNS_INT8);
	cns_wire_buf_link(buf, 0, 0, CNS_INPUT);
	ii = cns_list_nth_data(buf->iis[0], 0);
	ck_assert_int_eq(ii->idx, 0);
	ck_assert_int_eq(ii->itft, 0);

	cns_wire_buf_free(buf);

}
END_TEST

START_TEST(test_cns_wire_buf_unlink)
{
}
END_TEST

START_TEST(test_cns_wire_buf_append)
{
}
END_TEST
/* end of tests */

Suite *make_wire_suite(void)
{
	Suite *s;
	TCase *tc_wire;

	s = suite_create("wire");
	tc_wire = tcase_create("wire");
	tcase_add_checked_fixture(tc_wire, setup, teardown);

	tcase_add_test(tc_wire, test_cns_wire_buf_create);
	tcase_add_test(tc_wire, test_cns_wire_buf_index);
	tcase_add_test(tc_wire, test_cns_wire_buf_addr);
	tcase_add_test(tc_wire, test_cns_wire_buf_link);
	tcase_add_test(tc_wire, test_cns_wire_buf_unlink);
	tcase_add_test(tc_wire, test_cns_wire_buf_append);
	/* end of adding tests */

	suite_add_tcase(s, tc_wire);

	return s;
}
