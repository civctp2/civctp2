//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : user interface for chatbox
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
// - Added toggles for army and cell text graphic options. PFT, 07 Mar 05
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui_surface.h"
#include "aui_bitmapfont.h"
#include "aui_stringtable.h"
#include "aui_uniqueid.h"
#include "pixelutils.h"
#include "primitives.h"
#include "aui_ranger.h"

#include "c3ui.h"
#include "background.h"
#include "c3_hypertextbox.h"
#include "c3textfield.h"
#include "tiledmap.h"

#include "chatbox.h"

#include "network.h"

#include "SelItem.h"

#include "newturncount.h"
#include "TurnCnt.h"
#include "civapp.h"
#include "controlpanelwindow.h"
#include "director.h"
#include "radarmap.h"
#include "Civilisation.h"
#include "player.h"
#include "SlicEngine.h"

#include "radarwindow.h"
#include "ctp2_Window.h"

extern C3UI			*g_c3ui;
extern ColorSet		*g_colorSet;
extern Background	*g_background;
extern TiledMap		*g_tiledMap;
extern Network		g_network;
extern SelectedItem	*g_selected_item;
extern TurnCount	*g_turn;
extern CivApp		*g_civApp;

extern HWND			gHwnd;
extern ControlPanelWindow *g_controlPanel;
extern Director		*g_director;
extern RadarMap		*g_radarMap;
extern Player		**g_player;

#include "gamesounds.h"
#include "soundmanager.h"
extern SoundManager	*g_soundManager;

#include "XY_Coordinates.h"
#include "World.h"
extern World		*g_theWorld;

#include "tiledmap.h"
extern TiledMap		*g_tiledMap;

#include "gfx_options.h"

ChatBox				*g_chatBox = NULL;
extern MBCHAR g_slic_filename[_MAX_PATH];
extern BOOL g_letUIProcess;

void ChatBox::Initialize(void)
{
	g_chatBox = new ChatBox;
}

void ChatBox::Cleanup(void)
{
	if (g_chatBox)
		delete g_chatBox;

	g_chatBox = NULL;
}

ChatBox::ChatBox()
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	m_chatWindow = new ChatWindow(&errcode, aui_UniqueId(), "ChatWindow", 16,
									AUI_WINDOW_TYPE_FLOATING);
	Assert(AUI_NEWOK(m_chatWindow, errcode));
	if (!m_chatWindow || errcode != AUI_ERRCODE_OK) return;

	m_chatWindow->SetChatBox(this);

	m_active = FALSE;
}

ChatBox::~ChatBox()
{
	if (!m_chatWindow) return;

	if (m_active)
		g_c3ui->RemoveWindow(m_chatWindow->Id());

	m_active = FALSE;

	delete m_chatWindow;
	m_chatWindow = NULL;
}

void ChatBox::AddText(const MBCHAR *text)
{
	m_chatWindow->GetTextBox()->AppendHyperText(text);

	aui_Ranger *ranger = m_chatWindow->GetTextBox()->GetRanger();
	ranger->SetValue(ranger->GetValueX(), ranger->GetMaximumY());
}

void ChatBox::SetActive(BOOL active)
{
	if (!m_chatWindow) return;

	if (active) {

		g_c3ui->AddWindow(m_chatWindow);
		m_chatWindow->GetTextField()->SetKeyboardFocus();
	} else {
		g_c3ui->RemoveWindow(m_chatWindow->Id());
	}

	m_active = active;
}

void ChatBox::AddLine(sint32 playerNum, const MBCHAR *text)
{
	COLOR		color = g_colorSet->ComputePlayerColor(playerNum);
	COLORREF	colorRef = g_colorSet->GetColorRef(color);

	MBCHAR			coloredText[_MAX_PATH];

	m_chatWindow->ColorizeString(coloredText, text, colorRef);

	strcat(coloredText, "\n");

	m_chatWindow->GetTextBox()->AppendHyperText(coloredText);

	aui_Ranger *ranger = m_chatWindow->GetTextBox()->GetRanger();
	ranger->SetValue(ranger->GetValueX(), ranger->GetMaximumY());

	if (g_c3ui->GetWindow(m_chatWindow->Id()) == NULL) {
		g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0,
									gamesounds_GetGameSoundID(GAMESOUNDS_CHAT_MESSAGE),
									0,
									0);
	}

}





ChatWindow::ChatWindow(AUI_ERRCODE *retval, uint32 id, const MBCHAR *ldlBlock, sint32 bpp,
						AUI_WINDOW_TYPE type)
:	C3Window(retval, id, ldlBlock, bpp, type)
{
	*retval = InitCommonLdl(ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}

ChatWindow::~ChatWindow()
{
	if (m_textBox)
		delete m_textBox;
	if (m_textField)
		delete m_textField;
}

AUI_ERRCODE ChatWindow::InitCommonLdl(const MBCHAR *ldlBlock)
{
	MBCHAR			controlBlock[k_AUI_LDL_MAXBLOCK + 1];
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	sprintf(controlBlock, "%s.%s", ldlBlock, "ChatTextBox");
	m_textBox = new c3_HyperTextBox(&errcode, aui_UniqueId(), controlBlock, NULL, NULL);
	Assert( AUI_NEWOK(m_textBox, errcode) );
	if ( !AUI_NEWOK(m_textBox, errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;
	AddControl(m_textBox);

	sprintf(controlBlock, "%s.%s", ldlBlock, "ChatTextField");
	m_textField = new C3TextField(&errcode, aui_UniqueId(), controlBlock,
									ChatWindow::ChatCallback, (void *)this);
	Assert( AUI_NEWOK(m_textField, errcode) );
	if ( !AUI_NEWOK(m_textField, errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_textField->SetMaxFieldLen(k_CHATBOX_LINE_LENGTH);

	AddControl(m_textField);

	SetDraggable(TRUE);

	m_grabRegion->Move(0, 0);
	m_grabRegion->Resize(m_width, m_height);

	Move(0, g_radarWindow->Y() - m_height);

	return AUI_ERRCODE_OK;
}

void ChatWindow::ColorizeString(MBCHAR *destString, const MBCHAR *srcString, COLORREF colorRef)
{
	if (!destString) return;
	if (!srcString) return;

	destString[0] = '\0';

	MBCHAR		colorString[20];
	uint32		r, g, b;

	r = (colorRef & 0x000000FF);
	g = (colorRef & 0x0000FF00) >> 8;
	b = (colorRef & 0x00FF0000) >> 16;

	sprintf(colorString, "<c:%u,%u,%u>", r, g, b);
	strcat(destString, colorString);
	strcat(destString, srcString);

	sprintf(colorString, "<e>");
	strcat(destString, colorString);
}

void ChatWindow::ChatCallback(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if(action != (uint32)AUI_TEXTFIELD_ACTION_EXECUTE)
		return;

	ChatWindow		*chatWindow = (ChatWindow *)cookie;

	MBCHAR			str[k_CHATBOX_LINE_LENGTH];

	chatWindow->GetTextField()->GetFieldText(str, k_CHATBOX_LINE_LENGTH);
	chatWindow->GetTextField()->SetFieldText("");

	if (strlen(str) == 0 || !strcmp(str, "\n")) {
		chatWindow->GetChatBox()->SetActive(FALSE);
		return;
	}





	if (chatWindow->CheckForEasterEggs(str)) {
		return;
	}


	g_network.SendChatText(str, strlen(str));


















}

BOOL ChatWindow::CheckForEasterEggs(const MBCHAR *s)
{
	if (!strcmp(s, "pacman")) {
		return TRUE;
	} else
	if (!strcmp(s, "toe")) {

		return TRUE;
	} else
	if (!strncmp(s, "/rnd", 4) && !g_network.IsActive()) {
		extern BOOL gDone;
#if defined(WIN32)
		sint32 i, n;
		MSG	msg;

		MBCHAR *temp = s+4;
		while(isspace(*temp))
			temp++;

		n = atoi(temp);

		for (i=0; i<(n) && !gDone; i++) {

			NewTurnCount::StartNextPlayer(false);

			g_director->NextPlayer();
			do {
				g_controlPanel->Idle();
				if (g_civApp)
    				g_civApp->Process();

          		while (PeekMessage(&msg, gHwnd, 0, 0, PM_REMOVE) && !g_letUIProcess) {
					if (msg.message == WM_QUIT)
						gDone = TRUE;

					TranslateMessage(&msg);

					if (msg.message == WM_CHAR) {
						if ((MBCHAR)msg.wParam == 0x1B)
							i = n;
					}

					DispatchMessage(&msg);
    			}
	       		g_letUIProcess = FALSE;

			} while (((g_selected_item->GetCurPlayer() != g_selected_item->GetVisiblePlayer())) &&
				!gDone);


		}
#endif
		return TRUE;
#if 0
	} else
	if (!strcmp(s, "/A") && !g_network.IsActive()) {
        if (g_selected_item->GetCurPlayer() != g_selected_item->GetVisiblePlayer())
			return TRUE;

		if(g_network.IsActive()) {
			g_turn->NetworkEndTurn();
		} else {
            g_selected_item->Deselect(g_selected_item->GetCurPlayer());
 			g_turn->EndThisTurnBeginNewTurn();
          	g_selected_item->SetPlayerOnScreen(g_selected_item->GetCurPlayer());







			NewTurnCount::StartNextPlayer(true);

			g_director->AddCopyVision();

			g_tiledMap->InvalidateMix();
			g_tiledMap->InvalidateMap();
			g_tiledMap->Refresh();
			g_radarMap->Update();
			g_turn->InformMessages();
		}
		return TRUE;
#endif
	} else
	if (!strcmp(s, "/resetlos") && !g_network.IsActive()) {
		sint32 i;
		for(i = 0; i < k_MAX_PLAYERS; i++) {
			if(g_player[i]) {
				g_player[i]->m_vision->SetTheWholeWorldUnexplored();
			}
		}
		g_director->AddCopyVision();
#if 0
	} else
	if(!strcmp(s, "/goodmode") && !g_network.IsActive()) {
		extern sint32 g_placeGoodsMode;
		g_placeGoodsMode = !g_placeGoodsMode;
#endif
	} else
	if(!strcmp(s, "/reloadslic") && !g_network.IsActive()) {
		sint32 p;
		for(p = 0; p < k_MAX_PLAYERS; p++) {
			if(g_player[p]) {
				g_player[p]->m_messages->KillList();
			}
		}

		Assert(g_slicEngine);
		delete g_slicEngine;
		g_slicEngine = new SlicEngine;
		if(g_slicEngine->Load(g_slic_filename, k_NORMAL_FILE)) {
			g_slicEngine->Link();
		}
	} else
	if (!strncmp(s, "/exportmap", 10)) {
		const MBCHAR *temp = s + 11;
		g_theWorld->ExportMap(temp);
		return TRUE;
	} else
	if (!strncmp(s, "/importmap", 10)) {
		const MBCHAR *temp = s + 11;

		if (g_theWorld->ImportMap(temp)) {
			g_tiledMap->PostProcessMap();
			g_tiledMap->Refresh();
		}
		return TRUE;
	}  else
	if(!strncmp(s, "/attach", 7) && !g_network.IsActive()) {
		const MBCHAR *arg = s + 7;
		while(isspace(*arg))
			arg++;
		if(*arg >= '0' && *arg <= '9') {

			sint32 player = atoi(arg);
			if(g_network.IsActive()) {
				Assert(g_network.IsLocalPlayer(player));
				if(!g_network.IsLocalPlayer(player))
					return FALSE;

				if(g_network.IsClient() && !g_network.IsMyTurn())
					return FALSE;
			}

			if (g_player[player])
				g_player[player]->m_playerType = PLAYER_TYPE_ROBOT;




		}
	} else
	if(!strncmp(s, "/detach", 7) && !g_network.IsActive()) {
		const MBCHAR *arg = s + 7;
		while(isspace(*arg))
			arg++;
		if(*arg >= '0' && *arg <= '9') {

			sint32 player = atoi(arg);
			if(g_network.IsActive()) {
				Assert(g_network.IsLocalPlayer(player));
				if(!g_network.IsLocalPlayer(player))
					return FALSE;
				if(!g_network.IsHost())
					g_player[player]->m_playerType = PLAYER_TYPE_NETWORK;
				else
					g_player[player]->m_playerType = PLAYER_TYPE_HUMAN;
			} else {
				if(g_player[player])
					g_player[player]->m_playerType = PLAYER_TYPE_HUMAN;
			}




		}
	}
#if 0
 else
	if (!strncmp(s, "/demo", 5) && !g_network.IsActive()) {
		sint32 i, n;
		extern BOOL gDone;
		MSG	msg;

		const MBCHAR *temp = s+5;
		while(isspace(*temp))
			temp++;

		n = atoi(temp);







		for (i=0; i<(n) && !gDone; i++) {
			g_turn->NextRound();
			do {
				g_controlPanel->Idle();
				if (g_civApp)
					g_civApp->Process();

				while (PeekMessage(&msg, gHwnd, 0, 0, PM_REMOVE) ) {
					if (msg.message == WM_QUIT)
						gDone = TRUE;

					TranslateMessage(&msg);

					if (msg.message == WM_CHAR) {
						if ((MBCHAR)msg.wParam == 0x1B)
							i = n;
					}

					DispatchMessage(&msg);
				}







			} while (g_selected_item && !gDone &&
					 (g_selected_item->GetCurPlayer() != g_selected_item->GetVisiblePlayer()));





		}
		return TRUE;
	}
#endif
	else
	if(!strncmp(s, "/resetstrings", 13)) {
		sint32 i;
		for(i = 0; i < k_MAX_PLAYERS; i++) {
			if(g_player[i]) {
				g_player[i]->m_civilisation->AccessData()->ResetStrings();
			}
		}
	}
    else
	if(!strncmp(s, "/debugai", 8)  && !g_network.IsActive()) {

        if(g_graphicsOptions->IsArmyTextOn()){
			g_graphicsOptions->ArmyTextOff();
        }
		else
            g_graphicsOptions->ArmyTextOn();
	}

    else
	if(!strncmp(s, "/debugcells", 11)  && !g_network.IsActive()) {

		if(g_graphicsOptions->IsCellTextOn()){
			g_graphicsOptions->CellTextOff();
        }
		else
            g_graphicsOptions->CellTextOn();
	}
	return FALSE;
}

AUI_ERRCODE ChatWindow::DrawThis(aui_Surface *surface, sint32 x, sint32 y)
{

	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if (surface == NULL)
		surface = m_surface;

	RECT rect = { 0, 0, m_width, m_height };

	if (m_pattern)
		m_pattern->Draw(surface, &rect);

	primitives_FrameRect16(surface, &rect, g_colorSet->GetColor(COLOR_GREEN));

	m_dirtyList->AddRect( &rect );

	return AUI_ERRCODE_OK;

}
