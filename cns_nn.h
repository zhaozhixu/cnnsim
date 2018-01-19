#ifndef _CNS_NN_H_
#define _CNS_NN_H_

#include "cns_cell.h"

void cns_nn_conv2d(void *input, void *output, void *weight,
		cns_cell *cells, uint32_t size);

#endif	/* _CNS_NN_H_ */
