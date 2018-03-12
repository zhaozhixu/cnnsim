#include "cns_wire.h"

cns_wire_buf *cns_wire_buf_create(size_t len, cns_dtype dtype)
{
	cns_wire_buf *wire_buf;
	size_t size, i;

	wire_buf = (cns_wire_buf *)cns_alloc(sizeof(cns_wire_buf));
	size = cns_size_of(dtype) * len;
	wire_buf->buf = cns_alloc(size);
	memset(wire_buf->buf, 0, size);
	wire_buf->iis = (cns_list **)cns_alloc(sizeof(cns_list *) * len);
	for (i = 0; i < len; i++)
		wire_buf->iis[i] = NULL;
	wire_buf->dtype = dtype;
	wire_buf->len = len;
	wire_buf->head = 0;

	return wire_buf;
}

void cns_wire_buf_free(cns_wire_buf *wire_buf)
{
	cns_list *l;
	size_t i;

	for (i = 0; i < wire_buf->len; i++)
		cns_list_free_deep(wire_buf->iis[i]);
	cns_free(wire_buf->iis);
	cns_free(wire_buf->buf);
	cns_free(wire_buf);
}

int cns_wire_buf_index(cns_wire_buf *buf, void *addr)
{
	return cns_pointer_sub(addr, buf->buf, buf->dtype);
}

void *cns_wire_buf_addr(cns_wire_buf *buf, int b_idx)
{
	return cns_pointer_add(buf->buf, b_idx, buf->dtype);
}

void *cns_wire_buf_link(cns_wire_buf *buf, size_t b_idx, size_t idx, int itft)
{
	cns_ii *ii;

	if (b_idx > buf->len-1)
		cns_err_quit("ERROR: cns_wire_buf_link: buffer array out of bounds");
	ii = cns_ii_create(idx, itft);
	buf->iis[b_idx] = cns_list_append(buf->iis[b_idx], ii);

	return cns_pointer_add(buf->buf, b_idx, buf->dtype);
}

void cns_wire_buf_unlink(cns_wire_buf *buf, size_t b_idx, size_t idx, int itft)
{
	cns_ii *ii;
	int found_idx;

	if (b_idx > buf->len-1)
		cns_err_quit("ERROR: cns_wire_buf_unlink: buffer array out of bounds");
	ii = cns_ii_create(idx, itft);
	found_idx = cns_list_index_custom(buf->iis[b_idx], ii, ii_cmp);
	cns_free(ii);
	if (found_idx < 0)
		return;
	buf->iis[b_idx] = cns_list_remove_nth(buf->iis[b_idx], found_idx);
}

void *cns_wire_buf_append(cns_wire_buf *buf, size_t idx, int itft)
{
	cns_ii *ii;

	if (buf->head >= buf->len) {
		fprintf(stderr, "ERROR: cns_wire_append: buffer overflow\n");
		exit(EXIT_FAILURE);
	}

	ii = cns_ii_create(idx, itft);
	buf->iis[buf->head] = cns_list_append(buf->iis[buf->head], ii);
	buf->head++;

	return cns_pointer_add(buf->buf, buf->head-1, buf->dtype);
}
