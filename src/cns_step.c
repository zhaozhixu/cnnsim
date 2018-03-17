#include <string.h>
#include "cns_util.h"
#include "cns_step.h"
#include "cns_graph.h"

cns_step *cns_step_create(cns_block *block, cns_list *ens)
{
	cns_step *step;
	cns_graph *dep_graph;
	cns_list *l;

	step = (cns_step *)cns_alloc(sizeof(cns_step));
	step->block = block;
	step->ens = cns_list_copy_size_t(ens);

	for (l = ens; l; l = l->next)
		cns_block_set_en(block, (size_t)l->data, CNS_TRUE);
	dep_graph = cns_block_dep_graph(block);
	step->run_rounds = cns_graph_topsort(dep_graph, &step->run_list);
	if (step->run_rounds < 0)
		cns_err_quit("ERROR: cns_step_create: dependency graph has a cycle\n");

	for (l = ens; l; l = l->next)
		cns_block_set_en(block, (size_t)l->data, CNS_FALSE);
	cns_graph_free(dep_graph);

	return step;
}

void cns_step_free(cns_step *step)
{
	cns_list_free(step->ens);
	cns_graph_free_topsortlist(step->run_list);
	cns_free(step);
}

void cns_step_run(cns_step *step)
{
	cns_step_fill(step);
	cns_block_run(step->block, step->run_list);
	cns_step_dump(step);
}

cns_step_buf *cns_step_buf_create(size_t input_len, size_t weight_len,
				size_t chore_len, size_t output_len,
				cns_dtype dtype)
{
	cns_step_buf *buf;

	buf = (cns_step_buf *)cns_alloc(sizeof(cns_step_buf));
	buf->dtype = dtype;
	buf->input_len = input_len;
	buf->weight_len = weight_len;
	buf->chore_len = chore_len;
	buf->output_len = output_len;
	buf->input = cns_alloc(cns_size_of(dtype)*input_len);
	buf->weight = cns_alloc(cns_size_of(dtype)*weight_len);
	buf->chore = cns_alloc(cns_size_of(dtype)*chore_len);
	buf->output = cns_alloc(cns_size_of(dtype)*output_len);

	return buf;
}

void cns_step_buf_free(cns_step_buf *buf)
{
	cns_free(buf->input);
	cns_free(buf->weight);
	cns_free(buf->chore);
	cns_free(buf->output);
	cns_free(buf);
}

void cns_step_fill(cns_step *step)
{
	size_t dsize;

	dsize = cns_size_of(step->buf->dtype);
	memmove(step->block->rbuf_i->buf, step->buf->input,
		dsize*step->buf->input_len);
	memmove(step->block->rbuf_w->buf, step->buf->weight,
		dsize*step->buf->weight_len);
	memmove(step->block->rbuf_c->buf, step->buf->chore,
		dsize*step->buf->chore_len);
}

void cns_step_dump(cns_step *step)
{
	size_t dsize;

	dsize = cns_size_of(step->buf->dtype);
	memmove(step->buf->output, step->block->rbuf_o->buf,
		dsize*step->buf->output_len);
}
