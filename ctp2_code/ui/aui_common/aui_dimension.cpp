










#include "c3.h"
#include "aui_region.h"

#include "aui_dimension.h"



aui_Dimension::aui_Dimension( aui_Region *parent )
	:
	m_hanchorType( AUI_DIMENSION_HANCHOR_LEFT ),
	m_vanchorType( AUI_DIMENSION_VANCHOR_TOP ),
	m_hposType( AUI_DIMENSION_HPOSITION_ABSOLUTE ),
	m_vposType( AUI_DIMENSION_VPOSITION_ABSOLUTE ),
	m_hsizeType( AUI_DIMENSION_HSIZE_ABSOLUTE ),
	m_vsizeType( AUI_DIMENSION_VSIZE_ABSOLUTE ),
	m_hpos( 0 ),
	m_vpos( 0 ),
	m_hsize( 0 ),
	m_vsize( 0 ),
	m_parent( parent )
{
	AnchorLeft();
	AnchorTop();

	AbsoluteHorizontalPosition( TRUE );
	AbsoluteVerticalPosition( TRUE );
	AbsoluteHorizontalSize( TRUE );
	AbsoluteVerticalSize( TRUE );
}



aui_Region *aui_Dimension::SetParent( aui_Region *parent )
{
	aui_Region *prevParent = m_parent;
	m_parent = parent;
	return prevParent;
}



void aui_Dimension::AnchorLeft( void )
{
	m_hanchorType = AUI_DIMENSION_HANCHOR_LEFT;
}



void aui_Dimension::AnchorHorizontalCenter( void )
{
	m_hanchorType = AUI_DIMENSION_HANCHOR_CENTER;
}



void aui_Dimension::AnchorRight( void )
{
	m_hanchorType = AUI_DIMENSION_HANCHOR_RIGHT;
}



void aui_Dimension::AnchorTop( void )
{
	m_vanchorType = AUI_DIMENSION_VANCHOR_TOP;
}



void aui_Dimension::AnchorVerticalCenter( void )
{
	m_vanchorType = AUI_DIMENSION_VANCHOR_CENTER;
}



void aui_Dimension::AnchorBottom( void )
{
	m_vanchorType = AUI_DIMENSION_VANCHOR_BOTTOM;
}



void aui_Dimension::AbsoluteHorizontalPosition( BOOL absolute )
{
	if ( absolute )
		m_hposType = AUI_DIMENSION_HPOSITION_ABSOLUTE;
	else
		m_hposType = AUI_DIMENSION_HPOSITION_RELATIVE;
}



void aui_Dimension::AbsoluteVerticalPosition( BOOL absolute )
{
	if ( absolute )
		m_vposType = AUI_DIMENSION_VPOSITION_ABSOLUTE;
	else
		m_vposType = AUI_DIMENSION_VPOSITION_RELATIVE;
}



void aui_Dimension::AbsoluteHorizontalSize( BOOL absolute )
{
	if ( absolute )
		m_hsizeType = AUI_DIMENSION_HSIZE_ABSOLUTE;
	else
		m_hsizeType = AUI_DIMENSION_HSIZE_RELATIVE;
}



void aui_Dimension::AbsoluteVerticalSize( BOOL absolute )
{
	if ( absolute )
		m_vsizeType = AUI_DIMENSION_VSIZE_ABSOLUTE;
	else
		m_vsizeType = AUI_DIMENSION_VSIZE_RELATIVE;
}



sint32 aui_Dimension::CalculateX( void )
{
	if ( !m_parent ) return m_hpos;

	if ( m_hanchorType == AUI_DIMENSION_HANCHOR_CENTER )
	{
		return ( m_parent->Width() - CalculateWidth() ) / 2;
	}
	else if ( m_hanchorType == AUI_DIMENSION_HANCHOR_RIGHT )
	{
		sint32 x = m_hpos;

		if ( m_hposType == AUI_DIMENSION_HPOSITION_RELATIVE )
			x *= sint32((double)m_parent->Width() / 100.0 + 0.5);

		return m_parent->Width() - CalculateWidth() - x;
	}
	else if ( m_hanchorType == AUI_DIMENSION_HANCHOR_LEFT )
	{
		if ( m_hposType == AUI_DIMENSION_HPOSITION_RELATIVE )
			return sint32((double)m_parent->Width() * m_hpos / 100.0 + 0.5);
	}
	else
		
		Assert( FALSE );

	return m_hpos;
}



sint32 aui_Dimension::CalculateY( void )
{
	if ( !m_parent ) return m_vpos;

	if ( m_vanchorType == AUI_DIMENSION_VANCHOR_CENTER )
	{
		return ( m_parent->Height() - CalculateHeight() ) / 2;
	}
	else if ( m_vanchorType == AUI_DIMENSION_VANCHOR_BOTTOM )
	{
		sint32 y = m_vpos;

		if ( m_vposType == AUI_DIMENSION_VPOSITION_RELATIVE )
			y *= sint32((double)m_parent->Height() / 100.0 + 0.5);

		return m_parent->Height() - CalculateHeight() - y;
	}
	else if ( m_vanchorType == AUI_DIMENSION_VANCHOR_TOP )
	{
		if ( m_vposType == AUI_DIMENSION_VPOSITION_RELATIVE )
			return sint32((double)m_parent->Height() * m_vpos / 100.0 + 0.5);
	}
	else
		
		Assert( FALSE );

	return m_vpos;
}



sint32 aui_Dimension::CalculateWidth( void )
{
	if ( !m_parent ) return m_hsize;

	if ( m_hsizeType == AUI_DIMENSION_HSIZE_ABSOLUTE )
		return m_hsize;
	else if ( m_hsizeType == AUI_DIMENSION_HSIZE_RELATIVE )
		return sint32((double)m_parent->Width() * m_hsize / 100.0 + 0.5);
	else
		
		Assert( FALSE );

	return m_hsize;
}



sint32 aui_Dimension::CalculateHeight( void )
{
	if ( !m_parent ) return m_vsize;

	if ( m_vsizeType == AUI_DIMENSION_VSIZE_ABSOLUTE )
		return m_vsize;
	else if ( m_vsizeType == AUI_DIMENSION_VSIZE_RELATIVE )
		return sint32((double)m_parent->Height() * m_vsize / 100.0 + 0.5);
	else
		
		Assert( FALSE );

	return m_vsize;
}



void aui_Dimension::CalculateAll(
	sint32 *x,
	sint32 *y,
	sint32 *width,
	sint32 *height )
{
	
	
	

	*x = CalculateX();
	*y = CalculateY();
	*width = CalculateWidth();
	*height = CalculateHeight();
}


void aui_Dimension::SetHorizontalPosition( sint32 x )
{
	if ( !m_parent ) m_hpos = x;

	if ( m_hanchorType == AUI_DIMENSION_HANCHOR_CENTER )
	{
		
		
		m_hpos = x;
		
	}
	else if ( m_hanchorType == AUI_DIMENSION_HANCHOR_RIGHT )
	{
		m_hpos = m_parent->Width() - CalculateWidth() - x;

		if ( m_hposType == AUI_DIMENSION_HPOSITION_RELATIVE )
			m_hpos = sint32((double)m_hpos * 100.0 / m_parent->Width() + 0.5);

	}
	else if ( m_hanchorType == AUI_DIMENSION_HANCHOR_LEFT )
	{
		if ( m_hposType == AUI_DIMENSION_HPOSITION_RELATIVE )
			m_hpos = sint32((double) x * 100.0 / m_parent->Width() + 0.5);
		else
			m_hpos = x;

	}
	else
		
		Assert( FALSE );

}


void aui_Dimension::SetVerticalPosition( sint32 y )
{
	if ( !m_parent ) m_vpos = y;

	if ( m_vanchorType == AUI_DIMENSION_VANCHOR_CENTER )
	{
		
		
		m_vpos = y;
	}
	else if ( m_vanchorType == AUI_DIMENSION_VANCHOR_BOTTOM )
	{
		m_vpos = m_parent->Height() - CalculateHeight() - y;

		if ( m_vposType == AUI_DIMENSION_VPOSITION_RELATIVE )
			m_vpos = sint32((double)m_vpos * 100.0 / m_parent->Height() + 0.5);

	}
	else if ( m_vanchorType == AUI_DIMENSION_VANCHOR_TOP )
	{
		if ( m_vposType == AUI_DIMENSION_VPOSITION_RELATIVE )
			m_vpos = sint32((double)y * 100.0 / m_parent->Height() + 0.5);
		else
			m_vpos = y;

	}
	else
		
		Assert( FALSE );

}


void aui_Dimension::SetHorizontalSize( sint32 width )
{
	if ( !m_parent ) m_hsize = width;

	if ( m_hsizeType == AUI_DIMENSION_HSIZE_ABSOLUTE )
		m_hsize = width;
	else if ( m_hsizeType == AUI_DIMENSION_HSIZE_RELATIVE )
		m_hsize = sint32((double) width * 100.0 / m_parent->Width() + 0.5);

	else
		
		Assert( FALSE );

}


void aui_Dimension::SetVerticalSize( sint32 height )
{
	if ( !m_parent ) m_vsize = height;

	if ( m_vsizeType == AUI_DIMENSION_VSIZE_ABSOLUTE )
		m_vsize = height;
	else if ( m_vsizeType == AUI_DIMENSION_VSIZE_RELATIVE )
		m_vsize = sint32((double) height * 100.0 / m_parent->Height() + 0.5);
	else
		
		Assert( FALSE );

}
