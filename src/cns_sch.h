#ifndef _CNS_SCH_H_
#define _CNS_SCH_H_

#include "cns_list.h"

struct cns_step {
	cns_list *ens;	  /* indexes of enable cells, data type of size_t */
	cns_list *ops;	  /* operation on block bufs */
};

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif	/* _CNS_SCH_H_ */
