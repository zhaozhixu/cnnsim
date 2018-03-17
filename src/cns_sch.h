#ifndef _CNS_SCH_H_
#define _CNS_SCH_H_

#include "cns_list.h"
#include "cns_step.h"

typedef struct cns_sch cns_sch;
struct cns_sch {
	int       len;
	cns_list *steps;
};

#ifdef __cplusplus
extern "C" {
#endif

	cns_sch *cns_sch_create(void);
	void cns_sch_free(cns_sch *sch);
	void cns_sch_add_step(cns_sch *sch, cns_step *step);
	void cns_sch_run(cns_sch *sch);

#ifdef __cplusplus
}
#endif

#endif	/* _CNS_SCH_H_ */
