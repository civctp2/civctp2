//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Info/status handling
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
// - Hidden cities are no longer displayed in the info bar. 
//   - Oct. 15th 2004 Martin Gühmann
// - Hidden tile improvements are no longer displayed.
// - Does not show anymore the current tarrain stats but those from
//   the last visit.
// - Displays terrain from last visit, not the one since the last
//   terraforming event.
// - Displays now if a foreign city has been speacial attacked
//   as this is shown as well on the map.
//   - Last 4 on Dec. 22nd 2004 Martin Gühmann
// - InfoBar does not display anymore players without contact and player
//   fog of war is toggled on. - Dec. 24th 2004 Martin Gühmann
// - Game doesn't crash anymore if the the cursor is moved over the city
//   that belonged to the selected player and the sprite is missing.
// - Game doesn't crash anymore if the cursor is moved over a city that is
//   displayed as city belonging to a civilisation that was destroyed.
//   - Last 2 on Mar. 4th 2004 Martin Gühmann
//
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3ui.h"
#include "aui.h"
#include "pattern.h"
#include "primitives.h"
#include "InfoBar.h"
#include "c3cmdline.h"
#include "c3windows.h"
#include "aui_uniqueid.h"
#include "Cell.h"
#include "World.h"
#include "StrDB.h"
#include "TerrainRecord.h"
#include "Unit.h"
#include "SelItem.h"
#include "Player.h"
#include "Civilisation.h"
#include "TiledMap.h"
#include "TerrainImprovementRecord.h"
#include "CityData.h"

// Added by Martin Gühmann
#include "UnitData.h"
#include "unseencell.h" //Unseen cell info is needed

extern sint32		g_fog_toggle;
extern sint32		g_god;

extern sint32 g_ScreenWidth;
extern sint32 g_ScreenHeight;
extern C3UI					*g_c3ui;


InfoBar *g_infoBar = NULL;

void InfoBar::Initialize()
{
	AUI_ERRCODE errcode;

	if(!g_infoBar) {
		g_infoBar = new InfoBar(&errcode,
								aui_UniqueId(),
								196, 
								g_ScreenHeight - k_INFOBAR_HEIGHT,
								465, 
								k_INFOBAR_HEIGHT,
								16,
								k_PatternName); 

	}

	g_infoBar->Show();
}

void InfoBar::Cleanup()
{
	if(g_infoBar) {
		g_infoBar->Hide();
		g_c3ui->RemoveWindow(g_infoBar->Id());
		delete g_infoBar;
		g_infoBar = NULL;
	}
}

InfoBar::InfoBar(AUI_ERRCODE *retval,
				 uint32 id,
				 sint32 x,
				 sint32 y,
				 sint32 width,
				 sint32 height,
				 sint32 bpp,
				 MBCHAR *pattern )
	: C3Window(retval, 
			   id, 
			   x, y, width, height, 
			   bpp, pattern, 
			   AUI_WINDOW_TYPE_SINKING )
{
	memset(m_str, 0, sizeof(m_str));
	m_curX = m_curY = -1;
	m_textFromMap = false;
}

AUI_ERRCODE InfoBar::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	RECT rect = { 0, 0, m_width, m_height };

	m_pattern->Draw( m_surface, &rect );









	DrawText(surface, x, y);

	
	Invalidate(); 

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE InfoBar::DrawText(aui_Surface *surface,
							  sint32 x, sint32 y)
{
	
	if(!surface) surface = m_surface;

	RECT rect = { 0, 0, m_width, m_height };
	primitives_DropText((aui_DirectSurface*)surface, 15, 2, m_str, 0x0000, 1);
	
	Invalidate(); 
	return AUI_ERRCODE_OK;
}

void InfoBar::SetText(const char *str)
{
	m_textFromMap = false;
	strncpy(m_str, str, k_MAX_INFOBAR_TEXT);
	DrawThis();
}

void InfoBar::Clear()
{
	m_str[0] = 0;
}

void InfoBar::Concat(const char *s)
{
	strncat(m_str, s, k_MAX_INFOBAR_TEXT - strlen(m_str));
}

void InfoBar::SetTextFromMap(const MapPoint &point)
{
	Cell *cell = g_theWorld->GetCell(point);
	Clear();
	bool wroteOwner = false;

	if(m_textFromMap && m_curX == point.x && m_curY == point.y)
		return;

	if(!g_tiledMap->ReadyToDraw()) return;

	m_curX = point.x;
	m_curY = point.y;
	m_textFromMap = true;

	if(!g_player[g_selected_item->GetVisiblePlayer()] ||
		!g_player[g_selected_item->GetVisiblePlayer()]->IsExplored(point)) {
		Concat(g_theStringDB->GetNameStr("INFOBAR_UNEXPLORED"));

#ifdef _DEBUG
	MBCHAR buf[k_MAX_NAME_LEN];
	sprintf(buf, " (%d, %d)", point.x, point.y);
	Concat(buf);
#endif _DEBUG

	} else {
// Added by Martin Gühmann
		// Use the information from the last visit of that cell
		sint32 owner = g_tiledMap->GetVisibleCellOwner(const_cast<MapPoint&>(point));

		if(owner >= 0 
		&& g_player[owner]
		&&(g_player[owner]->HasContactWith(g_selected_item->GetVisiblePlayer())
		|| g_fog_toggle)
		){
			
			MBCHAR buf[k_MAX_NAME_LEN];
			g_player[owner]->m_civilisation->GetSingularCivName(buf);
			Concat(buf);
			Concat(" - ");
		}
		
#if defined(ACTIVISION_DEFAULT)
// Removed by Martin Gühmann
		Concat(g_theStringDB->GetNameStr(g_theTerrainDB->Get(cell->GetTerrainType())->GetName()));
		Concat("     ");

		if(cell->HasCity()) {
			Unit city = cell->GetCity();
#else
// Added by Martin Gühmann
		UnseenCellCarton ucell;
		BOOL hasUnseen = FALSE;
		if(!g_tiledMap->GetLocalVision()->IsVisible(point) 
		){
			hasUnseen = g_tiledMap->GetLocalVision()->GetLastSeen(point, ucell);
		}

		if(hasUnseen){
			Concat(g_theStringDB->GetNameStr(g_theTerrainDB->Get(ucell.m_unseenCell->GetTerrainType())->GetName()));
			Concat("     ");
		}
		else{
			Concat(g_theStringDB->GetNameStr(g_theTerrainDB->Get(cell->GetTerrainType())->GetName()));
			Concat("     ");
		}

// To hide the city name if the city is hidden
		Unit city = cell->GetCity();
		if(city.m_id
		&&(city->GetVisibility() & (1 << g_selected_item->GetVisiblePlayer()))
		){
			if(hasUnseen){
				if(g_theStringDB->GetNameStr("INFOBAR_CITY")) {
					Concat(g_theStringDB->GetNameStr("INFOBAR_CITY"));
				}
				Assert(ucell.m_unseenCell->GetCityName());
				if(ucell.m_unseenCell->GetCityName()){
					Concat(ucell.m_unseenCell->GetCityName());
				}
				else{
					// Shouldn't happen
					Concat(city.GetName());
				}

				if(g_player[ucell.m_unseenCell->GetCityOwner()]){ // Check whether player is still alive.
					MBCHAR civName[k_MAX_INFOBAR_TEXT];
					g_player[ucell.m_unseenCell->GetCityOwner()]->m_civilisation->GetSingularCivName(civName);
					Concat("(");
					Concat(civName);
					Concat(")");
				}

				if(ucell.m_unseenCell->IsBioInfected()) {
					Concat(g_theStringDB->GetNameStr("INFOBAR_BIO_INFECTION"));
				}
				if(ucell.m_unseenCell->IsNanoInfected()) {
					Concat(g_theStringDB->GetNameStr("INFOBAR_NANO_INFECTION"));
				}
				if(ucell.m_unseenCell->IsConverted()) {
					Concat(g_theStringDB->GetNameStr("INFOBAR_CONVERTED"));
				}
				if(ucell.m_unseenCell->IsFranchised()) {
					Concat(g_theStringDB->GetNameStr("INFOBAR_FRANCHISED"));
				}
				if(ucell.m_unseenCell->IsInjoined()) {
					Concat(g_theStringDB->GetNameStr("INFOBAR_INJOINED"));
				}
				if(ucell.m_unseenCell->IsRioting()) {
					Concat(g_theStringDB->GetNameStr("INFOBAR_RIOTING"));
				}
				if(ucell.m_unseenCell->IsWatchful()) {
					Concat(g_theStringDB->GetNameStr("INFOBAR_WATCHFUL"));
				}
			}
			else{
#endif
				if(g_theStringDB->GetNameStr("INFOBAR_CITY")) {
					Concat(g_theStringDB->GetNameStr("INFOBAR_CITY"));
				}
				Concat(city.GetName());

				MBCHAR civName[k_MAX_INFOBAR_TEXT];
				g_player[cell->GetCity().GetOwner()]->m_civilisation->GetSingularCivName(civName);
				Concat("(");
				Concat(civName);
				Concat(")");

				if(city.CD()->IsBioInfected()) {
					Concat(g_theStringDB->GetNameStr("INFOBAR_BIO_INFECTION"));
				}

				if(city.CD()->IsNanoInfected()) {
					Concat(g_theStringDB->GetNameStr("INFOBAR_NANO_INFECTION"));
				}
	
				if(city.CD()->IsConverted()) {
					Concat(g_theStringDB->GetNameStr("INFOBAR_CONVERTED"));
				}
				
				if(city.CD()->IsFranchised()) {
					Concat(g_theStringDB->GetNameStr("INFOBAR_FRANCHISED"));
				}

				if(city.CD()->IsInjoined()) {
					Concat(g_theStringDB->GetNameStr("INFOBAR_INJOINED"));
				}

				if(city.CD()->GetIsRioting()) {
					Concat(g_theStringDB->GetNameStr("INFOBAR_RIOTING"));
				}
			
				if(city.CD()->IsWatchful()) {
					Concat(g_theStringDB->GetNameStr("INFOBAR_WATCHFUL"));
				}
			}
			wroteOwner = true;
			Concat("     ");
		} else {
			sint32 i;
// Added by Martin Gühmann
			if(hasUnseen){
				// Use hidden information if necessary

				PointerList<UnseenImprovementInfo> *improvements = ucell.m_unseenCell->GetImprovements();

				PointerList<UnseenImprovementInfo>::Walker *walker = 
					new PointerList<UnseenImprovementInfo>::Walker(improvements);

				while(walker->IsValid()){
					sint32 type		= walker->GetObj()->m_type;
					sint32 percent	= walker->GetObj()->m_percentComplete;
					if(percent < 100){
						break;
					}
					Concat(" ");
					Concat(g_theStringDB->GetNameStr(g_theTerrainImprovementDB->Get(type)->GetName()));
					walker->Next();
					if(walker->IsValid() || cell->HasRiver()){
						Concat(",");
					}
				}
		
				delete walker;
			}
			else{
				for(i = 0; i < cell->GetNumDBImprovements(); i++) {
					Concat(" ");
					Concat(g_theStringDB->GetNameStr(g_theTerrainImprovementDB->Get(cell->GetDBImprovement(i))->GetName()));
					if(i < cell->GetNumDBImprovements() - 1 || cell->HasRiver()) {
						Concat(",");
					}
				}
			}

			if(cell->HasRiver()) {
				Concat(g_theStringDB->GetNameStr("INFOBAR_RIVER"));
			}

			sint32 gold, food, prod;
// Added by Martin Gühmann
			if(hasUnseen){
				// Use the values from the hidden info if the tile is hidden
				food = ucell.m_unseenCell->GetFoodProduced();
				prod = ucell.m_unseenCell->GetShieldsProduced();
				gold = ucell.m_unseenCell->GetGoldProduced();
			}
			else{
				food = cell->GetFoodProduced();
				prod = cell->GetShieldsProduced();
				gold = cell->GetGoldProduced();
			}

			Concat("(");
			char numBuf[20];
			if(food > 0) {
				Concat(g_theStringDB->GetNameStr("INFOBAR_FOOD_LABEL"));
				sprintf(numBuf, "%d", food);
				Concat(numBuf);
			}

			if(prod > 0) {
				Concat(g_theStringDB->GetNameStr("INFOBAR_PROD_LABEL"));
				sprintf(numBuf, "%d", prod);
				Concat(numBuf);
			}

			if(gold > 0) {
				Concat(g_theStringDB->GetNameStr("INFOBAR_GOLD_LABEL"));
				sprintf(numBuf, "%d", gold);
				Concat(numBuf);
			}
			Concat(")");
		}
		
#ifdef _DEBUG
		MBCHAR buf[k_MAX_NAME_LEN];
		sprintf(buf, " (%d, %d) ", point.x, point.y);
		Concat(buf);
#endif _DEBUG


		if(cell->GetNumUnits() > 0) {
			if(cell->AccessUnit(0).GetOwner() == g_selected_item->GetVisiblePlayer() ||
			   !cell->HasCity()) {
				sint32 i;
				bool anyVisible = false;
				for(i = 0; i < cell->GetNumUnits(); i++) {
					
					if(!cell->AccessUnit(i).IsValid())
						break;

					if(cell->AccessUnit(i).GetVisibility() & (1 << g_selected_item->GetVisiblePlayer())) {
						anyVisible = true;
						break;
					}
				}

				if(anyVisible) {
					if(g_theStringDB->GetNameStr("INFOBAR_UNITS")) {
						Concat(g_theStringDB->GetNameStr("INFOBAR_UNITS"));
					}

					if(!wroteOwner) {
						Concat("(");
						MBCHAR civName[k_MAX_INFOBAR_TEXT];
						g_player[cell->AccessUnit(0).GetOwner()]->m_civilisation->GetSingularCivName(civName);
						Concat(civName);
						Concat(") ");
						wroteOwner = true;
					}
					
					for(i = 0; i < cell->GetNumUnits(); i++) {
						if(cell->AccessUnit(i).GetVisibility() & (1 << g_selected_item->GetVisiblePlayer())) {
							Concat(cell->AccessUnit(i).GetName());
							if(i < cell->GetNumUnits() - 1)
								Concat(", ");
						}
					}
					Concat("     ");
				}
			}
		}		

		if(g_tiledMap) {
			
			
			g_tiledMap->GetFont()->TruncateString(m_str, m_width);
		}
	}

	ShouldDraw(TRUE);

}
