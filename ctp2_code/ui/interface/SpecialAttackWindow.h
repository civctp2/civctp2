//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Little popup window that shows the costs of a special attack
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
// HAVE_PRAGMA_ONCE
// - Compiles the file with #pragma once
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - This file is not part of the original source code.
// - File created. (Aug 15th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __SPECIAL_ATTACK_WINDOW__H__
#define __SPECIAL_ATTACK_WINDOW__H__ 1

#include "c3window.h"

class MapPoint;

class SpecialAttackWindow : public C3Window
{
public:
	SpecialAttackWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD) : C3Window(retval,id,ldlBlock,16,type,false) {};
	virtual ~SpecialAttackWindow() {};

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );


};

sint32 specialAttackWindow_Initialize();
void specialAttackWindow_DisplayData(MapPoint &p, sint32 type);
sint32 specialAttackWindow_Cleanup();

#endif // __SPECIAL_ATTACK_WINDOW__H__
