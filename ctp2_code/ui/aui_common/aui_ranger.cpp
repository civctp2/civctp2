










#include "c3.h"
#include "aui_ui.h"
#include "aui_blitter.h"
#include "aui_window.h"
#include "aui_surface.h"
#include "aui_uniqueid.h"
#include "aui_ldl.h"
#include "aui_button.h"
#include "aui_thumb.h"
#include "aui_action.h"
#include "aui_static.h"
#include "aui_ranger.h"
#include <string>


static const MBCHAR *k_AUI_RANGER_LDL_OVERLAP		= "overlap";
static const MBCHAR *k_AUI_RANGER_LDL_DISPLAY		= "display";
static const MBCHAR *k_AUI_RANGER_LDL_AUTO_DISPLAY	= "autodisplay";
static const MBCHAR *k_AUI_RANGER_LDL_AUTO_DECX		= "autodecx";
static const MBCHAR *k_AUI_RANGER_LDL_AUTO_INCX		= "autoincx";
static const MBCHAR *k_AUI_RANGER_LDL_AUTO_DECY		= "autodecy";
static const MBCHAR *k_AUI_RANGER_LDL_AUTO_INCY		= "autoincy";


aui_Ranger::aui_Ranger(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_Control( retval, id, ldlBlock, ActionFunc, cookie ),
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (const MBCHAR *)NULL )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateButtonsAndThumb( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



aui_Ranger::aui_Ranger(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	AUI_RANGER_TYPE type,
	AUI_RANGER_ORIENTATION orientation,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_Control( retval, id, x, y, width, height, ActionFunc, cookie ),
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon( type, orientation );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateButtonsAndThumb();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE aui_Ranger::InitCommonLdl( MBCHAR *ldlBlock )
{
	aui_Ldl *theLdl = g_ui->GetLdl();

	
	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	
	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	AUI_RANGER_TYPE type;
	MBCHAR *keyword = block->GetString( k_AUI_RANGER_LDL_TYPE );
	if ( !keyword )
		type = AUI_RANGER_TYPE_SCROLLER;
	else
	{
		if ( stricmp( keyword, k_AUI_RANGER_LDL_SLIDER ) == 0 )
			type = AUI_RANGER_TYPE_SLIDER;
		else if ( stricmp( keyword, k_AUI_RANGER_LDL_SPINNER ) == 0 )
			type = AUI_RANGER_TYPE_SPINNER;
		else
			type = AUI_RANGER_TYPE_SCROLLER;
	}

	AUI_RANGER_ORIENTATION orientation;
	keyword = block->GetString( k_AUI_RANGER_LDL_ORIENTATION );
	if ( !keyword )
		orientation = AUI_RANGER_ORIENTATION_BIDIRECTIONAL;
	else
	{
		if ( stricmp( keyword, k_AUI_RANGER_LDL_VERTICAL ) == 0 )
			orientation = AUI_RANGER_ORIENTATION_VERTICAL;
		else if ( stricmp( keyword, k_AUI_RANGER_LDL_HORIZONTAL ) == 0 )
			orientation = AUI_RANGER_ORIENTATION_HORIZONTAL;
		else
			orientation = AUI_RANGER_ORIENTATION_BIDIRECTIONAL;
	}

	AUI_ERRCODE errcode = InitCommon(
		type,
		orientation,
		block->GetAttributeType( k_AUI_RANGER_LDL_MINXVAL ) == ATTRIBUTE_TYPE_INT ?
			block->GetInt( k_AUI_RANGER_LDL_MINXVAL ) :
			k_AUI_RANGER_DEFAULTMINX,
		block->GetAttributeType( k_AUI_RANGER_LDL_MAXXVAL ) == ATTRIBUTE_TYPE_INT ?
			block->GetInt( k_AUI_RANGER_LDL_MAXXVAL ) :
			k_AUI_RANGER_DEFAULTMAXX,
		block->GetAttributeType( k_AUI_RANGER_LDL_MINYVAL ) == ATTRIBUTE_TYPE_INT ?
			block->GetInt( k_AUI_RANGER_LDL_MINYVAL ) :
			k_AUI_RANGER_DEFAULTMINY,
		block->GetAttributeType( k_AUI_RANGER_LDL_MAXYVAL ) == ATTRIBUTE_TYPE_INT ?
			block->GetInt( k_AUI_RANGER_LDL_MAXYVAL ) :
			k_AUI_RANGER_DEFAULTMAXY,
		block->GetAttributeType( k_AUI_RANGER_LDL_INCXVAL ) == ATTRIBUTE_TYPE_INT ?
			block->GetInt( k_AUI_RANGER_LDL_INCXVAL ) :
			k_AUI_RANGER_DEFAULTINCX,
		block->GetAttributeType( k_AUI_RANGER_LDL_INCYVAL ) == ATTRIBUTE_TYPE_INT ?
			block->GetInt( k_AUI_RANGER_LDL_INCYVAL ) :
			k_AUI_RANGER_DEFAULTINCY,
		block->GetAttributeType( k_AUI_RANGER_LDL_PAGEXVAL ) == ATTRIBUTE_TYPE_INT ?
			block->GetInt( k_AUI_RANGER_LDL_PAGEXVAL ) :
			k_AUI_RANGER_DEFAULTPAGEX,
		block->GetAttributeType( k_AUI_RANGER_LDL_PAGEYVAL ) == ATTRIBUTE_TYPE_INT ?
			block->GetInt( k_AUI_RANGER_LDL_PAGEYVAL ) :
			k_AUI_RANGER_DEFAULTPAGEY,
		block->GetAttributeType( k_AUI_RANGER_LDL_XVAL ) == ATTRIBUTE_TYPE_INT ?
			block->GetInt( k_AUI_RANGER_LDL_XVAL ) :
			k_AUI_RANGER_DEFAULTVALX,
		block->GetAttributeType( k_AUI_RANGER_LDL_YVAL ) == ATTRIBUTE_TYPE_INT ?
			block->GetInt( k_AUI_RANGER_LDL_YVAL ) :
			k_AUI_RANGER_DEFAULTVALY,
		block->GetBool( k_AUI_RANGER_LDL_RIGIDTHUMB ),
		block->GetBool( k_AUI_RANGER_LDL_QUANTIZEDRAG ),
		block->GetAttributeType( k_AUI_RANGER_LDL_MINTHUMBSIZE ) == ATTRIBUTE_TYPE_INT ?
			block->GetInt( k_AUI_RANGER_LDL_MINTHUMBSIZE ) :
			k_CONTROL_DEFAULT_SIZE );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	if(block->GetAttributeType(k_AUI_RANGER_LDL_OVERLAP) == ATTRIBUTE_TYPE_INT)
		m_overlap = block->GetInt(k_AUI_RANGER_LDL_OVERLAP);

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Ranger::InitCommon(
	AUI_RANGER_TYPE type,
	AUI_RANGER_ORIENTATION orientation,
	sint32 minx,
	sint32 maxx,
	sint32 miny,
	sint32 maxy,
	sint32 incx,
	sint32 incy,
	sint32 pagex,
	sint32 pagey,
	sint32 valx,
	sint32 valy,
	BOOL rigidThumb,
	BOOL quantizeDrag,
	sint32 minThumbSize )
{
	m_type = type,
	m_orientation = orientation,
	m_rangeContainer = NULL,
	m_thumb = NULL,
	m_incXButton = NULL,
	m_incYButton = NULL,
	m_decXButton = NULL,
	m_decYButton = NULL,
	m_overlap = 0,
	m_valX = valx,
	m_valY = valy,
	m_minX = minx,
	m_minY = miny,
	m_maxX = maxx,
	m_maxY = maxy,
	m_incX = incx,
	m_incY = incy,
	m_pageX = pagex,
	m_pageY = pagey,
	m_sliding = FALSE,
	m_slideDx = 0,
	m_slideDy = 0,
	m_quantized = quantizeDrag,
	m_rigidThumb = rigidThumb,
	m_minThumbSize = minThumbSize,
	m_buttonSize = k_CONTROL_DEFAULT_SIZE;

	return AUI_ERRCODE_OK;
}


aui_Button *aui_Ranger::CreateArrowButton(const MBCHAR *ldlBlock,
										  const MBCHAR *autoLdlName,
										  const MBCHAR *ldlName)
{
	
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	
	aui_Ldl *theLdl = g_ui->GetLdl();
	static MBCHAR block[k_AUI_LDL_MAXBLOCK + 1];

	
	aui_Button *arrowButton = NULL;

	
	if(ldlBlock) {
		
		arrowButton = (aui_Button *)aui_Ldl::BuildHierarchyFromRoot(const_cast<MBCHAR*>(
			std::string(ldlBlock).append(".").append(
			autoLdlName).c_str()));

		
		if(arrowButton) {
			
			arrowButton->SetActionFuncAndCookie(
				RangerButtonActionCallback, this);
		} else {	
			
			sprintf(block, "%s.%s", ldlBlock, ldlName);

			
			if(theLdl->GetLdl()->FindDataBlock(block))
				arrowButton = new aui_Button(&errcode, aui_UniqueId(),
				block, RangerButtonActionCallback, this);
		}
	}

	
	if(!arrowButton)
		arrowButton = new aui_Button(&errcode, aui_UniqueId(),
		0, 0, 0, 0, RangerButtonActionCallback, this);

	
	Assert(arrowButton);

	
	if(arrowButton)
		AddChild(arrowButton);

	
	return(arrowButton);
}


AUI_ERRCODE aui_Ranger::CreateButtonsAndThumb(MBCHAR *ldlBlock)
{
	
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	
	aui_Ldl *theLdl = g_ui->GetLdl();
	static MBCHAR block[k_AUI_LDL_MAXBLOCK + 1];

	
	
	if(ldlBlock) {
		
		m_rangeContainer = (aui_Control *)aui_Ldl::BuildHierarchyFromRoot(const_cast<MBCHAR*>(
			std::string(ldlBlock).append(".").append(
			k_AUI_RANGER_LDL_AUTO_DISPLAY).c_str()));
		
		

		
		if(!m_rangeContainer) {
			sprintf(block, "%s.%s", ldlBlock, k_AUI_RANGER_LDL_DISPLAY);
			if(theLdl->GetLdl()->FindDataBlock(block))
				m_rangeContainer = new aui_Static(&errcode,
				aui_UniqueId(), block);
		}
	}

	
	if(m_rangeContainer)
		AddChild(m_rangeContainer);

	
	if((m_type == AUI_RANGER_TYPE_SLIDER) ||
		(m_type == AUI_RANGER_TYPE_SCROLLER)) {
		
		
		if(ldlBlock) {
			sprintf( block, "%s.%s", ldlBlock, k_AUI_RANGER_LDL_THUMB );

			
			if(theLdl->GetLdl()->FindDataBlock(block))
				m_thumb = new aui_Thumb(&errcode, aui_UniqueId(), block,
				RangerThumbActionCallback, this);
		}

		
		if(!m_thumb)
			m_thumb = new aui_Thumb(&errcode, aui_UniqueId(),
				0, 0, 0, 0, RangerThumbActionCallback, this);

		
		Assert(AUI_NEWOK(m_thumb,errcode));
		if(!AUI_NEWOK(m_thumb,errcode))
			return AUI_ERRCODE_MEMALLOCFAILED;

		
		AddChild(m_thumb);

		
		RepositionThumb(false);
	}

	
	if((m_type == AUI_RANGER_TYPE_SCROLLER) ||
		(m_type == AUI_RANGER_TYPE_SPINNER )) {
		
		if((m_orientation == AUI_RANGER_ORIENTATION_HORIZONTAL) ||
			(m_orientation == AUI_RANGER_ORIENTATION_BIDIRECTIONAL)) {
			
			m_incXButton = CreateArrowButton(ldlBlock,
				k_AUI_RANGER_LDL_AUTO_INCX, k_AUI_RANGER_LDL_INCX);

			
			if(!m_incXButton)
				return(AUI_ERRCODE_MEMALLOCFAILED);

			
			m_decXButton = CreateArrowButton(ldlBlock,
				k_AUI_RANGER_LDL_AUTO_DECX, k_AUI_RANGER_LDL_DECX);

			
			if(!m_decXButton)
				return(AUI_ERRCODE_MEMALLOCFAILED);
		}

		
		if((m_orientation == AUI_RANGER_ORIENTATION_VERTICAL) ||
			(m_orientation == AUI_RANGER_ORIENTATION_BIDIRECTIONAL)) {
			
			m_incYButton = CreateArrowButton(ldlBlock,
				k_AUI_RANGER_LDL_AUTO_INCY, k_AUI_RANGER_LDL_INCY);

			
			if(!m_incYButton)
				return(AUI_ERRCODE_MEMALLOCFAILED);

			
			m_decYButton = CreateArrowButton(ldlBlock,
				k_AUI_RANGER_LDL_AUTO_DECY, k_AUI_RANGER_LDL_DECY);

			
			if(!m_decYButton)
				return(AUI_ERRCODE_MEMALLOCFAILED);
		}

		
		RepositionButtons();
	}

	
	return(errcode);
}



aui_Ranger::~aui_Ranger()
{
	if ( m_thumb )
	{
		delete m_thumb;
		m_thumb = NULL;
	}

	if ( m_incXButton )
	{
		delete m_incXButton;
		m_incXButton = NULL;
	}

	if ( m_incYButton )
	{
		delete m_incYButton;
		m_incYButton = NULL;
	}

	if ( m_decXButton )
	{
		delete m_decXButton;
		m_decXButton = NULL;
	}

	if ( m_decYButton )
	{
		delete m_decYButton;
		m_decYButton = NULL;
	}

	if(m_rangeContainer)
	{
		delete m_rangeContainer;
		m_rangeContainer = NULL;
	}
}



AUI_ERRCODE aui_Ranger::Show( void )
{
	AUI_ERRCODE errcode = aui_Control::Show();

	if ( m_orientation == AUI_RANGER_ORIENTATION_VERTICAL )
	{
		if ( m_incXButton && !m_incXButton->IsHidden() ) m_incXButton->Hide();
		if ( m_decXButton && !m_decXButton->IsHidden() ) m_decXButton->Hide();
	}
	else if ( m_orientation == AUI_RANGER_ORIENTATION_HORIZONTAL )
	{
		if ( m_incYButton && !m_incYButton->IsHidden() ) m_incYButton->Hide();
		if ( m_decYButton && !m_decYButton->IsHidden() ) m_decYButton->Hide();
	}

	return errcode;
}



AUI_ERRCODE aui_Ranger::Resize( sint32 width, sint32 height )
{
	aui_Control::Resize( width, height );

	RepositionThumb();
	RepositionButtons();

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Ranger::SetValue( sint32 valX, sint32 valY )
{
	
	if ( valX < m_minX ) valX = m_minX;
	if ( valX > m_maxX ) valX = m_maxX;
	if ( valY < m_minY ) valY = m_minY;
	if ( valY > m_maxY ) valY = m_maxY;
	
	if ( m_valX != valX || m_valY != valY )
	{
		m_valX = valX, m_valY = valY;

		return RepositionThumb();
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Ranger::SetMinimum( sint32 minX, sint32 minY )
{
	Assert( minX <= m_maxX );
	Assert( minY <= m_maxY );
	if ( minX > m_maxX
	||   minY > m_maxY )
		return AUI_ERRCODE_INVALIDDIMENSION;

	m_minX = minX, m_minY = minY;

	
	if ( m_valX < m_minX ) m_valX = m_minX;
	if ( m_valY < m_minY ) m_valY = m_minY;

	return RepositionThumb();
}



AUI_ERRCODE aui_Ranger::SetMaximum( sint32 maxX, sint32 maxY )
{
	Assert( maxX >= m_minX );
	Assert( maxY >= m_minY );
	if ( maxX < m_minX
	||   maxY < m_minY )
		return AUI_ERRCODE_INVALIDDIMENSION;

	m_maxX = maxX, m_maxY = maxY;

	
	if ( m_valX > m_maxX ) m_valX = m_maxX;
	if ( m_valY > m_maxY ) m_valY = m_maxY;

	return RepositionThumb();
}



AUI_ERRCODE aui_Ranger::SetIncrement( sint32 incX, sint32 incY )
{
	Assert( incX >= 0 );

	Assert( incY >= 0 );

	if ( incX < 0
	||   incX > m_pageX
	||   incY < 0
	||   incY > m_pageY )
		return AUI_ERRCODE_INVALIDDIMENSION;

	m_incX = incX, m_incY = incY;

	return RepositionThumb();
}



AUI_ERRCODE aui_Ranger::SetPage( sint32 pageX, sint32 pageY )
{


	if ( pageX < m_incX
	||   pageY < m_incY )
		return AUI_ERRCODE_INVALIDDIMENSION;

	m_pageX = pageX, m_pageY = pageY;

	return RepositionThumb();
}



AUI_ERRCODE aui_Ranger::IncrementUpX( void )
{
	m_valX += m_incX;
	if ( m_valX > m_maxX ) m_valX = m_maxX;
	return RepositionThumb();
}



AUI_ERRCODE aui_Ranger::IncrementUpY( void )
{
	m_valY += m_incY;
	if ( m_valY > m_maxY ) m_valY = m_maxY;
	return RepositionThumb();
}



AUI_ERRCODE aui_Ranger::IncrementDownX( void )
{
	m_valX -= m_incX;
	if ( m_valX < m_minX ) m_valX = m_minX;
	return RepositionThumb();
}



AUI_ERRCODE aui_Ranger::IncrementDownY( void )
{
	m_valY -= m_incY;
	if ( m_valY < m_minY ) m_valY = m_minY;
	return RepositionThumb();
}



AUI_ERRCODE aui_Ranger::PageUpX( void )
{
	m_valX += m_pageX;
	if ( m_valX > m_maxX ) m_valX = m_maxX;
	return RepositionThumb();
}



AUI_ERRCODE aui_Ranger::PageUpY( void )
{
	m_valY += m_pageY;
	if ( m_valY > m_maxY ) m_valY = m_maxY;
	return RepositionThumb();
}



AUI_ERRCODE aui_Ranger::PageDownX( void )
{
	m_valX -= m_pageX;
	if ( m_valX < m_minX ) m_valX = m_minX;
	return RepositionThumb();
}



AUI_ERRCODE aui_Ranger::PageDownY( void )
{
	m_valY -= m_pageY;
	if ( m_valY < m_minY ) m_valY = m_minY;
	return RepositionThumb();
}



AUI_ERRCODE aui_Ranger::ThumbMoved( BOOL forceQuantize )
{
	
	if ( m_thumb )
	{
		m_valX = m_thumb->X();
		m_valY = m_thumb->Y();

		PositionToValue( &m_valX, &m_valY );

		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;

		if ( m_quantized || forceQuantize )
			RepositionThumb();
		else if ( m_ActionFunc )
			m_ActionFunc( this, AUI_RANGER_ACTION_VALUECHANGE, 0, m_cookie );
		else if ( m_action )
			m_action->Execute( this, AUI_RANGER_ACTION_VALUECHANGE, 0 );
	}

	return AUI_ERRCODE_OK;
}





AUI_ERRCODE aui_Ranger::RepositionThumb( BOOL callActionFunc )
{
	
	if ( m_thumb )
	{
		
		sint32 range;
		sint32 page;

		range = m_maxX - m_minX;
		page = m_pageX + 1;
		sint32 w = sint32(double(m_width * page) / double(page + range) + 0.5);

		if ( m_rigidThumb && range ) w = m_minThumbSize;

		range = m_maxY - m_minY;
		page = m_pageY + 1;
		sint32 h = sint32(double(m_height * page) / double(page + range) + 0.5);

		if ( m_rigidThumb && range ) h = m_minThumbSize;

		
		if ( m_orientation != AUI_RANGER_ORIENTATION_VERTICAL
		&&   w < m_minThumbSize )
			w = m_width >= m_minThumbSize ? m_minThumbSize : m_width;
		if ( m_orientation != AUI_RANGER_ORIENTATION_HORIZONTAL
		&&   h < m_minThumbSize )
			h = m_height >= m_minThumbSize ? m_minThumbSize : m_height;

		m_thumb->Resize( w, h );

		
		sint32 x = m_valX;
		sint32 y = m_valY;

		ValueToPosition( &x, &y );

		m_thumb->Move( x, y );
	}

	
	
	
	
	if(m_incXButton) {
		if(m_valX >= m_maxX) {
			if(!m_incXButton->IsDisabled())
				m_incXButton->Enable(false);
		} else if(m_incXButton->IsDisabled())
			m_incXButton->Enable(true);
	}
	if(m_decXButton) {
		if(m_valX <= m_minX) {
			if(!m_decXButton->IsDisabled())
				m_decXButton->Enable(false);
		} else if(m_decXButton->IsDisabled())
			m_decXButton->Enable(true);
	}
	if(m_incYButton) {
		if(m_valY >= m_maxY) {
			if(!m_incYButton->IsDisabled())
				m_incYButton->Enable(false);
		} else if(m_incYButton->IsDisabled())
			m_incYButton->Enable(true);
	}
	if(m_decYButton) {
		if(m_valY <= m_minY) {
			if(!m_decYButton->IsDisabled())
				m_decYButton->Enable(false);
		} else if(m_decYButton->IsDisabled())
			m_decYButton->Enable(true);
	}

	m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;

	if ( callActionFunc )
	{
		if ( m_ActionFunc )
			m_ActionFunc( this, AUI_RANGER_ACTION_VALUECHANGE, 0, m_cookie );
		else if ( m_action )
			m_action->Execute( this, AUI_RANGER_ACTION_VALUECHANGE, 0 );
	}
	
	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Ranger::RepositionButtons()
{
	
	if(m_rangeContainer) {
		
		if(m_decXButton)
			m_decXButton->Move( 0, 0 );
		if(m_decYButton)
			m_decYButton->Move( 0, 0 );

		
		
		if(m_incXButton)
			m_incXButton->Move( m_width - m_incXButton->Width(), 0 );
		if(m_incYButton)
			m_incYButton->Move( 0, m_height - m_incYButton->Height() );

		
		
		if((m_orientation == AUI_RANGER_ORIENTATION_HORIZONTAL) ||
			(m_orientation == AUI_RANGER_ORIENTATION_BIDIRECTIONAL))
			m_rangeContainer->Resize(
			Width() -
			(m_decXButton ? (m_decXButton->Width() - m_overlap) : 0) -
			(m_incXButton ? (m_incXButton->Width() - m_overlap) : 0),
			m_rangeContainer->Height());

		
		
		if((m_orientation == AUI_RANGER_ORIENTATION_VERTICAL) ||
			(m_orientation == AUI_RANGER_ORIENTATION_BIDIRECTIONAL))
			m_rangeContainer->Resize(
			m_rangeContainer->Width(),
			Height() -
			(m_decXButton ? (m_decXButton->Height() - m_overlap) : 0) -
			(m_incXButton ? (m_incXButton->Height() - m_overlap) : 0));

		
		m_rangeContainer->Move(m_decXButton ?
			(m_decXButton->Width() - m_overlap) : 0,
			m_decYButton ? (m_decYButton->Height() - m_overlap) : 0);
	} else {	
		
		if(m_buttonSize) {
			if(m_decXButton)
				m_decXButton->Resize(m_buttonSize, m_decXButton->Height());
			if(m_decYButton)
				m_decYButton->Resize(m_decYButton->Width(), m_buttonSize);
			if(m_incXButton)
				m_incXButton->Resize(m_buttonSize, m_incXButton->Height());
			if(m_incYButton)
				m_incYButton->Resize(m_decYButton->Width(), m_buttonSize);
		}

		
		if(m_decXButton)
			m_decXButton->Move(-m_decXButton->Width(), 0);
		if(m_decYButton)
			m_decYButton->Move(0, -m_decYButton->Height());
		if(m_incXButton)
			m_incXButton->Move(m_width, 0);
		if(m_incYButton)
			m_incYButton->Move(0, m_height);
	}

	
	return(AUI_ERRCODE_OK);
}



AUI_ERRCODE aui_Ranger::ValueToPosition( sint32 *x, sint32 *y )
{
	
	if ( !m_thumb ) return AUI_ERRCODE_NOCONTROL;

	Assert( m_minX <= *x );
	Assert( *x <= m_maxX );
	Assert( m_minY <= *y );
	Assert( *y <= m_maxY );
	if ( *x < m_minX
	||   *x > m_maxX
	||   *y < m_minY
	||   *y > m_maxY )
		return AUI_ERRCODE_INVALIDDIMENSION;

	
	
	sint32 range;

	*x = (range = m_maxX - m_minX) ?
		sint32(double(( m_width - m_thumb->Width() ) * ( *x - m_minX )) / (double)range + 0.5):
		0;

	*y = (range = m_maxY - m_minY) ?
		sint32(double(( m_height - m_thumb->Height() ) * ( *y - m_minY )) / (double)range + 0.5):
		0;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Ranger::PositionToValue( sint32 *x, sint32 *y )
{
	
	if ( !m_thumb ) return AUI_ERRCODE_NOCONTROL;

	Assert( 0 <= *x );
	Assert( *x <= m_width - m_thumb->Width() );
	Assert( 0 <= *y );
	Assert( *y <= m_height - m_thumb->Height() );
	if ( *x < 0
	||   *x > m_width - m_thumb->Width()
	||   *y < 0
	||   *y > m_height - m_thumb->Height() )
		return AUI_ERRCODE_INVALIDDIMENSION;

	
	
	sint32 range;

	*x = m_minX + (
		(range = m_width - m_thumb->Width()) ?
		sint32(double(( m_maxX - m_minX ) * *x) / (double)range + 0.5):
		0 );

	*y = m_minY + (
		(range = m_height - m_thumb->Height()) ?
		sint32(double(( m_maxY - m_minY ) * *y) / (double)range + 0.5):
		0 );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Ranger::CenterPositionToValue( sint32 *x, sint32 *y )
{
	
	if ( !m_thumb ) return AUI_ERRCODE_NOCONTROL;

	Assert( 0 <= *x );
	Assert( *x < m_width );
	Assert( 0 <= *y );
	Assert( *y < m_height );
	if ( *x < 0
	||   *x >= m_width
	||   *y < 0
	||   *y >= m_height )
		return AUI_ERRCODE_INVALIDDIMENSION;

	
	
	sint32 range;

	*x = m_minX + (
		(range = m_width - m_thumb->Width()) ?
		sint32(double(( m_maxX - m_minX ) *
		( *x - m_thumb->Width() / 2 )) /
		(double)range + 0.5):
		0 );

	*y = m_minY + sint32(
		(range = m_height - m_thumb->Height()) ?
		sint32(double(( m_maxY - m_minY ) *
		( *y - m_thumb->Height() / 2 )) /
		(double)range + 0.5):
		0 );

	
	
	if ( m_valX < m_minX )
		m_valX = m_minX;
	else if ( m_valX > m_maxX )
		m_valX = m_maxX;

	if ( m_valY < m_minY )
		m_valY = m_minY;
	else if ( m_valY > m_maxY )
		m_valY = m_maxY;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Ranger::SlideThumb( void )
{
	m_valX += m_slideDx;
	m_valY += m_slideDy;

	if ( m_valX < m_minX )
		m_valX = m_minX;
	else if ( m_valX > m_maxX )
		m_valX = m_maxX;

	if ( m_valY < m_minY )
		m_valY = m_minY;
	else if ( m_valY > m_maxY )
		m_valY = m_maxY;

	return RepositionThumb();
}








void aui_Ranger::PostChildrenCallback( aui_MouseEvent *mouseData )
{
	
	
	if ( m_sliding && mouseData->time - m_startWaitTime > m_timeOut )
	if ( mouseData->time - m_lastRepeatTime > m_repeatTime )
	{
		m_lastRepeatTime = mouseData->time;

		SlideThumb();

		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;
		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;
	}
}



void aui_Ranger::MouseMoveInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( GetWhichSeesMouse() && GetWhichSeesMouse() != this && GetWhichSeesMouse() != m_thumb )
		MouseMoveAway( mouseData );
	else if ( !IsActive() )
		MouseMoveOver( mouseData );
}



void aui_Ranger::MouseMoveOver( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this || GetWhichSeesMouse() == m_thumb )
	{
		if ( !GetWhichSeesMouse() )
			SetWhichSeesMouse( this );

		PlaySound( AUI_SOUNDBASE_SOUND_ACTIVATE );

		
		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;

		
		m_attributes |= k_CONTROL_ATTRIBUTE_ACTIVE;
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSEMOVEOVER;

		if ( m_ActionFunc )
			m_ActionFunc( this, 0, 0, m_cookie );
		else if ( m_action )
			m_action->Execute( this, 0, 0 );
	}
	else
		MouseMoveOutside( mouseData );
}



void aui_Ranger::MouseLDragOver( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		
		if ( !m_thumb ) return;

		if ( GetMouseOwnership() == this )
		{
			m_sliding = TRUE;

			if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
				m_mouseCode = AUI_ERRCODE_HANDLED;
		}
	}
	else
		MouseLDragOutside( mouseData );
}



void aui_Ranger::MouseLDragAway( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	
	if ( !m_thumb ) return;

	if ( GetMouseOwnership() == this )
	{
		m_sliding = FALSE;

		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;
	}
}



void aui_Ranger::MouseLGrabInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	
	if ( !m_thumb ) return;

	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		
		HideTipWindow();

		
		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
		{
			PlaySound( AUI_SOUNDBASE_SOUND_ENGAGE );

			
			SetMouseOwnership();
			SetKeyboardFocus();

			
			sint32 x = mouseData->position.x - m_x;
			sint32 y = mouseData->position.y - m_y;

			
			if ( x < m_thumb->X() )
				m_slideDx = -m_pageX;
			else if ( x >= m_thumb->X() + m_thumb->Width() )
				m_slideDx = m_pageX;
			else
				m_slideDx = 0;

			if ( y < m_thumb->Y() )
				m_slideDy = -m_pageY;
			else if ( y >= m_thumb->Y() + m_thumb->Height() )
				m_slideDy = m_pageY;
			else
				m_slideDy = 0;

			
			
			SlideThumb();
			m_sliding = TRUE;
			m_startWaitTime = mouseData->time;

			m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;

			m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELGRABINSIDE;
		}
	}
	else
		MouseLGrabOutside( mouseData );
}



void aui_Ranger::MouseLDropInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	
	if ( !m_thumb ) return;

	if ( GetWhichSeesMouse() == m_thumb ) m_sliding = FALSE;
	else if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this)
	{
		SetWhichSeesMouse( this );

		if ( GetMouseOwnership() == this)
		{
			PlaySound( AUI_SOUNDBASE_SOUND_EXECUTE );

			
			ReleaseMouseOwnership();

			
			m_sliding = FALSE;

			HandleGameSpecificLeftClick( this );

			if(m_ActionFunc) {
				m_ActionFunc(this, AUI_RANGER_ACTION_RELEASE, 0, m_cookie);
			}
		}
		else
		{
			PlaySound( AUI_SOUNDBASE_SOUND_ACTIVATE );

			m_attributes |= k_CONTROL_ATTRIBUTE_ACTIVE;
			m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDROPINSIDE;
		}

		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;
	}
	else
		MouseLDropOutside( mouseData );
}


void aui_Ranger::MouseRDropInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this ) {
		SetWhichSeesMouse( this );

		HandleGameSpecificRightClick((void *)this);
		m_mouseCode = AUI_ERRCODE_HANDLED;
	}
	else {
		MouseRDropOutside( mouseData );
	}
}



void aui_Ranger::MouseLDropOutside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	
	if ( !m_thumb ) return;

	if ( GetMouseOwnership() == this  )
	{
		
		ReleaseMouseOwnership();

		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;

		m_attributes &= ~k_CONTROL_ATTRIBUTE_ACTIVE;
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDROPOUTSIDE;

	}
}



void aui_Ranger::MouseRGrabInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	
	if ( !m_thumb ) return;

	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		PlaySound( AUI_SOUNDBASE_SOUND_ENGAGE );

		m_valX = mouseData->position.x - m_x;
		m_valY = mouseData->position.y - m_y;

		CenterPositionToValue( &m_valX, &m_valY );

		RepositionThumb();

		m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;

		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSERGRABINSIDE;
	}
	else
		MouseRGrabOutside( mouseData );
}








void RangerThumbActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	aui_Ranger *ranger = (aui_Ranger *)cookie;

	switch ( action )
	{
	case AUI_THUMB_ACTION_DRAG:
		ranger->ThumbMoved();
		break;

	case AUI_THUMB_ACTION_DROP:
		ranger->ThumbMoved( TRUE );
		if(ranger->GetActionFunc()) {
			ranger->GetActionFunc()(ranger, AUI_RANGER_ACTION_RELEASE, 0, ranger->GetCookie());
		}
		break;
	}
}



void RangerButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	
	if ( action != (uint32)AUI_BUTTON_ACTION_PRESS ) return;

	aui_Ranger *ranger = (aui_Ranger *)cookie;
	aui_Button *button = (aui_Button *)control;

	if ( button == ranger->GetIncrementXButton() )
		ranger->IncrementUpX();
	else if ( button == ranger->GetIncrementYButton() )
		ranger->IncrementUpY();
	else if ( button == ranger->GetDecrementXButton() )
		ranger->IncrementDownX();
	else
		ranger->IncrementDownY();
}
