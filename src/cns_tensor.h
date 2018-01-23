#ifndef _CNS_TENSOR_H_
#define _CNS_TENSOR_H_

#include <stdint.h>

#include "cns_cell.h"
#include "cns_util.h"

#define CNS_MAXDIM 8

typedef struct cns_tensor {
	uint32_t  ndim;
	uint32_t  *dims;
	uint32_t  len;
	cns_dtype dtype;
	void      *data;
} cns_tensor;

int cns_tensor_is_valid(const cns_tensor *tensor);
int cns_tensor_is_same_shape(const cns_tensor *t1, const cns_tensor *t2);
cns_tensor *cns_tensor_create(uint32_t ndim, uint32_t *dims, void *data, cns_dtype dtype);
cns_tensor *cns_tensor_alloc(uint32_t ndim, uint32_t *dims, cns_dtype dtype);
void cns_tensor_free(cns_tensor *t, cns_bool_t do_free_data);
void cns_tensor_fprint(FILE *stream, const cns_tensor *tensor, const char *fmt);
void cns_tensor_print(const cns_tensor *tensor, const char *fmt);
void cns_tensor_save(const char *filename, const cns_tensor *tensor, const char *fmt);

#endif	/* _CNS_TENSOR_H_ */
