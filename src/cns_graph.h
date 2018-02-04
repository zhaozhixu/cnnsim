#ifndef _CNS_GRAPH_H_
#define _CNS_GRAPH_H_

#include <stdlib.h>
#include "cns_list.h"

typedef struct cns_graph_node cns_graph_node;
struct cns_graph_node {
	size_t    indegree;
	size_t    outdegree;
	void     *data;
	cns_list *adj_nodes;
};

typedef struct cns_graph cns_graph;
struct cns_graph {
	size_t       size;
	cns_list    *nodes;
};

#ifdef __cplusplus
extern "C" {
#endif

	cns_graph_node *cns_graph_node_create(void *data);
	cns_graph *cns_graph_create();
	void cns_graph_node_free(cns_graph_node *node);
	void cns_graph_free(cns_graph *graph);
	cns_graph_node *cns_graph_add(cns_graph *graph, void *data);
	cns_graph_node *cns_graph_find(cns_graph *graph, void *data);
	void cns_graph_link(cns_graph *graph, void *data1, void *data2);
	void cns_graph_unlink(cns_graph *graph, void *data1, void *data2);

#ifdef __cplusplus
}
#endif

#endif	/* _CNS_GRAPH_H_ */
