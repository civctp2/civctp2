











#pragma once
#ifndef __STR_REC_H__
#define __STR_REC_H__ 1

#include "dbtypes.h"

class StringRecord { 

public:

	
	
	StringId m_index;
	char *m_id;
	char *m_text;
	StringRecord *m_lesser, *m_greater; 

	StringRecord();
    ~StringRecord(); 
};

#endif
