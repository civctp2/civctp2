//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Base DB Template class header
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
// - Implemented GovernmentsModified subclass (allowing cdb files including
//   a GovernmentsModified record to produce parsers capable of reading and
//   storing subrecords for Government types.)
//   See http://apolyton.net/forums/showthread.php?s=&threadid=107916 for
//   more details  _____ by MrBaggins Jan-04
//
//   * GovernmentsModified member variable definitions and member function
//     prototypes
//
//   * GovernmentModifiedRecordNode class definition (used in by an array of
//     pointers to a PointerList as a templated class) for relation of
//     m_modifiedRecords to m_records (by government index)
//
//----------------------------------------------------------------------------

#ifndef __CTPDATABASE_H__
#define __CTPDATABASE_H__

class DBLexer;
enum C3DIR;
template <class T> class PointerList;

#define DBERROR(x) { lex->ReportError x; }

enum DBPARSE_ERROR {
	DBPARSE_OK,
	DBPARSE_DEFER,
	DBPARSE_OTHER,
};

class GovernmentModifiedRecordNode
{
public:
	sint32 m_governmentModified;
	sint32 m_modifiedRecord;
};



template <class T> class CTPDatabase
{

#if !defined(ACTIVISION_ORIGINAL) //GovMod
protected:
	PointerList<GovernmentModifiedRecordNode> **m_modifiedList;


	sint32* m_recordsModifiedLink;
	sint32 m_numRecordsModifiedLink;
	sint32 m_allocatedRecordsModifiedLinkSize;

	sint32* m_modList;
	sint32 m_numModList;
	sint32 m_allocatedListSize;

	T **m_modifiedRecords;
	sint32 m_numModifiedRecords;
	sint32 m_allocatedModifiedSize;

  void GrowModified();

public:
	T *Access(sint32 index,sint32 govIndex);
	const T *Get(sint32 index,sint32 govIndex);
#endif



protected:
	T **m_records;
	sint32 m_numRecords;
	sint32 m_allocatedSize;

	void Grow();



public:
	sint32 *m_indexToAlpha;
	sint32 *m_alphaToIndex;

	CTPDatabase();
	virtual ~CTPDatabase();



	sint32 Parse(DBLexer *lex);
	sint32 Parse(const C3DIR & c3dir, const char *filename);

	
	
	
	inline const T *Get(sint32 index)
	{
		Assert(index >= 0);
		Assert(index < m_numRecords);
		if((index < 0) || (index >= m_numRecords))
			return NULL;
	
		return m_records[index];
	}

	T *Access(sint32 index);

	sint32 GetName(sint32 index);
	const char *GetNameStr(sint32 index);
	void Add(T *obj);
	inline sint32 NumRecords() const 
	{ 
		return m_numRecords; 
	}
	bool GetNamedItem(sint32 name, sint32 &index);
	bool GetNamedItemID(sint32 index, sint32 &name);
	bool GetNamedItem(const char *name, sint32 &index);

	sint32 FindTypeIndex(const char *str) const;
	sint32 FindRecordNameIndex(const char *str) const;

	bool GetRecordFromLexer(DBLexer *lex, sint32 &index, DBPARSE_ERROR &err);
	bool ParseRecordInArray(DBLexer *lex, sint32 **array, sint32 *numElements, DBPARSE_ERROR &err);
	bool ParseRecordInArray(DBLexer *lex, sint32 *array, sint32 *numElements, sint32 maxSize, DBPARSE_ERROR &err);
	bool ResolveReferences();
};

#endif
