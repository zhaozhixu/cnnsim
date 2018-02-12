#ifndef _CNS_LIST_H_
#define _CNS_LIST_H_

#include "cns_util.h"

typedef struct cns_list_node cns_list_node;

struct cns_list_node {
	void          *data;
	cns_list_node *next;
};

typedef cns_list_node cns_list;

#ifdef __cplusplus
extern "C" {
#endif

	cns_list *cns_list_append(cns_list *list, void *data);
	void cns_list_free(cns_list *list);
	void cns_list_free_deep(cns_list *list);
	cns_list *cns_list_nth(cns_list *list, int n);
	void *cns_list_nth_data(cns_list *list, int n);
	cns_list *cns_list_remove(cns_list *list, void *data);
	cns_list *cns_list_remove_nth(cns_list *list, int n);
	cns_list *cns_list_insert_nth(cns_list *list, void *data, int n);
	cns_list *cns_list_find(cns_list *list, void *data);
	cns_list *cns_list_find_custom(cns_list *list, void *data,
				cns_cmp_func cmp);
	int cns_list_position(cns_list *list, cns_list *llink);
	int cns_list_index(cns_list *list, void *data);

#ifdef __cplusplus
}
#endif

#endif	/* _CNS_LIST_H_ */
