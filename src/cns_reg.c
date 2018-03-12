#include <string.h>
#include "cns_reg.h"

cns_reg *cns_reg_create(size_t ori, cns_list *iis)
{
	cns_reg *reg;

	reg = (cns_reg *)cns_alloc(sizeof(cns_reg));
	reg->ori = ori;
	reg->iis = cns_iis_copy(iis);
	reg->len = cns_list_length(iis);

	return reg;
}

void cns_reg_free(cns_reg *reg)
{
	cns_list_free_deep(reg->iis);
	cns_free(reg);
}

cns_reg_buf *cns_reg_buf_create(size_t len, cns_dtype dtype)
{
	cns_reg_buf *buf;
	size_t size;

	buf = (cns_reg_buf *)cns_alloc(sizeof(cns_reg_buf));
	size = cns_size_of(dtype) * len;
	buf->buf = cns_alloc(size);
	memset(buf->buf, 0, size);
	buf->regs = NULL;
	buf->dtype = dtype;
	buf->len = len;
	buf->head = 0;

	return buf;
}

void cns_reg_buf_free(cns_reg_buf *buf)
{
	cns_list_free_deep(buf->regs);
	cns_free(buf->buf);
	cns_free(buf);
}

void *cns_reg_buf_link(cns_reg_buf *buf, size_t ori, cns_list *iis)
{
	cns_reg *reg;

	reg = cns_reg_create(ori, iis);
	buf->regs = cns_list_append(buf->regs, reg);
	if (buf->head < ori + reg->len)
		buf->head = ori + reg->len;
	return cns_pointer_add(buf->buf, ori, buf->dtype);
}
