#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <err.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "cns_util.h"

#ifdef PATH_MAX
static long pathmax = PATH_MAX;
#else
static long pathmax = 0;
#endif

static long posix_version = 0;
static long xsi_version = 0;

/* if PATH_MAX is indeterminate, no guarantee this is adequate */
#define PATH_MAX_GUESS 1024

void *cns_alloc(size_t size)
{
     void *p;

     p = malloc(size);
     if (p == NULL) {
          err(EXIT_FAILURE, "malloc(%luz) failed", size);
     }

     return p;
}

char *cns_path_alloc(size_t *sizep)
{
     char *ptr;
     size_t size;

     if (posix_version == 0)
          posix_version = sysconf(_SC_VERSION);

     if (xsi_version == 0)
          xsi_version = sysconf(_SC_XOPEN_VERSION);

     if (pathmax == 0) {          /* first time through */
          errno = 0;
          if ((pathmax = pathconf("/", _PC_PATH_MAX)) < 0) {
               if (errno == 0)
                    pathmax = PATH_MAX_GUESS;
               else
                    err(EXIT_FAILURE, "pathconf error for _PC_PATH_MAX");
          } else {
               pathmax++;
          }
     }

     /*
      * Before PASIX.1-2001, we aren't guaranteed that PATH_MAX includes
      * the terminating null byte. Same goes for XPG3.
      */
     if ((posix_version < 200112L) && (xsi_version < 4))
          size = pathmax + 1;
     else
          size = pathmax;

     ptr = (char *)cns_alloc(size);

     if (sizep != NULL)
          *sizep = size;
     return ptr;
}

void *cns_clone(const void *src, size_t size)
{
     assert(src);
     void *p;
     p = cns_alloc(size);
     memmove(p, src, size);
     return p;
}

void *cns_repeat(void *data, size_t size, int times)
{
     assert(data && times > 0);
     void *p, *dst;
     int i;
     dst = p = cns_alloc(size * times);
     for (i = 0; i < times; i++, p = (char *)p + size * times)
          memmove(p, data, size);
     return dst;
}

int cns_compute_length(uint32_t ndim, uint32_t *dims)
{
     if (dims) {
          int i, len = 1;
          for (i = 0; i < ndim; i++)
               len *= dims[i];
          return len;
     }
     cns_err_msg("Warning: cns_compute_length: null dims\n");
     return 0;
}

size_t cns_size_of(cns_dtype dtype)
{
     size_t size;

     switch(dtype) {
     case CNS_BOOL:
          size = sizeof(cns_bool_t);
          break;
     case CNS_FLOAT:
          size = sizeof(float);
          break;
     case CNS_INT32:
          size = sizeof(int32_t);
          break;
     case CNS_INT16:
          size = sizeof(int16_t);
          break;
     case CNS_INT8:
          size = sizeof(int8_t);
          break;
     case CNS_UINT32:
          size = sizeof(uint32_t);
          break;
     case CNS_UINT16:
          size = sizeof(uint16_t);
          break;
     case CNS_UINT8:
          size = sizeof(uint8_t);
          break;
     default:
          cns_err_quit("ERROR: cns_size_of: unknown cns_dtype %d\n", dtype);
     }
     return size;
}

int cns_pointer_sub(void *p1, void *p2, cns_dtype dtype)
{
     switch (dtype) {
     case CNS_BOOL:
          return (cns_bool_t *)p1 - (cns_bool_t *)p2;
     case CNS_FLOAT:
          return (float *)p1 - (float *)p2;
     case CNS_INT32:
          return (int32_t *)p1 - (int32_t *)p2;
     case CNS_INT16:
          return (int16_t *)p1 - (int16_t *)p2;
     case CNS_INT8:
          return (int8_t *)p1 - (int8_t *)p2;
     case CNS_UINT32:
          return (uint32_t *)p1 - (uint32_t *)p2;
     case CNS_UINT16:
          return (uint16_t *)p1 - (uint16_t *)p2;
     case CNS_UINT8:
          return (uint8_t *)p1 - (uint8_t *)p2;
     default:
          cns_err_quit("ERROR: cns_pointer_sub: unknown cns_dtype %d\n", dtype);
     }
}

void *cns_pointer_add(void *p, int offset, cns_dtype dtype)
{
     switch (dtype) {
     case CNS_BOOL:
          return (cns_bool_t *)p + offset;
     case CNS_FLOAT:
          return (float *)p + offset;
     case CNS_INT32:
          return (int32_t *)p + offset;
     case CNS_INT16:
          return (int16_t *)p + offset;
     case CNS_INT8:
          return (int8_t *)p + offset;
     case CNS_UINT32:
          return (uint32_t *)p + offset;
     case CNS_UINT16:
          return (uint16_t *)p + offset;
     case CNS_UINT8:
          return (uint8_t *)p + offset;
     default:
          cns_err_quit("ERROR: cns_pointer_add: unknown cns_dtype %d\n", dtype);
     }
}

static void err_doit(int errnoflag, int error, const char *fmt, va_list ap)
{
     char buf[CNS_MAXLINE];

     vsnprintf(buf, CNS_MAXLINE-1, fmt, ap);
     if (errnoflag)
          snprintf(buf+strlen(buf), CNS_MAXLINE-strlen(buf)-1, ": %s",
               strerror(error));
     strcat(buf, "\n");
     fflush(stdout);
     fputs(buf, stderr);
     fflush(NULL);
}

/*
 * Nonfatal error unrelated to a system call.
 * Print a message and return.
 */
void cns_err_msg(const char *fmt, ...)
{
     va_list ap;
     va_start(ap, fmt);
     err_doit(0, 0, fmt, ap);
     va_end(ap);
}

/*
 * Nonfatal error unrelated to a system call.
 * Error code passed as explict parameter.
 * Print a message and return.
 */
void cns_err_cont(int error, const char *fmt, ...)
{
     va_list ap;
     va_start(ap, fmt);
     err_doit(1, error, fmt, ap);
     va_end(ap);
}

/*
 * Nonfatal error related to a system call.
 * Print a message and return.
 */
void cns_err_ret(const char *fmt, ...)
{
     va_list ap;
     va_start(ap, fmt);
     err_doit(1, errno, fmt, ap);
     va_end(ap);
}

/*
 * Fatal error unrelated to a system call.
 * Print a message and terminate.
 */
void cns_err_quit(const char *fmt, ...)
{
     va_list ap;
     va_start(ap, fmt);
     err_doit(0, 0, fmt, ap);
     va_end(ap);
     exit(1);
}

/*
 * Fatal error unrelated to a system call.
 * Error code passed as explict parameter.
 * Print a message and terminate.
 */
void cns_err_exit(int error, const char *fmt, ...)
{
     va_list
          ap;
     va_start(ap, fmt);
     err_doit(1, error, fmt, ap);
     va_end(ap);
     exit(1);
}

/*
 * Fatal error related to a system call.
 * Print a message and terminate.
 */
void cns_err_sys(const char *fmt, ...)
{
     va_list ap;
     va_start(ap, fmt);
     err_doit(1, errno, fmt, ap);
     va_end(ap);
     exit(1);
}

/*
 * Fatal error related to a system call.
 * Print a message, dump core, and terminate.
 */
void cns_err_dump(const char *fmt, ...)
{
     va_list ap;
     va_start(ap, fmt);
     err_doit(1, errno, fmt, ap);
     va_end(ap);
     abort();
/* dump core and terminate */
     exit(1);
/* shouldn’t get here */
}
