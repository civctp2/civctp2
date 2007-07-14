//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Multiplayer feat packet handler.
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
// _MSC_VER		
// - Compiler version (for the Microsoft C++ compiler only)
//
//----------------------------------------------------------------------------
//
// This file is not part of the original Activision code release.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef NET_FEAT_H
#define NET_FEAT_H

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

// None yet

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class NetFeatTracker;

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "net_packet.h"	// Packetizer interface

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class NetFeatTracker: public Packetizer
{
public:
	NetFeatTracker();
	virtual ~NetFeatTracker();

	void Packetize(uint8 * buf, uint16 & size);
	void Unpacketize(uint16 id, uint8 * buf, uint16 size);
};

#endif	// Multiple include guard
