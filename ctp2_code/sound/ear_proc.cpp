















#include <windows.h>
#include "ear.h"

HMODULE EAR_DLL;

UINT WM_EAR_CALLBACK;






DPROCD		EAR_AAA_Validate;
DPROCD		EAR_AssignHwnd;
DPROCDD		EAR_ChangeChannelControl;
DPROCDD		EAR_ClearChannel;
DPROCV		EAR_EarInactive;
DPROCDD		EAR_GetEventStatus;
DPROCV		EAR_GetLastError;
DPROCV		EAR_GetVersion;
DPROCD		EAR_InitializeEar;
DPROCD		EAR_MixEvent;
DPROCD		EAR_MoveEvent;
DPROCDD		EAR_RegisterBank;
DPROCDDDD	EAR_RegisterChannel;
DPROCD		EAR_RegisterEnvironment;
DPROCD		EAR_RegisterMatrix;
DPROCDD		EAR_RegisterPreset;
VPROCV		EAR_ResetEar;
VPROCDD		EAR_SetAttenuationLevel;
VPROCDD		EAR_SetUserDistanceUnit;
VPROCV		EAR_ShowLastError;
DPROCD		EAR_ShutDownBank;
DPROCD		EAR_ShutDownChannel;
VPROCV		EAR_ShutDownEar;
DPROCD		EAR_ShutDownEnvironment;
DPROCDD		EAR_ShutDownEvent;
DPROCD		EAR_ShutDownMatrix;
DPROCD		EAR_ShutDownPreset;
DPROCD		EAR_StartEvent;
DPROCV		EAR_StartTimer;
VPROCV		EAR_ShutDownTimer;
VPROCV		EAR_UpdateEar;


BOOL gbEarLoadedLibrary = FALSE;
BOOL gbEarLoadedPDS		= FALSE;
BOOL gbExtLoadedPDS		= FALSE;


BOOL EP_RegisterEarServiceTable(BOOL bForceDirectSound)
 {	if (EAR_DLL != 0 && EAR_UpdateEar != 0)
		return TRUE;

	char dll_name[(sizeof(EARPDS_DLL_FILENAME) + 1)];

	if (bForceDirectSound)
	 {	gbExtLoadedPDS = TRUE;
		strcpy(dll_name, EARPDS_DLL_FILENAME);
	 }
	
	else
	 {	strcpy(dll_name, EARIAS_DLL_FILENAME);
	 }

	EAR_DLL = GetModuleHandle(dll_name);
	
	
	if (EAR_DLL == 0)
	 {	gbEarLoadedLibrary = TRUE;
		EAR_DLL = LoadLibrary(dll_name);

		
		if (EAR_DLL == 0 && !bForceDirectSound)
		 {	gbEarLoadedLibrary = FALSE;
			gbEarLoadedPDS	   = TRUE;

			EAR_DLL = GetModuleHandle(EARPDS_DLL_FILENAME);

			
			if (EAR_DLL == 0)
			 {	gbEarLoadedLibrary = TRUE;
				EAR_DLL = LoadLibrary(EARPDS_DLL_FILENAME);

				
				if (EAR_DLL == 0)
					return FALSE;

			 } 

		 } 

	 } 

	if (EAR_UpdateEar != 0)
		return TRUE;

	BOOL failed = 0;

	failed = 
	(EAR_AAA_Validate = (DPROCD)GetProcAddress(EAR_DLL, "_EAR_DLL_AAA_Validate@4")) == NULL |
	(EAR_AssignHwnd = (DPROCD)GetProcAddress(EAR_DLL, "_EAR_DLL_AssignHwnd@4")) == NULL |
	(EAR_ChangeChannelControl = (DPROCDD)GetProcAddress(EAR_DLL, "_EAR_DLL_ChangeChannelControl@8")) == NULL |
	(EAR_ClearChannel = (DPROCDD)GetProcAddress(EAR_DLL, "_EAR_DLL_ClearChannel@8")) == NULL |
	(EAR_EarInactive = (DPROCV)GetProcAddress(EAR_DLL, "_EAR_DLL_EarInactive@0")) == NULL |
	(EAR_GetEventStatus = (DPROCDD)GetProcAddress(EAR_DLL, "_EAR_DLL_GetEventStatus@8")) == NULL |
	(EAR_GetLastError = (DPROCV)GetProcAddress(EAR_DLL, "_EAR_DLL_GetLastError@0")) == NULL |
	(EAR_GetVersion = (DPROCV)GetProcAddress(EAR_DLL, "_EAR_DLL_GetVersion@0")) == NULL |
	(EAR_InitializeEar = (DPROCD)GetProcAddress(EAR_DLL, "_EAR_DLL_InitializeEar@4")) == NULL |
	(EAR_MixEvent = (DPROCD)GetProcAddress(EAR_DLL, "_EAR_DLL_MixEvent@4")) == NULL |
	(EAR_MoveEvent = (DPROCD)GetProcAddress(EAR_DLL, "_EAR_DLL_MoveEvent@4")) == NULL |
	(EAR_RegisterBank = (DPROCDD)GetProcAddress(EAR_DLL, "_EAR_DLL_RegisterBank@8")) == NULL |
	(EAR_RegisterChannel = (DPROCDDDD)GetProcAddress(EAR_DLL, "_EAR_DLL_RegisterChannel@16")) == NULL |
	(EAR_RegisterEnvironment = (DPROCD)GetProcAddress(EAR_DLL, "_EAR_DLL_RegisterEnvironment@4")) == NULL |
	(EAR_RegisterMatrix = (DPROCD)GetProcAddress(EAR_DLL, "_EAR_DLL_RegisterMatrix@4")) == NULL |
	(EAR_RegisterPreset = (DPROCDD)GetProcAddress(EAR_DLL, "_EAR_DLL_RegisterPreset@8")) == NULL |
	(EAR_ResetEar = (VPROCV)GetProcAddress(EAR_DLL, "_EAR_DLL_ResetEar@0")) == NULL |
	(EAR_SetAttenuationLevel = (VPROCDD)GetProcAddress(EAR_DLL, "_EAR_DLL_SetAttenuationLevel@8")) == NULL |
	(EAR_SetUserDistanceUnit = (VPROCDD)GetProcAddress(EAR_DLL, "_EAR_DLL_SetUserDistanceUnit@8")) == NULL |
	(EAR_ShowLastError = (VPROCV)GetProcAddress(EAR_DLL, "_EAR_DLL_ShowLastError@0")) == NULL |
	(EAR_ShutDownBank = (DPROCD)GetProcAddress(EAR_DLL, "_EAR_DLL_ShutDownBank@4")) == NULL |
	(EAR_ShutDownChannel = (DPROCD)GetProcAddress(EAR_DLL, "_EAR_DLL_ShutDownChannel@4")) == NULL |
	(EAR_ShutDownEar = (VPROCV)GetProcAddress(EAR_DLL, "_EAR_DLL_ShutDownEar@0")) == NULL |
	(EAR_ShutDownEnvironment = (DPROCD)GetProcAddress(EAR_DLL, "_EAR_DLL_ShutDownEnvironment@4")) == NULL |
	(EAR_ShutDownEvent = (DPROCDD)GetProcAddress(EAR_DLL, "_EAR_DLL_ShutDownEvent@8")) == NULL |
	(EAR_ShutDownMatrix = (DPROCD)GetProcAddress(EAR_DLL, "_EAR_DLL_ShutDownMatrix@4")) == NULL |
	(EAR_ShutDownPreset = (DPROCD)GetProcAddress(EAR_DLL, "_EAR_DLL_ShutDownPreset@4")) == NULL |
	(EAR_StartEvent = (DPROCD)GetProcAddress(EAR_DLL, "_EAR_DLL_StartEvent@4")) == NULL |
	(EAR_StartTimer = (DPROCV)GetProcAddress(EAR_DLL, "_EAR_DLL_StartTimer@0")) == NULL |
	(EAR_ShutDownTimer = (VPROCV)GetProcAddress(EAR_DLL, "_EAR_DLL_ShutDownTimer@0")) == NULL |
	(EAR_UpdateEar = (VPROCV)GetProcAddress(EAR_DLL, "_EAR_DLL_UpdateEar@0")) == NULL;

	if (failed)
		return FALSE;
	
	
	WM_EAR_CALLBACK = RegisterWindowMessage(EAR_MONIKER);

	return TRUE;

 } 


VOID EP_ClearEarServiceTable(VOID)
 {	if (gbEarLoadedLibrary)
	 {	if (EAR_DLL == 0)
		 {	if (gbEarLoadedPDS || gbExtLoadedPDS)
				EAR_DLL = GetModuleHandle(EARPDS_DLL_FILENAME);
			else
				EAR_DLL = GetModuleHandle(EARIAS_DLL_FILENAME);
		 }
		
		if (EAR_DLL != 0)
			FreeLibrary(EAR_DLL);

	 } 

	EAR_DLL = 0;

	EAR_AAA_Validate = 0;
	EAR_AssignHwnd = 0;
	EAR_ChangeChannelControl = 0;
	EAR_ClearChannel = 0;
	EAR_EarInactive = 0;
	EAR_GetEventStatus = 0;
	EAR_GetLastError = 0;
	EAR_GetVersion = 0;
	EAR_InitializeEar = 0;
	EAR_MixEvent = 0;
	EAR_MoveEvent = 0;
	EAR_RegisterBank = 0;
	EAR_RegisterChannel = 0;
	EAR_RegisterEnvironment = 0;
	EAR_RegisterMatrix = 0;
	EAR_RegisterPreset = 0;
	EAR_ResetEar = 0;
	EAR_SetAttenuationLevel = 0;
	EAR_SetUserDistanceUnit = 0;
	EAR_ShowLastError = 0;
	EAR_ShutDownBank = 0;
	EAR_ShutDownChannel = 0;
	EAR_ShutDownEar = 0;
	EAR_ShutDownEvent = 0;
	EAR_ShutDownMatrix = 0;
	EAR_ShutDownPreset = 0;
	EAR_StartEvent = 0;
	EAR_StartTimer = 0 ;
	EAR_ShutDownTimer = 0;
	EAR_UpdateEar = 0;

	gbEarLoadedLibrary = 
	gbEarLoadedPDS = 
	gbExtLoadedPDS = FALSE;

 } 

