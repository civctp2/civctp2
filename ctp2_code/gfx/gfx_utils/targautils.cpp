//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Targa file format utilities
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Outcommented unused code. (Sep 9th 2005 Martin Gühmann)
// - Redesigned to get rid of global variables.
//
//----------------------------------------------------------------------------
///
/// \file   targautils.cpp
/// \brief  Targa (.tga) file support

#include "c3.h"
#include "targautils.h"

#include <algorithm>
#include "pixelutils.h"
extern sint32		g_is565Format;

#define BYTES_PER_PIXEL 2

#define MAX_WIDTH 256
#define MAX_HEIGHT 256
#define MAX_DATASIZE (MAX_WIDTH * MAX_HEIGHT * 4)

namespace
{
    BYTE GetByte(BYTE **p)
    {
        BYTE rval = **p;
	  (*p)++;
	  return rval;
    }

    /// Report that a file does not exist
    /// \param a_FileName     Name of the file
    void ReportFileNotFound(char const * a_FileName)
    {
        char error[128];
        sprintf(error, "%s not found.", a_FileName);
        MessageBox(NULL, error, NULL, MB_OK);
    }

} // namespace

SHORT TgaDecodeScanLine(BYTE *DecodedBuffer, WORD LineLength,
                        WORD PixelSize, BYTE **DpTga);

bool Get_TGA_Dimension (char const * fname,
                        int &Width,
                        int &Height,
                        int &Bpp)
{
	FILE *  fp  = fopen(fname, "rb");
	if (!fp)
	{
		ReportFileNotFound(fname);
		return false;
	}
	setvbuf(fp, NULL, _IONBF, 0);

	TGAHEADER head;
	if (fread(&head, sizeof(TGAHEADER), 1, fp) < 1)
	{
		DPRINTF(k_DBG_UI, ("Error reading file \"%s\"\n", fname));
		fclose(fp);
		return false;
	}

	switch (head.PixelDepth)
    {
    case 16:
    case 24:
    case 32:
        // Supported format
	    Width   = head.ImageWidth;
	    Height  = head.ImageHeight;
	    Bpp     = head.PixelDepth / 8;
        fclose(fp);
        return true;

    default:
		DPRINTF(k_DBG_UI, ("File \"%s\" is not a 16, 24 or 32 bit Targa\n", fname));
		fclose(fp);
		return false;
	}
}

#if	0 // Unused
static void Get_Pixel_Mask_Scale (
	unsigned int Mask,
	unsigned int &shift,
	unsigned int &scale)
{
   int m;
   int i;
   for (shift = 0, m = Mask; !(m & 1); m >>= 1) shift ++;
   for (scale = 0, i = 0x80; !(i & m); i >>=1) scale ++;
}
#endif

bool Load_TGA_File_Simple(char const * fname,
						  unsigned char *data,
						  int Buffer_Width,
						  int width,
						  int height
						  )
{
	FILE * fp = fopen(fname, "rb");
	if (!fp)
	{
		ReportFileNotFound(fname);
		return false;
	}

	setvbuf(fp, NULL, _IONBF, 0);

	TGAHEADER head;
	if (fread(&head, sizeof(TGAHEADER), 1, fp) < 1)
	{
		DPRINTF(k_DBG_UI, ("Error reading file \"%s\"\n", fname));
		fclose(fp);
		return false;
	}

    int bpp;
	switch (head.PixelDepth)
	{
	case 16: bpp=2;	break;
	case 24: bpp=3;	break;
	case 32: bpp=4;	break;
	default:
		DPRINTF(k_DBG_UI, ("File \"%s\" is not a 16, 24 or 32 bit Targa\n", fname));
		fclose(fp);
		return false;
	}

	width  = head.ImageWidth;
	height = head.ImageHeight;

	if (head.IdLength > 0)
		fseek(fp, head.IdLength, SEEK_CUR);

	long curpos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	long datasize = ftell(fp) - curpos;
	fseek(fp, curpos, SEEK_SET);

	if (head.ImageType==2)
	{

	   unsigned char *dataPtr = data + ((Buffer_Width * height) - Buffer_Width);

	   for (sint32 v=0; v<height; v++)
	   {
		if (fread(dataPtr,width*bpp,1,fp) < 1)
		{
			DPRINTF(k_DBG_UI, ("Error reading file \"%s\"\n", fname));
			fclose(fp);
			return false;
		}

		dataPtr -= Buffer_Width;
	   }

		fclose(fp);
	}
	else if (head.ImageType == 10)
	{
		unsigned char * tmpbuf  = new unsigned char[MAX_DATASIZE];

		if (fread(tmpbuf, datasize, 1, fp) < 1)
		{
			DPRINTF(k_DBG_UI, ("Error reading file \"%s\"\n", fname));
			fclose(fp);
			delete [] tmpbuf;
			return false;
		}

		fclose(fp);
		unsigned char * tmpbuf1 = new unsigned char[MAX_DATASIZE];
		unsigned char * dp      = tmpbuf1;
		unsigned char * tp      = tmpbuf;

		for (int i = 0; i < head.ImageHeight; i++)
		{
			int byteCount = TgaDecodeScanLine(dp, head.ImageWidth, (WORD)bpp, &tp);
			if (byteCount < 0)
			{
				DPRINTF(k_DBG_UI, ("Error decoding file \"%s\"\n", fname));
				delete [] tmpbuf;
				delete [] tmpbuf1;
				return false;
			}
			dp += byteCount;
		}

		delete [] tmpbuf;
		delete [] tmpbuf1;
	}

	return true;
}

void TGA2RGB32(Pixel32 * data, int datasize)
{
    struct TGA_DATA
    {
        char r,g,b,a;
    };

    struct TGA_DATA * dp = reinterpret_cast<struct TGA_DATA *>(data);

    for (int i = 0; i < datasize; ++i)
    {
        std::swap<char>(dp->b, dp->r);
        ++dp;
    }
}

bool Load_TGA_File(char const *fname,
			 unsigned char *data,
			 int Buffer_Width,
			 int width,
			 int height,
			 void *Pixel_Format,
			 bool convertToNative)
{
	FILE * fp = fopen(fname, "rb");
	if (!fp)
	{
		ReportFileNotFound(fname);
		return false;
	}
	setvbuf(fp, NULL, _IONBF, 0);

	TGAHEADER head;
	if (fread(&head, sizeof(TGAHEADER), 1, fp) < 1)
	{
		DPRINTF(k_DBG_UI, ("Error reading file \"%s\"\n", fname));
		fclose(fp);
		return false;
	}

	int bpp;
	switch (head.PixelDepth)
	{
	case 16: bpp=2;	break;
	case 24: bpp=3;	break;
	case 32: bpp=4;	break;
	default:
		DPRINTF(k_DBG_UI, ("File \"%s\" is not a 16, 24 or 32 bit Targa\n", fname));
		fclose(fp);
		return false;
	}

	width  = head.ImageWidth;
	height = head.ImageHeight;

	if (head.IdLength > 0)
		fseek(fp, head.IdLength, SEEK_CUR);

	long curpos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	long datasize = ftell(fp) - curpos;
	fseek(fp, curpos, SEEK_SET);

	unsigned char * tmpbuf  = NULL;
	unsigned char * tmpbuf1 = NULL;

	if (head.ImageType == 2)
	{
		unsigned char *dataPtr = data + ((Buffer_Width * height) - Buffer_Width);

		for (sint32 v=0; v<height; v++)
		{
			if (fread(dataPtr, width * 2, 1, fp) < 1)
			{
				DPRINTF(k_DBG_UI, ("Error reading file \"%s\"\n", fname));
				fclose(fp);
				return false;
			}

			if (convertToNative && g_is565Format)
			{
				Pixel16 * pixelPtr = reinterpret_cast<Pixel16 *>(dataPtr);
				std::transform(pixelPtr, pixelPtr + width, pixelPtr, pixelutils_Convert555to565);
			}

			dataPtr -= Buffer_Width;
		}

		fclose(fp);

	}
	else if (head.ImageType == 10)
	{
		tmpbuf  = new unsigned char[MAX_DATASIZE];

		if (fread(tmpbuf, datasize, 1, fp) < 1)
		{
			DPRINTF(k_DBG_UI, ("Error reading file \"%s\"\n", fname));
			fclose(fp);
			delete [] tmpbuf;
			return false;
		}
		fclose(fp);

		tmpbuf1 = new unsigned char[MAX_DATASIZE];
		unsigned char * dp = tmpbuf1;
		unsigned char * tp = tmpbuf;

		for (int i = 0; i < head.ImageHeight; i++)
		{
			int byteCount = TgaDecodeScanLine(dp, head.ImageWidth, (WORD)bpp, &tp);
			if (byteCount < 0)
			{
				DPRINTF(k_DBG_UI, ("Error decoding file \"%s\"\n", fname));
				delete [] tmpbuf;
				delete [] tmpbuf1;
				return false;
			}
			dp += byteCount;
		}
	}

	if ((BYTES_PER_PIXEL == 3) && (bpp == 4))
	{
		Assert(tmpbuf1);
		unsigned char *fp = data;
		unsigned char *tp = tmpbuf1;
		int count = (width * height);

		while(count--)
		{
			*tp++ = *fp++;
			*tp++ = *fp++;
			*tp++ = *fp++;
			fp++;
		}
	}
	else if ((BYTES_PER_PIXEL == 2) && (bpp==4))
	{
		Assert(tmpbuf1);
		unsigned char *fp = tmpbuf1;
		unsigned char *fp1= tmpbuf1 + 1;
		unsigned char *fp2= tmpbuf1 + 2;
		unsigned char *fp3= tmpbuf1 + 3;

		unsigned short int *tp = (unsigned short int *)data;
		int count = (width * height);

		while(count--)
		{
			*tp=(*fp<<12)&0xf000;
			*tp|=(*fp1<<8)&0x0f00;
			*tp|=(*fp2<<4)&0xfff0;
			*tp|=(*fp3&0x000f);
			tp++;
			fp++;
			fp1++;
			fp2++;
			fp3++;
		}
	}

	delete [] tmpbuf;
	delete [] tmpbuf1;
	return true;
}

bool write_tga(char const * fname, int width, int height, unsigned char const * data)
{
	FILE * fp = fopen(fname, "wb");
	if (!fp)
    {
		ReportFileNotFound(fname);
		return false;
	}

	setvbuf(fp, NULL, _IONBF, 0);

	TGAHEADER head;
	head.IdLength       = 0;
	head.CmapType       = 0;
	head.ImageType      = 2;
	head.CmapIndex      = 0;
	head.CmapLength     = 0;
	head.CmapEntrySize  = 0;
	head.X_Origin       = 0;
	head.Y_Origin       = 0;
	head.ImageWidth     = static_cast<WORD>(width);
	head.ImageHeight    = static_cast<WORD>(height);
	head.PixelDepth     = BYTES_PER_PIXEL * 8;
	head.ImagDesc       = 0;

	bool    isWritten   =
        (1 == fwrite(&head, sizeof(TGAHEADER), 1, fp)) &&
        (1 == fwrite(data, width * height * BYTES_PER_PIXEL, 1, fp));
	fclose(fp);

    if (!isWritten)
    {
		DPRINTF(k_DBG_UI, ("Error writing file \"%s\"\n", fname));
	}

	return isWritten;
}







SHORT TgaDecodeScanLine(unsigned char *DecodedBuffer, WORD LineLength,
						WORD PixelSize, BYTE **DpTga)
{
	WORD    i;
	SHORT   byteCount   = 0;
	WORD    bufIndex    = 0;
	WORD    pixelCount  = 0;

	while (pixelCount < LineLength)
	{
		WORD runCount = GetByte(DpTga);

		if (pixelCount + (runCount & 0x7f) + 1 > LineLength)
			return -1;

		if (runCount & 0x80)
		{
			runCount   &= ~0x80;

			pixelCount += (runCount + 1);
			byteCount  += ((runCount + 1) * PixelSize);

			WORD bufMark = bufIndex;

			for (i = 0; i < PixelSize; i++)
				DecodedBuffer[bufIndex++] = GetByte(DpTga);

			while (runCount--)
			{
				for (i = 0; i < PixelSize; i++)
					DecodedBuffer[bufIndex++] = DecodedBuffer[bufMark + i];
			}
		}
		else
		{
			pixelCount += (runCount + 1);
			byteCount  += ((runCount + 1) * PixelSize);

			do
			{
				for (i = 0; i < PixelSize; i++)
					DecodedBuffer[bufIndex++] = GetByte(DpTga);
			}
			while (runCount--);
		}
	}

	return byteCount;
}
