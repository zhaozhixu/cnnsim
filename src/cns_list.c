#include "cns_util.h"
#include "cns_list.h"

/* return the list with appended element (a new list if list == NULL) */
cns_list *cns_list_append(cns_list *list, void *data)
{
	cns_list *l;

	if (!list) {
		l = (cns_list *)cns_alloc(sizeof(cns_list));
		l->data = data;
		l->next = NULL;
		return l;
	}
	for (l = list; l->next; l = l->next)
		;
	l->next = (cns_list *)cns_alloc(sizeof(cns_list));
	l->next->data = data;
	l->next->next = NULL;
	return list;
}

void cns_list_free(cns_list *list)
{
	cns_list *tmp, *l;

	for (l = list; l;) {
		tmp = l->next;
		cns_free(l);
		l = tmp;
	}
}

/* return the nth element in list, or NULL if the position is off the end of list */
cns_list *cns_list_nth(cns_list *list, int n)
{
	cns_list *l;
	int pos;

	for (l = list, pos = 0; l; l = l->next, pos++)
		if (pos == n)
			return l;
	return NULL;
}

void *cns_list_nth_data(cns_list *list, int n)
{
	return cns_list_nth(list, n)->data;
}

cns_list *cns_list_remove_nth(cns_list *list, int n)
{
	cns_list **lp;
	cns_list *tmp;
	int i;

	for (i = 0, lp = &list; *lp; lp = &(*lp)->next, i++) {
		if (i == n) {
			tmp = *lp;
			*lp = tmp->next;
			cns_free(tmp);
			break;
		}
	}
	return list;
}

/* return the list with inserted element, or NULL if list == NULL */
cns_list *cns_list_insert_nth(cns_list *list, void *data, int n)
{
	cns_list **lp;
	cns_list *tmp;
	int i;

	for (i = 0, lp = &list; *lp; lp = &(*lp)->next, i++) {
		if (i == n) {
			tmp = *lp;
			*lp = (cns_list *)cns_alloc(sizeof(cns_list));
			(*lp)->data = data;
			(*lp)->next = tmp;
		}
	}
	return list;
}
