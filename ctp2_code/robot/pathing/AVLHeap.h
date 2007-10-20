//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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

#pragma once
#ifndef __AVL_HEAP_H__
#define  __AVL_HEAP_H__

#include "astarpnt.h"


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
    void MassDelete(const bool isunit);
    void Validate();

  
};

#endif