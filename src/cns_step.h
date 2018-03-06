#ifndef _CNS_STEP_H_
#define _CNS_STEP_H_

#include "cns_list.h"
#include "cns_block.h"

typedef struct cns_step_iwbuf cns_step_iwbuf;
struct cns_step_iwbuf {
	cns_dtype dtype;
	size_t    input_len;
	size_t    weight_len;
	void     *input;
	void     *weight;
};

typedef struct cns_step_obuf cns_step_obuf;
struct cns_step_obuf {
	cns_dtype dtype;
	size_t    output_len;
	void     *output;
};

typedef struct cns_step cns_step;
struct cns_step {
	cns_block    *block;
	cns_list     *ens;
	cns_list     *run_list;
	int           run_rounds;
	cns_block_op  op_pre;
	void         *data_pre;
	cns_block_op  op_post;
	void         *data_post;
};

#ifdef __cplusplus
extern "C" {
#endif

	cns_step *cns_step_create(cns_block *block, cns_list *ens,
				cns_block_op op_pre, void *data_pre,
				cns_block_op op_post, void *data_post);
	void cns_step_free(cns_step *step);
	void cns_step_run(cns_step *step);
	cns_step_iwbuf *cns_step_iwbuf_create(size_t input_len,
					size_t weight_len, cns_dtype dtype);
	void cns_step_iwbuf_free(cns_step_iwbuf *iwbuf);
	cns_step_obuf *cns_step_obuf_create(size_t output_len, cns_dtype dtype);
	void cns_step_obuf_free(cns_step_obuf *obuf);
	void cns_step_cpy_iw(cns_block *block, void *data);
	void cns_step_cpy_o(cns_block *block, void *data);

#ifdef __cplusplus
}
#endif

#endif	/* _CNS_STEP_H_ */
