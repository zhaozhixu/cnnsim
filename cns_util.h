#ifndef _CNS_UTIL_H_
#define _CNS_UTIL_H_

#include <stdlib.h>

#define cns_free free

void *cns_alloc(size_t size);
char *cns_path_alloc(size_t *sizep);

#endif	/* _CNS_UTIL_H_ */
