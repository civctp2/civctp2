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
// _BFR
// - Generate final release when set (forces CD check).
//
// _MSC_VER
// - Use Microsoft C++ extensions when set.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #pragma once commented out.
// - import structure modified to support mingw compilation.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __ID_H__
#define __ID_H__ 1

#if defined(_MSC_VER)

class CivArchive;

#define k_ID_VERSION_MAJOR	0
#define k_ID_VERSION_MINOR	0

#else	// _MSC_VER

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

#include "c3.h"					// General declarations
#include "civarchive.h"			// CivArchive

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

#endif	// _MSC_VER

class ID {
public:
	uint32 m_id;

/* 	ID () { m_id = 0; }; */
/* 	ID (sint32 val) { m_id = val; };  */
/* 	ID (uint32 val) { m_id = val; };  */
/* 	ID (sint64 val) { m_id = val; };  */
/* 	ID (uint64 val) { m_id = val; };  */
/* #if defined(WIN32) */
/* 	ID (const int val) { */
/* 		Assert (0 <= val);  */
/* 		m_id = unsigned int (val); };  */

/* 	ID (const unsigned int val) { */
/* 		m_id = val; };  */
/* #endif */

	ID (uint32 val = 0)
    :   m_id    (val)
    {  };

    ID (const ID & i)
    :   m_id    (i.m_id)
    {  };

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

#if defined(_MSC_VER)

	operator< (const ID &val) const { return m_id < val.m_id; };

	operator<= (const ID &val) const { return m_id <= val.m_id; };

	operator> (const ID &val) const { return m_id > val.m_id; };

	operator>= (const ID &val) const { return m_id >= val.m_id; };

#else	// _MSC_VER

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

#endif	// _MSC_VER

	ID & operator= (const ID &val) {
        m_id = val.m_id;
        return *this; };

	void Serialize(CivArchive &archive) ;
};

uint32 ID_ID_GetVersion(void) ;

#endif
