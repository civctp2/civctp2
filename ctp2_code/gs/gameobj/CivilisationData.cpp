










#include "c3.h"
#include "Globals.h"
#include "CivilisationRec.h"
#include "CivilisationDB.h"
#include "CivilisationData.h"
#include "Civilisation.h"
#include "ProfileDB.h"
#include "StrDB.h"
#include "network.h"
#include "CivArchive.h"
#include "Player.h"
#include "UnitDynArr.h"
#include "CityData.h"

	extern	StringDB	*g_theStringDB ;

	extern	CivilisationDatabase	*g_theCivilisationDB ;
	extern	ProfileDB				*g_theProfileDB; 









CivilisationData::CivilisationData(const ID &id, PLAYER_INDEX owner, CIV_INDEX civ, GENDER gender) : GAMEOBJ(id.m_id)
	{
	
	m_owner = owner ;
	memset(m_cityname_count, 0, sizeof(m_cityname_count)) ;
	m_civ = civ ;
	m_gender = gender;
	m_cityStyle = CITY_STYLE_MAX - 1;
	memset(m_leader_name, 0, k_MAX_NAME_LEN);
    memset(m_personality_description, 0, k_MAX_NAME_LEN); 
	memset(m_civilisation_name, 0, k_MAX_NAME_LEN);
	memset(m_country_name, 0, k_MAX_NAME_LEN);
	memset(m_singular_name, 0, k_MAX_NAME_LEN);
	ENQUEUE();
	}









CivilisationData::CivilisationData(const ID &id) : GAMEOBJ(id.m_id)
	{
    m_gender = GENDER_MALE; 
	m_cityStyle = CITY_STYLE_MAX - 1;
	memset(m_cityname_count, 0, sizeof(m_cityname_count));
	memset(m_leader_name, 0, k_MAX_NAME_LEN);
    memset(m_personality_description, 0, k_MAX_NAME_LEN); 
	memset(m_civilisation_name, 0, k_MAX_NAME_LEN);
	memset(m_country_name, 0, k_MAX_NAME_LEN);
	memset(m_singular_name, 0, k_MAX_NAME_LEN);
	}








CivilisationData::CivilisationData(CivArchive &archive) : GAMEOBJ(0)
	{
	Serialize(archive) ;
	}












void CivilisationData::Serialize(CivArchive &archive)
{

    CHECKSERIALIZE

	uint8 hasChild;
	if (archive.IsStoring()) {
		GAMEOBJ::Serialize(archive);
		archive.StoreChunk((uint8 *)&m_owner, ((uint8 *)&m_singular_name)+sizeof(m_singular_name));

		hasChild = m_lesser != NULL;
		archive << hasChild;
		if(m_lesser) {
			((CivilisationData*)(m_lesser))->Serialize(archive);
		}
		hasChild = m_greater != NULL;
		archive << hasChild;
		if(m_greater) {
			((CivilisationData*)(m_greater))->Serialize(archive);
		}

	} else {
		GAMEOBJ::Serialize(archive);
		archive.LoadChunk((uint8 *)&m_owner, ((uint8 *)&m_singular_name)+sizeof(m_singular_name));

		archive >> hasChild;
		if(hasChild) {
			m_lesser = new CivilisationData(archive);
		} else {
			m_lesser = NULL;
		}
		archive >> hasChild;
		if(hasChild) {
			m_greater = new CivilisationData(archive);
		} else {
			m_greater = NULL;
		}
	}
}


bool cityNameIsUsedByPlayer(const char *strName, sint32 player)
{
	
	
	
	if(player < 0 || player >= k_MAX_PLAYERS || !g_player[player]) {
		Assert(false);
		return false;
	}

	sint32 i;
	for(i = 0; i < g_player[player]->m_all_cities->Num(); i++) {
		if(stricmp(g_player[player]->m_all_cities->Access(i).CD()->GetName(), strName) == 0)
			return true;
	}
	return false;
}












sint32 CivilisationData::GetAnyCityName(void) const
	{
	sint32	i,
			count = 0xFFFFFFF,
			name = k_CITY_NAME_UNDEFINED,
			numNames ;

	numNames = g_theCivilisationDB->GetNumCities(m_civ) ;
	for (i=0; i<numNames; i++)
		{
		if (m_cityname_count[i] < count)
			{
			name = i ;
			count = m_cityname_count[i] ;
			char strName[k_MAX_NAME_LEN];
			GetCityName(name, strName);
			if (count == 0 && !cityNameIsUsedByPlayer(strName, m_owner))
				break ;

			}

		}

	return (name) ;
	}













void CivilisationData::GetCityName(const sint32 name, MBCHAR *s) const
	{
	StringId	strId ;

	Assert(name>=0) ;
	Assert(name<g_theCivilisationDB->GetNumCities(m_civ)) ;
	strId = g_theCivilisationDB->GetCityName(m_civ, name) ;
	if (m_cityname_count[name] > 0)
		sprintf(s, "%s%d", g_theStringDB->GetNameStr(strId), m_cityname_count[name]) ;
	else
		strcpy(s, g_theStringDB->GetNameStr(strId)) ;
	}













void CivilisationData::UseCityName(const sint32 name)
	{
	Assert(name>=0) ;
	Assert(name<g_theCivilisationDB->GetNumCities(m_civ)) ;
	m_cityname_count[name]++ ;

#ifdef _DEBUG
		{
		StringId	strId ;

		MBCHAR	s[k_MAX_NAME_LEN] ;

		strId = g_theCivilisationDB->GetCityName(m_civ, name) ;
		if (m_cityname_count[name] > 0)
			sprintf(s, "%s%d", g_theStringDB->GetNameStr(strId), m_cityname_count[name]) ;
		else
			strcpy(s, g_theStringDB->GetNameStr(strId)) ;

		DPRINTF(k_DBG_INFO, ("City Name %s used\n", s)) ;
		}

#endif
	}












void CivilisationData::ReleaseCityName(const sint32 name)
	{
	Assert(name>=0) ;
	Assert(name<g_theCivilisationDB->GetNumCities(m_civ)) ;

#ifdef _DEBUG
		{
		StringId	strId ;

		MBCHAR	s[k_MAX_NAME_LEN] ;

		strId = g_theCivilisationDB->GetCityName(m_civ, name) ;
		if (m_cityname_count[name] > 0)
			sprintf(s, "%s%d", g_theStringDB->GetNameStr(strId), m_cityname_count[name]) ;
		else
			strcpy(s, g_theStringDB->GetNameStr(strId)) ;

		DPRINTF(k_DBG_INFO, ("City Name %s release\n", s)) ;
		}

#endif

	if (m_cityname_count[name]>0)
		m_cityname_count[name]-- ;

	}


sint32 CivilisationData::GetUseCount(const sint32 name) const
	{
	Assert(name>=0) ;
	Assert(name<g_theCivilisationDB->GetNumCities(m_civ)) ;

	return (m_cityname_count[name]) ;
	}


MBCHAR *CivilisationData::GetLeaderName(void)
{
    if (m_leader_name[0]) {
        return (m_leader_name);
    } else {
        
        return (g_theProfileDB->GetLeaderName());
    }
}


void CivilisationData::SetLeaderName(const MBCHAR *s)
	{
	Assert(s[0]!=0) ;
	Assert(s!=NULL) ;
	strcpy(m_leader_name, s) ;
	}

void CivilisationData::SetPersonalityDescription(const MBCHAR* s)
{
	Assert(s[0] != 0) ;
	Assert(s != NULL) ;
	strcpy(m_personality_description, s) ;
}

MBCHAR* CivilisationData::GetPersonalityDescription(void)
{
    return m_personality_description;
}

void CivilisationData::GetPluralCivName(MBCHAR *s)
	{
	Assert(s!=NULL) ;
	strcpy(s, m_civilisation_name) ;
	}


void CivilisationData::SetPluralCivName(const MBCHAR *s)
	{
	Assert(s[0]!=0) ;
	Assert(s!=NULL) ;
	strcpy(m_civilisation_name, s) ;
	}


void CivilisationData::GetCountryName(MBCHAR *s)
	{
	Assert(s!=NULL) ;
	strcpy(s, m_country_name) ;
	}


void CivilisationData::SetCountryName(const MBCHAR *s)
	{
	Assert(s[0]!=0) ;
	Assert(s!=NULL) ;
	strcpy(m_country_name, s) ;
	}


void CivilisationData::GetSingularCivName(MBCHAR *s)
	{
	Assert(s!=NULL) ;
	strcpy(s, m_singular_name) ;
	}


void CivilisationData::SetSingularCivName(const MBCHAR *s)
	{
	Assert(s[0]!=0) ;
	Assert(s!=NULL) ;
	strcpy(m_singular_name, s) ;
	}


sint32 CivilisationData::GetCapitalName(void) const
	{ 
	return (g_theCivilisationDB->GetCapital(m_civ)) ; 
	}







void CivilisationData::ResetCiv(CIV_INDEX newCivIndex, GENDER gender) 
{
	StringId		strId;

	m_gender = gender;

	m_civ = newCivIndex;

	if (gender == GENDER_MALE)
		strId = g_theCivilisationDB->GetLeaderName(newCivIndex) ;
	else
		strId = g_theCivilisationDB->GetLeaderNameFemale(newCivIndex) ;

    SetLeaderName(g_theStringDB->GetNameStr(strId)) ;

    strId = g_theCivilisationDB->GetPersonalityDescription(newCivIndex);
    SetPersonalityDescription(g_theStringDB->GetNameStr(strId)); 

	strId = g_theCivilisationDB->GetPluralCivName(newCivIndex) ;
	SetPluralCivName(g_theStringDB->GetNameStr(strId)) ;
	
	strId = g_theCivilisationDB->GetCountryName(newCivIndex) ;
	SetCountryName(g_theStringDB->GetNameStr(strId)) ;
	
	strId = g_theCivilisationDB->GetSingularCivName(newCivIndex) ;
	SetSingularCivName(g_theStringDB->GetNameStr(strId)) ;
}

void CivilisationData::ResetStrings()
{
	if(m_gender == GENDER_MALE) {
		SetLeaderName(g_theStringDB->GetNameStr(g_theCivilisationDB->GetLeaderName(m_civ)));
	} else if(m_gender == GENDER_FEMALE) {
		SetLeaderName(g_theStringDB->GetNameStr(g_theCivilisationDB->GetLeaderNameFemale(m_civ)));
	}

	SetPersonalityDescription(g_theStringDB->GetNameStr(g_theCivilisationDB->GetPersonalityDescription(m_civ)));
	SetPluralCivName(g_theStringDB->GetNameStr(g_theCivilisationDB->GetPluralCivName(m_civ)));
	SetCountryName(g_theStringDB->GetNameStr(g_theCivilisationDB->GetCountryName(m_civ)));
	SetSingularCivName(g_theStringDB->GetNameStr(g_theCivilisationDB->GetSingularCivName(m_civ)));
}
