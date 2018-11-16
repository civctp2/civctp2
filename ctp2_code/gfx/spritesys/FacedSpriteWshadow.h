//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Sprite with facings and shadows
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
// - Added separate counters in Sprite-derived classes to prevent crashes.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __FACEDSPRITEWSHADOW_H__
#define __FACEDSPRITEWSHADOW_H__

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

// #include <>

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class FacedSpriteWshadow;

#define k_MAX_FACINGS		8
#define k_NUM_FACINGS		5

#define k_MAX_NAMES			100
#define k_MAX_NAME_LENGTH	_MAX_PATH

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "Sprite.h"

class aui_Surface;
class Token;

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class FacedSpriteWshadow : public Sprite
{
public:
	FacedSpriteWshadow();
	virtual ~FacedSpriteWshadow();

	void			Import(uint16 nframes, char *files[k_NUM_FACINGS][k_MAX_NAMES], char *shadowFiles[k_NUM_FACINGS][k_MAX_NAMES]);
	void			Import(uint16 nframes, char *files[k_NUM_FACINGS][k_MAX_NAMES]);

	void			Load(char *filename);

	void			Draw(sint32 drawX, sint32 drawY, sint32 facing, double m_scale, sint16 transparency,
							Pixel16 outlineColor, uint16 flags);

	void			DrawDirect(aui_Surface *surf, sint32 drawX, sint32 drawY, sint32 facing, double m_scale, sint16 transparency,
						Pixel16 outlineColor, uint16 flags);

	void			DirectionalDraw(sint32 drawX, sint32 drawY, sint32 facing,
					   double scale, sint16 transparency, Pixel16 outlineColor, uint16 flags);

	void			DrawShadow(sint32 SdrawX, sint32 SdrawY, sint32 facing, double m_scale, sint16 transparency,
						Pixel16 outlineColor, uint16 flags);

	void			DirectionalDrawShadow(sint32 drawX, sint32 drawY, sint32 facing,
					   double scale, sint16 transparency, Pixel16 outlineColor, uint16 flags);

	void			DrawFlashEffect(sint32 drawX, sint32 drawY, sint32 facing, double m_scale, sint16 transparency,
						Pixel16 outlineColor, uint16 flags);

	Pixel16			*GetFrameData(uint16 facing, uint16 frame) { return m_frames[facing][frame]; }
	size_t			GetFrameDataSize(uint16 facing, uint16 frame);
	Pixel16			*GetMiniFrameData(uint16 facing, uint16 frame) { return m_miniframes[facing][frame]; }
	size_t			GetMiniFrameDataSize(uint16 facing, uint16 frame);

	void			SetFrameData(uint16 facing, uint16 frame, Pixel16 *data, size_t size) { m_frames[facing][frame] = data; m_framesSizes[facing][frame] = size;}
	void			SetMiniFrameData(uint16 facing, uint16 frame, Pixel16 *data, size_t size) { m_miniframes[facing][frame] = data; m_miniframesSizes[facing][frame] = size;}

	Pixel16			*GetShadowFrameData(uint16 facing, uint16 frame) { return m_shadowFrames[facing][frame]; }
	size_t			GetShadowFrameDataSize(uint16 facing, uint16 frame);
	Pixel16			*GetMiniShadowFrameData(uint16 facing, uint16 frame) { return m_miniShadowFrames[facing][frame]; }
	size_t			GetMiniShadowFrameDataSize(uint16 facing, uint16 frame);

	void			SetShadowFrameData(uint16 facing, uint16 frame, Pixel16 *data, size_t size) { m_shadowFrames[facing][frame] = data; m_shadowFramesSizes[facing][frame] = size; }
	void			SetMiniShadowFrameData(uint16 facing, uint16 frame, Pixel16 *data, size_t size) { m_miniShadowFrames[facing][frame] = data; m_miniShadowFramesSizes[facing][frame] = size;}

	POINT			GetHotPoint(uint16 facing) { return m_hotPoints[facing]; }
	POINT			*GetHotPoints(void) { return m_hotPoints; }

	void			SetHotPoint(uint16 facing, sint32 x, sint32 y) { m_hotPoints[facing].x = x; m_hotPoints[facing].y = y; }
	void			SetHotPoints(POINT *points) { memcpy(m_hotPoints, points, sizeof(m_hotPoints)); }

	sint32			ParseFromTokens(Token *theToken);
	void			AllocateFrameArrays(size_t count);

	void			SetHasShadow(BOOL val) { m_hasShadow = val; }
	BOOL			GetHasShadow() { return m_hasShadow; }

	virtual uint16	GetNumFrames(void) const    { return m_shadowFrameCount; };
	virtual void	SetNumFrames(uint16 num)    { m_shadowFrameCount = num; }

protected:
	Pixel16			**m_frames[k_NUM_FACINGS];
	size_t			*m_framesSizes[k_NUM_FACINGS];
	Pixel16			**m_miniframes[k_NUM_FACINGS];
	size_t			*m_miniframesSizes[k_NUM_FACINGS];

	Pixel16			**m_shadowFrames[k_NUM_FACINGS];
	size_t			*m_shadowFramesSizes[k_NUM_FACINGS];
	Pixel16			**m_miniShadowFrames[k_NUM_FACINGS];
	size_t			*m_miniShadowFramesSizes[k_NUM_FACINGS];

	uint16			m_hasShadow;

	POINT			m_hotPoints[k_NUM_FACINGS];

	size_t          m_shadowFrameCount;         // number of valid entries per facing in the Pixel16 ** variables
};

#endif
