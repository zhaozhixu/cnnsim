#include "cns_step.h"
#include "cns_graph.h"

cns_step *cns_step_create(cns_block *block, cns_list *ens, cns_block_op op_pre,
			void *data_pre, cns_block_op op_post, void *data_post)
{
	cns_step *step;
	cns_graph *dep_graph;
	cns_list *l;

	step = (cns_step *)cns_alloc(sizeof(cns_step));
	step->block = block;
	step->ens = ens;
	step->op_pre = op_pre;
	step->data_pre = data_pre;
	step->op_post = op_post;
	step->data_post = data_post;

	for (l = ens; l; l = l->next)
		cns_block_set_en(block, (size_t)l->data, CNS_TRUE);
	dep_graph = cns_block_dep_graph(block);
	step->run_rounds = cns_graph_topsort(dep_graph, &step->run_list);
	if (step->run_rounds < 0) {
		fprintf(stderr,
			"ERROR: cns_step_create: dependency graph has a cycle\n");
		exit(EXIT_FAILURE);
	}

	for (l = ens; l; l = l->next)
		cns_block_set_en(block, (size_t)l->data, CNS_FALSE);
	cns_graph_free(dep_graph);

	return step;
}

void cns_step_free(cns_step *step)
{
	cns_graph_free_topsortlist(step->run_list);
	cns_free(step);
}

void cns_step_run(cns_step *step)
{
	step->op_pre(step->block, step->data_pre);
	cns_block_run(step->block, step->run_list);
	step->op_post(step->block, step->data_post);
}
