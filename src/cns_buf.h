#ifndef _CNS_BUF_H_
#define _CNS_BUF_H_

#include "cns_util.h"
#include "cns_sch.h"

struct cns_buf_array {
	size_t    len;
	void    **buf_array;
};

#endif	/* _CNS_BUF_H_ */
