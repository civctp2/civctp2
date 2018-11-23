#ifndef __SLIC_SYM_TAB_H__
#define __SLIC_SYM_TAB_H__

#include "StringHash.h"
#include "SlicSymbol.h"
#include "SlicNamedSymbol.h"

#define k_SLIC_SYM_TAB_HASH_SIZE 256

class SlicSymTab : public StringHash<SlicNamedSymbol>
{
private:

	sint32 m_arraySize;
	sint32 m_numEntries;


	SlicNamedSymbol **m_array;

public:
	SlicSymTab(sint32 size);
	SlicSymTab(CivArchive &archive);
	~SlicSymTab();
	void Serialize(CivArchive &archive);
	void PostSerialize();

	void Add(SlicNamedSymbol *sym);
	void Add(sint32 index);
	const SlicNamedSymbol *Get(sint32 index) const;
	SlicNamedSymbol *Access(sint32 index);

	const sint32 GetSize() { return m_arraySize; }
	const sint32 GetNumEntries() { return m_numEntries; }

	void GrowBy(sint32 num);
	void Resize(sint32 num);
};

#endif
