//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : MP Age Screen
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
// - Starting and ending age selection screen now uses the age names from
//   gl_str.txt, Martin Gühmann.
// - Compatibility restored. 
// - Memory leak repaired.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3window.h"
#include "dialogboxwindow.h"
#include "c3_listitem.h"
#include "c3_button.h"
#include "ns_item.h"
#include "c3ui.h"

#include "ctp2_dropdown.h"	
#include "aui_StringTable.h"

#include "agerecord.h"
#include "strdb.h"

#include "spnewgamewindow.h"

#include "agesscreen.h"

#include "ns_gamesetup.h"
extern	nf_GameSetup	g_gamesetup;

extern C3UI			*g_c3ui;
extern StringDB		*g_theStringDB;

namespace
{

//----------------------------------------------------------------------------
//
// Name       : DeleteAndNull
//
// Description: Delete and NULL a pointer.
//
// Parameters : a_Pointer	: the pointer to delete
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Only works for regular pointers (delete).
//				Do not use for arrays (delete [])!
//
//----------------------------------------------------------------------------

template <typename T> inline void DeleteAndNull(T * & a_Pointer)
{
	delete a_Pointer;
	a_Pointer = NULL;
};

};	// namespace

static DialogBoxWindow *s_agesScreen	= NULL;

static aui_Button		*s_back				= NULL;
static c3_Static		*s_name				= NULL;
static c3_Static		*s_start			= NULL;
static c3_Static		*s_end				= NULL;
static ctp2_DropDown	*s_startDropDown	= NULL;
static ctp2_DropDown	*s_endDropDown		= NULL;



static sint32 s_numAges = 0; 
static sint32 s_startAge = 0;
static sint32 s_endAge = s_numAges;
sint32 agesscreen_getStartAge( void )
{
	return s_startAge;
}
sint32 agesscreen_getEndAge( void )
{
	return s_endAge;
}






void agesscreen_setStartAge( sint32 index )
{
	Assert( index >= 0 && index < s_numAges );
	if ( index < 0 || index >= s_numAges )
		return;

	s_startDropDown->SetSelectedItem( index );

	
	g_gamesetup.SetStartAge(s_startAge = index);
}






void agesscreen_setEndAge( sint32 index )
{
	Assert( index >= 0 && index < s_numAges );
	if ( index < 0 || index >= s_numAges )
		return;

	s_endDropDown->SetSelectedItem( index );

	
	g_gamesetup.SetEndAge(s_endAge = index);

}





sint32	agesscreen_displayMyWindow(BOOL viewMode)
{
	sint32 retval=0;
	if(!s_agesScreen) { retval = agesscreen_Initialize(); }

	AUI_ERRCODE auiErr;

	
	s_startDropDown->Enable( !viewMode );
	s_endDropDown->Enable( !viewMode );

	auiErr = g_c3ui->AddWindow(s_agesScreen);
	Assert( auiErr == AUI_ERRCODE_OK );

	return retval;
}
sint32 agesscreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	agesscreen_setStartAge( s_startDropDown->GetSelectedItem() );
	agesscreen_setEndAge( s_endDropDown->GetSelectedItem() );

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( s_agesScreen->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );

	return 1;
}



AUI_ERRCODE agesscreen_Initialize( aui_Control::ControlActionCallback *callback )
{
	AUI_ERRCODE errcode;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sint32 i;

	if ( s_agesScreen ) return AUI_ERRCODE_OK; 

	strcpy(windowBlock, "agesscreen");

	if ( !callback ) callback = agesscreen_backPress;

	s_agesScreen = new DialogBoxWindow(
		&errcode,
		windowBlock,
		NULL );
	Assert( AUI_NEWOK(s_agesScreen, errcode) );
	if ( !AUI_NEWOK(s_agesScreen, errcode) ) errcode;

	
	s_back = new aui_Button(
		&errcode,
		aui_UniqueId(),
		"agesscreen.closebutton" );
	Assert( AUI_NEWOK(s_back,errcode) );
	if ( !AUI_NEWOK(s_back,errcode) ) return errcode;
	s_back->SetActionFuncAndCookie(callback, NULL);

	s_name = spNew_c3_Static(&errcode,windowBlock,"NameStatic");
	s_start = spNew_c3_Static(&errcode,windowBlock,"StartStatic");
	s_end = spNew_c3_Static(&errcode,windowBlock,"EndStatic");

	sprintf( controlBlock, "%s.%s", windowBlock, "StartDropDown" );
	
	s_startDropDown = new ctp2_DropDown(
		&errcode,
		aui_UniqueId(),
		controlBlock,
		agesscreen_startDropDownCallback,
		NULL );
	Assert( AUI_NEWOK(s_startDropDown, errcode) );
	if ( !AUI_NEWOK(s_startDropDown, errcode) ) return errcode;

	sprintf( controlBlock, "%s.%s", windowBlock, "EndDropDown" );
	
	s_endDropDown = new ctp2_DropDown(
		&errcode,
		aui_UniqueId(),
		controlBlock,
		agesscreen_endDropDownCallback,
		NULL );
	Assert( AUI_NEWOK(s_endDropDown, errcode) );
	if ( !AUI_NEWOK(s_endDropDown, errcode) ) return errcode;


	aui_StringTable	startagestrings(&errcode, "strings.startagestrings");
	s_numAges = g_theAgeDB->NumRecords();
	bool const		isLdlUsable = s_numAges == startagestrings.GetNumStrings();
	for ( i = 0; i < s_numAges; i++ )
	{
		
		
		





//Added by Martin Gühmann so that no *.ldl needs to be edited
//anymore when new ages are added.
		MBCHAR const *	ageId	= g_theAgeDB->GetNameStr(i);
		MBCHAR const *	name	= g_theAgeDB->Get(i)->GetNameText();

		if (isLdlUsable && (0 == strcmp(ageId, name)))
		{
			// Age name not defined in gl_str.txt: use the ldl file text.
			name = startagestrings.GetString(i);
		}
		else if (!name)
		{
			name = ageId;
		}
	
	
	

		

		{
			ns_ListItem *item = new ns_ListItem(
				&errcode,
				name,
				"listitems.ageitem" );
			Assert( AUI_NEWOK(item,errcode) );
			if ( !AUI_NEWOK(item,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

			s_startDropDown->AddItem( (ctp2_ListItem *)item );
		}

	
	
	

		

		{
			
			ns_ListItem *item = new ns_ListItem(
				&errcode,
				name,
				"listitems.ageitem" );
			Assert( AUI_NEWOK(item,errcode) );
			if ( !AUI_NEWOK(item,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

			s_endDropDown->AddItem( (ctp2_ListItem *)item );
		}
	}

	agesscreen_setStartAge(0);
	agesscreen_setEndAge(s_numAges - 1);


	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	
	if (s_back) {
		s_back->Move( s_agesScreen->Width() - s_back->Width() - 14, s_agesScreen->Height() - s_back->Height() - 17);
	}

	return AUI_ERRCODE_OK;
}



//----------------------------------------------------------------------------
//
// Name       : agesscreen_Cleanup
//
// Description: Release the memory of all static data from this file.
//
// Parameters : -
//
// Globals    : All static variables.
//
// Returns    : Useless error code (always AUI_ERRCODE_OK).
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

AUI_ERRCODE agesscreen_Cleanup()
{
	if (s_startDropDown)
	{
		s_startDropDown->Clear();
		DeleteAndNull(s_startDropDown);
	}
	if (s_endDropDown)
	{
		s_endDropDown->Clear();
		DeleteAndNull(s_endDropDown);
	}

	DeleteAndNull(s_name);
	DeleteAndNull(s_start);
	DeleteAndNull(s_end);
	DeleteAndNull(s_back);

	if (s_agesScreen)
	{
		g_c3ui->RemoveWindow(s_agesScreen->Id());
		DeleteAndNull(s_agesScreen);
	}

	return AUI_ERRCODE_OK;
}





void agesscreen_startDropDownCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	void *cookie )
{
	if ( action != (uint32)AUI_DROPDOWN_ACTION_SELECT ) return;

	
	
	
#if 0
	
	if ( s_startDropDown->GetSelectedItem() == s_numAges - 1 )
	{
		s_startDropDown->SetSelectedItem( s_numAges - 2 );
	}
	else
	{
		if ( s_startDropDown->GetSelectedItem() >=
			 s_endDropDown->GetSelectedItem() )
			s_endDropDown->SetSelectedItem(
				s_startDropDown->GetSelectedItem() + 1 );
	}
#else
	if ( s_startDropDown->GetSelectedItem() >
		 s_endDropDown->GetSelectedItem() )
		s_endDropDown->SetSelectedItem(
			s_startDropDown->GetSelectedItem() );
#endif 

}
void agesscreen_endDropDownCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	void *cookie )
{
	if ( action != (uint32)AUI_DROPDOWN_ACTION_SELECT ) return;

	
	
	
#if 0
	
	if ( s_endDropDown->GetSelectedItem() == 0 )
	{
		s_endDropDown->SetSelectedItem( 1 );
	}
	else
	{
		if ( s_endDropDown->GetSelectedItem() <=
			 s_startDropDown->GetSelectedItem() )
			s_startDropDown->SetSelectedItem(
				s_endDropDown->GetSelectedItem() - 1 );
	}
#else
	if ( s_endDropDown->GetSelectedItem() <
		 s_startDropDown->GetSelectedItem() )
		s_startDropDown->SetSelectedItem(
			s_endDropDown->GetSelectedItem() );
#endif 

}
void agesscreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{

	
	
	agesscreen_removeMyWindow(action);
}
