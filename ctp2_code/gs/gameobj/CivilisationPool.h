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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
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

#include "Civilisation.h"

enum CIV_INDEX
	{
	CIV_INDEX_VANDALS,
	CIV_INDEX_CIV_0=CIV_INDEX_VANDALS,
	CIV_INDEX_CIV_1,
	CIV_INDEX_CIV_2,
	CIV_INDEX_CIV_3,
	CIV_INDEX_CIV_4,
	CIV_INDEX_CIV_5,
	CIV_INDEX_CIV_6,
	CIV_INDEX_CIV_7,
	CIV_INDEX_CIV_8,
	CIV_INDEX_CIV_9,
	CIV_INDEX_CIV_10,
	CIV_INDEX_CIV_11,
	CIV_INDEX_CIV_12,
	CIV_INDEX_CIV_13,
	CIV_INDEX_CIV_14,
	CIV_INDEX_CIV_15,
	CIV_INDEX_CIV_16,
	CIV_INDEX_CIV_17,
	CIV_INDEX_CIV_18,
	CIV_INDEX_CIV_19,
	CIV_INDEX_CIV_20,
	CIV_INDEX_CIV_21,
	CIV_INDEX_CIV_22,
	CIV_INDEX_CIV_23,
	CIV_INDEX_CIV_24,
	CIV_INDEX_CIV_25,
	CIV_INDEX_CIV_26,
	CIV_INDEX_CIV_27,
	CIV_INDEX_CIV_28,
	CIV_INDEX_CIV_29,
	CIV_INDEX_CIV_30,
	CIV_INDEX_CIV_31,
	CIV_INDEX_CIV_32,
	CIV_INDEX_CIV_33,
	CIV_INDEX_CIV_34,
	CIV_INDEX_CIV_35,
	CIV_INDEX_CIV_36,
	CIV_INDEX_CIV_37,
	CIV_INDEX_CIV_38,
	CIV_INDEX_CIV_39,
	CIV_INDEX_CIV_40,
	CIV_INDEX_CIV_41,
	CIV_INDEX_CIV_42,
	CIV_INDEX_CIV_43,
	CIV_INDEX_CIV_44,
	CIV_INDEX_CIV_45,
	CIV_INDEX_CIV_46,
	CIV_INDEX_CIV_47,
	CIV_INDEX_CIV_48,
	CIV_INDEX_CIV_49,
	CIV_INDEX_CIV_50,
	CIV_INDEX_CIV_51,
	CIV_INDEX_CIV_52,
	CIV_INDEX_CIV_53,
	CIV_INDEX_CIV_54,
	CIV_INDEX_CIV_55,
	CIV_INDEX_CIV_56,
	CIV_INDEX_CIV_57,
	CIV_INDEX_CIV_58,
	CIV_INDEX_CIV_59,
	CIV_INDEX_CIV_60,
	CIV_INDEX_CIV_61,
	CIV_INDEX_CIV_62,
	CIV_INDEX_CIV_63,
	CIV_INDEX_INVALID,
	CIV_INDEX_RANDOM=1024,
	CIV_INDEX_MAX,
	} ;

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
#if !defined(ACTIVISION_ORIGINAL)
		void Release(CIV_INDEX const & civ);
#endif

		void Serialize(CivArchive &archive) ;

	} ;

extern CivilisationPool	*g_theCivilisationPool;
#else

class CivilisationPool ;

#endif
