#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef ARPROCES_H__
#define ARPROCES_H__

#include "ctp2_inttypes.h"  // sint32, uint32
#include "pixeltypes.h"     // Pixel32

sint32 RealConvolution(Pixel32 *InImage,
							   uint32 Col, uint32 Row,
							   uint32 Width, uint32 Height,
							   double *Kernel, uint32 KernelCols,
							   uint32 KernelRows, uint32 Scale,
							   uint32 Absolute, 
							   Pixel32 **OutImageBufPtr);

void AdjImageBrightness(Pixel32 *image, sint16 BrightnessFactor,
							uint32 Width, uint32 Height,
							uint32 Col, uint32 Row);

void StretchImageContrast(Pixel32 *image, uint32 Threshold,
						  uint32 Width, uint32 Height,
						  uint32 Col, uint32 Row);
void rgb_to_hsv(double r, double g, double b, double *h, double *s, double *v);
void hsv_to_rgb(double h, double s, double v, double *r, double *g, double *b);
void AdjustSV(Pixel32 *image, uint32 width, uint32 height, double deltaS, double deltaV);

void BlendWithGray(Pixel32 *image, uint32 width, uint32 height, Pixel16 grayR, Pixel16 grayG, Pixel16 grayB);

#endif