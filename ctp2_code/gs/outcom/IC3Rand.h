#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __IC3RAND_H__
#define __IC3RAND_H__

#include "ctp2_inttypes.h"  // sint32

#if !defined(USE_COM_REPLACEMENT)
#include <OBJBASE.H>

DEFINE_GUID(CLSID_IC3Rand,
			0xa3e09e60,
			0x1cb9,
			0x11d2,
			0xbd, 0x9b, 0x0, 0x60, 0x8, 0x27, 0x65, 0x9f);

#undef INTERFACE
#define INTERFACE IC3Rand

DECLARE_INTERFACE_(IC3Rand, IUnknown)
{
	STDMETHOD_(sint32, Next) (THIS_ sint32 range) PURE;
};
#else
#include "noCOMBase.h"

class IC3Rand : public IUnknown
{
public:
	virtual sint32 Next(sint32 range) = 0;
};

#endif

#endif
