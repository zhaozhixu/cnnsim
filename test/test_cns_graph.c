#include "test_cnnsim.h"
#include "../src/cns_util.h"
#include "../src/cns_graph.h"

static int *data;
static size_t data_len;
static 	cns_graph *graph;

static void setup(void)
{
	int i;
	data_len = 5;
	data = cns_alloc(sizeof(int) * data_len);
	for (i = 0; i < data_len; i++) {
		data[i] = i;
	}

	graph = cns_graph_create();
}

static void teardown(void)
{
	cns_free(data);
	cns_graph_free(graph);
}

START_TEST(test_graph_node_create)
{
	cns_graph_node *node;

	node = cns_graph_node_create(&data[0]);
	ck_assert_int_eq(*(int *)node->data, data[0]);
	ck_assert_int_eq(node->indegree, 0);
	ck_assert_int_eq(node->outdegree, 0);
	ck_assert_ptr_eq(node->adj_nodes, NULL);

	cns_graph_node_free(node);
}
END_TEST

START_TEST(test_graph_create)
{
	cns_graph *g;

	g = cns_graph_create();
	ck_assert_int_eq(g->size, 0);
	ck_assert_ptr_eq(g->nodes, NULL);

	cns_graph_free(g);
}
END_TEST

START_TEST(test_graph_add)
{
	cns_graph_node *n;
	cns_list *l;
	int i;

	for (i = 0; i < data_len; i++) {
		n = cns_graph_add(graph, &data[i]);
		ck_assert_int_eq(*(int *)n->data, data[i]);
		ck_assert_int_eq(graph->size, i+1);
	}
	for (i = 0, l = graph->nodes; l; l = l->next, i++)
		ck_assert_int_eq(*(int *)((cns_graph_node *)l->data)->data, data[i]);
}
END_TEST

START_TEST(test_graph_find)
{
	cns_graph_node **n_array;
	cns_graph_node *n;
	int i, num;

	n_array = (cns_graph_node **)cns_alloc(sizeof(cns_graph_node *)*data_len);
	for (i = 0; i < data_len; i++) {
		n_array[i] = cns_graph_add(graph, &data[i]);
		ck_assert_ptr_eq(cns_graph_find(graph, &data[i]), n_array[i]);
	}
	for (i = 0; i < data_len; i++) {
		n = cns_graph_add(graph, &data[i]);
		ck_assert_ptr_ne(cns_graph_find(graph, &data[i]), n);
	}
	for (i = 0; i < data_len; i++)
		ck_assert_ptr_eq(cns_graph_find(graph, &data[i]), n_array[i]);
	cns_free(n_array);

	num = data_len;
	ck_assert_ptr_eq(cns_graph_find(graph, &num), NULL);

	num = -1;
	ck_assert_ptr_eq(cns_graph_find(graph, &num), NULL);
}
END_TEST

START_TEST(test_graph_link)
{
	int i;

	for (i = 0; i < data_len; i++)
		cns_graph_add(graph, &data[i]);
	cns_graph_link(graph, &data[0], &data[1]);
	cns_graph_link(graph, &data[0], &data[2]);
	cns_graph_link(graph, &data[0], &data[3]);
	cns_graph_link(graph, &data[3], &data[4]);
	cns_graph_link(graph, &data[2], &data[3]);

	ck_assert_int_eq(cns_graph_find(graph, &data[0])->outdegree, 3);
	ck_assert_int_eq(cns_graph_find(graph, &data[0])->indegree, 0);
	ck_assert_int_eq(cns_graph_find(graph, &data[1])->outdegree, 0);
	ck_assert_int_eq(cns_graph_find(graph, &data[1])->indegree, 1);
	ck_assert_int_eq(cns_graph_find(graph, &data[2])->outdegree, 1);
	ck_assert_int_eq(cns_graph_find(graph, &data[2])->indegree, 1);
	ck_assert_int_eq(cns_graph_find(graph, &data[3])->outdegree, 1);
	ck_assert_int_eq(cns_graph_find(graph, &data[3])->indegree, 2);
	ck_assert_int_eq(cns_graph_find(graph, &data[4])->outdegree, 0);
	ck_assert_int_eq(cns_graph_find(graph, &data[4])->indegree, 1);

	ck_assert_ptr_eq(cns_graph_find(graph, &data[0])->adj_nodes->data, cns_graph_find(graph, &data[1]));
	ck_assert_ptr_eq(cns_graph_find(graph, &data[0])->adj_nodes->next->data, cns_graph_find(graph, &data[2]));
	ck_assert_ptr_eq(cns_graph_find(graph, &data[0])->adj_nodes->next->next->data, cns_graph_find(graph, &data[3]));
	ck_assert_ptr_eq(cns_graph_find(graph, &data[3])->adj_nodes->data, cns_graph_find(graph, &data[4]));
	ck_assert_ptr_eq(cns_graph_find(graph, &data[2])->adj_nodes->data, cns_graph_find(graph, &data[3]));
}
END_TEST

Suite *make_graph_suite(void)
{
	Suite *s;
	s = suite_create("graph");

	TCase *tc_graph;
	tc_graph = tcase_create("tc_graph");
	tcase_add_checked_fixture(tc_graph, setup, teardown);
	tcase_add_test(tc_graph, test_graph_node_create);
	tcase_add_test(tc_graph, test_graph_create);
	tcase_add_test(tc_graph, test_graph_add);
	tcase_add_test(tc_graph, test_graph_find);
	tcase_add_test(tc_graph, test_graph_link);
	suite_add_tcase(s, tc_graph);

	return s;
}
