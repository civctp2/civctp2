



























#ifndef assoctab_h
#define assoctab_h

#include <stddef.h>
#include "dynatab.h"







#include "dppackn.h"


typedef struct {
	int key;
	char value[1];	
} assoctab_item_t;

typedef 
	dynatab_t
assoctab_t;







assoctab_t *assoctab_create(size_t element_size);




void assoctab_destroy(assoctab_t *ptab);




#define assoctab_size(tab)	(tab->n_used)






void *assoctab_subscript_grow(assoctab_t *tab, int key);






void *assoctab_subscript_insert(assoctab_t *tab, int key, int n);






int assoctab_subscript_delete(assoctab_t *tab, int key);






void *assoctab_subscript(assoctab_t *tab, int key);







#define assoctab_getkey(tab, n) ((assoctab_item_t *)dynatab_subscript(tab, n))





#define assoctab_freeze(tab, fp) dynatab_freeze(tab, fp)





#define assoctab_thaw(tab, fp) dynatab_thaw(tab, fp)




#define assoctab_removeAll(tab) dynatab_removeAll(tab)




#if !defined(NDEBUG)

#define assoctab_assertValid(a) {\
	assert((a) != NULL);\
	assert((a)->unit < 20000);\
	assert((a)->n_used < 5000);\
	assert((a)->n_alloced < 5000);\
}

#else
#define assoctab_assertValid(d)
#endif


#include "dpunpack.h"

#endif
