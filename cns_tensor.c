#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "cns_tensor.h"

int cns_tensor_is_valid(const cns_tensor *tensor)
{
	return (tensor && tensor->data &&
		tensor->ndim < CNS_MAXDIM && tensor->ndim > 0 &&
		tensor->len == cns_compute_length(tensor->ndim, tensor->dims));
}

int cns_tensor_is_same_shape(const cns_tensor *t1, const cns_tensor *t2)
{
	assert(cns_tensor_is_valid(t1) && cns_tensor_is_valid(t2));
	if (t1->ndim == t2->ndim) {
		int ndim = t1->ndim;
		while (--ndim >= 0)
			if (t1->dims[ndim] != t2->dims[ndim])
				return 0;
		return 1;
	}
	return 0;
}

/* create tensor with ndim, dims, data and dtype */
cns_tensor *cns_tensor_create(uint32_t ndim, uint32_t *dims, void *data, cns_dtype dtype)
{
	assert(dims);		/* allow data be NULL */
	cns_tensor *t = (cns_tensor *)cns_alloc(sizeof(cns_tensor));
	t->data = data;
	t->dtype = dtype;
	t->ndim = ndim;
	t->dims = (uint32_t *)cns_alloc(sizeof(uint32_t) * ndim);
	memmove(t->dims, dims, sizeof(uint32_t) * ndim);
	t->len = cns_compute_length(ndim, dims);
	return t;
}

/* create tensor width ndim, dims, dtype and allocated space for data */
cns_tensor *cns_tensor_alloc(uint32_t ndim, uint32_t *dims, cns_dtype dtype)
{
	cns_tensor *t = cns_tensor_create(ndim, dims, NULL, dtype);
	void *data;
	switch (dtype) {
	case CNS_BOOL:
		data = (void *)cns_alloc(sizeof(cns_bool_t) * t->len);
		break;
	case CNS_FLOAT:
		data = (void *)cns_alloc(sizeof(float) * t->len);
		break;
	case CNS_INT32:
		data = (void *)cns_alloc(sizeof(int32_t) * t->len);
		break;
	case CNS_INT16:
		data = (void *)cns_alloc(sizeof(int16_t) * t->len);
		break;
	case CNS_INT8:
		data = (void *)cns_alloc(sizeof(int8_t) * t->len);
		break;
	case CNS_UINT32:
		data = (void *)cns_alloc(sizeof(uint32_t) * t->len);
		break;
	case CNS_UINT16:
		data = (void *)cns_alloc(sizeof(uint16_t) * t->len);
		break;
	case CNS_UINT8:
		data = (void *)cns_alloc(sizeof(uint8_t) * t->len);
		break;
	default:
		fprintf(stderr, "ERROR: unknown cns_dtype %d\n", dtype);
		exit(EXIT_FAILURE);
	}
	t->data = data;
	return t;
}

void cns_tensor_free(cns_tensor *t, int do_free_data)
{
	assert(cns_tensor_is_valid(t));
	cns_free(t->dims);
	if (do_free_data) {
		cns_free(t->data);
	}
	cns_free(t);
}

void cns_tensor_fprint(FILE *stream, const cns_tensor *tensor, const char *fmt)
{
	assert(cns_tensor_is_valid(tensor));
	uint32_t dim_sizes[CNS_MAXDIM];	/* dimision size */
	uint32_t dim_levels[CNS_MAXDIM]; /* how deep current number go */
	uint32_t ndim = tensor->ndim; /* pointer shortcuts */
	uint32_t len = tensor->len;
	uint32_t *dims = tensor->dims;
	void *data = tensor->data;
	cns_dtype dtype = tensor->dtype;
	char left_buf[CNS_MAXDIM+1]; /* buffer for brackets */
	char right_buf[CNS_MAXDIM+1];
	char *lp = left_buf;
	char *rp = right_buf;
	size_t right_len;
	int i, j, k;

	dim_sizes[ndim-1] = tensor->dims[ndim-1];
	dim_levels[ndim-1] = 0;
	for (i = ndim-2; i >= 0; i--) {
		dim_sizes[i] = dims[i] * dim_sizes[i+1];
		dim_levels[i] = 0;
	}
	for (i = 0; i < len; i++) {
		for (j = 0; j < ndim; j++) {
			if (i % dim_sizes[j] == 0)
				dim_levels[j]++;
			if (dim_levels[j] == 1) {
				*lp++ = '[';
				dim_levels[j]++;
			}
			if (dim_levels[j] == 3) {
				*rp++ = ']';
				if (j != 0 && dim_levels[j] > dim_levels[j-1]) {
					*lp++ = '[';
					dim_levels[j] = 2;
				} else
					dim_levels[j] = 0;
			}
		}
		*lp = *rp = '\0';
		fprintf(stream, "%s", right_buf);
		if (*right_buf != '\0') {
			fprintf(stream, "\n");
			right_len = strlen(right_buf);
			for (k = ndim-right_len; k > 0; k--)
				fprintf(stream, " ");
		}
		fprintf(stream, "%s", left_buf);
		if (*left_buf == '\0')
			fprintf(stream, " ");
		fprintf(stream, fmt, data[i]);
		lp = left_buf, rp = right_buf;
	}
	for (j = 0; j < ndim; j++)
		fprintf(stream, "]");
	fprintf(stream, "\n");
}
