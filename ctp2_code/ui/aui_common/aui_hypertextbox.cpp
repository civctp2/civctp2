










#include "c3.h"
#include "aui_ui.h"
#include "aui_window.h"
#include "aui_uniqueid.h"
#include "aui_ranger.h"
#include "aui_ldl.h"
#include "aui_static.h"

#include "aui_hypertextbox.h"

aui_HyperTextBox::aui_HyperTextBox(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (const MBCHAR *)NULL ),
	aui_Control( retval, id, ldlBlock, ActionFunc, cookie ),
	aui_HyperTextBase( retval, ldlBlock )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateRanger( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



aui_HyperTextBox::aui_HyperTextBox(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL ),
	aui_Control( retval, id, x, y, width, height, ActionFunc, cookie ),
	aui_HyperTextBase( retval, NULL, 0 )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateRanger( NULL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE aui_HyperTextBox::InitCommonLdl( MBCHAR *ldlBlock )
{
    ldl_datablock * block = aui_Ldl::FindDataBlock(ldlBlock);
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	AUI_ERRCODE errcode = InitCommon();
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	m_alwaysRanger = block->GetBool( k_AUI_HYPERTEXTBOX_LDL_ALWAYSRANGER );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_HyperTextBox::InitCommon( void )
{
	m_ranger = NULL;
	m_rangerSize = k_CONTROL_DEFAULT_SIZE;
	m_alwaysRanger = FALSE;

	m_virtualHeight = 0;
	m_curStaticPos.x = m_curStaticPos.y = 0;

	
	
	memset( m_hyperTtffile, 0, sizeof( m_hyperTtffile ) );
	strncpy( m_hyperTtffile, m_textttffile, MAX_PATH );
	m_hyperPointSize = m_textpointsize;
	m_hyperBold = m_textbold;
	m_hyperItalic = m_textitalic;
	m_hyperColor = m_textcolor;
	m_hyperUnderline = m_textunderline;
	m_hyperShadow = m_textshadow;
	m_hyperShadowColor = m_textshadowcolor;
	
	
	m_hyperFlags = k_AUI_BITMAPFONT_DRAWFLAG_JUSTLEFT;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_HyperTextBox::CreateRanger( MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	static MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( ldlBlock )
	{
		sprintf( block, "%s.%s", ldlBlock, k_AUI_HYPERTEXTBOX_LDL_RANGERY );

		
        if (aui_Ldl::GetLdl()->FindDataBlock(block))
			m_ranger = new aui_Ranger(
				&errcode,
				aui_UniqueId(),
				block,
				HyperTextBoxRangerActionCallback,
				this );
	}

	if ( !m_ranger )
		m_ranger = new aui_Ranger(
			&errcode,
			aui_UniqueId(),
			m_width,
			k_CONTROL_DEFAULT_SIZE,
			m_height - 2 * k_CONTROL_DEFAULT_SIZE,
			k_CONTROL_DEFAULT_SIZE,
			AUI_RANGER_TYPE_SCROLLER,
			AUI_RANGER_ORIENTATION_VERTICAL,
			HyperTextBoxRangerActionCallback,
			this );

	Assert( AUI_NEWOK(m_ranger,errcode) );
	if ( !AUI_NEWOK(m_ranger,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

	AddChild( m_ranger );

	return RepositionRanger();
}



aui_HyperTextBox::~aui_HyperTextBox()
{
	delete m_ranger;
}



AUI_ERRCODE aui_HyperTextBox::SetHyperText( const MBCHAR *hyperText )
{
	AUI_ERRCODE errcode = aui_HyperTextBase::SetHyperText( hyperText );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	return RepositionRanger();
}



AUI_ERRCODE aui_HyperTextBox::AppendHyperText( const MBCHAR *hyperText )
{
	AUI_ERRCODE errcode = aui_HyperTextBase::AppendHyperText( hyperText );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	return RepositionRanger();
}



AUI_ERRCODE aui_HyperTextBox::AddHyperStatics( const MBCHAR *hyperText )
{
	if ( !hyperText )
	{
		
		RemoveHyperStatics();
		m_virtualHeight = 0;
		m_curStaticPos.x = m_curStaticPos.y = 0;
		hyperText = m_hyperText;
	}

	uint32 len = strlen( hyperText );
	if ( !len ) return AUI_ERRCODE_OK;

	const MBCHAR *ptr = hyperText;
	const MBCHAR *stop = ptr + len;

	while ( ptr != stop )
	{
		if ( *ptr == '<' )
		{
			switch ( *++ptr )
			{
			case 't':
				sscanf( ++ptr, ":%[^>]", m_hyperTtffile );
				break;

			case 'p':
				sscanf( ++ptr, ":%d>", &m_hyperPointSize );
				break;

			case 'b':
				sscanf( ++ptr, ":%d>", &m_hyperBold );
				break;

			case 'i':
				sscanf( ++ptr, ":%d>", &m_hyperItalic );
				break;

			case 'c':
			{
				unsigned int r, g, b;
				sscanf( ++ptr, ":%u,%u,%u>", &r, &g, &b );
				m_hyperColorOld = m_hyperColor;
				m_hyperColor = RGB(r,g,b);
			}
			break;

			case 'e':
				m_hyperColor = m_hyperColorOld;
				break;

			case 'u':
				sscanf( ++ptr, ":%d>", &m_hyperUnderline );
				break;

			case 's':
				sscanf( ++ptr, ":%d>", &m_hyperShadow );
				break;

			case 'h':
			{
				unsigned int r, g, b;
				sscanf( ++ptr, ":%u,%u,%u>", &r, &g, &b );
				m_hyperShadowColor = RGB(r,g,b);
			}
			break;

			default:
				
				Assert( FALSE );
				return AUI_ERRCODE_INVALIDPARAM;
			}

			while ( *ptr++ != '>' )
				;
		}
		else
		{
			const MBCHAR *nextCmd = strchr( ptr, '<' );
			len = nextCmd ? nextCmd - ptr : strlen( ptr );

			const MBCHAR *nextLine = strchr( ptr, '\n' );
			if ( nextLine && ( !nextCmd || nextLine < nextCmd ) )
				len = nextLine - ptr + 1;

			const MBCHAR *subStop = ptr + len;
			while ( ptr != subStop )
			{
				aui_Static *hs = CreateHyperStatic(
					ptr,
					len,
					m_hyperTtffile,
					m_hyperPointSize,
					m_hyperBold,
					m_hyperItalic,
					m_hyperColor,
					m_hyperUnderline,
					m_hyperShadow,
					m_hyperShadowColor,
					m_hyperFlags );
				Assert( hs != NULL );
				if ( !hs ) return AUI_ERRCODE_MEMALLOCFAILED;

				m_hyperStaticList->AddTail( hs );

				sint32 nextX = m_curStaticPos.x + hs->Width();
				sint32 nextY = m_curStaticPos.y + hs->Height();

				if ( nextX > m_width )
				{
					RECT wrap =
					{
						0,
						0,
						m_width - m_curStaticPos.x,
						0
					};

					POINT penPos = { 0, 0 };

					const MBCHAR *start = ptr;
					hs->GetTextFont()->GetLineInfo(
						&wrap,
						&penPos,
						NULL,
						NULL,
						&ptr,
						subStop );

					if ( ptr == subStop )
					{
						hs->Move(
							m_curStaticPos.x = 0,
							m_curStaticPos.y = m_virtualHeight );

						m_virtualHeight += hs->Height();

						if ( hs->Width() > m_width )
							m_curStaticPos.y = m_virtualHeight;
						else
							m_curStaticPos.x = hs->Width();
					}
					else
					{
						uint32 truncLen = ptr - start;

						penPos.x = penPos.y = 0;
						const MBCHAR *testPtr = start;
						const MBCHAR *testSubStop = testPtr + truncLen;
						hs->GetTextFont()->GetLineInfo(
							&wrap,
							&penPos,
							NULL,
							NULL,
							&testPtr,
							testSubStop );

						if ( penPos.x > wrap.right )
						{
							hs->Move(
								m_curStaticPos.x = 0,
								m_curStaticPos.y = m_virtualHeight );

							m_virtualHeight += hs->Height();

							if ( hs->Width() > m_width  )
								m_curStaticPos.y = m_virtualHeight;
							else
								m_curStaticPos.x = hs->Width();

							ptr = subStop;
						}
						else
						{
							len -= truncLen;
							hs->SetText( start, truncLen );

							hs->Move( m_curStaticPos.x, m_curStaticPos.y );
							hs->Resize( penPos.x, hs->Height() );

							if ( nextY > m_virtualHeight )
								m_virtualHeight = nextY;

							m_curStaticPos.x = 0;
							m_curStaticPos.y = m_virtualHeight;
						}
					}
				}
				else
				{
					ptr = subStop;

					hs->Move( m_curStaticPos.x, m_curStaticPos.y );

					m_curStaticPos.x = nextX;

					if ( nextY > m_virtualHeight )
						m_virtualHeight = nextY;
				}

				if ( *(ptr - 1) == '\n' )
				{
					m_curStaticPos.x = 0;
					m_curStaticPos.y = m_virtualHeight;
				}

				
				if (m_hyperStaticList->L() > k_AUI_HYPERTEXTBOX_LDL_MAXSTATICS)
				{
					delete m_hyperStaticList->RemoveHead();

					sint32 topY = m_hyperStaticList->GetHead()->Y();
					ListPos pos = m_hyperStaticList->GetHeadPosition();
					for ( sint32 i = m_hyperStaticList->L(); i; i-- )
						m_hyperStaticList->GetNext( pos )->Offset( 0, -topY );
				}
			}
		}
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_HyperTextBox::Resize( sint32 width, sint32 height )
{
	aui_Control::Resize( width, height );

	RepositionRanger();
	AddHyperStatics( NULL );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_HyperTextBox::Show( void )
{
	aui_Control::Show();

	
	RepositionRanger();

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_HyperTextBox::RepositionRanger( void )
{
	sint32 rangerButtonSize = m_ranger->GetButtonSize();
	sint32 rangerDimension = m_height - 2 * rangerButtonSize;
	if ( rangerDimension < 0 ) rangerDimension = 0;

	
	m_ranger->Resize( m_rangerSize, rangerDimension );

	
	m_ranger->Move( m_width, rangerButtonSize );

	
	sint32 newMax = m_virtualHeight - m_height;
	if ( newMax < 0 ) newMax = 0;

	m_ranger->SetMaximum( m_ranger->GetMaximumX(), newMax );
	m_ranger->SetPage( m_ranger->GetPageX(), m_height );
	m_ranger->SetIncrement( m_ranger->GetIncrementX(), rangerButtonSize );

	if ( !IsHidden() && ( m_alwaysRanger || newMax ) )
	{
		if ( m_ranger->IsHidden() )
			m_ranger->Show();
	}
	else
	{
		if ( !m_ranger->IsHidden() )
		{
			m_ranger->Hide();
			if ( m_parent ) m_parent->ShouldDraw();
		}
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_HyperTextBox::DrawThis(
	aui_Surface *surface,
	sint32 x,
	sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	DrawThisStateImage(
		0,
		surface,
		&rect );

	DrawThisHyperText(
		surface,
		&rect,
		0,
		-m_ranger->GetValueY() );

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_HyperTextBox::RangerMoved( void )
{
	
	m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;

	return AUI_ERRCODE_OK;
}








void HyperTextBoxRangerActionCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	void *cookie )
{
	
	if ( action == (uint32)AUI_RANGER_ACTION_VALUECHANGE )
	{
		aui_HyperTextBox *hypertextbox = (aui_HyperTextBox *)cookie;

		hypertextbox->RangerMoved();
	}
}

