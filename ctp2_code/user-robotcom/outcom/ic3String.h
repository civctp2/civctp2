













#pragma once
#ifndef __IC3STRING_H__
#define __IC3STRING_H__


DEFINE_GUID(CLSID_IC3String, 
			0x1a3ca3a0, 
			0x442, 
			0x11d2, 
			0xbd, 0x9b, 0x0, 0x60, 0x8, 0x27, 0x65, 0x9f);

#undef INTERFACE
#define INTERFACE IC3String

DECLARE_INTERFACE_(IC3String, IUnknown)
{
	STDMETHOD_ (const MBCHAR *, GetString) (THIS_ const MBCHAR *stringId) PURE;
};

#endif

