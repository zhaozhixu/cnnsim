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
		fprintf(stderr, "ERROR: cns_tensor_alloc: unknown cns_dtype %d\n", dtype);
		exit(EXIT_FAILURE);
	}
	t->data = data;
	return t;
}

void cns_tensor_free(cns_tensor *t, cns_bool_t do_free_data)
{
	assert(cns_tensor_is_valid(t));
	cns_free(t->dims);
	if (do_free_data) {
		cns_free(t->data);
	}
	cns_free(t);
}

static void fprint_float(FILE *fp, const char *fmt, void *data)
{
	if (fmt == NULL) {
		fprintf(fp, "%f", *(float *)data);
		return;
	}
	fprintf(fp, fmt, *(float *)data);
}

static void fprint_bool(FILE *fp, const char *fmt, void *data)
{
	if (fmt == NULL) {
		fprintf(fp, "%d", *(cns_bool_t *)data);
		return;
	}
	fprintf(fp, fmt, *(cns_bool_t *)data);
}

static void fprint_int32(FILE *fp, const char *fmt, void *data)
{
	if (fmt == NULL) {
		fprintf(fp, "%d", *(int32_t *)data);
		return;
	}
	fprintf(fp, fmt, *(int32_t *)data);
}

static void fprint_int16(FILE *fp, const char *fmt, void *data)
{
	if (fmt == NULL) {
		fprintf(fp, "%d", *(int16_t *)data);
		return;
	}
	fprintf(fp, fmt, *(int16_t *)data);
}

static void fprint_int8(FILE *fp, const char *fmt, void *data)
{
	if (fmt == NULL) {
		fprintf(fp, "%d", *(int8_t *)data);
		return;
	}
	fprintf(fp, fmt, *(int8_t *)data);
}

static void fprint_uint32(FILE *fp, const char *fmt, void *data)
{
	if (fmt == NULL) {
		fprintf(fp, "%u", *(uint32_t *)data);
		return;
	}
	fprintf(fp, fmt, *(uint32_t *)data);
}

static void fprint_uint16(FILE *fp, const char *fmt, void *data)
{
	if (fmt == NULL) {
		fprintf(fp, "%u", *(uint16_t *)data);
		return;
	}
	fprintf(fp, fmt, *(uint16_t *)data);
}

static void fprint_uint8(FILE *fp, const char *fmt, void *data)
{
	if (fmt == NULL) {
		fprintf(fp, "%u", *(uint8_t *)data);
		return;
	}
	fprintf(fp, fmt, *(uint8_t *)data);
}

void cns_tensor_fprint(FILE *stream, const cns_tensor *tensor, const char *fmt)
{
	assert(cns_tensor_is_valid(tensor));
	uint32_t dim_sizes[CNS_MAXDIM];	/* dimision size */
	uint32_t dim_levels[CNS_MAXDIM]; /* how deep current char goes */
	uint32_t len = tensor->len; /* pointer shortcuts */
	int ndim = (int)tensor->ndim; /* cast to int to perform some test */
	uint32_t *dims = tensor->dims;
	void *data = tensor->data;
	cns_dtype dtype = tensor->dtype;
	char left_buf[CNS_MAXDIM+1]; /* buffer for brackets */
	char right_buf[CNS_MAXDIM+1];
	char *lp = left_buf;
	char *rp = right_buf;
	size_t right_len;
	int step;		/* step for data pointer adding */
	void (* fprint_data) (FILE *, const char *, void *); /* print func for different types */
	int i, j, k;

	switch (tensor->dtype) {
	case CNS_BOOL:
		step = sizeof(cns_bool_t) / sizeof(uint8_t);
		fprint_data = &fprint_bool;
		break;
	case CNS_FLOAT:
		step = sizeof(float) / sizeof(uint8_t);
		fprint_data = &fprint_float;
		break;
	case CNS_INT32:
		step = sizeof(int32_t) / sizeof(uint8_t);
		fprint_data = &fprint_int32;
		break;
	case CNS_INT16:
		step = sizeof(int16_t) / sizeof(uint8_t);
		fprint_data = &fprint_int16;
		break;
	case CNS_INT8:
		step = sizeof(int8_t) / sizeof(uint8_t);
		fprint_data = &fprint_int8;
		break;
	case CNS_UINT32:
		step = sizeof(uint32_t) / sizeof(uint8_t);
		fprint_data = &fprint_uint32;
		break;
	case CNS_UINT16:
		step = sizeof(uint16_t) / sizeof(uint8_t);
		fprint_data = &fprint_uint16;
		break;
	case CNS_UINT8:
		step = sizeof(uint8_t) / sizeof(uint8_t);
		fprint_data = &fprint_uint8;
		break;
	default:
		fprintf(stderr, "ERROR: cns_tensor_fprint: unknown cns_dtype %d\n", dtype);
		exit(EXIT_FAILURE);
	}

	for (i = ndim-1; i >= 0; i--) {
		if (i == ndim-1) {
			dim_sizes[i] = tensor->dims[i];
			dim_levels[i] = 0;
			continue;
		}
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
				} else {
					dim_levels[j] = 0;
				}
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
		(*fprint_data)(stream, fmt, (uint8_t *)data+i*step);
		lp = left_buf, rp = right_buf;
	}
	for (j = 0; j < ndim; j++)
		fprintf(stream, "]");
	fprintf(stream, "\n");
}

void cns_tensor_print(const cns_tensor *tensor, const char *fmt)
{
	cns_tensor_fprint(stdout, tensor, fmt);
}

void cns_tensor_save(const char *filename, const cns_tensor *tensor, const char *fmt)
{
	FILE *fp = fopen(filename, "w");
	cns_tensor_fprint(fp, tensor, fmt);
	fclose(fp);
}
