#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __C3IMAGEFORMATS_H__
#define __C3IMAGEFORMATS_H__

#include "aui_image.h"

class aui_Surface;

class TargaImageFormat : public aui_ImageFormat
{
public:
	TargaImageFormat() {}
	virtual ~TargaImageFormat() {}

	AUI_ERRCODE LoadRIM(MBCHAR const *filename, aui_Image *image);
	virtual AUI_ERRCODE Load(MBCHAR const * filename, aui_Image *image);
};


class TiffImageFormat : public aui_ImageFormat
{
public:

	TiffImageFormat() {}
	virtual ~TiffImageFormat() {}

	virtual AUI_ERRCODE	Load(MBCHAR const *filename, aui_Image *image );
};

#endif
