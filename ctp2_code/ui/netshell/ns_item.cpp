


#include "c3.h"

#include "aui_uniqueid.h"
#include "aui_ldl.h"

#include "c3_button.h"

#include "allinonewindow.h"
#include "spnewgametribescreen.h"

#include "ns_tribes.h"


#include "ns_item.h"



ns_ListItem::ns_ListItem(
	AUI_ERRCODE *retval,
	const MBCHAR *name,
	MBCHAR *ldlBlock )
	:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(name, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE ns_ListItem::InitCommonLdl(
	const MBCHAR *name,
	MBCHAR *ldlBlock)
{
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		retval;

	sprintf(block, "%s.%s", ldlBlock, "Name");
	c3_Static *subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	subItem->SetText( name );

	Update();

	return AUI_ERRCODE_OK;
}



ns_HPlayerItem::ns_HPlayerItem(
	AUI_ERRCODE *retval,
	void *player,
	BOOL isAI,
	MBCHAR *ldlBlock)
	:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(player, isAI, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}

ns_HPlayerItem::~ns_HPlayerItem()
{
	
	
	
	

	if (m_civpointsButton)
		delete m_civpointsButton;

	if (m_pwpointsButton)
		delete m_pwpointsButton;

}


AUI_ERRCODE ns_HPlayerItem::InitCommonLdl(
	void *player,
	BOOL isAI,
	MBCHAR *ldlBlock)
{
	m_player = player;
	m_isAI = isAI;

	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		retval;

	







	SetBlindness( TRUE ); 
	SetImageBltFlag( AUI_IMAGEBASE_BLTFLAG_CHROMAKEY );

	sprintf(block, "%s.%s", ldlBlock, "launched");
	m_launchedItem = new c3_Static(&retval, aui_UniqueId(), block);
	m_launchedItem->SetImageBltFlag( AUI_IMAGEBASE_BLTFLAG_CHROMAKEY );
	m_launchedItem->SetBlindness( TRUE ); 
	AddChild(m_launchedItem);

	sprintf(block, "%s.%s", ldlBlock, "name");
	m_nameItem = new c3_Static(&retval, aui_UniqueId(), block);
	m_nameItem->SetBlindness( TRUE ); 
	AddChild(m_nameItem);

	sprintf(block, "%s.%s", ldlBlock, "ping");
	m_pingItem = new c3_Static(&retval, aui_UniqueId(), block);
	m_pingItem->SetBlindness( TRUE ); 
	AddChild(m_pingItem);


	
	sprintf(block, "%s.%s", ldlBlock, "tribe");
	m_tribeItem = new c3_Static(&retval, aui_UniqueId(), block);

	aui_Control::ControlActionCallback TribesButtonCallback;
	sprintf(block, "%s.%s", ldlBlock, "tribe.button");
	m_tribeButton = new c3_Button(
		&retval,
		aui_UniqueId(),
		block,
		TribesButtonCallback,
		this );


	m_tribeItem->AddChild(m_tribeButton);

	AddChild(m_tribeItem);



















	
	sprintf(block, "%s.%s", ldlBlock, "civpoints");
	m_civpointsItem = new c3_Static(&retval, aui_UniqueId(),block);

	aui_Control::ControlActionCallback CivPointsButtonCallback;
	sprintf(block, "%s.%s", ldlBlock, "civpoints.button");
	m_civpointsButton = new c3_EditButton(
		&retval,
		aui_UniqueId(),
		block,
		CivPointsButtonCallback,
		this );


	m_civpointsItem->AddChild(m_civpointsButton);

	AddChild(m_civpointsItem);

	
	sprintf(block, "%s.%s", ldlBlock, "pwpoints");
	m_pwpointsItem = new c3_Static(&retval, aui_UniqueId(),block);

	aui_Control::ControlActionCallback PwPointsButtonCallback;
	sprintf(block, "%s.%s", ldlBlock, "pwpoints.button");
	m_pwpointsButton = new c3_EditButton(
		&retval,
		aui_UniqueId(),
		block,
		PwPointsButtonCallback,
		this );


	m_pwpointsItem->AddChild(m_pwpointsButton);

	AddChild(m_pwpointsItem);


	Update();

	return AUI_ERRCODE_OK;
}



void ns_HPlayerItem::SetTribe( sint32 tribe )
{
	m_tribeButton->SetText( g_nsTribes->GetStrings()->GetString( tribe ) );

	AllinoneWindow *w = g_allinoneWindow;
	if ( !IsAI() && w->IsMine( GetPlayer() ) )
		spnewgametribescreen_setTribeIndex(
			tribe - 1,
			strlen( w->m_lname ) ? w->m_lname : NULL );
}
















void ns_HPlayerItem::SetCivpoints( sint32 civpoints )
{
	aui_Control::ControlActionCallback *actionFunc =
		m_civpointsButton->GetActionFunc();
	void *cookie = m_civpointsButton->GetCookie();

	m_civpointsButton->SetActionFuncAndCookie( NULL, NULL );
	m_civpointsButton->SetValue( civpoints );
	m_civpointsButton->SetActionFuncAndCookie( actionFunc, cookie );
}



void ns_HPlayerItem::SetPwpoints( sint32 pwpoints )
{
	aui_Control::ControlActionCallback *actionFunc =
		m_pwpointsButton->GetActionFunc();
	void *cookie = m_pwpointsButton->GetCookie();

	m_pwpointsButton->SetActionFuncAndCookie( NULL, NULL );
	m_pwpointsButton->SetValue( pwpoints );
	m_pwpointsButton->SetActionFuncAndCookie( actionFunc, cookie );
}
