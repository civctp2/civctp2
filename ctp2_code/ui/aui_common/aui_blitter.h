#ifndef __AUI_BLITTER_H__
#define __AUI_BLITTER_H__


#include "aui_base.h"


class aui_Surface;
class aui_DirtyList;
struct aui_SpanList;



#define k_AUI_BLITTER_FLAG_COPY			0x00000001
#define k_AUI_BLITTER_FLAG_CHROMAKEY	0x00000002
#define k_AUI_BLITTER_FLAG_BLEND		0x00000004
#define k_AUI_BLITTER_FLAG_STIPPLE		0x00000008
#define k_AUI_BLITTER_FLAG_FAST			0x00000010


#define k_AUI_BLITTER_FLAG_OUT			0x00010000
#define k_AUI_BLITTER_FLAG_IN			0x00020000

struct aui_Stencil;
aui_Stencil *aui_CreateStencil(aui_Surface *pSurface);




class aui_Blitter : public aui_Base
{
public:
	
	aui_Blitter() {}
	virtual ~aui_Blitter() {}


	
	
	
	

	typedef AUI_ERRCODE (RobustBltFunc)(
		aui_Surface *destSurf,
		sint32 destx,
		sint32 desty,
		aui_Surface *srcSurf,
		RECT *srcRect,
		uint32 flags );

	typedef AUI_ERRCODE (NakedBltFunc)(
		aui_Surface *destSurf,
		RECT *destRect,
		aui_Surface *srcSurf,
		RECT *srcRect,
		uint32 flags );

	
	virtual RobustBltFunc Blt;

	virtual NakedBltFunc Blt8To8;
	virtual NakedBltFunc Blt16To16;
	virtual NakedBltFunc Blt24To24;


	
	
	
	

	typedef AUI_ERRCODE (RobustTileBltFunc)(
		aui_Surface *destSurf,
		RECT *destRect,
		aui_Surface *srcSurf,
		RECT *srcRect,
		sint32 anchorx,
		sint32 anchory,
		uint32 flags );

	typedef AUI_ERRCODE (NakedTileBltFunc)(
		aui_Surface *destSurf,
		RECT *destRect,
		aui_Surface *srcSurf,
		RECT *srcRect,
		sint32 anchorx,
		sint32 anchory,
		uint32 flags );

	
	virtual RobustTileBltFunc TileBlt;

	virtual NakedTileBltFunc TileBlt8To8;
	virtual NakedTileBltFunc TileBlt16To16;
	virtual NakedTileBltFunc TileBlt24To24;


	
	
	
	
	
	

	typedef AUI_ERRCODE (RobustBevelBltFunc)(
		aui_Surface *destSurf,
		RECT *destRect,
		RECT *bevelRect,
		sint32 bevelThickness,
		sint32 lightx,
		sint32 lighty,
		uint32 flags );

	typedef AUI_ERRCODE (NakedBevelBltFunc)(
		aui_Surface *destSurf,
		RECT *destRect,
		RECT *bevelRect,
		sint32 bevelThickness,
		sint32 lightx,
		sint32 lighty,
		uint32 flags );

	
	virtual RobustBevelBltFunc BevelBlt;

	virtual NakedBevelBltFunc BevelBlt8;
	virtual NakedBevelBltFunc BevelBlt16;
	virtual NakedBevelBltFunc BevelBlt24;


	
	

	typedef AUI_ERRCODE (RobustColorBltFunc)(
		aui_Surface *destSurf,
		RECT *destRect,
		COLORREF color,
		uint32 flags );

	typedef AUI_ERRCODE (NakedColorBltFunc)(
		aui_Surface *destSurf,
		RECT *destRect,
		uint32 color,
		uint32 flags );

	
	virtual RobustColorBltFunc ColorBlt;

	virtual NakedColorBltFunc ColorBlt8;
	virtual NakedColorBltFunc ColorBlt16;
	virtual NakedColorBltFunc ColorBlt24;


	
	typedef AUI_ERRCODE (RobustColorStencilBltFunc)(
		aui_Surface *destSurf,
		RECT *destRect,
		aui_Surface *stencilSurf,
		RECT *stencilRect,
		COLORREF color,
		uint32 flags );

	typedef AUI_ERRCODE (NakedColorStencilBltFunc)(
		aui_Surface *destSurf,
		RECT *destRect,
		aui_Surface *stencilSurf,
		RECT *stencilRect,
		uint32 color,
		uint32 flags );

	
	virtual RobustColorStencilBltFunc ColorStencilBlt;

	virtual NakedColorStencilBltFunc ColorStencilBlt8;
	virtual NakedColorStencilBltFunc ColorStencilBlt16;
	virtual NakedColorStencilBltFunc ColorStencilBlt24;

	AUI_ERRCODE StencilBlt16(
		aui_Surface *destSurf,
		RECT *destRect,
		aui_Surface *stencilSurf,
		RECT *stencilRect,
		aui_Surface *sourceSurf,
		RECT *sourceRect,
		uint32 flags);

	AUI_ERRCODE StencilMixBlt16(
		aui_Surface *destSurf,
		RECT *destRect,
		aui_Surface *topSurf,
		RECT *topRect,
		aui_Surface *bottmSurf,
		RECT *bottomRect,
		aui_Stencil *stencil,
		RECT *stencilRect);

	
	
	
	

	typedef AUI_ERRCODE (RobustStretchBltFunc)(
		aui_Surface *destSurf,
		RECT *destRect,
		aui_Surface *srcSurf,
		RECT *srcRect,
		uint32 flags );

	typedef AUI_ERRCODE (NakedStretchBltFunc)(
		aui_Surface *destSurf,
		RECT *destRect,
		aui_Surface *srcSurf,
		RECT *srcRect,
		uint32 flags );

	
	virtual RobustStretchBltFunc StretchBlt;

	virtual NakedStretchBltFunc StretchBlt8To8;
	virtual NakedStretchBltFunc StretchBlt16To16;
	virtual NakedStretchBltFunc StretchBlt24To24;


	

	AUI_ERRCODE SpanBlt(
		aui_Surface *destSurf,
		sint32 destx,
		sint32 desty,
		aui_Surface *srcSurf,
		aui_DirtyList *srcDirtyList,
		uint32 flags );

	AUI_ERRCODE SpanBlt16To16(
		aui_Surface *destSurf,
		sint32 destx,
		sint32 desty,
		aui_Surface *srcSurf,
		aui_SpanList *srcSpanListArray,
		uint32 flags );
};


#endif 
