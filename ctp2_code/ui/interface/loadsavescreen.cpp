//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Load/save screen
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
// you_want_ai_civs_from_singleplayer_saved_game_showing_up_in_netshell
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Repaired memory leaks.
// - Updated tribe index check.
// - Replaced the old civilisation database by a new one. (Aug 21st 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "civ3_main.h"
#include "civapp.h"
#include "gamefile.h"

#include "aui.h"
#include "aui_ldl.h"
#include "aui_uniqueid.h"
#include "aui_stringtable.h"
#include "aui_textfield.h"
#include "ns_gamesetup.h"

#include "c3ui.h"
#include "c3window.h"
#include "c3_static.h"
#include "c3_button.h"
#include "c3_dropdown.h"
#include "c3_listitem.h"
#include "c3_listbox.h"
#include "c3_utilitydialogbox.h"

#include "ctp2_button.h"

#include "player.h"
#include "Civilisation.h"

#include "initialplaywindow.h"
#include "loadsavewindow.h"
#include "netshell.h"

#include "profileDB.h"
#include "c3_utilitydialogbox.h"
#include "StrDB.h"

#include "keypress.h"


#include "spnewgametribescreen.h"
#include "spnewgameplayersscreen.h"

#ifdef WIN32
#include <io.h>
#include <direct.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

#include "civscenarios.h"
extern CivScenarios			*g_civScenarios;


#include "gameinit.h"
#include "civapp.h"
extern CivApp				*g_civApp;

#include "optionswindow.h"

extern Player **g_player;

extern C3UI					*g_c3ui;
extern CivApp				*g_civApp;
extern nf_GameSetup			g_gamesetup;
extern ProfileDB			*g_theProfileDB;
extern StringDB				*g_theStringDB;


#include "CivilisationRecord.h"

extern sint32				g_scenarioUsePlayerNumber;


#include "spnewgamediffscreen.h"
extern BOOL					g_setDifficultyUponLaunch;
extern sint32				g_difficultyToSetUponLaunch;
extern BOOL					g_setBarbarianRiskUponLaunch;
extern sint32				g_barbarianRiskUponLaunch;




SaveInfo					*g_savedGameRequest = NULL;


LoadSaveWindow				*g_loadsaveWindow = NULL;


#include "hotseatlist.h"


#include "MessageBoxDialog.h"


#include "TurnYearStatus.h"


static uint32 s_type = LSS_TOTAL;
static c3_Static					*s_name					= NULL;
static aui_StringTable				*s_nameString			= NULL;

void loadsavescreen_setMyType(uint32 type);

void loadsavescreen_deleteDialog( bool response, void *data )
{
	if ( !response ) return;

	loadsavescreen_delete();
}




sint32	loadsavescreen_displayMyWindow(uint32 type)
{
	sint32 retval=0;
	if(!g_loadsaveWindow) { retval = loadsavescreen_Initialize(); }

	
	
	g_loadsaveWindow->CleanUpSaveInfo();

	if(retval == AUI_ERRCODE_OK) {
		g_loadsaveWindow->SetType(type);

		g_c3ui->AddWindow(g_loadsaveWindow);
		keypress_RegisterHandler(g_loadsaveWindow);
	}

	return retval;
}
sint32 loadsavescreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( g_loadsaveWindow->Id() );
	keypress_RemoveHandler(g_loadsaveWindow);
	Assert( auiErr == AUI_ERRCODE_OK );

	return 1;
}



AUI_ERRCODE loadsavescreen_Initialize( aui_Control::ControlActionCallback *callback )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( g_loadsaveWindow ) return AUI_ERRCODE_OK; 


	strcpy(windowBlock, "LoadSaveWindow");

	g_loadsaveWindow= new LoadSaveWindow(&errcode, aui_UniqueId(), windowBlock, 16 , AUI_WINDOW_TYPE_STANDARD);
	Assert( AUI_NEWOK(g_loadsaveWindow, errcode) );
	if ( !AUI_NEWOK(g_loadsaveWindow, errcode) ) return errcode;

	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	
	switch ( g_loadsaveWindow->GetType() )
	{
	case LSS_LOAD_GAME:
	case LSS_LOAD_MP:
	case LSS_LOAD_SCEN:
	case LSS_LOAD_SCEN_MP:
		g_loadsaveWindow->GetOkButton()->Enable( FALSE );
		break;

	default:
		g_loadsaveWindow->GetOkButton()->Enable( TRUE );
		break;
	}

	g_loadsaveWindow->GetDeleteButton()->Enable( FALSE );




	g_loadsaveWindow->GetListOne()->GetHeader()->Enable( FALSE );
	g_loadsaveWindow->GetListTwo()->GetHeader()->Enable( FALSE );

	
	if ( callback )
		g_loadsaveWindow->GetOkButton()->SetActionFuncAndCookie(
			callback, NULL );

	return AUI_ERRCODE_OK;
}




AUI_ERRCODE loadsavescreen_Cleanup()
{
	if ( !g_loadsaveWindow  ) return AUI_ERRCODE_OK; 

	g_c3ui->RemoveWindow( g_loadsaveWindow->Id() );
	keypress_RemoveHandler(g_loadsaveWindow);

	delete g_loadsaveWindow;
	g_loadsaveWindow = NULL;

	return AUI_ERRCODE_OK;
}

void loadsavescreen_PostCleanupAction(void)
{
	g_c3ui->AddAction(new LSCleanupAction);
}

void LSCleanupAction::Execute(aui_Control *control, uint32 action, uint32 data)
{
	loadsavescreen_Cleanup();
}




static MBCHAR s_tempPath[_MAX_PATH];
static SaveInfo	*s_tempSaveInfo = NULL;





void loadsavescreen_HotseatCallback(sint32 launch, sint32 player, 
									 sint32 civ, BOOL human, 
									 MBCHAR *name, MBCHAR *email)
{
	if(launch) {
		g_civApp->PostLoadSaveGameAction(s_tempPath);





















	} else {
		
		g_hsPlayerSetup[player].civ = civ;
		g_hsPlayerSetup[player].isHuman = human;
		delete [] g_hsPlayerSetup[player].name;
		delete [] g_hsPlayerSetup[player].email;
		g_hsPlayerSetup[player].name = new MBCHAR[strlen(name) + 1];
		strcpy(g_hsPlayerSetup[player].name, name);
		g_hsPlayerSetup[player].email = new MBCHAR[strlen(email) + 1];
		strcpy(g_hsPlayerSetup[player].email, email);
	}
}





void loadsavescreen_SetupHotseatOrEmail(void)
{
	memset(g_hsPlayerSetup, 0, sizeof(g_hsPlayerSetup));

	if(!g_hotseatList) {
		g_hotseatList = new HotseatList((HotseatListCallback *)loadsavescreen_HotseatCallback);
	}
	g_hotseatList->DisplayWindow();
}




void loadsavescreen_DifficultyScreenActionCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	if (spnewgamediffscreen_removeMyWindow(action)) {
		sint32 diff = spnewgamediffscreen_getDifficulty1();
		sint32 risk = spnewgamediffscreen_getDifficulty2();

		g_setDifficultyUponLaunch = TRUE;
		g_difficultyToSetUponLaunch = diff;

		g_setBarbarianRiskUponLaunch = TRUE;
		g_barbarianRiskUponLaunch = risk;

		g_civApp->PostLoadSaveGameAction(s_tempPath);
	}
}




void loadsavescreen_TribeScreenActionCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	MBCHAR tempNameStr[k_MAX_NAME_LEN];

	spnewgametribescreen_removeMyWindow(action, tempNameStr );



	CIV_INDEX	tribeIndex = (CIV_INDEX)spnewgametribescreen_getTribeIndex();

	if ((tribeIndex < 0) || (tribeIndex >= INDEX_TRIBE_INVALID))
    {
		tribeIndex = CIV_INDEX_CIV_1;
    }

	g_theProfileDB->SetCivIndex(tribeIndex);
	
	
	
	







	
	if (s_tempSaveInfo && s_tempSaveInfo->startInfoType == STARTINFOTYPE_NOLOCS) {
		
		
		
		BOOL noCivsInList = TRUE;
		sint32 i;
		for (i=0; i<k_MAX_PLAYERS; i++) {
			if (s_tempSaveInfo->playerCivIndexList[i] > 0) {
				noCivsInList = FALSE;
				break;
			}
		}

		
		if (noCivsInList) {
			
			
			
			
			BOOL foundOne = FALSE;

			for (i=0; i<k_MAX_PLAYERS; i++) {
				MBCHAR		*civName;
				MBCHAR		*dbString;

				civName = s_tempSaveInfo->civList[i];
				dbString = (MBCHAR *)g_theStringDB->GetNameStr(g_theCivilisationDB->Get(tribeIndex)->GetPluralCivName());
				if (strlen(civName) > 0) {
					if (!stricmp(dbString, civName)) {
						
						g_scenarioUsePlayerNumber = i;
						foundOne = TRUE;
						break;
					}
				}
			}

			if (!foundOne) {
				
				g_scenarioUsePlayerNumber = 1;
			}
		} else {
			
			for (sint32 i=0; i<k_MAX_PLAYERS; i++) {
				if (s_tempSaveInfo->playerCivIndexList[i] == tribeIndex) {
					
					g_scenarioUsePlayerNumber = i;
					break;
				}
			}
		}

		
		delete s_tempSaveInfo;
		s_tempSaveInfo = NULL;
	}

	
	spnewgamediffscreen_Initialize(loadsavescreen_DifficultyScreenActionCallback);
	spnewgamediffscreen_displayMyWindow();
	
	
}




void loadsavescreen_PlayersScreenActionCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	spnewgameplayersscreen_removeMyWindow(AUI_BUTTON_ACTION_EXECUTE);

	
	
	
	g_useScenarioCivs = g_theProfileDB->GetNPlayers()-1;

	
	
	
	if (s_tempSaveInfo->startInfoType != STARTINFOTYPE_NOLOCS) {
		if (g_useScenarioCivs > s_tempSaveInfo->numPositions) {
			g_useScenarioCivs = s_tempSaveInfo->numPositions;

			g_theProfileDB->SetNPlayers(g_useScenarioCivs+1);
		}
	}

	
	
	
	if (s_tempSaveInfo->startInfoType == STARTINFOTYPE_CIVSFIXED) {
		if (g_startEmailGame || g_startHotseatGame) {
			
			
			hotseatlist_ClearOptions();
			for (sint32 i=0; i<g_theProfileDB->GetNPlayers(); i++) {
				CIV_INDEX civIndex = s_tempSaveInfo->playerCivIndexList[i];
				hotseatlist_SetPlayerCiv(i, civIndex);
			}
			hotseatlist_LockCivs();
			loadsavescreen_SetupHotseatOrEmail();
		} else {
			

			
			spnewgamediffscreen_Initialize(loadsavescreen_DifficultyScreenActionCallback);
			spnewgamediffscreen_displayMyWindow();


		}
	} else {
		
		
		if (g_startEmailGame || g_startHotseatGame) {
			
			
			
			if (s_tempSaveInfo->startInfoType == STARTINFOTYPE_POSITIONSFIXED) {
				
				
				hotseatlist_ClearOptions();
				hotseatlist_EnableAllCivs();
				loadsavescreen_SetupHotseatOrEmail();
			} else {
				if (s_tempSaveInfo->startInfoType == STARTINFOTYPE_NOLOCS) {
					
					
					hotseatlist_ClearOptions();
					hotseatlist_LockCivs();

					
					
					
					for (sint32 i=0; i<k_MAX_PLAYERS; i++) {
						hotseatlist_EnableCiv((CIV_INDEX)s_tempSaveInfo->playerCivIndexList[i]);
					}
					loadsavescreen_SetupHotseatOrEmail();
				} else {
					
					
					hotseatlist_ClearOptions();
					hotseatlist_DisableAllCivs();
					for (sint32 i=0; i<s_tempSaveInfo->numPositions; i++) {
						hotseatlist_EnableCiv((CIV_INDEX)s_tempSaveInfo->positions[i].civIndex);
					}
					loadsavescreen_SetupHotseatOrEmail();
				}
			}

		} else {
			
			
			

			
			spnewgametribescreen_Cleanup();
			spnewgametribescreen_Initialize( loadsavescreen_TribeScreenActionCallback );
			
			if (s_tempSaveInfo->startInfoType == STARTINFOTYPE_POSITIONSFIXED) {
				
				spnewgametribescreen_enableTribes();
				spnewgametribescreen_setTribeIndex(1 + rand() % (g_theCivilisationDB->NumRecords() - 1));
			} else {
				if (s_tempSaveInfo->startInfoType == STARTINFOTYPE_NOLOCS) {
					
					
					
					spnewgametribescreen_disableTribes();

					
					
					
					if (g_saveFileVersion >= 50 && s_tempSaveInfo->startingPlayer != -1)
					{
						
						spnewgametribescreen_enableTribe((CIV_INDEX)s_tempSaveInfo->playerCivIndexList[s_tempSaveInfo->startingPlayer]);

					} else						
					{
						
						s_tempSaveInfo->startingPlayer = -1;
						s_tempSaveInfo->showLabels=FALSE;
						
						
						
						
						
						
						BOOL noCivsInList = TRUE;
						sint32 i;
						for (i=0; i<k_MAX_PLAYERS; i++) {
							if (s_tempSaveInfo->playerCivIndexList[i] > 0) {
								noCivsInList = FALSE;
								break;
							}
						}
						
						
						if (noCivsInList) {
							
							
							
							
							BOOL foundOne = FALSE;
							
							for (i=0; i<k_MAX_PLAYERS; i++) {
								MBCHAR		*civName;
								MBCHAR		*dbString;
								
								civName = s_tempSaveInfo->civList[i];
								if (strlen(civName) > 0) {
									
									for (sint32 j=0; j<g_theCivilisationDB->NumRecords(); j++) {
										dbString = (MBCHAR *)g_theStringDB->GetNameStr(g_theCivilisationDB->Get(j)->GetPluralCivName());
										
										if (!stricmp(dbString, civName)) {
											
											spnewgametribescreen_enableTribe((CIV_INDEX)j);
											foundOne = TRUE;
											break;
										}
									}
								}
							}
							
							
							
							
							if (!foundOne) {
								spnewgametribescreen_Cleanup();
								
								
								
								spnewgamediffscreen_Initialize(loadsavescreen_DifficultyScreenActionCallback);
								spnewgamediffscreen_displayMyWindow();
								
								return;
							}
							
						} else {
							
							
							
							
							for (i=0; i<k_MAX_PLAYERS; i++) {
								spnewgametribescreen_enableTribe((CIV_INDEX)s_tempSaveInfo->playerCivIndexList[i]);
							}
						}
					}
					
					spnewgametribescreen_displayMyWindow(NULL, TRUE);

					
					return;
				} else {
					
					

					
					spnewgametribescreen_disableTribes();

					
					for ( sint32 i = 0; i < s_tempSaveInfo->numPositions; i++ )
					{
						spnewgametribescreen_enableTribe(s_tempSaveInfo->positions[i].civIndex);
					}

					sint32 index = rand() % s_tempSaveInfo->numPositions;
					spnewgametribescreen_setTribeIndex(s_tempSaveInfo->positions[index].civIndex);
				}
			}

			spnewgametribescreen_displayMyWindow(NULL, TRUE);
		}
	}

	if (s_tempSaveInfo) {
		delete s_tempSaveInfo;
		s_tempSaveInfo = NULL;
	}
}


void loadsavescreen_BeginLoadProcess(SaveInfo *saveInfo, MBCHAR *directoryPath)
{
	
	
	
	MBCHAR		path[_MAX_PATH];

	sprintf(path, "%s%s%s", directoryPath, FILE_SEP, saveInfo->fileName);

	
	
	
	
	
	if (saveInfo->startInfoType != STARTINFOTYPE_NONE) {
		
		
		
		
		
		
		
		if (saveInfo->scenarioName != NULL && strlen(saveInfo->scenarioName) > 0) {
			
			
			Scenario		*scen;
			ScenarioPack	*pack;

			if (g_civScenarios->FindScenario(saveInfo->scenarioName, &pack, &scen)) {
				
				g_civPaths->SetCurScenarioPath(scen->m_path);
				g_civPaths->SetCurScenarioPackPath(pack->m_path);
				g_theProfileDB->SetIsScenario(TRUE);
				
				
				strcpy(g_scenarioName, saveInfo->scenarioName);
			} else {
				
				
				MBCHAR tempStr[_MAX_PATH];
				sprintf(tempStr, "%s%s", (MBCHAR *)g_theStringDB->GetNameStr("str_ERR_CANT_LOCATE_SCEN"), saveInfo->scenarioName);
				
				
				MessageBoxDialog::Information(tempStr,"CantLoadScenario",NULL, NULL, "str_ldl_MB_OK", false);
				return;
			}
		} else {
			
			
			g_civPaths->SetCurScenarioPath(directoryPath);
			g_theProfileDB->SetIsScenario(TRUE);
			
			g_isScenario = true;
		}
	} else {
		
		BOOL wasScenario = saveInfo->scenarioName != NULL && strlen(saveInfo->scenarioName) > 0;

		
		

		
		
		
		
		g_civPaths->ClearCurScenarioPath();
		g_theProfileDB->SetIsScenario(FALSE);

		if (wasScenario) {
			
			Scenario *scen;
			ScenarioPack *pack;
			if(g_civScenarios->FindScenario(saveInfo->scenarioName, &pack, &scen)) {
				g_civPaths->SetCurScenarioPath(scen->m_path);
				g_civPaths->SetCurScenarioPackPath(pack->m_path);
			} else {
				
				
				MBCHAR tempStr[_MAX_PATH];
				sprintf(tempStr, "%s%s", (MBCHAR *)g_theStringDB->GetNameStr("str_ERR_CANT_LOCATE_SCEN"), saveInfo->scenarioName);
				
				
				MessageBoxDialog::Information(tempStr,"CantLoadScenarioData",NULL, NULL, "str_ldl_MB_OK", false);
				return;
			}

			g_civApp->CleanupAppDB();
			g_civApp->InitializeAppDB((*(CivArchive *)(NULL)));
		}
	}

	


	extern bool g_e3Demo;

	if (saveInfo->isScenario &&
		(saveInfo->startInfoType == STARTINFOTYPE_CIVS ||
		 saveInfo->startInfoType == STARTINFOTYPE_POSITIONSFIXED ||
		 saveInfo->startInfoType == STARTINFOTYPE_CIVSFIXED ||
		 saveInfo->startInfoType == STARTINFOTYPE_NOLOCS)) { 

		
		

		
		
		strcpy(s_tempPath, path);
		delete s_tempSaveInfo;
		s_tempSaveInfo = new SaveInfo(saveInfo);

		if(g_e3Demo) {
			spnewgamediffscreen_Cleanup();
			spnewgamediffscreen_Initialize(loadsavescreen_DifficultyScreenActionCallback);
			spnewgamediffscreen_setDifficulty1(0);
			spnewgamediffscreen_setDifficulty2(0);
			loadsavescreen_DifficultyScreenActionCallback(NULL, AUI_BUTTON_ACTION_EXECUTE, 0, NULL);
		} else
			



		if (s_tempSaveInfo->numPositions <= 3 || saveInfo->startInfoType == STARTINFOTYPE_NOLOCS) {
			
			
			
			loadsavescreen_PlayersScreenActionCallback(NULL, AUI_BUTTON_ACTION_EXECUTE, 0, NULL);
		} else {
			
			spnewgameplayersscreen_Cleanup();
			spnewgameplayersscreen_Initialize( loadsavescreen_PlayersScreenActionCallback );
			spnewgameplayersscreen_SetMaxPlayers(s_tempSaveInfo->numPositions);
			spnewgameplayersscreen_displayMyWindow();
		}
	} else {
		
		
		g_civApp->PostLoadSaveGameAction(path);
	}


	
	
	if (g_isScenario || (saveInfo->scenarioName && *saveInfo->scenarioName))
	{
		if (g_pTurnLengthOverride)
		{
			
			delete [] g_pTurnLengthOverride;
			g_pTurnLengthOverride = NULL;
		}
		
		MBCHAR overridePath[_MAX_PATH];
		sprintf(overridePath, "%s%s%s", g_civPaths->GetCurScenarioPath(), FILE_SEP, "turnlength.txt");

		g_useCustomYear = false;

		FILE *fp = fopen(overridePath, "r");
		if (fp)
		{
			
			int count = 0;

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
				g_pTurnLengthOverride = new sTurnLengthOverride[count];
				g_turnLengthOverrideSize = count;
				rewind(fp);
				for (int i = 0; i < count; i++)
				{
					fscanf(fp, "%d,%[^'\n']\n", &(g_pTurnLengthOverride[i].turn), &dummy);
					memset(g_pTurnLengthOverride[i].text, 0, 32);
					strncpy(g_pTurnLengthOverride[i].text, dummy, 31);
				}
			}

			g_useCustomYear = true;

			fclose(fp);
		}
	}
	else
	{
		g_useCustomYear = false;
		if (g_pTurnLengthOverride)
		{
			delete [] g_pTurnLengthOverride;
			g_pTurnLengthOverride = NULL;
		}
	}
}



void loadsavescreen_LoadGame(void)
{
	GameInfo	*gameInfo = g_loadsaveWindow->GetGameInfo();
	SaveInfo	*saveInfo = g_loadsaveWindow->GetSaveInfo();

	Assert(gameInfo);
	if (!gameInfo) return;

	
	if (!c3files_HasLegalCD()) return;


	if (!saveInfo) {
		loadsavescreen_displayMyWindow(0);
		return;
	}

	loadsavescreen_BeginLoadProcess(saveInfo, gameInfo->path);




























































}



void loadsavescreen_SaveGame(MBCHAR *usePath, MBCHAR *useName)
{
	
	SaveInfo		*saveInfo = g_loadsaveWindow->GetSaveInfoToSave();

	Assert( saveInfo != NULL );
	if ( !saveInfo ) return;

	if (!g_loadsaveWindow->GetGameName(saveInfo->gameName)) return;
	if (!g_loadsaveWindow->GetSaveName(saveInfo->fileName)) return;
	if (!g_loadsaveWindow->GetNote(saveInfo->note)) return;

	
	saveInfo->isScenario = g_isScenario;

	if (strlen(saveInfo->gameName) == 0) {
		
		if (g_loadsaveWindow->GetGameInfo() != NULL) {
			strcpy(saveInfo->gameName, g_loadsaveWindow->GetGameInfo()->name);
		} else {
			g_loadsaveWindow->BuildDefaultSaveName(NULL, saveInfo->gameName);
			saveInfo->gameName[6] = '\0';
		}
	}

	
	nf_GameSetup gs;

	
	sint32 j = 0;
	uint32 i;
	for(i = 0; i < k_MAX_PLAYERS; i++)
	{
		if(g_player[ i ])
		{
			TribeSlot ts;

			
			ts.key = 0;

#if you_want_ai_civs_from_singleplayer_saved_game_showing_up_in_netshell
			ts.isAI = g_player[ i ]->GetPlayerType() == PLAYER_TYPE_ROBOT;
#else
			ts.isAI = 0;
#endif 

			
			
			ts.tribe = g_player[ i ]->m_civilisation->GetCivilisation() + 1;

			ts.isFemale = (g_player[i]->m_civilisation->GetGender() == GENDER_FEMALE);


			
			if ( 1 < ts.tribe )
			{
				
				
				if ( j >= k_NS_MAX_PLAYERS ) break;

				gs.GetSavedTribeSlots()[ j++ ] = ts;
			}
		}
	}
	saveInfo->gameSetup = gs;

	MBCHAR	path[_MAX_PATH];
	MBCHAR	fullPath[_MAX_PATH];

	if(usePath) {
		strcpy(path, usePath);
	} else {
		if (!g_civPaths->GetSavePath(C3SAVEDIR_GAME, path)) return;
	}
	
	if(!useName) {
		
		char *testchars="\\*\"/:|?<>";
		bool charschanged=false;
		for(i=0; (unsigned) i<strlen(saveInfo->gameName); i++)
		{
			if(strchr(testchars,saveInfo->gameName[i]))
			{
				saveInfo->gameName[i]='#';
				charschanged=true;
			}
		}
		if(charschanged)
		{
			MessageBoxDialog::Information("str_ldl_InvalidCharsFixed", "InfoInvalidCharsFixed");
		}

		sprintf(fullPath, "%s%s%s", path, FILE_SEP, saveInfo->gameName);

		// Verify that this directory exists, and if it doesn't, create it
		if (!c3files_PathIsValid(fullPath)) {
			if (!c3files_CreateDirectory(fullPath)) {
				Assert(FALSE);
				// FIXME
				// unable to create the directory for the game, should report
				// to the user, here
				return;
			}
		}

		// Check for invalid characters in filename.
		bool charschanged2=false;
		for(i=0; (unsigned) i<strlen(saveInfo->fileName); i++)
		{
			if(strchr(testchars,saveInfo->fileName[i]))
			{
				saveInfo->fileName[i]='#';
				charschanged2=true;
			}
		}
		if(charschanged2 && !charschanged)
		{
			MessageBoxDialog::Information("str_ldl_InvalidCharsFixed", "InfoInvalidCharsFixed");
		}

		// Full path, including the save file's filename
		sprintf(saveInfo->pathName, "%s%s%s", fullPath, FILE_SEP, saveInfo->fileName);
	} else {
		sprintf(fullPath, "%s", path);
		strcpy(saveInfo->fileName, useName);
		sprintf(saveInfo->pathName, "%s%s%s", fullPath, FILE_SEP, useName);
	}

	// Build a power graph from the UI
	g_loadsaveWindow->GetPowerGraph(saveInfo);
	g_loadsaveWindow->GetRadarMap(saveInfo);

	// SAM021899 changed to make a save request
	if (g_savedGameRequest)
		delete g_savedGameRequest;
	
	g_savedGameRequest = new SaveInfo(saveInfo);

//	GameFile::SaveGame(saveInfo->pathName, saveInfo);

}

/////////////////////////////////////////////////////////////

void loadsavescreen_LoadMPGame(void)
{
	if(!g_loadsaveWindow) {
		Assert(g_netfunc && !g_netfunc->IsHost());
		// HACK FIXME I still don't understand what's going on, but now
		// this happens on the client instead.
		return;
	}

	GameInfo	*gameInfo = g_loadsaveWindow->GetGameInfo();
	SaveInfo	*saveInfo = g_loadsaveWindow->GetSaveInfo();

	// SAM042099 check for a valid CD-ROM before allowing a game to be loaded
	if ((!g_netfunc || g_netfunc->IsHost()) && !c3files_HasLegalCD())
		return;

	// HACK FIXME this happens on the client, which probably shouldn't call
	// this function at all, but it wasn't immediately obvious how to tell
	// that this is a client where this is being called.
	//Assert(gameInfo);
	// EAS02161999 - Must also check to see if you're not the host.
	// 'Cause in single player mode, somebody might've already created
	// a ligitimate *single*player gameInfo that's still lying around.
	if (!gameInfo || (g_netfunc && !g_netfunc->IsHost())) { 		
		g_civApp->PostStartGameAction();
		return;
	}

	Assert(saveInfo);
	if (!saveInfo) return;

	MBCHAR		path[_MAX_PATH];

	sprintf(path, "%s%s%s", gameInfo->path, FILE_SEP, saveInfo->fileName);
	g_civApp->PostLoadSaveGameAction(path);
}

/////////////////////////////////////////////////////////////

void loadsavescreen_SaveMPGame(void)
{
	SaveInfo		*saveInfo = g_loadsaveWindow->GetSaveInfoToSave();

	Assert( saveInfo != NULL );
	if ( !saveInfo ) return;

	if (!g_loadsaveWindow->GetGameName(saveInfo->gameName)) return;
	if (!g_loadsaveWindow->GetSaveName(saveInfo->fileName)) return;
	if (!g_loadsaveWindow->GetNote(saveInfo->note)) return;

	// SAM050399
	saveInfo->isScenario = g_isScenario;

	if (strlen(saveInfo->gameName) == 0) {
		// Empty game name in the save info, copy it over from the game info
		if (g_loadsaveWindow->GetGameInfo() != NULL) {
			strcpy(saveInfo->gameName, g_loadsaveWindow->GetGameInfo()->name);
		} else {
			g_loadsaveWindow->BuildDefaultSaveName(NULL, saveInfo->gameName);
			saveInfo->gameName[6] = '\0';
		}
	}

	saveInfo->gameSetup = g_gamesetup;

	memcpy(
		saveInfo->gameSetup.GetSavedTribeSlots(),
		saveInfo->gameSetup.GetTribeSlots(),
		k_NS_MAX_PLAYERS * sizeof( TribeSlot ) );
	memset(
		saveInfo->gameSetup.GetTribeSlots(),
		0,
		k_NS_MAX_PLAYERS * sizeof( TribeSlot ) );

	MBCHAR	path[_MAX_PATH];
	MBCHAR	fullPath[_MAX_PATH];

	if (!g_civPaths->GetSavePath(C3SAVEDIR_MP, path)) return;
	
	sprintf(fullPath, "%s%s%s", path, FILE_SEP, saveInfo->gameName);
	
	// Verify that this directory exists, and if it doesn't, create it
	if (!c3files_PathIsValid(fullPath)) {
		if (!c3files_CreateDirectory(fullPath)) {
			Assert(FALSE);
			// FIXME
			// unable to create the directory for the game, should report
			// to the user, here
			return;
		}
	}

	// Full path, including the save file's filename
	sprintf(saveInfo->pathName, "%s%s%s", fullPath, FILE_SEP, saveInfo->fileName);

	// Build a power graph from the UI
	g_loadsaveWindow->GetPowerGraph(saveInfo);
	g_loadsaveWindow->GetRadarMap(saveInfo);

	// SAM021899 changed to make a save request
	if (g_savedGameRequest)
		delete g_savedGameRequest;
	
	g_savedGameRequest = new SaveInfo(saveInfo);

//	GameFile::SaveGame(saveInfo->pathName, saveInfo);

}

/////////////////////////////////////////////////////////////

void loadsavescreen_LoadSCENGame(void)
{
	// SAM042099 check for a valid CD-ROM before allowing a game to be loaded
	if (!c3files_HasLegalCD()) return;

	GameInfo	*gameInfo = g_loadsaveWindow->GetGameInfo();
	SaveInfo	*saveInfo = g_loadsaveWindow->GetSaveInfo();

	Assert(gameInfo);
	if (!gameInfo) return;

	Assert(saveInfo);
	if (!saveInfo) return;

	MBCHAR		path[_MAX_PATH];

	sprintf(path, "%s%s%s", gameInfo->path, FILE_SEP, saveInfo->fileName);
//	g_civApp->PostLoadSaveGameAction(path);

	g_civPaths->SetCurScenarioPath(gameInfo->path);
	
	g_theProfileDB->SetIsScenario(TRUE);

	g_civApp->PostLoadScenarioGameAction(saveInfo->fileName);
}

/////////////////////////////////////////////////////////////

void loadsavescreen_SaveSCENGame(void)
{
	SaveInfo		*saveInfo = g_loadsaveWindow->GetSaveInfoToSave();

	Assert( saveInfo != NULL );
	if ( !saveInfo ) return;

	if (!g_loadsaveWindow->GetGameName(saveInfo->gameName)) return;
	if (!g_loadsaveWindow->GetSaveName(saveInfo->fileName)) return;
	if (!g_loadsaveWindow->GetNote(saveInfo->note)) return;

	if (strlen(saveInfo->gameName) == 0) {
		// Empty game name in the save info, copy it over from the game info
		if (g_loadsaveWindow->GetGameInfo() != NULL) {
			strcpy(saveInfo->gameName, g_loadsaveWindow->GetGameInfo()->name);
		} else {
			g_loadsaveWindow->BuildDefaultSaveName(NULL, saveInfo->gameName);
			saveInfo->gameName[6] = '\0';
		}
	}

	// Create a default gamesetup.
	nf_GameSetup gs;

	// Must save which tribes were used.
	sint32 j = 0;
	for ( sint32 i = 0; i < k_NS_MAX_PLAYERS; i++ )
	{
		if ( g_player[ i ] )
		{
			TribeSlot ts;

			// These fields don't matter for the savedtribeslots.
			ts.isAI = 0;
			ts.key  = 0;

			// This is the only important field.
			// +1 because netshell treats zero as "none" w/ barbarians == 1.
			ts.tribe = g_player[ i ]->m_civilisation->GetCivilisation() + 1;

			ts.isFemale = g_player[i]->m_civilisation->GetGender() == GENDER_FEMALE;

			// We don't want to store the barbarians.  The netshell skips them.
			if ( 1 < ts.tribe )
				gs.GetSavedTribeSlots()[ j++ ] = ts;
		}
	}

	saveInfo->gameSetup = gs;

	MBCHAR	path[_MAX_PATH];
	MBCHAR	fullPath[_MAX_PATH];

	if (!g_civPaths->GetSavePath(C3SAVEDIR_SCEN, path)) return;
	
	sprintf(fullPath, "%s%s%s", path, FILE_SEP, saveInfo->gameName);
	
	// Verify that this directory exists, and if it doesn't, create it
	if (!c3files_PathIsValid(fullPath)) {
		if (!c3files_CreateDirectory(fullPath)) {
			Assert(FALSE);
			// FIXME
			// unable to create the directory for the game, should report
			// to the user, here
			return;
		}
	}

	// Full path, including the save file's filename
	sprintf(saveInfo->pathName, "%s%s%s", fullPath, FILE_SEP, saveInfo->fileName);

	// Build a power graph from the UI
	g_loadsaveWindow->GetPowerGraph(saveInfo);
	g_loadsaveWindow->GetRadarMap(saveInfo);

	// SAM021899 changed to make a save request
	if (g_savedGameRequest)
		delete g_savedGameRequest;
	
	g_savedGameRequest = new SaveInfo(saveInfo);

//	GameFile::SaveGame(saveInfo->pathName, saveInfo);
}

BOOL loadsavescreen_CheckOverwrite( void );

/////////////////////////////////////////////////////////////
// CallBacks
/////////////////////////////////////////////////////////////

void loadsavescreen_executePress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	// calling function should do what ever necessary to continue game state
	// this is all this callback should do

	// need to check for file overwrite before closing the window if we're saving

	if ( action == (uint32)AUI_BUTTON_ACTION_EXECUTE ) {
		switch ( g_loadsaveWindow->GetType() ) {
		case LSS_SAVE_GAME:
		case LSS_SAVE_MP:
			if ( g_loadsaveWindow->NoName() ) return;
			if ( loadsavescreen_CheckOverwrite() ) return;
			break;
		}
	}

	// remove window and do operation
	if(loadsavescreen_removeMyWindow(action)) {
		switch(g_loadsaveWindow->GetType()) {
			case LSS_LOAD_GAME:		loadsavescreen_LoadGame();			break;
			case LSS_SAVE_GAME:		loadsavescreen_SaveGame();	break;

// EAS100698 - we don't want to immediately load when picking a saved game from w/i netshell.
			case LSS_LOAD_MP:
				/*loadsavescreen_LoadMPGame();*/
				break;
			case LSS_SAVE_MP:		loadsavescreen_SaveMPGame();		break;

			case LSS_LOAD_SCEN:		loadsavescreen_LoadSCENGame();		break;
			case LSS_LOAD_SCEN_MP:	break;	// in this case, do nothing here, LoadSCENGame() is called elsewhere
			case LSS_SAVE_SCEN:		loadsavescreen_SaveSCENGame();		break;
			default: 
				Assert(0); 
				break;
		}
		loadsavescreen_PostCleanupAction();
	}
}

/////////////////////////////////////////////////////////////

void loadsavescreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	// FIXED EMPTY CONTROLLED STATEMENT
	// ILD. 6/2000.
//	if(loadsavescreen_removeMyWindow(action)) ;
	loadsavescreen_removeMyWindow(action) ;

	loadsavescreen_PostCleanupAction();
}

void loadsavescreen_delete( void )
{
	GameInfo	*gameInfo = g_loadsaveWindow->GetGameInfo();
	SaveInfo	*saveInfo = g_loadsaveWindow->GetSaveInfo();

	Assert(gameInfo);
	if (!gameInfo) return;

//	Assert(saveInfo);
//	if (!saveInfo) return;

	if(saveInfo)
	{
		MBCHAR		path[_MAX_PATH];

		sprintf(path, "%s%s%s", gameInfo->path, FILE_SEP, saveInfo->fileName);

#ifdef WIN32
		if ( DeleteFile( path ) )
#else
		if (!unlink(path))
#endif
		{
			// FIXME ? Do we want to worry about deleting .gw files?

			// Refill list two.
			sint32 one = g_loadsaveWindow->GetListOne()->GetSelectedItemIndex();
			sint32 two = g_loadsaveWindow->GetListTwo()->GetSelectedItemIndex();

			g_loadsaveWindow->SetType( g_loadsaveWindow->GetType() );

			g_loadsaveWindow->GetListOne()->SelectItem( one );

			if ( two && two == g_loadsaveWindow->GetListTwo()->NumItems() ) --two;
			g_loadsaveWindow->GetListTwo()->SelectItem( two );

	// This wasn't working.
	//  		aui_Item *item = g_loadsaveWindow->GetListTwo()->GetSelectedItem();
	//  		if ( item )
	//  		{
	//  			g_loadsaveWindow->GetListTwo()->RemoveItem( item->Id() );
	//  			delete item;
	//  		}
		}
		else
		{
			Assert( "Couldn't delete file." == 0 );
		}
	}
	else
	{
#ifdef WIN32
		MBCHAR		path[_MAX_PATH];
		int fileHandle;

		sprintf(path, "%s%s*.*", FILE_SEP, gameInfo->path);

		_finddata_t findData;
		fileHandle=_findfirst(path,&findData);
		while(fileHandle)
		{
			sprintf(path, "%s%s%s", gameInfo->path, FILE_SEP, findData.name);
			DeleteFile(path);
			if(_findnext(fileHandle,&findData))
			{
				_findclose(fileHandle);
				fileHandle=0;
			}
		}
		sprintf(path, "%s", gameInfo->path);
		int retval=_rmdir(path);
		assert(!retval);
		g_loadsaveWindow->FillListTwo(NULL);
		g_loadsaveWindow->SetType(g_loadsaveWindow->GetType());
#endif
	}
}

void loadsavescreen_deletePress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	MBCHAR s[_MAX_PATH];
	sprintf( s, g_theStringDB->GetNameStr("DELETE_SAVE_CONFIRM") );

	if(g_loadsaveWindow->GetGameInfo()) {	
		MessageBoxDialog::Query( s, "ConfirmLoadSaveDelete", loadsavescreen_deleteDialog );
	}
}

/////////////////////////////////////////////////////////////

void loadsavescreen_ListOneHandler(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	// We only care about select actions
	if ( action != (uint32)AUI_LISTBOX_ACTION_SELECT ) return;

	c3_ListBox	*list = (c3_ListBox *)control;
	if (list == NULL) return;

	// check if there is a selected item in the list box
	LSGamesListItem *item = (LSGamesListItem *)list->GetSelectedItem();
	if (item == NULL)
	{
		g_loadsaveWindow->SetGameInfo(NULL);

		// Force the default names to come back.
		g_loadsaveWindow->SetType( g_loadsaveWindow->GetType() );

		g_loadsaveWindow->GetDeleteButton()->Enable( FALSE );
	}
	else
	{
		// Set the current game info
		g_loadsaveWindow->SetGameInfo(item->GetGameInfo());

		g_loadsaveWindow->GetDeleteButton()->Enable( TRUE );
	}

	if ( !g_loadsaveWindow->GetListTwo()->GetSelectedItem() )
	{
		switch ( g_loadsaveWindow->GetType() )
		{
		case LSS_LOAD_GAME:
		case LSS_LOAD_MP:
		case LSS_LOAD_SCEN:
		case LSS_LOAD_SCEN_MP:
			g_loadsaveWindow->GetOkButton()->Enable( FALSE );
			break;

		default:
			g_loadsaveWindow->GetOkButton()->Enable( TRUE );
			break;
		}

//		g_loadsaveWindow->GetDeleteButton()->Enable( FALSE );
	}
}

/////////////////////////////////////////////////////////////

void loadsavescreen_ListTwoHandler(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	// We only care about select actions
	if ( action != (uint32)AUI_LISTBOX_ACTION_SELECT ) return;

	c3_ListBox	*list = (c3_ListBox *)control;
	if (list == NULL) return;

	// If another list item was previously selected, make sure to dump its
	// extended info
	tech_WLList<sint32> *lastList = list->GetSelectedListLastTime();
	for (uint32 i=0; i<lastList->L(); i++) {
		sint32 index = lastList->GetAtIndex(i);
		LSSavesListItem *oldItem = (LSSavesListItem *)list->GetItemByIndex(index);
		if (oldItem != NULL) {
			SaveInfo *oldSaveInfo = oldItem->GetSaveInfo();
			if (oldSaveInfo) {
				// Dump power graph
				oldSaveInfo->powerGraphWidth = 0;
				oldSaveInfo->powerGraphHeight = 0;
				if (oldSaveInfo->powerGraphData) {
					delete[] oldSaveInfo->powerGraphData;
					oldSaveInfo->powerGraphData = NULL;
				}

				// Dump radar map
				oldSaveInfo->radarMapWidth = 0;
				oldSaveInfo->radarMapHeight = 0;
				if (oldSaveInfo->radarMapData) {
					delete[] oldSaveInfo->radarMapData;
					oldSaveInfo->radarMapData = NULL;
				}
			
				// Set load type back to basic 
				oldSaveInfo->loadType = SAVEINFOLOAD_BASIC;	
			}
		}
	}

	// check if there is a selected item in the list box
	LSSavesListItem *item = (LSSavesListItem *)list->GetSelectedItem();
	if (item == NULL)
	{
		switch ( g_loadsaveWindow->GetType() )
		{
		case LSS_LOAD_GAME:
		case LSS_LOAD_MP:
		case LSS_LOAD_SCEN:
		case LSS_LOAD_SCEN_MP:
			g_loadsaveWindow->GetOkButton()->Enable( FALSE );
			break;

		default:
			g_loadsaveWindow->GetOkButton()->Enable( TRUE );
			break;
		}

		g_loadsaveWindow->SetSaveInfo(NULL);

		// Force the default names to come back.
		g_loadsaveWindow->SetType( g_loadsaveWindow->GetType() );

		g_loadsaveWindow->GetDeleteButton()->Enable( FALSE );
	}
	else
	{
		SaveInfo	*info = item->GetSaveInfo();
		if (info == NULL) return;

		// Set the current save info
		if (info->loadType == SAVEINFOLOAD_BASIC)
			GameFile::FetchExtendedSaveInfo(info->pathName, info);

		g_loadsaveWindow->SetSaveInfo(info);
		g_loadsaveWindow->GetOkButton()->Enable( TRUE );
		g_loadsaveWindow->GetDeleteButton()->Enable( TRUE );
	}
}

/////////////////////////////////////////////////////////////

void loadsavescreen_CivListHandler(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_LISTBOX_ACTION_SELECT ) 
		return;
}

/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void loadsavescreen_OverwriteCallback( bool response, void *data )
{
	if ( response ) {

		switch(g_loadsaveWindow->GetType()) {
			case LSS_SAVE_GAME:		loadsavescreen_SaveGame();			break;
			case LSS_SAVE_MP:		loadsavescreen_SaveMPGame();		break;
		}

		// FIXED EMPTY CONTROLLED STATEMENT
		// ILD. 6/2000.
	//	if(loadsavescreen_removeMyWindow((uint32)AUI_BUTTON_ACTION_EXECUTE)) ;
		loadsavescreen_removeMyWindow((uint32)AUI_BUTTON_ACTION_EXECUTE) ;

		loadsavescreen_PostCleanupAction();
	}
}

/////////////////////////////////////////////////////////////

BOOL loadsavescreen_CheckOverwrite( void )
{
	// EAS012199 - save the one we originally set out to save.
	GameInfo	*gameInfo = g_loadsaveWindow->GetGameInfo();
	SaveInfo		*saveInfo = g_loadsaveWindow->GetSaveInfoToSave();

	Assert( saveInfo != NULL );
	if ( !saveInfo ) return FALSE;

	if (!g_loadsaveWindow->GetGameName(saveInfo->gameName)) return FALSE;
	if (!g_loadsaveWindow->GetSaveName(saveInfo->fileName)) return FALSE;
	if (!g_loadsaveWindow->GetNote(saveInfo->note)) return FALSE;

	if ( gameInfo ) {
		PointerList<SaveInfo>::Walker *walker = new PointerList<SaveInfo>::Walker(gameInfo->files);

		while (walker->IsValid()) {
			SaveInfo *info = walker->GetObj();
			if ( !strcmp(info->fileName, saveInfo->fileName) ) {
				MBCHAR s[_MAX_PATH];
				sprintf( s, g_theStringDB->GetNameStr("SAVE_OVERWRITE") );
				//c3_TextMessage( s, k_UTILITY_TEXTMESSAGE_YESNO, loadsavescreen_OverwriteCallback );
				MessageBoxDialog::Query( s, "ConfirmSaveOverwrite", loadsavescreen_OverwriteCallback );
				delete walker;
				return TRUE;
			}
			walker->Next();
		}
		delete walker;
	}

	return FALSE;
}

