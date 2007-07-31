
#include "c3.h"
#include "CTPDatabase.h"
#include "DBLexer.h"
#include "DBTokens.h"
#include "StrDB.h"
#include "PointerList.h"

#define k_INITIAL_DB_SIZE 10
#define k_GROW_DB_STEP 10

template <class T> CTPDatabase<T>::CTPDatabase()
{
	m_numRecords = 0;
	m_allocatedSize = k_INITIAL_DB_SIZE;
	m_records = new T *[m_allocatedSize];
}

template <class T> CTPDatabase<T>::~CTPDatabase()
{
	if(m_records) {
		sint32 i;
		for(i = 0; i < m_numRecords; i++) {
			if(m_records[i]) {
				delete m_records[i];
			}
		}
		delete [] m_records;
	}

	if(m_indexToAlpha) {
		delete [] m_indexToAlpha;
	}

	if(m_alphaToIndex) {
		delete [] m_alphaToIndex;
	}

}

template <class T> void CTPDatabase<T>::Grow()
{
	
	T **oldRecords = m_records;
	m_records = new T *[m_allocatedSize + k_GROW_DB_STEP];
	memcpy(m_records, oldRecords, m_allocatedSize * sizeof(T *));
	delete [] oldRecords;
	m_allocatedSize += k_GROW_DB_STEP;
}

template <class T> void CTPDatabase<T>::Add(T *obj)
{
	if(m_numRecords >= m_allocatedSize)
		Grow();

	Assert(m_numRecords < m_allocatedSize);
	obj->SetIndex(m_numRecords);
	m_records[m_numRecords] = obj;
	m_numRecords++;
}

template <class T> T *CTPDatabase<T>::Access(sint32 index)
{
	Assert(index >= 0);
	Assert(index < m_numRecords);
	if((index < 0) || (index >= m_numRecords))
		return NULL;

	return m_records[index];
}

template <class T> sint32 CTPDatabase<T>::GetName(sint32 index)
{
	Assert(index >= 0);
	Assert(index < m_numRecords);
	if((index < 0) || (index >= m_numRecords))
		return 0;

	return m_records[index]->m_name;
}
	
template <class T> const char *CTPDatabase<T>::GetNameStr(sint32 index)
{
	Assert(index >= 0);
	Assert(index < m_numRecords);
	if((index < 0) || (index >= m_numRecords))
		return 0;

	return g_theStringDB->GetNameStr(m_records[index]->m_name);
}
 
template <class T> sint32 CTPDatabase<T>::Parse(DBLexer *lex)
{
	T *obj = NULL;
	sint32 err = 0;

	while(!lex->EndOfInput()) {
		obj = new T();
		if(!obj->Parse(lex)) {
			err = 1;
			break;
		} else {
			Add(obj);
		}
	}

	m_indexToAlpha = new sint32[m_numRecords];
	m_alphaToIndex = new sint32[m_numRecords];
	
	sint32 i;
	for(i = 0; i < m_numRecords; i++) {
		m_indexToAlpha[i] = i;
		m_alphaToIndex[i] = i;
	}

	return !err;
}

template <class T> sint32 CTPDatabase<T>::Parse(const C3DIR & c3dir, const char *filename)
{
	sint32 result;
	DBLexer *lex = new DBLexer(c3dir, filename);
	result = Parse(lex);
	delete lex;
	return result;
}

template <class T> bool CTPDatabase<T>::GetRecordFromLexer(DBLexer *lex, sint32 &index, DBPARSE_ERROR &err)
{
	err = DBPARSE_OK;

	sint32 tok = lex->GetToken();
	if(tok != k_Token_Name) {
		if(tok == k_Token_Int) {
			index = atoi(lex->GetTokenText());
			return true;
		}
		DBERROR(("Expected record name"));
		err = DBPARSE_OTHER;
		return false;
	}

	sint32 strId;
	if(!g_theStringDB->GetStringID(lex->GetTokenText(), strId)) {

		
		sint32 i;
		for(i = 0; i < m_numRecords; i++) {
			if(!stricmp(m_records[i]->GetNameText(), lex->GetTokenText())) {
				index = i;
				return true;
			}
		}
		
		g_theStringDB->InsertStr(lex->GetTokenText(), lex->GetTokenText());
		if(g_theStringDB->GetStringID(lex->GetTokenText(), strId)) {
			index = strId | 0x80000000; 
			err = DBPARSE_DEFER;
			return true;
		} else {
			err = DBPARSE_OTHER;
			return false;
		}
	}

	if(GetNamedItem(strId, index)) {
		return true;
	} else {
		index = strId | 0x80000000;
		err = DBPARSE_DEFER;
		return true;
	}
}

template <class T> bool CTPDatabase<T>::ParseRecordInArray(DBLexer *lex, sint32 **array, sint32 *numElements,
														   DBPARSE_ERROR &err)
{
	err = DBPARSE_OK;

	
	
	sint32 tok = lex->GetToken();
	if(tok != k_Token_Name) {
		DBERROR(("Expected record name"));
		err = DBPARSE_OTHER;
		return false;
	}

	if(*numElements > 0) {
		sint32 *oldArray = *array;
		*array = new sint32[*numElements + 1];
		memcpy(*array, oldArray, (*numElements) * sizeof(sint32));
		delete oldArray;
	} else {
		*array = new sint32[1];
	}		

	sint32 i;
	for(i = 0; i < m_numRecords; i++) {
		if(((m_records[i]->m_name >= 0) &&
			(!strcmp(g_theStringDB->GetIdStr(m_records[i]->m_name), lex->GetTokenText()))) ||
		   ((m_records[i]->m_name < 0) &&
			(!strcmp(m_records[i]->GetNameText(), lex->GetTokenText())))) {
				(*array)[*numElements] = i;
				*numElements += 1;
				return true;
			}
	}

	
	
	sint32 strId;
	if(!g_theStringDB->GetStringID(lex->GetTokenText(), strId)) {
		g_theStringDB->InsertStr(lex->GetTokenText(), lex->GetTokenText());
	}

	if(g_theStringDB->GetStringID(lex->GetTokenText(), strId)) {
		err = DBPARSE_DEFER;
		(*array)[*numElements] = (strId | 0x80000000);
		*numElements += 1;
		return true;
	} else {
		err = DBPARSE_OTHER;
		return false;
	}
}

template <class T> bool CTPDatabase<T>::ParseRecordInArray(DBLexer *lex, sint32 *array, sint32 *numElements, sint32 maxSize,
														   DBPARSE_ERROR &err)
{
	err = DBPARSE_OK;
	
	
	sint32 tok = lex->GetToken();
	if(tok != k_Token_Name) {
		DBERROR(("Expected record name"));
		err = DBPARSE_OTHER;
		return false;
	}

	if(*numElements >= maxSize) {
		DBERROR(("too many entries"));
		err = DBPARSE_OTHER;
		return false;
	}

	sint32 i;
	for(i = 0; i < m_numRecords; i++) {
		if(((m_records[i]->m_name >= 0) &&
			(!strcmp(g_theStringDB->GetIdStr(m_records[i]->m_name), lex->GetTokenText()))) ||
		   ((m_records[i]->m_name < 0) &&
			(!strcmp(m_records[i]->GetNameText(), lex->GetTokenText())))) {
			array[*numElements] = i;
			*numElements += 1;
			return true;
		}
	}
	
	
	sint32 strId;
	if(!g_theStringDB->GetStringID(lex->GetTokenText(), strId)) {
		g_theStringDB->InsertStr(lex->GetTokenText(), lex->GetTokenText());
	}

	if(g_theStringDB->GetStringID(lex->GetTokenText(), strId)) {
		err = DBPARSE_DEFER;
		array[*numElements] = (strId | 0x80000000);
		*numElements += 1;
		return true;
	} else {
		err = DBPARSE_OTHER;
		return false;
	}
}

template <class T> bool CTPDatabase<T>::GetNamedItem(sint32 name, sint32 &index)
{
	sint32 i;
	for(i = 0; i < m_numRecords; i++) {
		if(name == m_records[i]->GetName()) {
			index = i;
			return true;
		}
	}
	return false;
}

template <class T> bool CTPDatabase<T>::GetNamedItem(const char *name, sint32 &index)
{
	sint32 strId;
	if(g_theStringDB->GetStringID(name, strId)) {
		return GetNamedItem(strId, index);
	}

	sint32 i;
	for(i = 0; i < m_numRecords; i++) {
		if(!stricmp(name, m_records[i]->GetNameText())) {
			index = i;
			return true;
		}
	}
	return false;
}

template <class T> bool CTPDatabase<T>::GetNamedItemID(sint32 index, sint32 &name)
{
	if(index < 0)
		return false;

	if(index >= m_numRecords)
		return true;

	name = m_records[index]->GetName();
	return true;
}

template <class T> bool CTPDatabase<T>::ResolveReferences()
{
	bool success = true;
	sint32 i;
	for(i = 0; i < m_numRecords; i++) {
		m_records[i]->ResolveDBReferences();
	}

	return success;
}

template <class T> sint32 CTPDatabase<T>::FindTypeIndex(const char *str) const
{
	sint32 i;
	for(i = 0; i < m_numRecords; i++) {
		if(stricmp(m_records[i]->GetNameText(), str) == 0) {
			return i;
		}
	}
	return -1;
}

template <class T> sint32 CTPDatabase<T>::FindRecordNameIndex(const char *str) const
{
	sint32 i;
	for(i = 0; i < m_numRecords; i++) {
		if(stricmp(m_records[i]->GetIDText(), str) == 0) {
			return i;
		}
	}
	return -1;
}

#include "IconRecord.h"
template class CTPDatabase<IconRecord>;

#include "SoundRecord.h"
template class CTPDatabase<SoundRecord>;

#include "TerrainRecord.h"
template class CTPDatabase<TerrainRecord>;

#include "ResourceRecord.h"
template class CTPDatabase<ResourceRecord>;

#include "AgeRecord.h"
template class CTPDatabase<AgeRecord>;

#include "AdvanceRecord.h"
template class CTPDatabase<AdvanceRecord>;

#include "AdvanceBranchRecord.h"
template class CTPDatabase<AdvanceBranchRecord>;


#include "FeatRecord.h"
template class CTPDatabase<FeatRecord>;


#include "WonderRecord.h"
template class CTPDatabase<WonderRecord>;

#include "WonderMovieRecord.h"
template class CTPDatabase<WonderMovieRecord>;


#include "BuildingRecord.h"
template class CTPDatabase<BuildingRecord>;

#ifndef __TILETOOL__

#include "UnitRecord.h"
template class CTPDatabase<UnitRecord>;

#include "SpriteRecord.h"
template class CTPDatabase<SpriteRecord>;

#include "GovernmentRecord.h"
template class CTPDatabase<GovernmentRecord>;

#include "SpecialAttackInfoRecord.h"
template class CTPDatabase<SpecialAttackInfoRecord>;

#include "SpecialEffectRecord.h"
template class CTPDatabase<SpecialEffectRecord>;

#include "TerrainImprovementRecord.h"
template class CTPDatabase<TerrainImprovementRecord>;

#include "OrderRecord.h"
template class CTPDatabase<OrderRecord>;

#include "GoalRecord.h"
template class CTPDatabase<GoalRecord>;

#include "UnitBuildListRecord.h"
template class CTPDatabase<UnitBuildListRecord>;

#include "BuildingBuildListRecord.h"
template class CTPDatabase<BuildingBuildListRecord>;

#include "WonderBuildListRecord.h"
template class CTPDatabase<WonderBuildListRecord>;

#include "ImprovementListRecord.h"
template class CTPDatabase<ImprovementListRecord>;

#include "StrategyRecord.h"
template class CTPDatabase<StrategyRecord>;

#include "BuildListSequenceRecord.h"
template class CTPDatabase<BuildListSequenceRecord>;

#include "DiplomacyRecord.h"
template class CTPDatabase<DiplomacyRecord>;

#include "AdvanceListRecord.h"
template class CTPDatabase<AdvanceListRecord>;

#include "CitySizeRecord.h"
template class CTPDatabase<CitySizeRecord>;

#include "PopRecord.h"
template class CTPDatabase<PopRecord>;

#include "DiplomacyProposalRecord.h"
template class CTPDatabase<DiplomacyProposalRecord>;

#include "DiplomacyThreatRecord.h"
template class CTPDatabase<DiplomacyThreatRecord>;

#include "PersonalityRecord.h"
template class CTPDatabase<PersonalityRecord>;

#include "EndGameObjectRecord.h"
template class CTPDatabase<EndGameObjectRecord>;

#include "CityStyleRecord.h"
template class CTPDatabase<CityStyleRecord>;

#include "AgeCityStyleRecord.h"
template class CTPDatabase<AgeCityStyleRecord>;

#endif 
