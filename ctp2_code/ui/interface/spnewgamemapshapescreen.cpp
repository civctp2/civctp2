//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Handling of user preferences.
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
// - Addion by Martin Gühmann: Two more world shape options,
//   flat world and Uranus world.
// - Memory leak repaired.
// - Restored compatibility.
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "spnewgamemapshapescreen.h"

#include "c3window.h"
#include "c3_popupwindow.h"
#include "c3_button.h"
#include "c3_listitem.h"
#include "c3_dropdown.h"
#include "c3_static.h"
#include "c3slider.h"
#include "c3ui.h"
#include "aui_radio.h"
#include "aui_switchgroup.h"
#include "aui_uniqueid.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "profileDB.h"			// WORLD_SHAPE
#include "custommapscreen.h"
#include "spnewgamewindow.h"
#include "keypress.h"
#include <vector>

extern C3UI			*g_c3ui;
extern ProfileDB	*g_theProfileDB;
extern World		*g_theWorld;

namespace
{
	typedef	std::vector<aui_Radio *>	MapShapeSelector;

	MapShapeSelector					s_checkBox;
    size_t        						s_mapShapeIndex;

    void SelectShape(size_t shape)
    {
        s_mapShapeIndex = shape;
		size_t	i		= 0;
        for
        (
        	MapShapeSelector::iterator	p = s_checkBox.begin();
        	p != s_checkBox.end();
        	++p
        )
        {
            (*p)->SetState(i == s_mapShapeIndex);
            ++i;
        }
    }

} // namespace

static c3_PopupWindow	*s_spNewGameMapShapeScreen	= NULL;


static aui_SwitchGroup	*s_group		= NULL;
static c3_Static	*s_ewLabel			= NULL; // Earth world
static c3_Static	*s_dwLabel			= NULL; // Doughnut world
//Added by Martin Gühmann
static c3_Static	*s_uwLabel			= NULL; // Uranus world
static c3_Static	*s_fwLabel			= NULL; // Flat world

static MBCHAR const	checknames[WORLD_SHAPE_COUNT][50] = {
	//Added two more shapes for more shape options by Martin Gühmann
	"MapShapeOne",   //Earth world (West-East wrap world)
	"MapShapeTwo",   //Doughnut world
	"MapShapeThree", //Flat world
	"MapShapeFour"   //Uranus world (North-South wrap world)
};

static sint32 s_useMode = 0;

size_t spnewgamemapshapescreen_getMapShapeIndex( void )
{
	return s_mapShapeIndex;
}






void spnewgamemapshapescreen_setMapShapeIndex(size_t index)
{
    SelectShape(index);
	g_theProfileDB->SetWorldShape(static_cast<WORLD_SHAPE>(index));
}

//----------------------------------------------------------------------------
//
// Name       : spnewgamemapshapescreen_displayMyWindow
//
// Description: Display the map shape selection window.
//
// Parameters : viewMode
//              useMode
//
// Globals    : s_spNewGameMapShapeScreen
//              s_checkBox
//              s_useMode
//
// Returns    : sint32          : 0 when the window is available
//
// Remark(s)  : When the window does not exist, it will be created.
//
//----------------------------------------------------------------------------
sint32 spnewgamemapshapescreen_displayMyWindow(BOOL viewMode, sint32 useMode)
{
    sint32 const    retval =
        s_spNewGameMapShapeScreen ? 0 : spnewgamemapshapescreen_Initialize();

	for
	(
		MapShapeSelector::iterator	p = s_checkBox.begin();
		p != s_checkBox.end();
		++p
	)
    {
        (*p)->Enable(!viewMode);
    }

	s_useMode = useMode;

#ifdef _DEBUG
	AUI_ERRCODE auiErr =
#endif
	g_c3ui->AddWindow(s_spNewGameMapShapeScreen);
	Assert(auiErr == AUI_ERRCODE_OK);

	keypress_RegisterHandler(s_spNewGameMapShapeScreen);

	return retval;
}

sint32 spnewgamemapshapescreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	uint32 id = s_group->WhichIsSelected();

	if (id)
	{
		size_t	i = 0;
		for
		(
			MapShapeSelector::iterator	p = s_checkBox.begin();
			p != s_checkBox.end();
			++p
		)
		{
			if (id == (*p)->Id())
			{
				spnewgamemapshapescreen_setMapShapeIndex(i);
			}
			++i;
		}
	}

#ifdef _DEBUG
	AUI_ERRCODE const auiErr =
#endif
        g_c3ui->RemoveWindow( s_spNewGameMapShapeScreen->Id());
	keypress_RemoveHandler(s_spNewGameMapShapeScreen);

	Assert( auiErr == AUI_ERRCODE_OK );

	if ( s_useMode == 1 ) {
		custommapscreen_displayMyWindow( FALSE, 1 );
	}

	spnewgamescreen_update();

	return 1;
}


AUI_ERRCODE spnewgamemapshapescreen_Initialize( aui_Control::ControlActionCallback *callback )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		switchBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sint32 i;

	if ( s_spNewGameMapShapeScreen ) return AUI_ERRCODE_OK;

	strcpy(windowBlock, "SPNewGameMapShapeScreen");

	{
		s_spNewGameMapShapeScreen = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(s_spNewGameMapShapeScreen, errcode) );

		s_spNewGameMapShapeScreen->Resize(s_spNewGameMapShapeScreen->Width(),s_spNewGameMapShapeScreen->Height());
		s_spNewGameMapShapeScreen->GrabRegion()->Resize(s_spNewGameMapShapeScreen->Width(),s_spNewGameMapShapeScreen->Height());
		s_spNewGameMapShapeScreen->SetStronglyModal(TRUE);
	}

	if ( !callback ) callback = spnewgamemapshapescreen_backPress;

	sprintf( controlBlock, "%s.%s", windowBlock, "Name" );
	s_spNewGameMapShapeScreen->AddTitle( controlBlock );
	s_spNewGameMapShapeScreen->AddClose( callback );


	sprintf( controlBlock, "%s.%s", windowBlock, "Group" );
	s_group = new aui_SwitchGroup( &errcode, aui_UniqueId(), controlBlock );
	Assert( AUI_NEWOK(s_group, errcode) );
	if ( !AUI_NEWOK(s_group, errcode) ) return errcode;

	for (i = 0; i < WORLD_SHAPE_COUNT; ++i)
    {
		sprintf(switchBlock, "%s.%s", controlBlock, checknames[i]);
		aui_Radio *	shape	=
			new aui_Radio(&errcode, aui_UniqueId(), switchBlock);
		s_checkBox.push_back(shape);
        s_group->AddSwitch(shape);
	}
	SelectShape(static_cast<size_t>(g_theProfileDB->GetWorldShape()));

	s_ewLabel = spNew_c3_Static( &errcode, windowBlock, "EWLabel" );
	s_dwLabel = spNew_c3_Static( &errcode, windowBlock, "DWLabel" );
	s_uwLabel = spNew_c3_Static( &errcode, windowBlock, "UWLabel" );
	s_fwLabel = spNew_c3_Static( &errcode, windowBlock, "FWLabel" );


	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return AUI_ERRCODE_OK;
}

//----------------------------------------------------------------------------
//
// Name       : spnewgamemapshapescreen_Cleanup
//
// Description: Release the memory of the screen.
//
// Parameters : -
//
// Globals    : s_spNewGameMapShapeScreen
//				s_checkBox
//				s_group
//				s_ewLabel
//				s_dwLabel
//				s_uwLabel
//				s_fwLabel
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void spnewgamemapshapescreen_Cleanup()
{
	if (s_spNewGameMapShapeScreen)
	{
		g_c3ui->RemoveWindow(s_spNewGameMapShapeScreen->Id());
		keypress_RemoveHandler(s_spNewGameMapShapeScreen);

		for
		(
			MapShapeSelector::iterator p = s_checkBox.begin();
			p != s_checkBox.end();
			++p
		)
		{
			delete *p;
		}
		MapShapeSelector().swap(s_checkBox);

#define mycleanup(mypointer) { delete mypointer; mypointer = NULL; }
		mycleanup(s_group);
		mycleanup(s_ewLabel);//Earth like world: East-West wrap world
		mycleanup(s_dwLabel);//Doughnut world
		// Added by Martin Gühmann
		mycleanup(s_uwLabel);//Uranus like world: North-South wrap world
		mycleanup(s_fwLabel);//Flat world
		mycleanup(s_spNewGameMapShapeScreen);
#undef mycleanup
	}
}

void spnewgamemapshapescreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	spnewgamemapshapescreen_removeMyWindow(action);
}
