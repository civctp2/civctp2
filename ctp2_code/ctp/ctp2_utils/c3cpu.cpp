//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Retrieve CPU information through cpuinf32.dll.
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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Stubbed out all functionality to prevent crashes on some machines.
//
//----------------------------------------------------------------------------


#include "c3.h"
#include "c3cpu.h"

#if defined(ACTIVISION_ORIGINAL)

HINSTANCE hLibrary;				

WORD (FAR *lpfnwincpuid)();
WORD (FAR *lpfnwincpuidsupport)();
WORD (FAR *lpfnwincpuidext)();
DWORD (FAR *lpfnwincpufeatures)();
char * (FAR *lpfnwinvendorstring)();
struct TIME_STAMP (FAR *lpfnwinrdtsc)();
struct FREQ_INFO * (FAR *lpfnwincpuspeed)(int BSFclocks);


static WORD		g_cpuType;			
static MBCHAR	g_vendorString[13];	
static DWORD	g_features;			
static struct FREQ_INFO g_cpuSpeed;	
static int		g_cpuidSupport;		
static WORD		g_extensions;		
static MBCHAR	g_cpuString[_MAX_PATH];

void c3cpu_Initialize(void)
{
	g_cpuType = 0;
	g_features = 0;
	g_cpuidSupport = 0;
	g_extensions = 0;

	memset(g_cpuString, 0, sizeof(g_cpuString));
}

void c3cpu_Examine(void)
{
	MBCHAR buf[_MAX_PATH];

	if (LoadLibrary(CPUINFODLL)==NULL) {		
		if (LOBYTE(LOWORD(GetVersion()))<4) {
			return ;					
		}
		else {
			sprintf(buf, "%s not found", CPUINFODLL);
			MessageBox(NULL,buf,"Error", MB_ICONINFORMATION); 
			return ;					
		}
	}

	hLibrary=LoadLibrary(CPUINFODLL);
	
	lpfnwincpuspeed =		(struct FREQ_INFO * (__cdecl *)(int))GetProcAddress(hLibrary,"cpuspeed");
	lpfnwincpuid =			(WORD (__cdecl *)())GetProcAddress(hLibrary,"wincpuid");
	lpfnwincpuidsupport =	(WORD (__cdecl *)())GetProcAddress(hLibrary, "wincpuidsupport");
	lpfnwincpufeatures =	(DWORD (__cdecl *)())GetProcAddress(hLibrary, "wincpufeatures");
	lpfnwincpuidext =		(WORD (__cdecl *)())GetProcAddress(hLibrary,"wincpuidext");
	lpfnwinvendorstring =	(char * (__cdecl *)())GetProcAddress(hLibrary, "winvendorstring");

	g_extensions = (*lpfnwincpuidext)();
	g_features = (*lpfnwincpufeatures)();
	memcpy(&g_cpuSpeed, (void *)(*lpfnwincpuspeed)(0), sizeof(struct FREQ_INFO));
	g_cpuType = (*lpfnwincpuid)();
	g_cpuidSupport = (*lpfnwincpuidsupport)();
	strcpy(g_vendorString, (MBCHAR *)(*lpfnwinvendorstring)());

	FreeLibrary(hLibrary);
}


MBCHAR *c3cpu_GetString(void)
{
	MBCHAR		mmx[10];
	MBCHAR		cpu[40];
	MBCHAR		mhz[10];

	if (g_features & 0x00800000) 
		strcpy(mmx, "MMX");
	else
		strcpy(mmx, "");

	if (!strcmp(g_vendorString, "GenuineIntel")) {
		if ( g_cpuType & CLONE_MASK ) {
			
			sprintf(cpu,"Intel Clone #%x", g_cpuType);
		} else {
			if (g_cpuidSupport) {
				
				switch (g_cpuType) {
				case 5 : strcpy(cpu, "Intel Pentium");
					break;
				case 6 :
					switch ((g_extensions & 0x00F0) >> 4) {
					case 1:
						strcpy(cpu, "Intel Pentium Pro");
						break;
					case 3:
						strcpy(cpu, "Intel Pentium II, Model 3");
						break;
					case 5:
						strcpy(cpu, "Intel Pentium II, Model 5");
					}
				break;
				}
			} else {
				
				sprintf(cpu, "Clone #%x", g_cpuType);
			}
		}
	} else
	if (!strcmp(g_vendorString, "CyrixInstead")) {
		switch (g_cpuType) {
		case 4 : strcpy(cpu, "Cyrix MediaGX");
			break;
		case 5 :
			switch ((g_extensions & 0x00F0) >> 4) {
			case 2 :
				strcpy(cpu, "Cyrix 6x86(L)");
				break;
			case 4:
				strcpy(cpu, "Cyrix MediaGX");
				strcpy(mmx, "MMX");
				break;
			}
			break;
		case 6 : strcpy(cpu, "Cyrix 6x86MX");
			break;
		default :
			strcpy(cpu, "Cyrix");
			break;
		}
	} else 
	if (!strcmp(g_vendorString, "AuthenticAMD")) {
		switch (g_cpuType) {
		case 5 : strcpy(cpu, "AMD K5");
			break;
		case 6: strcpy(cpu, "AMD K6");
			break;
		default:
			strcpy(cpu, "AMD");
			break;
		}
	}


	sprintf(mhz, "%d", g_cpuSpeed.norm_freq);

	sprintf(g_cpuString, "CPU: %s %s %sMhz", cpu, mmx, mhz);

	return g_cpuString;
}

#else

// Stub versions of the above

MBCHAR g_cpuString[] = "";

void c3cpu_Initialize(void)
{
}

void c3cpu_Examine(void)
{
}

MBCHAR * c3cpu_GetString(void)
{
	return g_cpuString;
}

#endif
