//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : 
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
// _MSC_VER		
// - When defined, allows Microsoft C++ extensions.
// - When not defined, generates standard C++.
//
// Note: For the blocks with _MSC_VER preprocessor directives, the following
//       is implied: the (_MSC_VER) preprocessor directive lines and the blocks 
//       between #else and #endif are modified Apolyton code. The blocks 
//       between #if and #else are the original Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - pragma once marked as MS specific.
// - Accessor functionality added. 
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __SLIC_STRUCT_H__
#define __SLIC_STRUCH_H__

#if !defined(ACTIVISION)
#include <vector>		// std::vector
#endif

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

#if !defined(ACTIVISION_ORIGINAL)
	void AddAccessor(char * name, SlicStructMemberData * symbol);

	SlicStructMemberData * GetMemberSymbol(sint32 index) const;
	sint32 GetMemberSymbolIndex(SlicStructMemberData * symbol) const;
#endif

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
#if !defined(ACTIVISION_ORIGINAL)
	std::vector<Member *>			m_accessors;
#endif
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
