//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Prevent assigning the same civilisation index twice.
// - Recycle civilisation indices to prevent a game crash.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3errors.h"
#include "Unit.h"
#include "Player.h"
#include "StrDB.h"
#include "CivilisationDB.h"
#include "CivilisationPool.h"
#include "CivArchive.h"
#include "RandGen.h"
#include "SimpleDynArr.h"
#include "ProfileDB.h"
#include "network.h"
#include "Globals.h"

	extern	Player	**g_player ;

	extern	StringDB	*g_theStringDB ;

	extern	CivilisationDatabase	*g_theCivilisationDB ;

	extern RandomGenerator *g_rand;
	
	extern ProfileDB *g_theProfileDB;









CivilisationPool::CivilisationPool(void) : ObjPool(k_BIT_GAME_OBJ_TYPE_CIVILISATION)
{
	m_usedCivs = new SimpleDynamicArray<CIV_INDEX>;
}









CivilisationPool::CivilisationPool(CivArchive &archive) : ObjPool(k_BIT_GAME_OBJ_TYPE_CIVILISATION)
{
	m_usedCivs = new SimpleDynamicArray<CIV_INDEX>;
	Serialize(archive) ;
}


CivilisationPool::~CivilisationPool(void)
{
	delete m_usedCivs;
}












void CivilisationPool::Serialize(CivArchive &archive)
	{
	CivilisationData	*newData ;

	sint32	i,
			count = 0 ;

    CHECKSERIALIZE

#define CIVPOOL_MAGIC 0xBCDE0123
	if (archive.IsStoring())
		{
		archive.PerformMagic(CIVPOOL_MAGIC) ;
		ObjPool::Serialize(archive);

		for (i=0; i<k_OBJ_POOL_TABLE_SIZE; i++)
			if(m_table[i])
				count++;

		archive<<count;
		for(i = 0; i < k_OBJ_POOL_TABLE_SIZE; i++)
			if(m_table[i])
				((CivilisationData *)(m_table[i]))->Serialize(archive) ;
		m_usedCivs->Serialize(archive);
		}
	else
		{
		archive.TestMagic(CIVPOOL_MAGIC) ;
		ObjPool::Serialize(archive);

		archive>>count;
		for (i=0; i<count; i++)
			{
			newData = new CivilisationData(archive) ;
			Insert(newData) ;
			}

		m_usedCivs->Serialize(archive);
		}

	}












Civilisation CivilisationPool::Create(const PLAYER_INDEX owner, CIV_INDEX requiredCiv, GENDER gender)
{
#if defined(ACTIVISION_ORIGINAL)
	sint32	civ ;

	sint32	numCivs,
			i,
			match ;

	StringId	strId ;

	CivilisationData* newData;

	civ = requiredCiv ;
	sint32 c;
	numCivs = g_theCivilisationDB->GetCivilisations() ;
	if (civ == CIV_INDEX_RANDOM) {
		if(g_theProfileDB->IsNonRandomCivs()) {
			civ = owner;
		} else {
			civ = g_rand->Next(numCivs);
		}
		for (c=0; c<numCivs; c++, civ++) {
			if(civ >= numCivs) {
				civ = 0;
			}
			match = FALSE ;
			for (i=0; i<m_usedCivs->Num(); i++) {
				if (m_usedCivs->Access(i) == civ) {
					match = TRUE ;
					break ;
				}

			}

			if (!match)
				break ;
		}

		if (civ == numCivs) {
			c3errors_FatalDialogFromDB("CIVILIZATION_ERROR", "CIVILIZATION_NO_MORE_CIVS_AVAILABLE") ;
			civ = CIV_INDEX_VANDALS ;								
		}

	}

	Assert((civ>=CIV_INDEX_CIV_0) && (civ<g_theCivilisationDB->GetCivilisations())) ;

	Civilisation newCivilisation(NewKey(k_BIT_GAME_OBJ_TYPE_CIVILISATION));

	if (gender == GENDER_RANDOM) {
		gender = (GENDER)(g_rand->Next() % 2);
	}

	newData = new CivilisationData(newCivilisation, owner, (CIV_INDEX)(civ), gender) ;
	
	m_usedCivs->Insert((CIV_INDEX)civ);

	if (gender == GENDER_MALE)
		strId = g_theCivilisationDB->GetLeaderName((CIV_INDEX)(civ)) ;
	else
		strId = g_theCivilisationDB->GetLeaderNameFemale((CIV_INDEX)(civ)) ;
#else
	sint32 const	numCivs	= g_theCivilisationDB->GetCivilisations();
	CIV_INDEX		civ		= requiredCiv;

	if (CIV_INDEX_RANDOM == civ)
	{
		if (g_theProfileDB->IsNonRandomCivs()) 
		{
			civ = static_cast<CIV_INDEX>(owner);
		} 
		else 
		{
			civ = static_cast<CIV_INDEX>(g_rand->Next(numCivs));
		}
	}

	for (sint32 c = 0; m_usedCivs->IsPresent(civ) && (c < numCivs); ++c) 
	{
		civ = static_cast<CIV_INDEX>((civ + 1 < numCivs) ? civ + 1 : 1);
	}

	if (civ >= numCivs) 
	{
		c3errors_FatalDialogFromDB("CIVILIZATION_ERROR", "CIVILIZATION_NO_MORE_CIVS_AVAILABLE") ;
		civ = CIV_INDEX_VANDALS;								
	}

	Assert((civ >= CIV_INDEX_CIV_0) && (civ < numCivs));

	Civilisation newCivilisation(NewKey(k_BIT_GAME_OBJ_TYPE_CIVILISATION));

	if (gender == GENDER_RANDOM) {
		gender = (GENDER)(g_rand->Next() % 2);
	}

	CivilisationData *	newData = new CivilisationData(newCivilisation, owner, civ, gender);
	
	m_usedCivs->Insert(civ);

	StringId	strId = (gender == GENDER_MALE) 
						? g_theCivilisationDB->GetLeaderName(civ)
						: g_theCivilisationDB->GetLeaderNameFemale(civ);

#endif
    newData->SetLeaderName(g_theStringDB->GetNameStr(strId)) ;

    strId = g_theCivilisationDB->GetPersonalityDescription((CIV_INDEX)(civ));
    newData->SetPersonalityDescription(g_theStringDB->GetNameStr(strId)); 


	strId = g_theCivilisationDB->GetPluralCivName((CIV_INDEX)(civ)) ;
	newData->SetPluralCivName(g_theStringDB->GetNameStr(strId)) ;
	strId = g_theCivilisationDB->GetCountryName((CIV_INDEX)(civ)) ;
	newData->SetCountryName(g_theStringDB->GetNameStr(strId)) ;
	strId = g_theCivilisationDB->GetSingularCivName((CIV_INDEX)(civ)) ;
	newData->SetSingularCivName(g_theStringDB->GetNameStr(strId)) ;

	newData->SetCityStyle(g_theCivilisationDB->GetCityStyle((CIV_INDEX)(civ)));

	Insert(newData) ;
	DPRINTF(k_DBG_INFO, ("Civilisation %d is in use\n", civ)) ;

	if(g_network.IsHost()) {
		g_network.Enqueue(newData);
	}

	return (newCivilisation) ;
}

#if !defined(ACTIVISION_ORIGINAL)
//----------------------------------------------------------------------------
//
// Name       : CivilisationPool::Release
//
// Description: Release a civilisation index for reuse.
//
// Parameters : civ		: civilisation index to release
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

void CivilisationPool::Release(CIV_INDEX const & civ)
{
	sint32 const	usedCount = m_usedCivs->Num();

	for (sint32 i = 0; i < usedCount; ++i) 
	{
		if (civ == m_usedCivs->Access(i)) 
		{
			m_usedCivs->DelIndex(i);
			return;
		}
	}
}

#endif
