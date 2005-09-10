//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Targa file format utilities
// Id           : $Id:$
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
// - Outcommented unused code. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "pixelutils.h"
#include "targautils.h"

#ifdef __MAKESPR__
#define DPRINTF
#endif

#define BYTES_PER_PIXEL 2

#define MAX_WIDTH 256
#define MAX_HEIGHT 256
#define MAX_DATASIZE (MAX_WIDTH * MAX_HEIGHT * 4)

SHORT TgaDecodeScanLine(BYTE *DecodedBuffer, WORD LineLength,
                        WORD PixelSize, BYTE **DpTga);

unsigned char *tmpbuf = NULL;
unsigned char *tmpbuf1= NULL;

bool Get_TGA_Dimension (char *fname,
                        int &Width,
                        int &Height,
                        int &Bpp)
{
	TGAHEADER head;
	int bpp;

	FILE *fp;
	fp = fopen(fname, "rb");

	if (fp == NULL) {
		char	Str[128];

		sprintf(Str,"%s not found.",fname);
		MessageBox(NULL,Str,NULL,MB_OK);
		return NULL;
	}
	setvbuf(fp, NULL, _IONBF, 0);

	if (fread(&head, sizeof(TGAHEADER), 1, fp) < 1)
	{
		DPRINTF(k_DBG_UI, ("Error reading file \"%s\"\n", fname));
		fclose(fp);
		return(0);
	}

	if (head.PixelDepth == 16) {
		bpp = 2;
	} else if (head.PixelDepth == 24) {
		bpp = 3;
	} else if (head.PixelDepth == 32) {
		bpp = 4;
	}
	else
	{
		DPRINTF(k_DBG_UI, ("File \"%s\" is not a 16, 24 or 32 bit Targa\n", fname));
		fclose(fp);
		return(0);
	}

	Width = head.ImageWidth;
	Height = head.ImageHeight;
	Bpp=bpp;

	fclose(fp);

	return TRUE;
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

bool Load_TGA_File_Simple(char *fname,
						  unsigned char *data,
						  int Buffer_Width,
						  int width,
						  int height
						  )
{
	TGAHEADER head;
	long fsize;
	long datasize;
	int bpp;

	FILE *fp;
	fp = fopen(fname, "rb");

	if (fp == NULL)
	{
		char	Str[128];

		sprintf(Str,"%s not found.",fname);
		MessageBox(NULL,Str,NULL,MB_OK);
		return NULL;
	}

	setvbuf(fp, NULL, _IONBF, 0);

	if (fread(&head, sizeof(TGAHEADER), 1, fp) < 1)
	{
		DPRINTF(k_DBG_UI, ("Error reading file \"%s\"\n", fname));
		fclose(fp);
		return(0);
	}

	switch (head.PixelDepth)
	{
	case 16: bpp=2;	break;
	case 24: bpp=3;	break;
	case 32: bpp=4;	break;
	default:
		DPRINTF(k_DBG_UI, ("File \"%s\" is not a 16, 24 or 32 bit Targa\n", fname));
		fclose(fp);
		return(0);
	}

	width  = head.ImageWidth;
	height = head.ImageHeight;

	fsize = head.ImageWidth * head.ImageHeight * bpp;

	if (head.IdLength > 0)
		fseek(fp, head.IdLength, SEEK_CUR);

	long curpos = ftell(fp);
	 fseek(fp, 0, SEEK_END);
	 datasize = ftell(fp) - curpos;
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
			return(0);
		}


		dataPtr -= Buffer_Width;
	   }


		fclose(fp);
	}
	else
		if (head.ImageType == 10)
		{

			int i, byteCount;
			unsigned char *dp, *tp;

			if (tmpbuf == NULL)
			{
				tmpbuf = (unsigned char *)malloc(MAX_DATASIZE);
				tmpbuf1 = (unsigned char *)malloc(MAX_DATASIZE);
			}

			if (fread(tmpbuf, datasize, 1, fp) < 1)
			{
				DPRINTF(k_DBG_UI, ("Error reading file \"%s\"\n", fname));
				fclose(fp);
				free(tmpbuf);
				free(tmpbuf1);
				tmpbuf=NULL;
				return(0);
			}
			fclose(fp);

			dp = tmpbuf1;
			tp = tmpbuf;

			for (i=0; i < head.ImageHeight; i++)
			{
				byteCount = TgaDecodeScanLine(dp, head.ImageWidth, (WORD)bpp, &tp);
				if (byteCount < 0)
				{
					DPRINTF(k_DBG_UI, ("Error decoding file \"%s\"\n", fname));
					free(tmpbuf);
					free(tmpbuf1);
					tmpbuf=NULL;
					return(0);
				}
				dp += byteCount;
			}
		}

	free(tmpbuf);
	free(tmpbuf1);
	tmpbuf=NULL;
	return(1);
}


void TGA2RGB32(Pixel32 *data,int datasize)
{

	struct TGA_DATA
	{
		char r,g,b,a;

	} orig,*converted,*dp;

	dp = (TGA_DATA *)data;


	for(int i=0;i<datasize;i++)
	{
		converted	= dp;
		orig		= *converted;

		converted->b=orig.r;
		converted->r=orig.b;

		dp ++;
	}
}


bool Load_TGA_File(char *fname,
			 unsigned char *data,
			 int Buffer_Width,
			 int width,
			 int height,
			 void *Pixel_Format,
			 BOOL convertToNative)
{
	TGAHEADER head;
	long fsize;
	long datasize;
	int bpp;

	FILE *fp;
	fp = fopen(fname, "rb");

	if (fp == NULL)
	{
		char	Str[128];

		sprintf(Str,"%s not found.",fname);
		MessageBox(NULL,Str,NULL,MB_OK);
		return NULL;
	}

	setvbuf(fp, NULL, _IONBF, 0);

	if (fread(&head, sizeof(TGAHEADER), 1, fp) < 1)
	{
		DPRINTF(k_DBG_UI, ("Error reading file \"%s\"\n", fname));
		fclose(fp);
		return(0);
	}

	switch (head.PixelDepth)
	{
	case 16: bpp=2;	break;
	case 24: bpp=3;	break;
	case 32: bpp=4;	break;
	default:
		DPRINTF(k_DBG_UI, ("File \"%s\" is not a 16, 24 or 32 bit Targa\n", fname));
		fclose(fp);
		return(0);
	}

	width  = head.ImageWidth;
	height = head.ImageHeight;

	fsize = head.ImageWidth * head.ImageHeight * bpp;

	if (head.IdLength > 0)
		fseek(fp, head.IdLength, SEEK_CUR);

	long curpos = ftell(fp);
	 fseek(fp, 0, SEEK_END);
	 datasize = ftell(fp) - curpos;
	 fseek(fp, curpos, SEEK_SET);


	if (head.ImageType == 2)
	{
		unsigned char *dataPtr = data + ((Buffer_Width * height) - Buffer_Width);

		for (sint32 v=0; v<height; v++)
		{
			if (fread(dataPtr, width * 2, 1, fp) < 1)
			{
				DPRINTF(k_DBG_UI, ("Error reading file \"%s\"\n", fname));
				fclose(fp);
				return(0);
			}

			if (convertToNative)
			{

				extern sint32		g_is565Format;
				if (g_is565Format)
				{
					Pixel16 *pixelPtr = (Pixel16 *)dataPtr;
					for (sint32 p=0; p<width; p++)
						pixelPtr[p] = pixelutils_Convert555to565(pixelPtr[p]);
				}
			}


			dataPtr -= Buffer_Width;
		}

		fclose(fp);

	}
	else
		if (head.ImageType == 10)
		{

			int i, byteCount;
			unsigned char *dp, *tp;

			if (tmpbuf == NULL)
			{
				tmpbuf = (unsigned char *)malloc(MAX_DATASIZE);
				tmpbuf1 = (unsigned char *)malloc(MAX_DATASIZE);
			}

			if (fread(tmpbuf, datasize, 1, fp) < 1)
			{
				DPRINTF(k_DBG_UI, ("Error reading file \"%s\"\n", fname));
				fclose(fp);
				free(tmpbuf);
				free(tmpbuf1);
				tmpbuf=NULL;
				return(0);
			}
			fclose(fp);

			dp = tmpbuf1;
			tp = tmpbuf;

			for (i=0; i < head.ImageHeight; i++)
			{
				byteCount = TgaDecodeScanLine(dp, head.ImageWidth, (WORD)bpp, &tp);
				if (byteCount < 0)
				{
					DPRINTF(k_DBG_UI, ("Error decoding file \"%s\"\n", fname));
					free(tmpbuf);
					free(tmpbuf1);
					tmpbuf=NULL;
					return(0);
				}
				dp += byteCount;
			}
		}

	if ((BYTES_PER_PIXEL == 3) && (bpp == 4))
	{
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

	if ((BYTES_PER_PIXEL == 2) && (bpp==4))
	{
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
	free(tmpbuf);
	free(tmpbuf1);
	tmpbuf=NULL;
	return(1);
}

int	write_tga(char *fname, int width, int height, unsigned char *data)
{
	TGAHEADER head;
	long fsize;

	FILE *fp;
	if ((fp = fopen(fname, "wb")) == NULL) {
		char	Str[128];

		sprintf(Str,"%s not found.",fname);
		MessageBox(NULL,Str,NULL,MB_OK);
		return NULL;
	}
	setvbuf(fp, NULL, _IONBF, 0);

	head.IdLength = 0;
	head.CmapType = 0;
	head.ImageType = 2;
	head.CmapIndex = 0;
	head.CmapLength = 0;
	head.CmapEntrySize = 0;
	head.X_Origin = 0;
	head.Y_Origin = 0;
	head.ImageWidth = width;
	head.ImageHeight = height;
	head.PixelDepth = BYTES_PER_PIXEL * 8;
	head.ImagDesc = 0;

	if (fwrite(&head, sizeof(TGAHEADER), 1, fp) < 1) {
		DPRINTF(k_DBG_UI, ("Error writing file \"%s\"\n", fname));
		fclose(fp);
		return(0);
	}

	fsize = width * height * BYTES_PER_PIXEL;

	if (fwrite(data, fsize, 1, fp) < 1) {
		DPRINTF(k_DBG_UI, ("Error writing file \"%s\"\n", fname));
		fclose(fp);
		return(0);
	}

	fclose(fp);
	return(1);
}






BYTE GetByte(BYTE **p)
{
	BYTE rval = **p;
	(*p)++;
	return(rval);
}

SHORT TgaDecodeScanLine(unsigned char *DecodedBuffer, WORD LineLength,
						WORD PixelSize, BYTE **DpTga)
{
	WORD    i;
	SHORT   byteCount;
	WORD    runCount;
	WORD    bufIndex;
	WORD    bufMark;
	WORD    pixelCount;

	bufIndex   = 0;
	byteCount  = 0;
	pixelCount = 0;


	while (pixelCount < LineLength)
	{

		runCount = GetByte(DpTga);


		if (pixelCount + (runCount & 0x7f) + 1 > LineLength)
			return(-1);


		if (runCount & 0x80)
		{
			runCount &= ~0x80;

			bufMark = bufIndex;


			pixelCount += (runCount + 1);


			byteCount += ((runCount + 1) * PixelSize);


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
	return(byteCount);
}
