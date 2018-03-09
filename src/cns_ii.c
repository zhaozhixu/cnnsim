#include "cns_ii.h"

int cns_ii_cmp(void *data1, void *data2)
{
	cns_ii *ii1, *ii2;

	ii1 = (cns_ii *)data1;
	ii2 = (cns_ii *)data2;
	if (ii1->idx == ii2->idx && ii1->itft == ii2->itft)
		return 0;
	return -1;
}

cns_ii *cns_ii_create(size_t idx, int itft)
{
	cns_ii *ii;

	ii = (cns_ii *)cns_alloc(sizeof(cns_ii));
	ii->idx = idx;
	ii->itft = itft;
	return ii;
}

void cns_ii_free(cns_ii *ii)
{
	cns_free(ii);
}

cns_list *cns_iis_append(cns_list *iis, size_t idx, int itft)
{
	return cns_list_append(iis, cns_ii_create(idx, itft));
}

cns_list *cns_iis_copy(cns_list *iis)
{
	cns_list *iis_cpy;
	cns_list *l;
	cns_ii *ii_cpy;
	cns_ii *ii;

	iis_cpy = NULL;
	for (l = iis; l; l = l->next) {
		ii = (cns_ii *)l->data;
		ii_cpy = cns_ii_create(ii->idx, ii->itft);
		iis_cpy = cns_list_append(iis_cpy, ii_cpy);
	}

	return iis_cpy;
}
