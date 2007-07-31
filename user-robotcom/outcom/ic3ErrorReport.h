













#pragma once
#ifndef __IC3ERRORREPORT_H__
#define __IC3ERRORREPORT_H__


DEFINE_GUID(CLSID_IC3ErrorReport, 
			0x63aa7420, 
			0x1fb, 
			0x11d2, 
			0x99, 0xee, 0x0, 0xe0, 0x98, 0x1, 0xd0, 0xf0);

#undef INTERFACE
#define INTERFACE IC3ErrorReport

DECLARE_INTERFACE_(IC3ErrorReport, IUnknown)
{
	STDMETHOD_ (void, ReportDBError) (THIS_ const MBCHAR *stringId) PURE;
	STDMETHOD_ (void, ReportError) (THIS_ const MBCHAR *text) PURE;
};

#endif
