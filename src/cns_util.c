#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <err.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

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
     fprintf(stderr, "Warning: null dims in computeLength\n");
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
          fprintf(stderr, "ERROR: cns_size_of: unknown cns_dtype %d\n", dtype);
          exit(EXIT_FAILURE);
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
          fprintf(stderr, "ERROR: cns_pointer_sub: unknown cns_dtype %d\n", dtype);
          exit(EXIT_FAILURE);
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
          fprintf(stderr, "ERROR: cns_pointer_add: unknown cns_dtype %d\n", dtype);
          exit(EXIT_FAILURE);
     }
}
