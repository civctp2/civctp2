//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Base DB Template class header
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
// - Removed an Assert which is annoyingly triggered all the time when having
//   an improvement file without sound defintions.
// - Removed some completely unused code.
// - Modernised some code: e.g. implemented the modified records list as a
//   std::vector, so we don't have to do the memory management ourselves.
// - Added Serialize method for datachecks. (Aug 23rd 2005 Martin Gühmann)
// - Parser for struct ADVANCE_CHANCES of DiffDB.txt can now be generated. (Jan 3rd 2006 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifndef CTPDATABASE_H__
#define CTPDATABASE_H__

#include <vector>

template <class T> class CTPDatabase;

#define DBERROR(x) { lex->ReportError x; }

enum DBPARSE_ERROR
{
	DBPARSE_OK,
	DBPARSE_DEFER,
	DBPARSE_OTHER,
};

#include "c3files.h"
#include "civarchive.h"
class DBLexer;
template <class T> class PointerList;

class GovernmentModifiedRecordNode
{
public:
	GovernmentModifiedRecordNode
	(
		sint32	a_governmentModified    = -1,
		sint32	a_modifiedRecord        = -1
	)
	:   m_governmentModified(a_governmentModified),
	    m_modifiedRecord(a_modifiedRecord)
	{ };
	void Serialize(CivArchive &archive){
		if(archive.IsStoring()) {
			archive << m_governmentModified;
			archive << m_modifiedRecord;
	} else {
			archive >> m_governmentModified;
			archive >> m_modifiedRecord;
		}
	}

	sint32 m_governmentModified;
	sint32 m_modifiedRecord;
};

template <class T> class CTPDatabase
{
protected:
	T **m_records;
	sint32 m_numRecords;
	sint32 m_allocatedSize;

	PointerList<GovernmentModifiedRecordNode> **m_modifiedList;
	std::vector<T *> m_modifiedRecords;

	void Grow();

public:
	sint32 *m_indexToAlpha;
	sint32 *m_alphaToIndex;

	CTPDatabase();
	virtual ~CTPDatabase();

	void Serialize(CivArchive &archive);

	bool Parse(DBLexer *lex);
	bool Parse(const C3DIR & c3dir, const char *filename);

	const T *Get(sint32 index)
	{
		Assert(index >= 0);
		Assert(index < m_numRecords);
		if((index < 0) || (index >= m_numRecords))
			return NULL;

		return m_records[index];
	}

	T *Access(sint32 index);
	T *Access(sint32 index, sint32 govIndex);
	const T *Get(sint32 index, sint32 govIndex);

	sint32 GetName(sint32 index);
	const char * GetNameStr(sint32 index);
	void Add(T *obj);
	sint32 NumRecords() const
	{
		return m_numRecords;
	}
	bool GetNamedItem(sint32 name, sint32 &index);
	bool GetNamedItemID(sint32 index, sint32 &name);
	bool GetNamedItem(const char *name, sint32 &index);

	sint32 FindTypeIndex(const char *str) const;
	sint32 FindRecordNameIndex(const char *str) const;

	bool GetRecordFromLexer(DBLexer *lex, sint32 &index);
	bool GetCurrentRecordFromLexer(DBLexer *lex, sint32 &index);
	bool ParseRecordInArray(DBLexer *lex, sint32 **array, sint32 *numElements);
	bool ParseRecordInArray(DBLexer *lex, sint32 *array, sint32 *numElements, sint32 maxSize);
	bool ResolveReferences();
};

#endif
