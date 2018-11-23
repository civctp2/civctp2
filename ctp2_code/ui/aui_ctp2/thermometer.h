#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __THERMOMETER_H__
#define __THERMOMETER_H__

#include "aui_control.h"
#include "patternbase.h"

#define k_THERMOMETER_PERCENT_FILLED "percent"
#define k_THERMOMETER_COLOR			 "color"

class Thermometer : public aui_Control, public PatternBase
{
public:

	Thermometer(
		AUI_ERRCODE *retval,
		uint32 id,
		const MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	Thermometer(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		const MBCHAR *pattern,
		sint32 percentFilled = 0,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~Thermometer() {}

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	AUI_ERRCODE InitCommonLdl(const MBCHAR *ldlBlock);

	sint32 GetPercentFilled() { return m_percentFilled; }
	void SetPercentFilled( sint32 percentFilled );

protected:
	sint32	m_percentFilled;

};

#endif
