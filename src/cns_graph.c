#include "cns_graph.h"
#include "cns_queue.h"
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

	gn = (cns_graph_node *)cns_alloc(sizeof(cns_graph_node));
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
	cns_list *l;

	for (l = graph->nodes; l; l = l->next)
		cns_graph_node_free(l->data); /* free every node */
	cns_list_free(graph->nodes);	      /* free node list */
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
	cns_graph_node n_data;
	cns_list *l_res;

	n_data.data = data;
	l_res = cns_list_find_custom(graph->nodes, &n_data, default_cmp);
	return l_res ? l_res->data : NULL;
}

void cns_graph_link(cns_graph *graph, void *data1, void *data2)
{
	cns_graph_node *node1;
	cns_graph_node *node2;

	node1 = cns_graph_find(graph, data1);
	node2 = cns_graph_find(graph, data2);
	if (!node1 || !node2)
		return;

	node1->adj_nodes = cns_list_append(node1->adj_nodes, node2);
	node1->outdegree++;
	node2->indegree++;
}

void cns_graph_unlink(cns_graph *graph, void *data1, void *data2)
{
	cns_graph_node *node1;
	cns_graph_node *node2;

	node1 = cns_graph_find(graph, data1);
	node2 = cns_graph_find(graph, data2);
	if (!node1 || !node2)
		return;

	if (!cns_list_find(node1->adj_nodes, node2))
		return;

	node1->adj_nodes = cns_list_remove(node1->adj_nodes, node2);
	node1->outdegree--;
	node2->indegree--;
}

/* the pointer pointed to by res should be NULL */
int cns_graph_topsort(cns_graph *graph, cns_list **res)
{
	cns_queue *queue;
	cns_list *nodes;
	cns_list *res_list;
	cns_list *sub_list;
	cns_graph_node *node;
	int node_count;
	int queue_size;

	node_count = 0;
	res_list = *res;
	sub_list = NULL;
	queue = cns_queue_create();
	for (nodes = graph->nodes; nodes; nodes = nodes->next) {
		node = nodes->data;
		if (node->indegree == 0)
			cns_queue_enqueue(queue, node);
	}

	while (queue->size != 0) {
		queue_size = queue->size;
		while (queue_size-- != 0) {
			node = cns_queue_dequeue(queue);
			sub_list = cns_list_append(sub_list, node);
			for (nodes = node->adj_nodes; nodes; nodes = nodes->next) {
				node = nodes->data;
				if (node->indegree == 1)
					/* TODO: unlink */
					cns_queue_enqueue(queue, node);
			}
		}
		res_list = cns_list_append(res_list, sub_list);
	}

	cns_queue_free(queue);
	return 0;
}

void cns_graph_free_topsortlist(cns_list *list)
{
	cns_list *l;

	for (l = list; l; l = l->next)
		cns_list_free(l->data);
	cns_list_free(list);
}
