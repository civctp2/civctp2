//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
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
//
//----------------------------------------------------------------------------
#ifndef __AUI_BASE_H__
#define __AUI_BASE_H__

class aui_Base
{
public:

	aui_Base() { m_baseRefCount++; }
	virtual ~aui_Base() { m_baseRefCount--; }

	static sint32	GetBaseRefCount( void ) { return m_baseRefCount; }
   static uint32   CalculateHash(const MBCHAR *string);

protected:
	static sint32	m_baseRefCount;
};

#endif
