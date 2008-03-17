//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Activision User Interface - input handling base class
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
// - Decreased MSVC dependency
// - Improved include structure
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef AUI_INPUT_H_
#define AUI_INPUT_H_

class aui_Input;

#include "auitypes.h"   // AUI_ERRCODE

class aui_Input
{
public:
	aui_Input();
	virtual ~aui_Input();

	virtual AUI_ERRCODE GetInput( void ) { return AUI_ERRCODE_OK; }
	virtual AUI_ERRCODE ReactToInput( void ) { return AUI_ERRCODE_OK; }

	virtual AUI_ERRCODE Acquire( void );
	virtual AUI_ERRCODE Unacquire( void );

protected:
	bool IsAcquired( void ) const { return m_acquired; }

private:
	bool m_acquired;
};


#endif 
