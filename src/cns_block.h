#ifndef _CNS_BLOCK_H_
#define _CNS_BLOCK_H_

#include "cns_util.h"
#include "cns_cell.h"
#include "cns_graph.h"

typedef struct cns_block cns_block;
struct cns_block {
	cns_dtype       buf_dtype;
	size_t          buf_size;
	size_t          size;
	void           *buf;
	cns_cell       *cells;
};

#ifdef __cplusplus
extern "C" {
#endif

	cns_block *cns_block_create(size_t size);
	void cns_block_free(cns_block *block);
	void cns_block_run(cns_block *block);
	void cns_block_set_data(cns_block *block, size_t index,
				void *input, void *weight, void *output);
	void cns_block_set_width(cns_block *block, size_t index, uint8_t width);
	void cns_block_set_dtype(cns_block *block, size_t index, int dtype);
	void cns_block_set_op(cns_block *block, size_t index, cns_cell_op op);
	void *cns_block_alloc_buf(cns_block *block, size_t n, cns_dtype dtype);
	void cns_block_add_dep(cns_block *block, size_t index, ssize_t dep);
	cns_graph *cns_block_dep_graph(cns_block *block);

#ifdef __cplusplus
}
#endif

#endif	/* _CNS_BLOCK_H_ */
