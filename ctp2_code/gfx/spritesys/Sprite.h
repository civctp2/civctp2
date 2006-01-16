//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Sprite
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

#ifndef __SPRITE_H__
#define __SPRITE_H__

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

// #include <>

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class Sprite;

enum SPRITETYPE 
{
	SPRITETYPE_NORMAL,
	SPRITETYPE_FACED,
	SPRITETYPE_FACEDWSHADOW,

	SPRITETYPE_MAX          // dummy last entry, used as counter
};

#define k_SPRITE_BASEFACING 0 

#define k_BIT_DRAWFLAGS_NONE			0x00000000
#define k_BIT_DRAWFLAGS_FEATHERING		0x00000001
#define k_BIT_DRAWFLAGS_TRANSPARENCY	0x00000002
#define k_BIT_DRAWFLAGS_OUTLINE			0x00000004
#define k_BIT_DRAWFLAGS_AA				0x00000008
#define k_BIT_DRAWFLAGS_AA_4_WAY		0x00000010
#define k_BIT_DRAWFLAGS_ADDITIVE		0x00000020
#define k_BIT_DRAWFLAGS_FOGGED			0x00000040
#define k_BIT_DRAWFLAGS_DESATURATED		0x00000080


#define k_BIT_DRAWFLAGS_SPECIAL1	    (k_BIT_DRAWFLAGS_TRANSPARENCY |	\
										 k_BIT_DRAWFLAGS_FOGGED		  |	\
										 k_BIT_DRAWFLAGS_DESATURATED  )

#define k_DRAWFLAGS_NORMAL		        k_BIT_DRAWFLAGS_FEATHERING


//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "pixelutils.h"

class aui_Surface;
class Token;

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------
 
typedef  void (Sprite::*_SPRITE_DRAWLOW1)(Pixel16 *frame, 
					  sint32 drawX, sint32 drawY, 
					  sint32 width, sint32 height,
					  uint16 transparency, 
					  Pixel16 outlineColor, 
					  uint16 flags);

typedef  void (Sprite::*_SPRITE_DRAWLOW2)(Pixel16 *frame, 
					  sint32 drawX, sint32 drawY, 
					  sint32 destwidth, sint32 destheight,
					  uint16 transparency, 
					  Pixel16 outlineColor, 
					  uint16 flags,
					  BOOL reversed);

class Sprite 
{
public:
	Sprite();
	virtual ~Sprite();
	
	void			Load(char *filename);
	
	void			Import(uint16 nframes, char **imageFiles, char **shadowFiles);
	
	void			ImportTIFF(uint16 index,char **imageFiles,Pixel32 **imageData);
	
	void			ImportTGA (uint16 index,char **imageFiles,Pixel32 **imageData);
	
	void			Save(char *filename);
	void			Export(FILE *file);

	void			LockSurface(aui_Surface *surf);
	void			UnlockSurface(void);
	void			SetSurface(void);

	uint16			GetType(void) { return m_type; }
	void			SetType(uint16 type) { m_type = (SPRITETYPE)type; }

	uint16			GetWidth(void) { return m_width; }
	uint16			GetHeight(void) { return m_height; }

	void			SetWidth(uint16 w) { m_width = w; }
	void			SetHeight(uint16 h) { m_height = h; }

	POINT			GetHotPoint(void) { return m_hotPoint; }
	void			SetHotPoint(sint32 hx, sint32 hy) { m_hotPoint.x = hx; m_hotPoint.y = hy; };

	virtual void	Draw(sint32 drawX, sint32 drawY, sint32 facing, double scale, sint16 transparency, 
						Pixel16 outlineColor, uint16 flags);
	virtual void	DirectionalDraw(sint32 drawX, sint32 drawY, sint32 facing, double scale, 
						sint16 transparency, Pixel16 outlineColor, uint16 flags);

	virtual void	DrawDirect(aui_Surface *surf, sint32 drawX, sint32 drawY, sint32 facing, double m_scale, sint16 transparency, 
						Pixel16 outlineColor, uint16 flags);


	virtual BOOL	HitTest(POINT mousePt, sint32 drawX, sint32 drawY, sint32 facing, double m_scale, sint16 transparency, 
						Pixel16 outlineColor, uint16 flags);

	virtual uint16	GetNumFrames(void) const { return m_numFrames; };
	virtual void	SetNumFrames(uint16 num) { m_numFrames = num; }

	sint32			GetCurrentFrame(void) { return m_currentFrame; };
	void			SetCurrentFrame(sint16 cFrame) { m_currentFrame = cFrame; };

	Pixel16*		GetFrameData(uint16 frameNum);
	size_t			GetFrameDataSize(uint16 frameNum);
	Pixel16*		GetMiniFrameData(uint16 frameNum);
	size_t			GetMiniFrameDataSize(uint16 frameNum);

	uint16			GetFirstFrame(void) const { return m_firstFrame; }
	void			SetFirstFrame(uint16 frame) { m_firstFrame = frame; }

	void			SetFrameData(uint16 frameNum, Pixel16 *data);
	void			SetMiniFrameData(uint16 frameNum, Pixel16 *data);

	sint32			ParseFromTokens(Token *theToken);

	void			AllocateFrameArrays(size_t count);
	void			AllocateFrameArraysBasic(void);

	
	void			InitializeDrawLow();

protected:
    
	_SPRITE_DRAWLOW1 _DrawLowClipped        	;
	_SPRITE_DRAWLOW1 _DrawLow               	;
	_SPRITE_DRAWLOW1 _DrawLowReversedClipped	;
	_SPRITE_DRAWLOW1 _DrawLowReversed       	;
	_SPRITE_DRAWLOW1 _DrawReflectionLow     	;
	_SPRITE_DRAWLOW1 _DrawFlashLow          	;
	_SPRITE_DRAWLOW1 _DrawFlashLowReversed  	;

	
	_SPRITE_DRAWLOW2 _DrawScaledLow		;
	_SPRITE_DRAWLOW2 _DrawFlashScaledLow;

	
	void	DrawLowClipped565(Pixel16 *frame, sint32 drawX, sint32 drawY, sint32 width, sint32 height,
							uint16 transparency, Pixel16 outlineColor, uint16 flags);
	void	DrawLow565(Pixel16 *frame, sint32 drawX, sint32 drawY, sint32 width, sint32 height,
						uint16 transparency, Pixel16 outlineColor, uint16 flags);
	void	DrawLowReversedClipped565(Pixel16 *frame, sint32 drawX, sint32 drawY, sint32 width, sint32 height,
						uint16 transparency, Pixel16 outlineColor, uint16 flags);
	void	DrawLowReversed565(Pixel16 *frame, sint32 drawX, sint32 drawY, sint32 width, sint32 height,
						uint16 transparency, Pixel16 outlineColor, uint16 flags);
	void	DrawReflectionLow565(Pixel16 *frame, sint32 drawX, sint32 drawY, sint32 width, sint32 height,
						uint16 transparency, Pixel16 outlineColor, uint16 flags);
	void	DrawFlashLow565(Pixel16 *frame, sint32 drawX, sint32 drawY, sint32 width, sint32 height,
						uint16 transparency, Pixel16 outlineColor, uint16 flags);
	void	DrawFlashLowReversed565(Pixel16 *frame, sint32 drawX, sint32 drawY, sint32 width, sint32 height,
						uint16 transparency, Pixel16 outlineColor, uint16 flags);

	void   DrawScaledLow565(Pixel16 *data, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight,
		   		 uint16 transparency, Pixel16 outlineColor, uint16 flags, BOOL reverse);

	void   DrawFlashScaledLow565(Pixel16 *data, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight,
							 uint16 transparency, Pixel16 outlineColor, uint16 flags, BOOL reverse);

	inline Pixel16	average_565(Pixel16 pixel1, Pixel16 pixel2, Pixel16 pixel3, Pixel16 pixel4);
	inline Pixel16	average_565(Pixel16 pixel1, Pixel16 pixel2);


	
	void	DrawLowClipped555(Pixel16 *frame, sint32 drawX, sint32 drawY, sint32 width, sint32 height,
							uint16 transparency, Pixel16 outlineColor, uint16 flags);
	void	DrawLow555(Pixel16 *frame, sint32 drawX, sint32 drawY, sint32 width, sint32 height,
						uint16 transparency, Pixel16 outlineColor, uint16 flags);
	void	DrawLowReversedClipped555(Pixel16 *frame, sint32 drawX, sint32 drawY, sint32 width, sint32 height,
						uint16 transparency, Pixel16 outlineColor, uint16 flags);
	void	DrawLowReversed555(Pixel16 *frame, sint32 drawX, sint32 drawY, sint32 width, sint32 height,
						uint16 transparency, Pixel16 outlineColor, uint16 flags);
	void	DrawReflectionLow555(Pixel16 *frame, sint32 drawX, sint32 drawY, sint32 width, sint32 height,
						uint16 transparency, Pixel16 outlineColor, uint16 flags);
	void	DrawFlashLow555(Pixel16 *frame, sint32 drawX, sint32 drawY, sint32 width, sint32 height,
						uint16 transparency, Pixel16 outlineColor, uint16 flags);
	void	DrawFlashLowReversed555(Pixel16 *frame, sint32 drawX, sint32 drawY, sint32 width, sint32 height,
						uint16 transparency, Pixel16 outlineColor, uint16 flags);

	void   DrawScaledLow555(Pixel16 *data, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight,
		   		 uint16 transparency, Pixel16 outlineColor, uint16 flags, BOOL reverse);

	void   DrawFlashScaledLow555(Pixel16 *data, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight,
							 uint16 transparency, Pixel16 outlineColor, uint16 flags, BOOL reverse);

	inline Pixel16	average_555(Pixel16 pixel1, Pixel16 pixel2, Pixel16 pixel3, Pixel16 pixel4);
	inline Pixel16	average_555(Pixel16 pixel1, Pixel16 pixel2);





	
	
	
	void Sprite::__Copy_16(PixelAddress &dest,PixelAddress &src,sint32 dest_inc,sint32 src_inc,sint32 num)
	{
		while(num)
		{
			*dest.w_ptr = *src.w_ptr;
			dest.w_ptr += dest_inc;
			src.w_ptr  += src_inc;
			num--;
		}
	};

	
	
	
	void Sprite::__Copy_32(PixelAddress &dest,PixelAddress &src,sint32 dest_inc,sint32 src_inc,sint32 num)
	{
		while(num)
		{
			*dest.l_ptr = *src.l_ptr;
			dest.l_ptr += dest_inc;
			src.l_ptr  += src_inc;
			num--;
		}
	};






	inline void __Additive_565_16	(PixelAddress &dest,PixelAddress &src,sint32 dest_inc,sint32 src_inc,sint32 num);
	inline void __Desaturate_565_16	(PixelAddress &dest,PixelAddress &src,sint32 dest_inc,sint32 src_inc,sint32 num);
	inline void __Blend_565_16		(PixelAddress &dest,PixelAddress &src,sint32 dest_inc,sint32 src_inc,sint32 num,uint16 alpha);
	inline void __BlendFast_565_16	(PixelAddress &dest,PixelAddress &src,sint32 dest_inc,sint32 src_inc,sint32 num,uint16 transparency);
	inline void __Shadow_565_16		(PixelAddress &dest,PixelAddress &src,sint32 dest_inc,sint32 src_inc,sint32 num);
	inline void __Shadow_565_16		(PixelAddress &dest,sint32 dest_inc,sint32 num);





	inline void __Additive_565_32	(PixelAddress &dest,PixelAddress &src,sint32 dest_inc,sint32 src_inc,sint32 num);
	inline void __Desaturate_565_32	(PixelAddress &dest,PixelAddress &src,sint32 dest_inc,sint32 src_inc,sint32 num);
	inline void __Blend_565_32		(PixelAddress &dest,PixelAddress &src,sint32 dest_inc,sint32 src_inc,sint32 num,uint16 alpha);
	inline void __BlendFast_565_32	(PixelAddress &dest,PixelAddress &src,sint32 dest_inc,sint32 src_inc,sint32 num,uint16 transparency);
	inline void __Shadow_565_32		(PixelAddress &dest,PixelAddress &src,sint32 dest_inc,sint32 src_inc,sint32 num);
	inline void __Shadow_565_32		(PixelAddress &dest,sint32 dest_inc,sint32 num);





	inline void __Additive_555_16	(PixelAddress &dest,PixelAddress &src,sint32 dest_inc,sint32 src_inc,sint32 num);
	inline void __Desaturate_555_16	(PixelAddress &dest,PixelAddress &src,sint32 dest_inc,sint32 src_inc,sint32 num);
	inline void __Blend_555_16		(PixelAddress &dest,PixelAddress &src,sint32 dest_inc,sint32 src_inc,sint32 num,uint16 alpha);
	inline void __BlendFast_555_16	(PixelAddress &dest,PixelAddress &src,sint32 dest_inc,sint32 src_inc,sint32 num,uint16 transparency);
	inline void __Shadow_555_16		(PixelAddress &dest,PixelAddress &src,sint32 dest_inc,sint32 src_inc,sint32 num);
	inline void __Shadow_555_16		(PixelAddress &dest,sint32 dest_inc,sint32 num);





	inline void __Additive_555_32	(PixelAddress &dest,PixelAddress &src,sint32 dest_inc,sint32 src_inc,sint32 num);
	inline void __Desaturate_555_32	(PixelAddress &dest,PixelAddress &src,sint32 dest_inc,sint32 src_inc,sint32 num);
	inline void __Blend_555_32		(PixelAddress &dest,PixelAddress &src,sint32 dest_inc,sint32 src_inc,sint32 num,uint16 alpha);
	inline void __BlendFast_555_32	(PixelAddress &dest,PixelAddress &src,sint32 dest_inc,sint32 src_inc,sint32 num,uint16 transparency);
	inline void __Shadow_555_32		(PixelAddress &dest,PixelAddress &src,sint32 dest_inc,sint32 src_inc,sint32 num);
	inline void __Shadow_555_32		(PixelAddress &dest,sint32 dest_inc,sint32 num);




	inline Pixel16	average(Pixel16 pixel1, Pixel16 pixel2, Pixel16 pixel3, Pixel16 pixel4);
	inline Pixel16	average(Pixel16 pixel1, Pixel16 pixel2);

	sint32			ReadTag(sint32 *mode, Pixel16 **rowData, sint32 *alpha);


	BOOL	HitTestLow(POINT mousePt, Pixel16 *frame, sint32 drawX, sint32 drawY, sint32 width, sint32 height,
						uint16 transparency, Pixel16 outlineColor, uint16 flags);
	BOOL	HitTestLowReversed(POINT mousePt, Pixel16 *frame, sint32 drawX, sint32 drawY, sint32 width, sint32 height,
						uint16 transparency, Pixel16 outlineColor, uint16 flags);
	BOOL	HitTestScaledLow(POINT mousePt, Pixel16 *data, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight,
							 uint16 transparency, Pixel16 outlineColor, uint16 flags, BOOL reverse);


protected:
	SPRITETYPE		m_type;

	uint16			m_width, m_height;
	POINT			m_hotPoint;

	uint16			m_numFrames;
	Pixel16			**m_frames;
	size_t                  *m_framesSizes;
	Pixel16			**m_miniframes;
	size_t                  *m_miniframesSizes;
	uint16			m_firstFrame;
	uint16			m_currentFrame;

	
	aui_Surface		*m_surface;
	sint32			m_surfWidth;
	sint32			m_surfHeight;
	sint32			m_surfPitch;
	uint8			*m_surfBase;

};

#endif
