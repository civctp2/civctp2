










#include "c3.h"
#include "aui_ui.h"
#include "aui_surface.h"

#include "aui_pixel.h"



sint32 aui_Pixel::m_lastRand = 0;
sint32 aui_Pixel::m_randRector[VECTOR_SIZE] = {0};
double **aui_Pixel::m_edge = NULL;


uint16 aui_Pixel::Get16BitRGB( uint8 red, uint8 green, uint8 blue )
{
	if ( g_ui->PixelFormat() == AUI_SURFACE_PIXELFORMAT_555 )
		return
			( ( red >> 3 ) << 10 )
		|	( ( green >> 3 ) << 5 )
		|	( blue >> 3 );
	else 
		return
			( ( red >> 3 ) << 11 )
		|	( ( green >> 3 ) << 6 )
		|	( blue >> 3 );
}

uint8 aui_Pixel::GetPaletteIndexedColor( uint8 red, uint8 green, uint8 blue, HPALETTE *hpal )
{
	uint16 valMain, valCompare = 0;
	uint8 r, g, b;
	uint8 color = 0;
	sint32 diff = INT_MAX;
	AUI_SURFACE_PIXELFORMAT pf = g_ui->PixelFormat();
	PALETTEENTRY *pe;

	pe = new PALETTEENTRY[ 256 ];
	Assert( pe != NULL );
	if ( !pe ) return 0;
	
	
	valMain = GetPaletteEntries( *hpal, 0, 256, pe );
	Assert( valMain == 256 );
	if ( valMain != 256 ) {
		delete[256] pe;
		return 0;
	}

	valMain = 0;
	valMain |= ( red >> 3 ) << 10;
	valMain |= ( green >> 3 ) << 5;
	valMain |= ( blue >> 3 );

	for ( uint8 i = 0; i < 256; i++ ) {
		valCompare = 0;
		b = pe[i].peBlue;
		g = pe[i].peGreen;
		r = pe[i].peRed;
		
		valCompare |= ( r >> 3 ) << 10;
		valCompare |= ( g >> 3 ) << 5;
		valCompare |= ( b >> 3 );

		if ( abs ( valCompare - valMain ) < diff ) {
			color = i;
			diff = abs( valCompare - valMain );
			if ( diff == 0 ) break;
		}

	}
	delete[256] pe;
	return color;

}

uint8 aui_Pixel::GetPaletteIndexedColor( uint8 red, uint8 green, uint8 blue, RGBQUAD *rgbq )
{
	uint16 valMain, valCompare;
	uint8 r, g, b;
	uint8 color = 0;
	sint32 diff = INT_MAX;
	AUI_SURFACE_PIXELFORMAT pf = g_ui->PixelFormat();

	valMain |= ( red >> 3 ) << 10;
	valMain |= ( green >> 3 ) << 5;
	valMain |= ( blue >> 3 );

	for ( uint8 i = 0; i < 256; i++ ) {
		valCompare = 0;
		b = rgbq[i].rgbBlue;
		g = rgbq[i].rgbGreen;
		r = rgbq[i].rgbRed;
		
		valCompare |= ( r >> 3 ) << 10;
		valCompare |= ( g >> 3 ) << 5;
		valCompare |= ( b >> 3 );

		if ( abs ( valCompare - valMain ) < diff ) {
			color = i;
			diff = abs( valCompare - valMain );
			if ( diff == 0 ) break;
		}

	}
	
	return color;

}

AUI_ERRCODE aui_Pixel::Convert24To16Dither(
							 uint16 *buf16,
							 uint8 *buf24, 
							 uint32 cols, 
							 uint32 rows, 
							 uint32 skipr, 
							 uint32 skipg, 
							 uint32 skipb)
{
    uint32 col;
    uint32 row;
	uint32 limitcol;
	sint32 err;
    uint32 fr, fg, fb;
    sint32 sr, sg, sb;
    sint32 tr, tg, tb;
    uint8 *fp;
    uint16 *tp;
    sint32 fs_direction;
	sint32 *thisrerr, *thisgerr, *thisberr;
	sint32 *nextrerr, *nextgerr, *nextberr;
	sint32 *temperr;

    
    m_edge = MakeEdge(buf24, cols, rows);

	
    thisrerr = new sint32[ cols + 2 ];
	thisgerr = new sint32[ cols + 2 ];
    thisberr = new sint32[ cols + 2 ];
    nextrerr = new sint32[ cols + 2 ];
    nextgerr = new sint32[ cols + 2 ];
    nextberr = new sint32[ cols + 2 ];

	if (!thisrerr || !thisgerr || !thisberr || 
		!nextrerr || !nextgerr || !nextberr) {
		if (thisrerr)
			delete thisrerr;
		if (thisgerr)
			delete thisgerr;
		if (thisberr)
			delete thisberr;
		if (nextrerr)
			delete nextrerr;
		if (nextgerr)
			delete nextgerr;
		if (nextberr)
			delete nextberr;
		return AUI_ERRCODE_LOADFAILED;
	}

    SeedRandom(GetTickCount());
	for ( col = 0; col < cols + 2; ++col ) {
	    thisrerr[col] = ScaleRandom();
	    thisgerr[col] = ScaleRandom();
	    thisberr[col] = ScaleRandom();
	    
    }
	fs_direction = 1;










    for ( row = 0; row < rows; ++row ) {
        memset(nextrerr, 0, (cols+2) * sizeof(sint32));
        memset(nextgerr, 0, (cols+2) * sizeof(sint32));
        memset(nextberr, 0, (cols+2) * sizeof(sint32));
        if ( fs_direction ) {
            col = 0;
            limitcol = cols;
            fp = buf24 + (3*row*cols);
            tp = buf16 + (row*cols);
	    } else {
            col = cols - 1;
            limitcol = -1;
            fp = buf24 + (3*row*cols) + (3*col);
            tp = buf16 + (row*cols) + col;
	    }
        do {
            fb = fp[0];
            fg = fp[1];
            fr = fp[2];




            if ((skipb != -1) && 
                (fb == skipb) && (fg == skipg) && (fr == skipr)) {
                
                sr = fr;
                sg = fg;
                sb = fb;
                tr = Shift(sr, 0, col, row);
                tg = Shift(sg, 0, col, row);
                tb = Shift(sb, 0, col, row);
            } else {                
                
                sr = fr + thisrerr[col + 1] / FS_SCALE;
                sg = fg + thisgerr[col + 1] / FS_SCALE;
                sb = fb + thisberr[col + 1] / FS_SCALE;
                if      (sr < 0)   sr = 0;
                else if (sr > 255) sr = 255;
                if      (sg < 0)   sg = 0;
                else if (sg > 255) sg = 255;
                if      (sb < 0)   sb = 0;
                else if (sb > 255) sb = 255;

                
                tr = Shift(sr, 1, col, row);
                tg = Shift(sg, 1, col, row);
                tb = Shift(sb, 1, col, row);
            }


            *tp = (uint16)((tr << 10) | (tg << 5) | tb);

            
            if ( fs_direction ) {
                err = ( sr - (tr << 3) ) * FS_SCALE;
                thisrerr[col + 2] += ( err * 7 ) / 16;
                nextrerr[col    ] += ( err * 3 ) / 16;
                nextrerr[col + 1] += ( err * 5 ) / 16;
                nextrerr[col + 2] += ( err     ) / 16;
                err = ( sg - (tg << 3) ) * FS_SCALE;
                thisgerr[col + 2] += ( err * 7 ) / 16;
                nextgerr[col    ] += ( err * 3 ) / 16;
                nextgerr[col + 1] += ( err * 5 ) / 16;
                nextgerr[col + 2] += ( err     ) / 16;
                err = ( sb - (tb << 3) ) * FS_SCALE;
                thisberr[col + 2] += ( err * 7 ) / 16;
                nextberr[col    ] += ( err * 3 ) / 16;
                nextberr[col + 1] += ( err * 5 ) / 16;
                nextberr[col + 2] += ( err     ) / 16;
		    } else {
                err = ( sr - (tr << 3) ) * FS_SCALE;
                thisrerr[col    ] += ( err * 7 ) / 16;
                nextrerr[col + 2] += ( err * 3 ) / 16;
                nextrerr[col + 1] += ( err * 5 ) / 16;
                nextrerr[col    ] += ( err     ) / 16;
                err = ( sg - (tg << 3) ) * FS_SCALE;
                thisgerr[col    ] += ( err * 7 ) / 16;
                nextgerr[col + 2] += ( err * 3 ) / 16;
                nextgerr[col + 1] += ( err * 5 ) / 16;
                nextgerr[col    ] += ( err     ) / 16;
                err = ( sb - (tb << 3) ) * FS_SCALE;
                thisberr[col    ] += ( err * 7 ) / 16;
                nextberr[col + 2] += ( err * 3 ) / 16;
                nextberr[col + 1] += ( err * 5 ) / 16;
                nextberr[col    ] += ( err     ) / 16;
		    }

            if ( fs_direction ) {
                ++col;
                fp += 3;
                tp += 1;
            } else {
                --col;
                fp -= 3;
                tp -= 1;
            }
	    } while ( col != limitcol );

	    temperr = thisrerr;
	    thisrerr = nextrerr;
	    nextrerr = temperr;
	    temperr = thisgerr;
	    thisgerr = nextgerr;
	    nextgerr = temperr;
	    temperr = thisberr;
	    thisberr = nextberr;
	    nextberr = temperr;
	    fs_direction = ! fs_direction;
	}

    delete thisrerr;
    delete thisgerr;
    delete thisberr;
    delete nextrerr;
    delete nextgerr;
    delete nextberr;

    Free2D(m_edge);

	return AUI_ERRCODE_OK;
}


sint32 aui_Pixel::Shift(sint32 x, sint32 add_noise, sint32 col, sint32 row)
{
    sint32 r;
    double rscale;
    if (add_noise) {
        
        r = Random() * Random();
        if (Random() > (RAND_MAX / 2)) 
            r = -r;
        rscale = 2.0 - (50.0 * m_edge[row][col]);
        if (rscale < 1.0)
            rscale = 1.0;
        r /= (sint32)((double)(RAND_MAX * RAND_MAX) / rscale);
        x = ((x+4) >> 3) + r;
    } else {
        x = ((x+4) >> 3);
    }
    if      (x < 0)   x = 0;
    else if (x > 31)  x = 31;
    return(x);
}


sint32 aui_Pixel::ScaleRandom()
{
    
    return ((Random() >> 4) - FS_SCALE);
}
double **aui_Pixel::Alloc2D(sint32 width, sint32 height)
{
   double *d1, **d2;
   sint32 y;
   

   d2 = (double **) new double *[height];
   if (d2 == NULL)
       return(NULL);


   d1 = (double *) new double[width*height];
   if (d1 == NULL)
       return(NULL);
       
   for (y=0; y<height; y++) {
       d2[y] = d1 + y*width;
   }

   return(d2);
}

void aui_Pixel::Free2D(double **d)
{
	delete d[0];
	delete d;
    


}

double **aui_Pixel::ImageToDouble(uint8 *image24, sint32 width, sint32 height)
{
    double **data;
    sint32 i, j;
    uint8 *p;
    double val;


    data = Alloc2D(width, height);
    if (data == NULL)
        return(NULL);

    p = image24;
    for(j = 0; j < height; j++) {
        for(i = 0; i < width; i++) {
            val  = 0.212671 * *p++;
            val += 0.715160 * *p++;
            val += 0.072169 * *p++;
            data[j][i] = val / 255.0;
        }
    }
    return(data);
}

void aui_Pixel::DoubleToImage(double **fimage, sint8 *image24, 
                    sint32 width, sint32 height)
{
    sint32 i, j;
    sint8 *p;
    sint8 val;

    p = image24;
    for(j = 0; j < height; j++) {
        for(i = 0; i < width; i++) {
            val = (sint8)((fimage[j][i] * 255.0) + 0.5);
            *p++ = val;
            *p++ = val;
            *p++ = val;
        }
    }
}


void aui_Pixel::Gradient(double **in_image, double **out_image, 
              sint32 width, sint32 height)
{
   sint32          x, y;
   double 	Dx, Dy;

   
   for (y = 1; y < (height-1); y++)
       for (x = 1; x < (width-1); x++) {
           Dx = (in_image[y+1][x+1] + 2*in_image[y][x+1] + in_image[y-1][x+1]
                 -  in_image[y+1][x-1] - 2*in_image[y][x-1] - in_image[y-1][x-1])/8;
           Dy = (in_image[y+1][x+1] + 2*in_image[y+1][x] + in_image[y+1][x-1]
                 -  in_image[y-1][x+1] - 2*in_image[y-1][x] - in_image[y-1][x-1])/8;
           out_image[y][x] = (double)sqrt((double)(Dx*Dx + Dy*Dy));
       }

   
   for (x = 0; x < width; x++) {
      out_image[0][x] = out_image[1][x];
      out_image[height-1][x] = out_image[height-2][x];
   }
   for (y = 0; y < height; y++) {
      out_image[y][0] = out_image[y][1];
      out_image[y][width-1] = out_image[y][width-2];
   }
}


double **aui_Pixel::MakeEdge(uint8 *image, sint32 width, sint32 height)
{
    double **fimage;
    double **gimage;

    fimage = ImageToDouble(image, width, height);
    gimage = Alloc2D(width, height);
    Gradient(fimage, gimage, width, height);

    Free2D(fimage);
    return(gimage);
}

void aui_Pixel::SeedRandom(uint32 seed)
{
	uint32 dum;
	uint32 j;

	srand(seed);

	for (j=0; j < VECTOR_SIZE; j++)
		dum = rand();

	for (j=0; j < VECTOR_SIZE; j++)
		m_randRector[j] = rand();

	m_lastRand = rand();
}	

sint32 aui_Pixel::Random()
{
	uint32 j;

	

	j = (m_lastRand / (RAND_MAX / VECTOR_SIZE)) % VECTOR_SIZE;

	m_lastRand = m_randRector[j];
	m_randRector[j] = rand();
	return(m_lastRand);
}



AUI_ERRCODE aui_Pixel::Convert24To16(
	aui_Surface *surface16,
	uint8 *buf24,
	uint32 srcWidth,
	uint32 srcHeight,
	uint32 srcPitch )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;

	Assert( surface16 != NULL );
	if ( !surface16 ) return AUI_ERRCODE_INVALIDPARAM;

	Assert( surface16->BitsPerPixel() == 16 );
	if ( surface16->BitsPerPixel() != 16 ) return AUI_ERRCODE_INVALIDPARAM;

	const sint32 destWidth = surface16->Width();
	const sint32 destHeight = surface16->Height();
	const sint32 destPitch = surface16->Pitch();

	
	const sint32 destDiff = destPitch / 2 - destWidth;	
	const sint32 srcDiff = srcPitch - 3 * srcWidth;		

	
	uint8 *stopHorizontal = buf24 + 3 * srcWidth; 
	const uint8 *stopVertical = buf24 + srcPitch * srcHeight;

	
	BOOL wasDestLocked;
	uint16 *destBuf = (uint16 *)surface16->Buffer();
	if ( !(wasDestLocked = destBuf != NULL) )
	if ( surface16->Lock(
		NULL, (LPVOID *)&destBuf, 0 ) != AUI_ERRCODE_OK )
	{
		destBuf = NULL;
		retcode = AUI_ERRCODE_SURFACELOCKFAILED;
	}

	
	if ( destBuf )
	{
		
		uint16 *origDestBuf = destBuf;

		uint8 red,green,blue;

		do
		{
			do
			{
				blue = *buf24++;
				green = *buf24++;
				red = *buf24++;
				*destBuf++ = Get16BitRGB( red, green, blue );
			} while ( buf24 != stopHorizontal );

			stopHorizontal += srcPitch;

			destBuf += destDiff;
		} while ( (buf24 += srcDiff) != stopVertical );

		if ( !wasDestLocked )
		{
			AUI_ERRCODE errcode = surface16->Unlock( (LPVOID)origDestBuf );
			if ( !AUI_SUCCESS(errcode) )
				retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
		}
	}

	return retcode;
}



AUI_ERRCODE aui_Pixel::Convert8To16(
	aui_Surface *surface16,
	uint8 *buf8,
	uint32 srcWidth,
	uint32 srcHeight,
	uint32 srcPitch,
	RGBQUAD *rgbq )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;

	Assert( surface16 != NULL );
	if ( !surface16 ) return AUI_ERRCODE_INVALIDPARAM;

	Assert( surface16->BitsPerPixel() == 16 );
	if ( surface16->BitsPerPixel() != 16 ) return AUI_ERRCODE_INVALIDPARAM;

	uint16 lookupTable[ 256 ];

	
	for ( uint32 i = 0; i < 256; i++ )
		lookupTable[i] =
			Get16BitRGB(
				rgbq[i].rgbRed,
				rgbq[i].rgbGreen,
				rgbq[i].rgbBlue );

	const sint32 destWidth = surface16->Width();
	const sint32 destHeight = surface16->Height();
	const sint32 destPitch = surface16->Pitch();

	
	const sint32 destDiff = destPitch / 2 - destWidth;	
	const sint32 srcDiff = srcPitch - srcWidth;		

	
	uint8 *stopHorizontal = buf8 + srcWidth; 
	const uint8 *stopVertical = buf8 + srcPitch * srcHeight;

	
	BOOL wasDestLocked;
	uint16 *destBuf = (uint16 *)surface16->Buffer();
	if ( !(wasDestLocked = destBuf != NULL) )
	if ( surface16->Lock(
		NULL, (LPVOID *)&destBuf, 0 ) != AUI_ERRCODE_OK )
	{
		destBuf = NULL;
		retcode = AUI_ERRCODE_SURFACELOCKFAILED;
	}

	
	if ( destBuf )
	{
		
		uint16 *origDestBuf = destBuf;

		do
		{
			do
			{
				*destBuf++ = lookupTable[ *buf8++ ];
			} while ( buf8 != stopHorizontal );

			stopHorizontal += srcPitch;

			destBuf += destDiff;
		} while ( (buf8 += srcDiff) != stopVertical );

		if ( !wasDestLocked )
		{
			AUI_ERRCODE errcode = surface16->Unlock( (LPVOID)origDestBuf );
			if ( !AUI_SUCCESS(errcode) )
				retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
		}
	}

	return retcode;
}
