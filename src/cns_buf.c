#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "cns_buf.h"

static int ii_cmp(void *data1, void *data2)
{
	cns_buf_ii *ii1, *ii2;

	ii1 = (cns_buf_ii *)data1;
	ii2 = (cns_buf_ii *)data2;
	if (ii1->idx == ii2->idx && ii1->itft == ii2->itft)
		return 0;
	return -1;
}

cns_buf *cns_buf_create(size_t length, cns_dtype dtype)
{
	cns_buf *buf;
	size_t size, i;

	buf = (cns_buf *)cns_alloc(sizeof(cns_buf));
	size = cns_size_of(dtype) * length;
	buf->buf = cns_alloc(size);
	memset(buf->buf, 0, size);
	buf->iis = (cns_list **)cns_alloc(sizeof(cns_list *) * length);
	for (i = 0; i < length; i++)
		buf->iis[i] = NULL;
	buf->dtype = dtype;
	buf->length = length;
	buf->head = 0;

	return buf;
}

void cns_buf_free(cns_buf *buf)
{
	size_t i;

	for (i = 0; i < buf->length; i++)
		cns_list_free_deep(buf->iis[i]);
	cns_free(buf->buf);
	cns_free(buf);
}

int cns_buf_index(cns_buf *buf, void *addr)
{
	return cns_pointer_sub(addr, buf->buf, buf->dtype);
}

void *cns_buf_addr(cns_buf *buf, int buf_idx)
{
	return cns_pointer_add(buf->buf, buf_idx, buf->dtype);
}

void *cns_buf_attach(cns_buf *buf, size_t buf_idx, size_t idx, int itft)
{
	assert(buf_idx <= buf->tail);
	cns_buf_ii *ii;

	ii = (cns_buf_ii *)cns_alloc(sizeof(cns_buf_ii));
	ii->idx = idx;
	ii->itft = itft;
	buf->iis[buf_idx] = cns_list_append(buf->iis[buf_idx], ii);

	return cns_pointer_add(buf->buf, buf_idx, buf->dtype);
}

void cns_buf_detach(cns_buf *buf, size_t buf_idx, size_t idx, int itft)
{
	assert(buf_idx <= buf->tail);
	cns_buf_ii *ii;
	int found_idx;

	ii = (cns_buf_ii *)cns_alloc(sizeof(cns_buf_ii));
	ii->idx = idx;
	ii->itft = itft;
	found_idx = cns_list_index_custom(buf->iis[buf_idx], ii, ii_cmp);
	cns_free(ii);
	if (found_idx < 0)
		return;
	buf->iis[buf_idx] = cns_list_remove_nth(buf->iis[buf_idx], found_idx);
}

void *cns_buf_append(cns_buf *buf, size_t idx, int itft)
{
	cns_buf_ii *ii;

	if (buf->head >= buf->length) {
		fprintf(stderr, "ERROR: cns_buf_append: buffer overflow\n");
		exit(EXIT_FAILURE);
	}

	ii = (cns_buf_ii *)cns_alloc(sizeof(cns_buf_ii));
	ii->idx = idx;
	ii->itft = itft;
	buf->iis[buf->head] = cns_list_append(buf->iis[buf->head], ii);
	buf->head++;

	return cns_pointer_add(buf->buf, buf->head-1, buf->dtype);
}

void cns_buf_rewind(cns_buf *buf)
{
	buf->head = 0;
}
