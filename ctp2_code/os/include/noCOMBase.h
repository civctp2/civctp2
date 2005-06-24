/**
 * $Id$
 */
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __os__include__noCOMBase_h__
#define __os__include__noCOMBase_h__ 1

class ICTP2Unknown {
public:
	virtual uint32 AddRef() = 0;
	virtual uint32 Release() = 0;
};

#endif

