
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

template <class T> class CTPDatabase
{
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
