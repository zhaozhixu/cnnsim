#include <string.h>
#include "cns_util.h"
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

cns_step_iwbuf *cns_step_iwbuf_create(size_t input_len, size_t weight_len,
				cns_dtype dtype)
{
	cns_step_iwbuf *iwbuf;

	iwbuf = (cns_step_iwbuf *)cns_alloc(sizeof(cns_step_iwbuf));
	iwbuf->dtype = dtype;
	iwbuf->input_len = input_len;
	iwbuf->weight_len = weight_len;
	iwbuf->input = cns_alloc(cns_size_of(dtype)*input_len);
	iwbuf->weight = cns_alloc(cns_size_of(dtype)*weight_len);

	return iwbuf;
}

void cns_step_iwbuf_free(cns_step_iwbuf *iwbuf)
{
	cns_free(iwbuf->input);
	cns_free(iwbuf->weight);
	cns_free(iwbuf);
}

cns_step_obuf *cns_step_obuf_create(size_t output_len, cns_dtype dtype)
{
	cns_step_obuf *obuf;

	obuf = (cns_step_obuf *)cns_alloc(sizeof(cns_step_obuf));
	obuf->dtype = dtype;
	obuf->output_len = output_len;
	obuf->output = cns_alloc(cns_size_of(dtype)*output_len);

	return obuf;
}

void cns_step_obuf_free(cns_step_obuf *obuf)
{
	cns_free(obuf->output);
	cns_free(obuf);
}

void cns_step_cpy_iw(cns_block *block, void *data)
{
	cns_step_iwbuf *iwbuf;

	iwbuf = (cns_step_iwbuf *)data;
	memmove(block->ibuf->buf, iwbuf->input,
		cns_size_of(iwbuf->dtype)*iwbuf->input_len);
	memmove(block->wbuf->buf, iwbuf->weight,
		cns_size_of(iwbuf->dtype)*iwbuf->weight_len);
}

void cns_step_cpy_o(cns_block *block, void *data)
{
	cns_step_obuf *obuf;

	obuf = (cns_step_obuf *)data;
	memmove(obuf->output, block->obuf->buf,
		cns_size_of(obuf->dtype) * obuf->output_len);
}
