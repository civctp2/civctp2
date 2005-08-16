//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Little popup window that shows the costs of a special attack
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
// - This file is not part of the original source code.
// - File created. (Aug 15th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "SpecialAttackWindow.h"
#include "colorset.h"
#include "primitives.h"
#include "c3ui.h"

#include "aui_uniqueid.h"

#include "c3_static.h"

#include "SelItem.h"
#include "player.h"
#include "maputils.h"
#include "World.h"
#include "ArmyData.h"

extern C3UI             *g_c3ui;
extern ColorSet         *g_colorSet;

namespace
{
    COLOR               s_saWindowBorderColor    = COLOR_GREEN;
}

SpecialAttackWindow     *g_theSpecialAttackWindow = NULL;
static c3_Static        *s_saWindowCostN          = NULL;
static c3_Static        *s_saWindowCostV          = NULL;
static c3_Static        *s_saWindowBackground     = NULL;

//----------------------------------------------------------------------------
//
// Name       : specialAttackWindow_Initialize
//
// Description: Initializes the tileimp tracker window.
//
// Parameters : -
//
// Globals    : g_theSpecialAttackWindow: The special attack window
//
// Returns    : 0 if initalization was successfull, -1 otherwise.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 specialAttackWindow_Initialize()
{
	MBCHAR          textBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR          controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	strcpy(textBlock, "SpecialAttackWindow");
	g_theSpecialAttackWindow = new SpecialAttackWindow( &errcode, aui_UniqueId(), textBlock, 16);
	Assert(AUI_NEWOK(g_theSpecialAttackWindow, errcode));
	if(!AUI_SUCCESS(errcode)) return -1;

	sprintf(controlBlock, "%s.%s", textBlock, "SpecialAttackCostN");
	s_saWindowCostN = new c3_Static( &errcode, aui_UniqueId(), controlBlock);
	Assert(AUI_NEWOK(s_saWindowCostN, errcode));
	if(!AUI_SUCCESS(errcode)) return -1;

	sprintf(controlBlock, "%s.%s", textBlock, "SpecialAttackCostV");
	s_saWindowCostV = new c3_Static( &errcode, aui_UniqueId(), controlBlock);
	Assert(AUI_NEWOK(s_saWindowCostV, errcode));
	if(!AUI_SUCCESS(errcode)) return -1;

	sprintf(controlBlock, "%s.%s", textBlock, "Background");
	s_saWindowBackground = new c3_Static(&errcode, aui_UniqueId(), controlBlock);
	Assert(AUI_NEWOK(s_saWindowBackground, errcode));
	if(!AUI_SUCCESS(errcode)) return -1;

	errcode = aui_Ldl::SetupHeirarchyFromRoot(textBlock);
	Assert(AUI_SUCCESS(errcode));
	if(!AUI_SUCCESS(errcode)) return -1;

	return 0;
}

//----------------------------------------------------------------------------
//
// Name       : specialAttackWindow_DisplayData
//
// Description: Displays the costs of a special attack
//
// Parameters : MapPoint &p: Position at that the given order should be
//              executed.
//              sint32 type: The type of the order in question.
//
// Globals    : g_selected_item:           The currently selected item
//              g_theOrderDB:              The order database
//              g_theSpecialAttackWindow:  The special attack window
//              g_c3ui:                    The civilization 3 graphical user interface
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void specialAttackWindow_DisplayData(MapPoint &p, sint32 type)
{
	if(!g_theSpecialAttackWindow){
		specialAttackWindow_Initialize();
	}
	Assert(g_theSpecialAttackWindow);
	if(!g_theSpecialAttackWindow)
		return;

	if(type < 0){
		g_c3ui->RemoveWindow(g_theSpecialAttackWindow->Id());
		return;
	}

	sint32 costs = 0;
	if(g_theOrderDB->Get(type)->GetUnitPretest_CanInciteRevolution()){
		ArmyData::GetInciteRevolutionCost(p, costs);
	}
	else if(g_theOrderDB->Get(type)->GetUnitPretest_CanInciteUprising()){
		ArmyData::GetInciteUprisingCost(p, costs);
	}
	else{
		costs = g_theOrderDB->Get(type)->GetGold();
	}

	MBCHAR       mytext[256];
	sprintf(mytext,"%d", costs);
	s_saWindowCostV->SetText(mytext);

	sint32       x, y;
	maputils_MapXY2PixelXY(p.x,p.y,&x,&y);
	g_theSpecialAttackWindow->Move(x,y);

	sint32		visPlayer = g_selected_item->GetVisiblePlayer();
	if(costs <= g_player[visPlayer]->GetGold()){
		s_saWindowBorderColor = COLOR_GREEN;
	}
	else{
		s_saWindowBorderColor = COLOR_RED;
	}

	g_c3ui->AddWindow(g_theSpecialAttackWindow);
	g_theSpecialAttackWindow->ShouldDraw();
}

//----------------------------------------------------------------------------
//
// Name       : specialAttackWindow_Cleanup
//
// Description: Cleans up the memory that was occupied by the special
//              attack window.
//
// Parameters : -
//
// Globals    : g_tileImpTrackerWindow:    The special attack window
//              g_c3ui:                    The civilization 3 graphical user interface
//
// Returns    : 1 if there is nothing to cleanup, otherwise 0.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 specialAttackWindow_Cleanup()
{
	if (!g_theSpecialAttackWindow) return 1;

	g_c3ui->RemoveWindow(g_theSpecialAttackWindow->Id());

	delete s_saWindowCostN;
	s_saWindowCostN = NULL;

	delete s_saWindowCostV;
	s_saWindowCostV = NULL;

	delete s_saWindowBackground;
	s_saWindowBackground = NULL;

	delete g_theSpecialAttackWindow;
	g_theSpecialAttackWindow = NULL;

	return 0;
}

//----------------------------------------------------------------------------
//
// Name       : SpecialAttackWindow::DrawThis
//
// Description: Draws the tileimp tracker window
//
// Parameters : aui_Surface *surface: The surface to draw on
//              sint32 x:             The x coordinate
//              sint32 y:             The y coordinate
//
// Globals    : g_colorSet: The color set
//
// Returns    : Returns always AUI_ERRCODE_OK.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
AUI_ERRCODE SpecialAttackWindow::DrawThis( aui_Surface *surface, sint32 x, sint32 y)
{
	if ( !surface ) surface = m_surface;

	RECT rect = { 0, 0, m_width, m_height };
	
	primitives_PaintRect16( surface, &rect, 0x0000 );
	
	C3Window::DrawThis(surface,x,y);

	primitives_FrameRect16(surface, &rect, g_colorSet->GetColor(s_saWindowBorderColor));

	return AUI_ERRCODE_OK;
}

