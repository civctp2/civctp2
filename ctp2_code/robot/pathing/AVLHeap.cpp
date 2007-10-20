//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : AVL-Heap
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - None
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "AVLHeap.h"
#include "pixelutils.h"


#include "XY_Coordinates.h"
#include "World.h"

extern World *g_theWorld; 

AVLHeap::AVLHeap()
{
    InitHeap(); 
}
void AVLHeap::InitHeap()

{ 
	m_block_list = NULL; 
	m_used_head = NULL; 
	m_used_tail = NULL;
	m_ready = NULL; 
}

AVLHeap::~AVLHeap()

{
    CleanUp(); 
}

void AVLHeap::CleanUp()

{
    AstarPoint *tmp; 

	while (m_block_list) { 
		tmp = m_block_list; 
   		m_block_list = m_block_list->m_next; 
		delete[] tmp;
	} 
    InitHeap(); 

} 


AstarPoint * AVLHeap::GetNew()


{ 
	if (m_ready == NULL) { 
		InitNewBlock(); 
	} 

	AstarPoint *tmp = m_ready; 

	m_ready = m_ready->m_next; 

	tmp->m_next = NULL; 
	if (m_used_tail == NULL) { 	
		m_used_head = tmp; 
		m_used_tail = tmp; 
	} else {
		m_used_tail->m_next = tmp; 
		m_used_tail = tmp; 
	}

#ifdef _DEBUG
    tmp->Clear(); 
#endif
	return tmp; 
} 

void AVLHeap::MassDelete(const bool isunit)

{
    sint32 c = pixelutils_RGB(0,0,30);

#ifdef _DEBUG
    if (isunit) { 
        AstarPoint *tmp=NULL; 
        for (tmp = m_used_head; tmp; tmp = tmp->m_next) { 
            g_theWorld->SetColor(tmp->m_pos, c); 
        }
    }
#endif

    if (m_used_tail != NULL) { 
    	m_used_tail->m_next = m_ready; 
	    m_ready =  m_used_head; 
	    m_used_head = m_used_tail = NULL; 
    }
}


void AVLHeap::InitNewBlock() 
{
	uint32 i; 

	AstarPoint *tmp = new AstarPoint[AVLHEAP_SIZE]; 

	tmp[0].m_next = m_block_list; 
	m_block_list = tmp; 
	for (i=1; i<(AVLHEAP_SIZE-1); i++) { 
		tmp[i].m_next = &(tmp[i+1]); 
	} 
	tmp[AVLHEAP_SIZE-1].m_next = NULL; 
    m_ready = &(tmp[1]); 
} 

void AVLHeap::Validate()

{ 
    AstarPoint *o, *p, *s, *test; 
    uint32 i; 
    BOOL searching; 

    
    for (o = m_block_list; o; o = o->m_next) { 
        p = o; 
        for (i=0; i<AVLHEAP_SIZE; i++) { 
            test = p[i].m_next; 
            searching = TRUE; 
            if (test == NULL) { 
                searching = FALSE; 
            } 

            for (s=m_block_list; searching && s; s = s->m_next) { 
                if ((uint32(&s[0]) <= uint32(test)) && (uint32(test) <= uint32(&s[AVLHEAP_SIZE-1]))) { 
                    searching = FALSE; 
                } 
            } 

            Assert (searching == FALSE); 
            if (searching == FALSE) { 
                p[i].m_next = NULL; 
            } 
        } 
    } 
}