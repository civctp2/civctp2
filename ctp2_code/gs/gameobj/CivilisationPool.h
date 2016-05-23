//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Civilisation pool
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Recycle civilisation indices to prevent a game crash.
// - Replaced CIV_INDEX by sint32. (2-Jan-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __CIVILISATIONPOOL_H__
#define __CIVILISATIONPOOL_H__

class CivilisationPool;

#include "ObjPool.h"
#include "GameObj_types.h"
#include "Civilisation.h"

template <class T> class SimpleDynamicArray;

class CivilisationPool : public ObjPool
{
public:
	SimpleDynamicArray<sint32> *m_usedCivs;

	CivilisationPool(void) ;
	CivilisationPool(CivArchive &archive) ;
	~CivilisationPool(void) ;

	CivilisationData* AccessData(const Civilisation id) { return ((CivilisationData*)Access(id)) ; }

	CivilisationData* GetData(const Civilisation id) const { return ((CivilisationData*)Get(id)) ; }

	Civilisation Create(const PLAYER_INDEX owner, sint32 civ, GENDER gender) ;
	void Release(sint32 const & civ);

	void Serialize(CivArchive &archive) ;
};

extern CivilisationPool	*g_theCivilisationPool;

#endif
