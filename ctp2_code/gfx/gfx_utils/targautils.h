#pragma once

#ifndef __TARGAUTILS_H__
#define __TARGAUTILS_H__




#pragma pack(1) 

typedef struct _TgaHeader
{                           
    BYTE   IdLength;            
    BYTE   CmapType;            
    BYTE   ImageType;           
    


    WORD   CmapIndex;           
    WORD   CmapLength;          
    BYTE   CmapEntrySize;       
    


    WORD   X_Origin;            
    WORD   Y_Origin;            
    WORD   ImageWidth;          
    WORD   ImageHeight;         
    BYTE   PixelDepth;          
    BYTE   ImagDesc;            
} TGAHEADER;

#pragma pack() 

extern bool Get_TGA_Dimension (char *fname,int &Width,int &Height,int &Bpp);
extern bool Load_TGA_File(char *fname,unsigned char *data,int Buffer_Width,int width, 
					int height,void *Pixel_Format, BOOL convertToNative);

extern bool Load_TGA_File_Simple(char *fname,unsigned char *data,int Buffer_Width,int width,int height);

extern void TGA2RGB32(Pixel32 *data,int datasize);

extern int write_tga(char *fname, int width, int height,
                     unsigned char *data);

#endif