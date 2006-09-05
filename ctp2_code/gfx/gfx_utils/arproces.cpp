#include "c3.h"
#include "arproces.h"

#include <algorithm>        // std::min, std::max
#include "pixelutils.h"

#define MINSAMPLEVAL		0
#define MAXSAMPLEVAL		255

Pixel32 GetPixelFromImage(Pixel32 *image, uint32 width, uint32 height, uint32 col, uint32 row)
{
	return *(image + width * row + col);
}

void PutPixelInImage(Pixel32 *image, uint32 width, uint32 height, uint32 col, uint32 row, Pixel32 pix)
{
	*(image + width * row + col) = pix;
}

#define MINPIXELVALUE		20

sint32 RealConvolution(Pixel32 *InImage,
							   uint32 Width, uint32 Height,
							   uint32 Col, uint32 Row,
							   double *Kernel, uint32 KernelCols,
							   uint32 KernelRows, uint32 Scale,
							   uint32 Absolute, 
							   Pixel32 **OutImageBufPtr)
{
	uint32		ColExtent, RowExtent;
	uint32		ImageCol, ImageRow, KernCol, KernRow;
	uint32		ColOffset, RowOffset, TempCol, TempRow;
	Pixel32		*OutputImageBuffer;
	double		rSum, gSum, bSum;
	double		*KernelPtr;
	Pixel16		r, g, b, a;
	
	
	if (Width >= KernelCols && Height >= KernelRows)
	{
		
		OutputImageBuffer = (Pixel32 *)malloc(Width*Height*sizeof(Pixel32));
		memset(OutputImageBuffer, 0, Width*Height*sizeof(Pixel32));

		if (OutputImageBuffer == NULL) {
			printf("Error Not enough memory for convolution output buffer\n");
			return (-1);
		}
		
		
		*OutImageBufPtr = OutputImageBuffer;
				
		ColOffset = KernelCols/2;
		RowOffset = KernelRows/2;

		
		Col += ColOffset;
		Row += RowOffset;
		Width  -= (KernelCols - 1);
		Height -= (KernelRows - 1);
		
		
		ColExtent = Col + Width;
		RowExtent = Row + Height;
		
		for (ImageRow = Row; ImageRow < RowExtent; ImageRow++)
		{
			TempRow = ImageRow - RowOffset;
			for (ImageCol = Col; ImageCol < ColExtent; ImageCol++)
			{
				TempCol = ImageCol - ColOffset;
				rSum = 0.0;
				gSum = 0.0;
				bSum = 0.0;

				Pixel32		pix32;
				double		red, green, blue;

				pix32 = GetPixelFromImage(InImage, Width, Height, ImageCol, ImageRow);
				RGB32Components(pix32, &r, &g, &b, &a);

				
				if (r != 0 || g != 0 || b != 0) {
					KernelPtr = Kernel;
					
					uint32		howMany = 0;

					for (KernCol = 0; KernCol < KernelCols; KernCol++) {
						for (KernRow = 0; KernRow < KernelRows; KernRow++) {
							double		mult = (*KernelPtr++);

							pix32 = GetPixelFromImage(InImage, Width, Height, TempCol + KernCol, TempRow + KernRow);
							RGB32Components(pix32, &r, &g, &b, &a);
							
							if (r < MINPIXELVALUE && g < MINPIXELVALUE && b < MINPIXELVALUE) continue;

							howMany++;

							red = (double)r;
							green = (double)g;
							blue = (double)b;

							rSum += red * (mult * 9);
							gSum += green * (mult * 9);
							bSum += blue * (mult * 9);
						}
					}

					rSum = rSum / howMany;
					gSum = gSum / howMany;
					bSum = bSum / howMany;

					
					if (Absolute) {
						rSum = fabs(rSum);
						gSum = fabs(gSum);
						bSum = fabs(bSum);
					}

					
					rSum /= (double)(1<<Scale);
					gSum /= (double)(1<<Scale);
					bSum /= (double)(1<<Scale);
					
					rSum = (rSum < MINSAMPLEVAL) ? MINSAMPLEVAL:rSum;
					rSum = (rSum > MAXSAMPLEVAL) ? MAXSAMPLEVAL:rSum;

					gSum = (gSum < MINSAMPLEVAL) ? MINSAMPLEVAL:gSum;
					gSum = (gSum > MAXSAMPLEVAL) ? MAXSAMPLEVAL:gSum;

					bSum = (bSum < MINSAMPLEVAL) ? MINSAMPLEVAL:bSum;
					bSum = (bSum > MAXSAMPLEVAL) ? MAXSAMPLEVAL:bSum;

					pix32 = ComponentsToRGB32((Pixel16)rSum, (Pixel16)gSum, (Pixel16)bSum, a);
					PutPixelInImage(OutputImageBuffer, Width, Height, ImageCol, ImageRow, pix32);
				}
			}
		}
	}
	else
		return(-1);
	
	return(0);
}

#define MAXQUANTLEVELS			256
uint8	Histogram[MAXQUANTLEVELS];

void InitializeLUT(uint8 *LookUpTable)
{
	uint32 Index;

  for (Index = 0; Index < MAXQUANTLEVELS; Index++)
     LookUpTable[Index] = (uint8)Index;
}


void PtTransform(Pixel32 *image, uint32 Width, uint32 Height, uint32 Col, uint32 Row, uint8 *LookUpTable)
{
	
	uint32		ImageCol, ImageRow;
	uint32		ColExtent, RowExtent;
	Pixel32		pix32;
	Pixel16		r, g, b, a;

	ColExtent = Col+Width;
	RowExtent = Row+Height;
	
	for (ImageRow=Row; ImageRow < RowExtent; ImageRow++) {
		for (ImageCol=Col; ImageCol < ColExtent; ImageCol++) {
			pix32 = GetPixelFromImage(image, Width, Height, ImageCol, ImageRow);
			
			RGB32Components(pix32, &r, &g, &b, &a);

			pix32 = ComponentsToRGB32(LookUpTable[r], LookUpTable[g], LookUpTable[b], a);

			if (r==0 && g == 0 && b == 0) continue;

			PutPixelInImage(image, Width, Height, ImageCol, ImageRow, pix32);
		}
	}
}

void GenHistogram(Pixel32 *image, unsigned Width, unsigned Height, unsigned Col, unsigned Row)
{
	uint32		ImageRow, ImageCol, RowExtent, ColExtent;
	uint32		Index;
	Pixel16		r,g,b,a;
	Pixel32		pix32;

	
	for (Index=0; Index < MAXQUANTLEVELS; Index++)
		Histogram[Index] = 0;
	
	RowExtent = Row+Height;
	ColExtent = Col+Width;
	
	
	for (ImageRow = Row; ImageRow < RowExtent; ImageRow++) {
		for (ImageCol = Col; ImageCol < ColExtent; ImageCol++) {

			pix32 = GetPixelFromImage(image, Width, Height, ImageCol, ImageRow);
			RGB32Components(pix32, &r, &g, &b, &a);



            Histogram[std::max(r, std::max(g, b))] += 1;


		}
	}
}

void StretchImageContrast(Pixel32 *image, uint32 Threshold,
						  uint32 Width, uint32 Height,
						  uint32 Col, uint32 Row)
{
   uint32		Index, NewMin, NewMax;
   double		StepSiz, StepVal;
   uint8		LookUpTable[MAXQUANTLEVELS];

   GenHistogram(image, Width, Height, Col, Row);

   




   for (Index=0; Index < MAXQUANTLEVELS; Index++)
      if (Histogram[Index] > Threshold)
         break;

   NewMin = Index;

   




   for (Index=MAXSAMPLEVAL; Index > NewMin; Index--)
      if (Histogram[Index] > Threshold)
         break;

   NewMax = Index;

   StepSiz = (double)MAXQUANTLEVELS/(double)(NewMax-NewMin+1);
   StepVal = 0.0;

   
   for (Index=0; Index < NewMin; Index++)
      LookUpTable[Index] = MINSAMPLEVAL;

   
   for (Index=NewMax+1; Index < MAXQUANTLEVELS; Index++)
      LookUpTable[Index] = MAXSAMPLEVAL;

   
   for (Index=NewMin; Index <= NewMax; Index++)
   {
      LookUpTable[Index] = (uint8)StepVal;
      StepVal += StepSiz;
   }
   


   PtTransform(image, Width, Height, Col, Row, LookUpTable);
}

void AdjImageBrightness(Pixel32 *image, short BrightnessFactor,
							uint32 Width, uint32 Height,
							uint32 Col, uint32 Row)
{
   uint32	Index;
   sint16	NewLevel;
   uint8    LookUpTable[MAXQUANTLEVELS];

   for (Index = MINSAMPLEVAL; Index < MAXQUANTLEVELS; Index++)
   {
      NewLevel = (sint16)(Index + BrightnessFactor);
      NewLevel = (NewLevel < MINSAMPLEVAL) ? MINSAMPLEVAL:NewLevel;
      NewLevel = (NewLevel > MAXSAMPLEVAL) ? MAXSAMPLEVAL:NewLevel;
      LookUpTable[Index] = (uint8)NewLevel;
   }

   PtTransform(image, Width, Height, Col, Row, LookUpTable);
}

void RGBtoHSV( double r, double g, double b, double *h, double *s, double *v )
{
    double const    maximum = std::max(r, std::max(g, b));

	*v = maximum;				
	if (0.0 == maximum)
    {
        *s = 0.0;
        *h = -1.0;
        return;
    }

    double const    minimum = std::min(r, std::min(g, b));
    double const    delta   = maximum - minimum;
	*s = delta / maximum;	

	if (r == maximum)
    {
		*h = ( g - b ) / delta;		
    }
	else if (g == maximum)
    {
		*h = 2.0 + ( b - r ) / delta;	
    }
	else
    {
		*h = 4.0 + ( r - g ) / delta;	
    }
	*h *= 60.0;				
	
    if (*h < 0.0)
    {
		*h += 360.0;
    }
}
void HSVtoRGB(double h, double s, double v, double *r, double *g, double *b)
{
	int i;
	double f, p, q, t;
	if( s == 0 ) {
		
		*r = *g = *b = v;
		return;
	}
	h /= 60;			
	i = (int)floor( h );
	f = h - i;			
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );
	switch( i ) {
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default:		
			*r = v;
			*g = p;
			*b = q;
			break;
	}
}

void AdjustSV(Pixel32 *image, uint32 width, uint32 height, double deltaS, double deltaV)
{
	
	Pixel32		pix32;
	Pixel16		r, g, b, a;
	sint32		i,j;
	double		dr, dg, db;
	double		dh, ds, dv;

	for (i=0; i < (sint32)height; i++) {
		for (j=0; j < (sint32)width; j++) {
			pix32 = GetPixelFromImage(image, width, height, j, i);
			
			RGB32Components(pix32, &r, &g, &b, &a);
			if (r==0 && g == 0 && b == 0) continue;
			
			dr = ((double)r) / 256.0;
			dg = ((double)g) / 256.0;
			db = ((double)b) / 256.0;

			RGBtoHSV(dr, dg, db, &dh, &ds, &dv);

			ds += deltaS;
			dv += deltaV;

			HSVtoRGB(dh, ds, dv, &dr, &dg, &db);

			r = (Pixel16) (dr * 256.0);
			g = (Pixel16) (dg * 256.0);
			b = (Pixel16) (db * 256.0);

			pix32 = ComponentsToRGB32(r, g, b, a);

			PutPixelInImage(image, width, height, j, i, pix32);
		}
	}
}

void BlendWithGray(Pixel32 *image, uint32 width, uint32 height, Pixel16 grayR, Pixel16 grayG, Pixel16 grayB)
{
	
	Pixel32		pix32;
	Pixel16		r, g, b, a;
	sint32		i,j;
	sint16		jitter = 0;

	for (i=0; i < (sint32)height; i++) {
		for (j=0; j < (sint32)width; j++) {
			pix32 = GetPixelFromImage(image, width, height, j, i);
			
			RGB32Components(pix32, &r, &g, &b, &a);
			if (r==0 && g == 0 && b == 0) continue;



			r = (grayR + grayR + grayR + r + jitter) >> 2;
			g = (grayG + grayG + grayG + g + jitter) >> 2;
			b = (grayB + grayB + grayB + b + jitter) >> 2;

			pix32 = ComponentsToRGB32(r, g, b, a);

			PutPixelInImage(image, width, height, j, i, pix32);
		}
	}
}
