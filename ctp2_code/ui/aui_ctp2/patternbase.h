











#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __PATTERNBASE_H__
#define __PATTERNBASE_H__


class Pattern;

#define k_PATTERNBASE_LDL_SRCXPIX	"srcxpix"
#define k_PATTERNBASE_LDL_SRCYPIX	"srcypix"
#define k_PATTERNBASE_LDL_SRCWIDTHPIX	"srcwidthpix"
#define k_PATTERNBASE_LDL_SRCHEIGHTPIX	"srcheightpix"




class PatternBase
{
public:
	
	PatternBase(
		MBCHAR *pattern );
	PatternBase(
		MBCHAR *ldlBlock,
		MBCHAR *pattern );
	virtual ~PatternBase();

protected:
	PatternBase() {}
	AUI_ERRCODE InitCommon( MBCHAR *pattern );
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock, MBCHAR *pattern );

public:
	
	Pattern *&ThePattern( void ) { return m_pattern; }

	sint32	SrcX( void ) { return m_srcX; }
	sint32	SrcY( void ) { return m_srcY; }
	sint32	SrcWidthPix( void ) { return m_srcWidthPix; }
	sint32	SrcHeightPix( void ) { return m_srcHeightPix; }

	void	MoveSrc( sint32 srcX, sint32 srcY ) { m_srcX = srcX; m_srcY = srcY; }
	void	ResizeSrc( sint32 widthPix, sint32 heightPix ) { m_srcWidthPix = widthPix; m_srcHeightPix = heightPix; }

protected:
	Pattern *m_pattern;

	sint32	m_srcX;
	sint32	m_srcY;
	sint32	m_srcWidthPix;
	sint32	m_srcHeightPix;
};


#endif 
