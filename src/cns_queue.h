#ifndef _CNS_QUEUE_H_
#define _CNS_QUEUE_H_

#include <stdlib.h>
#include "cns_list.h"

typedef struct cns_queue cns_queue;
struct cns_queue {
	cns_list *head;
	cns_list *tail;
	size_t    length;
};

cns_queue *cns_queue_enqueue(cns_queue *queue, void *data);
void *cns_queue_dequeue(cns_queue *queue);
void cns_queue_free(cns_queue *queue);

#endif	/* _CNS_QUEUE_H_ */
