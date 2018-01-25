#include "cns_queue.h"
#include "cns_util.h"

/* return the queue with insert element (a new queue if queue == NULL) */
cns_queue *cns_queue_enqueue(cns_queue *queue, void *data)
{
	cns_queue *q;

	if (!queue) {
		q = (cns_queue *)cns_alloc(sizeof(cns_queue));
		q->head = cns_list_append(NULL, data);
		q->tail = q->head;
		q->length = 1;
		return q;
	}

	q = queue;
	q->tail = cns_list_append(q->tail, data);
	q->length++;
	if (!q->head)
		q->head = q->tail;
	else
		q->tail = q->tail->next;

	return q;
}

void *cns_queue_dequeue(cns_queue *queue)
{
	void *data;

	data = cns_list_nth_data(queue->head, 0);
	queue->head = cns_list_remove_nth(queue->head, 0);
	if (queue->head == NULL)
		queue->tail = NULL;
	if (queue->length > 0)
		queue->length--;

	return data;
}

void cns_queue_free(cns_queue *queue)
{
	cns_list_free(queue->head);
	cns_free(queue);
}
