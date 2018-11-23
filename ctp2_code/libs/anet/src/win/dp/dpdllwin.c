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

/******************************************************************************

                  DLL Loading/Unloading Functions for DP (Win32)

                                    M. Livesay

                              (c) 1996 By Activision

  Notes:

   (1) This file replaces DPDLL.C for Win32 builds

  Revision Log:

    21-OCT-96    File Created [MSL]

 $Log: dpdllwin.c $
 Revision 1.10  1997/09/16 22:10:16  lbennett
 Removed unused variable.
 Revision 1.9  1997/09/01 18:33:20  dkegel
 Fixed crash bug if dll file did not exist!
 Revision 1.8  1997/07/28 02:41:58  dkegel
 Use dp_transport_t properly...
 Revision 1.7  1997/07/27 22:44:00  dkegel
 1. use dpio_transport_t properly to name transport DLL's.
 2. dpLoadDLL and dpUnloadDLL now passed dpio_t, not dp_t.
*******************************************************************************/

/******************************************************************************

                                    Literals

*******************************************************************************/

#include <windows.h>
#include <stddef.h>
#include <assert.h>

#include "dp.h"

#ifndef DYNALINK
  #define DYNALINK 1        // set to zero to test with static linking
#endif

/******************************************************************************

                                    Globals

*******************************************************************************/

static HINSTANCE hcommDLL = NULL;

#if DYNALINK
  // Hooks to DLL comm functions
  static int (* pcommNoOp      ) (commNoOpReq_t *,       commNoOpResp_t *      ) = NULL;
  static int (* pcommInit      ) (commInitReq_t *,       commInitResp_t *      ) = NULL;
  static int (* pcommTerm      ) (commTermReq_t *,       commTermResp_t *      ) = NULL;
  static int (* pcommDriverInfo) (commDriverInfoReq_t *, commDriverInfoResp_t *) = NULL;
  static int (* pcommPlayerInfo) (commPlayerInfoReq_t *, commPlayerInfoResp_t *) = NULL;
  static int (* pcommTxFull    ) (commTxFullReq_t *,     commTxFullResp_t *    ) = NULL;
  static int (* pcommTxPkt     ) (commTxPktReq_t *,      commTxPktResp_t *     ) = NULL;
  static int (* pcommPeekPkt   ) (commPeekPktReq_t *,    commPeekPktResp_t *   ) = NULL;
  static int (* pcommRxPkt     ) (commRxPktReq_t *,      commRxPktResp_t *     ) = NULL;
  static int (* pcommScanAddr  ) (commScanAddrReq_t *,   commScanAddrResp_t *  ) = NULL;
  static int (* pcommPrintAddr ) (commPrintAddrReq_t *,  commPrintAddrResp_t * ) = NULL;
  static int (* pcommGroupAlloc) (commGroupAllocReq_t *, commGroupAllocResp_t *) = NULL;
  static int (* pcommGroupFree ) (commGroupFreeReq_t *,  commGroupFreeResp_t * ) = NULL;
  static int (* pcommGroupAdd  ) (commGroupAddReq_t *,   commGroupAddResp_t *  ) = NULL;
  static int (* pcommSetParam  ) (commSetParamReq_t *,   commSetParamResp_t *  ) = NULL;
  static int (* pcommSayHi     ) (commSayHiReq_t *,      commSayHiResp_t *     ) = NULL;
  static int (* pcommSayBye    ) (commSayByeReq_t *,     commSayByeResp_t *    ) = NULL;
  static int (* pcommEnumPorts ) (commEnumPortsReq_t *,  commEnumPortsResp_t * ) = NULL;

  // structure to hook DLL exported function names to above comm function ptrs
  typedef struct _COMMFUNC
  {
    char    name[32];
    void ** procptr;
  }
  COMMFUNC;

  static COMMFUNC commfuncs[] =
  {
    {"commNoOp",       (void **) &pcommNoOp},
    {"commInit",       (void **) &pcommInit},
    {"commTerm",       (void **) &pcommTerm},
    {"commDriverInfo", (void **) &pcommDriverInfo},
    {"commPlayerInfo", (void **) &pcommPlayerInfo},
    {"commTxFull",     (void **) &pcommTxFull},
    {"commTxPkt",      (void **) &pcommTxPkt},
    {"commPeekPkt",    (void **) &pcommPeekPkt},
    {"commRxPkt",      (void **) &pcommRxPkt},
    {"commScanAddr",   (void **) &pcommScanAddr},
    {"commPrintAddr",  (void **) &pcommPrintAddr},
    {"commGroupAlloc", (void **) &pcommGroupAlloc},
    {"commGroupFree",  (void **) &pcommGroupFree},
    {"commGroupAdd",   (void **) &pcommGroupAdd},
    {"commSetParam",   (void **) &pcommSetParam},
    {"commSayHi",      (void **) &pcommSayHi},
    {"commSayBye",     (void **) &pcommSayBye},
    {"commEnumPorts",  (void **) &pcommEnumPorts},
  };

  #define NO_COMMDLL_FUNCS  (sizeof(commfuncs) / sizeof(commfuncs[0]))

  // public comm function stubs
  int cdecl commNoOp(commNoOpReq_t *req, commNoOpResp_t *resp) {return (*pcommNoOp)(req, resp);}

  int cdecl commInit(commInitReq_t *req, commInitResp_t *resp) {return (*pcommInit)(req, resp);}

  int cdecl commTerm(commTermReq_t *req, commTermResp_t *resp) {return (*pcommTerm)(req, resp);}

  int cdecl commDriverInfo(commDriverInfoReq_t *req, commDriverInfoResp_t *resp) {return (*pcommDriverInfo)(req, resp);}

  int cdecl commPlayerInfo(commPlayerInfoReq_t *req, commPlayerInfoResp_t *resp) {return (*pcommPlayerInfo)(req, resp);}

  int cdecl commTxFull(commTxFullReq_t *req, commTxFullResp_t *resp) {return (*pcommTxFull)(req, resp);}

  int cdecl commTxPkt(commTxPktReq_t *req, commTxPktResp_t *resp) {return (*pcommTxPkt)(req, resp);}

  int cdecl commPeekPkt(commPeekPktReq_t *req, commPeekPktResp_t *resp) {return (*pcommPeekPkt)(req, resp);}

  int cdecl commRxPkt(commRxPktReq_t *req, commRxPktResp_t *resp) {return (*pcommRxPkt)(req, resp);}

  int cdecl commScanAddr(commScanAddrReq_t *req, commScanAddrResp_t *resp) {return (*pcommScanAddr)(req, resp);}

  int cdecl commPrintAddr(commPrintAddrReq_t *req, commPrintAddrResp_t *resp) {return (*pcommPrintAddr)(req, resp);}

  int cdecl commGroupAlloc(commGroupAllocReq_t *req, commGroupAllocResp_t *resp) {return (*pcommGroupAlloc)(req, resp);}

  int cdecl commGroupFree(commGroupFreeReq_t *req, commGroupFreeResp_t *resp) {return (*pcommGroupFree)(req, resp);}

  int cdecl commGroupAdd(commGroupAddReq_t *req, commGroupAddResp_t *resp) {return (*pcommGroupAdd)(req, resp);}

  int cdecl commSetParam(commSetParamReq_t *req, commSetParamResp_t *resp) {return (*pcommSetParam)(req, resp);}

  int cdecl commSayHi(commSayHiReq_t *req, commSayHiResp_t *resp) {return (*pcommSayHi)(req, resp);}

  int cdecl commSayBye(commSayByeReq_t *req, commSayByeResp_t *resp) {return (*pcommSayBye)(req, resp);}

  int cdecl commEnumPorts(commEnumPortsReq_t *req, commEnumPortsResp_t *resp) {return (*pcommEnumPorts)(req, resp);}
#endif

#if 0
static void showErr()
{
	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL
	);

	// Display the string.
	MessageBox( NULL, lpMsgBuf, "GetLastError", MB_OK|MB_ICONINFORMATION );

	// Free the buffer.
	LocalFree( lpMsgBuf );

}
#endif

/*-----------------------------------------------------------------------
 Read description out of the DLL file.
-------------------------------------------------------------------------*/

DP_API dp_result_t dpGetTransportInfo(dp_transport_t *transport, comm_driverInfo_t *infoptr)
{
	FILE *fp;
	int match;
	int c;

	fp = fopen(transport->fname, "rb");
	if (!fp) {
		DPRINT(("dpGetTransportInfo: %s not found\n", transport->fname));
		return dp_RES_BAD;
	}
	for (match=0; (match<comm_DRIVER_SIGLEN) && ((c=getc(fp)) != EOF); ) {
		if (c == comm_DRIVER_SIGNATURE[match])
			match++;
		else
			match = 0;
	}
	if (match != comm_DRIVER_SIGLEN) {
		DPRINT(("dpGetTransportInfo: couldn't find signature in %s (%d)\n", transport->fname, match));
		fclose(fp);
		return dp_RES_EMPTY;
	}
	if (fread(&infoptr->recordLen, sizeof(comm_driverInfo_t)-comm_DRIVER_SIGLEN, 1, fp) != 1) {
		DPRINT(("dpGetTransportInfo: error reading info\n"));
		fclose(fp);
		return dp_RES_EMPTY;
	}
	if (infoptr->recordLen != sizeof(comm_driverInfo_t)) {
		DPRINT(("dpGetTransportInfo: bad recordLen %x, should be %x\n",
			infoptr->recordLen, sizeof(comm_driverInfo_t)));
		fclose(fp);
		return dp_RES_BUG;
	}
	fclose(fp);
	DPRINT(("dpGetTransportInfo(%s): needs %x, capabilities %x\n",
		transport->fname, infoptr->needs, infoptr->capabilities));
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
 Calls the given function once for each transport available.
 All calls are made before dpEnumTransports returns.

 Path argument "tranDir" is the directory to search for DLL's.
 Path may be NULL or empty, in which case the current directory is searched.
 Path may end in a / or \, or not.
 Reads description out of the DLL files.

 Windows-specific:
 if no path is given, searches the DLL subdirectory of the directory
 the current module lives in!
-------------------------------------------------------------------------*/

DP_API dp_result_t dpEnumTransports(dp_transport_t *tranDir, dpEnumTransportCallback_t cb, void *context)
{
  char fnamebuf[256];
  dp_transport_t dllname;
  char pathns[256];
  HANDLE            hfind;
  comm_driverInfo_t info;
  WIN32_FIND_DATA   finddata;

  if (tranDir && tranDir->fname[0]) {
    char *p;

    strcpy(pathns, tranDir->fname);

    // Strip trailing slash from pathns, even if it is the root directory.
    p = pathns+strlen(pathns)-1;

    if (*p == '/' || *p == '\\') *p = 0;

    sprintf(fnamebuf, "%s/*.dll", pathns);
  } else {
    char filename[512];
    char StartupName[_MAX_FNAME];
    char StartupDrv[_MAX_DRIVE];
    char StartupDir[_MAX_DIR];

	// Get Startup Drive & Directory
    GetModuleFileName(NULL, filename, sizeof(filename));
    _splitpath(filename, StartupDrv, StartupDir, StartupName, NULL);
    _makepath(fnamebuf, StartupDrv, StartupDir, "*", "DLL");
  }

  DPRINT(("dpEnumTransports: %s\n", fnamebuf));
  hfind = FindFirstFile(fnamebuf, &finddata);
  if (hfind == INVALID_HANDLE_VALUE)  return dp_RES_EMPTY;

  // scan all DLLs found in path, get their info, and call callback funciton
  do {
	memset(&dllname, 0, sizeof(dllname));
    if (tranDir && tranDir->fname[0])
		sprintf(dllname.fname, "%s/%s", pathns, finddata.cFileName);
	else
		sprintf(dllname.fname, "%s", finddata.cFileName);

    if (dpGetTransportInfo(&dllname, &info) != dp_RES_OK)  continue;

    cb(&dllname, &info, context);
  }
  while (FindNextFile(hfind, &finddata));

  FindClose(hfind);

  return(dp_RES_OK);
}

/*-------------------------------------------------------------------------
 Called internally by dpCreate to load a DLL.
-------------------------------------------------------------------------*/

dp_result_t dpLoadDLL(dpio_t *dpio, const dp_transport_t *transport)
{
#if DYNALINK

	int i;

	if (!transport)
		return dp_RES_BUG;

    if (hcommDLL)
		return dp_RES_FULL;   // Don't allow double loading.

	hcommDLL = LoadLibrary(transport->fname);
	if (!hcommDLL) {
		//showErr();
		return dp_RES_EMPTY;
	}

	for (i = 0; i < NO_COMMDLL_FUNCS; i++) {
		commfuncs[i].procptr[0] =
			(void *) GetProcAddress(hcommDLL, commfuncs[i].name);
		if (!commfuncs[i].procptr[0])  {
			FreeLibrary(hcommDLL);
			hcommDLL = NULL;
			return dp_RES_BAD;
		}
	}

#endif

  (void) dpio;

  return(dp_RES_OK);
}

/*-------------------------------------------------------------------------
 Called internally by dpDestroy to unload a DLL.
-------------------------------------------------------------------------*/

void dpUnloadDLL(dpio_t *dpio)
{
  #if DYNALINK
	if (hcommDLL)
		FreeLibrary(hcommDLL);
    hcommDLL = NULL;
  #endif

  (void) dpio;
}

/*-------------------------------------------------------------------------
 Call this during or after dpEnumTransports, but before dpCreate,
 and only on transports which have the comm_INIT_NEEDS_PORTNUM bit set
 in comm_driverInfo_t.needs.
 Present the returned list of port names to the user, then use the
 matching portnum as the value for commInitReq_t.portnum in dpCreate.

 On entry,
 transport is the name (from dpEnumTransports) of the transport to query.
 ports is an array to be filled with portnames.
 maxports is the size of the ports array.

 On exit,
 ports is filled with descriptions of the available ports.
 *pnPorts is set to the number of portnames placed in the ports array.
-------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpEnumPorts(const dp_transport_t *transport, commPortName_t *ports, int maxports, int *pnPorts)
{
	dp_result_t err;
	commEnumPortsReq_t req;
	commEnumPortsResp_t resp;

	assert(transport && ports && pnPorts);
	err = dpLoadDLL(NULL, transport);
	if (err != dp_RES_OK)
		return err;

	memset(&req, 0, sizeof(commEnumPortsReq_t));
	memset(&resp, 0, sizeof(commEnumPortsResp_t));

	req.maxports = maxports;
	req.buf = ports;
	req.reqLen = sizeof(req);

	commEnumPorts(&req, &resp);
	*pnPorts = resp.numports;

	dpUnloadDLL(NULL);
	return (dp_result_t)resp.status;
}
