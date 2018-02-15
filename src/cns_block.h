#ifndef _CNS_BLOCK_H_
#define _CNS_BLOCK_H_

#include "cns_util.h"
#include "cns_cell.h"
#include "cns_graph.h"
#include "cns_list.h"

enum cns_interface_type {
	CNS_INPUT,
	CNS_OUTPUT,
	CNS_WEIGHT
};

typedef struct cns_block cns_block;
struct cns_block {
	cns_dtype       buf_dtype;
	size_t          ibuf_size;
	size_t          wbuf_size;
	size_t          obuf_size;
	size_t          cbuf_size;
	size_t          ibuf_mark;
	size_t          obuf_mark;
	size_t          wbuf_mark;
	size_t          cbuf_mark;
	size_t          size;
	void           *ibuf;	/* input buffer */
	void           *obuf;	/* output buffer */
	void           *wbuf;	/* weight buffer */
	void           *cbuf;	/* chore buffer */

	/* Interfaces attached to each chore buffer element.
	   This is an *array* of cns_list*. */
	cns_list      **cbuf_itfs;
	cns_cell       *cells;
};

#ifdef __cplusplus
extern "C" {
#endif

	cns_block *cns_block_create(size_t size, cns_dtype dtype, uint8_t width);
	void cns_block_free(cns_block *block);
	void cns_block_run(cns_block *block);
	void cns_block_set_data(cns_block *block, size_t index,
				void *input, void *weight, void *output);
	void cns_block_set_width(cns_block *block, size_t index, uint8_t width);
	void cns_block_set_dtype(cns_block *block, size_t index, int dtype);
	void cns_block_set_op(cns_block *block, size_t index, cns_cell_op op);
	void cns_block_add_dep(cns_block *block, size_t index, ssize_t dep);
	cns_graph *cns_block_dep_graph(cns_block *block);
	void cns_block_link(cns_block *block, size_t idx1, int itft1,
			size_t idx2, int itft2);
	void cns_block_link_io(cns_block *block, size_t idx, int itft);

#ifdef __cplusplus
}
#endif

#endif	/* _CNS_BLOCK_H_ */
