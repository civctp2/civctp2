


#include "c3.h"

#include "aui.h"
#include "aui_uniqueid.h"
#include "aui_ldl.h"
#include "aui_window.h"
#include "aui_bitmapfont.h"

#include "pixelutils.h"
#include "primitives.h"

#include "c3_hypertipwindow.h"
#include "aui_hypertextbox.h"

#include "colorset.h"           // g_colorSet
#include "c3windows.h"

#include "ctp2_button.h"
#include "ctp2_Static.h"
#include "c3ui.h"


#include "AdvanceRecord.h"
#include "player.h"
#include "SelItem.h"
#include "Advances.h"
#include "prjfile.h"

#include "chart.h"

#include "StrDB.h"
#include "IconRecord.h"

#include "advanceutil.h"

extern C3UI			*g_c3ui;
extern ProjectFile	*g_GreatLibPF;

#define k_CHART_PATTERN	"chart.tga"

Chart::Chart( AUI_ERRCODE *retval,
			 uint32 id,
			 MBCHAR *ldlBlock,
			 ControlActionCallback *ActionFunc,
			 void *cookie )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	ctp2_Static( retval, id, ldlBlock )
{
	InitCommon( ldlBlock );
}

Chart::Chart( AUI_ERRCODE *retval,
			 uint32 id,
			 sint32 x,
			 sint32 y,
			 sint32 width,
			 sint32 height,
			 MBCHAR *pattern,
			 ControlActionCallback *ActionFunc,
			 void *cookie )
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL ),
	ctp2_Static( retval, id, x, y, width, height, pattern, NULL, 0, 0, 0 )
{
	InitCommon( NULL );
}

AUI_ERRCODE Chart::InitCommon( MBCHAR *ldlBlock )
{
	sint32 i;
	AUI_ERRCODE errcode;

	m_centerButton = NULL;
	m_left = NULL;
	m_right = NULL;

	for ( i = 0;i < k_MAX_PREREQ; i++ )
	{
		m_preReqButton[i] = NULL;
	}

	for ( i = 0;i < k_MAX_EITHER_PREREQ; i++ )
	{
		m_eitherPreReqButton[i] = NULL;
	}

	for ( i = 0;i < k_MAX_LEADS_TO;i++ )
	{
		m_leadsToButton[i] = NULL;
	}

	static MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( ldlBlock )
	{
		sprintf( block, "%s.%s", ldlBlock, k_CHART_LDL_LEFTIMAGE );

        if (aui_Ldl::GetLdl()->FindDataBlock(block))
		{
			m_left = new ctp2_Static(
				&errcode,
				aui_UniqueId(),
				block );
		}
	}

	if ( ldlBlock )
	{
		sprintf( block, "%s.%s", ldlBlock, k_CHART_LDL_RIGHTIMAGE );

        if (aui_Ldl::GetLdl()->FindDataBlock( block ) )
		{
			m_right = new ctp2_Static(
				&errcode,
				aui_UniqueId(),
				block );
		}
	}

	if ( ldlBlock )
	{
		sprintf( block, "%s.%s", ldlBlock, k_CHART_LDL_BUTTON );

        if (aui_Ldl::GetLdl()->FindDataBlock( block ) )
		{
			m_centerButton = new ctp2_Button(
				&errcode,
				aui_UniqueId(),
				block,
				ChartCenterActionCallback, this );

			errcode = AddSubControl( m_centerButton );
			Assert( errcode == AUI_ERRCODE_OK );
			if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_CONTROLFAILURE;

			for ( i = 0; i < k_MAX_PREREQ; i++ )
			{
				m_preReqButton[i] = new ctp2_Button(
					&errcode,
					aui_UniqueId(),
					block,
					ChartPreReqActionCallback, this );

				errcode = AddSubControl( m_preReqButton[i] );
				Assert( errcode == AUI_ERRCODE_OK );
				if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_CONTROLFAILURE;
			}

			for ( i = 0; i < k_MAX_EITHER_PREREQ; i++ )
			{
				m_eitherPreReqButton[i] = new ctp2_Button(
					&errcode,
					aui_UniqueId(),
					block,
					ChartEitherPreReqActionCallback, this );

				errcode = AddSubControl( m_eitherPreReqButton[i] );
				Assert( errcode == AUI_ERRCODE_OK );
				if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_CONTROLFAILURE;
			}

			for ( i = 0; i < k_MAX_LEADS_TO;i++ )
			{
				m_leadsToButton[i] = new ctp2_Button(
					&errcode,
					aui_UniqueId(),
					block,
					ChartLeadsToActionCallback, this );

				errcode = AddSubControl( m_leadsToButton[i] );
				Assert( errcode == AUI_ERRCODE_OK );
				if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_CONTROLFAILURE;
			}
		}
	}

	if ( !m_centerButton )
	{
		m_centerButton = new ctp2_Button( 
			&errcode, 
			aui_UniqueId(),
			NULL, k_CHART_PATTERN,
			0, 0, 100, 20,  
			ChartCenterActionCallback, 
			this );
		errcode = AddSubControl( m_centerButton );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_CONTROLFAILURE;
		aui_BitmapFont * font = m_centerButton->GetTextFont();
		Assert(font);
		font->SetPointSize(14);
		m_centerButton->TextFlags() = k_AUI_BITMAPFONT_DRAWFLAG_JUSTCENTER;

		for ( i = 0;i < k_MAX_PREREQ;i++ )
		{
			m_preReqButton[i] = new ctp2_Button( &errcode, aui_UniqueId(),
				NULL, k_CHART_PATTERN, 0, 0, 100, 20, ChartPreReqActionCallback, this );
			errcode = AddSubControl( m_preReqButton[i] );
			Assert( errcode == AUI_ERRCODE_OK );
			if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_CONTROLFAILURE;
			m_preReqButton[i]->Hide();

			m_preReqButton[i]->TextFlags() = k_AUI_BITMAPFONT_DRAWFLAG_JUSTCENTER;
			font = m_preReqButton[i]->GetTextFont();
			Assert(font);
			font->SetPointSize(14);
		}

		for ( i = 0;i < k_MAX_EITHER_PREREQ;i++ )
		{
			m_eitherPreReqButton[i] = new ctp2_Button( &errcode, aui_UniqueId(),
				NULL, k_CHART_PATTERN, 0, 0, 100, 20, ChartEitherPreReqActionCallback, this );
			errcode = AddSubControl( m_eitherPreReqButton[i] );
			Assert( errcode == AUI_ERRCODE_OK );
			if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_CONTROLFAILURE;
			m_eitherPreReqButton[i]->Hide();

			m_eitherPreReqButton[i]->TextFlags() = k_AUI_BITMAPFONT_DRAWFLAG_JUSTCENTER;
			font = m_eitherPreReqButton[i]->GetTextFont();
			Assert(font);
			font->SetPointSize(14);
		}

		for ( i = 0;i < k_MAX_LEADS_TO;i++ )
		{
			m_leadsToButton[i] = new ctp2_Button( &errcode, aui_UniqueId(),
				NULL, k_CHART_PATTERN, 0, 0, 100, 20, ChartLeadsToActionCallback, this );
			errcode = AddSubControl( m_leadsToButton[i] );
			Assert( errcode == AUI_ERRCODE_OK );
			if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_CONTROLFAILURE;
			m_leadsToButton[i]->Hide();

			m_leadsToButton[i]->TextFlags() = k_AUI_BITMAPFONT_DRAWFLAG_JUSTCENTER;
			font = m_leadsToButton[i]->GetTextFont();
			Assert(font);
			font->SetPointSize(14);
		}
	}

	m_heightBetweenButtons = ( m_height - (m_centerButton->Height() * 4) ) / 5;
	m_distFromSide = ( (m_width - (m_centerButton->Width() * 3)) / 4 );
	m_distFromCenter = m_distFromSide;

	return AUI_ERRCODE_OK;
}

Chart::~Chart()
{
	sint32 i;

	for ( i = 0;i < k_MAX_PREREQ;i++ )
	{
		delete m_preReqButton[i];
	}

	for ( i = 0;i < k_MAX_EITHER_PREREQ;i++ )
	{
		delete m_eitherPreReqButton[i];
	}

	for ( i = 0;i < k_MAX_LEADS_TO;i++ )
	{
		delete m_leadsToButton[i];
	}

	delete m_centerButton;
	delete m_left;
	delete m_right;
}



AUI_ERRCODE Chart::Show()
{
	AUI_ERRCODE errcode = ctp2_Static::Show();
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_CONTROLFAILURE;

	int	i;

	for (i = m_numPreReq; i < k_MAX_PREREQ; i++)
	{
		m_preReqButton[i]->Hide();
	}

	for (i = m_numEitherPreReq; i < k_MAX_EITHER_PREREQ; i++)
	{
		m_eitherPreReqButton[i]->Hide();
	}

	for ( i = m_numLeadsTo;i < k_MAX_LEADS_TO;i++ )
	{
		m_leadsToButton[i]->Hide();
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE Chart::Draw( aui_Surface *surface, sint32 x, sint32 y )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	
	if ( !IsHidden() )
		errcode = DrawThis( surface, x, y );

	if ( errcode == AUI_ERRCODE_OK )
		DrawChildren( surface, x, y );

	m_draw = 0;

	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	sint32 i;
	RECT tempRect;

	for ( i = 0; i < m_numPreReq; i++ )
	{
		tempRect.left = m_preReqButton[i]->X();
		tempRect.top = m_preReqButton[i]->Y();
		tempRect.right = tempRect.left + m_preReqButton[i]->Width();
		tempRect.bottom = tempRect.top + m_preReqButton[i]->Height();
		InflateRect( &tempRect, 1, 1 );
		OffsetRect( &tempRect, rect.left, rect.top );
		primitives_FrameRect16( surface, &tempRect, g_colorSet->GetColor((COLOR)m_preReqColor[i]) );
	}

	for ( i = 0; i < m_numEitherPreReq; i++ )
	{
		tempRect.left = m_eitherPreReqButton[i]->X();
		tempRect.top = m_eitherPreReqButton[i]->Y();
		tempRect.right = tempRect.left + m_eitherPreReqButton[i]->Width();
		tempRect.bottom = tempRect.top + m_eitherPreReqButton[i]->Height();
		InflateRect( &tempRect, 1, 1 );
		OffsetRect( &tempRect, rect.left, rect.top );
		primitives_FrameRect16( surface, &tempRect, g_colorSet->GetColor((COLOR)m_eitherPreReqColor[i]) );
	}

	for ( i = 0; i < m_numLeadsTo; i++ )
	{
		tempRect.left = m_leadsToButton[i]->X();
		tempRect.top = m_leadsToButton[i]->Y();
		tempRect.right = tempRect.left + m_leadsToButton[i]->Width();
		tempRect.bottom = tempRect.top + m_leadsToButton[i]->Height();
		InflateRect( &tempRect, 1, 1 );
		OffsetRect( &tempRect, rect.left, rect.top );
		primitives_FrameRect16( surface, &tempRect, g_colorSet->GetColor((COLOR)m_leadsToColor[i]) );
	}

	tempRect.left = m_centerButton->X();
	tempRect.top = m_centerButton->Y();
	tempRect.right = tempRect.left + m_centerButton->Width();
	tempRect.bottom = tempRect.top + m_centerButton->Height();
	InflateRect( &tempRect, 1, 1 );
	OffsetRect( &tempRect, rect.left, rect.top );
	primitives_FrameRect16( surface, &tempRect, g_colorSet->GetColor((COLOR)m_centerColor) );

	POINT center, leftCenter, rightCenter;
	center.x = rect.left + m_centerButton->X();
	center.y = rect.top + m_centerButton->Y() + m_centerButton->Height() / 2;
	leftCenter.x = center.x - m_distFromCenter / 2;
	leftCenter.y = center.y;
	rightCenter.x = center.x + m_centerButton->Width() + m_distFromCenter / 2;
	rightCenter.y = center.y;
	
	if ( m_numPreReq )
	{
		primitives_DrawLine16( surface, leftCenter.x, leftCenter.y, center.x, center.y, 0x0000 );
	}

	if ( m_numEitherPreReq )
	{
		primitives_DrawLine16( surface, leftCenter.x, leftCenter.y, center.x, center.y, g_colorSet->GetColor(COLOR_BLUE) );
	}

	if ( m_numLeadsTo )
	{
		primitives_DrawLine16( surface, center.x + m_centerButton->Width(), center.y,	rightCenter.x, rightCenter.y, 0x0000 );
	}

	for ( i = 0;i < m_numPreReq;i++ )
	{
		POINT l, r;
		l.x = rect.left + m_preReqButton[i]->X() + m_preReqButton[i]->Width();
		l.y = rect.top + m_preReqButton[i]->Y() + m_preReqButton[i]->Height() / 2;
		r.x = l.x + 5;
		r.y = l.y;
		primitives_DrawLine16( surface, l.x, l.y, r.x, r.y, 0x0000 );
		primitives_DrawLine16( surface, r.x, r.y, leftCenter.x, leftCenter.y, 0x0000 );
	}

	for ( i = 0;i < m_numEitherPreReq;i++ )
	{
		POINT l, r;
		l.x = rect.left + m_eitherPreReqButton[i]->X() + m_eitherPreReqButton[i]->Width();
		l.y = rect.top + m_eitherPreReqButton[i]->Y() + m_eitherPreReqButton[i]->Height() / 2;
		r.x = l.x + 5;
		r.y = l.y;
		primitives_DrawLine16( surface, l.x, l.y, r.x, r.y, g_colorSet->GetColor(COLOR_BLUE) );
		primitives_DrawLine16( surface, r.x, r.y, leftCenter.x, leftCenter.y, g_colorSet->GetColor(COLOR_BLUE) );
	}

	for ( i = 0;i < m_numLeadsTo;i++ )
	{
		POINT l, r;
		l.x = rect.left + m_leadsToButton[i]->X() - 5;
		l.y = rect.top + m_leadsToButton[i]->Y() + m_leadsToButton[i]->Height() / 2;
		r.x = l.x + 5;
		r.y = l.y;

		// If leadto advance has center advance as an eitherprerequisite then draw lines blue
		// (rather than black) from center to leadto advance.
		if(advanceutil_AdvanceHasEitherPrereq(GetLeadsToIndex(i), m_centerIndex))
		{
			primitives_DrawLine16( surface, l.x, l.y, r.x, r.y, g_colorSet->GetColor(COLOR_BLUE) );
			primitives_DrawLine16( surface, rightCenter.x, rightCenter.y, l.x, l.y, g_colorSet->GetColor(COLOR_BLUE) );
		}
		else
		{
			primitives_DrawLine16( surface, l.x, l.y, r.x, r.y, 0x0000 );
			primitives_DrawLine16( surface, rightCenter.x, rightCenter.y, l.x, l.y, 0x0000 );
		}
	}

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE Chart::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	ctp2_Static::DrawThis( surface, x, y );

	return AUI_ERRCODE_OK;
}

sint32 Chart::SetTipInfo( ctp2_Button *button, sint32 index )
{
	c3_HyperTipWindow *tipWindow = (c3_HyperTipWindow *)button->GetTipWindow();
	
	MBCHAR name[k_MAX_NAME_LEN];

	sprintf(name, "%s_HELP", g_theStringDB->GetIdStr(g_theAdvanceDB->Get(index)->m_name));
	
	MBCHAR str[_MAX_PATH];
	sprintf( str, "%s", g_theStringDB->GetNameStr(name) );
	tipWindow->SetHyperTipText( str );

    return(1);
}

AUI_ERRCODE Chart::Update( sint32 index )
{
	sint32 i;
	const MBCHAR *s;

	m_numPreReq = g_theAdvanceDB->Get(index)->GetNumPrerequisites();
	m_numEitherPreReq = g_theAdvanceDB->Get(index)->GetNumEitherPrerequisites();

	m_numLeadsTo = 0;
	for(i = 0; i < g_theAdvanceDB->NumRecords() && m_numLeadsTo < k_MAX_LEADS_TO; i++)
	{
		if(i == index) continue;

		// No more than 4 lead to buttons: prerequisities + eitherprerequisites.
		if(advanceutil_AdvanceHasPrereq(i, index))
			m_leadsToIndex[m_numLeadsTo++] = i;

		if(advanceutil_AdvanceHasEitherPrereq(i, index))
			m_leadsToIndex[m_numLeadsTo++] = i;
	}

	sint32 heightBetweenButtons = m_heightBetweenButtons;
	sint32 buttonHeight = m_centerButton->Height();
	sint32 buttonWidth = m_centerButton->Width();
	sint32 distanceFromCenter = m_distFromCenter;

	sint32 xpos = m_distFromSide;
	sint32 ypos = (Height() - ((m_numPreReq + m_numEitherPreReq) * buttonHeight
		+ ((m_numPreReq + m_numEitherPreReq) - 1) * heightBetweenButtons)) / 2;

	sint32 curPlayer = g_selected_item->GetVisiblePlayer();
	uint8 *adv = g_player[curPlayer]->m_advances->CanResearch();
	
	for ( i = 0; i < m_numPreReq; i++ )
	{
		m_preReqIndex[i] = g_theAdvanceDB->Get(index)->GetPrerequisitesIndex(i);
		s = g_theAdvanceDB->GetNameStr( m_preReqIndex[i] );

		m_preReqButton[i]->Move( xpos, ypos + i*heightBetweenButtons + i*buttonHeight );
		m_preReqButton[i]->SetText( s );
		m_preReqButton[i]->Show();

		aui_Ldl		*ldl = g_c3ui->GetLdl();
		if (ldl)
		{
			ldl->Remove((void *)m_preReqButton[i]);

			MBCHAR name[k_MAX_NAME_LEN];
			sprintf(name, "%s", g_theStringDB->GetIdStr(g_theAdvanceDB->Get(m_preReqIndex[i])->m_name));
			ldl->Associate((aui_Control *)m_preReqButton[i], name);
		}
		
		if ( g_player[curPlayer]->HasAdvance(m_preReqIndex[i]) )
		{
			m_preReqColor[i] = COLOR_GREEN;
		}
		else 
		{
			if ( adv[m_preReqIndex[i]] )
			{
				m_preReqColor[i] = COLOR_YELLOW;
			}
			else
			{
				m_preReqColor[i] = COLOR_RED;
			}
		}
	}

	while ( i < k_MAX_PREREQ )
	{
		m_preReqButton[i++]->Hide();
	}

	for ( i = 0; i < m_numEitherPreReq; i++ )
	{
		m_eitherPreReqIndex[i] = g_theAdvanceDB->Get(index)->GetEitherPrerequisitesIndex(i);
		s = g_theAdvanceDB->GetNameStr( m_eitherPreReqIndex[i] );

		m_eitherPreReqButton[i]->Move( xpos, ypos + i*heightBetweenButtons + i*buttonHeight );
		m_eitherPreReqButton[i]->SetText( s );
		m_eitherPreReqButton[i]->Show();

		aui_Ldl		*ldl = g_c3ui->GetLdl();
		if (ldl)
		{
			ldl->Remove((void *)m_eitherPreReqButton[i]);

			MBCHAR name[k_MAX_NAME_LEN];
			sprintf(name, "%s", g_theStringDB->GetIdStr(g_theAdvanceDB->Get(m_eitherPreReqIndex[i])->m_name));
			ldl->Associate((aui_Control *)m_eitherPreReqButton[i], name);
		}
		
		if ( g_player[curPlayer]->HasAdvance(m_eitherPreReqIndex[i]) )
		{
			m_eitherPreReqColor[i] = COLOR_GREEN;
		}
		else 
		{
			if ( adv[m_eitherPreReqIndex[i]] )
			{
				m_eitherPreReqColor[i] = COLOR_YELLOW;
			}
			else
			{
				m_eitherPreReqColor[i] = COLOR_RED;
			}
		}
	}

	while ( i < k_MAX_EITHER_PREREQ )
	{
		m_eitherPreReqButton[i++]->Hide();
	}

	xpos += buttonWidth + distanceFromCenter;
	ypos = (Height() - buttonHeight) / 2;

	m_centerIndex = index;
	s = g_theAdvanceDB->GetNameStr( m_centerIndex );

	m_centerButton->Move( xpos, ypos );
	m_centerButton->SetText( s );

	aui_Ldl		*ldl = g_c3ui->GetLdl();
	if (ldl)
	{
		ldl->Remove((void *)m_centerButton);

		MBCHAR name[k_MAX_NAME_LEN];
		sprintf(name, "%s", g_theStringDB->GetIdStr(g_theAdvanceDB->Get(m_centerIndex)->m_name));
		ldl->Associate((aui_Control *)m_centerButton, name);
	}

	if ( g_player[curPlayer]->HasAdvance(m_centerIndex) )
	{
		m_centerColor = COLOR_GREEN;
	}
	else
	{
		if ( adv[m_centerIndex] )
		{
			m_centerColor = COLOR_YELLOW;
		}
		else
		{
			m_centerColor = COLOR_RED;
		}
	}
	
	xpos += buttonWidth + distanceFromCenter;
	ypos = (Height() - (m_numLeadsTo * buttonHeight + (m_numLeadsTo - 1) * heightBetweenButtons)) / 2;

	for ( i = 0;i < m_numLeadsTo;i++ )
	{
		s = g_theAdvanceDB->GetNameStr( m_leadsToIndex[i] );

		m_leadsToButton[i]->Move( xpos, ypos + i*heightBetweenButtons + i*buttonHeight );
		m_leadsToButton[i]->SetText( s );
		m_leadsToButton[i]->Show();

		aui_Ldl		*ldl = g_c3ui->GetLdl();
		if (ldl)
		{
			ldl->Remove((void *)m_leadsToButton[i]);

			MBCHAR name[k_MAX_NAME_LEN];
			sprintf(name, "%s", g_theStringDB->GetIdStr(g_theAdvanceDB->Get(m_leadsToIndex[i])->m_name));
			ldl->Associate((aui_Control *)m_leadsToButton[i], name);
		}

		if ( g_player[curPlayer]->HasAdvance(m_leadsToIndex[i]) )
		{
			m_leadsToColor[i] = COLOR_GREEN;
		}
		else
		{
			if ( adv[m_leadsToIndex[i]] )
			{
				m_leadsToColor[i] = COLOR_YELLOW;
			}
			else
			{
				m_leadsToColor[i] = COLOR_RED;
			}
		}
	}

	while ( i < k_MAX_LEADS_TO)
	{
		m_leadsToButton[i++]->Hide();
	}

	delete adv;

	ShouldDraw();

	return AUI_ERRCODE_OK;
}

void ChartPreReqActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	Chart *chart = (Chart *)cookie;
	sint32 numPreReq = chart->GetNumPreReq();

	for ( sint32 i = 0;i < numPreReq;i++ )
	{
		if ( control->Id() == chart->GetPreReqButton(i)->Id() )
		{
			chart->Update( chart->GetPreReqIndex(i) );
		}
	}
}

void ChartEitherPreReqActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	Chart *chart = (Chart *)cookie;
	sint32 numEitherPreReq = chart->GetNumEitherPreReq();

	for ( sint32 i = 0;i < numEitherPreReq;i++ )
	{
		if ( control->Id() == chart->GetEitherPreReqButton(i)->Id() )
		{
			chart->Update( chart->GetEitherPreReqIndex(i) );
		}
	}
}

void ChartLeadsToActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	Chart *chart = (Chart *)cookie;
	sint32 numLeadsTo = chart->GetNumLeadsTo();

	for ( sint32 i = 0;i < numLeadsTo;i++ )
	{
		if ( control->Id() == chart->GetLeadsToButton(i)->Id() )
		{
			chart->Update( chart->GetLeadsToIndex(i) );
		}
	}
}

void ChartCenterActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
}

