//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface static image or text
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Event handlers declared in a notation that is more standard C++.
//
//----------------------------------------------------------------------------

#ifndef __CTP2_STATIC_H__
#define __CTP2_STATIC_H__

#include "aui_static.h"
#include "patternbase.h"

#define k_CTP2_STATIC_LDL_BEVELWIDTH		"bevelwidth"
#define k_CTP2_STATIC_LDL_BEVELTYPE			"beveltype"
#define k_CTP2_STATIC_LDL_STATICIMAGETYPE	"multiimagestatic"

#define k_CTP2_STATIC_ACTION_LMOUSE 1
#define k_CTP2_STATIC_ACTION_RMOUSE 2

enum
{
	STATIC_IMAGE_LEFT  = 0,
	STATIC_IMAGE_CENTER,
	STATIC_IMAGE_RIGHT ,
	STATIC_IMAGE_MAX
};

class aui_Surface;
class aui_ImageList;

class ctp2_Static : public aui_Static, public PatternBase
{
public:
	typedef AUI_ERRCODE (StaticDrawCallback)(
		ctp2_Static *control,
		aui_Surface *surface,
		RECT &rect,
		void *cookie );

	typedef void (ImageMapCallback)(
		ctp2_Static *control,
		aui_MouseEvent *event,
		void *cookie);

	ctp2_Static(AUI_ERRCODE *retval,
			uint32 id,
			const MBCHAR *ldlBlock );
	ctp2_Static(AUI_ERRCODE *retval,
			uint32 id,
			sint32 x,
			sint32 y,
			sint32 width,
			sint32 height,
			const MBCHAR *pattern,
			const MBCHAR *text,
			uint32 maxLength,
			uint32 bevelWidth,
			uint32 bevelType);

	AUI_ERRCODE InitCommonLdl(const MBCHAR *ldlBlock);
	AUI_ERRCODE InitCommon(uint32 bevelWidth, uint32 bevelType );

	virtual AUI_ERRCODE DrawThis(aui_Surface *surface,
			sint32 x,
			sint32 y);

	uint32 BevelWidth() { return m_bevelWidth; }

	void SetBevel(uint32 width, uint32 type);

	void SetDrawCallbackAndCookie(StaticDrawCallback *func, void *cookie,bool exclusive=true);
	void SetImageMapCallback(ImageMapCallback *func, void *cookie);




	void FitToBitmap(void);

	void SetChromaKey(sint32 r,sint32 g,sint32 b);

	void SetChromakey(sint32 r, sint32 g, sint32 b);

	bool	BuildImageList();

	virtual AUI_ERRCODE Resize(sint32 width, sint32 height);

	virtual bool IgnoreHighlight() { return m_ignoreHighlight; }

protected:
	virtual void	MouseLGrabInside(aui_MouseEvent * mouseData);
	virtual void	MouseLDropInside(aui_MouseEvent * mouseData);
	virtual void	MouseRGrabInside(aui_MouseEvent * mouseData);
	virtual void	MouseRDropInside(aui_MouseEvent * mouseData);

	bool	ConstructImageRect(uint32 index);

	AUI_ERRCODE DrawImages(aui_Surface *destSurf,RECT *destRect);

	uint32	m_bevelWidth;
	uint32	m_bevelType;
	bool	m_drawCallbackExclusive;

	bool   m_multiImageStatic;
	RECT   m_textRects[STATIC_IMAGE_MAX];

	StaticDrawCallback *m_drawFunc;
	void *m_drawCookie;

	ImageMapCallback *m_imageMapCallback;
	void *m_imageMapCookie;

	bool m_ignoreHighlight;
};

#endif
