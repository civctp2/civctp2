//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : 
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
// - Memory leaks repaired, cleanup in destructor.
//
//----------------------------------------------------------------------------

#include "c3.h"


#include "aui.h"
#include "aui_uniqueid.h"
#include "c3ui.h"
#include "aui_ldl.h"
#include "aui_UniqueId.h"
#include "aui_static.h"
#include "aui_hypertextbox.h"

#include "c3_Static.h"
#include "Thermometer.h"


#include "textbutton.h"
#include "c3_Button.h"


#include "c3textfield.h"


#include "c3listbox.h"
#include "c3_listbox.h"
#include "aui_listbox.h"


#include "c3window.h"
#include "c3windows.h"
#include "c3_popupwindow.h"
#include "c3_utilitydialogbox.h"
#include "SelItem.h"




#include "Player.h"
#include "UnitRec.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "Unit.h"
#include "UnitData.h"
#include "UnitDynArr.h"
#include "CityData.h"
#include "StrDB.h"
#include "BuildingRecord.h"
#include "WonderRecord.h"
#include "TerrainRecord.h"

#include "uiutils.h"

#include "network.h"

#include "keypress.h"

#include "hotseatlist.h"
#include "CivilisationDB.h"

#include "gameinit.h"

#include "ProfileDB.h"


#include "civilisationpool.h"

#include "CivScenarios.h"
#include "GameFile.h"

extern C3UI			*g_c3ui;
extern Player **g_player;
extern StringDB *g_theStringDB;
extern CivilisationDatabase *g_theCivilisationDB;

HotseatList *g_hotseatList = NULL;
extern ProfileDB *g_theProfileDB;

extern CivScenarios *g_civScenarios;




void HotseatListButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	



	if ((ctp2_Button*)control == g_hotseatList->m_window->Ok())
	{
		sint32 i;
		
		sint32 numHumans = 0;
		for(i = 0; i < g_theProfileDB->GetNPlayers() - 1; i++) {
			HotseatListItem *item = (HotseatListItem*)g_hotseatList->m_list->GetItemByIndex(i);
			item->Update();
			if(item->IsHuman()) {
				if(strlen(item->GetName()) < 1 ||
				   (g_startEmailGame && strlen(item->GetEmail()) < 1)) {
					return;
				}
				numHumans++;
			}
		}
		
		
		if(numHumans < 1)
			return;

		for(i = 0; i < g_theProfileDB->GetNPlayers() - 1; i++) {
			HotseatListItem *item = (HotseatListItem*)g_hotseatList->m_list->GetItemByIndex(i);
			item->Update();
			g_hotseatList->m_callback(0, i + 1, 
							  item->GetCiv(),
							  item->IsHuman(),
							  item->GetName(),
							  item->GetEmail());
		}
							  
		
		g_hotseatList->m_callback( 1, 0, 0, 0, NULL, NULL);

		
		g_hotseatList->RemoveWindow();
	}
}


HotseatList::HotseatList( HotseatListCallback *callback, MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	
	if (ldlBlock) strcpy(windowBlock,ldlBlock);	
	else strcpy(windowBlock,"HotseatListPopup");

	
	{ 
		m_window = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(m_window, errcode) );
		if ( !AUI_NEWOK(m_window, errcode) ) return;

		
		m_window->Resize(m_window->Width(),m_window->Height());
		m_window->GrabRegion()->Resize(m_window->Width(),m_window->Height());
		m_window->SetStronglyModal(TRUE);

	}

	
	m_ok = NULL;

	
	m_callback = callback;

	
	Initialize( windowBlock );
}

sint32 HotseatList::Initialize( MBCHAR *windowBlock )
{
	AUI_ERRCODE errcode;
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	





	m_window->AddOk(HotseatListButtonActionCallback);

	
	sprintf( controlBlock, "%s.%s", windowBlock, "PlayerList" );
	m_list = new c3_ListBox(&errcode, aui_UniqueId(), controlBlock, NULL, NULL);
	m_list->SetAbsorbancy(FALSE);
	Assert( AUI_NEWOK(m_list, errcode) );
	if ( !AUI_NEWOK(m_list, errcode) ) return -1;

	
	m_window->AddTitle();

	
	m_window->Title()->Move( (m_window->Width() - m_window->Title()->Width()) / 2, 17 );


	if (g_startEmailGame) {
		m_window->TitleText()->SetText(g_theStringDB->GetNameStr("ldl_str_EMAIL_LIST_TITLE"));
	} else {
		if (g_startHotseatGame) {
			m_window->TitleText()->SetText(g_theStringDB->GetNameStr("ldl_str_HOTSEAT_LIST_TITLE"));
		}
	}

	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return 0;
}


//----------------------------------------------------------------------------
//
// Name       : HotseatList::~HotseatList
//
// Description: Destructor
//
// Parameters : -
//
// Globals    : g_c3ui			: main user interface
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

HotseatList::~HotseatList()
{
	if (g_c3ui && m_window)
	{
		g_c3ui->RemoveWindow(m_window->Id());
	}

	if (m_list)
	{
		m_list->Clear();
		delete m_list;
	}

	delete m_window;
}


void HotseatList::DisplayWindow( void )
{
	AUI_ERRCODE auiErr;

	
	UpdateData();

	auiErr = g_c3ui->AddWindow(m_window);
	Assert( auiErr == AUI_ERRCODE_OK );

	keypress_RegisterHandler(this);
}

void HotseatList::RemoveWindow( void )
{
	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( m_window->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );

	keypress_RemoveHandler(this);
}

void HotseatList::kh_Close()
{
	RemoveWindow();
}

sint32 HotseatList::UpdateData( void )
{
	MBCHAR ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	AUI_ERRCODE		retval;

	
	strcpy(ldlBlock,"HotseatListItem");
	HotseatListItem *item = NULL;

	m_list->Clear();

	
	if(g_theProfileDB->GetNPlayers() > k_MAX_HOTSEAT_PLAYERS + 1)
		g_theProfileDB->SetNPlayers(k_MAX_HOTSEAT_PLAYERS + 1);

	Scenario *scen;
	ScenarioPack *pack;
	SaveInfo *info = NULL;
	if(g_civScenarios->FindScenario(g_scenarioName,
								    &pack, &scen)) {
		info = g_civScenarios->LoadSaveInfo(scen);
	}

	for ( sint32 i = 0 ; i < g_theProfileDB->GetNPlayers() - 1; i++ )
	{
		sint32 civ = i + 1;
		if(info && info->startInfoType == STARTINFOTYPE_NOLOCS) {
			civ = info->playerCivIndexList[i + 1];
		} else if(i == 0) {
			civ = g_theProfileDB->GetCivIndex();
		} else if(civ == g_theProfileDB->GetCivIndex()) {
			civ = 1;
		}
		item = new HotseatListItem(&retval, i, civ, i == 0, "", ldlBlock);
		m_list->AddItem((c3_ListItem *)item);
		m_items[i] = item;
	}
	if(info)
		delete info;

	return 0;
}

sint32 HotseatList::EnableButtons( void )
{
	return 1;
}

sint32 HotseatList::DisableButtons( void )
{
	return 1;
}

sint32 HotseatList::ChooseNextOpenCiv(HotseatListItem *curItem, sint32 curCiv)
{
	sint32	i;
	BOOL	found = FALSE, 
			used;

	sint32 realCiv = curCiv;
	curCiv = g_theCivilisationDB->m_indexToAlpha[curCiv];

	sint32	oldCiv = curCiv;
	
	sint32	numEnabledCivs = hotseatlist_NumEnabled();

	while(!found) {
		do {
			curCiv++;
			if(curCiv >= g_theCivilisationDB->m_nRec)
				curCiv = 0;
		} while(g_theCivilisationDB->m_alphaToIndex[curCiv] == 0); 

		realCiv = g_theCivilisationDB->m_alphaToIndex[curCiv];
		used = FALSE;

		
		if (!hotseatlist_CivEnabled((CIV_INDEX)realCiv)) {
			continue;
		}

		for(i = 0; i < g_theProfileDB->GetNPlayers() - 1; i++) {
			HotseatListItem *item = (HotseatListItem *)m_list->GetItemByIndex(i);
			if(item->GetCiv() == realCiv) {
				used = TRUE;

				if (numEnabledCivs == g_theProfileDB->GetNPlayers()) {
					
					
					item->SetCiv(curItem->GetCiv());
					item->Update();
					curItem->SetCiv(realCiv);
					
					found = TRUE;
					break;
				}
			}
		}

		if(!used) {
			found = TRUE;
		}
	}
	return realCiv;
}

	











HotseatListItem::HotseatListItem(AUI_ERRCODE *retval, sint32 index,
								 sint32 civ, BOOL isHuman, MBCHAR *email,
								 MBCHAR *ldlBlock)
	:
	c3_ListItem( retval, ldlBlock),
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL)
{
	m_index = index;

	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(civ, isHuman, email, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;	
}

void HotseatAIToggleCallback(aui_Control *control, uint32 action, uint32 data,
							 void *cookie)
{
	if(action != (uint32)AUI_BUTTON_ACTION_EXECUTE) return;

	HotseatListItem *item = (HotseatListItem *)cookie;
	item->SetHuman(!item->IsHuman());
}

void HotseatCivCallback(aui_Control *control, uint32 action, uint32 data,
						void *cookie)
{
	if(action != (uint32)AUI_BUTTON_ACTION_EXECUTE) return;

	HotseatListItem *item = (HotseatListItem *)cookie;
	item->ChooseCiv();
}

void HotseatEmailCallback(aui_Control *control, uint32 action, uint32 data,
						void *cookie)
{
	if(action != (uint32)AUI_TEXTFIELD_ACTION_EXECUTE) return;

	HotseatListItem *item = (HotseatListItem *)cookie;
	item->EnterEmail();
}

void HotseatNameCallback(aui_Control *control, uint32 action, uint32 data,
						void *cookie)
{
	if(action != (uint32)AUI_TEXTFIELD_ACTION_EXECUTE) return;

	HotseatListItem *item = (HotseatListItem *)cookie;
	item->Update();
}

AUI_ERRCODE HotseatListItem::InitCommonLdl(sint32 civ,
										   BOOL isHuman,
										   MBCHAR *email,
										   MBCHAR *ldlBlock)
{
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		retval;

	strncpy(m_email, email, 256);
	m_civ = civ;
	m_isHuman = isHuman;

	c3_Button       *subButton;
	C3TextField *subText;

	SetBlindness(TRUE);
	sprintf(block, "%s.%s", ldlBlock, "Civ");
	subButton = new c3_Button(&retval, aui_UniqueId(), block,
							HotseatCivCallback, this);
	AddChild(subButton);

	sprintf(block, "%s.%s", ldlBlock, "AIOrHuman");
	subButton = new c3_Button(&retval, aui_UniqueId(), block, 
							  HotseatAIToggleCallback, this);
	AddChild(subButton);

	sprintf(block, "%s.%s", ldlBlock, "Name");
	subText = new C3TextField(&retval, aui_UniqueId(), block,
							  HotseatNameCallback,  this);
	if(m_index == 0) {
		subText->SetFieldText(g_theProfileDB->GetLeaderName());
	}

	AddChild(subText);

	sprintf(block, "%s.%s", ldlBlock, "Email");
	subText = new C3TextField(&retval, aui_UniqueId(), block,
							  HotseatEmailCallback, this);
	AddChild(subText);

	Update();

	return AUI_ERRCODE_OK;
}

void HotseatListItem::Update(void)
{
	
	
	c3_Button *subButton;
	C3TextField *subText;

	
	subButton = (c3_Button *)GetChildByIndex(0);
	subButton->SetText(g_theStringDB->GetNameStr(g_theCivilisationDB->Get(m_civ)->m_singular_name));

	
	if (hotseatlist_PlayerCivsLocked()) {
		subButton->Enable(FALSE);
	} else {
		subButton->Enable(TRUE);
	}

	
	subButton = (c3_Button *)GetChildByIndex(1);
	if(m_isHuman) {
		subButton->SetText(g_theStringDB->GetNameStr("HOTSEAT_HUMAN"));

		subText = (C3TextField *)GetChildByIndex(2);
		subText->GetFieldText(m_name, 255);

		subText = (C3TextField *)GetChildByIndex(3);
		subText->GetFieldText(m_email, 255);

		if(g_startEmailGame)
			subText->Enable(TRUE);
		else
			subText->Enable(FALSE);
	} else {
		subButton->SetText(g_theStringDB->GetNameStr("HOTSEAT_AI"));
		
		subText = (C3TextField *)GetChildByIndex(2);
		subText->Enable(FALSE);

		subText = (C3TextField *)GetChildByIndex(3);
		subText->Enable(FALSE);
	}
}

sint32 HotseatListItem::Compare(c3_ListItem *item2, uint32 column)
{
	return m_index - ((HotseatListItem *)item2)->m_index;
}


void HotseatListItem::SetHuman(BOOL human)
{
	m_isHuman = human;
	C3TextField *subText;
	subText = (C3TextField *)GetChildByIndex(3);
	if(m_isHuman && g_startEmailGame) {
		subText->Enable(TRUE);
	} else {
		subText->SetFieldText("");
		subText->Enable(FALSE);
	}

	subText = (C3TextField *)GetChildByIndex(2);
	if(m_isHuman) {
		subText->Enable(TRUE);
	} else {
		subText->SetFieldText("");
		subText->Enable(FALSE);
	}

	Update();
}

void HotseatListItem::ChooseCiv()
{
	m_civ = g_hotseatList->ChooseNextOpenCiv(this, m_civ);
	Update();
}

void HotseatListItem::EnterEmail()
{
	C3TextField *subText;

	
	subText = (C3TextField *)GetChildByIndex(3);
	subText->GetFieldText(m_email, 255);

	Update();
}









CIV_INDEX	s_hotseatCivList[k_MAX_PLAYERS];
BOOL		s_legalCivList[CIV_INDEX_INVALID];
BOOL		s_playerCivsLocked;



void hotseatlist_ClearOptions(void)
{
	for(sint32 i=0; i<k_MAX_PLAYERS; i++) {
		s_hotseatCivList[i] = (CIV_INDEX)0;
	}

	for (i=0; i<CIV_INDEX_INVALID; i++) {
		s_legalCivList[i] = FALSE;
	}

	s_playerCivsLocked = FALSE;
}



void hotseatlist_SetPlayerCiv(PLAYER_INDEX index, CIV_INDEX civ)
{
}


void hotseatlist_LockCivs(void)
{
	s_playerCivsLocked = TRUE;
}


BOOL hotseatlist_PlayerCivsLocked(void)
{
	return s_playerCivsLocked;
}



void hotseatlist_EnableAllCivs(void)
{
	for (sint32 i=0; i<CIV_INDEX_INVALID; i++) {
		s_legalCivList[i] = TRUE;
	}
}


void hotseatlist_DisableAllCivs(void)
{
	for (sint32 i=0; i<CIV_INDEX_INVALID; i++) {
		s_legalCivList[i] = FALSE;
	}
}


void hotseatlist_EnableCiv(CIV_INDEX civ)
{
	s_legalCivList[civ] = TRUE;
}


void hotseatlist_DisableCiv(CIV_INDEX civ)
{
	s_legalCivList[civ] = FALSE;
}


BOOL hotseatlist_CivEnabled(CIV_INDEX civ)
{
	return s_legalCivList[civ];
}


sint32 hotseatlist_NumEnabled(void)
{
	sint32 count = 0;

	for (sint32 i=0; i<CIV_INDEX_INVALID; i++) {
		if (s_legalCivList[i])
			count++;
	}

	return count;
}
