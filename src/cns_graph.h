#ifndef _CNS_GRAPH_H_
#define _CNS_GRAPH_H_

#include <stdio.h>
#include <stdlib.h>
#include "cns_list.h"
#include "cns_util.h"

typedef struct cns_graph_node cns_graph_node;
struct cns_graph_node {
	size_t       indegree;
	size_t       outdegree;
	void        *data;
	cns_list    *adj_nodes;	/* data type cns_graph_node */
};

typedef struct cns_graph cns_graph;
struct cns_graph {
	size_t       size;
	cns_list    *nodes;	/* data type cns_graph_node */
};

#ifdef __cplusplus
extern "C" {
#endif

	cns_graph_node *cns_graph_node_create(void *data);
	cns_graph *cns_graph_create(void);
	void cns_graph_node_free(cns_graph_node *node);
	void cns_graph_free(cns_graph *graph);
	cns_graph_node *cns_graph_add(cns_graph *graph, void *data);
	cns_graph_node *cns_graph_find(cns_graph *graph, void *data);
	void cns_graph_link(cns_graph *graph, void *data1, void *data2);
	void cns_graph_unlink(cns_graph *graph, void *data1, void *data2);
	cns_graph *cns_graph_copy(cns_graph *graph);
	int cns_graph_num_outlier(cns_graph *graph);
	void cns_graph_free_topsortlist(cns_list *list);
	int cns_graph_topsort(cns_graph *graph, cns_list **res);
	void cns_graph_fprint(FILE *fp, cns_graph *graph, cns_fprint_func print_func);

#ifdef __cplusplus
}
#endif

#endif	/* _CNS_GRAPH_H_ */
