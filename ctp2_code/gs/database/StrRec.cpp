











#include "c3.h"

#include "StrRec.h"

StringRecord::StringRecord()

{ 
    m_index = -1; 
    m_id = NULL;
    m_lesser = NULL; 
	m_greater = NULL; 
    m_text = NULL;
}

StringRecord::~StringRecord()

{
	if(m_text)
		delete [] m_text;
	if(m_id)
		delete [] m_id;
}

