

#pragma once
#ifndef __ASTAR_QUEUE__
#define __ASTAR_QUEUE__ 1

#include "AstarPnt.h"

class AstarQueue { 

  AstarPoint *head; 

public:

	AstarQueue(); 
	void Enqueue(AstarPoint *node); 
	AstarPoint *Dequeue(); 
      
    sint32 Remove(const AstarPoint *node);

    void Empty() { head = NULL; }
    sint32 IsEmpty() { return head == NULL; }
  };

#endif
