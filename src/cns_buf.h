#ifndef _CNS_BUF_H_
#define _CNS_BUF_H_

#include "cns_util.h"
#include "cns_list.h"

/* index and interface attached to a buf element */
typedef struct cns_buf_ii cns_buf_ii;
struct cns_buf_ii {
	size_t idx;		/* index */
	int    itft;		/* interface type */
};

typedef struct cns_buf cns_buf;
struct cns_buf {
	cns_dtype   dtype;
	size_t      length;	/* number of buf elements */
	size_t      head;	/* index of the first unattached buf element */

	/* Indexes and interfaces attached to each buf element.
	   This is an array of cns_list* of cns_buf_ii*. */
	cns_list  **iis;
	void       *buf;
};

#ifdef __cplusplus
extern "C" {
#endif

	cns_buf_ii *cns_buf_ii_create(size_t idx, int itft);
	void cns_buf_ii_free(cns_buf_ii *ii);
	cns_buf *cns_buf_create(size_t length, cns_dtype dtype);
	void cns_buf_free(cns_buf *buf);
	int cns_buf_index(cns_buf *buf, void *addr);
	void *cns_buf_addr(cns_buf *buf, int buf_idx);
	void *cns_buf_attach(cns_buf *buf, size_t buf_idx, size_t idx, int itft);
	void cns_buf_detach(cns_buf *buf, size_t buf_idx, size_t idx, int itft);
	void *cns_buf_append(cns_buf *buf, size_t idx, int itft);
	int cns_buf_seek(cns_buf *buf, size_t pos);

#ifdef __cplusplus
}
#endif


#endif	/* _CNS_BUF_H_ */
