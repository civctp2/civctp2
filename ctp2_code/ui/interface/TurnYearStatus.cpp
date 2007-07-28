//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Turn display
// Id           : $Id:$
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
// - Crash prevented.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "TurnYearStatus.h"

#include <sstream>
#include "aui_ldl.h"
#include "ctp2_button.h"
#include "newturncount.h"
#include "StrDB.h"
#include "ctp2_Static.h"
#include "SelItem.h"
#include "player.h"
#include "primitives.h"
#include "colorset.h"           // g_colorSet
#include "gamefile.h"           // g_isScenario
#include "CivPaths.h"           // g_civPaths

sTurnLengthOverride *TurnYearStatus::s_pTurnLengthOverride    = NULL;
uint32               TurnYearStatus::s_turnLengthOverrideSize = 0;
bool                 TurnYearStatus::s_useCustomYear          = false;

const MBCHAR *TurnYearStatus::GetCurrentYear()
{
	static MBCHAR buf[1024];
	sint32 currentYear = NewTurnCount::GetCurrentYear(g_selected_item->GetVisiblePlayer());
	sint32 round = g_player[g_selected_item->GetVisiblePlayer()] ?
	                    g_player[g_selected_item->GetVisiblePlayer()]->m_current_round :
	                    NewTurnCount::GetCurrentRound();
	
	if(s_useCustomYear && s_pTurnLengthOverride)
	{
		if((unsigned) round >= s_turnLengthOverrideSize)
		{
			round = s_turnLengthOverrideSize - 1;
		}
		strcpy(buf, s_pTurnLengthOverride[round].text);
	}
	else
	{
		if(currentYear == 0)
		{
			currentYear = 1;
		}

		AUI_ERRCODE         errcode         = AUI_ERRCODE_OK;
		aui_StringTable *   table           = new aui_StringTable(&errcode, "YearStrings");
		sint32 const        yearStringIndex = (currentYear < 0) ? 0 : 1;
		sprintf(buf, "%ld%s", abs(currentYear), table->GetString(yearStringIndex));
		delete table;
	}

	return buf;

#if 0
	
	
	std::stringstream yearString;
	
	if (s_useCustomYear && g_pTurnLengthOverride)
	{
		uint32 round = NewTurnCount::GetCurrentRound();
		if (round > s_turnLengthOverrideSize)
		{
			round = s_turnLengthOverrideSize;
		}

		yearString << s_pTurnLengthOverride[round].text << std::ends;
	}
	else
	{
		
		yearString << abs(currentYear) << " "
			<< ((currentYear < 0) ?
			g_theStringDB->GetNameStr("str_tbl_ldl_BC") :
			g_theStringDB->GetNameStr("str_tbl_ldl_AD"))
			<< std::ends;
	}

	
	return(yearString.str());
#endif
}

const MBCHAR *TurnYearStatus::GetCurrentRound()
{
	static MBCHAR buf[1024];
	sint32 round = g_player[g_selected_item->GetVisiblePlayer()] ?
	                   g_player[g_selected_item->GetVisiblePlayer()]->m_current_round :
	                   NewTurnCount::GetCurrentRound();
	sprintf(buf, "%d %s", round, g_theStringDB->GetNameStr("str_ldl_Turns"));
	return buf;
#if 0
	
	
	std::stringstream roundString;
	roundString << NewTurnCount::GetCurrentRound() << " "
		<< g_theStringDB->GetNameStr("str_ldl_Turns")
		<< std::ends;

	
	return(roundString.str());
#endif
}

void TurnYearStatus::BuildTurnLengthOverride()
{
	if (g_isScenario || (g_scenarioName && *g_scenarioName))
	{
		MBCHAR overridePath[_MAX_PATH];
		sprintf(overridePath, "%s%s%s", g_civPaths->GetCurScenarioPath(), FILE_SEP, "turnlength.txt");

		s_useCustomYear = false;

		FILE *fp = fopen(overridePath, "r");
		if (fp)
		{
			sint32 count = 0;

			while (!feof(fp))
			{
				char dummy[256];
				if (fscanf(fp, "%[^'\n']\n", dummy))
				{
					count++;
				}
				else
				{
					break;
				}
			}

			if (count)
			{
				MBCHAR dummy[1024];
				s_pTurnLengthOverride = new sTurnLengthOverride[count];
				s_turnLengthOverrideSize = count;
				rewind(fp);
				for (int i = 0; i < count; i++)
				{
					fscanf(fp, "%d,%[^'\n']\n", &(s_pTurnLengthOverride[i].turn), &dummy);
					memset(s_pTurnLengthOverride[i].text, 0, 32);
					strncpy(s_pTurnLengthOverride[i].text, dummy, 31);
				}
			}

			s_useCustomYear = true;

			fclose(fp);
		}
	}
	else
	{
		CleanupTurnLengthOverride();
	}
}

void TurnYearStatus::CleanupTurnLengthOverride()
{
	s_useCustomYear = false;
	delete [] s_pTurnLengthOverride;
	s_pTurnLengthOverride = NULL;
}

TurnYearStatus::TurnYearStatus(MBCHAR *ldlBlock)
:   m_turnYearStatus (static_cast<ctp2_Button*>(aui_Ldl::GetObject(ldlBlock, "TurnYearStatus"))),
    m_dougsProgress  (static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock, "DougsProgressBar"))),
    m_displayType    (DISPLAY_YEAR)
{
	Assert(m_turnYearStatus);

	m_turnYearStatus->SetActionFuncAndCookie(TurnYearStatusActionCallback, this);

	m_dougsProgress->SetDrawCallbackAndCookie(DrawDougsProgress, this);
}

void TurnYearStatus::UpdatePlayer(PLAYER_INDEX player)
{
	Update();
}

void TurnYearStatus::Update()
{
	
	switch(m_displayType) 
	{
		case DISPLAY_YEAR:
			if (s_useCustomYear && s_pTurnLengthOverride)
			{
				uint32 round = NewTurnCount::GetCurrentRound();
				if (round > s_turnLengthOverrideSize)
				{
					round = s_turnLengthOverrideSize;
				}

				m_turnYearStatus->SetText(s_pTurnLengthOverride[round].text);
			}
			else
			{
				m_turnYearStatus->SetText(GetCurrentYear());
			}
			break;
		case DISPLAY_TURN:
			
			m_turnYearStatus->SetText(GetCurrentRound());
			break;
		default:
			Assert(false);
			break;
	}
	m_dougsProgress->ShouldDraw(TRUE);
}


void TurnYearStatus::TurnYearStatusActionCallback(aui_Control *control, uint32 action,
												  uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;


	
	TurnYearStatus *turnYearStatus = static_cast<TurnYearStatus*>(cookie);

	turnYearStatus->m_displayType = static_cast<DisplayType>(
		(turnYearStatus->m_displayType + 1) % NUMBER_OF_DISPLAY_TYPES);

	
	turnYearStatus->Update();
}


AUI_ERRCODE TurnYearStatus::DrawDougsProgress(ctp2_Static *control,
											  aui_Surface *surface,
											  RECT &rect,
											  void *cookie)
{
	
	if(g_selected_item == NULL)
		return AUI_ERRCODE_OK;

	if (NULL == g_player)
	{
		return AUI_ERRCODE_OK;
	}

	if(!g_player[g_selected_item->GetVisiblePlayer()]) {
		return AUI_ERRCODE_OK;
	}

	primitives_PaintRect16(surface, &rect, g_colorSet->GetColor(COLOR_BLACK));
	if(g_selected_item->GetVisiblePlayer() != g_selected_item->GetCurPlayer()) {
		sint32 p;

		sint32 alive = 0;
		sint32 progress = 0;
		
		sint32 startp = g_selected_item->GetVisiblePlayer() + 1;
		if(startp >= k_MAX_PLAYERS)
			startp = 0;
		for(p = startp; p != g_selected_item->GetVisiblePlayer(); p++) {
			if(g_player[p]) {
				alive++;
			}
			if(p == g_selected_item->GetCurPlayer()) {
				progress = alive;
			}
			if(p == k_MAX_PLAYERS - 1) {
				
				p = -1;
			}
		}

		if(alive > 0) {
			RECT tmp = rect;
			sint32 width = tmp.right - tmp.left;
			sint32 displayWidth = (width * progress) / alive;
			if(displayWidth > width) {
				displayWidth = width;
			}
			tmp.right = tmp.left + displayWidth;
			primitives_PaintRect16(surface, &tmp, g_colorSet->GetPlayerColor(g_selected_item->GetCurPlayer()));
		}
	}
	return AUI_ERRCODE_OK;
}
