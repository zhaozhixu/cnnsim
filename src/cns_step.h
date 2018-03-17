#ifndef _CNS_STEP_H_
#define _CNS_STEP_H_

#include "cns_list.h"
#include "cns_block.h"

typedef struct cns_step_buf cns_step_buf;
struct cns_step_buf {
	cns_dtype dtype;
	size_t    input_len;
	size_t    weight_len;
	size_t    chore_len;
	size_t    output_len;
	void     *input;
	void     *weight;
	void     *chore;
	void     *output;
};

typedef struct cns_step cns_step;
struct cns_step {
	cns_block    *block;
	cns_list     *ens;	/* type size_t */
	cns_list     *run_list;	/* type cns_list* of type ssize_t */
	int           run_rounds;
	cns_step_buf *buf;
};

#ifdef __cplusplus
extern "C" {
#endif

	cns_step *cns_step_create(cns_block *block, cns_list *ens);
	void cns_step_free(cns_step *step);
	void cns_step_run(cns_step *step);
	cns_step_buf *cns_step_buf_create(size_t input_len, size_t weight_len,
					size_t chore_len, size_t output_len,
					cns_dtype dtype);
	void cns_step_buf_free(cns_step_buf *buf);
	void cns_step_fill(cns_step *step);
	void cns_step_dump(cns_step *step);

#ifdef __cplusplus
}
#endif

#endif	/* _CNS_STEP_H_ */
