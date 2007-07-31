










#ifndef __AUIPIXEL_H__
#define __AUIPIXEL_H__


class aui_Surface;


#define VECTOR_SIZE					97
#define FS_SCALE					1024

#define k_AUI_PIXEL_MAXSCALAR		128
#define k_AUI_PIXEL_SCALARSHIFT		7



class aui_Pixel
{
public:
	static AUI_ERRCODE Convert24To16(
		aui_Surface *surface16,
		uint8 *buf24,
		uint32 srcWidth,
		uint32 srcHeight,
		uint32 srcPitch );
	static AUI_ERRCODE Convert24To16Dither(
		uint16 *buf16,
		uint8 *buf24, 
		uint32 cols, uint32 rows, 
		uint32 skipr = -1, 
		uint32 skipg = -1, 
		uint32 skipb = -1);
	static AUI_ERRCODE Convert8To16(
		aui_Surface *surface16,
		uint8 *buf8,
		uint32 srcWidth,
		uint32 srcHeight,
		uint32 srcPitch,
		RGBQUAD *rgbq );
	static uint16 Get16BitRGB( uint8 red, uint8 green, uint8 blue );
	static uint8 GetPaletteIndexedColor( uint8 red, uint8 green, uint8 blue, RGBQUAD *rgbq );
	static uint8 GetPaletteIndexedColor( uint8 red, uint8 green, uint8 blue, HPALETTE *hpal );

protected:

	static double **m_edge;
	static sint32 m_lastRand;
	static sint32 m_randRector[VECTOR_SIZE];

	static double **MakeEdge(uint8 *image, sint32 width, sint32 height);
	static void SeedRandom(uint32 seed);
	static sint32 ScaleRandom();
	static sint32 Random();
	static sint32 Shift(sint32 x, sint32 add_noise, sint32 col, sint32 row);

	static double **Alloc2D(sint32 width, sint32 height);
	static void Free2D(double **d);
	static double **ImageToDouble(uint8 *image24, sint32 width, sint32 height);
	static void DoubleToImage(double **fimage, sint8 *image24,
							  sint32 width, sint32 height);
	static void Gradient(double **in_image, double **out_image, 
						 sint32 width, sint32 height);

public:
	
	
	
	
	static uint16 Blend555( uint16 pixel1, uint16 pixel2, uint32 blend )
	{
		uint32 rb2, g2;
		uint32 rb0, g0;

		rb2 = (pixel2 & 0x7C1F);
		rb0 = (((rb2<<5)+blend*((pixel1 & 0x7C1F)-rb2))>>5) & 0x7C1F;

		g2 = (pixel2 & 0x03E0);
		g0 = (((g2<<5)+blend*((pixel1 & 0x03E0)-g2))>>5) & 0x07E0;

		return uint16(rb0 | g0);
	}

	
	
	
	
	static uint16 Blend565( uint16 pixel1, uint16 pixel2, uint32 blend )
	{
		uint32 rb2, g2;
		uint32 rb0, g0;

		rb2 = (pixel2 & 0xF81F);

		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		rb0 = (((rb2<<5)+(blend*((pixel1 & 0xF81F)-rb2)))>>5) & 0xF81F;

		g2 = (pixel2 & 0x07E0);
		g0 = (((g2<<5)+blend*((pixel1 & 0x07E0)-g2))>>5) & 0x07E0;

		return uint16(rb0 | g0);
	}

	
	
	
	static uint16 Darken555( uint16 pixel, sint32 scalar )
	{
		const sint32 newScalar = k_AUI_PIXEL_MAXSCALAR - scalar;

		sint32 r32 = ( ( ( pixel & 0x7C00 ) >> 10 ) * newScalar ) >>
			k_AUI_PIXEL_SCALARSHIFT;
		sint32 g32 = ( ( ( pixel & 0x03E0 ) >> 5 ) * newScalar ) >>
			k_AUI_PIXEL_SCALARSHIFT;
		sint32 b32 = ( ( pixel & 0x001F ) * newScalar ) >>
			k_AUI_PIXEL_SCALARSHIFT;

		if ( scalar < 0 )
		{
			if ( r32 > 0x001F ) r32 = 0x001F;
			if ( g32 > 0x001F ) g32 = 0x001F;
			if ( b32 > 0x001F ) b32 = 0x001F;
		}

		return uint16(( r32 << 10 ) | ( g32 << 5)  | b32);
	}

	
	
	
	static uint16 Darken565( uint16 pixel, sint32 scalar )
	{
		const sint32 newScalar = k_AUI_PIXEL_MAXSCALAR - scalar;

		sint32 r32 = ( ( ( pixel & 0xF800 ) >> 11 ) * newScalar ) >>
			k_AUI_PIXEL_SCALARSHIFT;
		sint32 g32 = ( ( ( pixel & 0x07E0 ) >> 5 ) * newScalar ) >>
			k_AUI_PIXEL_SCALARSHIFT;
		sint32 b32 = ( ( pixel & 0x001F ) * newScalar ) >>
			k_AUI_PIXEL_SCALARSHIFT;

		if ( scalar < 0 )
		{		
			if ( r32 > 0x001F ) r32 = 0x001F;
			if ( g32 > 0x003F ) g32 = 0x003F;
			if ( b32 > 0x001F ) b32 = 0x001F;
		}

		return uint16(( r32 << 11 ) | ( g32 << 5 ) | b32);
	}
};


#endif 
