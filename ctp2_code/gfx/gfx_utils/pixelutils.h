#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __PIXELUTILS_H__
#define __PIXELUTILS_H__

#define BLEND_LEVELS		16
#define BLEND_MAX_VALUE		64
#define RGB_VALUES			32768
#define k_MAX_PERCENT		128
#define k_PERCENT_SHIFT		7

#include "pixeltypes.h"






typedef union
{
  Pixel8  *b_ptr;
  Pixel16 *w_ptr;
  Pixel32 *l_ptr;

} PixelAddress;

void pixelutils_Initialize(void);

Pixel16 *RGB32ToRGB16(char *buf, uint16 width, uint16 height);

void RGB32Components(Pixel32 pixel, Pixel16 *r, Pixel16 *g, Pixel16 *b, Pixel16 *a);
Pixel32 ComponentsToRGB32(Pixel16 r, Pixel16 g, Pixel16 b, Pixel16 a) ;
void RGB32Info(Pixel32 pixel, Pixel16 *outPixel, unsigned char *alpha);
void pixelutils_ComputeBlendTable(void);





inline Pixel16 pixelutils_Blend(Pixel16 pixel1, Pixel16 pixel2, short blend)
{
	short			r1, g1, b1, r2, g2, b2;
	short			r0, g0, b0;
	extern sint32		g_is565Format;
	extern short	gPixelTable[BLEND_LEVELS][BLEND_MAX_VALUE][BLEND_MAX_VALUE];

	if (g_is565Format)
	{
		r1 = (short)((pixel1 & 0xF800) >> 10) ;
		g1 = (short)((pixel1 & 0x07E0) >> 5);
		b1 = (short)((pixel1 & 0x001F) << 1) ;

		r2 = (short)((pixel2 & 0xF800) >> 10);
		g2 = (short)((pixel2 & 0x07E0) >> 5);
		b2 = (short)((pixel2 & 0x001F) << 1);

		r0 = (short)((gPixelTable[blend][r1][r2] & 0xFFFE) << 10) ;
		g0 = (short)(gPixelTable[blend][g1][g2] << 5) ;
		b0 = (short)(gPixelTable[blend][b1][b2] >> 1) ;
	}
	else
	{
		r1 = (short)((pixel1 & 0x7C00) >> 9) ;
		g1 = (short)((pixel1 & 0x03E0) >> 4) ;
		b1 = (short)((pixel1 & 0x001F) << 1) ;

		r2 = (short)((pixel2 & 0x7C00) >> 9) ;
		g2 = (short)((pixel2 & 0x03E0) >> 4) ;
		b2 = (short)((pixel2 & 0x001F) << 1) ;

		r0 = (short)((gPixelTable[blend][r1][r2] & 0xFFFE) << 9) ;
		g0 = (short)((gPixelTable[blend][g1][g2] & 0xFFFE) << 4) ;
		b0 = (short)(gPixelTable[blend][b1][b2] >> 1) ;
	}

	return static_cast<Pixel16>(r0 | g0 | b0);
}

inline Pixel16 pixelutils_Additive(Pixel16 pixel1, Pixel16 pixel2)
{
	extern sint32		g_is565Format;

	Pixel16				r, g, b, sum = (short)(pixel2 & 0x1F) ;

	if (g_is565Format)
	{
		r = ((pixel1 & 0xF800) >> 11) + sum;
		g = ((pixel1 & 0x07E0) >> 5)  + (sum << 1);
		b = ((pixel1 & 0x001F))       + sum;

		if (r > 0x001F) r = 0x001F;
		if (g > 0x003F) g = 0x003F;
		if (b > 0x001F) b = 0x001F;

		return static_cast<Pixel16>((r<<11) | (g<<5) | b);
	}
	else
	{
		r = ((pixel1 & 0x7C00) >> 10) + sum;
		g = ((pixel1 & 0x03E0) >> 5)  + sum;
		b = ((pixel1 & 0x001F))       + sum;

		if (r > 0x001F) r = 0x001F;
		if (g > 0x001F) g = 0x001F;
		if (b > 0x001F) b = 0x001F;

		return static_cast<Pixel16>((r<<10) | (g<<5) | b);
	}
}

inline Pixel16 pixelutils_BlendFast(sint32 pixel1, sint32 pixel2, sint32 blend)
{
	sint32 rb2, g2;
	sint32 rb0, g0;
	extern sint32		g_is565Format;

	if (g_is565Format)
	{
		rb2 = (pixel2 & 0xF81F);







		rb0 = ( ((rb2<<5) + (blend * ((pixel1 & 0xF81F)-rb2) )) >> 5)  & 0xF81F;

		g2 = (pixel2 & 0x07E0);
		g0 = (((g2<<5)+blend*((pixel1 & 0x07E0)-g2))>>5) & 0x07E0;
	}
	else
	{
		rb2 = (pixel2 & 0x7C1F);
		rb0 = (((rb2<<5)+blend*((pixel1 & 0x7C1F)-rb2))>>5) & 0x7C1F;

		g2 = (pixel2 & 0x03E0);
		g0 = (((g2<<5)+blend*((pixel1 & 0x03E0)-g2))>>5) & 0x07E0;
	}

	Pixel16 pixel = (Pixel16)(rb0|g0);
	return ((!pixel) ? static_cast<Pixel16>(0x0001) : pixel);
}

inline Pixel16 pixelutils_Shadow(Pixel16 pixel)
{
	extern sint32		g_is565Format;

	if (g_is565Format)
      return static_cast<Pixel16>((pixel&0xF7DF)>>1);
	else
	{
	    short				r, g, b;

		r = (pixel & 0x7C00) >> 11;
		g = (pixel & 0x03E0) >> 6;
		b = (pixel & 0x001F) >> 1;

		Pixel16 p = static_cast<Pixel16>((r<<10) | (g<<5) | b);
		if (!p) p = 0x0001;
		return p;
	}
}

inline Pixel16 pixelutils_Lightening(Pixel16 pixel)
{
	short		r, g, b;
	extern sint32		g_is565Format;

	if (g_is565Format)
	{
		r = (pixel & 0xF800) >> 10;
		if (r > 0x001F)
			r = 0x001F;
		g = (pixel & 0x07E0) >> 4;
		if (g > 0x003F)
			g = 0x003F;
		b = (pixel & 0x001F) << 1;
		if (b > 0x001F)
			b = 0x001F;

		return static_cast<Pixel16>((r<<11) | (g<<5) | b);
	}
	else
	{
		r = (pixel & 0x7C00) >> 9;
		if (r > 0x001F)
			r = 0x001F;
		g = (pixel & 0x03E0) >> 4;
		if (g > 0x001F)
			g = 0x001F;
		b = (pixel & 0x001F) << 1;
		if (b > 0x001F)
			b = 0x001F;

		return static_cast<Pixel16>((r<<10) | (g<<5) | b);
	}
}

inline Pixel16 pixelutils_PercentDarken(Pixel16 pixel, sint32 percent)
{
	sint32 r32, g32, b32;
	sint16 r, g, b;
	sint32 newPercent = k_MAX_PERCENT - percent;
	extern sint32		g_is565Format;

	if (g_is565Format)
	{
		r32 = (pixel & 0xF800) >> 11;
		g32 = (pixel & 0x07E0) >> 5;
		b32 = (pixel & 0x001F);

		r32 = (r32 * newPercent) >> k_PERCENT_SHIFT;
		r = (sint16)r32;
		g32 = (g32 * newPercent) >> k_PERCENT_SHIFT;
		g = (sint16)g32;
		b32 = (b32 * newPercent) >> k_PERCENT_SHIFT;
		b = (sint16)b32;

		return static_cast<Pixel16>((r<<11) | (g<<5) | b);
	}
	else
	{
		r32 = (pixel & 0x7C00) >> 10;
		g32 = (pixel & 0x03E0) >> 5;
		b32 = (pixel & 0x001F);

		r32 = (r32 * newPercent) >> k_PERCENT_SHIFT;
		r = (sint16)r32;
		g32 = (g32 * newPercent) >> k_PERCENT_SHIFT;
		g = (sint16)g32;
		b32 = (b32 * newPercent) >> k_PERCENT_SHIFT;
		b = (sint16)b32;

		return static_cast<Pixel16>((r<<10) | (g<<5) | b);
	}
}

inline Pixel16 pixelutils_PercentLighten(Pixel16 pixel, sint32 percent)
{
	sint32 r32, g32, b32;
	sint16 r, g, b;
	sint32 newPercent = k_MAX_PERCENT + percent;
	extern sint32		g_is565Format;

	if (g_is565Format)
	{
		r32 = (pixel & 0xF800) >> 11;
		g32 = (pixel & 0x07E0) >> 5;
		b32 = (pixel & 0x001F);

		r32 = (r32 * newPercent) >> k_PERCENT_SHIFT;
		if (r32 > 0x001F)
			r32 = 0x001F;
		r = (sint16)r32;
		g32 = (g32 * newPercent) >> k_PERCENT_SHIFT;
		if (g32 > 0x003F)
			g32 = 0x003F;
		g = (sint16)g32;
		b32 = (b32 * newPercent) >> k_PERCENT_SHIFT;
		if (b32 > 0x001F)
			b32 = 0x001F;
		b = (sint16)b32;

		return static_cast<Pixel16>((r<<11) | (g<<5) | b);
	}
	else
	{
		r32 = (pixel & 0x7C00) >> 10;
		g32 = (pixel & 0x03E0) >> 5;
		b32 = (pixel & 0x001F);

		r32 = (r32 * newPercent) >> k_PERCENT_SHIFT;
		if (r32 > 0x001F)
			r32 = 0x001F;
		r = (sint16)r32;
		g32 = (g32 * newPercent) >> k_PERCENT_SHIFT;
		if (g32 > 0x001F)
			g32 = 0x001F;
		g = (sint16)g32;
		b32 = (b32 * newPercent) >> k_PERCENT_SHIFT;
		if (b32 > 0x001F)
			b32 = 0x001F;
		b = (sint16)b32;

		return static_cast<Pixel16>((r<<10) | (g<<5) | b);
	}
}




inline Pixel16 pixelutils_Convert565to555(Pixel16 pixel)
{
	extern sint32		g_is565Format;

	if (g_is565Format) return pixel;

	return static_cast<Pixel16>(((pixel & 0xFFC0) >> 1) | (pixel & 0x001F));
}

inline Pixel16 pixelutils_Convert555to565(Pixel16 pixel)
{
	extern sint32		g_is565Format;

	if (!g_is565Format) return pixel;

	return static_cast<Pixel16>(((pixel & 0x7FE0) << 1) | (pixel & 0x001F));
}

Pixel16 pixelutils_Convert565to555(Pixel16 pixel);
Pixel16 pixelutils_Convert555to565(Pixel16 pixel);







inline Pixel16 pixelutils_Blend_565(Pixel16 pixel1, Pixel16 pixel2, short blend)
{
	Pixel16			r1, g1, b1, r2, g2, b2;
	Pixel16			r0, g0, b0;
	extern short	gPixelTable[BLEND_LEVELS][BLEND_MAX_VALUE][BLEND_MAX_VALUE];

	r1 = ((pixel1 & 0xF800) >> 10) ;
	g1 = ((pixel1 & 0x07E0) >> 5);
	b1 = ((pixel1 & 0x001F) << 1) ;

	r2 = ((pixel2 & 0xF800) >> 10);
	g2 = ((pixel2 & 0x07E0) >> 5);
	b2 = ((pixel2 & 0x001F) << 1);

	r0 = ((gPixelTable[blend][r1][r2] & 0xFFFE) << 10) ;
	g0 = ( gPixelTable[blend][g1][g2] << 5) ;
	b0 = ( gPixelTable[blend][b1][b2] >> 1) ;

	return static_cast<Pixel16>(r0 | g0 | b0);

}

inline Pixel16 pixelutils_Additive_565(Pixel16 pixel1, Pixel16 pixel2)
{
	Pixel16				r, g, b, sum = (short)(pixel2 & 0x1F) ;

	r = ((pixel1 & 0xF800) >> 11) + sum;
	g = ((pixel1 & 0x07E0) >> 5)  + (sum << 1);
	b = ((pixel1 & 0x001F))       + sum;

	if (r > 0x001F) r = 0x001F;
	if (g > 0x003F) g = 0x003F;
	if (b > 0x001F) b = 0x001F;

	return static_cast<Pixel16>((r<<11) | (g<<5) | b);
}

inline Pixel16 pixelutils_BlendFast_565(sint32 pixel1, sint32 pixel2, sint32 blend)
{

	sint32 rb2, g2;
	sint32 rb0, g0;

	rb2 = (pixel2 & 0xF81F);





	rb0 = ( ((rb2<<5) + (blend * ((pixel1 & 0xF81F)-rb2) )) >> 5)  & 0xF81F;

	g2 = (pixel2 & 0x07E0);
	g0 = (((g2<<5)+blend*((pixel1 & 0x07E0)-g2))>>5) & 0x07E0;

	return static_cast<Pixel16>(rb0 | g0);
}

inline Pixel16 pixelutils_Shadow_565(Pixel16 pixel)
{
    return static_cast<Pixel16>((pixel&0xF7DF)>>1);
}

inline Pixel16 pixelutils_Lightening_565(Pixel16 pixel)
{
	short r, g, b;

	r = (pixel & 0xF800) >> 10;
	if (r > 0x001F)
   		r = 0x001F;

	g = (pixel & 0x07E0) >> 4;
	if (g > 0x003F)
   		g = 0x003F;

	b = (pixel & 0x001F) << 1;
	if (b > 0x001F)
   		b = 0x001F;

    return static_cast<Pixel16>((r<<11) | (g<<5) | b);
}

inline Pixel16 pixelutils_PercentDarken_565(Pixel16 pixel, sint32 percent)
{
	sint32 r32, g32, b32;
	sint16 r, g, b;
	sint32 newPercent = k_MAX_PERCENT - percent;

	r32 = (pixel & 0xF800) >> 11;
	g32 = (pixel & 0x07E0) >> 5;
	b32 = (pixel & 0x001F);

	r32 = (r32 * newPercent) >> k_PERCENT_SHIFT;
	r = (sint16)r32;
	g32 = (g32 * newPercent) >> k_PERCENT_SHIFT;
	g = (sint16)g32;
	b32 = (b32 * newPercent) >> k_PERCENT_SHIFT;
	b = (sint16)b32;

	return static_cast<Pixel16>((r<<11) | (g<<5) | b);
}

inline Pixel16 pixelutils_PercentLighten_565(Pixel16 pixel, sint32 percent)
{
	sint32 r32, g32, b32;
	sint16 r, g, b;
	sint32 newPercent = k_MAX_PERCENT + percent;

	r32 = (pixel & 0xF800) >> 11;
	g32 = (pixel & 0x07E0) >> 5;
	b32 = (pixel & 0x001F);

	r32 = (r32 * newPercent) >> k_PERCENT_SHIFT;
	if (r32 > 0x001F)
		r32 = 0x001F;
	r = (sint16)r32;
	g32 = (g32 * newPercent) >> k_PERCENT_SHIFT;
	if (g32 > 0x003F)
		g32 = 0x003F;
	g = (sint16)g32;
	b32 = (b32 * newPercent) >> k_PERCENT_SHIFT;
	if (b32 > 0x001F)
		b32 = 0x001F;
	b = (sint16)b32;

	return static_cast<Pixel16>((r<<11) | (g<<5) | b);
}

inline Pixel16 pixelutils_Desaturate_565(Pixel16 pixel)
{
	sint32 ave = (((pixel & 0xF800) >> 11) + ((pixel & 0x07E0) >> 6) + (pixel & 0x001F)+128)>>2;

	return ((Pixel16)(((ave & 0x1F) << 11) | ((ave & 0x3F) << 6) | (ave & 0x1F)));
}

Pixel32 pixelutils_Blend32_565(Pixel32 pixel1, Pixel32 pixel2, short blend);
Pixel32 pixelutils_Additive32_565(Pixel32 pixel1, Pixel32 pixel2);
Pixel32 pixelutils_BlendFast32_565(sint32 pixel1, sint32 pixel2, sint32 blend);

inline Pixel32 pixelutils_Shadow32_565(Pixel32 pixel)
{
  return ((pixel&0xF7DEF7DE)>>1);
}


Pixel32 pixelutils_Lightening32_565(Pixel16 pixel);
Pixel32 pixelutils_PercentDarken32_565(Pixel32 pixel, sint32 percent);
Pixel32 pixelutils_PercentLighten32_565(Pixel32 pixel, sint32 percent);
Pixel16 pixelutils_Desaturate_565(Pixel16 pixel);





inline Pixel16 pixelutils_Blend_555(Pixel16 pixel1, Pixel16 pixel2, short blend)
{
	Pixel16			r1, g1, b1, r2, g2, b2;
	Pixel16			r0, g0, b0;
	extern short	gPixelTable[BLEND_LEVELS][BLEND_MAX_VALUE][BLEND_MAX_VALUE];

	r1 = ((pixel1 & 0x7C00) >> 9) ;
	g1 = ((pixel1 & 0x03E0) >> 4) ;
	b1 = ((pixel1 & 0x001F) << 1) ;

	r2 = ((pixel2 & 0x7C00) >> 9) ;
	g2 = ((pixel2 & 0x03E0) >> 4) ;
	b2 = ((pixel2 & 0x001F) << 1) ;

	r0 = ((gPixelTable[blend][r1][r2] & 0xFFFE) << 9) ;
	g0 = ((gPixelTable[blend][g1][g2] & 0xFFFE) << 4) ;
	b0 = (gPixelTable[blend][b1][b2] >> 1) ;

	return static_cast<Pixel16>(r0 | g0 | b0);
}

inline Pixel16 pixelutils_Additive_555(Pixel16 pixel1, Pixel16 pixel2)
{
	Pixel16				r, g, b, sum = (short)(pixel2 & 0x1F) ;

	r = ((pixel1 & 0x7C00) >> 10) + sum;
	g = ((pixel1 & 0x03E0) >> 5)  + sum;
	b = ((pixel1 & 0x001F))       + sum;

	if (r > 0x001F) r = 0x001F;
	if (g > 0x001F) g = 0x001F;
	if (b > 0x001F) b = 0x001F;

	return static_cast<Pixel16>((r<<10) | (g<<5) | b);
}

inline Pixel16 pixelutils_BlendFast_555(sint32 pixel1, sint32 pixel2, sint32 blend)
{
	sint32 rb2, g2;
	sint32 rb0, g0;

	rb2 = (pixel2 & 0x7C1F);
	rb0 = (((rb2<<5)+blend*((pixel1 & 0x7C1F)-rb2))>>5) & 0x7C1F;

	g2 = (pixel2 & 0x03E0);
	g0 = (((g2<<5)+blend*((pixel1 & 0x03E0)-g2))>>5) & 0x07E0;

	return static_cast<Pixel16>(rb0 | g0);
}

inline Pixel16 pixelutils_Shadow_555(Pixel16 pixel)
{
    return static_cast<Pixel16>((pixel&0x7BDF)>>1);
}

inline Pixel16 pixelutils_Lightening_555(Pixel16 pixel)
{
	short		r, g, b;
	r = (pixel & 0x7C00) >> 9;
	if (r > 0x001F)
		r = 0x001F;
	g = (pixel & 0x03E0) >> 4;
	if (g > 0x001F)
		g = 0x001F;
	b = (pixel & 0x001F) << 1;
	if (b > 0x001F)
		b = 0x001F;

	return static_cast<Pixel16>((r<<10) | (g<<5) | b);
}

inline Pixel16 pixelutils_PercentDarken_555(Pixel16 pixel, sint32 percent)
{
	sint32 r32, g32, b32;
	sint16 r, g, b;
	sint32 newPercent = k_MAX_PERCENT - percent;

	r32 = (pixel & 0x7C00) >> 10;
	g32 = (pixel & 0x03E0) >> 5;
	b32 = (pixel & 0x001F);

	r32 = (r32 * newPercent) >> k_PERCENT_SHIFT;
	r = (sint16)r32;
	g32 = (g32 * newPercent) >> k_PERCENT_SHIFT;
	g = (sint16)g32;
	b32 = (b32 * newPercent) >> k_PERCENT_SHIFT;
	b = (sint16)b32;

	return static_cast<Pixel16>((r<<10) | (g<<5) | b);
}

inline Pixel16 pixelutils_PercentLighten_555(Pixel16 pixel, sint32 percent)
{
	sint32 r32, g32, b32;
	sint16 r, g, b;
	sint32 newPercent = k_MAX_PERCENT + percent;

	r32 = (pixel & 0x7C00) >> 10;
	g32 = (pixel & 0x03E0) >> 5;
	b32 = (pixel & 0x001F);

	r32 = (r32 * newPercent) >> k_PERCENT_SHIFT;
	if (r32 > 0x001F)
		r32 = 0x001F;
	r = (sint16)r32;
	g32 = (g32 * newPercent) >> k_PERCENT_SHIFT;
	if (g32 > 0x001F)
		g32 = 0x001F;
	g = (sint16)g32;
	b32 = (b32 * newPercent) >> k_PERCENT_SHIFT;
	if (b32 > 0x001F)
		b32 = 0x001F;
	b = (sint16)b32;

	return static_cast<Pixel16>((r<<10) | (g<<5) | b);
}


inline Pixel16 pixelutils_Desaturate_555(Pixel16 pixel)
{
	Pixel16 const ave =
        static_cast<Pixel16>((((pixel & 0x7C00) >> 10) + ((pixel & 0x03E0) >> 5) + (pixel & 0x001F)) / 3);

	return static_cast<Pixel16>(((ave & 0x1F) << 10) | ((ave & 0x1F) << 5) | (ave & 0x1f));
}

Pixel32 pixelutils_Blend32_555(Pixel32 pixel1, Pixel32 pixel2, short blend);
Pixel32 pixelutils_Additive32_555(Pixel32 pixel1, Pixel32 pixel2);
Pixel32 pixelutils_BlendFast32_555(Pixel32 pixel1,Pixel32 pixel2, sint32 blend);

inline Pixel32 pixelutils_Shadow32_555(Pixel32 pixel)
{
    return static_cast<Pixel32>((pixel&0x7BDE7BDE)>>1);
}

Pixel32 pixelutils_Lightening32_555(Pixel32 pixel);
Pixel32 pixelutils_PercentDarken32_555(Pixel32 pixel, sint32 percent);
Pixel32 pixelutils_PercentLighten32_555(Pixel32 pixel, sint32 percent);

Pixel16 pixelutils_Desaturate(Pixel16 pixel);
Pixel16 pixelutils_RGB(int r,int g,int b);
void pixelutils_ComputeRGBTable(void);

#endif
