#ifndef _CNS_REG_H_
#define _CNS_REG_H_

#include "cns_util.h"
#include "cns_list.h"
#include "cns_ii.h"

typedef struct cns_reg cns_reg;
struct cns_reg {
	size_t    ori;
	size_t    len;
	cns_list *iis;
};

typedef struct cns_reg_buf cns_reg_buf;
struct cns_reg_buf {
	cns_dtype   dtype;
	size_t      len;
	size_t      head;
	cns_list   *regs;
	void       *buf;
};

#ifdef __cplusplus
extern "C" {
#endif

	cns_reg *cns_reg_create(size_t ori, cns_list *iis);
	void cns_reg_free(cns_reg *reg);
	cns_reg_buf *cns_reg_buf_create(size_t len, cns_dtype dtype);
	void cns_reg_buf_free(cns_reg_buf *buf);
	void *cns_reg_buf_link(cns_reg_buf *buf, size_t ori, cns_list *iis);

#ifdef __cplusplus
}
#endif

#endif	/* _CNS_REG_H_ */
