#pragma once
#ifndef __SLIC_STRUCT_H__
#define __SLIC_STRUCH_H__



#include "slicif.h"
#include "SlicStack.h"
#include "SlicSymbol.h"
#include "SlicBuiltinEnum.h"

class SlicStruct;
class SlicSymbolData;
class SlicNamedSymbol;
class SlicStructInstance;
class SlicStructMemberData;







class SlicStructDescription {
public:
	class Member {
	public:
		Member(SlicStructDescription *parent, char *name, SLIC_SYM type);
		Member(SlicStructDescription *parent, char *name, SlicStructMemberData *sym);
		~Member();

		SlicStructDescription *GetParent() { return m_parent; }
		
	private:
		friend class SlicStructDescription;
		friend class SlicStructInstance;

		SLIC_SYM m_type;
		char *m_name;
		SlicStructDescription *m_parent;
		SlicStructMemberData *m_symbol; 
	};

	SlicStructDescription(char *name, SLIC_BUILTIN type);
	~SlicStructDescription();

	void AddMember(SlicStructDescription::Member *member);
	void AddMember(char *name, SLIC_SYM type);
	void AddMember(char *name, SlicStructMemberData *sym);
	const char *GetName() { return m_name; }
	sint32 GetMemberIndex(char *name);
	const char *GetMemberName(sint32 index);
	sint32 GetNumMembers() { return m_numMembers; }

	SLIC_BUILTIN GetType() { return m_type; }

	SlicSymbolData *CreateInstance(SS_TYPE type, SlicStackValue value);
	SlicSymbolData *CreateInstance();
	


	virtual SlicSymbolData *CreateDataSymbol();

	friend class SlicStructInstance;
	friend slicsymbol_Load(CivArchive &archive);
private:
	char *m_name;
	SLIC_BUILTIN m_type;
	SlicStructDescription::Member **m_members;
	sint32 m_numMembers;
};





class SlicStructMemberData : public SlicSymbolData
{
protected:
	SlicStructInstance *m_parent;

public:
	SlicStructMemberData() {
		m_parent = NULL;
	}

	SlicStructMemberData(SlicStructInstance *parent, SlicSymbolData *data) :
		SlicSymbolData(data)
	{
		m_parent = parent;
	}

	SlicStructMemberData(SlicStructInstance *parent, SLIC_SYM type) :
		SlicSymbolData(type)
	{
		m_parent = parent;
	}

	~SlicStructMemberData() {}

	void Serialize(CivArchive &archive) {}
	void SerializeMemberReference(CivArchive &archive);
	virtual SLIC_SYM_SERIAL_TYPE GetSerializeType() { return SLIC_SYM_SERIAL_MEMBER; }
	SLIC_SYM GetType() const { return SLIC_SYM_STRUCT_MEMBER; }
	SlicStructInstance *GetParent() { return m_parent; }
	void SetParent(SlicStructInstance *parent) { m_parent = parent; }

	virtual SlicStructMemberData *MakeCopy(SlicStructInstance *parent) {
		return new SlicStructMemberData(parent, m_type);
	}
};



class SlicStructInstance {
public:
	SlicStructInstance(SlicStructDescription *desc, SlicSymbolData *dataSym = NULL);
	SlicStructInstance(CivArchive &archive);
	~SlicStructInstance();
	void Serialize(CivArchive &archive);
	


	SlicSymbolData *GetMemberSymbol(sint32 memberIndex);
	SlicSymbolData *GetMemberSymbolByName(char *name);
	
	SlicStructDescription *GetDescription() { return m_description; }
	SlicSymbolData *GetDataSymbol();
	sint32 GetMemberSymbolIndex(SlicStructMemberData *memb);

private:
	void CreateMember(sint32 index);

	SlicStructDescription *m_description;
	SlicStructMemberData **m_members;
	SlicSymbolData *m_dataSymbol;
	sint32 m_dataSymbolIndex;
	bool m_createdData;
};

#endif
