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
// _MSC_VER		
// - Use Microsoft C++ extensions when set.
//
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Import structure changed to compile with Mingw
// - Event handlers declared in a notation that is more standard C++.
//
//----------------------------------------------------------------------------

#ifndef __AUI_ACTION_H__
#define __AUI_ACTION_H__

#if defined(ACTIVISION_ORIGINAL)

#include "aui_base.h"


class aui_Control;

#else // ACTIVISION_ORIGINAL

//----------------------------------------------------------------------------
// Library imports
//----------------------------------------------------------------------------

// None

//----------------------------------------------------------------------------
// Exported names
//----------------------------------------------------------------------------

class aui_Action;

//----------------------------------------------------------------------------
// Project imports
//----------------------------------------------------------------------------

#include "aui_base.h"			// aui_Base
#include "aui_control.h"		// aui_Control
#include "c3types.h"			// uint32

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

#endif // ACTIVISION_ORIGINAL


class aui_Action : public aui_Base
{
public:
	typedef void (ActionCallback)(
		aui_Control *control,
		uint32 action,
		uint32 data );

	
	aui_Action() {}
	virtual ~aui_Action() {}

#if defined(_MSC_VER)	
	virtual ActionCallback Execute {}
#else
	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	) {};
#endif
};


#endif 
