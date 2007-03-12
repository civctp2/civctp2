//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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
//----------------------------------------------------------------------------
///
/// \file   targautils.h
/// \brief  Targa (.tga) file support

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef TARGAUTILS_H__
#define TARGAUTILS_H__

//----------------------------------------------------------------------------
//
// Library imports
//
//----------------------------------------------------------------------------

#include "windows.h"    // BYTE, WORD

//----------------------------------------------------------------------------
//
// Exported names
//
//----------------------------------------------------------------------------

// The following is supported by MSVC and GCC 4.1 and higher.
#pragma pack(push, 1)

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

// The following is supported by MSVC and GCC 4.1 and higher.
#pragma pack(pop) 

//----------------------------------------------------------------------------
//
// Project imports
//
//----------------------------------------------------------------------------

#include "pixeltypes.h" // Pixel32

//----------------------------------------------------------------------------
//
// Declarations
//
//----------------------------------------------------------------------------

bool Get_TGA_Dimension 
(
    char const *            fname, 
    int &                   Width,
    int &                   Height,
    int &                   Bpp
);
bool Load_TGA_File
(
    char const *            fname,
    unsigned char *         data,
    int                     Buffer_Width,
    int                     width, 
    int                     height,
    void *                  Pixel_Format, 
    bool                    convertToNative
);
bool Load_TGA_File_Simple
(
    char const *            fname,
    unsigned char *         data,
    int                     Buffer_Width,
    int                     width,
    int                     height
);
void TGA2RGB32
(
    Pixel32 *               data, 
    int                     datasize
);
bool write_tga
(
    char const *            fname, 
    int                     width, 
    int                     height,
    unsigned char const *   data
);

#endif
