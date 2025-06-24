//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Activision User Interface blitter
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
// - Standardized code (May 21th 2006 Martin Gühmann)
// - Replaced inline assembly for 32 bit Intel with regular C++ library call.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "aui_blitter.h"

#include <algorithm>          // std::fill, std::max, std::min
#include "aui_surface.h"
#include "aui_dirtylist.h"
#include "aui_pixel.h"

AUI_ERRCODE aui_Blitter::Blt(
	aui_Surface *destSurf,
	sint32 destx,
	sint32 desty,
	aui_Surface *srcSurf,
	RECT *srcRect,
	uint32 flags )
{
	Assert( destSurf != NULL );
	if ( !destSurf ) return AUI_ERRCODE_INVALIDPARAM;
	Assert( srcSurf != NULL );
	if ( !srcSurf ) return AUI_ERRCODE_INVALIDPARAM;

	const sint32 bytesPerPixel = destSurf->BytesPerPixel();

	Assert( srcSurf->BytesPerPixel() == bytesPerPixel );
	if ( srcSurf->BytesPerPixel() != bytesPerPixel )
		return AUI_ERRCODE_INVALIDPARAM;

	Assert( srcRect != NULL );
	if ( !srcRect ) return AUI_ERRCODE_INVALIDPARAM;

	Assert( srcRect->left <= srcRect->right );
	Assert( srcRect->top <= srcRect->bottom );
	if ( srcRect->left > srcRect->right
	||   srcRect->top > srcRect->bottom )
		return AUI_ERRCODE_INVALIDPARAM;

	if ( srcRect->left == srcRect->right
	||   srcRect->top == srcRect->bottom )
		return AUI_ERRCODE_OK;

	RECT clippedSrcRect = *srcRect;

	sint32 dx = clippedSrcRect.left;
	if ( dx > 0 ) dx = 0;
	sint32 dy = clippedSrcRect.top;
	if ( dy > 0 ) dy = 0;

	clippedSrcRect.left -= dx;
	clippedSrcRect.top -= dy;
	if ( clippedSrcRect.right > srcSurf->Width() )
		clippedSrcRect.right = srcSurf->Width();
	if ( clippedSrcRect.bottom > srcSurf->Height() )
		clippedSrcRect.bottom = srcSurf->Height();

	if ( clippedSrcRect.left >= clippedSrcRect.right
	||   clippedSrcRect.top >= clippedSrcRect.bottom )
		return AUI_ERRCODE_OK;

	destx -= dx;
	desty -= dy;
	RECT destRect =
	{
		destx,
		desty,
		destx + clippedSrcRect.right - clippedSrcRect.left,
		desty + clippedSrcRect.bottom - clippedSrcRect.top
	};

	sint32 leftClip     = std::min<sint32>(destRect.left, 0);
	sint32 topClip      = std::min<sint32>(destRect.top, 0);
	sint32 rightClip    = std::max<sint32>(destRect.right - destSurf->Width(), 0);
	sint32 bottomClip   = std::max<sint32>(destRect.bottom - destSurf->Height(), 0);

	destRect.left -= leftClip;
	destRect.top -= topClip;
	destRect.right -= rightClip;
	destRect.bottom -= bottomClip;

	if ( destRect.left >= destRect.right
	||   destRect.top >= destRect.bottom )
		return AUI_ERRCODE_OK;

	clippedSrcRect.left -= leftClip;
	clippedSrcRect.top -= topClip;
	clippedSrcRect.right -= rightClip;
	clippedSrcRect.bottom -= bottomClip;

	switch ( bytesPerPixel )
	{
	case 1:
		return Blt8To8(
			destSurf,
			&destRect,
			srcSurf,
			&clippedSrcRect,
			flags );
	case 2:
		return Blt16To16(
			destSurf,
			&destRect,
			srcSurf,
			&clippedSrcRect,
			flags );
	case 3:
		return Blt24To24(
			destSurf,
			&destRect,
			srcSurf,
			&clippedSrcRect,
			flags );
	default:

		Assert( FALSE );
		return AUI_ERRCODE_INVALIDPARAM;
	}
}

AUI_ERRCODE aui_Blitter::Blt8To8(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *srcSurf,
	RECT *srcRect,
	uint32 flags )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	AUI_ERRCODE errcode;

	const sint32 destPitch = destSurf->Pitch();
	const sint32 srcPitch = srcSurf->Pitch();

	uint8 * destBuf         = destSurf->Buffer();
	bool    wasDestLocked   = destBuf != NULL;

	if (wasDestLocked)
	{
		destBuf += destRect->top * destPitch + destRect->left;
	}
	else if ( destSurf->Lock(
		destRect, (LPVOID *)&destBuf, 0 ) != AUI_ERRCODE_OK )
	{
		destBuf = NULL;
		retcode = AUI_ERRCODE_SURFACELOCKFAILED;
	}

	if ( destBuf )
	{
		uint8 * origDestBuf     = destBuf;
		uint8 * srcBuf          = srcSurf->Buffer();
		bool    wasSrcLocked    = srcBuf != NULL;
		if (wasSrcLocked)
		{
			srcBuf += srcRect->top * srcPitch + srcRect->left;
		}
		else if ( srcSurf->Lock(
			srcRect, (LPVOID *)&srcBuf, 0 ) != AUI_ERRCODE_OK )
		{
			srcBuf = NULL;
			retcode = AUI_ERRCODE_SURFACELOCKFAILED;
		}

		if ( srcBuf )
		{

			uint8 *origSrcBuf = srcBuf;

			const sint32 scanWidth = srcRect->right - srcRect->left;

			if ( flags & k_AUI_BLITTER_FLAG_COPY )
			{

				const uint8 *stop = srcBuf +
					srcPitch * ( srcRect->bottom - srcRect->top );
				destBuf -= destPitch;

				do
				{

					memcpy( destBuf += destPitch, srcBuf, scanWidth );
				} while ( (srcBuf += srcPitch) != stop );
			}
			else if ( (flags & k_AUI_BLITTER_FLAG_CHROMAKEY)
					  && (flags & k_AUI_BLITTER_FLAG_BLEND) )
			{
				retcode = AUI_ERRCODE_INVALIDPARAM;
			}
			else if ( flags & k_AUI_BLITTER_FLAG_CHROMAKEY )
			{

				const sint32 destDiff = destPitch - scanWidth;
				const sint32 srcDiff = srcPitch - scanWidth;

				uint8 *stopHorizontal = srcBuf + scanWidth;
				const uint8 *stopVertical = srcBuf +
					srcPitch * ( srcRect->bottom - srcRect->top );
				destBuf--;

				const uint8 chromakey = (uint8)srcSurf->GetChromaKey();

				do
				{
					do
					{

						if ( *srcBuf != chromakey )
							*++destBuf = *srcBuf;
						else
							destBuf++;
					} while ( ++srcBuf != stopHorizontal );

					stopHorizontal += srcPitch;

					destBuf += destDiff;
				} while ( (srcBuf += srcDiff) != stopVertical );
			}
			else
			{
				retcode = AUI_ERRCODE_INVALIDPARAM;
			}

			if ( !wasSrcLocked )
			{
				errcode = srcSurf->Unlock( (LPVOID)origSrcBuf );

				if ( !AUI_SUCCESS(errcode) )
					retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
			}
		}

		if ( !wasDestLocked )
		{
			errcode = destSurf->Unlock( (LPVOID)origDestBuf );

			if ( !AUI_SUCCESS(errcode) )
				retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
		}
	}

	return retcode;
}

AUI_ERRCODE aui_Blitter::Blt16To16(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *srcSurf,
	RECT *srcRect,
	uint32 flags )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	AUI_ERRCODE errcode;

	const sint32 destPitch = destSurf->Pitch() / 2;
	const sint32 srcPitch = srcSurf->Pitch() / 2;

	uint16 *    destBuf         = (uint16 *)destSurf->Buffer();
	bool        wasDestLocked   = destBuf != NULL;

	if (wasDestLocked)
	{
		destBuf += destRect->top * destPitch + destRect->left;
	}
	else if ( destSurf->Lock(
		destRect, (LPVOID *)&destBuf, 0 ) != AUI_ERRCODE_OK )
	{
		destBuf = NULL;
		retcode = AUI_ERRCODE_SURFACELOCKFAILED;
	}

	if ( destBuf )
	{
		uint16 *    origDestBuf     = destBuf;
		uint16 *    srcBuf          = (uint16 *)srcSurf->Buffer();
		bool        wasSrcLocked    = srcBuf != NULL;

		if (wasSrcLocked)
		{
			srcBuf += srcRect->top * srcPitch + srcRect->left;
		}
		else if ( srcSurf->Lock(
			srcRect, (LPVOID *)&srcBuf, 0 ) != AUI_ERRCODE_OK )
		{
			srcBuf = NULL;
			retcode = AUI_ERRCODE_SURFACELOCKFAILED;
		}

		if ( srcBuf )
		{
			uint16 *origSrcBuf = srcBuf;

			if ( flags & k_AUI_BLITTER_FLAG_COPY )
			{
				const sint32 scanWidth = 2 * ( srcRect->right - srcRect->left );

				const uint16*     stop = srcBuf + srcPitch * ( srcRect->bottom - srcRect->top );
				destBuf -= destPitch;

				do
				{
					memcpy( destBuf += destPitch, srcBuf, scanWidth );
				} while ( (srcBuf += srcPitch) != stop );
			}
			else if ( (flags & k_AUI_BLITTER_FLAG_CHROMAKEY)
					  && (flags & k_AUI_BLITTER_FLAG_BLEND) )
			{
				const sint32 scanWidth = srcRect->right - srcRect->left;

				const sint32 destDiff = destPitch - scanWidth;
				const sint32 srcDiff = srcPitch - scanWidth;

				uint16 *stopHorizontal = srcBuf + scanWidth;
				const uint16 *stopVertical = srcBuf +
					srcPitch * ( srcRect->bottom - srcRect->top );

				const uint16 chromakey = (uint16)srcSurf->GetChromaKey();

				const uint32 blend = 16;

				if ( destSurf->PixelFormat() == AUI_SURFACE_PIXELFORMAT_555 )
				{
					do
					{
						do
						{
							if ( *srcBuf != chromakey )
								*destBuf = aui_Pixel::Blend555(
									*destBuf, *srcBuf, blend );
							destBuf++;
						}
						while ( ++srcBuf != stopHorizontal );

						stopHorizontal += srcPitch;

						destBuf += destDiff;
					} while ( (srcBuf += srcDiff) != stopVertical );
				}
				else
				{
					do
					{
						do
						{

							if ( *srcBuf != chromakey )
								*destBuf = aui_Pixel::Blend565(
									*destBuf, *srcBuf, blend );

							destBuf++;
						}
						while ( ++srcBuf != stopHorizontal );

						stopHorizontal += srcPitch;

						destBuf += destDiff;
					} while ( (srcBuf += srcDiff) != stopVertical );
				}
			}
			else if ( flags & k_AUI_BLITTER_FLAG_CHROMAKEY )
			{
				const sint32 scanWidth = srcRect->right - srcRect->left;

				const sint32 destDiff = destPitch - scanWidth;
				const sint32 srcDiff = srcPitch - scanWidth;

				uint16 *stopHorizontal = srcBuf + scanWidth;
				const uint16 *stopVertical = srcBuf +
					srcPitch * ( srcRect->bottom - srcRect->top );
				destBuf--;

				const uint16 chromakey = (uint16)srcSurf->GetChromaKey();

				do
				{
					do
					{

						if ( *srcBuf != chromakey )
							*++destBuf = *srcBuf;
						else
							destBuf++;
					} while ( ++srcBuf != stopHorizontal );

					stopHorizontal += srcPitch;

					destBuf += destDiff;
				} while ( (srcBuf += srcDiff) != stopVertical );
			}
			else if ( flags & k_AUI_BLITTER_FLAG_BLEND )
			{
				const sint32 scanWidth = srcRect->right - srcRect->left;
				const sint32 destDiff  = destPitch      - scanWidth;
				const sint32 srcDiff   = srcPitch       - scanWidth;

				uint16 *stopHorizontal = srcBuf + scanWidth;
				const uint16 *stopVertical = srcBuf +
					srcPitch * ( srcRect->bottom - srcRect->top );

				const uint32 blend = 16;

				if ( destSurf->PixelFormat() == AUI_SURFACE_PIXELFORMAT_555 )
				{
					do
					{
						do
						{
							*destBuf =
								aui_Pixel::Blend555( *destBuf, *srcBuf, blend );
							destBuf++;
						}
						while ( ++srcBuf != stopHorizontal );

						stopHorizontal += srcPitch;

						destBuf += destDiff;
					} while ( (srcBuf += srcDiff) != stopVertical );
				}
				else
				{
					do
					{
						do
						{
							*destBuf =
								aui_Pixel::Blend565( *destBuf, *srcBuf, blend );
							destBuf++;
						}
						while ( ++srcBuf != stopHorizontal );

						stopHorizontal += srcPitch;

						destBuf += destDiff;
					} while ( (srcBuf += srcDiff) != stopVertical );
				}
			}
			else
			{
				retcode = AUI_ERRCODE_INVALIDPARAM;
			}

			if ( !wasSrcLocked )
			{
				errcode = srcSurf->Unlock( (LPVOID)origSrcBuf );

				if ( !AUI_SUCCESS(errcode) )
					retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
			}
		}

		if ( !wasDestLocked )
		{
			errcode = destSurf->Unlock( (LPVOID)origDestBuf );

			if ( !AUI_SUCCESS(errcode) )
				retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
		}
	}

	return retcode;
}

AUI_ERRCODE aui_Blitter::Blt24To24(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *srcSurf,
	RECT *srcRect,
	uint32 flags )
{
	Assert( FALSE );
	return AUI_ERRCODE_INVALIDPARAM;
}

AUI_ERRCODE aui_Blitter::TileBlt(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *srcSurf,
	RECT *srcRect,
	sint32 anchorx,
	sint32 anchory,
	uint32 flags )
{
	Assert( destSurf != NULL );
	if ( !destSurf ) return AUI_ERRCODE_INVALIDPARAM;
	Assert( srcSurf != NULL );
	if ( !srcSurf ) return AUI_ERRCODE_INVALIDPARAM;

	const sint32 bytesPerPixel = destSurf->BytesPerPixel();

	Assert( srcSurf->BytesPerPixel() == bytesPerPixel );
	if ( srcSurf->BytesPerPixel() != bytesPerPixel )
		return AUI_ERRCODE_INVALIDPARAM;

	Assert( destRect != NULL );
	if ( !destRect ) return AUI_ERRCODE_INVALIDPARAM;

	Assert( destRect->left <= destRect->right );
	Assert( destRect->top <= destRect->bottom );
	if ( destRect->left > destRect->right
	||   destRect->top > destRect->bottom )
		return AUI_ERRCODE_INVALIDPARAM;

	Assert( srcRect != NULL );
	if ( !srcRect ) return AUI_ERRCODE_INVALIDPARAM;

	Assert( srcRect->left <= srcRect->right );
	Assert( srcRect->top <= srcRect->bottom );
	if ( srcRect->left > srcRect->right
	||   srcRect->top > srcRect->bottom )
		return AUI_ERRCODE_INVALIDPARAM;

	if ( destRect->left == destRect->right
	||   destRect->top == destRect->bottom )
		return AUI_ERRCODE_OK;

	if ( srcRect->left == srcRect->right
	||   srcRect->top == srcRect->bottom )
		return AUI_ERRCODE_OK;

	RECT clippedDestRect = *destRect;

	sint32 destdx = clippedDestRect.left;
	if ( destdx > 0 ) destdx = 0;
	sint32 destdy = clippedDestRect.top;
	if ( destdy > 0 ) destdy = 0;

	clippedDestRect.left -= destdx;
	clippedDestRect.top -= destdy;
	if ( clippedDestRect.right > destSurf->Width() )
		clippedDestRect.right = destSurf->Width();
	if ( clippedDestRect.bottom > destSurf->Height() )
		clippedDestRect.bottom = destSurf->Height();

	if ( clippedDestRect.left >= clippedDestRect.right
	||   clippedDestRect.top >= clippedDestRect.bottom )
		return AUI_ERRCODE_OK;

	RECT clippedSrcRect = *srcRect;

	sint32 srcdx = clippedSrcRect.left;
	if ( srcdx > 0 ) srcdx = 0;
	sint32 srcdy = clippedSrcRect.top;
	if ( srcdy > 0 ) srcdy = 0;

	clippedSrcRect.left -= srcdx;
	clippedSrcRect.top -= srcdy;
	if ( clippedSrcRect.right > srcSurf->Width() )
		clippedSrcRect.right = srcSurf->Width();
	if ( clippedSrcRect.bottom > srcSurf->Height() )
		clippedSrcRect.bottom = srcSurf->Height();

	if ( clippedSrcRect.left >= clippedSrcRect.right
	||   clippedSrcRect.top >= clippedSrcRect.bottom )
		return AUI_ERRCODE_OK;

	sint32 destWidth    = clippedDestRect.right - clippedDestRect.left;
	sint32 srcWidth     = clippedSrcRect.right - clippedSrcRect.left;
	if (destWidth < srcWidth)
	{
		srcWidth                = destWidth;
		clippedSrcRect.right    = clippedSrcRect.left + destWidth;
	}

	sint32 destHeight   = clippedDestRect.bottom - clippedDestRect.top;
	sint32 srcHeight    = clippedSrcRect.bottom - clippedSrcRect.top;
	if (destHeight < srcHeight)
	{
		srcHeight               = destHeight;
		clippedSrcRect.bottom   = clippedSrcRect.top + destHeight;
	}

	anchorx = Mod(anchorx,srcWidth);
	anchory = Mod(anchory,srcHeight);

	switch ( bytesPerPixel )
	{
	case 1:
		return TileBlt8To8(
			destSurf,
			&clippedDestRect,
			srcSurf,
			&clippedSrcRect,
			anchorx,
			anchory,
			flags );
	case 2:
		return TileBlt16To16(
			destSurf,
			&clippedDestRect,
			srcSurf,
			&clippedSrcRect,
			anchorx,
			anchory,
			flags );
	case 3:
		return TileBlt24To24(
			destSurf,
			&clippedDestRect,
			srcSurf,
			&clippedSrcRect,
			anchorx,
			anchory,
			flags );
	default:

		Assert( false );
		return AUI_ERRCODE_INVALIDPARAM;
	}
}

AUI_ERRCODE aui_Blitter::TileBlt8To8(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *srcSurf,
	RECT *srcRect,
	sint32 anchorx,
	sint32 anchory,
	uint32 flags )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	AUI_ERRCODE errcode;

	const sint32 destPitch = destSurf->Pitch();
	const sint32 srcPitch  = srcSurf->Pitch();

	uint8 * destBuf         = destSurf->Buffer();
	bool    wasDestLocked   = destBuf != NULL;
	if (wasDestLocked)
	{
		destBuf += destRect->top * destPitch + destRect->left;
	}
	else if ( destSurf->Lock(
		destRect, (LPVOID *)&destBuf, 0 ) != AUI_ERRCODE_OK )
	{
		destBuf = NULL;
		retcode = AUI_ERRCODE_SURFACELOCKFAILED;
	}

	if ( destBuf )
	{
		uint8 * origDestBuf     = destBuf;
		uint8 * srcBuf          = srcSurf->Buffer();
		bool    wasSrcLocked    = srcBuf != NULL;
		if (wasSrcLocked)
		{
			srcBuf += srcRect->top * srcPitch + srcRect->left;
		}
		else if ( srcSurf->Lock(
			srcRect, (LPVOID *)&srcBuf, 0 ) != AUI_ERRCODE_OK )
		{
			srcBuf = NULL;
			retcode = AUI_ERRCODE_SURFACELOCKFAILED;
		}

		if ( srcBuf )
		{
			uint8 *origSrcBuf = srcBuf;

			if ( flags & k_AUI_BLITTER_FLAG_COPY )
			{
				const sint32 destScanWidth = destRect->right - destRect->left;
				const sint32 srcScanWidth = srcRect->right - srcRect->left;

				const sint32 remainder = Mod(destScanWidth,srcScanWidth);
				const sint32 destDiff  = destPitch - destScanWidth + remainder;

				uint8 *stopHorizontal  = destBuf + destScanWidth - remainder;
				const uint8 *stopVertical = destBuf + destPitch *
					( destRect->bottom - destRect->top );
				const uint8 *stop = srcBuf + srcPitch *
					( srcRect->bottom - srcRect->top );

				bool done = false;
				do
				{
					do
					{
						do
						{
							memcpy( destBuf, srcBuf, srcScanWidth );
						} while ( (destBuf += srcScanWidth) != stopHorizontal );

						memcpy( destBuf, srcBuf, remainder );

						stopHorizontal += destPitch;

						if ( (destBuf += destDiff) == stopVertical )
						{
							done = true;
							break;
						}

					} while ( (srcBuf += srcPitch) != stop );

					srcBuf = origSrcBuf;
				} while ( !done );
			}
			else if ( (flags & k_AUI_BLITTER_FLAG_CHROMAKEY)
					  && (flags & k_AUI_BLITTER_FLAG_BLEND) )
			{
				retcode = AUI_ERRCODE_INVALIDPARAM;
			}
			else if ( flags & k_AUI_BLITTER_FLAG_CHROMAKEY )
			{
				const sint32 destScanWidth = destRect->right - destRect->left;
				const sint32 srcScanWidth = srcRect->right - srcRect->left;

				const sint32 remainder = Mod(destScanWidth,srcScanWidth);

				const sint32 destDiff = destPitch - destScanWidth;
				const sint32 srcDiff = srcPitch - remainder;

				uint8 *stopDestHorizontal = destBuf + destScanWidth;
				const uint8 *stopDestVertical = destBuf + destPitch *
					( destRect->bottom - destRect->top );
				uint8 *stopSrcHorizontal = srcBuf + srcScanWidth;
				const uint8 *stopSrcVertical = srcBuf + srcPitch *
					( srcRect->bottom - srcRect->top );

				uint8 *beginningSrcLine = srcBuf;

				const uint8 chromakey = (uint8)srcSurf->GetChromaKey();

				bool done = false;
				do
				{
					do
					{
						do
						{
							if ( *srcBuf != chromakey )
								*destBuf = *srcBuf;

							if ( ++srcBuf == stopSrcHorizontal )
								srcBuf = beginningSrcLine;
						} while ( ++destBuf != stopDestHorizontal );

						if ( (destBuf += destDiff) == stopDestVertical )
						{
							done = true;
							break;
						}

						stopDestHorizontal += destPitch;
						stopSrcHorizontal += srcPitch;

					} while ( (beginningSrcLine = (srcBuf += srcDiff))
							  != stopSrcVertical );

					srcBuf = beginningSrcLine = origSrcBuf;
					stopSrcHorizontal = srcBuf + srcScanWidth;
				} while ( !done );
			}
			else
			{
				retcode = AUI_ERRCODE_INVALIDPARAM;
			}

			if ( !wasSrcLocked )
			{
				errcode = srcSurf->Unlock( (LPVOID)origSrcBuf );

				if ( !AUI_SUCCESS(errcode) )
					retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
			}
		}

		if ( !wasDestLocked )
		{
			errcode = destSurf->Unlock( (LPVOID)origDestBuf );

			if ( !AUI_SUCCESS(errcode) )
				retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
		}
	}

	return retcode;
}

AUI_ERRCODE aui_Blitter::TileBlt16To16(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *srcSurf,
	RECT *srcRect,
	sint32 anchorx,
	sint32 anchory,
	uint32 flags )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	AUI_ERRCODE errcode;

	const sint32 destPitch = destSurf->Pitch() / 2;
	const sint32 srcPitch = srcSurf->Pitch() / 2;

	uint16 *    destBuf         = (uint16 *)destSurf->Buffer();
	bool        wasDestLocked   = destBuf != NULL;

	if (wasDestLocked)
	{
		destBuf += destRect->top * destPitch + destRect->left;
	}
	else if ( destSurf->Lock(
		destRect, (LPVOID *)&destBuf, 0 ) != AUI_ERRCODE_OK )
	{
		destBuf = NULL;
		retcode = AUI_ERRCODE_SURFACELOCKFAILED;
	}

	if ( destBuf )
	{
		uint16 *    origDestBuf     = destBuf;
		uint16 *    srcBuf          = (uint16 *)srcSurf->Buffer();
		bool        wasSrcLocked    = srcBuf != NULL;

		if (wasSrcLocked)
		{
			srcBuf += srcRect->top * srcPitch + srcRect->left;
		}
		else if ( srcSurf->Lock(
			srcRect, (LPVOID *)&srcBuf, 0 ) != AUI_ERRCODE_OK )
		{
			srcBuf = NULL;
			retcode = AUI_ERRCODE_SURFACELOCKFAILED;
		}

		if ( srcBuf )
		{
			uint16 *origSrcBuf = srcBuf;

			if ( flags & k_AUI_BLITTER_FLAG_COPY )
			{
				const sint32 destScanWidth = 2 *
					( destRect->right - destRect->left );
				const sint32 srcScanWidth = 2 *
					( srcRect->right - srcRect->left );

				const sint32 remainder = Mod(destScanWidth,srcScanWidth);

				const sint32 destDiff =
					destPitch - ( destScanWidth - remainder ) / 2;

				uint16 *stopHorizontal = destBuf - destDiff + destPitch;
				const uint16 *stopVertical = destBuf + destPitch *
					( destRect->bottom - destRect->top );
				const uint16 *stop = srcBuf + srcPitch *
					( srcRect->bottom - srcRect->top );

				const sint32 skip = srcScanWidth / 2;

				bool done = false;
				do
				{
					do
					{
						do
						{
							memcpy( destBuf, srcBuf, srcScanWidth );
						} while ( (destBuf += skip) != stopHorizontal );


						memcpy( destBuf, srcBuf, remainder );

						stopHorizontal += destPitch;

						if ( (destBuf += destDiff) == stopVertical )
						{
							done = true;
							break;
						}

					} while ( (srcBuf += srcPitch) != stop );

					srcBuf = origSrcBuf;
				} while ( !done );
			}
			else if ( (flags & k_AUI_BLITTER_FLAG_CHROMAKEY)
					  && (flags & k_AUI_BLITTER_FLAG_BLEND) )
			{
				retcode = AUI_ERRCODE_INVALIDPARAM;
			}
			else if ( flags & k_AUI_BLITTER_FLAG_CHROMAKEY )
			{
				const sint32 destScanWidth = destRect->right - destRect->left;
				const sint32 srcScanWidth  =  srcRect->right -  srcRect->left;

				const sint32 remainder = Mod(destScanWidth,srcScanWidth);

				const sint32 destDiff = destPitch - destScanWidth;
				const sint32 srcDiff  =  srcPitch - remainder;

				uint16 *stopDestHorizontal = destBuf + destScanWidth;
				const uint16 *stopDestVertical = destBuf + destPitch *
					( destRect->bottom - destRect->top );
				uint16 *stopSrcHorizontal = srcBuf + srcScanWidth;
				const uint16 *stopSrcVertical = srcBuf + srcPitch *
					( srcRect->bottom - srcRect->top );

				uint16 *beginningSrcLine = srcBuf;

				const uint16 chromakey = (uint16)srcSurf->GetChromaKey();

				bool done = false;
				do
				{
					do
					{
						do
						{
							if ( *srcBuf != chromakey )
								*destBuf = *srcBuf;

							if ( ++srcBuf == stopSrcHorizontal )
								srcBuf = beginningSrcLine;
						} while ( ++destBuf != stopDestHorizontal );

						if ( (destBuf += destDiff) == stopDestVertical )
						{
							done = true;
							break;
						}

						stopDestHorizontal += destPitch;
						stopSrcHorizontal += srcPitch;

					} while ( (beginningSrcLine = (srcBuf += srcDiff))
							  != stopSrcVertical );

					srcBuf = beginningSrcLine = origSrcBuf;
					stopSrcHorizontal = srcBuf + srcScanWidth;
				} while ( !done );
			}
			else
			{
				retcode = AUI_ERRCODE_INVALIDPARAM;
			}

			if ( !wasSrcLocked )
			{
				errcode = srcSurf->Unlock( (LPVOID)origSrcBuf );

				if ( !AUI_SUCCESS(errcode) )
					retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
			}
		}

		if ( !wasDestLocked )
		{
			errcode = destSurf->Unlock( (LPVOID)origDestBuf );

			if ( !AUI_SUCCESS(errcode) )
				retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
		}
	}

	return retcode;
}

AUI_ERRCODE aui_Blitter::TileBlt24To24(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *srcSurf,
	RECT *srcRect,
	sint32 anchorx,
	sint32 anchory,
	uint32 flags )
{
	Assert( false );
	return AUI_ERRCODE_INVALIDPARAM;
}

AUI_ERRCODE aui_Blitter::BevelBlt(
	aui_Surface *destSurf,
	RECT *destRect,
	RECT *bevelRect,
	sint32 bevelThickness,
	sint32 lightx,
	sint32 lighty,
	uint32 flags )
{
	Assert( destSurf != NULL );
	if ( !destSurf ) return AUI_ERRCODE_INVALIDPARAM;

	Assert( destRect != NULL );
	if ( !destRect ) return AUI_ERRCODE_INVALIDPARAM;

	Assert( destRect->left <= destRect->right );
	Assert( destRect->top <= destRect->bottom );
	if ( destRect->left > destRect->right
	||   destRect->top > destRect->bottom )
		return AUI_ERRCODE_INVALIDPARAM;

	Assert( bevelRect != NULL );
	if ( !bevelRect ) return AUI_ERRCODE_INVALIDPARAM;

	Assert( bevelRect->left <= bevelRect->right );
	Assert( bevelRect->top <= bevelRect->bottom );
	if ( bevelRect->left > bevelRect->right
	||   bevelRect->top > bevelRect->bottom )
		return AUI_ERRCODE_INVALIDPARAM;

	if ( destRect->left == destRect->right
	||   destRect->top == destRect->bottom )
		return AUI_ERRCODE_OK;

	if ( bevelRect->left == bevelRect->right
	||   bevelRect->top == bevelRect->bottom )
		return AUI_ERRCODE_OK;

	RECT clippedDestRect = *destRect;

	sint32 dx = clippedDestRect.left;
	if ( dx > 0 ) dx = 0;
	sint32 dy = clippedDestRect.top;
	if ( dy > 0 ) dy = 0;

	clippedDestRect.left -= dx;
	clippedDestRect.top -= dy;
	if ( clippedDestRect.right > destSurf->Width() )
		clippedDestRect.right = destSurf->Width();
	if ( clippedDestRect.bottom > destSurf->Height() )
		clippedDestRect.bottom = destSurf->Height();

	if ( clippedDestRect.left >= clippedDestRect.right
	||   clippedDestRect.top >= clippedDestRect.bottom )
		return AUI_ERRCODE_OK;

	if ( bevelRect->left >= clippedDestRect.right
	||   bevelRect->top >= clippedDestRect.bottom
	||   bevelRect->right <= clippedDestRect.left
	||   bevelRect->bottom <= clippedDestRect.top )
		return AUI_ERRCODE_OK;

	sint32 bevelRectWidth = bevelRect->right - bevelRect->left;
	sint32 bevelRectHeight = bevelRect->bottom - bevelRect->top;
	if ( bevelThickness * 2 > bevelRectWidth )
		bevelThickness = bevelRectWidth / 2;
	if ( bevelThickness * 2 > bevelRectHeight )
		bevelThickness = bevelRectHeight / 2;

	if ( !bevelThickness ) return AUI_ERRCODE_OK;

	switch ( destSurf->BytesPerPixel() )
	{
	case 1:
		return BevelBlt8(
			destSurf,
			&clippedDestRect,
			bevelRect,
			bevelThickness,
			lightx,
			lighty,
			flags );
	case 2:
		return BevelBlt16(
			destSurf,
			&clippedDestRect,
			bevelRect,
			bevelThickness,
			lightx,
			lighty,
			flags );
	case 3:
		return BevelBlt24(
			destSurf,
			&clippedDestRect,
			bevelRect,
			bevelThickness,
			lightx,
			lighty,
			flags );
	default:

		Assert( FALSE );
		return AUI_ERRCODE_INVALIDPARAM;
	}
}

AUI_ERRCODE aui_Blitter::BevelBlt8(
	aui_Surface *destSurf,
	RECT *destRect,
	RECT *bevelRect,
	sint32 bevelThickness,
	sint32 lightx,
	sint32 lighty,
	uint32 flags )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	AUI_ERRCODE errcode;

	uint8 * destBuf         = destSurf->Buffer();
	bool    wasDestLocked   = destBuf != NULL;

	if (!wasDestLocked)
	{
		if ( destSurf->Lock( destRect, (LPVOID *)&destBuf, 0 ) != AUI_ERRCODE_OK )
		{
			destBuf = NULL;
			retcode = AUI_ERRCODE_SURFACELOCKFAILED;
		}
	}

	if ( destBuf )
	{
		uint8 *origDestBuf = destBuf;

		const sint32 destPitch = destSurf->Pitch();

		destBuf = destSurf->Buffer() +
			bevelRect->top * destPitch + bevelRect->left;

		const sint32 scanWidth = bevelRect->right - bevelRect->left;
		const sint32 destDiff  = destPitch - scanWidth;

		const sint32 skipWidth  = scanWidth - 2 * bevelThickness;
		const sint32 skipHeight = bevelRect->bottom - bevelRect->top -
			2 * bevelThickness;

		uint8 black = 0x00;
		uint8 white = 0xff;

		if ( flags & k_AUI_BLITTER_FLAG_IN )
		{
			black = 0xff;
			white = 0x00;
		}
		else if ( !(flags & k_AUI_BLITTER_FLAG_OUT) )
		{
			retcode = AUI_ERRCODE_INVALIDPARAM;
		}

		uint8 *stop = destBuf + scanWidth;

		sint32 i;
		for (i = 0; i < bevelThickness; i++ )
		{
			std::fill(destBuf, destBuf + i, white);
			std::fill(destBuf +i, stop, white);
			std::fill(stop, stop + i, black);

			destBuf	= stop + i + destDiff;
			stop   += destPitch - 1;
		}

		for (i = skipHeight; i > 0; --i )
		{
			std::fill(destBuf, destBuf + bevelThickness, white);
			destBuf += bevelThickness + skipWidth;
			std::fill(destBuf, destBuf + bevelThickness, black);
			destBuf += bevelThickness + destDiff;
		}

		stop += destPitch * skipHeight + 1;

		for (i = bevelThickness; i > 0; --i)
		{
			std::fill(destBuf, destBuf + i, white);
			std::fill(destBuf + i, stop, black);
			std::fill(stop, stop + i, black);

			destBuf	= stop + i + destDiff;
			stop   += destPitch + 1;
		}

		if ( !wasDestLocked )
		{
			errcode = destSurf->Unlock( (LPVOID)origDestBuf );

			if ( !AUI_SUCCESS(errcode) )
				retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
		}
	}

	return retcode;
}

AUI_ERRCODE aui_Blitter::BevelBlt16(
	aui_Surface *destSurf,
	RECT *destRect,
	RECT *bevelRect,
	sint32 bevelThickness,
	sint32 lightx,
	sint32 lighty,
	uint32 flags )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	AUI_ERRCODE errcode;

	uint16 *    destBuf         = (uint16 *) destSurf->Buffer();
	bool        wasDestLocked   = destBuf != NULL;
	if ( !wasDestLocked)
	{
		if ( destSurf->Lock( destRect, (LPVOID *)&destBuf, 0 ) != AUI_ERRCODE_OK )
		{
			destBuf = NULL;
			retcode = AUI_ERRCODE_SURFACELOCKFAILED;
		}
	}

	if ( destBuf )
	{
		uint16 *origDestBuf = destBuf;

		const sint32 destPitch = destSurf->Pitch() / 2;

		destBuf = (uint16 *)destSurf->Buffer();
		destBuf += bevelRect->top * destPitch + bevelRect->left;

		const sint32 scanWidth = bevelRect->right - bevelRect->left;
		const sint32 destDiff  = destPitch - scanWidth;

		const sint32 skipWidth  = scanWidth - 2 * bevelThickness;
		const sint32 skipHeight = bevelRect->bottom - bevelRect->top -
			2 * bevelThickness;

		sint32 xscalar = 0;
		sint32 yscalar = 0;

		if ( flags & k_AUI_BLITTER_FLAG_IN )
		{
			xscalar = -64;
			yscalar = -48;
		}
		else if ( flags & k_AUI_BLITTER_FLAG_OUT )
		{
			xscalar = 64;
			yscalar = 48;
		}
		else
		{
			retcode = AUI_ERRCODE_INVALIDPARAM;
		}

		uint16 *stop = destBuf + scanWidth;

		if ( destSurf->PixelFormat() == AUI_SURFACE_PIXELFORMAT_555 )
		{
			sint32 i;
			for ( i = 0; i < bevelThickness; i++ )
			{
				sint32 j;
				for ( j = i; j; j-- )
				{
					*destBuf = aui_Pixel::Darken555( *destBuf, -xscalar );
					destBuf++;
				}

				do
				{
					*destBuf = aui_Pixel::Darken555( *destBuf, -yscalar );
					destBuf++;
				}
				while ( destBuf != stop );

				for ( j = i; j; j-- )
				{
					*destBuf = aui_Pixel::Darken555( *destBuf, xscalar );
					destBuf++;
				}

				destBuf += destDiff;
				stop += destPitch - 1;
			}

			for ( i = skipHeight; i; i-- )
			{
				sint32 j;
				for ( j = bevelThickness; j; j-- )
				{
					*destBuf = aui_Pixel::Darken555( *destBuf, -xscalar );
					destBuf++;
				}

				destBuf += skipWidth;

				for ( j = bevelThickness; j; j-- )
				{
					*destBuf = aui_Pixel::Darken555( *destBuf, xscalar );
					destBuf++;
				}

				destBuf += destDiff;
			}

			stop += destPitch * skipHeight + 1;

			for ( i = bevelThickness; i; i-- )
			{
				sint32 j;
				for ( j = i - 1; j; j-- )
				{
					*destBuf = aui_Pixel::Darken555( *destBuf, -xscalar );
					destBuf++;
				}

				do
				{
					*destBuf = aui_Pixel::Darken555( *destBuf, yscalar );
					destBuf++;
				}
				while ( destBuf != stop );

				for ( j = i - 1; j; j-- )
				{
					*destBuf = aui_Pixel::Darken555( *destBuf, xscalar );
					destBuf++;
				}

				destBuf += destDiff;
				stop += destPitch + 1;
			}
		}
		else
		{
			sint32 i;
			for ( i = 0; i < bevelThickness; i++ )
			{
				sint32 j;
				for ( j = i; j; j-- )
				{
					*destBuf = aui_Pixel::Darken565( *destBuf, -xscalar );
					destBuf++;
				}

				do
				{
					*destBuf = aui_Pixel::Darken565( *destBuf, -yscalar );
					destBuf++;
				}
				while ( destBuf != stop );

				for ( j = i; j; j-- )
				{
					*destBuf = aui_Pixel::Darken565( *destBuf, xscalar );
					destBuf++;
				}

				destBuf += destDiff;
				stop += destPitch - 1;
			}

			for ( i = skipHeight; i; i-- )
			{
				sint32 j;
				for ( j = bevelThickness; j; j-- )
				{
					*destBuf = aui_Pixel::Darken565( *destBuf, -xscalar );
					destBuf++;
				}

				destBuf += skipWidth;

				for ( j = bevelThickness; j; j-- )
				{
					*destBuf = aui_Pixel::Darken565( *destBuf, xscalar );
					destBuf++;
				}

				destBuf += destDiff;
			}

			stop += destPitch * skipHeight + 1;

			for ( i = bevelThickness; i; i-- )
			{
				sint32 j;
				for ( j = i - 1; j; j-- )
				{
					*destBuf = aui_Pixel::Darken565( *destBuf, -xscalar );
					destBuf++;
				}

				do
				{
					*destBuf = aui_Pixel::Darken565( *destBuf, yscalar );
					destBuf++;
				}
				while ( destBuf != stop );

				for ( j = i - 1; j; j-- )
				{
					*destBuf = aui_Pixel::Darken565( *destBuf, xscalar );
					destBuf++;
				}

				destBuf += destDiff;
				stop += destPitch + 1;
			}
		}

		if ( !wasDestLocked )
		{
			errcode = destSurf->Unlock( (LPVOID)origDestBuf );

			if ( !AUI_SUCCESS(errcode) )
				retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
		}
	}

	return retcode;
}

AUI_ERRCODE aui_Blitter::BevelBlt24(
	aui_Surface *destSurf,
	RECT *destRect,
	RECT *bevelRect,
	sint32 bevelThickness,
	sint32 lightx,
	sint32 lighty,
	uint32 flags )
{
	Assert( FALSE );
	return AUI_ERRCODE_INVALIDPARAM;
}

AUI_ERRCODE aui_Blitter::ColorBlt(
	aui_Surface *destSurf,
	RECT *destRect,
	COLORREF color,
	uint32 flags )
{
	Assert( destSurf != NULL );
	if ( !destSurf ) return AUI_ERRCODE_INVALIDPARAM;

	Assert( destRect != NULL );
	if ( !destRect ) return AUI_ERRCODE_INVALIDPARAM;

	Assert( destRect->left <= destRect->right );
	Assert( destRect->top <= destRect->bottom );
	if ( destRect->left > destRect->right
	||   destRect->top > destRect->bottom )
		return AUI_ERRCODE_INVALIDPARAM;

	if ( destRect->left == destRect->right
	||   destRect->top == destRect->bottom )
		return AUI_ERRCODE_OK;

	RECT clippedDestRect = *destRect;

	sint32 dx = clippedDestRect.left;
	if ( dx > 0 ) dx = 0;
	sint32 dy = clippedDestRect.top;
	if ( dy > 0 ) dy = 0;

	clippedDestRect.left -= dx;
	clippedDestRect.top -= dy;
	if ( clippedDestRect.right > destSurf->Width() )
		clippedDestRect.right = destSurf->Width();
	if ( clippedDestRect.bottom > destSurf->Height() )
		clippedDestRect.bottom = destSurf->Height();

	if ( clippedDestRect.left >= clippedDestRect.right
	||   clippedDestRect.top >= clippedDestRect.bottom )
		return AUI_ERRCODE_OK;

	switch ( destSurf->BytesPerPixel() )
	{
	case 1:
	{

		uint32 pixelColor =
			destSurf->PixelFormat() == AUI_SURFACE_PIXELFORMAT_332
			?
			( GetBValue(color) >> 5 ) |
			( ( GetGValue(color) >> 5 ) << 3 ) |
			( ( GetRValue(color) >> 6 ) << 6 )
			:
			(uint32)color;
		return ColorBlt8(
			destSurf,
			&clippedDestRect,
			pixelColor,
			flags );
	}
	case 2:
	{
		uint32 pixelColor = (uint32)color;
		switch ( destSurf->PixelFormat() )
		{
		case AUI_SURFACE_PIXELFORMAT_555:

			pixelColor =
				( GetBValue(color) >> 3 ) |
				( ( GetGValue(color) >> 3 ) << 5 ) |
				( ( GetRValue(color) >> 3 ) << 10 );
			break;
		case AUI_SURFACE_PIXELFORMAT_565:

			pixelColor =
				( GetBValue(color) >> 3 ) |
				( ( GetGValue(color) >> 3 ) << 6 ) |
				( ( GetRValue(color) >> 3 ) << 11 );
			break;
		default:
			Assert( FALSE );
			return AUI_ERRCODE_INVALIDPARAM;
		}
		return ColorBlt16(
			destSurf,
			&clippedDestRect,
			pixelColor,
			flags );
	}
	case 3:
	{
		Assert( destSurf->PixelFormat() == AUI_SURFACE_PIXELFORMAT_888 );

		return ColorBlt24(
			destSurf,
			&clippedDestRect,
			(uint32)color,
			flags );
	}
	default:

		Assert( FALSE );
		return AUI_ERRCODE_INVALIDPARAM;
	}
}

AUI_ERRCODE aui_Blitter::ColorBlt8(
	aui_Surface *destSurf,
	RECT *destRect,
	uint32 color,
	uint32 flags )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	AUI_ERRCODE errcode;

	const sint32 destPitch      = destSurf->Pitch();
	uint8 *     destBuf         = destSurf->Buffer();
	bool        wasDestLocked   = destBuf != NULL;

	if (wasDestLocked)
	{
		destBuf += destRect->top * destPitch + destRect->left;
	}
	else if ( destSurf->Lock(
		destRect, (LPVOID *)&destBuf, 0 ) != AUI_ERRCODE_OK )
	{
		destBuf = NULL;
		retcode = AUI_ERRCODE_SURFACELOCKFAILED;
	}

	if ( destBuf )
	{
		uint8 *origDestBuf = destBuf;

		const sint32 scanWidth = destRect->right - destRect->left;

		sint32 height = destRect->bottom - destRect->top;
		destBuf -= destPitch;
		const uint8 color8 = (uint8)color;

		do
		{
			memset( destBuf += destPitch, color8, scanWidth );
		} while ( --height );

		if ( !wasDestLocked )
		{
			errcode = destSurf->Unlock( (LPVOID)origDestBuf );

			if ( !AUI_SUCCESS(errcode) )
				retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
		}
	}

	return retcode;
}

AUI_ERRCODE aui_Blitter::ColorBlt16(
	aui_Surface *destSurf,
	RECT *destRect,
	uint32 color,
	uint32 flags )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	AUI_ERRCODE errcode;

	const sint32 destPitch      = destSurf->Pitch() / 2;
	uint16 *    destBuf         = (uint16 *)destSurf->Buffer();
	bool        wasDestLocked   = destBuf != NULL;

	if (wasDestLocked)
	{
		destBuf += destRect->top * destPitch + destRect->left;
	}
	else if ( destSurf->Lock(
		destRect, (LPVOID *)&destBuf, 0 ) != AUI_ERRCODE_OK )
	{
		destBuf = NULL;
		retcode = AUI_ERRCODE_SURFACELOCKFAILED;
	}

	if ( destBuf )
	{
		uint16 *origDestBuf = destBuf;

		const sint32 scanWidth = destRect->right - destRect->left;

		const sint32 destDiff = destPitch - scanWidth;

		uint16 *stopHorizontal = destBuf + scanWidth;
		const uint16 *stopVertical = destBuf +
			destPitch * ( destRect->bottom - destRect->top );
		sint32 width = scanWidth;
		const uint16 color16 = (uint16)color;

		do
		{
			do
			{
				*destBuf++ = color16;
			} while ( --width );

			stopHorizontal += destPitch;

			width = scanWidth;
		} while ( (destBuf += destDiff) != stopVertical );

		if ( !wasDestLocked )
		{
			errcode = destSurf->Unlock( (LPVOID)origDestBuf );

			if ( !AUI_SUCCESS(errcode) )
				retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
		}
	}

	return retcode;
}

AUI_ERRCODE aui_Blitter::ColorStencilBlt8(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *stencilSurf,
	RECT *stencilRect,
	uint32 color,
	uint32 flags )
{
	return ColorStencilBlt16(destSurf, destRect, stencilSurf, stencilRect, color, flags);
}

AUI_ERRCODE aui_Blitter::ColorStencilBlt(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *stencilSurf,
	RECT *stencilRect,
	COLORREF color,
	uint32 flags )
{
	return ColorStencilBlt16(destSurf, destRect, stencilSurf, stencilRect, color, flags);
}

AUI_ERRCODE aui_Blitter::ColorStencilBlt24(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *stencilSurf,
	RECT *stencilRect,
	uint32 color,
	uint32 flags )
{
	return ColorStencilBlt16(destSurf, destRect, stencilSurf, stencilRect, color, flags);
}

AUI_ERRCODE aui_Blitter::ColorStencilBlt16(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *stencilSurf,
	RECT *stencilRect,
	uint32 color,
	uint32 flags )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	AUI_ERRCODE errcode;

	const sint32 destPitch = destSurf->Pitch() / 2;
	uint16 *    destBuf         = (uint16 *)destSurf->Buffer();
	bool        wasDestLocked   = destBuf != NULL;

	if (wasDestLocked)
	{
		destBuf += destRect->top * destPitch + destRect->left;
	}
	else if ( destSurf->Lock(
		destRect, (LPVOID *)&destBuf, 0 ) != AUI_ERRCODE_OK )
	{
		destBuf = NULL;
		retcode = AUI_ERRCODE_SURFACELOCKFAILED;
	}

	const sint32 stencilPitch = stencilSurf->Pitch() / 2;
	uint16 *    stencilBuf          = (uint16 *)stencilSurf->Buffer();
	bool        wasStencilLocked    = stencilBuf != NULL;

	if (wasStencilLocked)
	{
		stencilBuf += stencilRect->top * stencilPitch + stencilRect->left;
	}
	else if ( stencilSurf->Lock(
		stencilRect, (LPVOID *)&stencilBuf, 0) != AUI_ERRCODE_OK)
	{
		stencilBuf = NULL;
		retcode = AUI_ERRCODE_SURFACELOCKFAILED;
	}

	if ( destBuf && stencilBuf)
	{
		uint16 *origDestBuf = destBuf;
		uint16 *origStencilBuf = stencilBuf;

		const sint32 scanWidth   = destRect->right - destRect->left;
		const sint32 destDiff    = destPitch - scanWidth;
		const sint32 stencilDiff = stencilPitch - (stencilRect->right - stencilRect->left);

		uint16 *stopHorizontal = destBuf + scanWidth;
		const uint16 *stopVertical = destBuf +
			destPitch * ( destRect->bottom - destRect->top );
		sint32 width = scanWidth;
		const uint16 color16 = (uint16)color;

		do
		{
			do
			{
				if(!(*stencilBuf++))
					*destBuf = color16;
				destBuf++;
			} while ( --width );

			stopHorizontal += destPitch;

			width = scanWidth;
			destBuf += destDiff;
			stencilBuf += stencilDiff;
		} while ( destBuf != stopVertical );

		if ( !wasDestLocked )
		{
			errcode = destSurf->Unlock( (LPVOID)origDestBuf );

			if ( !AUI_SUCCESS(errcode) )
				retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
		}

		if ( !wasStencilLocked )
		{
			errcode = stencilSurf->Unlock((LPVOID)origStencilBuf);
			if(!AUI_SUCCESS(errcode))
			{
				retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
			}
		}
	}

	return retcode;
}

AUI_ERRCODE aui_Blitter::ColorBlt24(
	aui_Surface *destSurf,
	RECT *destRect,
	uint32 color,
	uint32 flags )
{
	Assert( false );
	return AUI_ERRCODE_INVALIDPARAM;
}

AUI_ERRCODE aui_Blitter::StencilBlt16(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *stencilSurf,
	RECT *stencilRect,
	aui_Surface *sourceSurf,
	RECT *sourceRect,
	uint32 flags )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	AUI_ERRCODE errcode;

	const sint32 destPitch      = destSurf->Pitch() / 2;
	uint16 *    destBuf         = (uint16 *) destSurf->Buffer();
	bool        wasDestLocked   = destBuf != NULL;

	if (wasDestLocked)
	{
		destBuf += destRect->top * destPitch + destRect->left;
	}
	else if ( destSurf->Lock(
		destRect, (LPVOID *)&destBuf, 0 ) != AUI_ERRCODE_OK )
	{
		destBuf = NULL;
		retcode = AUI_ERRCODE_SURFACELOCKFAILED;
	}

	const sint32 stencilPitch       = stencilSurf->Pitch() / 2;
	uint16 *    stencilBuf          = (uint16 *) stencilSurf->Buffer();
	bool        wasStencilLocked    = stencilBuf != NULL;

	if (wasStencilLocked)
	{
		stencilBuf += stencilRect->top * stencilPitch + stencilRect->left;
	}
	else if ( stencilSurf->Lock(
		stencilRect, (LPVOID *)&stencilBuf, 0) != AUI_ERRCODE_OK)
	{
		stencilBuf = NULL;
		retcode = AUI_ERRCODE_SURFACELOCKFAILED;
	}

	const sint32 sourcePitch = sourceSurf->Pitch() / 2;
	uint16 *    sourceBuf       = (uint16 *)sourceSurf->Buffer();
	bool        wasSourceLocked = sourceBuf != NULL;

	if (wasSourceLocked)
	{
		sourceBuf += sourceRect->top * sourcePitch + sourceRect->left;
	}
	else if(sourceSurf->Lock(
		sourceRect, (LPVOID *)&sourceBuf, 0) != AUI_ERRCODE_OK)
	{
		sourceBuf = NULL;
		retcode = AUI_ERRCODE_SURFACELOCKFAILED;
	}

	if ( destBuf && stencilBuf && sourceBuf)
	{
		uint16 *origDestBuf    = destBuf;
		uint16 *origStencilBuf = stencilBuf;
		uint16 *origSourceBuf  = sourceBuf;

		const sint32 scanWidth = destRect->right - destRect->left;

		const sint32 destDiff    = destPitch    - scanWidth;
		const sint32 stencilDiff = stencilPitch - (stencilRect->right - stencilRect->left);
		const sint32 sourceDiff  = sourcePitch  - (sourceRect->right - sourceRect->left);

		uint16 *stopHorizontal = destBuf + scanWidth;
		const uint16 *stopVertical = destBuf +
			destPitch * ( destRect->bottom - destRect->top );
		sint32 width = scanWidth;

		do
		{
			do
			{
				if((*stencilBuf++))
					*destBuf = *sourceBuf;
				destBuf++;
				sourceBuf++;
			} while ( --width );

			stopHorizontal += destPitch;

			width = scanWidth;
			destBuf += destDiff;
			stencilBuf += stencilDiff;
			sourceBuf += sourceDiff;
		} while ( destBuf != stopVertical );

		if ( !wasDestLocked )
		{
			errcode = destSurf->Unlock( (LPVOID)origDestBuf );

			if ( !AUI_SUCCESS(errcode) )
				retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
		}

		if ( !wasStencilLocked )
		{
			errcode = stencilSurf->Unlock((LPVOID)origStencilBuf);
			if(!AUI_SUCCESS(errcode))
			{
				retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
			}
		}

		if(!wasSourceLocked)
		{
			errcode = sourceSurf->Unlock((LPVOID)origSourceBuf);
			if(!AUI_SUCCESS(errcode))
			{
				retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
			}
		}
	}

	return retcode;
}

typedef sint16 spanIndex;
#define kSpanNull (-1)

struct aui_StencilSpan
{
	sint16 length;
	spanIndex next;
};

struct aui_Stencil
{
	sint32 height;
	aui_StencilSpan *spans;
	spanIndex rowStart[1];
};

static spanIndex CreateStencilRow(uint16 *src, sint32 width, aui_StencilSpan *spans, int index)
{
	uint16 last = 0;
	sint16 length = 0;
	for (int i=0; i<width; i++)
	{
		if (src[i] != last)
		{
			last = src[i];
			if (spans)
			{
				aui_StencilSpan &s = spans[index];
				s.length = length;
				s.next = static_cast<spanIndex>(index + 1);
			}
			index++;
			length = 0;
		}
		length++;
	}
	if (spans)
	{
		aui_StencilSpan &s = spans[index];
		s.length = length;
		s.next = kSpanNull;
	}

	return static_cast<spanIndex>(index + 1);
}

aui_Stencil *aui_CreateStencil(aui_Surface *pSurface)
{
	sint32      height      = pSurface->Height();
	sint32      width       = pSurface->Width();
	sint32      srow        = pSurface->Pitch() >> 1;
	uint16 *    pSrcBase    = (uint16 *) pSurface->Buffer();
	bool        wasUnlocked = pSrcBase == NULL;

	if (wasUnlocked)
	{
		AUI_ERRCODE errcode = pSurface->Lock(NULL,(LPVOID *)&pSrcBase,0);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK)
			return NULL;
	}

	sint32 i;
	uint16 *src = pSrcBase;
	spanIndex numSpans = 0;
	for (i=0; i<height; i++)
	{
		numSpans = CreateStencilRow(src, width, NULL, numSpans);
		src += srow;
	}

	sint32 structMemory = sizeof(aui_Stencil) + (height - 1) * sizeof(spanIndex);

	aui_Stencil *pBuffer = (aui_Stencil *)malloc(structMemory + numSpans * sizeof(aui_StencilSpan));

	pBuffer->spans = (aui_StencilSpan *)(((uint8 *)pBuffer) + structMemory);

	spanIndex index = 0;
	src = pSrcBase;
	for (i=0; i<height; i++)
	{
		pBuffer->rowStart[i] = index;
		index = CreateStencilRow(src, width, pBuffer->spans, index);
		src += srow;
	}

	Assert(index == numSpans);

	if (wasUnlocked)
	{
		AUI_ERRCODE errcode = pSurface->Unlock((LPVOID)pSrcBase);
		Assert(errcode == AUI_ERRCODE_OK);
	}

	return pBuffer;
}

AUI_ERRCODE aui_Blitter::StencilMixBlt16(
		aui_Surface *destSurf,
		RECT *destRect,
		aui_Surface *topSurf,
		RECT *topRect,
		aui_Surface *bottomSurf,
		RECT *bottomRect,
		aui_Stencil *stencil,
		RECT *stencilRect)
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	AUI_ERRCODE errcode;

#define MIX_BLT_LOCK(check, pitch, lockBuf, buf, surf, rect) \
	sint32 pitch = 0; \
	bool check = true; \
	uint16 *buf = NULL; \
	if (surf != NULL) \
	{ \
		buf = (uint16 *)surf->Buffer();\
		pitch = surf->Pitch() / 2; \
		check = buf != NULL; \
		if (check) { \
			buf += rect->top * pitch + rect->left; \
		} else if (surf->Lock(rect, (LPVOID *)&buf, 0) != AUI_ERRCODE_OK) { \
			buf = NULL; \
			retcode = AUI_ERRCODE_SURFACELOCKFAILED; \
		} \
	} \
	uint16 *lockBuf = buf;

	MIX_BLT_LOCK(wasDestLocked, destPitch, origDestBuf, destBuf, destSurf, destRect);
	MIX_BLT_LOCK(wasTopLocked, topPitch, origTopBuf, topBuf, topSurf, topRect);
	MIX_BLT_LOCK(wasBottomLocked, bottomPitch, origBottomBuf, bottomBuf, bottomSurf, bottomRect);

	if ( retcode == AUI_ERRCODE_OK )
	{
		sint32 stencilHeight = stencilRect->bottom - stencilRect->top;
		sint32 stencilRow = stencilRect->top;
		const aui_StencilSpan *spans = stencil->spans;
		const sint32 max = stencilRect->right - stencilRect->left;

		do
		{
			sint32 start = 0;
			uint16 *srcBuf = bottomBuf;
			sint32 pos = -stencilRect->left;
			for
			(
				spanIndex	index = stencil->rowStart[stencilRow++];
				(pos < max) && (index != kSpanNull);
				index = spans[index].next
			)
			{
				sint32 length = spans[index].length;
				if (length > 0)
				{
					pos = std::min<sint32>(pos + length, max);

					if (pos > start)
					{
						if (srcBuf != NULL)
						{
							std::copy(srcBuf + start, srcBuf + pos, destBuf + start);
						}

						start = pos;
					}
				}
				srcBuf = (srcBuf==topBuf) ? bottomBuf : topBuf;
			}

			destBuf += destPitch;
			topBuf += topPitch;
			bottomBuf += bottomPitch;
		} while ( --stencilHeight );
	}

#define MIX_BLT_UNLOCK(check, surf, buf) \
	if ((!check) && (buf != NULL)) {\
		errcode = surf->Unlock((LPVOID)buf);\
		if(!AUI_SUCCESS(errcode))\
			retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;\
	}

	MIX_BLT_UNLOCK(wasDestLocked, destSurf, origDestBuf);
	MIX_BLT_UNLOCK(wasTopLocked, topSurf, origTopBuf);
	MIX_BLT_UNLOCK(wasBottomLocked, bottomSurf, origBottomBuf);
	return retcode;
}

AUI_ERRCODE aui_Blitter::StretchBlt(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *srcSurf,
	RECT *srcRect,
	uint32 flags )
{
	Assert( destSurf != NULL );
	if ( !destSurf ) return AUI_ERRCODE_INVALIDPARAM;
	Assert( srcSurf != NULL );
	if ( !srcSurf ) return AUI_ERRCODE_INVALIDPARAM;

	const sint32 bytesPerPixel = destSurf->BytesPerPixel();

	Assert( srcSurf->BytesPerPixel() == bytesPerPixel );
	if ( srcSurf->BytesPerPixel() != bytesPerPixel )
		return AUI_ERRCODE_INVALIDPARAM;

	Assert( destRect != NULL );
	if ( !destRect ) return AUI_ERRCODE_INVALIDPARAM;

	Assert( destRect->left <= destRect->right );
	Assert( destRect->top <= destRect->bottom );
	if ( destRect->left > destRect->right
	||   destRect->top > destRect->bottom )
		return AUI_ERRCODE_INVALIDPARAM;

	Assert( srcRect != NULL );
	if ( !srcRect ) return AUI_ERRCODE_INVALIDPARAM;

	Assert( srcRect->left <= srcRect->right );
	Assert( srcRect->top <= srcRect->bottom );
	if ( srcRect->left > srcRect->right
	||   srcRect->top > srcRect->bottom )
		return AUI_ERRCODE_INVALIDPARAM;

	if ( destRect->left == destRect->right
	||   destRect->top == destRect->bottom )
		return AUI_ERRCODE_OK;

	if ( srcRect->left == srcRect->right
	||   srcRect->top == srcRect->bottom )
		return AUI_ERRCODE_OK;

	RECT clippedDestRect = *destRect;

	sint32 destdx = clippedDestRect.left;
	if ( destdx > 0 ) destdx = 0;
	sint32 destdy = clippedDestRect.top;
	if ( destdy > 0 ) destdy = 0;

	clippedDestRect.left -= destdx;
	clippedDestRect.top -= destdy;
	if ( clippedDestRect.right > destSurf->Width() )
		clippedDestRect.right = destSurf->Width();
	if ( clippedDestRect.bottom > destSurf->Height() )
		clippedDestRect.bottom = destSurf->Height();

	if ( clippedDestRect.left >= clippedDestRect.right
	||   clippedDestRect.top >= clippedDestRect.bottom )
		return AUI_ERRCODE_OK;

	RECT clippedSrcRect = *srcRect;

	sint32 srcdx = clippedSrcRect.left;
	if ( srcdx > 0 ) srcdx = 0;
	sint32 srcdy = clippedSrcRect.top;
	if ( srcdy > 0 ) srcdy = 0;

	clippedSrcRect.left -= srcdx;
	clippedSrcRect.top -= srcdy;
	if ( clippedSrcRect.right > srcSurf->Width() )
		clippedSrcRect.right = srcSurf->Width();
	if ( clippedSrcRect.bottom > srcSurf->Height() )
		clippedSrcRect.bottom = srcSurf->Height();

	if ( clippedSrcRect.left >= clippedSrcRect.right
	||   clippedSrcRect.top >= clippedSrcRect.bottom )
		return AUI_ERRCODE_OK;

	switch ( bytesPerPixel )
	{
	case 1:
		return StretchBlt8To8(
			destSurf,
			&clippedDestRect,
			srcSurf,
			&clippedSrcRect,
			flags );
	case 2:
		return StretchBlt16To16(
			destSurf,
			&clippedDestRect,
			srcSurf,
			&clippedSrcRect,
			flags );
	case 3:
		return StretchBlt24To24(
			destSurf,
			&clippedDestRect,
			srcSurf,
			&clippedSrcRect,
			flags );
	default:

		Assert( false );
		return AUI_ERRCODE_INVALIDPARAM;
	}
}

AUI_ERRCODE aui_Blitter::StretchBlt8To8(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *srcSurf,
	RECT *srcRect,
	uint32 flags )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	AUI_ERRCODE errcode;

	const sint32 destPitch = destSurf->Pitch();
	const sint32 srcPitch = srcSurf->Pitch();

	uint8 * destBuf         = destSurf->Buffer();
	bool    wasDestLocked   = destBuf != NULL;

	if (wasDestLocked)
	{
		destBuf += destRect->top * destPitch + destRect->left;
	}
	else if ( destSurf->Lock(
		destRect, (LPVOID *)&destBuf, 0 ) != AUI_ERRCODE_OK )
	{
		destBuf = NULL;
		retcode = AUI_ERRCODE_SURFACELOCKFAILED;
	}

	if ( destBuf )
	{
		uint8 * origDestBuf     = destBuf;
		uint8 * srcBuf          = srcSurf->Buffer();
		bool    wasSrcLocked    = srcBuf != NULL;

		if (wasSrcLocked)
		{
			srcBuf += srcRect->top * srcPitch + srcRect->left;
		}
		else if ( srcSurf->Lock(
			srcRect, (LPVOID *)&srcBuf, 0 ) != AUI_ERRCODE_OK )
		{
			srcBuf = NULL;
			retcode = AUI_ERRCODE_SURFACELOCKFAILED;
		}

		if ( srcBuf )
		{
			uint8 *origSrcBuf = srcBuf;

			const sint32 destScanWidth = destRect->right - destRect->left;
			const sint32 destDiff      = destPitch       - destScanWidth;

			uint8 *stopHorizontal = destBuf + destScanWidth;
			const uint8 *stopVertical = destBuf + destPitch *
				( destRect->bottom - destRect->top );

			const double stepHorizontal =
				double(srcRect->right - srcRect->left) /
				double(destScanWidth);
			const double stepVertical =
				double(srcRect->bottom - srcRect->top) /
				double(destRect->bottom - destRect->top);

			double srcPtr = (double)size_t(srcBuf);
			double srcLine = 0.0;

			if ( flags & k_AUI_BLITTER_FLAG_COPY )
			{
				do
				{
					do
					{
						*destBuf = *srcBuf;

						srcBuf =
							(uint8 *)size_t((srcPtr += stepHorizontal) + 0.5);

					} while ( ++destBuf != stopHorizontal );

					srcPtr = (double)size_t(origSrcBuf +
						size_t((srcLine += stepVertical) + 0.5) * srcPitch);

					stopHorizontal += destPitch;

				} while ( (destBuf += destDiff) != stopVertical );
			}
			else if ( (flags & k_AUI_BLITTER_FLAG_CHROMAKEY)
					  && (flags & k_AUI_BLITTER_FLAG_BLEND) )
			{
				retcode = AUI_ERRCODE_INVALIDPARAM;
			}
			else if ( flags & k_AUI_BLITTER_FLAG_CHROMAKEY )
			{
				const uint8 chromakey = (uint8)srcSurf->GetChromaKey();

				do
				{
					do
					{
						if ( *srcBuf != chromakey )
							*destBuf = *srcBuf;

						srcBuf =
							(uint8 *)size_t((srcPtr += stepHorizontal) + 0.5);

					} while ( ++destBuf != stopHorizontal );

					srcPtr = (double)size_t(srcBuf = origSrcBuf +
						uint32((srcLine += stepVertical) + 0.5) * srcPitch);

					stopHorizontal += destPitch;

				} while ( (destBuf += destDiff) != stopVertical );
			}
			else
			{
				retcode = AUI_ERRCODE_INVALIDPARAM;
			}

			if ( !wasSrcLocked )
			{
				errcode = srcSurf->Unlock( (LPVOID)origSrcBuf );

				if ( !AUI_SUCCESS(errcode) )
					retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
			}
		}

		if ( !wasDestLocked )
		{
			errcode = destSurf->Unlock( (LPVOID)origDestBuf );

			if ( !AUI_SUCCESS(errcode) )
				retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
		}
	}

	return retcode;
}

AUI_ERRCODE aui_Blitter::StretchBlt16To16(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *srcSurf,
	RECT *srcRect,
	uint32 flags )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	AUI_ERRCODE errcode;

	const sint32 destPitch = destSurf->Pitch() / 2;
	const sint32 srcPitch = srcSurf->Pitch() / 2;

	uint16 *    destBuf         = (uint16 *) destSurf->Buffer();
	bool        wasDestLocked   = destBuf != NULL;

	if (wasDestLocked)
	{
		destBuf += destRect->top * destPitch + destRect->left;
	}
	else if ( destSurf->Lock(
		destRect, (LPVOID *)&destBuf, 0 ) != AUI_ERRCODE_OK )
	{
		destBuf = NULL;
		retcode = AUI_ERRCODE_SURFACELOCKFAILED;
	}

	if ( destBuf )
	{
		uint16 *    origDestBuf     = destBuf;
		uint16 *    srcBuf          = (uint16 *) srcSurf->Buffer();
		bool        wasSrcLocked    = srcBuf != NULL;

		if (wasSrcLocked)
		{
			srcBuf += srcRect->top * srcPitch + srcRect->left;
		}
		else if ( srcSurf->Lock(
			srcRect, (LPVOID *)&srcBuf, 0 ) != AUI_ERRCODE_OK )
		{
			srcBuf = NULL;
			retcode = AUI_ERRCODE_SURFACELOCKFAILED;
		}

		if ( srcBuf )
		{
			uint16 *origSrcBuf = srcBuf;

			const sint32 destScanWidth = destRect->right - destRect->left;
			const sint32 destDiff      = destPitch       - destScanWidth;

			uint16 *stopHorizontal = destBuf + destScanWidth;
			const uint16 *stopVertical = destBuf + destPitch *
				( destRect->bottom - destRect->top );

			const double stepHorizontal =
				double(2 * ( srcRect->right - srcRect->left )) /
				double(destScanWidth);
			const double stepVertical =
				double(srcRect->bottom - srcRect->top) /
				double(destRect->bottom - destRect->top);

			double srcPtr = (double)(size_t)srcBuf;
			double srcLine = 0.0;

			if ( flags & k_AUI_BLITTER_FLAG_COPY )
			{
				do
				{
					do
					{
						*destBuf = *srcBuf;

						srcBuf =
							(uint16 *)size_t((srcPtr += stepHorizontal) + 0.0);

						if ( (size_t)srcBuf & 0x1 )
							srcBuf = (uint16 *)(size_t(srcBuf) - 1);

					} while ( ++destBuf != stopHorizontal );

					srcBuf = (uint16 *)(origSrcBuf +
						sint32((srcLine += stepVertical) + 0.0) * srcPitch);
					srcPtr = (double)(size_t)srcBuf;

					stopHorizontal += destPitch;

				} while ( (destBuf += destDiff) != stopVertical );
			}
			else if ( (flags & k_AUI_BLITTER_FLAG_CHROMAKEY)
					  && (flags & k_AUI_BLITTER_FLAG_BLEND) )
			{
				retcode = AUI_ERRCODE_INVALIDPARAM;
			}
			else if ( flags & k_AUI_BLITTER_FLAG_CHROMAKEY )
			{
				const uint16 chromakey = (uint16)srcSurf->GetChromaKey();

				do
				{
					do
					{
						if ( *srcBuf != chromakey )
							*destBuf = *srcBuf;

						srcBuf =
							(uint16 *)size_t((srcPtr += stepHorizontal) + 0.0);

						if ( (size_t)srcBuf & 0x1 )
							srcBuf = (uint16 *)(size_t(srcBuf) - 1);

					} while ( ++destBuf != stopHorizontal );

					srcBuf = (uint16 *)(origSrcBuf +
						sint32((srcLine += stepVertical) + 0.0) * srcPitch);
					srcPtr = (double)(size_t)srcBuf;

					stopHorizontal += destPitch;

				} while ( (destBuf += destDiff) != stopVertical );
			}
			else
			{
				retcode = AUI_ERRCODE_INVALIDPARAM;
			}

			if ( !wasSrcLocked )
			{
				errcode = srcSurf->Unlock( (LPVOID)origSrcBuf );

				if ( !AUI_SUCCESS(errcode) )
					retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
			}
		}

		if ( !wasDestLocked )
		{
			errcode = destSurf->Unlock( (LPVOID)origDestBuf );

			if ( !AUI_SUCCESS(errcode) )
				retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
		}
	}

	return retcode;
}

AUI_ERRCODE aui_Blitter::StretchBlt24To24(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *srcSurf,
	RECT *srcRect,
	uint32 flags )
{

	Assert( FALSE );
	return AUI_ERRCODE_INVALIDPARAM;
}

AUI_ERRCODE aui_Blitter::SpanBlt(
	aui_Surface *destSurf,
	sint32 destx,
	sint32 desty,
	aui_Surface *srcSurf,
	aui_DirtyList *srcDirtyList,
	uint32 flags )
{
	Assert( destx == 0 && desty == 0 );
	if ( destx || desty ) return AUI_ERRCODE_INVALIDPARAM;

	Assert( destSurf != NULL );
	if ( !destSurf ) return AUI_ERRCODE_INVALIDPARAM;
	Assert( srcSurf != NULL );
	if ( !srcSurf ) return AUI_ERRCODE_INVALIDPARAM;

	const sint32 bytesPerPixel = destSurf->BytesPerPixel();

	Assert( srcSurf->BytesPerPixel() == bytesPerPixel );
	if ( srcSurf->BytesPerPixel() != bytesPerPixel )
		return AUI_ERRCODE_INVALIDPARAM;

	Assert( srcDirtyList != NULL );
	if ( !srcDirtyList ) return AUI_ERRCODE_INVALIDPARAM;

	aui_SpanList *srcSpanListArray = srcDirtyList->GetSpans();
	Assert( srcSpanListArray != NULL );
	if ( !srcSpanListArray ) return AUI_ERRCODE_INVALIDPARAM;

	if ( srcDirtyList->IsEmpty() )
		return AUI_ERRCODE_OK;

	switch ( bytesPerPixel )
	{
	case 2:
		return SpanBlt16To16(
			destSurf,
			destx,
			desty,
			srcSurf,
			srcSpanListArray,
			flags );
	case 3:

		return AUI_ERRCODE_INVALIDPARAM;
	default:

		Assert( FALSE );
		return AUI_ERRCODE_INVALIDPARAM;
	}
}

AUI_ERRCODE aui_Blitter::SpanBlt16To16(
	aui_Surface *destSurf,
	sint32 destx,
	sint32 desty,
	aui_Surface *srcSurf,
	aui_SpanList *srcSpanListArray,
	uint32 flags )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	AUI_ERRCODE errcode;

	const sint32 destPitch = destSurf->Pitch() / 2;
	const sint32 srcPitch = srcSurf->Pitch() / 2;

	uint16 *destBuf = (uint16 *)destSurf->Buffer();
	if ( destBuf || destSurf->Lock(
		NULL, (LPVOID *)&destBuf, 0 ) != AUI_ERRCODE_OK )
	{
		destBuf = NULL;
		retcode = AUI_ERRCODE_SURFACELOCKFAILED;
	}

	if ( destBuf )
	{
		uint16 *origDestBuf = destBuf;

		uint16 *srcBuf = (uint16 *)srcSurf->Buffer();
		if ( srcBuf || srcSurf->Lock(
			NULL, (LPVOID *)&srcBuf, 0 ) != AUI_ERRCODE_OK )
		{
			srcBuf = NULL;
			retcode = AUI_ERRCODE_SURFACELOCKFAILED;
		}

		if ( srcBuf )
		{
			uint16 *origSrcBuf = srcBuf;

			if ( flags & k_AUI_BLITTER_FLAG_COPY )
			{
				aui_SpanList *          curSpanList  = srcSpanListArray;
				const aui_SpanList *    stopSpanList = curSpanList + srcSurf->Height();

				do
				{
					uint16 *destLineStart = destBuf;
					uint16 *srcLineStart = srcBuf;

					sint32 num = curSpanList->num;
					aui_Span *curSpan = curSpanList->spans;
					while ( num-- )
					{
						memcpy(
							destBuf += curSpan->run,
							srcBuf += curSpan->run,
							curSpan->length << 1 );

						destBuf += curSpan->length;
						srcBuf += curSpan->length;
						curSpan++;
					}

					destBuf = ( destLineStart += destPitch );
					srcBuf = ( srcLineStart += srcPitch );

				} while ( ++curSpanList != stopSpanList );
			}
			else
			{
				retcode = AUI_ERRCODE_INVALIDPARAM;
			}

			errcode = srcSurf->Unlock( (LPVOID)origSrcBuf );

			if ( !AUI_SUCCESS(errcode) )
				retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
		}

		errcode = destSurf->Unlock( (LPVOID)origDestBuf );

		if ( !AUI_SUCCESS(errcode) )
			retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
	}

	return retcode;
}
