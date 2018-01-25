#include "cns_graph.h"
#include "cns_util.h"

static int default_cmp(void *a, void *b)
{
	cns_graph_node *gna;
	cns_graph_node *gnb;

	gna = (cns_graph_node *)a;
	gnb = (cns_graph_node *)b;
	return gna->data - gnb->data;
}

cns_graph_node *cns_graph_node_create(void *data)
{
	cns_graph_node *gn;

	gn = (cns_graph *)cns_alloc(sizeof(cns_graph_node));
	gn->adj_nodes = NULL;
	gn->data = data;
	gn->indegree = 0;
	gn->outdegree = 0;
	return gn;
}

cns_graph *cns_graph_create()
{
	cns_graph *g;

	g = (cns_graph *)cns_alloc(sizeof(cns_graph));
	g->nodes = NULL;
	g->size = 0;
	return g;
}

void cns_graph_node_free(cns_graph_node *node)
{
	cns_list_free(node->adj_nodes);
	cns_free(node);
}

void cns_graph_free(cns_graph *graph)
{
	cns_list_free(graph->nodes);
	cns_free(graph);
}

cns_graph_node *cns_graph_add(cns_graph *graph, void *data)
{
	cns_graph_node *node;

	node = cns_graph_node_create(data);
	graph->nodes = cns_list_append(graph->nodes, node);
	graph->size++;
	return node;
}

cns_graph_node *cns_graph_find(cns_graph *graph, void *data)
{
	cns_graph_node *n;
	cns_graph_node n_data;

	n_data.data = data;
	n = cns_list_find_custom(graph->nodes, &n_data, default_cmp);
	return n;
}

void cns_graph_link(cns_graph *graph, void *data1, void *data2)
{
	cns_graph_node *node1;
	cns_graph_node *node2;

	node1 = cns_graph_find(graph, data1);
	node2 = cns_graph_find(graph, data2);
	if (!node1 || !node2)
		return;

	node1->outdegree++;
	node2->indegree++;
	node1->adj_nodes = cns_list_append(node1->adj_nodes, node2);
}
