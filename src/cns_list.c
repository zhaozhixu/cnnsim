#include "cns_list.h"
#include "cns_util.h"

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

void cns_list_free_deep(cns_list *list)
{
	cns_list *tmp, *l;

	for (l = list; l;) {
		tmp = l->next;
		cns_free(l->data);
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
	cns_list *l;
	l = cns_list_nth(list, n);
	return l ? l->data : NULL;
}

cns_list *cns_list_remove(cns_list *list, void *data)
{
	cns_list **lp;
	cns_list *tmp;

	for (lp = &list; *lp; lp = &(*lp)->next) {
		if ((*lp)->data == data) {
			tmp = *lp;
			*lp = tmp->next;
			cns_free(tmp);
			break;
		}
	}
	return list;
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

/*
 * Return the list with inserted element, or NULL if list == NULL.
 * If the position n is negative or larger or equal than the length
 * of the list, the new element is added on to the end of the list.
 */
cns_list *cns_list_insert_nth(cns_list *list, void *data, int n)
{
	cns_list **lp;
	cns_list *tmp;
	int i;

	if (n < 0)
		return cns_list_append(list, data);

	for (i = 0, lp = &list; *lp; lp = &(*lp)->next, i++) {
		if (i == n) {
			tmp = *lp;
			*lp = (cns_list *)cns_alloc(sizeof(cns_list));
			(*lp)->data = data;
			(*lp)->next = tmp;
			break;
		}
	}

	if (!*lp)
		*lp = cns_list_append(NULL, data);

	return list;
}

cns_list *cns_list_find(cns_list *list, void *data)
{
	cns_list *l;

	for (l = list; l; l = l->next)
		if (data == l->data)
			return l;
	return NULL;
}

cns_list *cns_list_find_custom(cns_list *list, void *data, cns_cmp_func cmp)
{
	cns_list *l;

	for (l = list; l; l = l->next)
		if (cmp(data, l->data) == 0)
			return l;
	return NULL;
}

int cns_list_position(cns_list *list, cns_list *llink)
{
	cns_list *l;
	int i;

	for (i = 0, l = list; l; l = l->next, i++)
		if (l == llink)
			return i;
	return -1;
}

int cns_list_index(cns_list *list, void *data)
{
	cns_list *l;
	int i;

	for (i = 0, l = list; l; l = l->next, i++)
		if (l->data == data)
			return i;
	return -1;
}
