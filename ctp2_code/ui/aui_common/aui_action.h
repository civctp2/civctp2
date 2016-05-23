//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface action
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
// - Event handlers declared in a notation that is more standard C++.
// - Structure changed to support compilers other than MSVC6.
// - Added AUI_ACTION_BASIC macro.
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __AUI_ACTION_H__
#define __AUI_ACTION_H__

//----------------------------------------------------------------------------
// Library imports
//----------------------------------------------------------------------------

// None

//----------------------------------------------------------------------------
// Exported names
//----------------------------------------------------------------------------

class aui_Action;

//----------------------------------------------------------------------------
//
// Name       : AUI_ACTION_BASIC
//
// Description: Class generation macro to define an aui_Action derived class
//              that only overrides the Execute action.
//
// Parameters : ClassName   : name of the derived class
//
// Remark(s)  : MACRO
//
//----------------------------------------------------------------------------
#define AUI_ACTION_BASIC(ClassName)     \
class ClassName : public aui_Action     \
{                                       \
public:                                 \
	virtual void	Execute             \
	(                                   \
		aui_Control	*	control,        \
		uint32			action,         \
		uint32			data            \
	);                                  \
}

//----------------------------------------------------------------------------
// Project imports
//----------------------------------------------------------------------------

#include "aui_base.h"			// aui_Base
#include "aui_control.h"		// aui_Control
#include "c3types.h"			// uint32

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class aui_Action : public aui_Base
{
public:
	aui_Action() : aui_Base() { ; };
	virtual ~aui_Action(void) { ; };

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	)
    {
        // Default: do nothing at all.
    };
};

#endif
