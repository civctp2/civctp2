//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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
// - #pragma once added.
//
//----------------------------------------------------------------------------
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __IC3_CIVARCHIVE_H__
#define __IC3_CIVARCHIVE_H__ 1

#if !defined(USE_COM_REPLACEMENT)
#undef INTERFACE
#define INTERFACE IC3CivArchive

DEFINE_GUID(CLSID_IC3CivArchive,
    0x151bb9c0,
    0xd098,
    0x11d1,
    0x85, 0xa7, 0x00, 0x60, 0x08, 0x3a, 0x40, 0x82
);

DECLARE_INTERFACE_(IC3CivArchive, IUnknown) {

STDMETHOD_ (void, Store) (THIS_ uint8 *pbData, uint32 ulLen) PURE;
STDMETHOD_ (void, Load) (THIS_ uint8 *pbData, uint32 ulLen) PURE ;
STDMETHOD_ (BOOL, IsStoring)(THIS) PURE ;
};
#else
#include "noCOMBase.h"

class IC3CivArchive : public IUnknown {
public:
  virtual void Store(uint8 *pbData, uint32 ulLen) = 0;
  virtual void Load(uint8 *pbData, uint32 ulLen) = 0;
  virtual BOOL IsStoring() = 0;
};
#endif// !USE_COM_REPLACEMENT

#endif
