

































#ifndef dynatab_h
#define dynatab_h

#include <stdio.h>
#include <stddef.h>







#include "dppackn.h"


#define dynatab_NONE	-1

typedef struct {
	void *buf;
	size_t unit;	
	int  n_used;	
	int  n_alloced;	
} dynatab_t;





dynatab_t *dynatab_create(size_t element_size);




void dynatab_destroy(dynatab_t *ptab);





void dynatab_remove(dynatab_t *tab, int subscript);




void dynatab_removeAll(dynatab_t *tab);






void *dynatab_subscript_grow(dynatab_t *tab, int subscript);






void *dynatab_subscript_insert(dynatab_t *tab, int n);






#ifdef dyatab_NONINLINE
void *dynatab_subscript(dynatab_t *tab, int subscript);
#else
#define dynatab_subscript(tab, subscript) \
	((void *)(((subscript) < 0 || (subscript) >= (tab)->n_used) ? NULL : \
	(((char *)((tab)->buf) + ((subscript) * (tab)->unit)))))
#endif





int dynatab_find(dynatab_t *tab, void* addr);




#define dynatab_size(tab)	(tab->n_used)




#define dynatab_elemSize(tab)	(tab->unit)





void dynatab_freeze(dynatab_t *tab, FILE *fp);







void *dynatab_thaw(dynatab_t *tab, FILE *fp);

#include "dpunpack.h"
#endif
