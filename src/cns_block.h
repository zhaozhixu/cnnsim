#ifndef _CNS_BLOCK_H_
#define _CNS_BLOCK_H_

#include "cns_util.h"
#include "cns_cell.h"
#include "cns_reg.h"
#include "cns_wire.h"
#include "cns_graph.h"
#include "cns_list.h"
#include "cns_ii.h"

typedef struct cns_block cns_block;
struct cns_block {
	uint8_t         width;	/* data width (for cns_block_expand) */
	cns_dtype       dtype;	/* data type (for cns_block_expand) */
	size_t          len;	/* number of cells */
	cns_cell       *cells;	/* array of cells */
	cns_reg_buf    *rbuf_i;	/* input buffer */
	cns_reg_buf    *rbuf_o;	/* output buffer */
	cns_reg_buf    *rbuf_w;	/* weight buffer */
	cns_reg_buf    *rbuf_c;	/* chore buffer */
	cns_wire_buf   *wbuf_c;	/* chore buffer of wire */
};

/* typedef void (* cns_block_op) (cns_block *block, void *data); */

#ifdef __cplusplus
extern "C" {
#endif

	cns_block *cns_block_create(size_t len, cns_dtype dtype, uint8_t width);
	void cns_block_free(cns_block *block);
	void cns_block_run(cns_block *block, cns_list *run_list);
	void cns_block_set_data(cns_block *block, size_t index,
				void *input, void *weight, void *output);
	void cns_block_set_width(cns_block *block, size_t index, uint8_t width);
	void cns_block_set_dtype(cns_block *block, size_t index, int dtype);
	void cns_block_set_op(cns_block *block, size_t index, cns_cell_op op);
	void cns_block_set_en(cns_block *block, size_t index, cns_bool_t en);
	void cns_block_add_dep(cns_block *block, size_t index, ssize_t dep);
	cns_graph *cns_block_dep_graph(cns_block *block);
	void **cns_block_find_itfp(cns_block *block, size_t idx, int itft);
	void cns_block_link(cns_block *block, size_t idx1, int itft1,
			size_t idx2, int itft2);
	void cns_block_link_io(cns_block *block, size_t ori, cns_list *iis,
			int itft);
	void cns_block_link_c(cns_block *block, size_t ori, cns_list *iis);
	cns_block *cns_block_expand(cns_block *block, int multiple, int extra);
	size_t cns_block_size(cns_block *block);
	void cns_block_fill(cns_block *block, int itft, void *src, size_t n);
	void cns_block_dump(cns_block *block, int itft, void *dst, size_t n);
	cns_list *cns_block_en_expand(cns_block *block, cns_list *ens, int base,
				int multiple, cns_list *extras);

#ifdef __cplusplus
}
#endif

#endif	/* _CNS_BLOCK_H_ */
