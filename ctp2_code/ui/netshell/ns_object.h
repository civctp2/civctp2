//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : 
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
// - Corrected strange access of non-static members from static data.
//
//----------------------------------------------------------------------------

#ifndef __NS_OBJECT_H__
#define __NS_OBJECT_H__

#include "netfunc.h"
#include "ns_accessor.h"

template <class T, class NetShellT>

class ns_Object : public ns_Accessor<NetShellT>, public NETFunc::Key
{
	T *nf_object;
	bool mine;
public:
	ns_Object(T * t)
	:	ns_Accessor<NetShellT>(),
		NETFunc::Key(t),
		nf_object(t),
		mine(false)
	{
		((NetShellT *) this)->Update(t);
	};






	bool IsMine() {
		return mine;
	}

	void SetMine(bool m) {
		mine = m;
	}

	
	T *GetNETFuncObject(void) {
		return nf_object;
	}
	
	void Update(T *t) {
		mine = false;
	};
	virtual ~ns_Object() { 
		nf_object = 0;
	}

	
	void SetKey( void ) {}
};


#endif 
