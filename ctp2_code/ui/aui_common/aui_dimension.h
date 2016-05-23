#ifndef __AUI_DIMENSION_H__
#define __AUI_DIMENSION_H__

#include "aui_base.h"

class aui_Region;

enum AUI_DIMENSION_HANCHOR
{
	AUI_DIMENSION_HANCHOR_LEFT =		0x00,
	AUI_DIMENSION_HANCHOR_CENTER =		0x01,
	AUI_DIMENSION_HANCHOR_RIGHT =		0x02
};

enum AUI_DIMENSION_VANCHOR
{
	AUI_DIMENSION_VANCHOR_TOP =			0x00,
	AUI_DIMENSION_VANCHOR_CENTER =		0x01,
	AUI_DIMENSION_VANCHOR_BOTTOM =		0x02
};

enum AUI_DIMENSION_HPOSITION
{
	AUI_DIMENSION_HPOSITION_ABSOLUTE =	0x00,
	AUI_DIMENSION_HPOSITION_RELATIVE =	0x01
};

enum AUI_DIMENSION_VPOSITION
{
	AUI_DIMENSION_VPOSITION_ABSOLUTE =	0x00,
	AUI_DIMENSION_VPOSITION_RELATIVE =	0x01
};

enum AUI_DIMENSION_HSIZE
{
	AUI_DIMENSION_HSIZE_ABSOLUTE =		0x00,
	AUI_DIMENSION_HSIZE_RELATIVE =		0x01
};

enum AUI_DIMENSION_VSIZE
{
	AUI_DIMENSION_VSIZE_ABSOLUTE =		0x00,
	AUI_DIMENSION_VSIZE_RELATIVE =		0x01
};


class aui_Dimension : public aui_Base
{
public:

	aui_Dimension( aui_Region *parent = NULL );
	virtual ~aui_Dimension() {}

	aui_Region	*SetParent( aui_Region *parent );
	aui_Region	*GetParent( void ) const { return m_parent; }

	AUI_DIMENSION_HANCHOR	GetHorizontalAnchorType( void ) { return (AUI_DIMENSION_HANCHOR)m_hanchorType; }
	AUI_DIMENSION_VANCHOR	GetVerticalAnchorType( void ) { return (AUI_DIMENSION_VANCHOR)m_vanchorType; }
	AUI_DIMENSION_HPOSITION	GetHorizontalPositionType( void ) { return (AUI_DIMENSION_HPOSITION)m_hposType; }
	AUI_DIMENSION_VPOSITION	GetVerticalPositionType( void ) { return (AUI_DIMENSION_VPOSITION)m_vposType; }
	AUI_DIMENSION_HSIZE		GetHorizontalSizeType( void ) { return (AUI_DIMENSION_HSIZE)m_hsizeType; }
	AUI_DIMENSION_VSIZE		GetVerticalSizeType( void ) { return (AUI_DIMENSION_VSIZE)m_vsizeType; }

	sint32	&HorizontalPositionData( void ) { return m_hpos; }
	sint32	&VerticalPositionData( void ) { return m_vpos; }
	sint32	&HorizontalSizeData( void ) { return m_hsize; }
	sint32	&VerticalSizeData( void ) { return m_vsize; }

	void	AnchorLeft( void );
	void	AnchorHorizontalCenter( void );
	void	AnchorRight( void );
	void	AnchorTop( void );
	void	AnchorVerticalCenter( void );
	void	AnchorBottom( void );

	void	AbsoluteHorizontalPosition( BOOL absolute );
	void	AbsoluteVerticalPosition( BOOL absolute );
	void	AbsoluteHorizontalSize( BOOL absolute );
	void	AbsoluteVerticalSize( BOOL absolute );

	void	SetHorizontalPosition( sint32 x );
	void	SetVerticalPosition( sint32 y );
	void	SetHorizontalSize( sint32 width );
	void	SetVerticalSize( sint32 height );

	sint32	CalculateX( void );
	sint32	CalculateY( void );
	sint32	CalculateWidth( void );
	sint32	CalculateHeight( void );

	void	CalculateAll(
		sint32 *x,
		sint32 *y,
		sint32 *width,
		sint32 *height );

protected:
	unsigned	m_hanchorType : 2;
	unsigned	m_vanchorType : 2;
	unsigned	m_hposType : 1;
	unsigned	m_vposType : 1;
	unsigned	m_hsizeType : 1;
	unsigned	m_vsizeType : 1;

	sint32		m_hpos;
	sint32		m_vpos;
	sint32		m_hsize;
	sint32		m_vsize;

	aui_Region	*m_parent;
};

#endif
