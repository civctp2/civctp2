#include "c3.h"

#include "aui.h"
#include "aui_uniqueid.h"
#include "aui_ldl.h"
#include "c3ui.h"

#include "background.h"
#include "statuswindow.h"
#include "radarwindow.h"
#include "statswindow.h"

#include "chatbox.h"

#include "network.h"

#include "TradePool.h"
#include "tiledmap.h"
#include "c3windows.h"
#include "c3cmdline.h"

#include "aui_static.h"

#include "backgroundwin.h"
#include "controlpanelwindow.h"

#include "bset.h"

#include "profileDB.h"

#include "director.h"

#include "screenmanager.h"
extern ScreenManager	*g_screenManager;

extern Director			*g_director;

Background				*g_background = NULL;

void DumpSpanList(aui_DirtyList *list);




extern sint32 g_ScreenWidth;
extern sint32 g_ScreenHeight;

extern C3UI						*g_c3ui;
extern StatusWindow				*g_statusWindow;
extern StatsWindow				*g_statsWindow;
extern ControlPanelWindow		*g_controlPanel;

extern TiledMap 		*g_tiledMap;
extern Network			g_network;

extern RECT				g_backgroundViewport;

#ifdef _DEBUG
#include "DataCheck.h"
extern DataCheck		*g_dataCheck;
#endif

extern int sprite_Update(aui_Surface *surf);
extern void DisplayFrame (aui_Surface *surf);

extern sint32			g_modalWindow;
extern ChatBox			*g_chatBox;

extern ProfileDB		*g_theProfileDB;

sint32 backgroundWin_Initialize(bool fullscreen)
{
	AUI_ERRCODE errcode;

	if ( g_background ) return 0;






	sint32 backgroundWidth = g_ScreenWidth + 5 * k_TILE_GRID_WIDTH / 2;

	sint32 controlPanelHeight=0;

	if (!fullscreen)
	{
		if (g_ScreenWidth < 1024)
		{

			controlPanelHeight = 100;
		}
		else
		{

			controlPanelHeight = 150;
		}
	}
	sint32 backgroundHeight = g_ScreenHeight  + 2 * k_TILE_GRID_HEIGHT;

	sint32 backgroundX = 0 - k_TILE_GRID_WIDTH;

	sint32 backgroundY = 0 - k_TILE_GRID_HEIGHT;

	sint32 widthAdjust = 0;
	sint32 heightAdjust = 0;
	if (backgroundWidth & 0x01)
	{
		widthAdjust = 1;
		backgroundWidth += widthAdjust;
	}
	if (backgroundHeight & 0x01)
	{
		heightAdjust = 1;
		backgroundHeight += heightAdjust;
	}




	SetRect(
		&g_backgroundViewport,
		k_TILE_GRID_WIDTH,
		k_TILE_GRID_HEIGHT,
		backgroundWidth - widthAdjust - 3 * k_TILE_GRID_WIDTH / 2,
		backgroundHeight - heightAdjust - k_TILE_GRID_HEIGHT );

	g_background = new Background(
		&errcode,
		k_ID_BACKGROUND,
		backgroundX, backgroundY, backgroundWidth, backgroundHeight,
		16,
		background_draw_handler );
	Assert( g_background != NULL );
	if ( !g_background ) return -1;






	return 0;
}

void backgroundWin_Cleanup(void)
{
	if (g_c3ui && g_background)
    {
    	g_c3ui->RemoveWindow(g_background->Id());
    }

	delete g_background;
	g_background = NULL;
}

#ifdef _PLAYTEST
sint32 g_debugOwner = k_DEBUG_OWNER_NONE;
#endif

AUI_ERRCODE background_draw_handler(LPVOID bg)
{
	Background  *   back    = reinterpret_cast<Background *>(bg);
	aui_Surface	*   surface = (back)    ? back->TheSurface() : NULL;
	aui_Mouse *     mouse   = (g_c3ui)  ? g_c3ui->TheMouse() : NULL;

    if (!mouse || !g_tiledMap)
    {
        // Busy initialising: postpone drawing until ready.
        return AUI_ERRCODE_INVALIDPARAM;
    }

	if (g_modalWindow > 0)
    {
		g_screenManager->LockSurface(surface);
		g_tiledMap->DrawChatText();
		g_screenManager->UnlockSurface();

		return AUI_ERRCODE_OK;
	}

	g_tiledMap->UpdateMixFromMap(surface);

	if (g_theProfileDB->IsWaterAnim())
    {
        g_tiledMap->DrawWater();
    }

	g_theTradePool->Draw(surface);
	g_tiledMap->RepaintSprites(surface, g_tiledMap->GetMapViewRect(), false);

	g_tiledMap->DrawUnfinishedMove(surface);

	POINT pos;
	pos.y = mouse->Y() - back->Y();
	if (pos.y < back->Height())
	{
		g_tiledMap->DrawHiliteMouseTile(surface);
		g_tiledMap->DrawLegalMove(surface);
	}

#ifdef _PLAYTEST
	switch(g_debugOwner)
    {
#ifdef _DEBUG
	case k_DEBUG_OWNER_CRC:
		if(g_dataCheck) {
			g_dataCheck->DisplayCRC(surface);
			g_tiledMap->InvalidateMix();
		}
		break;
	case k_DEBUG_OWNER_NETWORK_CHAT:
		g_network.DisplayChat(surface);
		g_tiledMap->InvalidateMix();
		break;
#endif
	case k_DEBUG_OWNER_COMMANDLINE:
		g_commandLine.DisplayOutput(surface);
		g_tiledMap->InvalidateMix();
		break;
    case k_DEBUG_OWNER_FRAME_RATE:
        DisplayFrame (surface);
        break;
	default:
		break;
	}
#endif // _PLAYTEST

	g_tiledMap->CopyMixDirtyRects(back->GetDirtyList());
	g_tiledMap->ClearMixDirtyRects();

	return AUI_ERRCODE_OK;
}

void DumpSpanList(aui_DirtyList *list)
{
	sint32			i,j;
	FILE			*outFile;

	outFile = fopen("spans.txt", "wt");

	if (!outFile) return;

	aui_SpanList	*spanList = list->GetSpans();
	aui_SpanList	*curSpanList;

	for (i=0; i<list->GetHeight(); i++) {
		curSpanList = (spanList + i);
		if (curSpanList->num > 0) {
			fprintf(outFile, "Line %#.3d", i);
			for (j=0; j<curSpanList->num; j++) {
				fprintf(outFile, " Run %#.3d Len %#.3d", curSpanList->spans[j].run,
						curSpanList->spans[j].length);
			}
			fprintf(outFile, "\n");
		}
	}

	fclose(outFile);
}
