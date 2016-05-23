#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __COLORICONSWITCH_H__
#define __COLORICONSWITCH_H__


#include "c3_switch.h"


class Picture;


class ColorIconSwitch : public c3_Switch
{
public:

	ColorIconSwitch(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		const MBCHAR *pattern,
		const MBCHAR *icon,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );

	ColorIconSwitch(
		AUI_ERRCODE *retval,
		uint32 id,
		const MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );

	virtual AUI_ERRCODE	InitCommon(const MBCHAR *ldlBlock,BOOL isLDL = FALSE);

	virtual ~ColorIconSwitch();

	virtual AUI_ERRCODE Resize(sint32 width, sint32 height);
	void	ResizePictureRect(void);

	void SetIcon(const MBCHAR *name);
	void ShrinkToFit(BOOL fit) { m_shrinkToFit = fit; }

	const MBCHAR *GetFilename() { return m_filename; }

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

protected:
	MBCHAR		*m_filename;
	BOOL		m_shrinkToFit;
	RECT		m_pictureRect;
};

#endif
