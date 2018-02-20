#ifndef _CNS_BUF_H_
#define _CNS_BUF_H_

#include "cns_util.h"
#include "cns_list.h"

typedef struct cns_buf cns_buf;
struct cns_buf {
	cns_dtype   dtype;
	size_t      length;
	size_t      tail;
	cns_list  **itfs;
	void       *buf;
};

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif


#endif	/* _CNS_BUF_H_ */
