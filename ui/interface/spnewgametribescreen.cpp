

#include "c3.h"
#include "c3window.h"
#include "c3_popupwindow.h"
#include "c3_button.h"
#include "c3_listitem.h"
#include "c3_dropdown.h"
#include "c3_static.h"
#include "c3slider.h"
#include "c3ui.h"
#include "aui_Radio.h"
#include "c3textfield.h"
#include "textradio.h"
#include "aui_SwitchGroup.h"
#include "aui_uniqueid.h"

#include "Civilisation.h"
#include "CivilisationDB.h"
#include "StrDB.h"
#include "ProfileDB.h"

#include "player.h"

#include "spnewgamewindow.h"
#include "spnewgametribescreen.h"
#include "keypress.h"
#include "SelItem.h"

#include "GameFile.h"

extern C3UI					*g_c3ui;
extern CivilisationDatabase	*g_theCivilisationDB;
extern StringDB				*g_theStringDB;
extern ProfileDB			*g_theProfileDB;

extern Player				**g_player;

extern sint32				g_isCheatModeOn;



#define k_NUM_TRIBECOLS		4
#define k_NUM_TRIBEROWS		11
#define k_NUM_TRIBEBOXES	(k_NUM_TRIBECOLS*k_NUM_TRIBEROWS+1)

c3_PopupWindow	*g_spNewGameTribeScreen	= NULL;




static aui_SwitchGroup	*s_group		= NULL;
static aui_Radio		**s_checkBox;

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


static sint32 s_tribeIndex = -1; 
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
	
	if ( index == -1 )
	{
		
		for (sint32 i = 0;i < k_NUM_TRIBEBOXES; i++ )
			s_checkBox[ i ]->SetState( 0 );

		s_tribeIndex = -1;

		return;
	}

	
	
	BOOL shouldSetProfileDB = FALSE;

	if (!g_isCheatModeOn || 
		(g_isCheatModeOn && 
			g_player[g_selected_item->GetVisiblePlayer()] && 
			g_player[g_selected_item->GetVisiblePlayer()]->GetPlayerType() == PLAYER_TYPE_HUMAN)) {

		shouldSetProfileDB = TRUE;
	}

	
	if ( index < 0 || index >= k_NUM_TRIBEBOXES )
		return;

	for (sint32 i = 0;i < k_NUM_TRIBEBOXES; i++ )
		s_checkBox[ i ]->SetState( 0 );
	s_checkBox[ g_theCivilisationDB->m_indexToAlpha[ index ] ]->SetState( 1 );

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
	for ( sint32 i = 0; i < k_NUM_TRIBEBOXES; i++ )
	{
		if ( s_checkBox[ i ] && s_checkBox[ i ]->IsDisabled() )
		{
			s_checkBox[ i ]->Enable( TRUE );
		}
	}
}
void spnewgametribescreen_disableTribes( void )
{
	for ( sint32 i = 0; i < k_NUM_TRIBEBOXES; i++ )
	{
		if ( s_checkBox[ i ] && !s_checkBox[ i ]->IsDisabled() )
		{
			s_checkBox[ i ]->Enable( FALSE );
		}
	}
}
void spnewgametribescreen_disableTribe( sint32 tribe )
{
	
	if ( tribe == -1 ) return;

	if ( tribe >= k_NUM_TRIBEBOXES ) return;

	const sint32 alpha = g_theCivilisationDB->m_indexToAlpha[ tribe ];
	if ( alpha < k_NUM_TRIBEBOXES )
	if ( s_checkBox[ alpha ] && !s_checkBox[ alpha ]->IsDisabled() )
	{
		s_checkBox[ alpha ]->Enable( FALSE );
	}
}
void spnewgametribescreen_enableTribe( sint32 tribe )
{
	
 	if ( tribe == -1 ) return;

	if ( tribe >= k_NUM_TRIBEBOXES ) return;

	const sint32 alpha = g_theCivilisationDB->m_indexToAlpha[ tribe ];
	if ( alpha < k_NUM_TRIBEBOXES )
	if ( s_checkBox[ alpha ] && s_checkBox[ alpha ]->IsDisabled() )
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
		for ( sint32 i = 0;i < k_NUM_TRIBEBOXES;i ++ ) {
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
	sint32 x,y;
	sint32 i;

	if ( g_spNewGameTribeScreen ) return AUI_ERRCODE_OK; 

	s_tribeIndex = -1;	

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

	s_checkBox = new aui_Radio*[k_NUM_TRIBEBOXES];

	sint32 adjustForBarbarians = 0;

	for ( x = 0; x < k_NUM_TRIBECOLS;x ++ ) {
		sprintf( switchBlock, "%s.%s", controlBlock, checknames[x] );
		for ( y = 0;y < k_NUM_TRIBEROWS;y++ ) {
			i = x * k_NUM_TRIBEROWS + y + adjustForBarbarians;

			
			
			if ( i >= k_NUM_TRIBEBOXES ) goto EndLoop;

			sint32 ind = i < g_theCivilisationDB->m_nRec ?
				g_theCivilisationDB->m_alphaToIndex[ i ] : -1;

			s_checkBox[i] = new aui_Radio(
				&errcode,
				aui_UniqueId(),
				switchBlock,
				spnewgametribescreen_switchPress,
				(void *)ind );
			Assert( AUI_NEWOK(s_checkBox[i], errcode) );
			if ( !AUI_NEWOK(s_checkBox[i], errcode) ) return errcode;

			s_checkBox[i]->Offset( 0, y * 30);

			
			BOOL addit = TRUE;

			
			if ( ind >= 0 ) {
				s_checkBox[i]->SetText(
					g_theStringDB->GetNameStr(
						g_theCivilisationDB->GetSingularCivName(
							(CIV_INDEX)ind)) );
			}
			else {
				s_checkBox[i]->SetText( "" );
				s_checkBox[i]->Enable( FALSE );
				addit = FALSE;
			}

			
			if ( ind == 0 )
			{
				
				--y;
				adjustForBarbarians = 1;

				
				aui_Ldl::Remove( s_checkBox[ i ] );
			}
			else
			{
				if ( addit )
					s_group->AddSwitch( (aui_Radio *)s_checkBox[i] );
				else
					aui_Ldl::Remove( s_checkBox[ i ] );
			}
		}
	}


EndLoop:

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

	for (sint32 i = 0;i < k_NUM_TRIBEBOXES;i++ ) {
		mycleanup( s_checkBox[i] );
	}

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

	
	for (sint32 i = 0;i < k_NUM_TRIBEBOXES; i++ )
		if ( s_checkBox[ i ]->GetState() )
			break;
	if ( i == k_NUM_TRIBEBOXES ) return;

	CIV_INDEX	civ = (CIV_INDEX)g_theCivilisationDB->m_alphaToIndex[ i ];
	StringId nameString;
	nameString = g_theCivilisationDB->GetLeaderName(civ);
	s_leaderNameTextField->SetFieldText( g_theStringDB->GetNameStr(nameString) );
}
void spnewgametribescreen_femalePress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_SWITCH_ACTION_ON ) return;

	
	for (sint32 i = 0;i < k_NUM_TRIBEBOXES; i++ )
		if ( s_checkBox[ i ]->GetState() )
			break;
	if ( i == k_NUM_TRIBEBOXES ) return;

	CIV_INDEX	civ = (CIV_INDEX)g_theCivilisationDB->m_alphaToIndex[ i ];
	StringId nameString;
	nameString = g_theCivilisationDB->GetLeaderNameFemale(civ);
	s_leaderNameTextField->SetFieldText( g_theStringDB->GetNameStr(nameString) );
}
