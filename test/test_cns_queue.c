#include "test_cnnsim.h"
#include "../src/cns_util.h"
#include "../src/cns_queue.h"

static int *data;
static size_t data_len;

static void setup(void)
{
	int i;
	data_len = 3;
	data = cns_alloc(sizeof(int) * data_len);
	for (i = 0; i < data_len; i++) {
		data[i] = i;
	}
}

static void teardown(void)
{
	cns_free(data);
}

START_TEST(test_queue_create)
{
	cns_queue *q;

	q = cns_queue_create();
	ck_assert_int_eq(q->size, 0);
	ck_assert_ptr_eq(q->head, NULL);
	ck_assert_ptr_eq(q->tail, NULL);
	cns_queue_free(q);
}
END_TEST

START_TEST(test_queue_enqueue_dequeue)
{
	cns_queue *q;

	q = cns_queue_enqueue(NULL, &data[0]);
	q = cns_queue_enqueue(q, &data[1]);
	q = cns_queue_enqueue(q, &data[2]);
	ck_assert_int_eq(q->size, 3);
	ck_assert_int_eq(*(int *)cns_queue_dequeue(q), 0);
	ck_assert_int_eq(*(int *)cns_queue_dequeue(q), 1);
	ck_assert_int_eq(*(int *)cns_queue_dequeue(q), 2);
	ck_assert_ptr_eq(cns_queue_dequeue(q), NULL);
	ck_assert_int_eq(q->size, 0);

	cns_queue_free(q);
}
END_TEST

Suite *make_queue_suite(void)
{
	Suite *s;
	s = suite_create("queue");

	TCase *tc_queue;
	tc_queue = tcase_create("tc_queue");
	tcase_add_checked_fixture(tc_queue, setup, teardown);
	tcase_add_test(tc_queue, test_queue_create);
	tcase_add_test(tc_queue, test_queue_enqueue_dequeue);
	suite_add_tcase(s, tc_queue);

	return s;
}
