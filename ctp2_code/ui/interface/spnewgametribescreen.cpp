//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Single player tribe selection screen.
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
// USE_SELECT_BARBARIANS    : make Barbarians selectable 
// USE_SHOW_BARBARIANS      : make Barbarians visible
// 
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Repaired memory leak.
// - Allow choice of 64 instead of 44 playable civs to be selected.
// - Crash prevention: improved box index checking.
// - Reordered layout to support mods.
//
//----------------------------------------------------------------------------

#include "c3.h"                     // Precompiled header

#include "spnewgametribescreen.h"   // Own declarations: consistency check

#include "c3window.h"
#include "c3_popupwindow.h"
#include "c3_button.h"
#include "c3_listitem.h"
#include "c3_dropdown.h"
#include "c3_static.h"
#include "c3slider.h"
#include "c3ui.h"
#include "aui_radio.h"
#include "c3textfield.h"
#include "textradio.h"
#include "aui_switchgroup.h"
#include "aui_uniqueid.h"

#include "Civilisation.h"
#include "CivilisationDB.h"
#include "StrDB.h"
#include "profileDB.h"

#include "player.h"

#include "spnewgamewindow.h"
#include "keypress.h"
#include "SelItem.h"

#include "gamefile.h"

#include <vector>           // std::vector

extern C3UI					*g_c3ui;
extern CivilisationDatabase	*g_theCivilisationDB;
extern StringDB				*g_theStringDB;
extern ProfileDB			*g_theProfileDB;

extern Player				**g_player;

extern sint32				g_isCheatModeOn;



#define k_NUM_TRIBECOLS		4
#define k_NUM_TRIBEROWS		16
#define k_NUM_TRIBEBOXES	(k_NUM_TRIBECOLS * k_NUM_TRIBEROWS)

c3_PopupWindow	*g_spNewGameTribeScreen	= NULL;




static aui_SwitchGroup	*s_group		= NULL;

static c3_Static		*s_leaderNameStatic = NULL;
static C3TextField		*s_leaderNameTextField = NULL;
static aui_SwitchGroup	*s_maleFemaleSwitchGroup = NULL;

static GENDER			s_gender;


aui_Radio		*s_maleRadio = NULL;
aui_Radio		*s_femaleRadio = NULL;

static MBCHAR			checknames[k_NUM_TRIBECOLS][50] = {
	"TribeOne",
	"TribeTwo",
	"TribeThree",
	"TribeFour"
};


namespace
{
sint32 const	            INDEX_ILLEGAL	= -1;

std::vector<aui_Radio *>    s_checkBox;
sint32			            s_tribeIndex	= INDEX_ILLEGAL;

//----------------------------------------------------------------------------
//
// Name       : ClearSelection
//
// Description: Clear the current - user - selection.
//
// Parameters : a_List  			: checkbox list (vector, actually)
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void ClearSelection(std::vector<aui_Radio *> & a_List)
{
    for 
    (
        std::vector<aui_Radio *>::iterator  p   = a_List.begin();
        p != a_List.end();
        ++p
    )
    {
        (*p)->SetState(0);
    }
}

//----------------------------------------------------------------------------
//
// Name       : IsGetSelectionIndex
//
// Description: Get the current user selection - when it exists.
//
// Parameters : a_List  			: checkbox list (vector, actually)
//
// Globals    : -
//
// Returns    : bool                : user selection exists.
//              index               : user selection index.
//
// Remark(s)  : When the user index does not exist, the index is filled with
//              INDEX_ILLEGAL.
//
//----------------------------------------------------------------------------
bool IsGetSelectionIndex(std::vector<aui_Radio *> const & a_List, size_t & index)
{
    index   = 0;

    for 
    (
        std::vector<aui_Radio *>::const_iterator  p   = a_List.begin();
        p != a_List.end();
        ++p, ++index
    )
    {
        if ((*p) && (*p)->GetState())
        {
            return true;
        }
    }

    index   = INDEX_ILLEGAL;
    return false;
}



//----------------------------------------------------------------------------
//
// Name       : LexicographicIndex
//
// Description: Get the index of a civilisation when ordered lexicographically
//              on name.
//
// Parameters : a_DbIndex			: index of civilisation in the database
//
// Globals    : g_theCivilisationDB	: the civilisation database
//
// Returns    : sint32				: index in lexicographic ordering
//
// Remark(s)  : When the civilisation index is not in the database, 
//              INDEX_ILLEGAL is returned.
//
//----------------------------------------------------------------------------
sint32 LexicographicIndex(sint32 a_DbIndex)
{
	return (a_DbIndex < 0) || (a_DbIndex >= g_theCivilisationDB->GetNumRec())
		   ? INDEX_ILLEGAL
		   : g_theCivilisationDB->m_indexToAlpha[a_DbIndex];
}

} // namespace


sint32 spnewgametribescreen_getTribeIndex( void )
{
	return s_tribeIndex;
}

GENDER spnewgametribescreen_getGender(void)
{
	return s_gender;
}







void spnewgametribescreen_setTribeIndex(
	sint32 index,
	MBCHAR *lname )
{
	
	if (index < 0)
	{
        ClearSelection(s_checkBox);
        s_tribeIndex = INDEX_ILLEGAL;
		return;
	}

	
	
	BOOL shouldSetProfileDB = FALSE;

	if (!g_isCheatModeOn || 
		(g_isCheatModeOn && 
			g_player[g_selected_item->GetVisiblePlayer()] && 
			g_player[g_selected_item->GetVisiblePlayer()]->GetPlayerType() == PLAYER_TYPE_HUMAN)) {

		shouldSetProfileDB = TRUE;
	}

	
	sint32 const	alpha = LexicographicIndex(index);

	if ((alpha < 0) || (alpha >= s_checkBox.size())) 
	{
		return;
	}

    ClearSelection(s_checkBox);
	s_checkBox[alpha]->SetState(1);

	s_tribeIndex = index;

	CIV_INDEX	civ = (CIV_INDEX)index;

	if (shouldSetProfileDB)
		g_theProfileDB->SetCivIndex(civ);

	
	StringId civString = g_theCivilisationDB->GetPluralCivName(civ);

	if (shouldSetProfileDB)
		g_theProfileDB->SetCivName((MBCHAR *)g_theStringDB->GetNameStr(civString));

    if (s_maleRadio->GetState() ) {
	 	if (shouldSetProfileDB)
	       g_theProfileDB->SetGender(GENDER_MALE);

		s_gender = GENDER_MALE;
    } else {
		if (shouldSetProfileDB)
			g_theProfileDB->SetGender(GENDER_FEMALE);
		
		s_gender = GENDER_FEMALE;
    }

	if ( lname )
	{
		s_leaderNameTextField->SetFieldText( lname );

		if (shouldSetProfileDB)
			g_theProfileDB->SetLeaderName( lname );

		spnewgamescreen_setPlayerName( lname );
	}
	else
	{
		
		StringId nameString;
		if ( s_maleRadio->GetState() )
			nameString = g_theCivilisationDB->GetLeaderName(civ);
		else
			nameString = g_theCivilisationDB->GetLeaderNameFemale(civ);

		s_leaderNameTextField->SetFieldText(
			g_theStringDB->GetNameStr(nameString) );

		if (shouldSetProfileDB)
			g_theProfileDB->
				SetLeaderName((MBCHAR *)g_theStringDB->GetNameStr(nameString));

		spnewgamescreen_setPlayerName(g_theStringDB->GetNameStr(nameString));
	}
}
void spnewgametribescreen_getLeaderName( MBCHAR *lname )
{
	s_leaderNameTextField->GetFieldText( lname, 100 );
}
void spnewgametribescreen_enableTribes( void )
{
	for ( sint32 i = 0; i < s_checkBox.size(); i++ )
	{
		if ( s_checkBox[ i ] && s_checkBox[ i ]->IsDisabled() )
		{
			s_checkBox[ i ]->Enable( TRUE );
		}
	}
}
void spnewgametribescreen_disableTribes( void )
{
	for ( sint32 i = 0; i < s_checkBox.size(); i++ )
	{
		if ( s_checkBox[ i ] && !s_checkBox[ i ]->IsDisabled() )
		{
			s_checkBox[ i ]->Enable( FALSE );
		}
	}
}
void spnewgametribescreen_disableTribe( sint32 tribe )
{
	sint32 const alpha	= LexicographicIndex(tribe);

	if ((alpha >= 0) && (alpha < s_checkBox.size()) &&
		!s_checkBox[alpha]->IsDisabled()
	   )
	{
		s_checkBox[ alpha ]->Enable( FALSE );
	}
}
void spnewgametribescreen_enableTribe( sint32 tribe )
{
	sint32 const alpha	= LexicographicIndex(tribe);

	if ((alpha < 0) || (alpha >= s_checkBox.size()))
	{
		return;
	}
	
	if (s_checkBox[alpha]->IsDisabled())
	{
		s_checkBox[ alpha ]->Enable( TRUE );
	}

	
	CIV_INDEX	civ = (CIV_INDEX)tribe;
	StringId nameString;
	if ( s_maleRadio->GetState() )
		nameString = g_theCivilisationDB->GetLeaderName(civ);
	else
		nameString = g_theCivilisationDB->GetLeaderNameFemale(civ);
	s_leaderNameTextField->SetFieldText( g_theStringDB->GetNameStr(nameString) );
	s_checkBox[alpha]->SetState(1);
}





sint32	spnewgametribescreen_displayMyWindow( void *cookie, BOOL edit )
{
	sint32 retval=0;
	if(!g_spNewGameTribeScreen) { retval = spnewgametribescreen_Initialize(); }

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->AddWindow(g_spNewGameTribeScreen);
	keypress_RegisterHandler(g_spNewGameTribeScreen);

	Assert( auiErr == AUI_ERRCODE_OK );

	if ( edit && !(g_isScenario && g_startInfoType == STARTINFOTYPE_NOLOCS))
		s_leaderNameTextField->Enable( TRUE );
	else
		s_leaderNameTextField->Enable( FALSE );

	
	
	g_spNewGameTribeScreen->Ok()->SetActionFuncAndCookie( g_spNewGameTribeScreen->Ok()->GetActionFunc(), cookie );

	return retval;
}

sint32 spnewgametribescreen_removeMyWindow(uint32 action,MBCHAR *lname)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	sint32 index = -1;
	uint32 id = s_group->WhichIsSelected();
	if ( id ) {
		for ( sint32 i = 0; i < s_checkBox.size(); ++i) {
			if ( id == s_checkBox[i]->Id() ) {
				index = g_theCivilisationDB->m_alphaToIndex[ i ];
				break;
			}
		}
	}

	
	if ( lname )
		s_leaderNameTextField->GetFieldText( lname, 100 );

	spnewgametribescreen_setTribeIndex( index, lname );

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( g_spNewGameTribeScreen->Id() );
	keypress_RemoveHandler(g_spNewGameTribeScreen);

	Assert( auiErr == AUI_ERRCODE_OK );

	spnewgamescreen_update();

	return 1;
}



AUI_ERRCODE spnewgametribescreen_Initialize( aui_Control::ControlActionCallback *callback )
{
	AUI_ERRCODE errcode;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		switchBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( g_spNewGameTribeScreen ) return AUI_ERRCODE_OK; 

	s_tribeIndex = INDEX_ILLEGAL;	

	strcpy(windowBlock, "SPNewGameTribeScreen");

	{ 
		g_spNewGameTribeScreen = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(g_spNewGameTribeScreen, errcode) );
		if ( !AUI_NEWOK(g_spNewGameTribeScreen, errcode) ) errcode;

		
		g_spNewGameTribeScreen->Resize(g_spNewGameTribeScreen->Width(),g_spNewGameTribeScreen->Height());
		g_spNewGameTribeScreen->GrabRegion()->Resize(g_spNewGameTribeScreen->Width(),g_spNewGameTribeScreen->Height());
		g_spNewGameTribeScreen->SetStronglyModal(TRUE);
	}
	
	if ( !callback ) callback = spnewgametribescreen_backPress;

	
	sprintf( controlBlock, "%s.%s", windowBlock, "Name" );
	g_spNewGameTribeScreen->AddTitle( controlBlock );
	g_spNewGameTribeScreen->AddClose( callback );



	sprintf( controlBlock, "%s.%s", windowBlock, "Group" );
	s_group = new aui_SwitchGroup( &errcode, aui_UniqueId(), controlBlock );
	Assert( AUI_NEWOK(s_group, errcode) );
	if ( !AUI_NEWOK(s_group, errcode) ) return errcode;

    size_t const    civCount            = 
        std::min<size_t>(g_theCivilisationDB->GetNumRec(), k_NUM_TRIBEBOXES);
    size_t const    rowCount            = 
        std::min<size_t>(k_NUM_TRIBEROWS, 
                         (civCount + k_NUM_TRIBECOLS - 1) / k_NUM_TRIBECOLS
                        );
	s_checkBox.resize(civCount, (aui_Radio *) NULL);

    size_t          column              = 0;
    size_t          row                 = 0;

    for (size_t i = 0; i < civCount; ++i)
    {
		sprintf(switchBlock, "%s.%s", controlBlock, checknames[column]);

        // Sort lexicographically:
        // i < civCount <= g_theCivilisationDB->GetNumRec()
        CIV_INDEX const ind = 
            static_cast<CIV_INDEX>(g_theCivilisationDB->m_alphaToIndex[i]);

	    s_checkBox[i] = new aui_Radio(&errcode,
                                      aui_UniqueId(),
                                      switchBlock,
	                                  spnewgametribescreen_switchPress,
	                                  (void *) ind 
                                     );
	    Assert(AUI_NEWOK(s_checkBox[i], errcode));
        if (!AUI_NEWOK(s_checkBox[i], errcode)) return errcode;

	    s_checkBox[i]->Offset(0, row * 30);
		s_checkBox[i]->SetText(g_theStringDB->GetNameStr
                                (g_theCivilisationDB->GetSingularCivName(ind))
                              );

	    if (CIV_INDEX_VANDALS == ind)
        {
#if defined(USE_SHOW_BARBARIANS) || defined(USE_SELECT_BARBARIANS)
		    s_group->AddSwitch(s_checkBox[i]);
            ++row;
#if !defined(USE_SELECT_BARBARIANS)
            s_checkBox[i]->Enable(false);
#endif // USE_SELECT_BARBARIANS
#endif // USE_SHOW_BARBARIANS || USE_SELECT_BARBARIANS
        }
        else
        {
		    s_group->AddSwitch(s_checkBox[i]);
            ++row;
        }

        if (row >= rowCount)
        {
            row = 0;
            ++column;
        }
    } // for

	sprintf( controlBlock, "%s.%s", windowBlock, "leadernamestatic" );
	s_leaderNameStatic = new c3_Static(
		&errcode,
		aui_UniqueId(),
		controlBlock );

	sprintf( controlBlock, "%s.%s", windowBlock, "leadernametextfield" );
	s_leaderNameTextField = new C3TextField(
		&errcode,
		aui_UniqueId(),
		controlBlock );

	sprintf( controlBlock, "%s.%s", windowBlock, "malefemaleswitchgroup" );
	s_maleFemaleSwitchGroup = new aui_SwitchGroup(
		&errcode,
		aui_UniqueId(),
		controlBlock );

	sprintf( controlBlock, "%s.%s", windowBlock, "malefemaleswitchgroup.maleradio" );
	s_maleRadio = new aui_Radio(
		&errcode,
		aui_UniqueId(),
		controlBlock,
		spnewgametribescreen_malePress );

	sprintf( controlBlock, "%s.%s", windowBlock, "malefemaleswitchgroup.femaleradio" );
	s_femaleRadio = new aui_Radio(
		&errcode,
		aui_UniqueId(),
		controlBlock,
		spnewgametribescreen_femalePress );

	
	if(g_isScenario && g_startInfoType == STARTINFOTYPE_NOLOCS)
	{
		s_leaderNameTextField->Enable(FALSE);
		s_maleRadio->Enable(FALSE);
		s_femaleRadio->Enable(FALSE);
	}
	else
	{
		s_leaderNameTextField->Enable(TRUE);
		s_maleRadio->Enable(TRUE);
		s_femaleRadio->Enable(TRUE);
	}

	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE spnewgametribescreen_Cleanup()
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };

	if ( !g_spNewGameTribeScreen  ) return AUI_ERRCODE_OK; 

	g_c3ui->RemoveWindow( g_spNewGameTribeScreen->Id() );
	keypress_RemoveHandler(g_spNewGameTribeScreen);

    for
    (
        std::vector<aui_Radio *>::iterator  p = s_checkBox.begin();
        p != s_checkBox.end();
        ++p
    )
    {
        delete *p;
    }
    s_checkBox.swap(std::vector<aui_Radio *>());

	mycleanup( s_group );


	mycleanup( s_leaderNameStatic );
	mycleanup( s_leaderNameTextField );
	mycleanup( s_maleFemaleSwitchGroup );
	mycleanup( s_maleRadio );
	mycleanup( s_femaleRadio );
	


	delete g_spNewGameTribeScreen;
	g_spNewGameTribeScreen = NULL;

	return AUI_ERRCODE_OK;

#undef mycleanup
}




void spnewgametribescreen_acceptPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	spnewgametribescreen_removeMyWindow(action);
}
void spnewgametribescreen_cancelPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	spnewgametribescreen_removeMyWindow(action);
}
void spnewgametribescreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	const sint32 size = 100;
	MBCHAR lname[ size + 1 ];
	spnewgametribescreen_getLeaderName( lname );
	
	
	spnewgametribescreen_removeMyWindow(action, lname);
}
void spnewgametribescreen_switchPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_SWITCH_ACTION_ON ) return;

	if ( (sint32)cookie == -1 ) return;

	
	CIV_INDEX	civ = (CIV_INDEX)sint32(cookie);
	StringId nameString;
	if ( s_maleRadio->GetState() )
		nameString = g_theCivilisationDB->GetLeaderName(civ);
	else
		nameString = g_theCivilisationDB->GetLeaderNameFemale(civ);
	s_leaderNameTextField->SetFieldText( g_theStringDB->GetNameStr(nameString) );
}

void spnewgametribescreen_malePress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_SWITCH_ACTION_ON ) return;

	size_t  i;
    if (IsGetSelectionIndex(s_checkBox, i))
    {
	    CIV_INDEX const civ         = 
            static_cast<CIV_INDEX>(g_theCivilisationDB->m_alphaToIndex[i]);
	    StringId const  nameString  = g_theCivilisationDB->GetLeaderName(civ);
	    s_leaderNameTextField->SetFieldText(g_theStringDB->GetNameStr(nameString));
    }
}
void spnewgametribescreen_femalePress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_SWITCH_ACTION_ON ) return;

	size_t  i;
    if (IsGetSelectionIndex(s_checkBox, i))
    {
	    CIV_INDEX const civ         = 
            static_cast<CIV_INDEX>(g_theCivilisationDB->m_alphaToIndex[i]);
	    StringId const  nameString  = g_theCivilisationDB->GetLeaderNameFemale(civ);
	    s_leaderNameTextField->SetFieldText(g_theStringDB->GetNameStr(nameString));
    }
}
