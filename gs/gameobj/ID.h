
#pragma once
#ifndef __ID_H__
#define __ID_H__ 1

class CivArchive;

#define k_ID_VERSION_MAJOR	0										
#define k_ID_VERSION_MINOR	0										

class ID { 

public:
		
	uint32 m_id; 
	
	ID () { m_id = 0; };
	ID (sint32 val) { m_id = val; }; 
	ID (uint32 val) { m_id = val; }; 
	ID (const int val) {
		Assert (0 <= val); 
		m_id = unsigned int (val); }; 
	
	ID (const unsigned int val) {
		m_id = val; }; 
	
	void Castrate() { } 
	void DelPointers() {} 
	
	
	
	operator int() const  { return m_id; }; 
	operator unsigned int() const  { return m_id; }; 
	operator unsigned long() const  { return m_id; };
   
	sint32 operator! () const { return !m_id; };

	uint32 operator~ () const { return ~m_id; };
	uint32  operator& (const ID &val) const { return m_id & val.m_id; };  
	

	uint32  operator| (const ID &val) const { return m_id | val.m_id; };
	
 
	sint32 operator== (const ID &val) const { return m_id == val.m_id; };
	
   
	sint32 operator!= (const ID &val) const { return m_id != val.m_id; };
	

	sint32 operator&& (const ID &val) const { return m_id && val.m_id; };
	
  
	sint32 operator|| (const ID &val) const { return m_id || val.m_id; };
	

 
	operator< (const ID &val) const { return m_id < val.m_id; };
	

	operator<= (const ID &val) const { return m_id <= val.m_id; };
	

	operator> (const ID &val) const { return m_id > val.m_id; };
	
	operator>= (const ID &val) const { return m_id >= val.m_id; };
	
  

	ID & operator= (const ID &val) { 
        m_id = val.m_id;
        return *this; };

	
	void Serialize(CivArchive &archive) ;
};

uint32 ID_ID_GetVersion(void) ;
#endif
