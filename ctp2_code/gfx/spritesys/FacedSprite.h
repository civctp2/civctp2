
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __FACEDSPRITE_H__
#define __FACEDSPRITE_H__

#define k_MAX_FACINGS		8
#define k_NUM_FACINGS		5

#define k_MAX_NAMES			100
#define k_MAX_NAME_LENGTH	_MAX_PATH

#include "Sprite.h"

class Token;

class FacedSprite : public Sprite {
public:
	FacedSprite();
	virtual ~FacedSprite();

	void			Import(uint16 nframes, char *files[k_NUM_FACINGS][k_MAX_NAMES], char *shadowFiles[k_NUM_FACINGS][k_MAX_NAMES]);
	void			Export(FILE *file);

	void			Load(char *filename);

	virtual void	Draw(sint32 drawX, sint32 drawY, sint32 facing, double m_scale, sint16 transparency, 
						Pixel16 outlineColor, uint16 flags);
	virtual void	DirectionalDraw(sint32 drawX, sint32 drawY, sint32 facing,
					   double scale, sint16 transparency, Pixel16 outlineColor, uint16 flags);
	virtual void	DrawDirect(aui_Surface *surf, sint32 drawX, sint32 drawY, sint32 facing, double m_scale, sint16 transparency, 
						Pixel16 outlineColor, uint16 flags);

	virtual BOOL	HitTest(POINT mousePt, sint32 drawX, sint32 drawY, sint32 facing, double m_scale, sint16 transparency, 
						Pixel16 outlineColor, uint16 flags);

	Pixel16			*GetFrameData(uint16 facing, uint16 frame) { return m_frames[facing][frame]; }
	size_t			GetFrameDataSize(uint16 facing, uint16 frame) { return m_framesSizes[facing][frame]; }
	Pixel16			*GetMiniFrameData(uint16 facing, uint16 frame) { return m_miniframes[facing][frame]; }
	size_t			GetMiniFrameDataSize(uint16 facing, uint16 frame) { return m_miniframesSizes[facing][frame]; }
	void			SetFrameData(uint16 facing, uint16 frame, Pixel16 *data, size_t size);
	void			SetMiniFrameData(uint16 facing, uint16 frame, Pixel16 *data, size_t size);

	POINT			GetHotPoint(uint16 facing) { return m_hotPoints[facing]; }
	POINT			*GetHotPoints(void) { return m_hotPoints; }
	void			SetHotPoint(uint16 facing, sint32 x, sint32 y) { m_hotPoints[facing].x = x; m_hotPoints[facing].y = y; }
	void			SetHotPoints(POINT *points) { memcpy(m_hotPoints, points, sizeof(m_hotPoints)); }

	sint32			ParseFromTokens(Token *theToken);
	void			AllocateFrameArrays(void);

protected:
	Pixel16			**m_frames[k_NUM_FACINGS];
	size_t			*m_framesSizes[k_NUM_FACINGS];
	Pixel16			**m_miniframes[k_NUM_FACINGS];
	size_t			*m_miniframesSizes[k_NUM_FACINGS];
	POINT			m_hotPoints[k_NUM_FACINGS];
};

#endif
