//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Civilisation pool
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
// Note: For the blocks with _MSC_VER preprocessor directives, the following
//       is implied: the (_MSC_VER) preprocessor directive lines, and the blocks
//       that are inactive for _MSC_VER value 1200 are modified Apolyton code. 
//       The blocks that are inactiThe blocks that are active for _MSC_VER value 
//       1200 are the original Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Recycle civilisation indices to prevent a game crash.
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#ifndef __CIVILISATIONPOOL_H__
#define __CIVILISATIONPOOL_H__

#include "ObjPool.h"
#include "GameObj_types.h"
#include "Civilisation.h"

template <class T> class SimpleDynamicArray;

class CivilisationPool : public ObjPool
	{
	public:
		SimpleDynamicArray<CIV_INDEX> *m_usedCivs;

	public:
		CivilisationPool(void) ;
		CivilisationPool(CivArchive &archive) ;
		~CivilisationPool(void) ;

		CivilisationData* AccessData(const Civilisation id) { return ((CivilisationData*)Access(id)) ; }

		CivilisationData* GetData(const Civilisation id) const { return ((CivilisationData*)Get(id)) ; }

		Civilisation Create(const PLAYER_INDEX owner, CIV_INDEX civ, GENDER gender) ;
		void Release(CIV_INDEX const & civ);

		void Serialize(CivArchive &archive) ;

	} ;

extern CivilisationPool	*g_theCivilisationPool;
#else

class CivilisationPool ;

#endif
