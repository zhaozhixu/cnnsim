#include "cns_sch.h"

cns_sch *cns_sch_create(void)
{
	cns_sch *sch;

	sch = (cns_sch *)cns_alloc(sizeof(cns_sch));
	sch->len = 0;
	sch->steps = NULL;

	return sch;
}

void cns_sch_free(cns_sch *sch)
{
	cns_list_free(sch->steps);
	cns_free(sch);
}

void cns_sch_add_step(cns_sch *sch, cns_step *step)
{
	cns_list_append(sch->steps, step);
	sch->len++;
}

void cns_sch_run(cns_sch *sch)
{
	cns_list *l;
	cns_step *step;

	for (l = sch->steps; l; l = l->next) {
		step = (cns_step *)l->data;
		cns_step_run(step);
	}
}
