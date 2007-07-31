














#pragma once
#ifndef  __AVL_HEAP_H__
#define  __AVL_HEAP_H__

#include "AstarPnt.h"


#define AVLHEAP_SIZE ((4096)/sizeof(AstarPoint))

class AVLHeap { 

	AstarPoint *m_block_list; 
	AstarPoint *m_used_head; 
	AstarPoint *m_used_tail;
	AstarPoint *m_ready; 

    void InitNewBlock();

public:

    AVLHeap();
    ~AVLHeap();

    void InitHeap(); 
    void CleanUp();
    
    AstarPoint * GetNew();
    void MassDelete(const sint32 isunit);
    void Validate();

  
};

#endif