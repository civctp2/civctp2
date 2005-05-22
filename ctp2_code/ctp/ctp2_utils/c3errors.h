//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
// Description  :
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
//
// Modifications from the original Activision code:
//
//
//----------------------------------------------------------------------------
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __C3ERRORS_H__
#define __C3ERRORS_H__

void c3errors_FatalDialog(const char *module, const char *fmt, ...);
void c3errors_FatalDialogFromDB(const char *module, const char *err, ...);
void c3errors_ErrorDialog(const char *module, const char *fmt, ...);
void c3errors_ErrorDialogFromDB(const char *module, const char *err, ...);

#endif
