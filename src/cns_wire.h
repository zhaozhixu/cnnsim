#ifndef _CNS_WIRE_H_
#define _CNS_WIRE_H_

#include "cns_util.h"
#include "cns_list.h"
#include "cns_ii.h"

typedef struct cns_wire_buf cns_wire_buf;
struct cns_wire_buf {
	cns_dtype  dtype;
	size_t     len;
	size_t     head;
	cns_list **iis;
	void      *buf;
};

#ifdef __cplusplus
extern "C" {
#endif

	cns_wire_buf *cns_wire_buf_create(size_t len, cns_dtype dtype);
	void cns_wire_buf_free(cns_wire_buf *wire_buf);
	int cns_wire_buf_index(cns_wire_buf *buf, void *addr);
	void *cns_wire_buf_addr(cns_wire_buf *buf, int b_idx);
	void *cns_wire_buf_link(cns_wire_buf *buf, size_t b_idx, size_t idx, int itft);
	void cns_wire_buf_unlink(cns_wire_buf *buf, size_t b_idx, size_t idx, int itft);
	void *cns_wire_buf_append(cns_wire_buf *buf, size_t idx, int itft);

#ifdef __cplusplus
}
#endif

#endif	/* _CNS_WIRE_H_ */
