//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Base DB Template class
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
// __TILETOOL__
// - Probably supposed to generate the tool for creating the *.til files.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Implemented GovernmentsModified subclass (allowing cdb files including
//   a GovernmentsModified record to produce parsers capable of reading and
//   storing subrecords for Government types.)
//   See http://apolyton.net/forums/showthread.php?s=&threadid=107916 for
//   more details  _____ by MrBaggins Jan-04
//
//   * Added m_modifiedList member variable (array of pointers to PointerList
//     of a new class GovernmentModifiedRecordNode, which has (only) 2 member
//     variables, to store which government goes with which subrecord.  The
//     array subscript is synonymous with the actual record (identical
//     m_record index.)  Please note that its a pointer to an array, not a
//     direct array.
//
//   * Added m_modifiedRecords member variable (array of pointers to template
//     class.)  This is the container for the subrecords.  The Add member
//     function determines if the templated class has
//     m_hasGovernmentsModified set to yes, (via the Generic accessor created
//     via ctpdb.exe, and ultimately from RecordDescription.cpp,) and if the
//     parsed record contains a number (more than 0) of valid
//     governments in its GovernmentsModified properties.  If so it adds
//     the templated class into the m_modifiedRecords member variable, rather
//     than m_records.  It also creates a list of links, by inserting each
//     government index, and the m_modifiedRecords index at the head of the
//     m_modifiedList pointer list (at the subscript of the appropriate
//     m_records.)
//
//   * m_numModifiedRecords, and m_allocatedModifiedSize are auxilary member
//     variables to aid in insertion and dynamic growth (of m_modifiedRecords)
//     
//   * Included memory destructors and dynamic growth code for new member
//     variables
//
//   * Added overloaded public accessor member functions, which take the
//     subscript of the m_records requested, and the government to search for
//     if a government is found in the pointer list, then the applicable
//     m_modifiedRecords object is returned, otherwise the regular
//     m_records object is returned (as per normal.)
//
// - Repaired memory leaks.
// - Removed some completely unused code.
// - Modernised some code: e.g. implemented the modified records list as a 
//   std::vector, so we don't have to do the memory management ourselves. 
// - Prevented crash in Parse when m_numrecords is 0.
// - Added the new civilisation database. (Aug 20th 2005 Martin Gühmann)
// - Added Serialize method for datachecks. (Aug 23rd 2005 Martin Gühmann)
// - Records can now be also parsed as quoted string. (Aug 26th 2005 Martin Gühmann)
// - The new databases can now be ordered alphabethical like the old ones. (Aug 26th 2005 Martin Gühmann)
// - Added the new risk database. (Aug 29th 2005 Martin Gühmann)
// - Parser for struct ADVANCE_CHANCES of DiffDB.txt can now be generated. (Jan 3rd 2006 Martin Gühmann)
// - If database records have no name a default name is generated. e.g.
//   DIFFICULTY_5 for the sixth entry in the DifficultyDB. (Jan 3rd 2006 Martin Gühman)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "CTPDatabase.h"
#include "DBLexer.h"
#include "DBTokens.h"
#include "StrDB.h"
#include "pointerlist.h"

#include "GovernmentRecord.h"

#define k_INITIAL_DB_SIZE 10
#define k_GROW_DB_STEP 10


template <class T> CTPDatabase<T>::CTPDatabase()
:
    m_numRecords        (0),
    m_modifiedRecords   (),
    m_indexToAlpha      (NULL),
    m_alphaToIndex      (NULL),
    m_allocatedSize     (k_INITIAL_DB_SIZE)
{
    m_records       = new T *[m_allocatedSize];
    m_modifiedList  = new PointerList<GovernmentModifiedRecordNode> *[m_allocatedSize];
}


template <class T> CTPDatabase<T>::~CTPDatabase()
{
	if (m_records) 
	{
		for (sint32 i = 0; i < m_numRecords; i++) 
		{
			delete m_records[i];
		}
		delete [] m_records;
	}

	delete [] m_indexToAlpha;
	delete [] m_alphaToIndex;


    for 
    (
        std::vector<T *>::iterator p = m_modifiedRecords.begin();
        p != m_modifiedRecords.end();
        ++p
    )
    {
        delete *p;
    }
    std::vector<T *>().swap(m_modifiedRecords);

	if (m_modifiedList) 
	{
		for (sint32 k = 0; k < m_numRecords; k++) 
		{
			if (m_modifiedList[k]) 
			{
				m_modifiedList[k]->DeleteAll();
				delete m_modifiedList[k];
			}
		}
		delete [] m_modifiedList;
	}
}

//----------------------------------------------------------------------------
//
// Name       : CTPDatabase<T>::Serialize
//
// Description: Store/Load CTPDatabase<T>
//
// Parameters : CivArchive &archive       :
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Does not Serialize the government modified stuff.
//              Fortunately these database serialize methods are 
//              not thought for loading, just for database check.
//              But this is on the TODO list.
//
//----------------------------------------------------------------------------
template <class T> void CTPDatabase<T>::Serialize(CivArchive &archive)
{
	sint32 i;

	if(archive.IsStoring()) {
		archive << m_allocatedSize;
		archive << m_numRecords;
		for(i = 0; i < m_numRecords; ++i){
			m_records[i]->Serialize(archive);
		}
	} else {
		archive >> m_allocatedSize;
		archive >> m_numRecords;

		delete [] m_indexToAlpha;
		delete [] m_alphaToIndex;
		m_indexToAlpha = (m_numRecords > 0) ? new sint32[m_numRecords] : NULL;
		m_alphaToIndex = (m_numRecords > 0) ? new sint32[m_numRecords] : NULL;
		m_records = new T *[m_allocatedSize];

		for (i = 0; i < m_numRecords; ++i){
			m_indexToAlpha[i]	= i;
			m_alphaToIndex[i]	= i;
			m_records[i] = new T(archive);
		}

		memset(m_indexToAlpha, 0, sizeof(sint32) * m_numRecords);
		memset(m_alphaToIndex, 0, sizeof(sint32) * m_numRecords);

		for(i = 0; i < m_numRecords; ++i){
			const MBCHAR *str = m_records[i]->GetNameText();
			sint32 a;
			for (a = 0; a < i; ++a)
			{
				if(_stricoll(str, m_records[m_alphaToIndex[a]]->GetNameText()) < 0)
				{
					memmove(
						m_alphaToIndex + a + 1,
						m_alphaToIndex + a,
						(i - a) * sizeof(sint32));

					for(sint32 j = 0; j < i; ++j)
						if(m_indexToAlpha[j] >= a)
							++m_indexToAlpha[j];

					break;

				}
			}
			m_alphaToIndex[a] = i;
			m_indexToAlpha[i] = a;
		}
	}
}

template <class T> T *CTPDatabase<T>::Access(sint32 index, sint32 govIndex)
{
	sint32 const    numberGovernmentRecords = 
		g_theGovernmentDB ? g_theGovernmentDB->NumRecords() : 0;

	Assert(index >= 0);
	Assert(index < m_numRecords);
	Assert(govIndex >= 0);
	Assert(govIndex < numberGovernmentRecords);
	
	if((index < 0) || (index >= m_numRecords) || (govIndex < 0) || (govIndex >= numberGovernmentRecords))
		return NULL;

	T *     result  = m_records[index]; // generic value (default)

	// Check for govermnent spefic overrides
	PointerList<GovernmentModifiedRecordNode>::Walker   walk = 
	    PointerList<GovernmentModifiedRecordNode>::Walker(m_modifiedList[index]);

	for (bool found = false; walk.IsValid() && (!found); walk.Next()) 
	{
		sint32 const    thisIndex = walk.GetObj()->m_governmentModified;
		
		if (thisIndex == govIndex)
		{
			result  = m_modifiedRecords[walk.GetObj()->m_modifiedRecord];
			found   = true;
		}
	}

	return result;
}


template <class T> const T * CTPDatabase<T>::Get(sint32 index,sint32 govIndex)
{
	return const_cast<const T *>(Access(index, govIndex));
}


template <class T> void CTPDatabase<T>::Grow()
{

	PointerList<GovernmentModifiedRecordNode> **oldList = m_modifiedList;
	m_modifiedList = new PointerList<GovernmentModifiedRecordNode> *[m_allocatedSize + k_GROW_DB_STEP];
	memcpy(m_modifiedList, oldList, m_allocatedSize * sizeof(PointerList<GovernmentModifiedRecordNode> *));
	delete [] oldList;	

  

	T **oldRecords = m_records;
	m_records = new T *[m_allocatedSize + k_GROW_DB_STEP];
	memcpy(m_records, oldRecords, m_allocatedSize * sizeof(T *));
	delete [] oldRecords;
	m_allocatedSize += k_GROW_DB_STEP;



}


template <class T> void CTPDatabase<T>::Add(T *obj)
{
	if (obj->GetHasGovernmentsModified() && 
	    (obj->GenericGetNumGovernmentsModified() > 0)
	   )
	{
		sint32 numberGovernmentRecords;
		if (g_theGovernmentDB)
		{
			numberGovernmentRecords=g_theGovernmentDB->NumRecords();
		}
		else 
		{
			numberGovernmentRecords=0;
			DPRINTF(k_DBG_FIX, ("GovMod- No Government Records \n"));
		}


		sint32 validIndex = 0;
		for (sint32 j = 0; j < obj->GenericGetNumGovernmentsModified(); j++)
		{
			if ((obj->GenericGetGovernmentsModifiedIndex(j) >= 0) && 
				(obj->GenericGetGovernmentsModifiedIndex(j) < numberGovernmentRecords)
			   )
			{
				validIndex++;
			}
		}
	  
		sint32 mainRecord=FindRecordNameIndex(obj->GetIDText());
		if ((mainRecord >= 0) && (validIndex > 0))
		{
			// Add the new object to the list of modified records.
			sint32 const	newIndex	= m_modifiedRecords.size();	
			obj->SetIndex(newIndex);
			m_modifiedRecords.push_back(obj);

			// Add references to the modified list
			if (!m_modifiedList[mainRecord])
			{
				m_modifiedList[mainRecord] = new PointerList<GovernmentModifiedRecordNode>;
			}

			for (sint32 i = 0; i < obj->GenericGetNumGovernmentsModified(); i++)
			{
				if ((obj->GenericGetGovernmentsModifiedIndex(i) >= 0) && 
					(obj->GenericGetGovernmentsModifiedIndex(i) < numberGovernmentRecords)
				   ) 
				{
					DPRINTF(k_DBG_FIX, ("GovMod- Adding modified record %s, Gov Index %d \n",obj->GetIDText(),obj->GenericGetGovernmentsModifiedIndex(i)));
					m_modifiedList[mainRecord]->AddHead
						(new GovernmentModifiedRecordNode
							(obj->GenericGetGovernmentsModifiedIndex(i), newIndex)
						);
				}
			}
		}
		else
		{
			DPRINTF(k_DBG_FIX, ("GovMod- No main record, or no valid GovernmentsModified %s \n",obj->GetIDText()));
		}
	}
	else 
	{

		if (m_numRecords >= m_allocatedSize)
			Grow();
		Assert(m_numRecords < m_allocatedSize);
		obj->SetIndex(m_numRecords);
		m_records[m_numRecords] = obj;


		m_modifiedList[m_numRecords] = new PointerList<GovernmentModifiedRecordNode>();
		m_modifiedList[m_numRecords]->AddHead(new GovernmentModifiedRecordNode());
		m_numRecords++;
		

	}

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

//----------------------------------------------------------------------------
//
// Name       : CTPDatabase<T>::Parse
//
// Description: Parses the data from text files into the data structures.
//
// Parameters : DBLexer *lex: The lexer used to parse the data.
//
// Globals    : -
//
// Returns    : 1 if the database was parsed successfully otherwise 0.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
template <class T> sint32 CTPDatabase<T>::Parse(DBLexer *lex)
{
	sint32 isOk = 1;

	while (!lex->EndOfInput())
	{
		T * obj = new T();

		if (obj->Parse(lex, m_numRecords))
		{
			Add(obj);
		}
		else
		{
			delete obj;
			isOk = 0;
		}
	}

	delete [] m_indexToAlpha;
	delete [] m_alphaToIndex;
	m_indexToAlpha = (m_numRecords > 0) ? new sint32[m_numRecords] : NULL;
	m_alphaToIndex = (m_numRecords > 0) ? new sint32[m_numRecords] : NULL;

	memset(m_indexToAlpha, 0, sizeof(sint32) * m_numRecords);
	memset(m_alphaToIndex, 0, sizeof(sint32) * m_numRecords);

	// A merge sort algorithm is of course better, but the complexity
	// is the same as for the old databases even the constant is the same.
	for(sint32 i = 0; i < m_numRecords; ++i){
		const MBCHAR *str = m_records[i]->GetNameText();
		sint32 a;
		for (a = 0; a < i; ++a)
		{
			if(_stricoll(str, m_records[m_alphaToIndex[a]]->GetNameText()) < 0)
			{
				memmove(
					m_alphaToIndex + a + 1,
					m_alphaToIndex + a,
					(i - a) * sizeof(sint32));

				
				for(sint32 j = 0; j < i; ++j)
					if(m_indexToAlpha[j] >= a)
						++m_indexToAlpha[j];

				break;

			}
		}
		m_alphaToIndex[a] = i;
		m_indexToAlpha[i] = a;
	}

	return isOk;
}

template <class T> sint32 CTPDatabase<T>::Parse(const C3DIR & c3dir, const char *filename)
{
	DBLexer *       lex     = new DBLexer(c3dir, filename);
	sint32 const    result  = Parse(lex);
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
		else if(tok != k_Token_String){
			DBERROR(("Expected record name1"));
			err = DBPARSE_OTHER;
			return false;
		}
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

template <class T> bool CTPDatabase<T>::GetCurrentRecordFromLexer(DBLexer *lex, sint32 &index, DBPARSE_ERROR &err)
{
	err = DBPARSE_OK;

	sint32 tok = lex->GetCurrentToken();
	if(tok != k_Token_Name) {
		if(tok == k_Token_Int) {
			index = atoi(lex->GetTokenText());
			return true;
		}
		else if(tok != k_Token_String){
			DBERROR(("Expected record name1"));
			err = DBPARSE_OTHER;
			return false;
		}
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
		DBERROR(("Expected record name3"));
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

#include "IconRecord.h" // 0
template class CTPDatabase<IconRecord>;

#include "SoundRecord.h" // 1
template class CTPDatabase<SoundRecord>;

#include "TerrainRecord.h" // 2
template class CTPDatabase<TerrainRecord>;

#include "ResourceRecord.h" // 3
template class CTPDatabase<ResourceRecord>;

#include "AgeRecord.h" // 4
template class CTPDatabase<AgeRecord>;

#include "AdvanceRecord.h" // 5
template class CTPDatabase<AdvanceRecord>;

#include "AdvanceBranchRecord.h" // 6
template class CTPDatabase<AdvanceBranchRecord>;

#include "FeatRecord.h" // 7
template class CTPDatabase<FeatRecord>;

#include "WonderRecord.h" // 8
template class CTPDatabase<WonderRecord>;

#include "WonderMovieRecord.h" // 9
template class CTPDatabase<WonderMovieRecord>;

#include "BuildingRecord.h" // 10
template class CTPDatabase<BuildingRecord>;

#ifndef __TILETOOL__

#include "UnitRecord.h" // 11
template class CTPDatabase<UnitRecord>;

#include "SpriteRecord.h" // 12
template class CTPDatabase<SpriteRecord>;

#include "GovernmentRecord.h" // 13
template class CTPDatabase<GovernmentRecord>;

#include "SpecialAttackInfoRecord.h" // 14
template class CTPDatabase<SpecialAttackInfoRecord>;

#include "SpecialEffectRecord.h" // 15
template class CTPDatabase<SpecialEffectRecord>;

#include "TerrainImprovementRecord.h" // 26
template class CTPDatabase<TerrainImprovementRecord>;

#include "OrderRecord.h" // 17
template class CTPDatabase<OrderRecord>;

#include "GoalRecord.h" // 18
template class CTPDatabase<GoalRecord>;

#include "UnitBuildListRecord.h" // 19
template class CTPDatabase<UnitBuildListRecord>;

#include "BuildingBuildListRecord.h" // 20
template class CTPDatabase<BuildingBuildListRecord>;

#include "WonderBuildListRecord.h" // 21
template class CTPDatabase<WonderBuildListRecord>;

#include "ImprovementListRecord.h" // 22
template class CTPDatabase<ImprovementListRecord>;

#include "StrategyRecord.h" // 23
template class CTPDatabase<StrategyRecord>;

#include "BuildListSequenceRecord.h" // 24
template class CTPDatabase<BuildListSequenceRecord>;

#include "DiplomacyRecord.h" // 25
template class CTPDatabase<DiplomacyRecord>;

#include "AdvanceListRecord.h" // 26
template class CTPDatabase<AdvanceListRecord>;

#include "CitySizeRecord.h" // 27
template class CTPDatabase<CitySizeRecord>;

#include "PopRecord.h" // 28
template class CTPDatabase<PopRecord>;

#include "DiplomacyProposalRecord.h" // 29
template class CTPDatabase<DiplomacyProposalRecord>;

#include "DiplomacyThreatRecord.h" // 30
template class CTPDatabase<DiplomacyThreatRecord>;

#include "PersonalityRecord.h" // 31
template class CTPDatabase<PersonalityRecord>;

#include "EndGameObjectRecord.h" // 32
template class CTPDatabase<EndGameObjectRecord>;

#include "CityStyleRecord.h" // 33
template class CTPDatabase<CityStyleRecord>;

#include "AgeCityStyleRecord.h" // 34
template class CTPDatabase<AgeCityStyleRecord>;

#include "CivilisationRecord.h" // 35
template class CTPDatabase<CivilisationRecord>;

#include "RiskRecord.h" // 36
template class CTPDatabase<RiskRecord>;

#include "DifficultyRecord.h" // 37
template class CTPDatabase<DifficultyRecord>;
#endif // __TILETOOL__
