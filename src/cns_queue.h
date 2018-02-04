#ifndef _CNS_QUEUE_H_
#define _CNS_QUEUE_H_

#include <stdlib.h>
#include "cns_list.h"

typedef struct cns_queue cns_queue;
struct cns_queue {
	cns_list *head;
	cns_list *tail;
	size_t    size;
};

#ifdef __cplusplus
extern "C" {
#endif

	cns_queue *cns_queue_create();
	cns_queue *cns_queue_enqueue(cns_queue *queue, void *data);
	void *cns_queue_dequeue(cns_queue *queue);
	void cns_queue_free(cns_queue *queue);

#ifdef __cplusplus
}
#endif

#endif	/* _CNS_QUEUE_H_ */
