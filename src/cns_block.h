#ifndef _CNS_BLOCK_H_
#define _CNS_BLOCK_H_

#include "cns_util.h"
#include "cns_cell.h"
#include "cns_buf.h"
#include "cns_graph.h"
#include "cns_list.h"

typedef struct cns_block cns_block;
struct cns_block {
	size_t          length;	/* number of cells */
	cns_buf        *ibuf;	/* input buffer */
	cns_buf        *obuf;	/* output buffer */
	cns_buf        *wbuf;	/* weight buffer */
	cns_buf        *cbuf;	/* chore buffer */
	cns_cell       *cells;	/* array of cells */
};

#ifdef __cplusplus
extern "C" {
#endif

	cns_block *cns_block_create(size_t length, cns_dtype dtype, uint8_t width);
	void cns_block_free(cns_block *block);
	void cns_block_run(cns_block *block);
	void cns_block_set_data(cns_block *block, size_t index,
				void *input, void *weight, void *output);
	void cns_block_set_width(cns_block *block, size_t index, uint8_t width);
	void cns_block_set_dtype(cns_block *block, size_t index, int dtype);
	void cns_block_set_op(cns_block *block, size_t index, cns_cell_op op);
	void cns_block_set_en(cns_block *block, size_t index, cns_bool_t en);
	void cns_block_add_dep(cns_block *block, size_t index, ssize_t dep);
	cns_graph *cns_block_dep_graph(cns_block *block);
	void cns_block_link(cns_block *block, size_t idx1, int itft1,
			size_t idx2, int itft2);
	void cns_block_link_io(cns_block *block, size_t idx, int itft);

#ifdef __cplusplus
}
#endif

#endif	/* _CNS_BLOCK_H_ */
