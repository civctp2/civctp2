












#pragma once
#ifndef __C3SPINNER_H__
#define __C3SPINNER_H__


#include "aui_ranger.h"
#include "patternbase.h"



class C3Spinner : public aui_Ranger, public PatternBase
{
public:
	
	C3Spinner(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	C3Spinner(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		BOOL isVertical,
		MBCHAR *pattern,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~C3Spinner() {}

protected:
	C3Spinner() : aui_Ranger() {}
	AUI_ERRCODE InitCommon( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( BOOL isVertical );
	AUI_ERRCODE CreateButtons( void );

public:
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	BOOL	IsVertical( void ) const { return m_isVertical; }

protected:
	BOOL	m_isVertical;	
};


#endif 
