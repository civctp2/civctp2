










#ifndef __AUI_IMAGE_H__
#define __AUI_IMAGE_H__


#include "aui_base.h"
#include "aui_memmap.h"


#define  k_DEFAULT_CHROMA_RED	255
#define  k_DEFAULT_CHROMA_GREEN	0
#define  k_DEFAULT_CHROMA_BLUE	255

class aui_Surface;
class aui_Image;




class aui_ImageFormat : public aui_FileFormat
{
public:
	
	aui_ImageFormat() : aui_FileFormat() {}
	virtual ~aui_ImageFormat() {}

	virtual AUI_ERRCODE	Load( const MBCHAR *filename, aui_Image *image ) = 0;
};



class aui_Image : public aui_Base
{
public:
	
	aui_Image(
		AUI_ERRCODE *retval,
		const MBCHAR *filename = NULL );
	virtual ~aui_Image();

	
	
	
	void SetChromakey(sint32 r, sint32 g, sint32 b);


protected:
	aui_Image() : aui_Base() {}
	AUI_ERRCODE InitCommon( const MBCHAR *filename );

public:
	
	AUI_ERRCODE Load( void );
	AUI_ERRCODE Unload( void );

	
	virtual AUI_ERRCODE LoadEmpty( sint32 width, sint32 height, sint32 bpp );

    
    AUI_ERRCODE LoadFileMapped( sint32 width, sint32 height,
                                sint32 bpp, sint32 pitch, 
                                uint8 *buffer );

	
	aui_Surface	*TheSurface( void ) const { return m_surface; }
	MBCHAR		*GetFilename( void ) const { return (MBCHAR *)m_filename; }

	
	AUI_ERRCODE	SetFilename( const MBCHAR *filename );

protected:
	MBCHAR			m_filename[ MAX_PATH + 1 ];	

	aui_ImageFormat	*m_format;		
	aui_Surface		*m_surface;		
};









class aui_BmpImageFormat : public aui_ImageFormat
{
public:
	
	aui_BmpImageFormat() : aui_ImageFormat() {}
	virtual ~aui_BmpImageFormat() {}

	virtual AUI_ERRCODE	Load( const MBCHAR *filename, aui_Image *image );
};


#endif 
