//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Sprite with facings
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
// - Added Get*Size() methods to increase portability (_msize=windows api)
// - Added size argument to Set*Data methods for increasing portability
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __FACEDSPRITE_H__
#define __FACEDSPRITE_H__

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

// #include <>

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class FacedSprite;

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

class FacedSprite : public Sprite
{
public:
	FacedSprite();
	virtual ~FacedSprite();

	void			Import(size_t nframes, char *files[k_NUM_FACINGS][k_MAX_NAMES], char *shadowFiles[k_NUM_FACINGS][k_MAX_NAMES]);
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

	Pixel16			*GetFrameData(uint16 facing, uint16 frame);
	size_t			GetFrameDataSize(uint16 facing, uint16 frame);
	Pixel16			*GetMiniFrameData(uint16 facing, uint16 frame);
	size_t			GetMiniFrameDataSize(uint16 facing, uint16 frame);
	void			SetFrameData(uint16 facing, uint16 frame, Pixel16 *data, size_t size);
	void			SetMiniFrameData(uint16 facing, uint16 frame, Pixel16 *data, size_t size);

	POINT			GetHotPoint(uint16 facing) { return m_hotPoints[facing]; }
	POINT			*GetHotPoints(void) { return m_hotPoints; }
	void			SetHotPoint(uint16 facing, sint32 x, sint32 y) { m_hotPoints[facing].x = x; m_hotPoints[facing].y = y; }
	void			SetHotPoints(POINT *points) { memcpy(m_hotPoints, points, sizeof(m_hotPoints)); }

	virtual sint32	ParseFromTokens(Token *theToken);
	virtual void	AllocateFrameArrays(size_t count);

	virtual size_t	GetNumFrames(void) const    { return m_facedFrameCount; };
	virtual void	SetNumFrames(uint16 num)    { m_facedFrameCount = num; }

protected:
	Pixel16			**m_frames[k_NUM_FACINGS];
	size_t			*m_framesSizes[k_NUM_FACINGS];
	Pixel16			**m_miniframes[k_NUM_FACINGS];
	size_t			*m_miniframesSizes[k_NUM_FACINGS];
	POINT			m_hotPoints[k_NUM_FACINGS];
    size_t          m_facedFrameCount;          // number of valid entries per facing in the Pixel16 ** variables
};

#endif
