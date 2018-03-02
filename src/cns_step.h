#ifndef _CNS_STEP_H_
#define _CNS_STEP_H_

#include "cns_list.h"
#include "cns_block.h"

typedef struct cns_step cns_step;
struct cns_step {
	cns_block    *block;
	cns_list     *ens;
	cns_list     *run_list;
	int           run_times;
	cns_block_op  op_pre;
	void         *data_pre;
	cns_block_op  op_post;
	void         *data_post;
};

#ifdef __cplusplus
extern "C" {
#endif

	cns_step *cns_step_create(cns_block *block, cns_list *ens, cns_block_op op_pre,
				void *data_pre, cns_block_op op_post, void *data_post);
	void cns_step_free(cns_step *step);

#ifdef __cplusplus
}
#endif

#endif	/* _CNS_STEP_H_ */
