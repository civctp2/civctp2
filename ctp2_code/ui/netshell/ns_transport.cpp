//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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

#include "c3.h"

#include "ns_transport.h"



ns_Transport::ns_Transport(NETFunc::Transport * transport) 
:	ns_Object<NETFunc::Transport, ns_Transport>(transport) 
{
	list.push_back(Struct(STRING,	&m_name));
};


void ns_Transport::Update( NETFunc::Transport *transport ) {
	SetMine(false);
	m_name = transport->GetName();
}

