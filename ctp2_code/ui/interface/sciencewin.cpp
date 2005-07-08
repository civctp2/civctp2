//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Science window
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
// - Use the same science percentage everywhere.
//
//----------------------------------------------------------------------------

#include "c3.h"


#include "aui.h"
#include "aui_directsurface.h"
#include "aui_uniqueid.h"
#include "c3ui.h"
#include "aui_ldl.h"
#include "aui_static.h"
#include "c3_static.h"
#include "tech_wllist.h"
#include "aui_stringtable.h"

#include "c3dropdown.h"
#include "aui_listbox.h"
#include "c3_listbox.h"
#include "c3dropdown.h"

#include "controlsheet.h"
#include "statictextbox.h"
#include "c3_hypertextbox.h"

#include "pixelutils.h"
#include "c3_switch.h"
#include "colorset.h"
#include "tileset.h"
#include "c3_icon.h"

#include "c3slider.h"
#include "thermometer.h"


#include "textbutton.h"
#include "textswitch.h"
#include "aui_switchgroup.h"
#include "picturebutton.h"
#include "aui_button.h"
#include "c3_button.h"
#include "ctp2_button.h"

#include "aui_listbox.h"
#include "c3_listbox.h"

#include "StrDB.h"
#include "BuildingRecord.h"
#include "WonderRecord.h"
#include "Unit.h"
#include "UnitData.h"
#include "citydata.h"
#include "Advances.h"
#include "Globals.h"
#include "AdvanceRecord.h"

#include "player.h"
#include "Readiness.h"
#include "SelItem.h"
#include "ObjPool.h"

#include "debugwindow.h"

#include "UnitData.h"
#include "player.h"
#include "PlayHap.h"
#include "SelItem.h"
#include "Sci.h"

#include "chart.h"


#include "pixelutils.h"


#include "sci_advancescreen.h"

#include "c3windows.h"
#include "sciencewindow.h"

#include "c3window.h"

#include "UIUtils.h"
#include "screenutils.h"

#include "sciencewin.h"
#include "keypress.h"

#include "AdvanceBranchRecord.h"
#include "c3math.h"		// AsPercentage


extern C3UI					*g_c3ui;
extern Player				**g_player;
extern SelectedItem			*g_selected_item; 
extern DebugWindow			*g_debugWindow;
extern StringDB				*g_theStringDB;
extern ColorSet				*g_colorSet;

extern aui_Surface			*g_sharedSurface;


ScienceWin			*g_scienceWin = NULL;

static c3_Button		*s_returnButton;


static sint32			s_scienceTax;

static c3_Static	*s_lt;
static c3_Static	*s_ct;
static c3_Static	*s_rt;
static c3_Static	*s_left;
static c3_Static	*s_right;
static c3_Static	*s_bottom;

static c3_Static	*s_listtop;
static c3_Static	*s_listbl;
static c3_Static	*s_listbc;
static c3_Static	*s_listbr;

static c3_Static	*s_titleText;



static MBCHAR			s_ldlBlocks[k_EXTRA_PLAYERS][50] = {
	"PlayerOne",
	"PlayerTwo",
	"PlayerThree",
	"PlayerFour",
	"PlayerFive",
	"PlayerSix",
	"PlayerSeven",
	"PlayerEight"
};

static MBCHAR			s_flagBlocks[k_EXTRA_PLAYERS][50] = {
	"FlagOne",
	"FlagTwo",
	"FlagThree",
	"FlagFour",
	"FlagFive",
	"FlagSix",
	"FlagSeven",
	"FlagEight"
};

static c3_HyperTextBox	*s_givesBox;

static c3_Static		*s_civBox;
static c3_Static		*s_civText;




extern sint32			g_modalWindow;

void sciencewin_ExitCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	close_ScienceStatus();
}

void ScienceWin::kh_Close()
{
	close_ScienceStatus();
}

void sciencewin_LibraryButtonCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;



	Chart *tree = (Chart *)cookie;

	open_GreatLibrary( tree->GetCenterIndex(), TRUE );
	close_ScienceStatus();
}

void sciencewin_ChangeButtonCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	sci_advancescreen_displayMyWindow(NULL, k_SCI_INCLUDE_CANCEL);
	close_ScienceStatus();
}

void sciencewin_SciButtonCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	Player *p = g_player[g_selected_item->GetVisiblePlayer()];

	if ( (c3_Button *)control == g_scienceWin->PlusButton() ) {
		
		s_scienceTax += 10;
		
		
		if ( s_scienceTax > 100 )
			s_scienceTax = 100;
	}
	else if ( (c3_Button *)control == g_scienceWin->MinusButton() ) {
		
		s_scienceTax -= 10;
		
		
		if ( s_scienceTax < 0 )
			s_scienceTax = 0;
	}

	
	double taxRate = (double)s_scienceTax / 100;
	p->SetTaxes( taxRate );

	
	g_scienceWin->UpdateData( SCI_UPDATE_NOCHART );
	
	


}


void sciencewin_PrereqActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	Chart *chart = (Chart *)cookie;
	sint32 numPreReq = chart->GetNumPreReq();

	for ( sint32 i = 0;i < numPreReq;i++ ) {
		if ( control->Id() == chart->GetPreReqButton(i)->Id() ) {

			chart->Update( chart->GetPreReqIndex(i) );
		}
	}
}

void sciencewin_LeadsToActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	Chart *chart = (Chart *)cookie;
	sint32 numLeadsTo = chart->GetNumLeadsTo();

	for ( sint32 i = 0;i < numLeadsTo;i++ ) {
		if ( control->Id() == chart->GetLeadsToButton(i)->Id() ) {

			chart->Update( chart->GetLeadsToIndex(i) );
		}
	}

}

void sciencewin_AdvanceListCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_LISTBOX_ACTION_SELECT &&
		 action != (uint32)AUI_LISTBOX_ACTION_RMOUSESELECT) return;
	if ( !g_scienceWin->AdvanceList() ) return;

	AdvanceListItem *item;
	item = (AdvanceListItem *)g_scienceWin->AdvanceList()->GetSelectedItem();

	if ( action == (uint32) AUI_LISTBOX_ACTION_RMOUSESELECT) {
		HandleGameSpecificRightClick((void *)item);
		return;
	}

	if ( item ) {



		g_scienceWin->Tree()->Update( item->GetIndex() );
	}
}

sint32 knowledgewin_Initialize( void )
{

	AUI_ERRCODE		errcode;
	MBCHAR			windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR			buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	MBCHAR			controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];




















	






















































































































































	sprintf( buttonBlock, "%s.%s", windowBlock, "GivesBox" );
	s_givesBox = new c3_HyperTextBox( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK(s_givesBox, errcode) );
	if ( !AUI_NEWOK(s_givesBox, errcode) ) return -4;

	sprintf( buttonBlock, "%s.%s", windowBlock, "CivBox" );
	s_civBox = new c3_Static( &errcode, aui_UniqueId(), buttonBlock );
	Assert( AUI_NEWOK(s_civBox, errcode) );
	if ( !AUI_NEWOK(s_civBox, errcode) ) return -5;

	sprintf( controlBlock, "%s.%s", buttonBlock, "CivText" );
	s_civText = new c3_Static( &errcode, aui_UniqueId(), controlBlock );
	Assert( AUI_NEWOK(s_civText, errcode) );
	if ( !AUI_NEWOK(s_civText, errcode) ) return -6;





	sprintf( controlBlock, "%s.%s", windowBlock, "TitleText" );
	s_titleText = new c3_Static( &errcode, aui_UniqueId(), controlBlock );
	Assert( AUI_NEWOK(s_titleText, errcode ) );
	if ( !AUI_NEWOK(s_titleText, errcode) ) return -23;

	
	knowledgewin_InitGraphicTrim( windowBlock );

	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return -1;



	
	g_modalWindow++;




	return 0;
}

sint32 knowledgewin_InitGraphicTrim( MBCHAR *windowBlock )
{
	AUI_ERRCODE errcode;
	MBCHAR		imageBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	sprintf( imageBlock, "%s.%s", windowBlock, "Lt" );
	s_lt = new c3_Static( &errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_lt, errcode) );
	if ( !AUI_NEWOK(s_lt, errcode) ) return -10;
	sprintf( imageBlock, "%s.%s", windowBlock, "Ct" );
	s_ct = new c3_Static( &errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_ct, errcode) );
	if ( !AUI_NEWOK(s_ct, errcode) ) return -10;
	sprintf( imageBlock, "%s.%s", windowBlock, "Rt" );
	s_rt = new c3_Static( &errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_rt, errcode) );
	if ( !AUI_NEWOK(s_rt, errcode) ) return -10;
	sprintf( imageBlock, "%s.%s", windowBlock, "LeftImage" );
	s_left = new c3_Static( &errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_left, errcode) );
	if ( !AUI_NEWOK(s_left, errcode) ) return -10;
	sprintf( imageBlock, "%s.%s", windowBlock, "RightImage" );
	s_right = new c3_Static( &errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_right, errcode) );
	if ( !AUI_NEWOK(s_right, errcode) ) return -10;
	sprintf( imageBlock, "%s.%s", windowBlock, "BottomImage" );
	s_bottom = new c3_Static( &errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_bottom, errcode) );
	if ( !AUI_NEWOK(s_bottom, errcode) ) return -10;

	sprintf( imageBlock, "%s.%s", windowBlock, "ListTop" );
	s_listtop = new c3_Static( &errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_listtop, errcode) );
	if ( !AUI_NEWOK(s_listtop, errcode) ) return -10;
	sprintf( imageBlock, "%s.%s", windowBlock, "ListBl" );
	s_listbl = new c3_Static( &errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_listbl, errcode) );
	if ( !AUI_NEWOK(s_listbl, errcode) ) return -10;
	sprintf( imageBlock, "%s.%s", windowBlock, "ListBc" );
	s_listbc = new c3_Static( &errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_listbc, errcode) );
	if ( !AUI_NEWOK(s_listbc, errcode) ) return -10;
	sprintf( imageBlock, "%s.%s", windowBlock, "ListBr" );
	s_listbr = new c3_Static( &errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_listbr, errcode) );
	if ( !AUI_NEWOK(s_listbr, errcode) ) return -10;

















































	return 0;
}

sint32 knowledgewin_Cleanup( void )
{




	delete s_returnButton;
	s_returnButton = NULL;








	
	
	delete s_givesBox;
	s_givesBox = NULL;
	delete s_civBox;
	s_civBox = NULL;

	delete s_civText;
	s_civText = NULL;

	delete s_titleText;
	s_titleText = NULL;

	
	delete s_lt;
	s_lt = NULL;
	delete s_ct;
	s_ct = NULL;
	delete s_rt;
	s_rt = NULL;
	delete s_left;
	s_left = NULL;
	delete s_right;
	s_right = NULL;
	delete s_bottom;
	s_bottom = NULL;

	delete s_listtop;
	s_listtop = NULL;
	delete s_listbl;
	s_listbl = NULL;
	delete s_listbc;
	s_listbc = NULL;
	delete s_listbr;
	s_listbr = NULL;






	return 0;
}


KnowledgeListItem::KnowledgeListItem(AUI_ERRCODE *retval, sint32 index, MBCHAR *ldlBlock)
	:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(index, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;	
}


AUI_ERRCODE KnowledgeListItem::InitCommonLdl(sint32 index, MBCHAR *ldlBlock)
{
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		retval;

	m_index = index;

	c3_Static		*subItem;

	sprintf(block, "%s.%s", ldlBlock, "Name");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	Update();

	return AUI_ERRCODE_OK;
}

void KnowledgeListItem::Update(void)
{


	c3_Static *subItem;

	
	subItem = (c3_Static *)GetChildByIndex(0);
	subItem->SetText( g_theAdvanceDB->GetNameStr(m_index) );
}

sint32 KnowledgeListItem::Compare(c3_ListItem *item2, uint32 column)
{
	sint32	val1, val2;

	if (column < 0) return 0;

	switch (column) {
	case 0:
		val1 = m_index; 
		val2 = ((KnowledgeListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	}

	return 0;
}


EmbassyListItem::EmbassyListItem(AUI_ERRCODE *retval, sint32 index, MBCHAR *ldlBlock)
	:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(index, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;	
}


AUI_ERRCODE EmbassyListItem::InitCommonLdl(sint32 index, MBCHAR *ldlBlock)
{
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		retval;

	m_index = index;

	c3_Static		*subItem;

	sprintf(block, "%s.%s", ldlBlock, "Name");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	Update();

	return AUI_ERRCODE_OK;
}

void EmbassyListItem::Update(void)
{
	MBCHAR			name[_MAX_PATH];

	c3_Static *subItem;

	g_player[m_index]->GetPluralCivName( name );

	
	subItem = (c3_Static *)GetChildByIndex(0);
	subItem->SetText( name );
}

sint32 EmbassyListItem::Compare(c3_ListItem *item2, uint32 column)
{
	sint32	val1, val2;

	if (column < 0) return 0;

	switch (column) {
	case 0:
		val1 = m_index; 
		val2 = ((EmbassyListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	}

	return 0;
}


AdvanceListItem::AdvanceListItem(AUI_ERRCODE *retval, sint32 index, MBCHAR *ldlBlock)
	:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(index, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;	
}

AdvanceListItem::~AdvanceListItem()
{
	ListPos position = m_childList->GetHeadPosition();

	for ( sint32 i = m_childList->L(); i; i-- ) {
		aui_Region		*subControl;
		
		subControl = m_childList->GetNext( position );
		if (subControl) {
			ListPos	subPos = subControl->ChildList()->GetHeadPosition();

			for (sint32 j = subControl->ChildList()->L(); j; j--) {
				delete subControl->ChildList()->GetNext(subPos);
			}
		}
		delete subControl;
	}

	m_childList->DeleteAll();
}

AUI_ERRCODE AdvanceListItem::InitCommonLdl(sint32 index, MBCHAR *ldlBlock)
{
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR			subBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		retval;

	m_index = index;
	m_branchVal = g_theAdvanceBranchDB->Get( g_theAdvanceDB->Get(m_index)->GetBranchIndex() )->GetValue();

	c3_Static		*subItem;
	c3_Static		*branchItem;

	sprintf(block, "%s.%s", ldlBlock, "PlayerFlag");
	subItem = new c3_Icon(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "SymTwo");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

		sprintf( subBlock, "%s.%s", block, "Branch" );
		branchItem = new c3_Static( &retval, aui_UniqueId(), subBlock );
		subItem->AddChild( branchItem );

	sprintf(block, "%s.%s", ldlBlock, "Name");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "FlagOne");
	subItem = new c3_Icon(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "FlagTwo");
	subItem = new c3_Icon(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "FlagThree");
	subItem = new c3_Icon(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "FlagFour");
	subItem = new c3_Icon(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "FlagFive");
	subItem = new c3_Icon(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "FlagSix");
	subItem = new c3_Icon(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf(block, "%s.%s", ldlBlock, "FlagSeven");
	subItem = new c3_Icon(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	
	
	
	
	aui_Ldl		*ldl = g_c3ui->GetLdl();
	if (ldl) {
		ldl->Remove((void *)this);

		
		MBCHAR name[k_MAX_NAME_LEN];

		sprintf(name, "%s", g_theStringDB->GetIdStr(g_theAdvanceDB->Get(index)->m_name));

		ldl->Associate((aui_Control *)this, name);
	}

	Update();

	return AUI_ERRCODE_OK;
}

void AdvanceListItem::Update(void)
{
	c3_Static *subItem;
	c3_Icon	*subIcon;
	c3_Static *branchItem;

	sint32 curPlayer = g_selected_item->GetVisiblePlayer();

	
	subIcon = (c3_Icon*)GetChildByIndex(0);
	if ( g_player[curPlayer] && g_player[curPlayer]->HasAdvance(m_index) ) {
		subIcon->SetColor( g_colorSet->ComputePlayerColor(curPlayer) );
		subIcon->SetMapIcon( MAPICON_FLAG );
	}
	else {
		subIcon->SetMapIcon( MAPICON_MAX );
	}

	
	subItem = (c3_Static *)GetChildByIndex(1);
	branchItem = (c3_Static *)subItem->GetChildByIndex(0);
	if ( g_scienceWin ) {
		branchItem->SetImage( g_scienceWin->GetString(g_theAdvanceDB->Get(m_index)->GetBranchIndex()) );
	}

	
	subItem = (c3_Static *)GetChildByIndex(2);
	subItem->SetText( g_theAdvanceDB->GetNameStr(m_index) );

	sint32 i,x = 0;

	
	for ( i = 0;i < k_MAX_PLAYERS;i++ ) {
		if ( i != curPlayer ) {
			
			if ( g_player[i] && g_player[curPlayer]->HasEmbassyWith(i) ) {
				subIcon = (c3_Icon *)GetChildByIndex(x+++3);

				
				if (subIcon) {
					
					if ( g_player[i]->HasAdvance(m_index) ) {
						subIcon->SetColor( g_colorSet->ComputePlayerColor(i) );
						subIcon->SetMapIcon( MAPICON_FLAG );
					}
					
					else {
						subIcon->SetMapIcon( MAPICON_MAX );
					}
				}
			}
		}
	}

	
	while ( x < k_PLAYERS ) {
		subIcon = (c3_Icon *)GetChildByIndex(x+++3);
		
		
		if (subIcon) {
			subIcon->SetMapIcon( MAPICON_MAX );
		}
	}
}

sint32 AdvanceListItem::Compare(c3_ListItem *item2, uint32 column)
{
	c3_Static		*i1, *i2;
	sint32	val1, val2;

	if (column < 0) return 0;

	switch (column) {
	case 0:
		val1 = m_index; 
		val2 = ((AdvanceListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	case 1:
		val1 = m_branchVal; 
		val2 = ((AdvanceListItem *)item2)->GetBranchVal();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	case 2:
		i1 = (c3_Static *)this->GetChildByIndex(column);
		i2 = (c3_Static *)item2->GetChildByIndex(column);

		return strcmp(i1->GetText(), i2->GetText());

		break;
	case 3:
		val1 = m_index; 
		val2 = ((AdvanceListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	case 4:
		val1 = m_index; 
		val2 = ((AdvanceListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	case 5:
		val1 = m_index; 
		val2 = ((AdvanceListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	case 6:
		val1 = m_index; 
		val2 = ((AdvanceListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	case 7:
		val1 = m_index; 
		val2 = ((AdvanceListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	case 8:
		val1 = m_index; 
		val2 = ((AdvanceListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	case 9:
		val1 = m_index; 
		val2 = ((AdvanceListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	case 10:
		val1 = m_index; 
		val2 = ((AdvanceListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	}

	return 0;
}

sint32 sciencewin_Initialize( void )
{
	if ( g_scienceWin ) {
		g_scienceWin->UpdateData( SCI_UPDATE_ALL );
		g_scienceWin->m_window->MoveOG();
		return 0;	
	}

	g_scienceWin = new ScienceWin();
	g_scienceWin->UpdateData( SCI_UPDATE_ALL );

	return 0;
}

sint32 sciencewin_Cleanup( void )
{
	if ( g_scienceWin ) delete g_scienceWin;
	g_scienceWin = NULL;

	return 0;
}

ScienceWin::ScienceWin( void )
{
	AUI_ERRCODE errcode;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	strcpy(windowBlock,"ScienceWin");

	m_window = new C3Window( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false );
	Assert( AUI_NEWOK(m_window, errcode) );
	if ( !AUI_NEWOK(m_window, errcode) ) return;

	m_window->SetSurface( g_sharedSurface );

	
	m_window->Resize(m_window->Width(),m_window->Height());
	m_window->SetStronglyModal(TRUE);


	m_window->GrabRegion()->Resize( m_window->Width(), 20 );
	m_window->SetDraggable( TRUE );

	
	Initialize( windowBlock );
}

sint32 ScienceWin::Initialize( MBCHAR *windowBlock )
{
	AUI_ERRCODE errcode;
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	sprintf( controlBlock, "%s.%s", windowBlock, "CloseButton" );
	m_closeButton = new c3_Button( &errcode, aui_UniqueId(), controlBlock, sciencewin_ExitCallback );
	TestControl( m_closeButton );
	
	sprintf( controlBlock, "%s.%s", windowBlock, "Title" );
	m_title = new c3_Static( &errcode, aui_UniqueId(), controlBlock );
	TestControl( m_title );
	m_title->SetBlindness( TRUE );

	sprintf( controlBlock, "%s.%s", windowBlock, "AdvanceList" );
	m_advanceList = new c3_ListBox( &errcode, aui_UniqueId(), controlBlock, sciencewin_AdvanceListCallback );
	TestControl( m_advanceList );

	
	sprintf( buttonBlock, "%s.%s", windowBlock, "ChangeButton" );
	m_changeButton = new c3_Button( &errcode, aui_UniqueId(), buttonBlock, sciencewin_ChangeButtonCallback );
	TestControl( m_changeButton );

	
	sprintf( controlBlock, "%s.%s", windowBlock, "ResearchBox" );
	m_researchBox = new c3_Static( &errcode, aui_UniqueId(), controlBlock );
	TestControl( m_researchBox );

	
	sprintf( controlBlock, "%s.%s", windowBlock, "ResearchMeter" );
	m_researchMeter = new Thermometer( &errcode, aui_UniqueId(), controlBlock );
	TestControl( m_researchMeter );

	
	sprintf( controlBlock, "%s.%s", windowBlock, "ResearchClock" );
	m_researchClock = new c3_Static( &errcode, aui_UniqueId(), controlBlock );
	TestControl( m_researchClock );

	
	sprintf( controlBlock, "%s.%s", windowBlock, "TurnsBox" );
	m_turnsBox = new c3_Static( &errcode, aui_UniqueId(), controlBlock );
	TestControl( m_turnsBox );

	
	sprintf( controlBlock, "%s.%s", windowBlock, "CostLabel" );
	m_costLabel = new c3_Static( &errcode, aui_UniqueId(), controlBlock );
	TestControl( m_costLabel );

	
	sprintf( controlBlock, "%s.%s", windowBlock, "CostBox" );
	m_costBox = new c3_Static( &errcode, aui_UniqueId(), controlBlock );
	TestControl( m_costBox );

	
	sprintf( buttonBlock, "%s.%s", windowBlock, "PlusButton" );
	m_plusButton = new c3_Button( &errcode, aui_UniqueId(), buttonBlock, sciencewin_SciButtonCallback );
	TestControl( m_plusButton );

	
	sprintf( buttonBlock, "%s.%s", windowBlock, "MinusButton" );
	m_minusButton = new c3_Button( &errcode, aui_UniqueId(), buttonBlock, sciencewin_SciButtonCallback );
	TestControl( m_minusButton );

	
	sprintf( controlBlock, "%s.%s", windowBlock, "PercentBox" );
	m_percentBox = new c3_Static( &errcode, aui_UniqueId(), controlBlock );
	TestControl( m_percentBox );

	
	sprintf( controlBlock, "%s.%s", windowBlock, "SciLabel" );
	m_sciLabel = new c3_Static( &errcode, aui_UniqueId(), controlBlock );
	TestControl( m_sciLabel );

	
	sprintf( controlBlock, "%s.%s", windowBlock, "SciBeaker" );
	m_sciBeaker = new c3_Static( &errcode, aui_UniqueId(), controlBlock );
	TestControl( m_sciBeaker );

	
	sprintf( controlBlock, "%s.%s", windowBlock, "SciBox" );
	m_sciBox = new c3_Static( &errcode, aui_UniqueId(), controlBlock );
	TestControl( m_sciBox );

	
	sprintf( controlBlock, "%s.%s", windowBlock, "Chart" );
	m_tree = new Chart( &errcode, aui_UniqueId(), controlBlock );
	TestControl( m_tree );
	m_tree->Update(0);

	
	sint32 i;
	for ( i = 0;i < 4 ;i++ ) {
		m_tree->GetPreReqButton(i)->SetActionFuncAndCookie( sciencewin_PrereqActionCallback, m_tree );
	}

	for ( i = 0;i < 4 ;i++ ) {
		m_tree->GetLeadsToButton(i)->SetActionFuncAndCookie( sciencewin_LeadsToActionCallback, m_tree );
	}

	
	sprintf( buttonBlock, "%s.%s", windowBlock, "LibraryButton" );
	m_libraryButton = new c3_Button( &errcode, aui_UniqueId(), buttonBlock, sciencewin_LibraryButtonCallback, m_tree );
	TestControl( m_libraryButton );

	
	sprintf( controlBlock, "%s.%s", windowBlock, "KnownToLabel" );
	m_knownToLabel = new c3_Static( &errcode, aui_UniqueId(), controlBlock );
	TestControl( m_knownToLabel );

	
	m_playerLabel = new c3_Static*[ k_EXTRA_PLAYERS ];
	m_playerFlag = new c3_Icon*[ k_EXTRA_PLAYERS];

	for ( i = 0;i < k_EXTRA_PLAYERS;i++ ) {
		sprintf( controlBlock, "%s.%s", windowBlock, s_ldlBlocks[i] );
		m_playerLabel[i] = new c3_Static( &errcode, aui_UniqueId(), controlBlock );
		TestControl( m_playerLabel );

		sprintf( controlBlock, "%s.%s", windowBlock, s_flagBlocks[i] );
		m_playerFlag[i] = new c3_Icon( &errcode, aui_UniqueId(), controlBlock );
		TestControl( m_playerFlag );
		m_playerFlag[i]->SetMapIcon( MAPICON_MAX );
	}

	
	m_string = new aui_StringTable( &errcode, "ScienceStrings" );
	TestControl( m_string );

	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return 0;
}

ScienceWin::~ScienceWin( void )
{
	sint32 i;


	AUI_ERRCODE errcode;
	errcode = g_c3ui->RemoveWindow( m_window->Id() );
	Assert( errcode == AUI_ERRCODE_OK );

	RemoveControl( m_closeButton );

	RemoveControl( m_title );

	RemoveControl( m_advanceList );

	RemoveControl( m_changeButton );
	RemoveControl( m_researchBox );
	RemoveControl( m_researchMeter );
	RemoveControl( m_researchClock );
	RemoveControl( m_turnsBox );
	RemoveControl( m_costLabel );
	RemoveControl( m_costBox );
	RemoveControl( m_plusButton );
	RemoveControl( m_minusButton );
	RemoveControl( m_percentBox );
	RemoveControl( m_sciLabel );
	RemoveControl( m_sciBeaker );
	RemoveControl( m_sciBox );
	RemoveControl( m_libraryButton );
	RemoveControl( m_tree );
	RemoveControl( m_knownToLabel );

	for ( i= 0;i < k_EXTRA_PLAYERS;i++ ) {
		RemoveControl( m_playerLabel[i] );
	}
	RemoveControl( m_playerLabel );

	for ( i = 0;i < k_EXTRA_PLAYERS;i++ ) {
		RemoveControl( m_playerFlag[i] );
	}
	RemoveControl( m_playerFlag );

	RemoveControl( m_string );

	
	
	if (m_window)
		m_window->SetSurface(NULL);

	RemoveControl( m_window );
}

void ScienceWin::Display( void )
{
	AUI_ERRCODE errcode;

	errcode = g_c3ui->AddWindow( m_window );
	Assert( errcode == AUI_ERRCODE_OK );

	keypress_RegisterHandler(this);

	g_modalWindow++;
}

void ScienceWin::Remove( void )
{
	AUI_ERRCODE errcode;

	errcode = g_c3ui->RemoveWindow( m_window->Id() );
	Assert( errcode == AUI_ERRCODE_OK );

	keypress_RemoveHandler(this);

	g_modalWindow--;
}

sint32 ScienceWin::UpdateData( SCI_UPDATE update )
{
	MBCHAR str[_MAX_PATH];

	sint32 curPlayer = g_selected_item->GetVisiblePlayer();
	Player *p = g_player[g_selected_item->GetVisiblePlayer()];

	
	sint32 researching = g_player[curPlayer]->m_advances->GetResearching();
	
	BOOL alreadyHas = p->HasAdvance(p->m_advances->GetResearching());

	if(alreadyHas)
	{
		str[0]=0;
	}
	else
	{
		sprintf( str, "%s", g_theAdvanceDB->GetNameStr(researching) );
	}
	m_researchBox->SetText( str );

	
	sint32 sciLevel = p->GetCurrentScienceLevel();
	sint32 advanceCost = p->GetCurrentScienceCost();

	
	if(alreadyHas)
	{
		m_researchMeter->SetPercentFilled( 0 );
	}
	else
	{
		m_researchMeter->SetPercentFilled( (sciLevel * 100) / advanceCost );
	}

	
	if(alreadyHas)
	{
		str[0]=0;
	}
	else
	{
		sprintf( str, "%d/%d", sciLevel, advanceCost );
	}
	m_costBox->SetText( str );

	
	sciLevel = p->m_advances->GetProjectedScience();
	if ( sciLevel < 0 ) {
		sciLevel = 0;
	}
	sprintf( str, "%d", sciLevel );
	m_sciBox->SetText( str );
		
	double scienceTax;
	
	p->GetScienceTaxRate( scienceTax );

	s_scienceTax = AsPercentage(scienceTax);

	sprintf(str,"%d%%",s_scienceTax);
	m_percentBox->SetText(str);

	sint32 advanceTurns = p->m_advances->TurnsToNextAdvance();

	
	if ( advanceTurns == -1 ) {
		sprintf( str, "-" );
	}
	else if ( advanceTurns < 0 ) {
		sprintf( str, "1" );
	}
	else {
		sprintf( str, "%d", advanceTurns + 1 );
	}

	m_turnsBox->SetText( str );

	if ( update == SCI_UPDATE_NOCHART ) return 0;	

	m_tree->Update(researching );


	if ( update == SCI_UPDATE_NOLIST ) return 0;	
	
	UpdateList();

	
	g_player[curPlayer]->GetPluralCivName( str );
	m_playerLabel[0]->SetText( str );
	m_playerLabel[0]->Show();
	m_playerFlag[0]->SetMapIcon( MAPICON_FLAG );
	m_playerFlag[0]->SetColor( g_colorSet->ComputePlayerColor(curPlayer) );

	sint32 x = 1;
	for ( sint32 i = 0;i < k_MAX_PLAYERS;i++ ) {
		if ( i != curPlayer ) {
			if ( g_player[i] && g_player[curPlayer]->HasEmbassyWith(i) && x < k_EXTRA_PLAYERS) {
				g_player[i]->GetPluralCivName( str );
				m_playerLabel[x]->SetText( str );
				m_playerLabel[x]->Show();
				m_playerFlag[x]->SetMapIcon( MAPICON_FLAG );
				m_playerFlag[x]->SetColor( g_colorSet->ComputePlayerColor(i) );
				x++;
			}
		}
	}

	while ( x < k_PLAYERS ) {
		m_playerLabel[x]->Hide();
		m_playerLabel[x]->SetText( "" );
		m_playerFlag[x]->SetMapIcon( MAPICON_MAX );
		x++;
	}

	return 0;
}

sint32 ScienceWin::UpdateList( void )
{
	AUI_ERRCODE errcode;
	MBCHAR		ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	sint32		curPlayer = g_selected_item->GetVisiblePlayer();
	sint32		num = g_theAdvanceDB->NumRecords();

	
	m_advanceList->Clear();

	sprintf( ldlBlock, "AdvanceListItem" );

	for ( sint32 i = 0;i < num;i++ ) {

		if ( g_player[curPlayer]->HasAdvance(i) ) {
			AdvanceListItem *item = new AdvanceListItem( &errcode, i, ldlBlock );
			m_advanceList->AddItem( (c3_ListItem *)item );	
		}
		else {
			for ( sint32 j = 0;j < k_MAX_PLAYERS;j++ ) {
				if ( j != curPlayer ) {
					if ( g_player[j] && g_player[curPlayer]->HasEmbassyWith(j) && g_player[j]->HasAdvance(i) ) {
						AdvanceListItem *item = new AdvanceListItem( &errcode, i, ldlBlock );
						m_advanceList->AddItem( (c3_ListItem *)item );	
						break;
					}
				}
			}
		}
	}

	return 0;
}


void SW_UpdateAction::Execute(aui_Control *control, uint32 action, uint32 data)
{
	if ( g_scienceWin ) {
		if ( g_c3ui->GetWindow(g_scienceWin->m_window->Id()) ) {
			if ( m_all ) {
				g_scienceWin->UpdateData( SCI_UPDATE_ALL );
			}
			else {
				g_scienceWin->UpdateData( SCI_UPDATE_NOCHART );
			}
		}
	}
}

