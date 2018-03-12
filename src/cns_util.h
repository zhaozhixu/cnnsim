#ifndef _CNS_UTIL_H_
#define _CNS_UTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef enum cns_bool_t cns_bool_t;
enum cns_bool_t {
	CNS_FALSE = 0,
	CNS_TRUE = 1
};

typedef enum cns_dtype cns_dtype;
enum cns_dtype {
	CNS_BOOL,
	CNS_FLOAT,
	CNS_INT32,
	CNS_INT16,
	CNS_INT8,
	CNS_UINT32,
	CNS_UINT16,
	CNS_UINT8
};

enum cns_interface_type {
	CNS_INPUT,
	CNS_OUTPUT,
	CNS_WEIGHT
};

typedef int (*cns_cmp_func)(void *, void *);
typedef void (* cns_fprint_func) (FILE *fp, void *data);

#define cns_free free

#define CNS_MAXLINE 4096

#ifdef __cplusplus
extern "C" {
#endif

	void *cns_alloc(size_t size);
	char *cns_path_alloc(size_t *sizep);
	void *cns_clone(const void *src, size_t size);
	void *cns_repeat(void *data, size_t size, int times);
	int cns_compute_length(uint32_t ndim, uint32_t *dims);
	size_t cns_size_of(cns_dtype dtype);
	int cns_pointer_sub(void *p1, void *p2, cns_dtype dtype);
	void *cns_pointer_add(void *p, int offset, cns_dtype dtype);
	void cns_err_msg(const char *fmt, ...);
	void cns_err_cont(int error, const char *fmt, ...);
	void cns_err_ret(const char *fmt, ...);
	void cns_err_quit(const char *fmt, ...) __attribute__((noreturn));
	void cns_err_exit(int error, const char *fmt, ...) __attribute__((noreturn));
	void cns_err_sys(const char *fmt, ...) __attribute__((noreturn));
	void cns_err_dump(const char *fmt, ...) __attribute__((noreturn));

#ifdef __cplusplus
}
#endif

#endif	/* _CNS_UTIL_H_ */
