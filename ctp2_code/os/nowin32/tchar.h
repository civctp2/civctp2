//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
// Description  : Windows stub header for non-Windows systems
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
//----------------------------------------------------------------------------
#ifndef __TCHAR_H__
#define __TCHAR_H__ 1

#if defined(WIN32)
#error "WIN32 defined, but included tchar.h from os/nowin32!"
#endif

typedef char TCHAR;

#endif
