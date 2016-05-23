//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Sprite
// Id           : $Id$
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
// - None
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

#include "c3.h"
#include "Sprite.h"

#include "pixelutils.h"
#include "tiffutils.h"
#include "targautils.h"
#include "spriteutils.h"
#include "primitives.h"

#include "aui.h"
#include "aui_surface.h"

#include "SpriteFile.h"
#include "Anim.h"
#include "screenmanager.h"

#include "tiledmap.h"

#include "Token.h"

extern sint32 g_is565Format;
extern ScreenManager *g_screenManager;
extern TiledMap		*g_tiledMap;


Sprite::Sprite()
{
	m_width = 0;
	m_height = 0;

	m_hotPoint.x = 0;
	m_hotPoint.y = 0;

	m_numFrames = 0;
	m_frames = NULL;
	m_framesSizes = NULL;
	m_miniframes = NULL;
	m_miniframesSizes = NULL;
	m_currentFrame = 0;
	m_firstFrame = 0;

	m_type = SPRITETYPE_NORMAL;

	m_surface = NULL;
	m_surfBase = NULL;
	m_surfWidth = 0;
	m_surfHeight = 0;
	m_surfPitch = 0;

	InitializeDrawLow();
}


Sprite::~Sprite()
{
	for (size_t i = 0; i < m_numFrames; ++i)
	{
		if (m_frames[i] != NULL) {
			delete [] (m_frames[i]);
			m_frames[i] = NULL;
		}
		if (m_miniframes[i] != NULL) {
			delete [] (m_miniframes[i]);
			m_miniframes[i] = NULL;
		}
	}
	if (m_frames != NULL) {
		delete [] m_frames;
		m_frames = NULL;
	}
	if (m_framesSizes != NULL) {
		delete [] m_framesSizes;
		m_framesSizes = NULL;
	}
	if (m_miniframes != NULL) {
		delete [] m_miniframes;
		m_miniframes = NULL;
	}
	if (m_miniframesSizes != NULL) {
		delete [] m_miniframesSizes;
		m_miniframesSizes = NULL;
	}
}


void Sprite::Load(char const * filename)
{
}




void Sprite::Save(char const * filename)
{
	SpriteFile *file = new SpriteFile(filename);
	Anim		*a = new Anim();

	file->Create(SPRITEFILETYPE_PLAIN,k_SPRITEFILE_VERSION0,SPRDATA_REGULAR);
	file->Write(this, a);
	file->CloseWrite();

	delete file;
	delete a;
}




void Sprite::ImportTIFF(uint16 index, char **imageFiles,Pixel32 **imageData, size_t *size)
{

		*imageData = (Pixel32 *)StripTIF2Mem(imageFiles[index], &m_width, &m_height, size);
}
#if 0

		if (tif)
		{
			uint16	width, height;

			spriteutils_CreateQuarterSize((Pixel32 *)tif, m_width, m_height, (Pixel32 **)&minitif, TRUE);

			char *shadowTif = StripTIF2Mem(shadowFiles[index], &width, &height);

			if (shadowTif)
			{

				spriteutils_CreateQuarterSize((Pixel32 *)shadowTif, m_width, m_height, (Pixel32 **)&minishadow, FALSE);
			}

			size_t size;
			Pixel16 *frame = spriteutils_RGB32ToEncoded((Pixel32 *)tif, (Pixel32 *)shadowTif, m_width, m_height, &size);
			if (frame) {
				SetFrameData(index, frame, size);
			}

			Pixel16	*miniframe = spriteutils_RGB32ToEncoded((Pixel32 *)minitif, (Pixel32 *)minishadow, m_width >> 1, m_height >> 1, &size);
			if (miniframe) {
				SetMiniFrameData(index, miniframe, size);
			}

			if (shadowTif)  free (shadowTif);
			if (minishadow) free (minishadow);

			if (tif) free(tif);
			if (minitif) free(minitif);
		}
		else
		{
			printf("Could not find %s.\n", imageFiles[index]);
		}
}
#endif




void Sprite::ImportTGA(uint16 index, char **imageFiles,Pixel32 **imageData, size_t *size)
{
	int		bpp;
	int     w,h;

	if (!Get_TGA_Dimension(imageFiles[index], w, h, bpp))
	{
		printf("Bad TGA Sprite File(%s)\n",imageFiles[index]);
		*imageData = NULL;
		fcloseall();
		exit(0);
		return;
	}

	if (bpp!=4)
	{
		printf("TGA Sprite File not 32-bits(%s)\n",imageFiles[index]);
		*imageData=NULL;
		fcloseall();
		exit(0);
		return;
	}

	*imageData = new Pixel32[w*h];
	if (size)
		*size = w * h * sizeof(Pixel32);

	Load_TGA_File_Simple(imageFiles[index],(unsigned char *)*imageData,w*sizeof(Pixel32),w,h);

	m_width  = (uint16)w;
	m_height = (uint16)h;

	TGA2RGB32((Pixel32 *)*imageData,w*h);
}





void Sprite::Import(size_t nframes, char **imageFiles, char **shadowFiles)
{
	m_numFrames = static_cast<uint16>(nframes);

	if (m_frames) {
		delete [] m_frames;
	}
	m_frames = new Pixel16*[m_numFrames];
	if (m_framesSizes) {
		delete [] m_framesSizes;
	}
	m_framesSizes = new size_t[m_numFrames];

	if (m_miniframes) {
		delete [] m_miniframes;
	}
	m_miniframes = new Pixel16*[m_numFrames];
	if (m_miniframesSizes) {
		delete [] m_miniframesSizes;
	}
	m_miniframesSizes = new size_t[m_numFrames];

	Pixel32 *image,*miniimage;
	size_t   imageSize, miniimageSize;
	Pixel32 *shadow,*minishadow;
	size_t   shadowSize, minishadowSize;

	for (uint16 i=0; i<m_numFrames; i++)
	{
		char ext[_MAX_DIR];

		Pixel16 *data   = NULL;
		size_t dataSize = 0;
		image		= NULL;
		imageSize       = 0;
		miniimage	= NULL;
		miniimageSize   = 0;
		shadow		= NULL;
		shadowSize      = 0;
		minishadow	= NULL;
		minishadowSize  = 0;

		_splitpath(imageFiles[i],NULL,NULL,NULL,ext);

		if (strstr(strupr(ext),"TIF"))
			ImportTIFF(i,imageFiles,&image,&imageSize);
		else
			if (strstr(strupr(ext),"TGA"))
				ImportTGA(i,imageFiles,&image, &imageSize);
			else
			{
				printf("Unknown image file \"%s\"\n",imageFiles[i]);
				fcloseall();
				exit(-1);
			}

		_splitpath(shadowFiles[i],NULL,NULL,NULL,ext);

		if (strstr(strupr(ext),"TIF"))
			ImportTIFF(i,shadowFiles,&shadow, &shadowSize);
		else
			if (strstr(strupr(ext),"TGA"))
				ImportTGA(i,shadowFiles,&shadow, &shadowSize);

		if (image)
		{



































			spriteutils_CreateQuarterSize(image, m_width, m_height,&miniimage, TRUE);

			data = spriteutils_RGB32ToEncoded(image,shadow, m_width, m_height, &dataSize);
			SetFrameData(i, data, dataSize);

			if (shadow)
				spriteutils_CreateQuarterSize(shadow, m_width, m_height,&minishadow, FALSE);

			data = spriteutils_RGB32ToEncoded(miniimage, minishadow, m_width >> 1, m_height >> 1, &dataSize);
			SetMiniFrameData(i, data, dataSize);
		}

		if (image) 		delete []image;
		if (shadow)		delete []shadow;
		if (miniimage) 	delete []miniimage;
		if (minishadow)	delete []minishadow;

		printf(".");
	}
}

void Sprite::LockSurface(aui_Surface *surf)
{
	uint8 *         buffer;
	AUI_ERRCODE		errcode = surf->Lock(NULL, (LPVOID *)&buffer, 0);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return;

	m_surface = surf;
	m_surfBase = buffer;
	m_surfWidth = surf->Width();
	m_surfHeight = surf->Height();
	m_surfPitch = surf->Pitch();
}

void Sprite::UnlockSurface(void)
{
	AUI_ERRCODE		errcode = m_surface->Unlock(m_surfBase);
	Assert(errcode == AUI_ERRCODE_OK);

	m_surfBase = NULL;
	m_surface = NULL;
	m_surfWidth = 0;
	m_surfHeight = 0;
	m_surfPitch = 0;
}

void Sprite::SetSurface(void)
{
	m_surface = g_screenManager->GetSurface();
	m_surfBase = g_screenManager->GetSurfBase();
	m_surfWidth = g_screenManager->GetSurfWidth();
	m_surfHeight = g_screenManager->GetSurfHeight();
	m_surfPitch = g_screenManager->GetSurfPitch();
}






void Sprite::InitializeDrawLow()
{
	if (g_is565Format)
	{
		_DrawLowClipped        	= &Sprite::DrawLowClipped565;
		_DrawLow               	= &Sprite::DrawLow565;
		_DrawLowReversedClipped	= &Sprite::DrawLowReversedClipped565;
		_DrawLowReversed       	= &Sprite::DrawLowReversed565;
		_DrawReflectionLow     	= &Sprite::DrawReflectionLow565;
		_DrawFlashLow          	= &Sprite::DrawFlashLow565;
		_DrawFlashLowReversed  	= &Sprite::DrawFlashLowReversed565;
		_DrawScaledLow			= &Sprite::DrawScaledLow565;
		_DrawFlashScaledLow		= &Sprite::DrawFlashScaledLow565;
	}
	else
	{
		_DrawLowClipped        	= &Sprite::DrawLowClipped555;
		_DrawLow               	= &Sprite::DrawLow555;
		_DrawLowReversedClipped	= &Sprite::DrawLowReversedClipped555;
		_DrawLowReversed       	= &Sprite::DrawLowReversed555;
		_DrawReflectionLow     	= &Sprite::DrawReflectionLow555;
		_DrawFlashLow          	= &Sprite::DrawFlashLow555;
		_DrawFlashLowReversed  	= &Sprite::DrawFlashLowReversed555;
		_DrawScaledLow			= &Sprite::DrawScaledLow555;
		_DrawFlashScaledLow		= &Sprite::DrawFlashScaledLow555;
	}
}


void Sprite::Draw(sint32 drawX, sint32 drawY, sint32 facing, double scale, sint16 transparency, Pixel16 outlineColor, uint16 flags)
{
	SetSurface();

	if (facing < 5) {
		drawX -= (sint32)((double)m_hotPoint.x * scale);
	} else {
		drawX -= (sint32)((double)(m_width-m_hotPoint.x) * scale);
	}

	drawY -= (sint32)((double)m_hotPoint.y * scale);

	if (scale == g_tiledMap->GetZoomScale(k_ZOOM_LARGEST)) {
		if (facing < 5) {
			if (flags & k_BIT_DRAWFLAGS_ADDITIVE) {
				(this->*_DrawFlashLow)(m_frames[m_currentFrame], drawX, drawY,  m_width, m_height,transparency, outlineColor, flags);
			} else {
				(this->*_DrawLow)(m_frames[m_currentFrame], drawX, drawY,  m_width, m_height,transparency, outlineColor, flags);
			}
		} else {
			if (flags & k_BIT_DRAWFLAGS_ADDITIVE) {
				(this->*_DrawFlashLowReversed)(m_frames[m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
			} else {
				(this->*_DrawLowReversed)(m_frames[m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
			}
		}
	} else {
		if (scale == g_tiledMap->GetZoomScale(k_ZOOM_SMALLEST)) {
			if (facing < 5) {
				if (flags & k_BIT_DRAWFLAGS_ADDITIVE) {
					(this->*_DrawFlashLow)(m_miniframes[m_currentFrame], drawX, drawY,  m_width>>1, m_height>>1,transparency, outlineColor, flags);
				} else {
					(this->*_DrawLow)(m_miniframes[m_currentFrame], drawX, drawY,  m_width>>1, m_height>>1,transparency, outlineColor, flags);
				}
			} else {
				if (flags & k_BIT_DRAWFLAGS_ADDITIVE) {
					(this->*_DrawFlashLowReversed)(m_miniframes[m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
				} else {
					(this->*_DrawLowReversed)(m_miniframes[m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
				}
			}
		} else {

			sint32 destWidth = (sint32)(m_width * scale);
			sint32 destHeight = (sint32)(m_height * scale);

			if (facing < 5) {
				if (flags & k_BIT_DRAWFLAGS_ADDITIVE) {
					(this->*_DrawFlashScaledLow)((Pixel16 *)m_frames[m_currentFrame], drawX, drawY, destWidth, destHeight,
										transparency, outlineColor, flags, FALSE);
				} else {
					(this->*_DrawScaledLow)((Pixel16 *)m_frames[m_currentFrame], drawX, drawY, destWidth, destHeight,
										transparency, outlineColor, flags, FALSE);
				}
			} else {
				if (flags & k_BIT_DRAWFLAGS_ADDITIVE) {
					(this->*_DrawFlashScaledLow)((Pixel16 *)m_frames[m_currentFrame], drawX, drawY, destWidth, destHeight,
										transparency, outlineColor, flags, TRUE);
				} else {
					(this->*_DrawScaledLow)((Pixel16 *)m_frames[m_currentFrame], drawX, drawY, destWidth, destHeight,
										transparency, outlineColor, flags, TRUE);
				}
			}
		}
	}
}




sint32 Sprite::ReadTag(sint32 *mode, Pixel16 **rowData, sint32 *alpha)
{
	sint32			len;
	Pixel16		tag = **rowData;

	*mode = (tag & 0x0F00) >> 8;
	(*rowData)++;

	switch (*mode) {
		case k_CHROMAKEY_RUN_ID	:
			len = tag & 0x00FF;
			break;
		case k_COPY_RUN_ID			:
			len = tag & 0x00FF;
			break;
		case k_SHADOW_RUN_ID		:
			len = tag & 0x00FF;
			break;
		case k_FEATHERED_RUN_ID	:
			*alpha = tag & 0x00FF;
			len = 1;
		break;
		default :
			len = 1;
	}
	return len;
}

BOOL Sprite::HitTest(POINT mousePt, sint32 drawX, sint32 drawY, sint32 facing, double scale, sint16 transparency,
					Pixel16 outlineColor, uint16 flags)
{
	if (facing < 5) {
		drawX -= (sint32)((double)m_hotPoint.x * scale);
	} else {
		drawX -= (sint32)((double)(m_width-m_hotPoint.x) * scale);
	}

	drawY -= (sint32)((double)m_hotPoint.y * scale);

	if (scale == g_tiledMap->GetZoomScale(k_ZOOM_LARGEST)) {
		if (facing < 5) {
			return HitTestLow(mousePt, m_frames[m_currentFrame], drawX, drawY,  m_width, m_height,transparency, outlineColor, flags);
		} else {
			return HitTestLowReversed(mousePt, m_frames[m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
		}
	} else {
		if (scale == g_tiledMap->GetZoomScale(k_ZOOM_SMALLEST)) {
			if (facing < 5) {
				return HitTestLow(mousePt, m_miniframes[m_currentFrame], drawX, drawY,  m_width>>1, m_height>>1,transparency, outlineColor, flags);
			} else {
				return HitTestLowReversed(mousePt, m_miniframes[m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			}
		} else {

			sint32 destWidth = (sint32)(m_width * scale);
			sint32 destHeight = (sint32)(m_height * scale);

			if (facing < 5) {
				return HitTestScaledLow(mousePt, (Pixel16 *)m_frames[m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, FALSE);
			} else {
				return HitTestScaledLow(mousePt, (Pixel16 *)m_frames[m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, TRUE);
			}
		}
	}
}




void Sprite::DrawDirect(aui_Surface *surf, sint32 drawX, sint32 drawY, sint32 facing, double scale,
				  sint16 transparency, Pixel16 outlineColor, uint16 flags)
{
	BOOL wasNull = FALSE;

	if (surf == NULL) {
		SetSurface();
		surf = m_surface;
		wasNull = TRUE;
	} else
		LockSurface(surf);

	if (facing < 5) {
		drawX -= (sint32)((double)m_hotPoint.x * scale);
	} else {
		drawX -= (sint32)((double)(m_width-m_hotPoint.x) * scale);
	}
	drawY -= (sint32)((double)m_hotPoint.y * scale);


	if (drawX > surf->Width() - 1 || drawX  < -(m_width*scale)) {
		UnlockSurface();
		return;
	}

	if (drawY > surf->Height() - 1 || drawY  < -(m_height*scale)) {
		UnlockSurface();
		return;
	}

	if (scale == g_tiledMap->GetZoomScale(k_ZOOM_LARGEST)) {
		if (facing < 5) {
			if (flags & k_BIT_DRAWFLAGS_ADDITIVE) {
				(this->*_DrawFlashLow)(m_frames[m_currentFrame], drawX, drawY,  m_width, m_height,transparency, outlineColor, flags);
			} else {
				(this->*_DrawLow)(m_frames[m_currentFrame], drawX, drawY,  m_width, m_height,transparency, outlineColor, flags);
			}
		} else {
			if (flags & k_BIT_DRAWFLAGS_ADDITIVE) {
				(this->*_DrawFlashLowReversed)(m_frames[m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
			} else {
				(this->*_DrawLowReversed)(m_frames[m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
			}
		}
	} else {
		if (scale == g_tiledMap->GetZoomScale(k_ZOOM_SMALLEST)) {
			if (facing < 5) {
				if (flags & k_BIT_DRAWFLAGS_ADDITIVE) {
					(this->*_DrawFlashLow)(m_miniframes[m_currentFrame], drawX, drawY,  m_width>>1, m_height>>1,transparency, outlineColor, flags);
				} else {
					(this->*_DrawLow)(m_miniframes[m_currentFrame], drawX, drawY,  m_width>>1, m_height>>1,transparency, outlineColor, flags);
				}
			} else {
				if (flags & k_BIT_DRAWFLAGS_ADDITIVE) {
					(this->*_DrawFlashLowReversed)(m_miniframes[m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
				} else {
					(this->*_DrawLowReversed)(m_miniframes[m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
				}
			}
		} else {

			sint32 destWidth = (sint32)(m_width * scale);
			sint32 destHeight = (sint32)(m_height * scale);

			if (facing < 5) {
				if (flags & k_BIT_DRAWFLAGS_ADDITIVE) {
					(this->*_DrawFlashScaledLow)((Pixel16 *)m_frames[m_currentFrame], drawX, drawY, destWidth, destHeight,
										transparency, outlineColor, flags, FALSE);
				} else {
					(this->*_DrawScaledLow)((Pixel16 *)m_frames[m_currentFrame], drawX, drawY, destWidth, destHeight,
										transparency, outlineColor, flags, FALSE);
				}
			} else {
				if (flags & k_BIT_DRAWFLAGS_ADDITIVE) {
					(this->*_DrawFlashScaledLow)((Pixel16 *)m_frames[m_currentFrame], drawX, drawY, destWidth, destHeight,
										transparency, outlineColor, flags, TRUE);
				} else {
					(this->*_DrawScaledLow)((Pixel16 *)m_frames[m_currentFrame], drawX, drawY, destWidth, destHeight,
										transparency, outlineColor, flags, TRUE);
				}
			}
		}
	}

	if (surf != NULL && !wasNull)
		UnlockSurface();
}


void Sprite::DirectionalDraw(sint32 drawX, sint32 drawY, sint32 facing, double scale,
				  sint16 transparency, Pixel16 outlineColor, uint16 flags)
{
	SetSurface();

	if (facing < 5) {
		drawX -= (sint32)((double)m_hotPoint.x * scale);
	} else {
		drawX -= (sint32)((double)(m_width-m_hotPoint.x) * scale);
	}
	drawY -= (sint32)((double)m_hotPoint.y * scale);






	if (drawX > m_surfWidth - (m_width*scale) || drawX < 0) return;
	if (drawY > m_surfHeight - (m_height*scale) || drawY < 0) return;

	if (scale == g_tiledMap->GetZoomScale(k_ZOOM_LARGEST)) {
		if (facing < 4 && facing > 0)
		{
			(this->*_DrawLow)(m_frames[m_currentFrame], drawX, drawY,  m_width, m_height,transparency, outlineColor, flags);
		}
		else if (facing == 4 || facing == 0)
		{
			(this->*_DrawLowReversed)(m_frames[m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
		}
		else
		{
			(this->*_DrawLowReversed)(m_frames[m_currentFrame], drawX, drawY, m_width, m_height, transparency, outlineColor, flags);
		}
	} else {
		if (scale == g_tiledMap->GetZoomScale(k_ZOOM_SMALLEST)) {
			if (facing < 4 && facing > 0)
			{
				(this->*_DrawLow)(m_miniframes[m_currentFrame], drawX, drawY,  m_width>>1, m_height>>1,transparency, outlineColor, flags);
			}
			else if (facing == 4 || facing == 0)
			{
				(this->*_DrawLowReversed)(m_miniframes[m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			} else
			{
				(this->*_DrawLowReversed)(m_miniframes[m_currentFrame], drawX, drawY, m_width>>1, m_height>>1, transparency, outlineColor, flags);
			}
		} else {

			sint32 destWidth = (sint32)(m_width * scale);
			sint32 destHeight = (sint32)(m_height * scale);

			if (facing < 4 && facing > 0)
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_frames[m_currentFrame], drawX, drawY, destWidth, destHeight,
					transparency, outlineColor, flags, FALSE);
			}
			else if (facing == 4 || facing == 0)
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_frames[m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, TRUE);
			} else
			{
				(this->*_DrawScaledLow)((Pixel16 *)m_frames[m_currentFrame], drawX, drawY, destWidth, destHeight,
									transparency, outlineColor, flags, TRUE);
			}
		}
	}
}


Pixel16 *Sprite::GetFrameData(uint16 frameNum)
{
	Assert(frameNum < m_numFrames);
	if (frameNum >= m_numFrames) return NULL;

	Assert(m_frames != NULL);
	if (m_frames == NULL) return NULL;

	return m_frames[frameNum];
}

size_t Sprite::GetFrameDataSize(uint16 frameNum)
{
	Assert(frameNum < m_numFrames);
	if (frameNum >= m_numFrames) return 0;

	Assert(m_framesSizes != NULL);
	if (m_framesSizes == NULL) return 0;

#ifdef _WINDOWS
	Assert(m_framesSizes[frameNum] == _msize(GetFrameData(frameNum)));

	return _msize(GetFrameData(frameNum));
#else
	return m_framesSizes[frameNum];
#endif
}

Pixel16 *Sprite::GetMiniFrameData(uint16 frameNum)
{
	Assert(frameNum < m_numFrames);
	if (frameNum >= m_numFrames) return NULL;

	Assert(m_miniframes != NULL);
	if (m_miniframes == NULL) return NULL;

	return m_miniframes[frameNum];
}

size_t Sprite::GetMiniFrameDataSize(uint16 frameNum)
{
	Assert(frameNum < m_numFrames);
	if (frameNum >= m_numFrames) return NULL;

	Assert(m_miniframesSizes != NULL);
	if (m_miniframesSizes == NULL) return 0;

#ifdef _WINDOWS
	Assert(m_miniframesSizes[frameNum] == _msize(GetMiniFrameData(frameNum)));

	return _msize(GetMiniFrameData(frameNum));
#else
	return m_miniframesSizes[frameNum];
#endif
}

void Sprite::SetFrameData(uint16 frameNum, Pixel16 *data, size_t size)
{
	Assert(data != NULL);
	if (data == NULL) return;
	Assert(frameNum < m_numFrames);
	if (frameNum >= m_numFrames) return;
	Assert(m_frames != NULL);
	if (m_frames == NULL) return;

	m_frames[frameNum] = data;

	Assert(m_framesSizes != NULL);
	if (m_framesSizes == NULL) return;
#ifdef _WINDOWS
//	Assert(size == _msize(data));
#endif
	m_framesSizes[frameNum] = size;
}




void Sprite::SetMiniFrameData(uint16 frameNum, Pixel16 *data, size_t size)
{
	Assert(data != NULL);
	if (data == NULL) return;
	Assert(frameNum < m_numFrames);
	if (frameNum >= m_numFrames) return;
	Assert(m_miniframes != NULL);
	if (m_miniframes == NULL) return;

	m_miniframes[frameNum] = data;

	Assert(m_miniframesSizes != NULL);
	if (m_miniframesSizes == NULL) return;
#ifdef _WINDOWS
//	Assert(size == _msize(data));
#endif
	m_miniframesSizes[frameNum] = size;
}





sint32 Sprite::ParseFromTokens(Token *theToken)
{
	sint32 tmp;

	if (!token_ParseAnOpenBraceNext(theToken)) return FALSE;

	if (!token_ParseValNext(theToken, TOKEN_SPRITE_NUM_FRAMES, tmp)) return FALSE;
	m_numFrames = (uint16)tmp;

	if (!token_ParseValNext(theToken, TOKEN_SPRITE_FIRST_FRAME, tmp)) return FALSE;
	m_firstFrame = (uint16)tmp;

	if (!token_ParseValNext(theToken, TOKEN_SPRITE_WIDTH, tmp)) return FALSE;
	m_width = (uint16)tmp;

	if (!token_ParseValNext(theToken, TOKEN_SPRITE_HEIGHT, tmp)) return FALSE;
	m_height = (uint16)tmp;

	if (!token_ParseKeywordNext(theToken, TOKEN_SPRITE_HOT_POINT)) return FALSE;
	{
		POINT		p;

		if (theToken->Next() == TOKEN_NUMBER) theToken->GetNumber(tmp);
		else return FALSE;

		p.x = tmp;

		if (theToken->Next() == TOKEN_NUMBER) theToken->GetNumber(tmp);
		else return FALSE;

		p.y = tmp;

		m_hotPoint = p;
	}

	if (!token_ParseAnCloseBraceNext(theToken)) return FALSE;

	return TRUE;
}


void Sprite::AllocateFrameArrays(size_t count)
{
    Assert(!m_frames && !m_miniframes);
    Assert(!m_framesSizes && !m_miniframesSizes);

    m_numFrames     = static_cast<uint16>(count);
	m_frames        = new Pixel16*[m_numFrames];
	m_framesSizes   = new size_t[m_numFrames];
	m_miniframes    = new Pixel16*[m_numFrames];
	m_miniframesSizes = new size_t[m_numFrames];
}

void Sprite::Export(FILE *file)
{
	extern TokenData	g_allTokens[];

	fprintf(file, "\t{\n");

	fprintf(file, "\t\t%s\t%d\n", g_allTokens[TOKEN_SPRITE_NUM_FRAMES].keyword, m_numFrames);

	fprintf(file, "\t\t%s\t%d\n", g_allTokens[TOKEN_SPRITE_FIRST_FRAME].keyword, m_firstFrame);

	fprintf(file, "\t\t%s\t%d\n", g_allTokens[TOKEN_SPRITE_WIDTH].keyword, m_width);

	fprintf(file, "\t\t%s\t%d\n", g_allTokens[TOKEN_SPRITE_HEIGHT].keyword, m_height);

	fprintf(file, "\t\t%s\t%d %d\n", g_allTokens[TOKEN_SPRITE_HOT_POINT].keyword, m_hotPoint.x, m_hotPoint.y);

	fprintf(file, "\t}\n\n");

	return;
}




inline Pixel16 Sprite::average(Pixel16 pixel1, Pixel16 pixel2, Pixel16 pixel3, Pixel16 pixel4)
{
	uint16		r1, g1, b1,
				r2, g2, b2,
				r3, g3, b3,
				r4, g4, b4;
	uint16		r0, g0, b0;

	if (g_is565Format) {
		r1 = (pixel1 & 0xF800) >> 11;
		g1 = (pixel1 & 0x07E0) >> 5;
		b1 = (pixel1 & 0x001F);

		r2 = (pixel2 & 0xF800) >> 11;
		g2 = (pixel2 & 0x07E0) >> 5;
		b2 = (pixel2 & 0x001F);

		r3 = (pixel3 & 0xF800) >> 11;
		g3 = (pixel3 & 0x07E0) >> 5;
		b3 = (pixel3 & 0x001F);

		r4 = (pixel4 & 0xF800) >> 11;
		g4 = (pixel4 & 0x07E0) >> 5;
		b4 = (pixel4 & 0x001F);

		r0 = (r1 + r2 + r3 + r4) >> 2;
		g0 = (g1 + g2 + g3 + g4) >> 2;
		b0 = (b1 + b2 + b3 + b4) >> 2;

		return static_cast<Pixel16>((r0 << 11) | (g0 << 5) | b0);
	} else {
		r1 = (pixel1 & 0x7C00) >> 10;
		g1 = (pixel1 & 0x03E0) >> 5;
		b1 = (pixel1 & 0x001F);

		r2 = (pixel2 & 0x7C00) >> 10;
		g2 = (pixel2 & 0x03E0) >> 5;
		b2 = (pixel2 & 0x001F);

		r3 = (pixel3 & 0x7C00) >> 10;
		g3 = (pixel3 & 0x03E0) >> 5;
		b3 = (pixel3 & 0x001F);

		r4 = (pixel4 & 0x7C00) >> 10;
		g4 = (pixel4 & 0x03E0) >> 5;
		b4 = (pixel4 & 0x001F);

		r0 = (r1 + r2 + r3 + r4) >> 2;
		g0 = (g1 + g2 + g3 + g4) >> 2;
		b0 = (b1 + b2 + b3 + b4) >> 2;

		return static_cast<Pixel16>((r0 << 10) | (g0 << 5) | b0);
	}
}




inline Pixel16 Sprite::average(Pixel16 pixel1, Pixel16 pixel2)
{
	uint16		r1, g1, b1,
				r2, g2, b2;
	uint16		r0, g0, b0;

	if (g_is565Format) {
		r1 = (pixel1 & 0xF800) >> 11;
		g1 = (pixel1 & 0x07E0) >> 5;
		b1 = (pixel1 & 0x001F);

		r2 = (pixel2 & 0xF800) >> 11;
		g2 = (pixel2 & 0x07E0) >> 5;
		b2 = (pixel2 & 0x001F);

		r0 = (r1 + r2) >> 1;
		g0 = (g1 + g2) >> 1;
		b0 = (b1 + b2) >> 1;

		return static_cast<Pixel16>((r0 << 11) | (g0 << 5) | b0);
	} else {
		r1 = (pixel1 & 0x7C00) >> 10;
		g1 = (pixel1 & 0x03E0) >> 5;
		b1 = (pixel1 & 0x001F);

		r2 = (pixel2 & 0x7C00) >> 10;
		g2 = (pixel2 & 0x03E0) >> 5;
		b2 = (pixel2 & 0x001F);

		r0 = (r1 + r2) >> 1;
		g0 = (g1 + g2) >> 1;
		b0 = (b1 + b2) >> 1;

		return static_cast<Pixel16>((r0 << 10) | (g0 << 5) | b0);
	}
}
