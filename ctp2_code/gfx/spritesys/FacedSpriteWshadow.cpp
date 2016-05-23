//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Sprite with facings and shadows.
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
// None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added separate counters in Sprite-derived classes to prevent crashes.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "FacedSpriteWshadow.h"

#include "tiffutils.h"
#include "pixelutils.h"
#include "spriteutils.h"
#include "c3errors.h"
#include "screenmanager.h"

#include "aui.h"
#include "aui_surface.h"
#include "aui_directsurface.h"

#include "Sprite.h"

#include "Token.h"

extern ScreenManager *g_screenManager;


FacedSpriteWshadow::FacedSpriteWshadow()
:   Sprite              (),
    m_hasShadow         (TRUE),
    m_shadowFrameCount  (0)
{
	for (size_t facing = 0; facing < k_NUM_FACINGS; ++facing)
	{
		m_frames[facing] = NULL;
		m_framesSizes[facing] = NULL;
		m_miniframes[facing] = NULL;
		m_miniframesSizes[facing] = NULL;
		m_shadowFrames[facing] = NULL;
		m_shadowFramesSizes[facing] = NULL;
		m_miniShadowFrames[facing] = NULL;
		m_miniShadowFramesSizes[facing] = NULL;
	}
	m_type = SPRITETYPE_FACEDWSHADOW;
}


FacedSpriteWshadow::~FacedSpriteWshadow()
{

	for (sint32 facing=0; facing < k_NUM_FACINGS; facing++)
	{
		if (m_frames[facing])
		{
			for (size_t n = 0; n < m_shadowFrameCount; n++) {
				if (m_frames[facing][n])
					delete m_frames[facing][0];

				m_frames[facing][n] = 0;
			}
			delete[] m_frames[facing];
			m_frames[facing] = 0;
		}
		if (m_framesSizes[facing])
		{
			delete[] m_framesSizes[facing];
			m_framesSizes[facing] = 0;
		}
		if (m_miniframes[facing])
		{
			for (size_t n = 0; n < m_shadowFrameCount; n++) {
				if (m_miniframes[facing][n])
					delete m_miniframes[facing][0];

				m_miniframes[facing][n] = 0;
			}

			delete[] m_miniframes[facing];
			m_miniframes[facing] = 0;
		}
		if (m_miniframesSizes[facing])
		{
			delete[] m_miniframesSizes[facing];
			m_miniframesSizes[facing] = 0;
		}
		if (m_shadowFrames[facing])
		{
			for (size_t n = 0; n < m_shadowFrameCount; n++) {
				if (m_shadowFrames[facing][n])
					delete m_shadowFrames[facing][0];

				m_shadowFrames[facing][n] = 0;
			}

			delete[] m_shadowFrames[facing];
			m_shadowFrames[facing] = 0;
		}
		if (m_shadowFramesSizes[facing] != NULL)
		{
			delete[] m_shadowFramesSizes[facing];
			m_shadowFramesSizes[facing] = 0;
		}
		if (m_miniShadowFrames[facing] != NULL)
		{
			for (size_t n = 0; n < m_shadowFrameCount; n++) {
				if (m_miniShadowFrames[facing][n])
					delete m_miniShadowFrames[facing][0];

				m_miniShadowFrames[facing][n] = 0;
			}

			delete[] m_miniShadowFrames[facing];
			m_miniShadowFrames[facing] = 0;
		}
		if (m_miniShadowFramesSizes[facing] != NULL)
		{
			delete[] m_miniShadowFramesSizes[facing];
			m_miniShadowFramesSizes[facing] = 0;
		}
	}
}

void FacedSpriteWshadow::Import(uint16 nframes, char *imageFiles[k_NUM_FACINGS][k_MAX_NAMES], char *shadowFiles[k_NUM_FACINGS][k_MAX_NAMES])
{
	AllocateFrameArrays(nframes);

	for (sint32 facing=0; facing < k_NUM_FACINGS; facing++)
	{
		for (sint32 i=0; i < nframes; i++)
		{
			size_t tifsize = 0;
			char *tif = StripTIF2Mem(imageFiles[facing][i], &m_width, &m_height, &tifsize);
			if (tif)
			{
				char *minitif = NULL;
				char *minishadow = NULL; // Not used

				spriteutils_CreateQuarterSize((Pixel32 *)tif, m_width, m_height, (Pixel32 **)&minitif, TRUE);

				size_t framesize = 0;
				m_frames[facing][i]     = spriteutils_RGB32ToEncoded((Pixel32 *)tif, m_width, m_height, &framesize);
				m_framesSizes[facing][i] = (m_frames[facing][i] ? framesize : 0);
		                m_miniframes[facing][i] = spriteutils_RGB32ToEncoded((Pixel32 *)minitif, m_width >> 1, m_height >> 1, &framesize);
		                m_miniframesSizes[facing][i] = (m_miniframes[facing][i] ? framesize : 0);

				uint16 width;
				uint16 height;
				char *shadowTif = StripTIF2Mem(shadowFiles[facing][i], &width, &height);

				if (shadowTif)
				{
					m_shadowFrames[facing][i] = spriteutils_RGB32ToEncoded((Pixel32 *)shadowTif, m_width, m_height, &framesize);
					m_shadowFramesSizes[facing][i] = (m_shadowFrames[facing][i] ? framesize : 0);

					char *minishadow = NULL;
					spriteutils_CreateQuarterSize((Pixel32 *)shadowTif, m_width, m_height, (Pixel32 **)&minishadow, FALSE);

					if(minishadow)
					{
						m_miniShadowFrames[facing][i] =
							spriteutils_RGB32ToEncoded((Pixel32 *)minishadow, m_width >> 1, m_height >> 1, &framesize);
						m_miniShadowFramesSizes[facing][i] = (m_miniShadowFrames[facing][i] ? framesize : 0);
					}

					free(shadowTif);
				}

				free(tif);
			}

			printf(".");
		}
	}
}




void FacedSpriteWshadow::Import(uint16 nframes, char *imageFiles[k_NUM_FACINGS][k_MAX_NAMES])
{
	AllocateFrameArrays(nframes);

	for (sint32 facing=0; facing < k_NUM_FACINGS; facing++)
	{
		for (sint32 i=0; i < nframes; i++)
		{
			char *tif = StripTIF2Mem(imageFiles[facing][i], &m_width, &m_height);

			if (tif)
			{
				char *minitif = NULL;
				spriteutils_CreateQuarterSize((Pixel32 *)tif, m_width, m_height, (Pixel32 **)&minitif, TRUE);

				size_t framesize = 0;
				m_frames[facing][i]     = spriteutils_RGB32ToEncoded((Pixel32 *)tif, m_width, m_height, &framesize);
				m_framesSizes[facing][i] = (m_frames[facing][i] ? framesize : 0);
				m_miniframes[facing][i] = spriteutils_RGB32ToEncoded((Pixel32 *)minitif, m_width >> 1, m_height >> 1, &framesize);
				m_miniframesSizes[facing][i] = (m_miniframes[facing][i] ? framesize : 0);

				free(tif);
			}

			printf(".");
		}
	}
}

void FacedSpriteWshadow::Draw(sint32 drawX, sint32 drawY, sint32 facing,
					   double scale, sint16 transparency, Pixel16 outlineColor, uint16 flags)
{
	SetSurface();

	if (facing < 5)
	{
		drawX -= (sint32)((double)m_hotPoints[facing].x * scale);
		drawY -= (sint32)((double)m_hotPoints[facing].y * scale);
	}
	else
	{
		drawX -=  (m_width - (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].x * scale));
		drawY -= (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].y * scale);
	}

	if(m_frames[facing][m_currentFrame] == NULL)
	{
		facing = k_SPRITE_BASEFACING;
	}
	Assert(m_frames[facing][m_currentFrame] != NULL);
	if(m_frames[facing][m_currentFrame] == NULL)
	{
		c3errors_ErrorDialog("FacedSpriteWshadow", "base sprite facing is missing or invalid for Draw");
		return;
	}




	if (drawX > g_screenManager->GetSurfWidth() - (m_width*scale) || drawX < 0) return;
	if (drawY > g_screenManager->GetSurfHeight() - (m_height*scale) || drawY < 0) return;

	if (scale == 1.0)
	{
		if (facing < 5)
		{
			(this->*_DrawLow)((Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);

		}
		else
			(this->*_DrawLowReversed)((Pixel16 *)m_frames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
	}
	else
	{
		if (scale == 0.5)
		{
			if (facing < 5)
				(this->*_DrawLow)((Pixel16 *)m_miniframes[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			else
				(this->*_DrawLowReversed)((Pixel16 *)m_miniframes[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
		}
		else
		{

			sint32 destWidth = (sint32)(m_width * scale);
			sint32 destHeight = (sint32)(m_height * scale);

			if (facing < 5)
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, FALSE);
			}
			else
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_frames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, TRUE);
			}
		}
	}
}

void FacedSpriteWshadow::DrawDirect(aui_Surface *surf, sint32 drawX, sint32 drawY, sint32 facing,
					   double scale, sint16 transparency, Pixel16 outlineColor, uint16 flags)
{
	LockSurface(surf);

	if (facing < 5)
	{
		drawX -= (sint32)((double)m_hotPoints[facing].x * scale);
		drawY -= (sint32)((double)m_hotPoints[facing].y * scale);
	}
	else
	{
		drawX -=  (m_width - (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].x * scale));
		drawY -= (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].y * scale);
	}

	if(m_frames[facing][m_currentFrame] == NULL)
	{
		facing = k_SPRITE_BASEFACING;
	}
	Assert(m_frames[facing][m_currentFrame] != NULL);
	if(m_frames[facing][m_currentFrame] == NULL)
	{
		c3errors_ErrorDialog("FacedSpriteWshadow", "base sprite facing is missing or invalid for Draw");
		UnlockSurface();
		return;
	}




	if (drawX > surf->Width() - (m_width*scale) || drawX < 0) {
		UnlockSurface();
		return;
	}
	if (drawY > surf->Height() - (m_height*scale) || drawY < 0) {
		UnlockSurface();
		return;
	}

	if (scale == 1.0)
	{
		if (facing < 5)
		{
			(this->*_DrawLow)((Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);

		}
		else {
			(this->*_DrawLowReversed)((Pixel16 *)m_frames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
		}
	}
	else
	{
		if (scale == 0.5)
		{
			if (facing < 5)
				(this->*_DrawLow)((Pixel16 *)m_miniframes[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			else {
				(this->*_DrawLowReversed)((Pixel16 *)m_miniframes[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			}
		}
		else
		{

			sint32 destWidth = (sint32)(m_width * scale);
			sint32 destHeight = (sint32)(m_height * scale);

			if (facing < 5)
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, FALSE);
			}
			else
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_frames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, TRUE);
			}
		}
	}
	UnlockSurface();

}




void FacedSpriteWshadow::DirectionalDraw(sint32 drawX, sint32 drawY, sint32 facing,
					   double scale, sint16 transparency, Pixel16 outlineColor, uint16 flags)
{
	SetSurface();

	if (facing < 5)
	{
		drawX -= (sint32)((double)m_hotPoints[facing].x * scale);
		drawY -= (sint32)((double)m_hotPoints[facing].y * scale);
	}
	else
	{
		drawX -=  (m_width - (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].x * scale));
		drawY -= (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].y * scale);
	}

	if(m_frames[facing][m_currentFrame] == NULL)
	{
		facing = k_SPRITE_BASEFACING;
	}

	Assert(m_frames[facing][m_currentFrame] != NULL);
	if(m_frames[facing][m_currentFrame] == NULL)
	{
		c3errors_ErrorDialog("FacedSpriteWshadow", "base sprite facing is missing or invalid for Directional Draw");
		return;
	}


	if (drawX > g_screenManager->GetSurfWidth() - (m_width*scale) || drawX < 0) return;
	if (drawY > g_screenManager->GetSurfHeight() - (m_height*scale) || drawY < 0) return;

	if (scale == 1.0)
	{
		if (facing < 4 && facing > 0)
		{
			(this->*_DrawLow)((Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);

		}
		else if (facing == 4 || facing == 0)
		{
			(this->*_DrawLowReversed)((Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
		}
		else
		{
			(this->*_DrawLowReversed)((Pixel16 *)m_frames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
		}
	}
	else
	{
		if (scale == 0.5)
		{
			if (facing < 4 && facing > 0)
			{
				(this->*_DrawLow)((Pixel16 *)m_miniframes[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			}
			else if (facing == 4 || facing == 0)
			{
				(this->*_DrawLowReversed)((Pixel16 *)m_miniframes[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			}
			else
			{
				(this->*_DrawLowReversed)((Pixel16 *)m_miniframes[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			}
		}
		else
		{

			sint32 destWidth = (sint32)(m_width * scale);
			sint32 destHeight = (sint32)(m_height * scale);

			if (facing < 4 && facing > 0)
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, FALSE);
			}
			else if(facing == 4 || facing == 0)
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, TRUE);
			}
			else
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_frames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, TRUE);
			}
		}
	}
}




void FacedSpriteWshadow::DrawShadow(sint32 drawX, sint32 drawY, sint32 facing,
					   double scale, sint16 transparency, Pixel16 outlineColor, uint16 flags)
{
	SetSurface();

	if (facing < 5)
	{
		drawX -= (sint32)((double)m_hotPoints[facing].x * scale);
		drawY -= (sint32)((double)m_hotPoints[facing].y * scale);
	}
	else
	{
		drawX -=  (m_width - (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].x * scale));
		drawY -= (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].y * scale);
	}

	if(m_shadowFrames[facing][m_currentFrame] == NULL)
	{
		facing = k_SPRITE_BASEFACING;
	}
	if(m_shadowFrames[facing][m_currentFrame] == NULL)
	{

		return;
	}


	if (drawX > g_screenManager->GetSurfWidth() - (m_width*scale) || drawX < 0) return;
	if (drawY > g_screenManager->GetSurfHeight() - (m_height*scale) || drawY < 0) return;

	if (scale == 1.0)
	{
		if (facing < 5)
		{
			(this->*_DrawLow)((Pixel16 *)m_shadowFrames[facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);

		}
		else
			(this->*_DrawLowReversed)((Pixel16 *)m_shadowFrames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
	}
	else
	{
		if (scale == 0.5)
		{
			if (facing < 5)
				(this->*_DrawLow)((Pixel16 *)m_miniShadowFrames[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			else
				(this->*_DrawLowReversed)((Pixel16 *)m_miniShadowFrames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
		}
		else
		{

			sint32 destWidth = (sint32)(m_width * scale);
			sint32 destHeight = (sint32)(m_height * scale);

			if (facing < 5)
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_shadowFrames[facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, FALSE);
			}
			else
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_shadowFrames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, TRUE);
			}
		}
	}
}




void FacedSpriteWshadow::DrawFlashEffect(sint32 drawX, sint32 drawY, sint32 facing,
					   double scale, sint16 transparency, Pixel16 outlineColor, uint16 flags)
{
	SetSurface();

	if (facing < 5)
	{
		drawX -= (sint32)((double)m_hotPoints[facing].x * scale);
		drawY -= (sint32)((double)m_hotPoints[facing].y * scale);
	}
	else
	{
		drawX -=  (m_width - (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].x * scale));
		drawY -= (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].y * scale);
	}

	if(m_frames[facing][m_currentFrame] == NULL)
	{
		facing = k_SPRITE_BASEFACING;
	}

	Assert(m_frames[facing][m_currentFrame] != NULL);
	if(m_frames[facing][m_currentFrame] == NULL)
	{
		c3errors_ErrorDialog("FacedSpriteWshadow", "base sprite facing is missing or invalid Draw Flash Effect");
		return;
	}


	if (drawX > g_screenManager->GetSurfWidth() - (m_width*scale) || drawX < 0) return;
	if (drawY > g_screenManager->GetSurfHeight() - (m_height*scale) || drawY < 0) return;

	if (scale == 1.0)
	{
		if (facing < 5)
		{
			(this->*_DrawFlashLow)((Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
		}
		else
		{
			(this->*_DrawFlashLowReversed)((Pixel16 *)m_frames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
		}
	}
	else
	{
		if (scale == 0.5)
		{
			if (facing < 5)
			{
				(this->*_DrawFlashLow)((Pixel16 *)m_miniframes[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			}
			else
			{
				(this->*_DrawFlashLowReversed)((Pixel16 *)m_miniframes[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			}
		}
		else
		{

			sint32 destWidth = (sint32)(m_width * scale);
			sint32 destHeight = (sint32)(m_height * scale);

			if (facing < 5)
			{
				(this->*_DrawFlashScaledLow)((Pixel16 *)m_frames[facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
					transparency, outlineColor, flags, FALSE);
			}
			else
			{
				(this->*_DrawFlashScaledLow)((Pixel16 *)m_frames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
					transparency, outlineColor, flags, TRUE);
			}
		}
	}
}





void FacedSpriteWshadow::DirectionalDrawShadow(sint32 drawX, sint32 drawY, sint32 facing,
					   double scale, sint16 transparency, Pixel16 outlineColor, uint16 flags)
{
	SetSurface();

	if (facing < 5)
	{
		drawX -= (sint32)((double)m_hotPoints[facing].x * scale);
		drawY -= (sint32)((double)m_hotPoints[facing].y * scale);
	}
	else
	{
		drawX -=  (m_width - (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].x * scale));
		drawY -= (sint32)((double)m_hotPoints[k_MAX_FACINGS - facing].y * scale);
	}

	if(m_shadowFrames[facing][m_currentFrame] == NULL)
	{
		facing = k_SPRITE_BASEFACING;
	}

	if(m_shadowFrames[facing][m_currentFrame] == NULL)
	{
		return;
	}


	if (drawX > g_screenManager->GetSurfWidth() - (m_width*scale) || drawX < 0) return;
	if (drawY > g_screenManager->GetSurfHeight() - (m_height*scale) || drawY < 0) return;

	if (scale == 1.0)
	{
		if (facing < 4 && facing > 0)
		{
			(this->*_DrawLow)((Pixel16 *)m_shadowFrames[facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);

		}
		else if (facing == 4 || facing == 0)
		{
			(this->*_DrawLowReversed)((Pixel16 *)m_shadowFrames[facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
		}
		else
		{
			(this->*_DrawLowReversed)((Pixel16 *)m_shadowFrames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
		}
	}
	else
	{
		if (scale == 0.5)
		{
			if (facing < 4 && facing > 0)
			{
				(this->*_DrawLow)((Pixel16 *)m_miniShadowFrames[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			}
			else if (facing == 4 || facing == 0)
			{
				(this->*_DrawLowReversed)((Pixel16 *)m_miniShadowFrames[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			}
			else
			{
				(this->*_DrawLowReversed)((Pixel16 *)m_miniShadowFrames[facing][m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			}
		}
		else
		{

			sint32 destWidth = (sint32)(m_width * scale);
			sint32 destHeight = (sint32)(m_height * scale);

			if (facing < 4 && facing > 0)
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_shadowFrames[facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, FALSE);
			}
			else if(facing == 4 || facing == 0)
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_shadowFrames[facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, TRUE);
			}
			else
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_shadowFrames[k_MAX_FACINGS - facing][m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, TRUE);
			}
		}
	}
}


sint32 FacedSpriteWshadow::ParseFromTokens(Token *theToken)
{
	sint32		tmp;
	sint32		i;

	if (!token_ParseAnOpenBraceNext(theToken)) return FALSE;

	if (!token_ParseValNext(theToken, TOKEN_SPRITE_NUM_FRAMES, tmp)) return FALSE;
	m_numFrames = (uint16)tmp;

	if (!token_ParseValNext(theToken, TOKEN_SPRITE_FIRST_FRAME, tmp)) return FALSE;
	m_firstFrame = (uint16)tmp;

	if (!token_ParseValNext(theToken, TOKEN_SPRITE_WIDTH, tmp)) return FALSE;
	m_width = (uint16)tmp;

	if (!token_ParseValNext(theToken, TOKEN_SPRITE_HEIGHT, tmp)) return FALSE;
	m_height = (uint16)tmp;

	if (!token_ParseKeywordNext(theToken, TOKEN_SPRITE_HOT_POINTS)) return FALSE;
	for (i=0; i<k_NUM_FACINGS; i++)
	{
		POINT		p;

		if (theToken->Next() == TOKEN_NUMBER) theToken->GetNumber(tmp);
		else return FALSE;

		p.x = tmp;

		if (theToken->Next() == TOKEN_NUMBER) theToken->GetNumber(tmp);
		else return FALSE;

		p.y = tmp;

		m_hotPoints[i] = p;
	}

	if (!token_ParseAnCloseBraceNext(theToken)) return FALSE;

	return TRUE;
}

//----------------------------------------------------------------------------
//
// Name       : FacedSpriteWshadow::AllocateFrameArrays
//
// Description: Allocate memory for the faced sprites.
//
// Parameters : count   : number of sprites per facing to reserve
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Assumption: Memory has not been allocated before.
//
//----------------------------------------------------------------------------
void FacedSpriteWshadow::AllocateFrameArrays(size_t count)
{
	Assert(0 == m_shadowFrameCount);

	for (size_t facing = 0; facing < k_NUM_FACINGS; ++facing)
	{
		m_frames[facing]                = new Pixel16 *[count];
		m_framesSizes[facing]           = new size_t   [count];
		m_miniframes[facing]            = new Pixel16 *[count];
		m_miniframesSizes[facing]       = new size_t   [count];
		m_shadowFrames[facing]          = new Pixel16 *[count];
		m_shadowFramesSizes[facing]     = new size_t   [count];
		m_miniShadowFrames[facing]      = new Pixel16 *[count];
		m_miniShadowFramesSizes[facing] = new size_t   [count];

		for (size_t i = 0; i < count; ++i)
		{
			m_frames[facing][i]                = 0;
			m_framesSizes[facing][i]           = 0;
			m_miniframes[facing][i]            = 0;
			m_miniframesSizes[facing][i]       = 0;
			m_shadowFrames[facing][i]          = 0;
			m_shadowFramesSizes[facing][i]     = 0;
			m_miniShadowFrames[facing][i]      = 0;
			m_miniShadowFramesSizes[facing][i] = 0;
		}
	}

	m_shadowFrameCount = count;
}

size_t FacedSpriteWshadow::GetFrameDataSize(uint16 facing, uint16 frame)
{
	Assert(facing < k_NUM_FACINGS);
	Assert(frame < m_numFrames);

	if (!m_framesSizes[facing])
		return 0;

	if (frame >= m_numFrames)
		return 0;
	return m_framesSizes[facing][frame];
}

size_t FacedSpriteWshadow::GetMiniFrameDataSize(uint16 facing, uint16 frame)
{
	Assert(facing < k_NUM_FACINGS);
	Assert(frame < m_numFrames);

	if (!m_miniframesSizes[facing])
		return 0;

	if (frame >= m_numFrames)
		return 0;

	return m_miniframesSizes[facing][frame];
}

size_t FacedSpriteWshadow::GetShadowFrameDataSize(uint16 facing, uint16 frame)
{
	Assert(facing < k_NUM_FACINGS);
	Assert(frame < m_numFrames);

	if (!m_shadowFramesSizes[facing])
		return 0;

	if (frame >= m_numFrames)
		return 0;

	return m_shadowFramesSizes[facing][frame];
}

size_t FacedSpriteWshadow::GetMiniShadowFrameDataSize(uint16 facing, uint16 frame)
{
	Assert(facing < k_NUM_FACINGS);
	Assert(frame < m_numFrames);

	if (!m_miniShadowFramesSizes[facing])
		return 0;

	if (frame >= m_numFrames)
		return 0;

	return m_miniShadowFramesSizes[facing][frame];
}
