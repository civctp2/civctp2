
#pragma once
#ifndef __SLIC_ARRAY_H__
#define __SLIC_ARRAY_H__

#include "slicif.h"
#include "SlicStack.h"

enum SS_TYPE;
class CivArchive;
class SlicStructDescription;

class SlicArray {
private:
	SS_TYPE m_type; 
	SLIC_SYM m_varType; 
	SlicStackValue *m_array;
	sint32 m_arraySize;
	sint32 m_allocatedSize;
	SlicStructDescription *m_structTemplate;
	bool m_sizeIsFixed;

public:
	SlicArray(SS_TYPE type, SLIC_SYM varType);
	SlicArray(SlicStructDescription *structTemplate);
	SlicArray(CivArchive &archive);
	~SlicArray();
	void Serialize(CivArchive &archive);

	void FixSize(sint32 size);

	SS_TYPE GetType() const { return m_type; }
	void SetType(SS_TYPE type, SLIC_SYM varType);

	BOOL Lookup(sint32 index, SS_TYPE &type, SlicStackValue &value);
	BOOL Insert(sint32 index, SS_TYPE type, SlicStackValue value);

	
	void Prune(sint32 size);

	SlicStructDescription *GetStructTemplate() { return m_structTemplate; }
	void SetStructTemplate(SlicStructDescription *desc) { m_structTemplate = desc; }

	sint32 GetSize() { return m_arraySize; }
};

#endif
