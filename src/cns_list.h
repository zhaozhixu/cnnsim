#ifndef _CNS_LIST_H_
#define _CNS_LIST_H_

typedef struct cns_list_node cns_list_node;

struct cns_list_node {
	void          *data;
	cns_list_node *next;
};

typedef cns_list_node cns_list;

cns_list *cns_list_append(cns_list *list, void *data);
void cns_list_free(cns_list *list);
cns_list *cns_list_nth(cns_list *list, int n);
void *cns_list_nth_data(cns_list *list, int n);
cns_list *cns_list_remove_nth(cns_list *list, int n);
cns_list *cns_list_insert_nth(cns_list *list, void *data, int n);

#endif	/* _CNS_LIST_H_ */
