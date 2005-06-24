/* 
Copyright (C) 1995-2001 Activision, Inc.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

/* Many parts modified from debug.c
 * 	Carl Chimes, September 1996
 * 	Gordon Moyes, Guarded Memory Checking April 1997
 */
#ifndef NO_NETWORK
#define NO_NETWORK /* needed to shield dp stuff in dpprivy.h */
#endif
#ifndef AEHMAP_PRIVATE
#define AEHMAP_PRIVATE
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "aeh.h"
#include "dpcrc32.h"
#include "../dp/dpprivy.h"
#include "../../h/eclock.h"

#ifdef _WIN32
#include <windows.h>
#include <errno.h>
#include <tlhelp32.h>
#include "detect.h"
#include "tlstuff.h"

#ifdef _DEBUG
#include <assert.h>
#include <crtdbg.h>
#endif
#endif

/* static definitions -- used for return values for module/module functions */
static char *kernel  = "(kernel)";  /* for when in supervisor space */
static char *unknown = "(unknown)"; /* for when cannot determine */
static char *bogus = "(addr too large)"; /* for when offset address is beyond reasonable function size */

#ifdef DEBUG_AEHEXCP
static int aehcurline;
static char aehcurfile[aeh_MAX_PATH];
/*--------------------------------------------------------------------------
 Record current line and file.  Used to help debug aeh-caused exceptions.
--------------------------------------------------------------------------*/
void aeh_SetCurrent(int line, char *file)
{
	aehcurline = line;
	strncpy(aehcurfile, file, aeh_MAX_PATH);
	aehDPRINT(("aeh_SetCurrent: recorded line %d file %s, time %d\n", line, file, eclock()));

#if 0
	/* Checking the heap goes so slowly, it's too expensive to do here.*/
#if defined(_WIN32) && defined(_DEBUG)
	assert(_CrtCheckMemory());
#endif
#endif
}
/*--------------------------------------------------------------------------
 Get most recently recorded line and file.  Used to help debug aeh-caused
 exceptions.
--------------------------------------------------------------------------*/
void aeh_GetCurrent(int *line, char **file)
{
	*line = aehcurline;
	*file = strdup(aehcurfile);
}
#endif

#ifndef aeh_IO_ONLY

#ifdef _WIN32

/* used by guarded header test and all stack retracers
 * traps access violation (page not readable) so that invalid pointers
 * can return FALSE instead of crashing the game
 * and stack retraces can gracefully stop at invalid locations
 */
LONG _cdecl MemoryAccessExceptionFilter (LPEXCEPTION_POINTERS ep)
{
	aeh_SetCurrent(__LINE__, __FILE__);
	if (!ep) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return (EXCEPTION_CONTINUE_SEARCH);
	}
	
	/* if access violation, simply have guarded handler return 'FALSE' */
	if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return (EXCEPTION_EXECUTE_HANDLER);
	}

	/* stack overflow is special case of access violation */
	if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return (EXCEPTION_EXECUTE_HANDLER);
	}

   	/* not an access violation - handle by enclosing handler */
	aeh_SetCurrent(__LINE__, __FILE__);
	return (EXCEPTION_CONTINUE_SEARCH);
}

/***********************************************************************
 ****  Static methods for finding modules involved during a crash   ****
 ***********************************************************************/

/* single entry in list of crash modules;
 * list is ordered in descending order from highest load address to lowest */
typedef struct aeh_module_entry_s{
	aeh_modfile_t aehmod;
	unsigned long start_addr;
	char *path;
	BOOLEAN bCrash;
	struct aeh_module_entry_s *next;
} aeh_module_entry_t;

/* first entry in list of crash modules (one with the highest load address) */
static aeh_module_entry_t *firstmod = NULL;

/*--------------------------------------------------------------------------
 Add an entry to modulelist with the given parameters
 Return: aeh_RES_NOMEM if a malloc fails
         aeh_RES_BAD if args are bad
         aeh_RES_OK on success
--------------------------------------------------------------------------*/
static int aehmod_add(char *mName, char *path, HMODULE hMod)
{
	aeh_module_entry_t *modentry;
	aeh_SetCurrent(__LINE__, __FILE__);
	if (!mName || !path || (hMod > (void *)aeh_WIN32_ADDR_LIMIT)) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return aeh_RES_BAD;
	}
	modentry = (aeh_module_entry_t *)malloc(sizeof(aeh_module_entry_t));
	if (!modentry) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return aeh_RES_NOMEM;
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	memset(modentry, 0, sizeof(aeh_module_entry_t));
	aeh_SetCurrent(__LINE__, __FILE__);
	modentry->aehmod.name = strdup(mName);
	modentry->path = strdup(path);
	if (!modentry->aehmod.name || !modentry->path) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return aeh_RES_NOMEM;
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	modentry->start_addr = (unsigned long)hMod;
	modentry->bCrash = FALSE;
	aeh_SetCurrent(__LINE__, __FILE__);
	if (!firstmod) {
		modentry->next = NULL;
		firstmod = modentry;
	} else {
		aeh_module_entry_t *modbuf = firstmod;
		aeh_module_entry_t *modprev = NULL;
		do {
			if (modentry->start_addr > modbuf->start_addr) {
				if (modprev)
					modprev->next = modentry;
				else
					firstmod = modentry;
				modentry->next = modbuf;
				break;
			}
			modprev = modbuf;
			modbuf = modbuf->next;
		} while (modbuf != NULL);
		aeh_SetCurrent(__LINE__, __FILE__);
		if (modbuf == NULL) {	
			modprev->next = modentry;
			modentry->next = NULL;
		}
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	return aeh_RES_OK;
}

/* get all the modules loaded by the current process */
static int aehmod_Create()
{
	int err;
	MODULEENTRY32 mod;
	HANDLE hSnap;
	Toolhlp tool;

	aeh_SetCurrent(__LINE__, __FILE__);
	firstmod = NULL;
	if ((err = InitSnapshot(&tool)) != aeh_RES_OK) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return err;
	}
	hSnap = tool.pCreate32Snapshot(TH32CS_SNAPMODULE, 0);
	if (hSnap == (HANDLE)-1) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return aeh_RES_BUG;
	}
	mod.dwSize = sizeof(MODULEENTRY32);
	aeh_SetCurrent(__LINE__, __FILE__);
	if (tool.pModule32First(hSnap, &mod)) {
		if ((DWORD) mod.hModule < aeh_WIN32_ADDR_LIMIT) {
			err = aehmod_add(mod.szModule, mod.szExePath, mod.hModule);
			/* return RES_BUG for internal error on bad args to aehmod_add
			 * since the args were generated internally here */
			if (err == aeh_RES_BAD) err = aeh_RES_BUG;
			if (err != aeh_RES_OK) {
				aeh_SetCurrent(__LINE__, __FILE__);
				return err;
			}
		}
		aeh_SetCurrent(__LINE__, __FILE__);
		while (tool.pModule32Next(hSnap, &mod)) {
			if ((DWORD) mod.hModule < aeh_WIN32_ADDR_LIMIT) {
				err = aehmod_add(mod.szModule, mod.szExePath, mod.hModule);
				/* return RES_BUG for internal error on bad args to aehmod_add
				 * since the args were generated internally here */
				if (err == aeh_RES_BAD) err = aeh_RES_BUG;
				if (err != aeh_RES_OK) {
					aeh_SetCurrent(__LINE__, __FILE__);
					return err;
				}
			}
		}
	}
	CloseHandle(hSnap);
	aeh_SetCurrent(__LINE__, __FILE__);
	return aeh_RES_OK;
}

/* free memory allocated by module list */
void aehmod_Destroy()
{
	aeh_module_entry_t *modbuf = firstmod;
	aeh_module_entry_t *modnext;
	aeh_SetCurrent(__LINE__, __FILE__);
	__try {
		while (modbuf != NULL) {
			modnext = modbuf->next;
			if (modbuf->path)
				free(modbuf->path);
			if (modbuf->aehmod.name)
				free(modbuf->aehmod.name);
			free(modbuf);
			modbuf = modnext;
		}
	} __except (MemoryAccessExceptionFilter (GetExceptionInformation())) {
		/* we don't really care if we can't free the memory */ ;
	}
	firstmod = NULL;
	aeh_SetCurrent(__LINE__, __FILE__);
}

/* get the array index (starting from 1) in the modulelist of the module at
 * address caller; returns 0 if not found */
static int aehmod_PosGet(unsigned caller, BOOLEAN crshadd)
{
	int ipos = 1;

	aeh_SetCurrent(__LINE__, __FILE__);
	if (caller > aeh_WIN32_ADDR_LIMIT) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return 0;
	}
	if (firstmod) {
		__try {
		aeh_module_entry_t *modbuf = firstmod;
		do {
			aeh_SetCurrent(__LINE__, __FILE__);
			if (caller > modbuf->start_addr) {
				aeh_SetCurrent(__LINE__, __FILE__);
				if (crshadd && modbuf->bCrash == FALSE) {
					/* calc crc here since don't want to calc unless necessary*/
					unsigned long crc;
					FILE *f;

					aeh_SetCurrent(__LINE__, __FILE__);
					if (!modbuf->path || !(f = fopen(modbuf->path, "rb"))) {
						aehDPRINT(("aehmod_PosGet: null %s\n", modbuf->path));
						aeh_SetCurrent(__LINE__, __FILE__);
						return 0;
					}
					aeh_SetCurrent(__LINE__, __FILE__);
					crc = dp_fil_crc32(f);
					fclose(f);
					modbuf->aehmod.crc = crc;
					modbuf->bCrash = TRUE;
				}
				aeh_SetCurrent(__LINE__, __FILE__);
				return ipos;
			}
			aeh_SetCurrent(__LINE__, __FILE__);
			modbuf = modbuf->next;
			ipos++;
		} while (modbuf != NULL);
		} __except (MemoryAccessExceptionFilter (GetExceptionInformation())) {
			return 0;
		}
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	return 0;
}

/* get the module name for the module at address caller */
static char *aehmod_NameGet(unsigned caller)
{
	aeh_SetCurrent(__LINE__, __FILE__);
	if (caller > aeh_WIN32_ADDR_LIMIT) return (kernel);
	if (firstmod) {
		aeh_module_entry_t *modbuf = firstmod;
		do {
			if (caller > modbuf->start_addr) {
				aeh_SetCurrent(__LINE__, __FILE__);
				return modbuf->aehmod.name;
			}
			modbuf = modbuf->next;
		} while (modbuf != NULL);
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	return (unknown);
}

#endif  /* ifdef WIN32 */

/***********************************************************************
 ********************** Methods acting on aeh_t ************************
 ***********************************************************************/

#define MAX_FUNC_SIZE 100000 /* use to determine if load address is too large */
/*------------------------------------------------------------------------
 Fill a modfunc with info from corresponding mapfile.
 Returns aeh_RES_OK on success,
 		 aeh_RES_BAD/BUG/NOMEM on various errors.
------------------------------------------------------------------------*/
static int aeh_modfunc_Fill(aeh_modfunc_t *modfunc, aeh_mapcat_t *aehmapcat, aeh_stack_entry_t *stk, int mapindex)
{
	aeh_map_t *mapbuf;
	aeh_map_func_t *pointer;
	aeh_SetCurrent(__LINE__, __FILE__);
	if (!modfunc || !stk || !aehmapcat || !(aehmapcat->map) ||
		((mapindex >= 0) && (mapindex >= aehmapcat->nmap))) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return aeh_RES_BAD;
	}
	if (stk->mod_index == 0 || mapindex < 0) {
		if (stk->offset_addr > aeh_WIN32_ADDR_LIMIT) {
			modfunc->load_addr = stk->offset_addr;
			modfunc->offset_addr = 0;
			modfunc->mappath = strdup(unknown);
			modfunc->name = strdup(kernel);
			if (!modfunc->mappath || !modfunc->name) {
				aeh_SetCurrent(__LINE__, __FILE__);
				return aeh_RES_NOMEM;
			}
			aeh_SetCurrent(__LINE__, __FILE__);
			return aeh_RES_OK;
		} else {
			if (stk->mod_index == 0)
				modfunc->load_addr = stk->offset_addr;
			else
				modfunc->load_addr = 0;
			modfunc->offset_addr = 0;
			modfunc->mappath = strdup(unknown);
			modfunc->name = strdup(unknown);
			if (!modfunc->mappath || !modfunc->name) {
				aeh_SetCurrent(__LINE__, __FILE__);
				return aeh_RES_NOMEM;
			}
			aeh_SetCurrent(__LINE__, __FILE__);
			return aeh_RES_OK;
		}
	}

	/* address is within a function if it is after the beginning of the
	 * function */
	mapbuf = (aehmapcat->map)+mapindex;
	pointer = mapbuf->firstfunc;
	if (!pointer)
		aehDPRINT(("aeh_modfunc_Fill: no mapfile loaded for entry %d in stack trace\n", mapindex));
	if (mapbuf->reldir) {
		while (pointer) {
			if (stk->offset_addr >= pointer->start_addr) {
				if (!pointer->name) break;
				if ((pointer == mapbuf->firstfunc) &&
					(stk->offset_addr - pointer->start_addr > MAX_FUNC_SIZE)) {
					modfunc->load_addr = 0;
					modfunc->offset_addr = 0;
					modfunc->name = strdup(bogus);
				} else {
					modfunc->load_addr = mapbuf->load_addr + stk->offset_addr;
					modfunc->offset_addr = stk->offset_addr - pointer->start_addr;
					modfunc->name = strdup(pointer->name);
				}
				modfunc->mappath = strdup(mapbuf->reldir);
				if (!modfunc->mappath || !modfunc->name) {
					aeh_SetCurrent(__LINE__, __FILE__);
					return aeh_RES_NOMEM;
				}
				aeh_SetCurrent(__LINE__, __FILE__);
				return aeh_RES_OK;
			}
			pointer = pointer->next;
		}
	}

	/* address is before any known symbol */
	modfunc->load_addr = 0;
	modfunc->offset_addr = 0;
	modfunc->mappath = strdup(unknown);
	modfunc->name = strdup(unknown);
	if (!modfunc->mappath || !modfunc->name) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return aeh_RES_NOMEM;
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	return aeh_RES_OK;
}

/* Given aeh and index in stack trace, return corresponding module name */
static char *aeh_StkModName(const aeh_t *aeh, const unsigned int stk_index)
{
	aeh_SetCurrent(__LINE__, __FILE__);
	if (!aeh || (aeh->stk[stk_index].mod_index < 0) ||
		(aeh->stk[stk_index].mod_index > aeh->nmod)) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return NULL;
	}
	if (aeh->stk[stk_index].mod_index == 0) {
		if (aeh->stk[stk_index].offset_addr > aeh_WIN32_ADDR_LIMIT) {
			aeh_SetCurrent(__LINE__, __FILE__);
			return (kernel);
		} else {
			aeh_SetCurrent(__LINE__, __FILE__);
			return (unknown);
		}
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	return (aeh->mod + (aeh->stk[stk_index].mod_index-1))->name;
}

/* Given aeh and index in stack trace, return corresponding module crc */
static unsigned long aeh_StkModCrc(const aeh_t *aeh, const unsigned int stk_index)
{
  	if (!aeh || (aeh->stk[stk_index].mod_index < 0) ||
		(aeh->stk[stk_index].mod_index > aeh->nmod) ||
		(aeh->stk[stk_index].mod_index == 0)) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return 0;
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	return (aeh->mod + (aeh->stk[stk_index].mod_index-1))->crc;
}

#ifdef _WIN32

/* Set data into aeh's stack and module array list */
static int aeh_setStackandModInfo(aeh_t *aeh)
{
	int err;
	unsigned int i, j;
	aeh_module_entry_t *modbuf = firstmod;

	aeh_SetCurrent(__LINE__, __FILE__);
	if (!aeh || !modbuf) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return aeh_RES_BAD;
	}
	aeh->nmod = 0;
	aeh->mod = (aeh_modfile_t *)malloc(aeh_BUF_MAXLEN);
	if (!aeh->mod) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return aeh_RES_NOMEM;
	}
	memset(aeh->mod, 0, aeh_BUF_MAXLEN);
	aeh_SetCurrent(__LINE__, __FILE__);
	/* find out about as many modules as we can. */
	err = aeh_RES_OK;
	__try {
		for (i = 0; modbuf != NULL; i++, modbuf = modbuf->next) {
			aeh_SetCurrent(__LINE__, __FILE__);
			if (!modbuf->bCrash)
				continue;
			aeh_SetCurrent(__LINE__, __FILE__);
			aeh->mod[aeh->nmod].crc = modbuf->aehmod.crc;
			aeh->mod[aeh->nmod].name = strdup(modbuf->aehmod.name);
			if (!aeh->mod[aeh->nmod].name) {
				err = aeh_RES_NOMEM;
				break;
			}
			aeh->nmod++;
			aeh_SetCurrent(__LINE__, __FILE__);
			for (j = 0; j < aeh->nstk; j++) {
				if (aeh->stk[j].mod_index == i + 1) {
					aeh->stk[j].mod_index = aeh->nmod;
					aeh->stk[j].offset_addr -= modbuf->start_addr;
				}
			}
			aeh_SetCurrent(__LINE__, __FILE__);
			
			if ((aeh->nmod + 1) * sizeof(aeh_modfile_t) > aeh_BUF_MAXLEN)
				break;
		}
	} __except (MemoryAccessExceptionFilter (GetExceptionInformation())) {
		err = aeh_RES_BUG;
	}
	/* Fix the rest of the mod_indices if we terminated early (full buf) */
	if ((err != aeh_RES_OK) || (modbuf != NULL)) {
		aehDPRINT(("aeh_setStackandModInfo: terminated early err:%d\n", err));
		for (j = 0; j < aeh->nstk; j++) {
			if (aeh->stk[j].mod_index > aeh->nmod) {
				aeh->stk[j].mod_index = 0;
				aeh->stk[j].offset_addr = 0;
			}
		}
	}
	/* clipping the excess of the buffer isn't really needed...
	 * aeh_SetCurrent(__LINE__, __FILE__);
	 * buffer = realloc(aeh->mod, aeh->nmod * sizeof(aeh_modfile_t));
	 * if (buffer)
	 *   aeh->mod = buffer;
	 */
	aeh_SetCurrent(__LINE__, __FILE__);
	return err;
}

/* get stack trace and modules involved in crash */
static int aeh_StackandModDump(LPEXCEPTION_POINTERS ep, FILE *f, aeh_t *aeh)
{
	unsigned base_pointer;   /* base of stack frame pointer */
	unsigned frame_limit;    /* limit of this stack frame (actually start of next frame) */
	unsigned caller;         /* address of statement to return to */
	unsigned int mod_pos;
	char *mod_name;
	aeh_stack_entry_t *buffer;
	int finished;
	aeh->nstk = 0;

	aeh_SetCurrent(__LINE__, __FILE__);
	if (!aeh || !ep) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return aeh_RES_BAD;
	}
	aeh->stk = (aeh_stack_entry_t *)malloc(aeh_BUF_MAXLEN);
	if (!aeh->stk) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return aeh_RES_NOMEM;
	}
	memset(aeh->stk, 0, aeh_BUF_MAXLEN);
	base_pointer = (DWORD) ep->ContextRecord->Ebp;

	/* Get the function we are currently in */
	aeh_SetCurrent(__LINE__, __FILE__);
	mod_pos = aehmod_PosGet(ep->ContextRecord->Eip, TRUE);
	aeh->stk->mod_index = mod_pos;
	aeh->stk->offset_addr = ep->ContextRecord->Eip;
	if (f) {
		mod_name = aehmod_NameGet(ep->ContextRecord->Eip);
		fprintf(f, "%x %s\n", (DWORD) ep->ContextRecord->Eip, mod_name);
	}
	aeh->nstk++;

	/*
	 * Keep retracing until the bottom of stack is reached
	 */
	aeh_SetCurrent(__LINE__, __FILE__);
	finished = 0;
	while (!finished) {
		/* find address of caller */
		__try {
			caller = *((unsigned *) (base_pointer + 4));
		} __except (MemoryAccessExceptionFilter (GetExceptionInformation())) {
			finished = 1;
			aehDPRINT(("aeh_StackandModDump: got memory access exception for base_pointer %x\n", (unsigned long)(base_pointer + 4)));
		}

		/* we are done if caller is the windows system */
		if (caller >= aeh_WIN32_ADDR_LIMIT) {
			finished = 1;
		}

		/* ok, print out this stack frame */
		aeh_SetCurrent(__LINE__, __FILE__);
		if (!finished) {
			mod_pos = aehmod_PosGet(caller, TRUE);
			((aeh->stk)+aeh->nstk)->mod_index = mod_pos;
			((aeh->stk)+aeh->nstk)->offset_addr = caller;
			if (f) {
				mod_name = aehmod_NameGet(caller);
				fprintf(f, "%x %s\n", caller, mod_name);
			}
			frame_limit = *((unsigned *)base_pointer);  /* beginning of next stack frame */
			base_pointer = frame_limit;
			aeh->nstk++;
		}
		/* if (aeh->nstk >= aeh_MAX_STACK_DEPTH) */
		if ((aeh->nstk + 1) * sizeof(aeh_stack_entry_t) > aeh_BUF_MAXLEN)
			finished = 1;
	}

	buffer = realloc(aeh->stk, aeh->nstk * sizeof(aeh_stack_entry_t));
	if (buffer)
		aeh->stk = buffer;
	aeh_SetCurrent(__LINE__, __FILE__);
	return aeh_setStackandModInfo(aeh); /* get the real addr offset and index */
}

void getExceptionInstruction(long segreg, long pinstr, unsigned char *instr)
{
	__asm {
		push gs
		push edi
		push esi
		mov cx, aeh_NINSTB 
		mov esi, pinstr
		mov edi, instr
		mov eax, segreg
		mov gs, ax
		rep movs byte ptr es:[edi], byte ptr gs:[esi]
		pop esi
		pop edi
		pop gs
	}
}

/*--------------------------------------------------------------------------
 Get crucial crash info.  Called by an exception handler during a crash.
 Call aeh_Destroy() after finished using the returned value of aeh.
 Input: ep     (exception pointer; can be NULL)
        aehapp (app info gotten for instance by dpReadAnetInf(); can be NULL)
        systemDesc (info on user's system; can be NULL)
		assertln   (line number of assertion failure; can be NULL);
		assertfile (file where assertion failure occurred; can be NULL);
		asserttxt  (text of assertion failure; can be NULL);
 Output: aeh (preallocated)
 Return: aeh_RES_NOMEM if memory couldn't be allocated
         aeh_RES_ALREADY if exception caused by previous call to aeh_Create
         aeh_RES_BAD if couldn't get call stack info
         aeh_RES_OK on success
--------------------------------------------------------------------------*/
int aeh_Create(aeh_t *aeh, const LPEXCEPTION_POINTERS ep, const aeh_appParam_t *aehapp, const char *systemDesc, const int *assertln, const char *assertfile, const char *asserttxt)

#else  /* ifdef _WIN32 */
int aeh_Create(aeh_t *aeh, const void *ep, const aeh_appParam_t *aehapp, const char *systemDesc, const int *assertln, const char *assertfile, const char *asserttxt)

#endif /* if not _WIN32 */
{
	static short bPrevExc = 0;

	/* see if there were any previous exceptions thrown before this one; if so,
	 * the handler is generating an exception (possibly here) so just return */
	aeh_SetCurrent(__LINE__, __FILE__);
	if (!aeh) {
		aehDPRINT(("aeh_Create: null aeh\n"));
		aeh_SetCurrent(__LINE__, __FILE__);
		return aeh_RES_BAD;
	}
	if (!ep && !assertln) {
		aehDPRINT(("aeh_Create: null ep and assert parameters\n"));
		aeh_SetCurrent(__LINE__, __FILE__);
		return aeh_RES_BAD;
	}
	memset(aeh, 0, sizeof(aeh_t));
	if (bPrevExc) {
		if (bPrevExc == 1) {
			aeh->retcode = aeh_EXCEPTION_CODE;
			aeh_SetCurrent(__LINE__, __FILE__);
			return aeh_RES_OK;
		} else {
			aeh_SetCurrent(__LINE__, __FILE__);		
			return aeh_RES_ALREADY;
		}
	}
	bPrevExc++;

#ifdef _WIN32
	/* find loaded module addresses and do stack trace */
	aeh_SetCurrent(__LINE__, __FILE__);
	if (ep) {
		int err;
		int infoind;
		FILE *flog = NULL;		/* log file in ASCII form */
		if (aehmod_Create() != aeh_RES_OK) {
			aehDPRINT(("aeh_Create:can't find loaded modules\n"));
			bPrevExc--;
			aeh_SetCurrent(__LINE__, __FILE__);
			return aeh_RES_BAD;
		}

		aeh_SetCurrent(__LINE__, __FILE__);
		aeh->retcode = ep->ExceptionRecord->ExceptionCode;
		aeh->retflag = ep->ExceptionRecord->ExceptionFlags;
		aeh->retaddr = (unsigned long)ep->ExceptionRecord->ExceptionAddress;

		if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION
			&& ep->ExceptionRecord->NumberParameters >= 2)
			aeh->ninfo = 3;
		else
			aeh->ninfo = 2;

		aeh_SetCurrent(__LINE__, __FILE__);
		aeh->info = (aeh_info_t *)malloc(aeh->ninfo * sizeof(aeh_info_t));
		if (!aeh->info) {
			aeh_SetCurrent(__LINE__, __FILE__);
			return aeh_RES_NOMEM;
		}
		memset(aeh->info, 0, sizeof(aeh_info_t));
		infoind = 0;
		if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION
			&& ep->ExceptionRecord->NumberParameters >= 2) {
			unsigned char *temp;
			aeh->info[infoind].id = aeh_WIN32afl_ID;
			temp = (unsigned char *)malloc(sizeof(unsigned char));
			if (temp == NULL) {
				aeh_SetCurrent(__LINE__, __FILE__);
				return aeh_RES_NOMEM;
			}
			memset(temp, 0, sizeof(unsigned char));
			*temp = (unsigned char)(ep->ExceptionRecord->ExceptionInformation[0] & 0xff);
			(unsigned char *)(aeh->info[infoind].data) = temp;
			infoind++;
		}

		aeh_SetCurrent(__LINE__, __FILE__);
		aeh->info[infoind].id = aeh_WIN32info_ID;
		(aeh_WIN32info_t *)(aeh->info[infoind].data) = (aeh_WIN32info_t *)malloc(sizeof(aeh_WIN32info_t));
		if (!aeh->info[infoind].data) {
			aeh_SetCurrent(__LINE__, __FILE__);
			return aeh_RES_NOMEM;
		}
		memset(aeh->info[infoind].data, 0, sizeof(aeh_WIN32info_t));
		((aeh_WIN32info_t *)(aeh->info[infoind].data))->eax = ep->ContextRecord->Eax;
		((aeh_WIN32info_t *)(aeh->info[infoind].data))->ebx = ep->ContextRecord->Ebx;
		((aeh_WIN32info_t *)(aeh->info[infoind].data))->ecx = ep->ContextRecord->Ecx;
		((aeh_WIN32info_t *)(aeh->info[infoind].data))->edx = ep->ContextRecord->Edx;
		((aeh_WIN32info_t *)(aeh->info[infoind].data))->esi = ep->ContextRecord->Esi;
		((aeh_WIN32info_t *)(aeh->info[infoind].data))->edi = ep->ContextRecord->Edi;
		((aeh_WIN32info_t *)(aeh->info[infoind].data))->ebp = ep->ContextRecord->Ebp;
		((aeh_WIN32info_t *)(aeh->info[infoind].data))->eip = ep->ContextRecord->Eip;
		((aeh_WIN32info_t *)(aeh->info[infoind].data))->esp = ep->ContextRecord->Esp;
		((aeh_WIN32info_t *)(aeh->info[infoind].data))->segCs = ep->ContextRecord->SegCs;
		((aeh_WIN32info_t *)(aeh->info[infoind].data))->segSs = ep->ContextRecord->SegSs;
		((aeh_WIN32info_t *)(aeh->info[infoind].data))->segDs = ep->ContextRecord->SegDs;
		((aeh_WIN32info_t *)(aeh->info[infoind].data))->segEs = ep->ContextRecord->SegEs;
		((aeh_WIN32info_t *)(aeh->info[infoind].data))->segFs = ep->ContextRecord->SegFs;
		((aeh_WIN32info_t *)(aeh->info[infoind].data))->segGs = ep->ContextRecord->SegGs;
		((aeh_WIN32info_t *)(aeh->info[infoind].data))->contextFlags = ep->ContextRecord->ContextFlags;
		if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION
			&& ep->ExceptionRecord->NumberParameters >= 2)
			((aeh_WIN32info_t *)(aeh->info[infoind].data))->access_addr = ep->ExceptionRecord->ExceptionInformation[1];
		infoind++;

		/* This is buggy - crashes most of the time */
#if 0
		__try {
			unsigned char *temp;
			
			aeh_SetCurrent(__LINE__, __FILE__);
			aeh->info[infoind].id = aeh_WIN32inst_ID;
			temp = (unsigned char *)malloc(aeh_NINSTB * sizeof(unsigned char));
			aeh_SetCurrent(__LINE__, __FILE__);
			if (temp == NULL) {
				aeh_SetCurrent(__LINE__, __FILE__);
				return aeh_RES_NOMEM;
			}
			memset(temp, 0, aeh_NINSTB * sizeof(unsigned char));
			aeh_SetCurrent(__LINE__, __FILE__);
			getExceptionInstruction(ep->ContextRecord->SegCs, ep->ContextRecord->Eip, temp);
			aeh_SetCurrent(__LINE__, __FILE__);
			(unsigned char *)(aeh->info[infoind].data) = temp;
			aeh_SetCurrent(__LINE__, __FILE__);
			infoind++;
		} __except (MemoryAccessExceptionFilter (GetExceptionInformation())) {
			aehDPRINT(("aeh_Create: got memory access exception while getting instruction bytes\n"));
			free(aeh->info[infoind].data);
			aeh->ninfo--;
		}
#else
		/* God what a kludge.  ninfo should be set to infoind at the end. */
		aeh->ninfo--;
#endif

		aeh_SetCurrent(__LINE__, __FILE__);
		if ((DWORD) ep->ContextRecord->Ebp < aeh_WIN32_ADDR_LIMIT)
			err = aeh_StackandModDump(ep, flog, aeh);
		else
			err = aeh_RES_OK;
		aehmod_Destroy();
		if (err != aeh_RES_OK) {
			aehDPRINT(("aeh_Create: stack dump failed, err:%d\n", err));
			aeh_Destroy(aeh);
			bPrevExc--;
			aeh_SetCurrent(__LINE__, __FILE__);
			return err;
		}
	}
#endif

	/* set assertion info */
	aeh_SetCurrent(__LINE__, __FILE__);
	if (assertln) {
		if (!aeh->retcode) aeh->retcode = aeh_ASSERTION_CODE;
		aeh->assertln = *assertln;
		if (assertfile) {
			aeh->assertfile = strdup(assertfile);
			if (!aeh->assertfile) {
				aeh_SetCurrent(__LINE__, __FILE__);
				return aeh_RES_NOMEM;
			}
		}
		if (asserttxt) {
			aeh->asserttxt = strdup(asserttxt);
			if (!aeh->assertfile) {
				aeh_SetCurrent(__LINE__, __FILE__);
				return aeh_RES_NOMEM;
			}
		}
	}

	/* set system info */
	aeh_SetCurrent(__LINE__, __FILE__);
	if (systemDesc) {
		aeh_SetCurrent(__LINE__, __FILE__);
		if (strlen(systemDesc)) {
			char *temp;
			aeh_SetCurrent(__LINE__, __FILE__);
			temp = strdup(systemDesc);
			if (!temp) {
				aeh_SetCurrent(__LINE__, __FILE__);
				return aeh_RES_NOMEM;
			}
			aeh_SetCurrent(__LINE__, __FILE__);
			aeh->systemDesc = temp;
		}
	}

	/* set application info */
	aeh_SetCurrent(__LINE__, __FILE__);
	if (aehapp)
		aeh->app = *aehapp;

	/* modfunc gets determined by analysis program; so do nothing here */
	bPrevExc--;
	aeh_SetCurrent(__LINE__, __FILE__);
	return aeh_RES_OK;
}

#endif /* ifndef aeh_IO_ONLY */

/*--------------------------------------------------------------------------
 Free memory allocated to aeh during aeh_Create() or aeh_readInputStream().
 Input: aeh
--------------------------------------------------------------------------*/
void aeh_Destroy(aeh_t *aeh)
{
	unsigned int i;
	aeh_SetCurrent(__LINE__, __FILE__);
	if (aeh->stk) {
		if (aeh->modfunc) {
			for (i = 0; i < aeh->nstk; i++) {
				if (aeh->modfunc[i].name)
					free(aeh->modfunc[i].name);
				if (aeh->modfunc[i].mappath)
					free(aeh->modfunc[i].mappath);
			}
			free(aeh->modfunc);
		}
		free(aeh->stk);
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	if (aeh->mod) {
		for (i = 0; i < aeh->nmod; i++) {
			if (aeh->mod[i].name)
				free(aeh->mod[i].name);
		}
		free(aeh->mod);
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	if (aeh->assertfile)
		free(aeh->assertfile);
	aeh_SetCurrent(__LINE__, __FILE__);
	if (aeh->asserttxt)
		free(aeh->asserttxt);
	aeh_SetCurrent(__LINE__, __FILE__);
	if (aeh->systemDesc)
		free(aeh->systemDesc);
	aeh_SetCurrent(__LINE__, __FILE__);
	if (aeh->info) {
		for (i = 0; i < aeh->ninfo; i++) {
			switch(aeh->info[i].id) {
				case aeh_WIN32afl_ID:
					aeh_SetCurrent(__LINE__, __FILE__);
					free(aeh->info[i].data);
					break;
				case aeh_WIN32info_ID:
					aeh_SetCurrent(__LINE__, __FILE__);
					free(aeh->info[i].data);
					break;
				case aeh_WIN32inst_ID:
					aeh_SetCurrent(__LINE__, __FILE__);
					free(aeh->info[i].data);
					break;
				case aeh_COMMENT_ID:
					aeh_SetCurrent(__LINE__, __FILE__);
					free(aeh->info[i].data);
					break;
				default:
					aehDPRINT(("aeh_Destroy: unknown info id %d", aeh->info[i].id));	
					break;
			}
		}
		aeh_SetCurrent(__LINE__, __FILE__);
		free(aeh->info);
	}
	aeh_SetCurrent(__LINE__, __FILE__);
}

/*--------------------------------------------------------------------------
 Get a nearly unique 4 byte identifier for this crash, based on the stack
 trace.
 If a typical game has 2^12 bugs, the chance of an id collision is
 2^(2*12 - 1)/2^32 = 1/2^9.
 On error or empty stack, returns 0.
--------------------------------------------------------------------------*/
unsigned long aeh_getSignature(const aeh_t *aeh)
{
	unsigned long crc = 0xffffffff;
	unsigned int i;

	if (!aeh) {
		aehDPRINT(("aeh_getSignature: null aeh\n"));
		return 0;
	}
	if ((aeh->nstk < 1) || (aeh->nstk > aeh_MAX_STACK_DEPTH)) {
		aehDPRINT(("aeh_getSignature: nstk:%d not in range [1,%d]\n", aeh->nstk,
			aeh_MAX_STACK_DEPTH));
		return 0;
	}
	for (i = 0; i < aeh->nstk; i++) {
		unsigned int mod_index = aeh->stk[i].mod_index;
		if (mod_index == 0)
			continue;	/* probably kernel */
		if (mod_index > aeh->nmod) {
			aehDPRINT(("aeh_getSignature: stk[%d].mod_index:%d > nmod:%d\n",
				i, mod_index, aeh->nmod));
			return 0;
		}
		crc = dp_crc32_inc((char *)&aeh->mod[mod_index-1].crc, sizeof(long), crc);
		crc = dp_crc32_inc((char *)&aeh->stk[i].offset_addr, sizeof(long), crc);
	}
	return ~crc;	/* Return one's complement as per crc32 spec */
}

/*--------------------------------------------------------------------------
 Print out an aeh signature to text string.
 Input: signature (from getSignature)
 Output: 9 byte string plus null termination
--------------------------------------------------------------------------*/
void aeh_signature_toString(unsigned long crc, char *buf)
{
	sprintf(buf, "%04X-%04X", (unsigned)((crc >> 16) & 0xffff), (unsigned)(crc & 0xffff));
}

/*--------------------------------------------------------------------------
 Add a comment field to an aeh.
 Returns aeh_RES_OK on success.
--------------------------------------------------------------------------*/
int aeh_addComment(aeh_t *aeh, const char *comment)
{
	char *tmp;

	if (!aeh || !comment)
		return aeh_RES_BAD;

	/* stuff the comment into the info list */
	if (!aeh->ninfo) {
		aeh->info = (aeh_info_t *)malloc(sizeof(aeh_info_t));
		if (aeh->info == NULL) {
			aehDPRINT(("aeh_addComment: malloc(ninfo->1) failed\n"));
			return aeh_RES_NOMEM;
		}
	} else {
		void *tmp = realloc(aeh->info, (aeh->ninfo + 1)*sizeof(aeh_info_t));
		if (tmp == NULL) {
			aehDPRINT(("aeh_addComment: realloc(ninfo->%d) failed\n", aeh->ninfo + 1));
			return aeh_RES_NOMEM;
		}
		aeh->info = (aeh_info_t *)tmp;
	}
	aeh->info[aeh->ninfo].id = aeh_COMMENT_ID;
	tmp = (void *)strdup(comment);
	if (!tmp) {
		aehDPRINT(("aeh_addComment: strdup failed\n"));
		return aeh_RES_NOMEM;
	}
	aeh->info[aeh->ninfo].data = tmp;
	aeh->ninfo++;
	return aeh_RES_OK;
}

#if 0
/*--------------------------------------------------------------------------
 Get the nth comment from aeh.
 Usually there is only one comment and an n=1 call will suffice.
 No more than len characters are written to comment.
 Returns aeh_RES_OK on success,
		 aeh_RES_EMPTY if there is no nth comment.
--------------------------------------------------------------------------*/
int aeh_getComment(aeh_t *aeh, char *comment, int len, int n)
{
	int i;
	int nComments = 0;

	if (!aeh || !comment)
		return aeh_RES_BAD;
	if (!aeh->ninfo)
		return aeh_RES_EMPTY;
	for (i = 0; i < aeh->ninfo; i++) {
		if ((aeh->info[i].id == aeh_COMMENT_ID)
		&&  (++nComments == n)) {
			strncpy(comment, (char *)aeh->info[i].data, len);
			comment[len - 1] = 0;
			return aeh_RES_OK;
		}
	}
	return aeh_RES_EMPTY;
}

/*--------------------------------------------------------------------------
 Get the name of the last (deepest) known module in the stack trace.
 No more than len characters are written to name.
 Returns aeh_RES_OK on success,
		 aeh_RES_EMPTY if the stack trace contains no known modules.
--------------------------------------------------------------------------*/
int aeh_getLastModule(aeh_t *aeh, char *name, int len)
{
	int i;
	int mod_index;

	if (!aeh || !name)
		return aeh_RES_BAD;
	if (!aeh->nstk || !aeh->nmod)
		return aeh_RES_EMPTY;
	for (i = 0; i < aeh->nstk; i++) {
		mod_index = aeh->stk[i].mod_index;
		if ((mod_index > 0) && (mod_index <= aeh->nmod)) {
			strncpy(name, aeh->mod[mod_index - 1].name, len);
			name[len - 1] = 0;
			return aeh_RES_OK;
		}
	}
	return aeh_RES_EMPTY;
}
#endif

#ifndef aeh_IO_ONLY

/*--------------------------------------------------------------------------
 Evaluate crash data in aeh along with info in corresponding map files to
  get the module function crash stack.
 Map file paths are stored in a catalog file indexed by module crc and
  selected by matching crc to that of the crash modules.
 Input: aeh (not detailed; like when gotten from aeh_readInputStream())
		aehmapcat (gotten by previous call to aeh_map_Create())
 Output: aeh (detailed -- contain module function crash stack)
 Return: aeh_RES_NOMEM if memory couldn't be allocated
         aeh_RES_EMPTY if module array or stack size is zero
         aeh_RES_BAD if bad arguments
         aeh_RES_OK on success
--------------------------------------------------------------------------*/
int aeh_getAllInfo(aeh_t *aeh, aeh_mapcat_t *aehmapcat)
{
	unsigned int i;
	aeh_SetCurrent(__LINE__, __FILE__);
	if (!aeh || !aehmapcat) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return aeh_RES_BAD;
	}
	if (aeh->nmod == 0) {
		aehDPRINT(("aeh_getAllInfo: nmod 0\n"));
		aeh_SetCurrent(__LINE__, __FILE__);
		return aeh_RES_EMPTY;
	}
	if (!aeh->nstk || !(aeh->modfunc = malloc(aeh->nstk * sizeof(aeh_modfunc_t)))) {
		aehDPRINT(("aeh_getAllInfo: nstk(%d) or can't allocate memory for aeh->modfunc", aeh->nstk));
		aeh_SetCurrent(__LINE__, __FILE__);
		return (!aeh->nstk) ? aeh_RES_EMPTY : aeh_RES_NOMEM;
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	memset(aeh->modfunc, 0, aeh->nstk * sizeof(aeh_modfunc_t));
	/* get corresponding map files and load its function info */
	for (i = 0; i < aeh->nstk; i++) {
		int err;
		int index;
		int modind = aeh->stk[i].mod_index - 1;
		if (modind < 0)
			index = -1;
		else if (modind >= aeh->nmod) {
			aehDPRINT(("aeh_getAllInfo: bug: mod_index %d > aeh->nmod %d\n",
				aeh->stk[i].mod_index, aeh->nmod));
			index = -1;
		} else
			index = aeh_map_Find(aehmapcat, aeh->mod+modind);
		err = aeh_modfunc_Fill((aeh->modfunc)+i, aehmapcat, aeh->stk+i, index);
		if (err != aeh_RES_OK) {
			aehDPRINT(("aeh_getAllInfo: aeh_modfunc_Fill returns err:%d\n", err));
			return err;
		}
	}
	aehDPRINT(("exit aeh_getAllInfo time %d\n", eclock()));
	aeh_SetCurrent(__LINE__, __FILE__);
	return aeh_RES_OK;
}


static int parseDispCardInfo(const char *dispCard, char *szName, char *szDriver, unsigned long *VersionMS, unsigned long *VersionLS)
{
	const char *pbuf = dispCard;
	const char *ptr = pbuf;
	aeh_SetCurrent(__LINE__, __FILE__);
	pbuf = strchr(ptr, ',');
	if (!pbuf) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return aeh_RES_EMPTY;
	}
	memcpy(szName, ptr, (pbuf - ptr));
	szName[(pbuf - ptr)] = '\0';
	ptr = ++pbuf;
	pbuf = strchr(ptr, ',');
	if (!pbuf) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return aeh_RES_EMPTY;
	}
	memcpy(szDriver, ptr, (pbuf - ptr));
	szDriver[(pbuf - ptr)] = '\0';
	if (sscanf(pbuf, ",%lx,%lx", VersionMS, VersionLS) != 2) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return aeh_RES_EMPTY;
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	return aeh_RES_OK;
}
	
static char *parseSystemInfo(const char *sysDesc, char **crshtxt)
{
	unsigned long VersionMS, VersionLS;
	char buf[BUFFER_SIZE], buftxt[BUFFER_SIZE];
	char szName[BUFFER_SIZE], szDriver[BUFFER_SIZE];
	const char *pbuf = sysDesc;
	const char *ptr = pbuf;
	char *retStr = NULL;
	aeh_SetCurrent(__LINE__, __FILE__);
	buf[0] = '\0';
	*crshtxt = NULL;
	if (pbuf && (ptr = strstr(pbuf, "@@##"))) {
		memcpy(buftxt, sysDesc, ptr - sysDesc);
		buftxt[(ptr - sysDesc) + 1] = '\0';
		*crshtxt = strdup(buftxt);
		if (!crshtxt) {
			aeh_SetCurrent(__LINE__, __FILE__);
			return NULL;
		}
		pbuf = ptr + 4;
	}
	while (pbuf) {
		int err;
		err = parseDispCardInfo(pbuf, szName, szDriver, &VersionMS, &VersionLS);
		if (err != aeh_RES_OK) break;
		if (VersionMS || VersionLS) {
			sprintf(&buf[strlen(buf)], " %s: %s (%d.%d.%d.%d)\n",szName,szDriver,
					(unsigned int)((VersionMS>>16)&0xffff),
					(unsigned int)(VersionMS&0xffff),
					(unsigned int)((VersionLS>>16)&0xffff),
					(unsigned int)(VersionLS&0xffff));
		} else
			sprintf(&buf[strlen(buf)], " %s: %s (unknown version)\n", szName,
					szDriver);
		ptr = pbuf;
		pbuf = strchr(ptr, '|');
		if (pbuf) pbuf++;
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	if ((buf[0] == '\0' && pbuf) || (pbuf = strchr(ptr, ';'))) {
		if (buf[0]) pbuf++;
		sprintf(&buf[strlen(buf)], " %s\n", pbuf);
	}
	if (strlen(buf))
		retStr = strdup(buf);
	aeh_SetCurrent(__LINE__, __FILE__);
	return retStr;	/* NULL if NOMEM */
}

void codetostring(int retcode, char *desc)
{
	switch (retcode) {
		case EXCEPTION_ACCESS_VIOLATION:
			strcpy(desc, "Access violation");
			break;
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			strcpy(desc, "Array bounds exceeded");
			break;
		case EXCEPTION_BREAKPOINT:
			strcpy(desc, "Breakpoint");
			break;
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			strcpy(desc, "Datatype misalignment");
			break;
		case EXCEPTION_FLT_DENORMAL_OPERAND:
			strcpy(desc, "Float denormal operand");
			break;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			strcpy(desc, "Float divide by zero");
			break;
		case EXCEPTION_FLT_INEXACT_RESULT:
			strcpy(desc, "Float inexact result");
			break;
		case EXCEPTION_FLT_INVALID_OPERATION:
			strcpy(desc, "Float invalid operation");
			break;
		case EXCEPTION_FLT_OVERFLOW:
			strcpy(desc, "Float overflow");
			break;
		case EXCEPTION_FLT_STACK_CHECK:
			strcpy(desc, "Float stack check");
			break;
		case EXCEPTION_FLT_UNDERFLOW:
			strcpy(desc, "Float underflow");
			break;
		case EXCEPTION_GUARD_PAGE:
			strcpy(desc, "Guard page");
			break;
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			strcpy(desc, "Illegal instruction");
			break;
		case EXCEPTION_IN_PAGE_ERROR:
			strcpy(desc, "In page error");
			break;
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			strcpy(desc, "Integer divide by zero");
			break;
		case EXCEPTION_INT_OVERFLOW:
			strcpy(desc, "Integer overflow");
			break;
		case EXCEPTION_INVALID_DISPOSITION:
			strcpy(desc, "Invalid disposition");
			break;
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			strcpy(desc, "Noncontinuable exception");
			break;
		case EXCEPTION_PRIV_INSTRUCTION:
			strcpy(desc, "Priv instruction");
			break;
		case EXCEPTION_SINGLE_STEP:
			strcpy(desc, "Single step");
			break;
		case EXCEPTION_STACK_OVERFLOW:
			strcpy(desc, "Stack overflow");
			break;
		default:
			sprintf(desc, "Unknown exception; code %x", retcode);
			break;
	}
}

/*--------------------------------------------------------------------------
 Print out aeh info to text string.
 Input: aeh (can be detailed or not detailed)
        len (length of aehDesc allocated)
 Output: aehDesc
         len (actual length of aehDesc)
 Return: length needed to completely write out info
--------------------------------------------------------------------------*/
int aeh_toString(const aeh_t *aeh, char *aehDesc, unsigned int *len)
{
	unsigned int i;
	char tmp[BUFFER_SIZE + 1];
	long crc;
	aeh_SetCurrent(__LINE__, __FILE__);
	tmp[0] = '\0';
	aehDPRINT(("enter aeh_toString time %d\n", eclock()));
	if (!aeh || !aehDesc || !len || !*len) return 0;
	if (aeh->retcode == aeh_EXCEPTION_CODE) {
		sprintf(&tmp[strlen(tmp)], "excDesc: aeh-caused exception\n");
		if (aeh->assertln) {
			sprintf(&tmp[strlen(tmp)], " line %d", aeh->assertln);
			if (aeh->assertfile && strlen(aeh->assertfile))
				sprintf(&tmp[strlen(tmp)], ", file %s", aeh->assertfile);
			sprintf(&tmp[strlen(tmp)], "\n");
		}
		if (*len > strlen(tmp) + 1)
			*len = strlen(tmp) + 1;
		strncpy(aehDesc, tmp, *len);
		aehDesc[*len] = '\0';
		aeh_SetCurrent(__LINE__, __FILE__);
		return (strlen(tmp) + 1);
	}
	if (aeh->retcode == aeh_ASSERTION_CODE) {
		sprintf(&tmp[strlen(tmp)], "excDesc: assertion failure");
		if (aeh->retflag)
			sprintf(&tmp[strlen(tmp)], ", retFlag:%lx", aeh->retflag);
		if (aeh->retaddr)
			sprintf(&tmp[strlen(tmp)], ", retAddr:%lx", aeh->retaddr);
		sprintf(&tmp[strlen(tmp)], "\n");
	} else {
		char exceptdesc[50];
		codetostring(aeh->retcode, exceptdesc);
		sprintf(&tmp[strlen(tmp)], "excDesc: %s, retFlag:%lx", exceptdesc, aeh->retflag);
		if (aeh->retaddr)
			sprintf(&tmp[strlen(tmp)], ", retAddr:%lx", aeh->retaddr);
		sprintf(&tmp[strlen(tmp)], "\n");
	}
	if (aeh->assertln) {
		sprintf(&tmp[strlen(tmp)], "assert: line %d", aeh->assertln);
		if (aeh->assertfile && strlen(aeh->assertfile))
			sprintf(&tmp[strlen(tmp)], ", file %s", aeh->assertfile);
		if (aeh->asserttxt && strlen(aeh->asserttxt))
			sprintf(&tmp[strlen(tmp)], ", text %s", aeh->asserttxt);
		sprintf(&tmp[strlen(tmp)], "\n");
	}
	if (aeh->ninfo) {
		for (i = 0; i < aeh->ninfo; i++) {
			if ((aeh->info[i].id == aeh_WIN32afl_ID) &&
				(aeh->retcode == EXCEPTION_ACCESS_VIOLATION)) {
				sprintf(&tmp[strlen(tmp)], " Access violation type: %s\n",
					*((unsigned char *)(aeh->info[i].data)) ? "write" : "read");
			} else if (aeh->info[i].id == aeh_WIN32info_ID) {
				if (aeh->retcode == EXCEPTION_ACCESS_VIOLATION) {
					sprintf(&tmp[strlen(tmp)], " Access violation addr: %lx\n",
						((aeh_WIN32info_t *)(aeh->info[i].data))->access_addr);
				}
				sprintf(&tmp[strlen(tmp)], "Reg: ax:%08lx bx:%08lx cx:%08lx dx:%08lx si:%08lx di:%08lx\n", ((aeh_WIN32info_t *)(aeh->info[i].data))->eax, ((aeh_WIN32info_t *)(aeh->info[i].data))->ebx, ((aeh_WIN32info_t *)(aeh->info[i].data))->ecx, ((aeh_WIN32info_t *)(aeh->info[i].data))->edx, ((aeh_WIN32info_t *)(aeh->info[i].data))->esi, ((aeh_WIN32info_t *)(aeh->info[i].data))->edi);
				sprintf(&tmp[strlen(tmp)], "Reg: bp:%08lx sp:%08lx ip:%08lx cs:%08lx ss:%08lx\n", ((aeh_WIN32info_t *)(aeh->info[i].data))->ebp, ((aeh_WIN32info_t *)(aeh->info[i].data))->esp, ((aeh_WIN32info_t *)(aeh->info[i].data))->eip, ((aeh_WIN32info_t *)(aeh->info[i].data))->segCs, ((aeh_WIN32info_t *)(aeh->info[i].data))->segSs);
				sprintf(&tmp[strlen(tmp)], "Reg: ds:%08lx es:%08lx fs:%08lx gs:%08lx ContextFlags:%08lx\n", ((aeh_WIN32info_t *)(aeh->info[i].data))->segDs, ((aeh_WIN32info_t *)(aeh->info[i].data))->segEs, ((aeh_WIN32info_t *)(aeh->info[i].data))->segFs, ((aeh_WIN32info_t *)(aeh->info[i].data))->segGs, ((aeh_WIN32info_t *)(aeh->info[i].data))->contextFlags);
			} else if (aeh->info[i].id == aeh_WIN32inst_ID) {
				int j;
				sprintf(&tmp[strlen(tmp)], "Bytes at cs:ip: ");
				for (j = 0; j < aeh_NINSTB; j++)
					sprintf(&tmp[strlen(tmp)], "%02x ", ((unsigned char *)(aeh->info[i].data))[j]);
				sprintf(&tmp[strlen(tmp)], "\n");
			} else if (aeh->info[i].id == aeh_COMMENT_ID) {
				sprintf(&tmp[strlen(tmp)], "User Comment:%s\n", (char *)aeh->info[i].data);
			}
		}
	}
	if (aeh->app.sessionType != 0)
		sprintf(&tmp[strlen(tmp)], "SessType: %d, Plat: %d, Lang: %d, Vers: %d.%d\n", aeh->app.sessionType, aeh->app.platform, aeh->app.language, aeh->app.major_version, aeh->app.minor_version);
	if (aeh->systemDesc && strlen(aeh->systemDesc)) {
		char *buf = NULL, *crshtxt;
		if ((buf = parseSystemInfo(aeh->systemDesc, &crshtxt))) {
			if (crshtxt && (strlen(tmp) + strlen(crshtxt) + 22 < BUFFER_SIZE))
				sprintf(&tmp[strlen(tmp)], "Caller Comment:\n %s\n", crshtxt);
			if (buf && (strlen(tmp) + strlen(buf) + 22 < BUFFER_SIZE))
				sprintf(&tmp[strlen(tmp)], "System Description:\n%s\n", buf);
			if (buf)
				free(buf);
			if (crshtxt)
				free(crshtxt);
		} else
			if (strlen(tmp) + strlen(aeh->systemDesc) + 22 < BUFFER_SIZE)
				sprintf(&tmp[strlen(tmp)], "System Description:\n%s\n", aeh->systemDesc);
	}
	crc = aeh_getSignature(aeh);
	strcat(tmp, "Stack CRC: ");
	aeh_signature_toString(crc, &tmp[strlen(tmp)]);
	strcat(tmp, "\n");
	if (aeh->nstk && aeh->nmod) {
		if (strlen(tmp) + 13 < BUFFER_SIZE)
			sprintf(&tmp[strlen(tmp)], "Call stack:\n");
		if (strlen(tmp) + 29 < BUFFER_SIZE)
			sprintf(&tmp[strlen(tmp)], " abs_addr  mod_off func_off\n");
		for (i = 0; i < aeh->nstk; i++) {
			unsigned long stkoffsetaddr, stkmodcrc;
			char *stkmodname;
			stkoffsetaddr = aeh->stk[i].offset_addr;
			stkmodname = aeh_StkModName(aeh, i);
			stkmodcrc = aeh_StkModCrc(aeh, i);
			if (!stkmodname) {
				aehDPRINT(("aeh_toString: serious error, null aeh\n"));
				stkmodname = unknown;
			}
			if (aeh->modfunc && (aeh->modfunc[i].load_addr == stkoffsetaddr))
				stkoffsetaddr = 0;
			if (aeh->modfunc&&aeh->modfunc[i].name&&aeh->modfunc[i].mappath) {
				if (strlen(stkmodname) + strlen(aeh->modfunc[i].name) +
					strlen(aeh->modfunc[i].mappath) + 34 +
					strlen(tmp) > BUFFER_SIZE) break;
				if (!strcmp(aeh->modfunc[i].name, unknown) &&
					strcmp(stkmodname, unknown))
					sprintf(&tmp[strlen(tmp)],
							" %8lx %8lx %8lx  %s(crc:%8lx)  %s  %s\n",
							aeh->modfunc[i].load_addr, stkoffsetaddr,
							aeh->modfunc[i].offset_addr, stkmodname, stkmodcrc,
							aeh->modfunc[i].name, aeh->modfunc[i].mappath);
				else
					sprintf(&tmp[strlen(tmp)], " %8lx %8lx %8lx  %s  %s  %s\n",
							aeh->modfunc[i].load_addr, stkoffsetaddr,
							aeh->modfunc[i].offset_addr, stkmodname,
							aeh->modfunc[i].name, aeh->modfunc[i].mappath);
			} else {
				if (strlen(stkmodname) + 44 + strlen(tmp) > BUFFER_SIZE) break;
				if (stkmodcrc) 
					sprintf(&tmp[strlen(tmp)], " %8x %8lx %8x  %s(crc:%8lx)\n",
							0, stkoffsetaddr, 0, stkmodname, stkmodcrc);
				else
					sprintf(&tmp[strlen(tmp)], " %8x %8lx %8x  %s\n",
							0, stkoffsetaddr, 0, stkmodname);
			}
		}
	}
	if (*len > strlen(tmp) + 1)
		*len = strlen(tmp) + 1;
	strncpy(aehDesc, tmp, *len);
	aehDesc[*len] = '\0';
	aehDPRINT(("exit aeh_toString time %d\n", eclock()));
	aeh_SetCurrent(__LINE__, __FILE__);
	return (strlen(tmp) + 1);
}

#endif /* ifndef aeh_IO_ONLY */

/***********************************************************************
 **** Methods to convert between aeh_t and aeh_buf_t(binary stream) ****
 ***********************************************************************/

static int putStream(unsigned char **p, const void *dat, unsigned int lendat, unsigned char *peos)
{
	aeh_SetCurrent(__LINE__, __FILE__);
	if ((peos < *p) || (peos - *p < lendat)) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return -1;
	}
	if (lendat) writeSwap((void**)p, dat, lendat);
	aeh_SetCurrent(__LINE__, __FILE__);
	return 0;
}

static int getStream(const unsigned char **p, void *dat, unsigned int lendat, const unsigned char *peos)
{
	aeh_SetCurrent(__LINE__, __FILE__);
	if ((peos < *p) || (peos - *p < lendat)) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return -1;
	}
	if (lendat) readSwap((const void**)p, dat, lendat);
	aeh_SetCurrent(__LINE__, __FILE__);
	return 0;
}

static int putCharStream(unsigned char **p, const char *dat, unsigned char *peos)
{
	unsigned int lendat = 0;
	aeh_SetCurrent(__LINE__, __FILE__);
	if ((peos < *p) || (peos - *p < sizeof(lendat))) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return -1;
	}
	if (dat) {
		lendat = strlen(dat);
		if (peos - *p < (int)(lendat + sizeof(lendat)))
			lendat = (peos - *p) - sizeof(lendat);
	}
	writeSwap((void**)p, &lendat, sizeof(lendat));
	if (lendat) {
		memcpy(*p, dat, lendat);
		*p += lendat;
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	return 0;
}

static int getCharStream(const unsigned char **p, char **dat, const unsigned char *peos)
{
	unsigned int lendat = 0;
	aeh_SetCurrent(__LINE__, __FILE__);
	if ((peos < *p) || (peos - *p < sizeof(lendat))) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return -1;
	}
	readSwap((const void**)p, &lendat, sizeof(lendat));
	if ((peos < *p) || (peos - *p < lendat)) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return -1;
	}
	if (lendat) {
		if (!(*dat = (char *)malloc(lendat + 1))) {
			aeh_SetCurrent(__LINE__, __FILE__);
			return aeh_RES_NOMEM;
		}
		memcpy(*dat, *p, lendat);
		(*dat)[lendat] = '\0';
		*p += lendat;
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	return 0;
}

static int putStreamErrFull(unsigned char *p, aeh_buf_t *aehbuf)
{
	aeh_SetCurrent(__LINE__, __FILE__);
	aehbuf->buflen = p - &(aehbuf->buf[0]);
	aeh_SetCurrent(__LINE__, __FILE__);
	return aeh_RES_FULL;
}

/*--------------------------------------------------------------------------
 Convert crash info from an object format (aeh_t) to a binary stream
  format (aeh_buf_t). Call this to put crash info into a compact form 
  for doing network or file operations.
 Input: aeh
 Output: aehbuf (preallocated)
 Return: aeh_RES_BAD if null arguments
         aeh_RES_FULL if info is greater than aehbuf buffer size
         aeh_RES_OK on success
--------------------------------------------------------------------------*/
int aeh_writeOutputStream(const aeh_t *aeh, aeh_buf_t *aehbuf)
{
	unsigned int i;
	unsigned char *pwrite, *pend;
	void *psave;
	aeh_SetCurrent(__LINE__, __FILE__);
	if (!aeh || !aehbuf) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return aeh_RES_BAD;
	}
	pwrite = aehbuf->buf;
	pend = aehbuf->buf + aeh_BUF_MAXLEN;
	if (putStream(&pwrite, &aeh->retcode, sizeof(aeh->retcode), pend)) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return putStreamErrFull(pwrite, aehbuf);
	}
	if (aeh->retcode == aeh_EXCEPTION_CODE)  {
		/* KLUDGE: assertln, assertfile can be set by dpReportCrash()
		 * to record latest file position when aeh-caused crash occurred */
		if (aeh->assertln) {
			if (putStream(&pwrite,&aeh->assertln,sizeof(aeh->assertln),pend) ||
				putCharStream(&pwrite,aeh->assertfile,pend)) {
				aeh_SetCurrent(__LINE__, __FILE__);
				return putStreamErrFull(pwrite, aehbuf);
			}
		}
		aehbuf->buflen = pwrite - &(aehbuf->buf[0]);
		aeh_SetCurrent(__LINE__, __FILE__);
		return aeh_RES_OK;
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	if (putStream(&pwrite, &aeh->retflag, sizeof(aeh->retflag), pend) ||
		putStream(&pwrite, &aeh->retaddr, sizeof(aeh->retaddr), pend) ||
		putStream(&pwrite, &aeh->assertln, sizeof(aeh->assertln), pend)) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return putStreamErrFull(pwrite, aehbuf);
	}
	if (aeh->assertln) {
		if (putCharStream(&pwrite, aeh->assertfile, pend) ||
			putCharStream(&pwrite, aeh->asserttxt, pend)) {
			aeh_SetCurrent(__LINE__, __FILE__);
			return putStreamErrFull(pwrite, aehbuf);
		}
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	psave = pwrite;
	if (putStream(&pwrite, &aeh->nstk, sizeof(aeh->nstk), pend)) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return putStreamErrFull(pwrite, aehbuf);
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	for (i = 0; i < aeh->nstk; i++) {
		unsigned char *ptr = pwrite;
		if (putStream(&pwrite, &aeh->stk[i].mod_index, sizeof(aeh->stk[i].mod_index), pend) || putStream(&pwrite, &aeh->stk[i].offset_addr, sizeof(aeh->stk[i].offset_addr), pend)) {
			writeSwap((void**)&psave, (void*)&i, sizeof(aeh->nstk));
			aeh_SetCurrent(__LINE__, __FILE__);
			return putStreamErrFull(ptr, aehbuf);
		}
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	psave = pwrite;
	if (putStream(&pwrite, &aeh->nmod, sizeof(aeh->nmod), pend)) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return putStreamErrFull(pwrite, aehbuf);
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	for (i = 0; i < aeh->nmod; i++) {
		unsigned char *ptr = pwrite;
		if (putStream(&pwrite, &aeh->mod[i].crc, sizeof(aeh->mod[i].crc), pend) || putCharStream(&pwrite, aeh->mod[i].name, pend)) {
			writeSwap((void**)&psave, (void*)&i, sizeof(aeh->nstk));
			aeh_SetCurrent(__LINE__, __FILE__);
			return putStreamErrFull(ptr, aehbuf);
		}
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	psave = pwrite;
	if (putCharStream(&pwrite, aeh->systemDesc, pend)) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return putStreamErrFull(pwrite, aehbuf);
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	if (putStream(&pwrite, &aeh->app.sessionType, sizeof(aeh->app.sessionType), pend)) return putStreamErrFull(pwrite, aehbuf);
	aeh_SetCurrent(__LINE__, __FILE__);
	if (aeh->app.sessionType) {
		if (putStream(&pwrite, (void*)&aeh->app.platform, sizeof(aeh->app.platform), pend)) {
			aeh_SetCurrent(__LINE__, __FILE__);
			return putStreamErrFull(pwrite, aehbuf);
		}
		if (putStream(&pwrite, &aeh->app.language, sizeof(aeh->app.language), pend)) {
			aeh_SetCurrent(__LINE__, __FILE__);
			return putStreamErrFull(pwrite, aehbuf);
		}
		if (putStream(&pwrite, &aeh->app.major_version, sizeof(aeh->app.major_version), pend)) {
			aeh_SetCurrent(__LINE__, __FILE__);
			return putStreamErrFull(pwrite, aehbuf);
		}
		if (putStream(&pwrite, &aeh->app.minor_version, sizeof(aeh->app.minor_version), pend)) {
			aeh_SetCurrent(__LINE__, __FILE__);
			return putStreamErrFull(pwrite, aehbuf);
		}
	}
	if (putStream(&pwrite, &aeh->ninfo, sizeof(aeh->ninfo), pend)) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return putStreamErrFull(pwrite, aehbuf);
	}
	for (i = 0; i < aeh->ninfo; i++) {
		if (putStream(&pwrite, &aeh->info[i].id, sizeof(aeh->info[i].id), pend)) {
			aeh_SetCurrent(__LINE__, __FILE__);
			return putStreamErrFull(pwrite, aehbuf);
		}
		switch(aeh->info[i].id) {
			case aeh_WIN32afl_ID:
				if (aeh->retcode == EXCEPTION_ACCESS_VIOLATION) {
					if (putStream(&pwrite, aeh->info[i].data, sizeof(unsigned char), pend)) {
						aeh_SetCurrent(__LINE__, __FILE__);
						return putStreamErrFull(pwrite, aehbuf);
					}
				}
				break;
			case aeh_WIN32info_ID:
				if (putStream(&pwrite, &(((aeh_WIN32info_t *)(aeh->info[i].data))->eax), sizeof(unsigned long), pend) ||
					putStream(&pwrite, &(((aeh_WIN32info_t *)(aeh->info[i].data))->ebx), sizeof(unsigned long), pend) ||
					putStream(&pwrite, &(((aeh_WIN32info_t *)(aeh->info[i].data))->ecx), sizeof(unsigned long), pend) ||
					putStream(&pwrite, &(((aeh_WIN32info_t *)(aeh->info[i].data))->edx), sizeof(unsigned long), pend) ||
					putStream(&pwrite, &(((aeh_WIN32info_t *)(aeh->info[i].data))->esi), sizeof(unsigned long), pend) ||
					putStream(&pwrite, &(((aeh_WIN32info_t *)(aeh->info[i].data))->edi), sizeof(unsigned long), pend) ||
					putStream(&pwrite, &(((aeh_WIN32info_t *)(aeh->info[i].data))->ebp), sizeof(unsigned long), pend) ||
					putStream(&pwrite, &(((aeh_WIN32info_t *)(aeh->info[i].data))->eip), sizeof(unsigned long), pend) ||
					putStream(&pwrite, &(((aeh_WIN32info_t *)(aeh->info[i].data))->esp), sizeof(unsigned long), pend) ||
					putStream(&pwrite, &(((aeh_WIN32info_t *)(aeh->info[i].data))->segCs), sizeof(unsigned long), pend) ||
					putStream(&pwrite, &(((aeh_WIN32info_t *)(aeh->info[i].data))->segSs), sizeof(unsigned long), pend) ||
					putStream(&pwrite, &(((aeh_WIN32info_t *)(aeh->info[i].data))->segDs), sizeof(unsigned long), pend) ||
					putStream(&pwrite, &(((aeh_WIN32info_t *)(aeh->info[i].data))->segEs), sizeof(unsigned long), pend) ||
					putStream(&pwrite, &(((aeh_WIN32info_t *)(aeh->info[i].data))->segFs), sizeof(unsigned long), pend) ||
					putStream(&pwrite, &(((aeh_WIN32info_t *)(aeh->info[i].data))->segGs), sizeof(unsigned long), pend) ||
					putStream(&pwrite, &(((aeh_WIN32info_t *)(aeh->info[i].data))->contextFlags), sizeof(unsigned long), pend)) {
					aeh_SetCurrent(__LINE__, __FILE__);
					return putStreamErrFull(pwrite, aehbuf);
				}
				if (aeh->retcode == EXCEPTION_ACCESS_VIOLATION) {
					if (putStream(&pwrite, &(((aeh_WIN32info_t *)(aeh->info[i].data))->access_addr), sizeof(unsigned long), pend)) {
						aeh_SetCurrent(__LINE__, __FILE__);
						return putStreamErrFull(pwrite, aehbuf);
					}
				}
				break;
			case aeh_WIN32inst_ID:
				if (pwrite + aeh_NINSTB > pend) {
					aeh_SetCurrent(__LINE__, __FILE__);
					return putStreamErrFull(pwrite, aehbuf);
				}
				memcpy(pwrite, (unsigned char *)(aeh->info[i].data), aeh_NINSTB);
				pwrite += aeh_NINSTB;
				break;
			case aeh_COMMENT_ID:
				if (putCharStream(&pwrite, aeh->info[i].data, pend)) {
					aeh_SetCurrent(__LINE__, __FILE__);
					return putStreamErrFull(pwrite, aehbuf);
				}
				break;
			default:
				aehDPRINT(("aeh_writeOutputStream: unknown info id %d", aeh->info[i].id));	
				break;
		}
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	aehbuf->buflen = pwrite - &(aehbuf->buf[0]);
	aeh_SetCurrent(__LINE__, __FILE__);
	return aeh_RES_OK;
}

/*--------------------------------------------------------------------------
 Convert crash info from a binary stream format (aeh_buf_t) to an object
  format (aeh_t). Call this to put compact format crash info from network or
  file operations into a easily useable form.
 Call aeh_Destroy() after finished using the returned value of aeh.
 Input: aehbuf
 Output: aeh (preallocated)
 Return: aeh_RES_BAD if null arguments
         aeh_RES_FULL if incomplete data read back (due to full aehbuf buffer)
         aeh_RES_OK on success
--------------------------------------------------------------------------*/
int aeh_readInputStream(aeh_t *aeh, const aeh_buf_t *aehbuf)
{
	int err, err1;
	unsigned int i;
	const unsigned char *pread, *pend;
	aeh_SetCurrent(__LINE__, __FILE__);
	if (!aeh || !aehbuf) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return aeh_RES_BAD;
	}
	aehDPRINT(("enter aeh_readInputStream time %d\n", eclock()));
	memset(aeh, 0, sizeof(aeh_t));
	err1 = (aehbuf->buflen == aeh_BUF_MAXLEN) ? aeh_RES_FULL : aeh_RES_BUG;
	pread = aehbuf->buf;
	pend = aehbuf->buf + aehbuf->buflen;
	if (getStream(&pread, &aeh->retcode, sizeof(aeh->retcode), pend)) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return err1;
	}
	if (aeh->retcode == aeh_EXCEPTION_CODE) {
		/* KLUDGE: assertln, assertfile can be set by dpReportCrash()
		 * to record latest file position when aeh-caused crash occurred */
		if (pread == pend) {  /* no additional info */
			aeh_SetCurrent(__LINE__, __FILE__);
			return aeh_RES_OK;
		}
		if (getStream(&pread, &aeh->assertln, sizeof(aeh->assertln), pend)) {
			aeh_SetCurrent(__LINE__, __FILE__);
			return err1;
		}
		if ((err = getCharStream(&pread, &aeh->assertfile, pend))) {
			aeh_SetCurrent(__LINE__, __FILE__);
			if (err == aeh_RES_NOMEM) return err;
			return err1;
		}
		return aeh_RES_OK;
	}
	if (getStream(&pread, &aeh->retflag, sizeof(aeh->retflag), pend) ||
		getStream(&pread, &aeh->retaddr, sizeof(aeh->retaddr), pend) ||
		getStream(&pread, &aeh->assertln, sizeof(aeh->assertln), pend)) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return err1;
	}
	if (aeh->assertln) {
		if ((err = getCharStream(&pread, &aeh->assertfile, pend)) ||
			(err = getCharStream(&pread, &aeh->asserttxt, pend))) {
			aeh_SetCurrent(__LINE__, __FILE__);
			if (err == aeh_RES_NOMEM) return err;
			return err1;
		}
	}
	if (getStream(&pread, &aeh->nstk, sizeof(aeh->nstk), pend)) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return err1;
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	if (aeh->nstk > 0) {
		if (!(aeh->stk = (aeh_stack_entry_t *)malloc(aeh->nstk*sizeof(aeh_stack_entry_t)))) {
			aeh_SetCurrent(__LINE__, __FILE__);
			return aeh_RES_NOMEM;
		}
		memset(aeh->stk, 0, aeh->nstk * sizeof(aeh_stack_entry_t));
		for (i = 0; i < aeh->nstk; i++) {
			if (getStream(&pread, &aeh->stk[i].mod_index, sizeof(aeh->stk[i].mod_index), pend) ||
				getStream(&pread, &aeh->stk[i].offset_addr, sizeof(aeh->stk[i].offset_addr), pend)) {
				aeh_SetCurrent(__LINE__, __FILE__);
				return err1;
			}
		}
	} else {
		aeh->nstk = 0;
		aeh->stk = NULL;
	}
	if (getStream(&pread, &aeh->nmod, sizeof(aeh->nmod), pend)) return err1;
	if (aeh->nmod > 0) {
		if (!(aeh->mod = (aeh_modfile_t*)malloc(aeh->nmod * sizeof(aeh_modfile_t)))) {
			aeh_SetCurrent(__LINE__, __FILE__);		
			return aeh_RES_NOMEM;
		}
		memset(aeh->mod, 0, aeh->nmod * sizeof(aeh_modfile_t));
		for (i = 0; i < aeh->nmod; i++) {
			if (getStream(&pread, &aeh->mod[i].crc, sizeof(aeh->mod[i].crc), pend)) {
				aeh_SetCurrent(__LINE__, __FILE__);
				return err1;
			}
			if ((err = getCharStream(&pread, &aeh->mod[i].name, pend))) {
				aeh_SetCurrent(__LINE__, __FILE__);
				if (err == aeh_RES_NOMEM) return err;
				return err1;
			}
		}
	} else {
		aeh->nmod = 0;
		aeh->mod = NULL;
	}
	if ((err = getCharStream(&pread, &aeh->systemDesc, pend))) {
		aeh_SetCurrent(__LINE__, __FILE__);
		if (err == aeh_RES_NOMEM) return err;
		return err1;
	}
	if (getStream(&pread, &aeh->app.sessionType, sizeof(aeh->app.sessionType), pend)) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return err1;
	}
	if (aeh->app.sessionType) {
		if (getStream(&pread, &aeh->app.platform, sizeof(aeh->app.platform), pend)) {
			aeh_SetCurrent(__LINE__, __FILE__);
			return err1;
		}
		if (getStream(&pread, &aeh->app.language, sizeof(aeh->app.language), pend)) {
			aeh_SetCurrent(__LINE__, __FILE__);
			return err1;
		}
		if (getStream(&pread, &aeh->app.major_version, sizeof(aeh->app.major_version), pend)) {
			aeh_SetCurrent(__LINE__, __FILE__);
			return err1;
		}
		if (getStream(&pread, &aeh->app.minor_version, sizeof(aeh->app.minor_version), pend)) {
			aeh_SetCurrent(__LINE__, __FILE__);
			return err1;
		}
	}
	/* if end of buffer, this is old record; return aeh_RES_OK */
	if (getStream(&pread, &aeh->ninfo, sizeof(aeh->ninfo), pend)) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return aeh_RES_OK;
	}
	if (aeh->ninfo > 0) {
		if (!(aeh->info = (aeh_info_t *)malloc(aeh->ninfo * sizeof(aeh_info_t)))) {
			aeh_SetCurrent(__LINE__, __FILE__);
			return aeh_RES_NOMEM;
		}
		aeh_SetCurrent(__LINE__, __FILE__);
		memset(aeh->info, 0, aeh->ninfo * sizeof(aeh_info_t));
		for (i = 0; i < aeh->ninfo; i++) {
			if (getStream(&pread, &aeh->info[i].id, sizeof(aeh->info[i].id), pend)) return err1;
			switch(aeh->info[i].id) {
				case aeh_WIN32afl_ID:
					if (aeh->retcode == EXCEPTION_ACCESS_VIOLATION) {
						if (!(aeh->info[i].data = (unsigned char *)malloc(sizeof(unsigned char)))) {
							aeh_SetCurrent(__LINE__, __FILE__);
							return aeh_RES_NOMEM;
						}
						memset(aeh->info[i].data, 0, sizeof(unsigned char));
						if (getStream(&pread, aeh->info[i].data, sizeof(unsigned char), pend)) {
							aeh_SetCurrent(__LINE__, __FILE__);
							return err1;
						}
					}
					break;
				case aeh_WIN32info_ID:
					if (!(aeh->info[i].data = (aeh_WIN32info_t *)malloc(sizeof(aeh_WIN32info_t)))) {
						aeh_SetCurrent(__LINE__, __FILE__);
						return aeh_RES_NOMEM;
					}
					memset(aeh->info[i].data, 0, sizeof(aeh_WIN32info_t));
					if (getStream(&pread, &(((aeh_WIN32info_t *)(aeh->info[i].data))->eax), sizeof(unsigned long), pend) ||
						getStream(&pread, &(((aeh_WIN32info_t *)(aeh->info[i].data))->ebx), sizeof(unsigned long), pend) ||
						getStream(&pread, &(((aeh_WIN32info_t *)(aeh->info[i].data))->ecx), sizeof(unsigned long), pend) ||
						getStream(&pread, &(((aeh_WIN32info_t *)(aeh->info[i].data))->edx), sizeof(unsigned long), pend) ||
						getStream(&pread, &(((aeh_WIN32info_t *)(aeh->info[i].data))->esi), sizeof(unsigned long), pend) ||
						getStream(&pread, &(((aeh_WIN32info_t *)(aeh->info[i].data))->edi), sizeof(unsigned long), pend) ||
						getStream(&pread, &(((aeh_WIN32info_t *)(aeh->info[i].data))->ebp), sizeof(unsigned long), pend) ||
						getStream(&pread, &(((aeh_WIN32info_t *)(aeh->info[i].data))->eip), sizeof(unsigned long), pend) ||
						getStream(&pread, &(((aeh_WIN32info_t *)(aeh->info[i].data))->esp), sizeof(unsigned long), pend) ||
						getStream(&pread, &(((aeh_WIN32info_t *)(aeh->info[i].data))->segCs), sizeof(unsigned long), pend) ||
						getStream(&pread, &(((aeh_WIN32info_t *)(aeh->info[i].data))->segSs), sizeof(unsigned long), pend) ||
						getStream(&pread, &(((aeh_WIN32info_t *)(aeh->info[i].data))->segDs), sizeof(unsigned long), pend) ||
						getStream(&pread, &(((aeh_WIN32info_t *)(aeh->info[i].data))->segEs), sizeof(unsigned long), pend) ||
						getStream(&pread, &(((aeh_WIN32info_t *)(aeh->info[i].data))->segFs), sizeof(unsigned long), pend) ||
						getStream(&pread, &(((aeh_WIN32info_t *)(aeh->info[i].data))->segGs), sizeof(unsigned long), pend) ||
						getStream(&pread, &(((aeh_WIN32info_t *)(aeh->info[i].data))->contextFlags), sizeof(unsigned long), pend)) {
						aeh_SetCurrent(__LINE__, __FILE__);
						return err1;
					}
					if (aeh->retcode == EXCEPTION_ACCESS_VIOLATION) {
						if (getStream(&pread, &(((aeh_WIN32info_t *)(aeh->info[i].data))->access_addr), sizeof(unsigned long), pend)) {
							aeh_SetCurrent(__LINE__, __FILE__);
							return err1;
						}
					}
					break;
				case aeh_WIN32inst_ID:
					if (!(aeh->info[i].data = (unsigned char *)malloc(aeh_NINSTB * sizeof(unsigned char)))) {
						aeh_SetCurrent(__LINE__, __FILE__);
						return aeh_RES_NOMEM;
					}
					memset(aeh->info[i].data, 0, aeh_NINSTB * sizeof(unsigned char));
					if (pread + aeh_NINSTB > pend) {
						aeh_SetCurrent(__LINE__, __FILE__);
						return err1;
					}
					memcpy((unsigned char *)(aeh->info[i].data), pread, aeh_NINSTB);
					pread += aeh_NINSTB;
					break;
				case aeh_COMMENT_ID:
					if ((err = getCharStream(&pread, (char **)&aeh->info[i].data, pend))) {
						aeh_SetCurrent(__LINE__, __FILE__);
						if (err == aeh_RES_NOMEM) return err;
						return err1;
					}
					break;
				default:
					aehDPRINT(("aeh_writeOutputStream: unknown info id %d", aeh->info[i].id));	
					break;
			}
		}
	} else {
		aeh->ninfo = 0;
		aeh->info = NULL;
	}
	aehDPRINT(("exit aeh_readInputStream time %d\n", eclock()));
	aeh_SetCurrent(__LINE__, __FILE__);
	return aeh_RES_OK;
}

/***********************************************************************
 ****          Methods to filter aeh_buf_t (binary stream)          ****
 ***********************************************************************/

/*--------------------------------------------------------------------------
 Strip system description from crash info. Takes aeh and returns it with
  null system description string.
 Input: aeh
 Output: aeh
 Return: aeh_RES_BAD if bad arguments
         aeh_RES_OK on success
--------------------------------------------------------------------------*/
int aeh_stripSysDesc(aeh_t *aeh)
{
	aeh_SetCurrent(__LINE__, __FILE__);
	if (!aeh) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return aeh_RES_BAD;
	}
	free(aeh->systemDesc);
	aeh->systemDesc = NULL;
	aeh_SetCurrent(__LINE__, __FILE__);
	return aeh_RES_OK;
}

/*--------------------------------------------------------------------------
 Strip selected info id's from crash info. Takes aeh and array and size of
  array of info id's to be removed and returns it without them.
 Input: aeh, nids, stripIds
 Output: aeh
 Return: aeh_RES_BAD if bad arguments
         aeh_RES_OK on success
--------------------------------------------------------------------------*/
int aeh_stripInfoId(aeh_t *aeh, unsigned int nids, unsigned int stripIds[])
{
	unsigned int n, n_left;
	
	aeh_SetCurrent(__LINE__, __FILE__);
	if (!aeh || !nids || !stripIds) {
		aeh_SetCurrent(__LINE__, __FILE__);
		return aeh_RES_BAD;
	}
	/* All id's not matching one in stripIds are moved to aeh->info[0-n_left] */
	for (n = 0, n_left = 0; n < aeh->ninfo; n++) {
		unsigned int j;

		for (j = 0; j < nids; j++) {
			if (aeh->info[n].id == stripIds[j])
				break;
		}
		if (j >= nids) {
			/* No match in stripIds, keep this one */
			aeh->info[n_left].id = aeh->info[n].id;
			aeh->info[n_left].data = aeh->info[n].data;
			n_left++;
		} /* else if aeh->info[n] has any malloc'd fields, free them now */
	}
	if (n_left == 0) {
		if (aeh->info)
			free(aeh->info);
		aeh->info = NULL;
		aeh->ninfo = 0;
	} else if (n_left < n) {
		aeh_info_t *temp;

		temp = (aeh_info_t *)realloc(aeh->info, n_left*sizeof(aeh_info_t));
		if (temp == NULL) {
			aeh_SetCurrent(__LINE__, __FILE__);
			return aeh_RES_NOMEM;
		}
		aeh->info = temp;
		aeh->ninfo = n_left;
	} /* else n_left == n and we don't need to do anything */
	aeh_SetCurrent(__LINE__, __FILE__);
	return aeh_RES_OK;
}

/*--------------------------------------------------------------------------
 Strip address info. Takes aeh and return it with its address field set to zero.
 Input: aeh
 Output: aeh
 Return: aeh_RES_BAD if bad arguments
         aeh_RES_OK on success
--------------------------------------------------------------------------*/
int aeh_stripExceptionAddress(aeh_t *aeh)
{
	if (!aeh) 
		return aeh_RES_BAD;
	aeh->retaddr = 0;
	return aeh_RES_OK;
}
