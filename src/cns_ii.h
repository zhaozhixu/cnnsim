#ifndef _CNS_II_H_
#define _CNS_II_H_

#include "cns_util.h"
#include "cns_list.h"

/* index and interface of a cell */
typedef struct cns_ii cns_ii;
struct cns_ii {
	size_t idx;		/* index */
	int    itft;		/* interface type */
};

#ifdef __cplusplus
extern "C" {
#endif

	int cns_ii_cmp(void *data1, void *data2);
	cns_ii *cns_ii_create(size_t idx, int itft);
	void cns_ii_free(cns_ii *ii);
	cns_list *cns_iis_append(cns_list *iis, size_t idx, int itft);
	cns_list *cns_iis_copy(cns_list *iis);

#ifdef __cplusplus
}
#endif

#endif	/* _CNS_JOINT_H_ */
