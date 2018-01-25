#include "test_cnnsim.h"
#include "../src/cns_util.h"
#include "../src/cns_graph.h"

static int *data;
static size_t data_len;
static 	cns_graph *graph;

static void setup(void)
{
	int i;
	data_len = 3;
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
	/* cns_graph_node **n; */
	cns_graph_node *n;
	int i, num;

	/* for (i = 0; i < data_len; i++) { */
	/* 	if (i == 0) { */
	/* 		printf("%p\n", n); */
	/* 		printf("%p\n", graph->nodes); */
	/* 	} */
	/* 	n = cns_graph_add(graph, &data[i]); */
	/* 	if (i == 0) { */
	/* 		printf("%p\n", n); */
	/* 		printf("%p\n", graph->nodes); */
	/* 	} */
	/* 	ck_assert_ptr_eq(cns_graph_find(graph, &data[i]), n); */
	/* } */
	/* n = (cns_graph_node **)cns_alloc(sizeof(cns_graph_node *)*data_len); */
	/* for (i = 0; i < data_len; i++) */
	/* 	n[i] = cns_graph_add(graph, &data[i]); */
	/* for (i = 0; i < data_len; i++) */
	/* 	ck_assert_ptr_eq(cns_graph_find(graph, &data[i]), n[i]); */
	/* cns_free(n); */

	num = 3;
	ck_assert_ptr_eq(cns_graph_find(graph, &num), NULL);

	num = -1;
	ck_assert_ptr_eq(cns_graph_find(graph, &num), NULL);
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
	suite_add_tcase(s, tc_graph);

	return s;
}
