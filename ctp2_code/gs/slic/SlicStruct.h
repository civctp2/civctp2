//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Slic Structure
// Id           : $Id:$
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
// - Standardized code. (May 29th 2006 Martin Gühmann) 
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __SLIC_STRUCT_H__
#define __SLIC_STRUCT_H__ 1

#include <vector>		// std::vector

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
		Member(SlicStructDescription *parent, char const * name, SLIC_SYM type);
		Member(SlicStructDescription *parent, char const * name, SlicStructMemberData *sym);
		~Member();

		SlicStructDescription *GetParent() { return m_parent; }
		
	private:
		friend class SlicStructDescription;
		friend class SlicStructInstance;

		SLIC_SYM m_type;
		char *m_name;
		class SlicStructDescription *m_parent;
		class SlicStructMemberData *m_symbol; 
	};

	SlicStructDescription(char const * name, SLIC_BUILTIN type);
	virtual ~SlicStructDescription();

	void AddAccessor(char const * name, SlicStructMemberData * symbol);

	SlicStructMemberData * GetMemberSymbol(sint32 index) const;
	sint32 GetMemberSymbolIndex(SlicStructMemberData * symbol) const;
	size_t GetNumAccessors(void) const 
		{ return m_accessors.size(); };

	void AddMember(SlicStructDescription::Member *member);
	void AddMember(char const * name, SLIC_SYM type);
	void AddMember(char const * name, SlicStructMemberData *sym);
	const char * GetName() const { return m_name; }
	sint32 GetMemberIndex(char const * name) const;
	const char *GetMemberName(sint32 index) const;
	sint32 GetNumMembers() { return m_numMembers; }

	SLIC_BUILTIN GetType() const { return m_type; }

	SlicSymbolData *CreateInstance(SS_TYPE type, SlicStackValue value);
	SlicSymbolData *CreateInstance();
	


	virtual SlicSymbolData *CreateDataSymbol();

	friend class SlicStructInstance;
	friend SlicSymbolData *slicsymbol_Load(CivArchive &archive);
private:
	char *m_name;
	SLIC_BUILTIN m_type;
	SlicStructDescription::Member **m_members;
	sint32 m_numMembers;
	std::vector<Member *>			m_accessors;
};





class SlicStructMemberData : public SlicSymbolData
{
protected:
	SlicStructInstance *m_parent;

public:

	SlicStructMemberData
    (
        SlicStructInstance *    parent, 
        SlicSymbolData const &  data
    ) 
    :   SlicSymbolData  (data),
        m_parent        (parent)
    { ; }

	SlicStructMemberData
	(
		SlicStructInstance *	parent	= NULL, 
		SLIC_SYM				type	= SLIC_SYM_UNDEFINED
	)
	:	SlicSymbolData  (type),
		m_parent        (parent)
	{ ; };

	virtual ~SlicStructMemberData() {}

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
	size_t				m_validIndexCount;	// members + accessors
};

#endif
