//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : 
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #pragma once commented out.
// - import structure modified to support mingw compilation.
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __ID_H__
#define __ID_H__ 1

//----------------------------------------------------------------------------
// Library imports
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Exported names
//----------------------------------------------------------------------------

class	ID;

#define k_ID_VERSION_MAJOR	0										
#define k_ID_VERSION_MINOR	0										

//----------------------------------------------------------------------------
// Project imports
//----------------------------------------------------------------------------

#include "civarchive.h"			// CivArchive
#include "ctp2_inttypes.h"      // sint32, uint32

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class ID { 
public:
	uint32 m_id; 
	
	ID () { m_id = 0; };
	ID (sint32 val) { m_id = val; }; 
	ID (uint32 val) { m_id = val; }; 
	
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
	

	bool operator < (const ID & val) const 
	{ 
		return m_id < val.m_id; 
	};
	
	bool operator <= (const ID & val) const 
	{ 
		return m_id <= val.m_id; 
	};
	
	bool operator > (const ID & val) const 
	{ 
		return m_id > val.m_id; 
	};
	
	bool operator >= (const ID & val) const 
	{ 
		return m_id >= val.m_id; 
	};

	ID & operator= (const ID &val) { 
        m_id = val.m_id;
        return *this; };

	
	void Serialize(CivArchive &archive) ;
};

uint32 ID_ID_GetVersion(void) ;

#endif
