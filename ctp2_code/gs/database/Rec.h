











#pragma once
#ifndef __REC_H__
#define __REC_H__ 1

class CivArchive; 

#define k_MAX_OBSOLETE 8

#include "dbtypes.h"

class Record { 
    
public:
    
	
	

    
	
    StringId m_name; 
    sint32 m_enable; 
protected:
    sint32 m_obsolete[k_MAX_OBSOLETE];
    sint32 m_nObsolete;
public:
	
	

	
	
	

	
	
    Record();
	virtual ~Record() {};
    void SetName(const StringId string_id); 
    StringId GetName() const { return m_name; }
    void SetEnabling(const sint32 e); 
    void SetObsolete(const sint32 o, sint32 index); 

    sint32 GetEnabling() const; 
    sint32 GetObsolete(sint32 index) const; 
	sint32 GetNumObsolete() const;

	void Serialize(CivArchive &archive) ;
}; 

#endif
