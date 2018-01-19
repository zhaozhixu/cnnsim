#ifndef _CNS_TENSOR_H_
#define _CNS_TENSOR_H_

#include <stdint.h>

#include "cns_cell.h"

#define CNS_MAXDIM 8

typedef struct cns_tensor {
	uint32_t  ndim;
	uint32_t  *dims;
	uint32_t  len;
	void      *data;
} cns_tensor;


#endif	/* _CNS_TENSOR_H_ */
