#include "cns_util.h"
#include "cns_list.h"

cns_list *cns_list_append(cns_list *list, void *data)
{
	cns_list *l;

	if (list == NULL) {
		l = (cns_list *)cns_alloc(sizeof(cns_list));
		l->data = data;
		l->next = NULL;
		return l;
	}
	for (l = list; l->next != NULL; l = l->next)
		;
	l->next = (cns_list *)cns_alloc(sizeof(cns_list));
	l->next->data = data;
	l->next->next = NULL;
	return list;
}

void cns_list_free(cns_list *list)
{
	cns_list *tmp, *l;

	for (l = list; l != NULL;) {
		tmp = l->next;
		cns_free(l);
		l = tmp;
	}
}
