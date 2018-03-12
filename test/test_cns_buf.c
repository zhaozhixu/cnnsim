#include "test_cnnsim.h"
#include "../src/cns_buf.h"
#include "../src/cns_util.h"

static cns_buf *buf;
static size_t length;

static void setup(void)
{
	length = 5;
	buf = cns_buf_create(length, CNS_UINT8);
}

static void teardown(void)
{
	cns_buf_free(buf);
}

START_TEST(test_buf_create)
{
	size_t i;

	ck_assert_int_eq(buf->dtype, CNS_UINT8);
	ck_assert_int_eq(buf->length, length);
	ck_assert_int_eq(buf->head, 0);
	for (i = 0; i < length; i++) {
		ck_assert_ptr_eq(buf->iis[i], NULL);
		ck_assert_int_eq(((uint8_t *)buf->buf)[i], 0);
	}
}
END_TEST

START_TEST(test_buf_append)
{
	void *p;

	p = cns_buf_append(buf, 0, CNS_INPUT);
	ck_assert_int_eq(buf->head, 1);
	ck_assert_ptr_eq(buf->buf, p);
	ck_assert_int_eq(((cns_buf_ii *)buf->iis[0]->data)->idx, 0);
	ck_assert_int_eq(((cns_buf_ii *)buf->iis[0]->data)->itft, CNS_INPUT);
	ck_assert_ptr_eq((cns_buf_ii *)buf->iis[0]->next, NULL);

	p = cns_buf_append(buf, 1, CNS_INPUT);
	ck_assert_int_eq(buf->head, 2);
	ck_assert_ptr_eq((uint8_t *)buf->buf+1, p);
	ck_assert_int_eq(((cns_buf_ii *)buf->iis[1]->data)->idx, 1);
	ck_assert_int_eq(((cns_buf_ii *)buf->iis[1]->data)->itft, CNS_INPUT);
	ck_assert_ptr_eq((cns_buf_ii *)buf->iis[1]->next, NULL);
}
END_TEST

START_TEST(test_buf_attach)
{
	void *p;

	cns_buf_append(buf, 0, CNS_INPUT);
	cns_buf_append(buf, 1, CNS_INPUT);

	p = cns_buf_attach(buf, 0, 2, CNS_INPUT);
	ck_assert_int_eq(buf->head, 2);
	ck_assert_ptr_eq(buf->buf, p);
	ck_assert_int_eq(((cns_buf_ii *)buf->iis[0]->next->data)->idx, 2);
	ck_assert_int_eq(((cns_buf_ii *)buf->iis[0]->next->data)->itft, CNS_INPUT);
	ck_assert_ptr_eq((cns_buf_ii *)buf->iis[0]->next->next, NULL);

	p = cns_buf_attach(buf, 1, 3, CNS_INPUT);
	ck_assert_int_eq(buf->head, 2);
	ck_assert_ptr_eq((uint8_t *)buf->buf+1, p);
	ck_assert_int_eq(((cns_buf_ii *)buf->iis[1]->next->data)->idx, 3);
	ck_assert_int_eq(((cns_buf_ii *)buf->iis[1]->next->data)->itft, CNS_INPUT);
	ck_assert_ptr_eq((cns_buf_ii *)buf->iis[1]->next->next, NULL);
}
END_TEST

START_TEST(test_buf_detach)
{
	cns_buf_append(buf, 0, CNS_INPUT);
	cns_buf_attach(buf, 0, 2, CNS_WEIGHT);
	cns_buf_detach(buf, 0, 0, CNS_INPUT);
	ck_assert_int_eq(buf->head, 1);
	ck_assert_int_eq(((cns_buf_ii *)buf->iis[0]->data)->idx, 2);
	ck_assert_int_eq(((cns_buf_ii *)buf->iis[0]->data)->itft, CNS_WEIGHT);
	ck_assert_ptr_eq((cns_buf_ii *)buf->iis[0]->next, NULL);

	cns_buf_detach(buf, 0, 2, CNS_INPUT);
	ck_assert_int_eq(buf->head, 1);
	ck_assert_int_eq(((cns_buf_ii *)buf->iis[0]->data)->idx, 2);
	ck_assert_int_eq(((cns_buf_ii *)buf->iis[0]->data)->itft, CNS_WEIGHT);
	ck_assert_ptr_eq((cns_buf_ii *)buf->iis[0]->next, NULL);

	cns_buf_detach(buf, 0, 2, CNS_WEIGHT);
	ck_assert_int_eq(buf->head, 1);
	ck_assert_ptr_eq((cns_buf_ii *)buf->iis[0], NULL);
}
END_TEST

START_TEST(test_buf_seek)
{
	cns_buf_append(buf, 0, CNS_INPUT);
	cns_buf_append(buf, 1, CNS_INPUT);

	cns_buf_seek(buf, 0);
	ck_assert_int_eq(buf->head, 0);

	ck_assert_int_eq(cns_buf_seek(buf, 9), -1);
	ck_assert_int_eq(buf->head, 0);
}
END_TEST

START_TEST(test_buf_index)
{
	int idx;
	void *p;

	p = cns_buf_append(buf, 0, CNS_INPUT);
	idx = cns_buf_index(buf, p);
	ck_assert_int_eq(idx, 0);

	p = cns_buf_append(buf, 1, CNS_INPUT);
	idx = cns_buf_index(buf, p);
	ck_assert_int_eq(idx, 1);
}
END_TEST

START_TEST(test_buf_addr)
{
	void *p;

	cns_buf_append(buf, 0, CNS_INPUT);
	p = cns_buf_addr(buf, 0);
	ck_assert_ptr_eq(p, buf->buf);

	cns_buf_append(buf, 1, CNS_INPUT);
	p = cns_buf_addr(buf, 1);
	ck_assert_ptr_eq(p, (uint8_t *)buf->buf+1);
}
END_TEST
/* end of tests */

Suite *make_buf_suite(void)
{
	Suite *s;
	s = suite_create("buf");

	TCase *tc_buf;
	tc_buf = tcase_create("tc_buf");
	tcase_add_checked_fixture(tc_buf, setup, teardown);

	tcase_add_test(tc_buf, test_buf_create);
	tcase_add_test(tc_buf, test_buf_append);
	tcase_add_test(tc_buf, test_buf_attach);
	tcase_add_test(tc_buf, test_buf_detach);
	tcase_add_test(tc_buf, test_buf_seek);
	tcase_add_test(tc_buf, test_buf_index);
	tcase_add_test(tc_buf, test_buf_addr);
	/* end of adding tests */

	suite_add_tcase(s, tc_buf);

	return s;
}
