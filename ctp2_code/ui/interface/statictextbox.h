#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __STATICTEXTBOX_H__
#define __STATICTEXTBOX_H__

#include "aui_static.h"


class StaticTextBox : public aui_Static
{
public:

	StaticTextBox(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *text = NULL,
		uint32 maxLength = 0,
		uint32 size = 0,
		sint32 bevel = 0);
	StaticTextBox(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock);

	virtual ~StaticTextBox() {}

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	sint32 GetBevel() { return m_bevel; }
	void SetBevel( sint32 bevel ) { m_bevel = bevel; }

protected:
	sint32 m_bevel;
};

#endif
