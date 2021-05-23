//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Window to select next advance to research
// Id           : $Id$
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Start the great library with the current research project of the player.
// - Start the "change to"-list with the current research selected.
// - Initialized local variables. (Sep 9th 2005 Martin G�hmann)
// - Fixed memory leaks.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "sci_advancescreen.h"

#include "aui_uniqueid.h"
#include "c3window.h"
#include "ctp2_button.h"
#include "c3_listitem.h"
#include "ctp2_Static.h"
#include "c3ui.h"
#include "ctp2_listbox.h"
#include "ctp2_hypertextbox.h"
#include "aui_stringtable.h"

#include "Advances.h"
#include "AdvanceRecord.h"
#include "StrDB.h"
#include "SelItem.h"
#include "player.h"
#include "Sci.h"
#include "Gold.h"
#include "prjfile.h"

#include "greatlibrary.h"

#include "screenutils.h"

#include "spnewgamewindow.h"
#include "sciencewin.h"

#include "keypress.h"
#include "keyboardhandler.h"
#include "IconRecord.h"

#include "UnitRecord.h"
#include "BuildingRecord.h"
#include "WonderRecord.h"
#include "TerrainImprovementRecord.h"

#include "terrainutil.h"

#include "colorset.h"
#include "GovernmentRecord.h"
#include "buildingutil.h"
#include "wonderutil.h"

#include "director.h"
extern Director		*g_director;

extern C3UI			*g_c3ui;
extern ProjectFile	*g_GreatLibPF;
extern GreatLibrary	*g_greatLibrary;

extern ScienceWin	*g_scienceWin;

#include "ScienceManagementDialog.h"

#include "soundmanager.h"
#include "gamesounds.h"
extern SoundManager	*g_soundManager;

static C3Window		*s_sci_advanceScreen	= NULL;

static ctp2_Button	*s_back				= NULL;
static ctp2_Button	*s_cancel			= NULL;
static ctp2_Button	*s_goal				= NULL;
static ctp2_Static	*s_name				= NULL;
static ctp2_ListBox	*s_advanceList		= NULL;

static ctp2_Static	*s_changeLabel		= NULL;
static ctp2_Static	*s_changeBox		= NULL;
static ctp2_Static	*s_turnsLabel		= NULL;
static ctp2_Static	*s_turnsBox			= NULL;

static ctp2_Static	*s_goaltext			= NULL;

static ctp2_Static	*s_background		= NULL;

static ctp2_HyperTextBox	*s_glStats		= NULL;
static ctp2_HyperTextBox	*s_message		= NULL;

static aui_StringTable	*s_advanceString = NULL;

static sint32 s_oldResearching = -1;

static bool *s_scienceGoalTree=NULL;

sint32 ScienceSortCallback(ctp2_ListItem *item1, ctp2_ListItem *item2, sint32 column);

void sci_advancescreen_fillGoalArray(sint32 goal);

class SciAdvanceScreenKeyboardHandler : public KeyboardHandler
{
public:
	void kh_Close() {
		sci_advancescreen_removeMyWindow(AUI_BUTTON_ACTION_EXECUTE);
	}
};

SciAdvanceScreenKeyboardHandler s_keyboardHandler;

void sci_advancescreen_StatsCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)CTP2_HYPERLINK_ACTION_EXECUTE ) return;

	ctp2_HyperLink *hl = ((ctp2_HyperTextBox *)control)->GetSelectedHyperLink();

	if ( hl ) {
		open_GreatLibrary();
		g_greatLibrary->SetLibrary( hl->m_index, (DATABASE)hl->m_db );
	}
}

void sci_advancescreen_GoalCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	open_GreatLibrary();
}

void sci_advancescreen_listAction( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	switch ( action )
	{
	case AUI_LISTBOX_ACTION_SELECT:

		ctp2_ListBox *lb;
		ctp2_ListItem *item;

		lb = (ctp2_ListBox *)control;
		item = (ctp2_ListItem *)lb->GetSelectedItem();
		if(!item) return;

		sci_advancescreen_updateData( NULL, FALSE );
		break;

	case AUI_LISTBOX_ACTION_DOUBLECLICKSELECT:
		if(sci_advancescreen_removeMyWindow(action))
		{
			ctp2_ListBox *lb = (ctp2_ListBox *)control;

			ctp2_ListItem *item = (ctp2_ListItem *)lb->GetSelectedItem();
			if(!item) return;

			g_player[g_selected_item->GetVisiblePlayer()]->SetResearching( (long)item->GetUserData() );
			if(g_scienceManagementDialog)
			{
				g_scienceManagementDialog->Update();
			}

			if ( g_scienceWin )
				g_scienceWin->UpdateData( SCI_UPDATE_NOLIST );
		}
		break;
	}
}

sint32	sci_advancescreen_displayMyWindow( MBCHAR *messageText, sint32 from)
{
	Player *p = g_player[ g_selected_item->GetVisiblePlayer() ];
	s_oldResearching = p->m_advances->GetResearching();

	sint32 retval=0;
	if(!s_sci_advanceScreen) { retval = sci_advancescreen_Initialize( messageText ); }
	else
	{
		sci_advancescreen_loadList();
		sci_advancescreen_updateData( messageText );
	}

	AUI_ERRCODE auiErr;

	if ( from == k_SCI_INCLUDE_CANCEL )
	{
		s_cancel->Show();
	}
	else
	{
		s_cancel->Hide();
		sint32 soundID = gamesounds_GetGameSoundID(GAMESOUNDS_CHEER_CASTLE);
		g_soundManager->AddSound(SOUNDTYPE_SFX, 0, soundID);
	}

	if(from != k_SCI_INCLUDE_CANCEL)
	{
			g_soundManager->AddGameSound(GAMESOUNDS_ADVANCE);
	}
	auiErr = g_c3ui->AddWindow(s_sci_advanceScreen);
	Assert( auiErr == AUI_ERRCODE_OK );

	if (from == k_SCI_INCLUDE_CANCEL)
	{
		keypress_RegisterHandler(&s_keyboardHandler);
	}

	return retval;
}

sint32 sci_advancescreen_removeMyWindow(uint32 action)
{
	if ( (action == (uint32)AUI_BUTTON_ACTION_EXECUTE ) || (action == (uint32)AUI_LISTBOX_ACTION_DOUBLECLICKSELECT) ) {
		AUI_ERRCODE auiErr;

		auiErr = g_c3ui->RemoveWindow( s_sci_advanceScreen->Id() );
		keypress_RemoveHandler(&s_keyboardHandler);

		Assert( auiErr == AUI_ERRCODE_OK );

		g_director->ExternalActionFinished(DEA_ADVANCE_WINDOW);
	}

	return 1;
}

AUI_ERRCODE sci_advancescreen_Initialize( MBCHAR *messageText )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( s_sci_advanceScreen ) {
		s_sci_advanceScreen->MoveOG();
		return AUI_ERRCODE_OK;
	}

	strcpy(windowBlock, "SciAdvanceScreen");

	{
		s_sci_advanceScreen = new C3Window( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_STANDARD, false);
		Assert( AUI_NEWOK(s_sci_advanceScreen, errcode) );
		if ( !AUI_NEWOK(s_sci_advanceScreen, errcode) ) errcode;

		s_sci_advanceScreen->Resize(s_sci_advanceScreen->Width(),s_sci_advanceScreen->Height());
		s_sci_advanceScreen->GrabRegion()->Resize(s_sci_advanceScreen->Width(),20);
		s_sci_advanceScreen->SetDraggable( TRUE );
		s_sci_advanceScreen->SetStronglyModal(TRUE);
	}

	s_background = new ctp2_Static( &errcode, aui_UniqueId(),
		"SciAdvanceScreen.Background" );
	s_background->Enable(false);

	s_back = new ctp2_Button(&errcode, aui_UniqueId(),
		"SciAdvanceScreen.Background.BackButton", sci_advancescreen_backPress);
	s_cancel = new ctp2_Button(&errcode, aui_UniqueId(),
		"SciAdvanceScreen.Background.CancelButton", sci_advancescreen_cancelPress);
	s_goal = new ctp2_Button(&errcode, aui_UniqueId(),
		"SciAdvanceScreen.Background.GoalButton");

	if(s_goal)
		s_goal->SetActionFuncAndCookie(sci_advancescreen_GoalCallback, NULL);

	s_name = new ctp2_Static(&errcode, aui_UniqueId(), "SciAdvanceScreen.Background.Name");

	sprintf( controlBlock, "%s.%s", windowBlock, "Background.SciAdvanceList" );
	s_advanceList = new ctp2_ListBox( &errcode, aui_UniqueId(), controlBlock,
		sci_advancescreen_listAction );
	Assert( AUI_NEWOK(s_advanceList, errcode) );
	if ( !AUI_NEWOK(s_advanceList, errcode) ) return errcode;
	s_advanceList->DoneInstantiating();

	s_advanceList->SetForceSelect( TRUE );

	s_changeLabel = new ctp2_Static( &errcode, aui_UniqueId(),
		"SciAdvanceScreen.Background.ChangeLabel" );
	s_changeBox = new ctp2_Static( &errcode, aui_UniqueId(),
		"SciAdvanceScreen.Background.ChangeBox" );
	s_turnsLabel = new ctp2_Static( &errcode, aui_UniqueId(),
		"SciAdvanceScreen.Background.TurnsLabel" );
	s_turnsBox = new ctp2_Static( &errcode, aui_UniqueId(),
		"SciAdvanceScreen.Background.TurnsBox" );
	s_goaltext = new ctp2_Static( &errcode, aui_UniqueId(),
		"SciAdvanceScreen.Background.ResearchGoal" );

	sprintf( controlBlock, "%s.%s", windowBlock, "Background.GlStats" );
	s_glStats = new ctp2_HyperTextBox( &errcode, aui_UniqueId(), controlBlock,
		sci_advancescreen_StatsCallback );
	Assert( AUI_NEWOK(s_glStats, errcode) );
	if ( !AUI_NEWOK(s_glStats, errcode) ) return errcode;

	sprintf( controlBlock, "%s.%s", windowBlock, "Background.Message" );
	s_message = new ctp2_HyperTextBox( &errcode, aui_UniqueId(), controlBlock,
		sci_advancescreen_StatsCallback);

	Assert( AUI_NEWOK(s_message, errcode) );
	if ( !AUI_NEWOK(s_message, errcode) ) return errcode;

	sprintf( controlBlock, "SciAdvanceString" );
	s_advanceString = new aui_StringTable( &errcode, controlBlock );
	if ( !AUI_NEWOK(s_advanceString, errcode) ) return errcode;

	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	sci_advancescreen_loadList();
	sci_advancescreen_updateData( messageText );

	return AUI_ERRCODE_OK;
}

void sci_advancescreen_Cleanup(void)
{
	if (s_sci_advanceScreen && g_c3ui)
	{
		g_c3ui->RemoveWindow(s_sci_advanceScreen->Id());
	}

	keypress_RemoveHandler(&s_keyboardHandler);

#define mycleanup(mypointer)    { delete mypointer; mypointer = NULL; }
	mycleanup(s_name);
	mycleanup(s_back);
	mycleanup(s_cancel);
	mycleanup(s_goal);
	mycleanup(s_advanceList);
	mycleanup(s_changeLabel);
	mycleanup(s_changeBox);
	mycleanup(s_turnsLabel);
	mycleanup(s_turnsBox);
	mycleanup(s_goaltext);
	mycleanup(s_background);
	mycleanup(s_glStats);
	mycleanup(s_message);
	mycleanup(s_advanceString);
	mycleanup(s_sci_advanceScreen);
#undef mycleanup

	delete [] s_scienceGoalTree;
	s_scienceGoalTree = NULL;
}

void sci_advancescreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	if(sci_advancescreen_removeMyWindow(action))
	{
		ctp2_ListItem *item;

		item = (ctp2_ListItem*)s_advanceList->GetSelectedItem();

		if (!item)
		{
			return;
		}

		g_player[g_selected_item->GetVisiblePlayer()]->SetResearching( (long)item->GetUserData() );
		if(g_scienceManagementDialog) {
			g_scienceManagementDialog->Update();
		}

		if ( g_scienceWin )
			g_scienceWin->UpdateData( SCI_UPDATE_NOLIST );
	}
}

void sci_advancescreen_cancelPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_player[g_selected_item->GetVisiblePlayer()]->SetResearching( s_oldResearching );

	if(g_scienceManagementDialog)
	{
		g_scienceManagementDialog->Update();
	}

	if(sci_advancescreen_removeMyWindow(action))
	{
	}
}

//----------------------------------------------------------------------------
//
// Name       : sci_advancescreen_loadList
//
// Description: Generate the list of advances that may be researched next.
//
// Parameters : -
//
// Globals    : g_player			: list of players
//				g_selected_item		: determines currently active player
//				g_theAdvanceDB		: advance database
//				g_theStringDB		: language dependent text to display
//				s_scienceGoalTree	: list of advances that lead to the goal
//
// Returns    : s_advanceList		: filled with the generated list
//				s_goaltext			: set from current research goal
//				s_sci_advanceScreen	: redrawn
//				s_scienceGoalTree	: created (empty) when not existing yet
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 sci_advancescreen_loadList( void )
{
	Player *p = g_player[g_selected_item->GetVisiblePlayer()];
	MBCHAR str[_MAX_PATH];
	if(p->m_researchGoal >= 0)
	{
		sint32 all;
		sint32 have;
		sci_advancescreen_initAndFillGoalArray(p->m_researchGoal);
		sci_advancescreen_getGoalAdvances(have, all);
		sprintf(str,"%s %s (%d/%d)",g_theStringDB->GetNameStr("str_ldl_ResearchGoal"),
			g_theAdvanceDB->Get(p->m_researchGoal)->GetNameText(), have, all);
	}
	else
	{
		sci_advancescreen_clearGoalArray();
		sprintf(str,"%s",g_theStringDB->GetNameStr("str_ldl_ResearchGoal"));
	}
	s_goaltext->SetText(str);

	if (!s_advanceList) return 0;

	ctp2_ListItem *item = NULL;
	ctp2_Static *child = NULL;

	s_advanceList->Clear();

	uint8 *			advances		= p->m_advances->CanResearch();
	sint32 const	advanceCount	= g_theAdvanceDB->NumRecords();
	sint32			curItemIndex	= 0;

	for (sint32 i = 0; i < advanceCount; ++i)
	{
		if( advances[i] )
		{
			if(s_scienceGoalTree[i])
			{
				sprintf( str, "%s*", g_theStringDB->GetNameStr(g_theAdvanceDB->Get(i)->m_name) );
			}
			else
			{
				sprintf( str, "%s", g_theStringDB->GetNameStr(g_theAdvanceDB->Get(i)->m_name) );
			}

			item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("SciListItem");
			child = (ctp2_Static *)item->GetChildByIndex(0);
			if(child)
			{
				child->SetText(str);
			}
			item->SetUserData((void*)i);
			item->SetCompareCallback(ScienceSortCallback);

			s_advanceList->AddItem( item );
			curItemIndex++;
		}
	}

	delete advances;

	s_advanceList->SortByColumn(0,TRUE);

	// Find the current research in the sorted list.
	// If not found, index 0 (the cheapest) will be selected.
	sint32 const	research	= p->m_advances->GetResearching();
	bool			isIndexOk	= false;

	for (sint32 index = curItemIndex - 1; (index >= 0) && !isIndexOk; --index)
	{
		s_advanceList->SelectItem(index);
		ctp2_ListItem *	item =
			reinterpret_cast<ctp2_ListItem *>(s_advanceList->GetSelectedItem());
		isIndexOk = (research == reinterpret_cast<sint32>(item->GetUserData()));
	}

	s_sci_advanceScreen->ShouldDraw(TRUE);

	return 0;
}

sint32 sci_advancescreen_updateData( MBCHAR *messageText, BOOL defaultMessage )
{
	MBCHAR str[_MAX_PATH];
	sint32 advanceTurns;

	Player *p = g_player[g_selected_item->GetVisiblePlayer()];

	if ( messageText )
	{
		s_message->SetHyperText( messageText );
	}
	else if ( defaultMessage )
	{
		s_message->SetHyperText( s_advanceString->GetString(0) );
	}

	ctp2_ListItem *item;

	item = (ctp2_ListItem*)s_advanceList->GetSelectedItem();

	if ( !item )
	{
		sprintf( str, "-" );
		s_turnsBox->SetText( str );
		s_changeBox->SetText( "" );
		s_glStats->SetHyperText( "" );
		return 0;
	}

	sci_advancescreen_setStatsInfo( (long)item->GetUserData(), g_selected_item->GetVisiblePlayer());

	advanceTurns = p->m_advances->TurnsToNextAdvance((AdvanceType)item->GetUserData());

	if ( advanceTurns == -1 )
	{
		sprintf( str, "-" );
	}
	else
	{
		if ( advanceTurns <= 0 )
		{
			sprintf( str, "1" );
		}
		else
		{
			sprintf( str, "%d", advanceTurns + 1 );
		}
	}

	s_turnsBox->SetText( str );

	s_changeBox->SetText( item->GetText() );

	return 1;
}

sint32 sci_advancescreen_setStatsInfo(const sint32 index, const sint32 owner)
{
	if (s_glStats == NULL)
		return(1);

#define GIVES_TEXT_LEN 8192
	MBCHAR givesText[GIVES_TEXT_LEN];
	MBCHAR linkText[GIVES_TEXT_LEN];
	givesText[0] = 0;

	bool isAdvance = false;

	bool anyAdvance = false;

	sint32 i;
	for(i = 0; i < g_theUnitDB->NumRecords(); i++)
	{
		const UnitRecord *rec = g_theUnitDB->Get(i);
		if(rec->GetEnableAdvanceIndex() == index)
		{

			if( !isAdvance )
			{
				sprintf(givesText, "%s\n", g_theStringDB->GetNameStr("ADVANCE_GIVES_UNITS"));
				isAdvance = true;
				anyAdvance = true;
			}

			sprintf(linkText, "  <L:DATABASE_UNITS,%s>%s<e>\n", rec->GetIDText(), rec->GetNameText());
			strncat(givesText, linkText, GIVES_TEXT_LEN - strlen(givesText));
		}
	}

	isAdvance = false;

	if(strlen(givesText) > GIVES_TEXT_LEN - 100)
	{
		return 1;
	}

	for(i = 0; i < g_theBuildingDB->NumRecords(); i++)
	{
		const BuildingRecord *rec = buildingutil_Get(i, owner);
		if(rec->GetEnableAdvanceIndex() == index)
		{

			if( !isAdvance )
			{
				sprintf(givesText + strlen(givesText), "%s\n",
					g_theStringDB->GetNameStr("ADVANCE_GIVES_BUILDINGS"));
				isAdvance = true;
				anyAdvance = true;
			}

			sprintf(linkText, "  <L:DATABASE_BUILDINGS,%s>%s<e>\n", rec->GetIDText(), rec->GetNameText());
			strncat(givesText, linkText, GIVES_TEXT_LEN - strlen(givesText));
		}
	}

	if(strlen(givesText) > GIVES_TEXT_LEN - 100)
	{
		return 1;
	}

	isAdvance = false;

	for(i = 0; i < g_theWonderDB->NumRecords(); i++)
	{
		const WonderRecord *rec = wonderutil_Get(i, owner);

		if(rec->GetEnableAdvanceIndex() == index)
		{
			if( !isAdvance )
			{
				sprintf(givesText + strlen(givesText), "%s\n",
					g_theStringDB->GetNameStr("ADVANCE_GIVES_WONDERS"));
				isAdvance = true;
				anyAdvance = true;
			}

			sprintf(linkText, "  <L:DATABASE_WONDERS,%s>%s<e>\n", rec->GetIDText(), rec->GetNameText());
			strncat(givesText, linkText, GIVES_TEXT_LEN - strlen(givesText));
		}
	}

	bool isTileImp = false;
	for(i = 0; i < g_theTerrainImprovementDB->NumRecords(); i++)
	{
		const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(i);

		if(terrainutil_AdvanceEnablesImprovementForPlayer(g_selected_item->GetVisiblePlayer(),
														  index,
														  i))
		{
			if(!isTileImp)
			{
				sprintf(givesText + strlen(givesText), "%s\n",
						g_theStringDB->GetNameStr("ADVANCE_GIVES_TILE_IMPROVEMENTS"));
				isTileImp = true;
				anyAdvance = true;
			}
			sprintf(linkText, "  <L:DATABASE_TILE_IMPROVEMENTS,%s>%s<e>\n", rec->GetIDText(), rec->GetNameText());
			strncat(givesText, linkText, GIVES_TEXT_LEN - strlen(givesText));
		}
	}

	bool isGov = false;
	for(i = 0; i < g_theGovernmentDB->NumRecords(); i++)
	{
		const GovernmentRecord *rec = g_theGovernmentDB->Get(i);
		if(rec->GetEnableAdvanceIndex() == index)
		{
			if(!isGov)
			{
				sprintf(givesText + strlen(givesText), "%s\n", g_theStringDB->GetNameStr("ADVANCE_GIVES_GOVS"));
				isGov = true;
				anyAdvance = true;
			}
			sprintf(linkText, "  <L:DATABASE_GOVERNMENTS,%s>%s<e>\n", rec->GetIDText(), rec->GetNameText());
			strncat(givesText, linkText, GIVES_TEXT_LEN - strlen(givesText));
		}
	}

	if( !anyAdvance )
	{
		sprintf( givesText, "%s", g_theStringDB->GetNameStr( "ADVANCE_NO_SPECIFIC_ADVANCES" ) );
	}

	s_glStats->SetHyperText(givesText);

#if 0
	sprintf( variableFile, g_theAdvanceDB->Get(index)->GetIcon()->GetVari() );

	text = (char *)(g_GreatLibPF->getData(variableFile, &size));

	if (text == NULL)
	{
		s_glStats->SetHyperText(" ");
		return(0);
	}

	terminated_text = new char[size+1];
	memcpy(terminated_text, text, size);
	terminated_text[size] = 0;

	g_GreatLibPF->freeData(text);

	s_glStats->SetHyperText(terminated_text);

	delete [] terminated_text;

#endif

	if(s_sci_advanceScreen)
		s_sci_advanceScreen->ShouldDraw(TRUE);

	return(1);
}

sint32 sci_advancescreen_isOnScreen()
{
	if(!s_sci_advanceScreen)
		return 0;
	if(!g_c3ui->GetWindow(s_sci_advanceScreen->Id()))
		return 0;
	return 1;
}

sint32 ScienceSortCallback(ctp2_ListItem *item1, ctp2_ListItem *item2, sint32 column)
{
	sint32 turns1,turns2;
	turns1 = g_theAdvanceDB->Get((long)item1->GetUserData())->GetCost();
	turns2 = g_theAdvanceDB->Get((long)item2->GetUserData())->GetCost();
	return turns1-turns2;
}

void sci_advancescreen_fillGoalArray(sint32 goal)
{
	const AdvanceRecord *rec = g_theAdvanceDB->Get(goal);

	s_scienceGoalTree[goal]=true;

	sint32 i;
	for(i = 0; i < rec->GetNumPrerequisites(); i++)
	{
		if(rec->GetIndex() != rec->GetPrerequisitesIndex(i))
		{
			sci_advancescreen_fillGoalArray(rec->GetPrerequisitesIndex(i));
		}
	}
}

void sci_advancescreen_initAndFillGoalArray(sint32 goal)
{
	sci_advancescreen_clearGoalArray();
	sci_advancescreen_fillGoalArray(goal);
}

void sci_advancescreen_clearGoalArray()
{
	if(!s_scienceGoalTree)
	{
		s_scienceGoalTree = new bool[g_theAdvanceDB->NumRecords()];
	}

	memset(s_scienceGoalTree, 0, sizeof(bool)*g_theAdvanceDB->NumRecords());
}

bool sci_advancescreen_hasGoal()
{
	return s_scienceGoalTree != NULL;
}

bool sci_advancescreen_isGoal(sint32 goal)
{
	return s_scienceGoalTree && s_scienceGoalTree[goal];
}

void sci_advancescreen_getGoalAdvances(sint32 & have, sint32 & all)
{
	Player *p = g_player[g_selected_item->GetVisiblePlayer()];

	have = 0;
	all  = 0;

	for(sint32 i = 0; i < g_theAdvanceDB->NumRecords(); ++i)
	{
		if(s_scienceGoalTree[i])
		{
			all++;

			if(p->HasAdvance(i))
			{
				have++;
			}
		}
	}
}
