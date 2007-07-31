

#pragma once

#ifndef __C3_RANGER_H__
#define __C3_RANGER_H__

#include "aui_ranger.h"
#include "patternbase.h"

#define k_C3_RANGER_DEFAULTPATTERN		"pattern.tga"

class aui_Surface;
class aui_Static;


class c3_Ranger : public aui_Ranger, public PatternBase
{
public:
	
	c3_Ranger(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	c3_Ranger(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		AUI_RANGER_TYPE type,
		AUI_RANGER_ORIENTATION orientation,
		MBCHAR *pattern,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~c3_Ranger();

protected:
	c3_Ranger() : aui_Ranger() {}
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( void );
	AUI_ERRCODE CreateButtonsAndThumb( MBCHAR *ldlBlock );

public:
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

protected:
	virtual AUI_ERRCODE RepositionButtons( void );

	aui_Static *m_arrows[ 4 ];
};

#endif
