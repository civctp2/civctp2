//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Slic Structure
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Memory leaks repaired.
// - Accessor functionality added.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "SlicStruct.h"
#include "SlicSymbol.h"
#include "SlicEngine.h"
#include "SlicNamedSymbol.h"
#include "civarchive.h"
#include "gamefile.h"

SlicStructDescription::Member::Member(SlicStructDescription *parent, char const * name, SLIC_SYM type)
{
	m_parent = parent;
	m_name = new char[strlen(name) + 1];
	strcpy(m_name, name);

	m_type = type;
	m_symbol = NULL;
}

SlicStructDescription::Member::Member(SlicStructDescription *parent, char const * name, SlicStructMemberData *sym)
{
	m_parent = parent;
	m_name = new char[strlen(name) + 1];
	strcpy(m_name, name);

	m_type = sym->GetType();
	m_symbol = sym;

}

SlicStructDescription::Member::~Member()
{
	delete [] m_name;
	delete m_symbol;
}

SlicStructDescription::SlicStructDescription(char const * name, SLIC_BUILTIN type)
:	m_accessors()
{
	m_name = new char[strlen(name) + 1];
	m_type = type;
	strcpy(m_name, name);

	m_members = NULL;
	m_numMembers = 0;
}

SlicStructDescription::~SlicStructDescription()
{
	delete [] m_name;

	if(m_members) {
		sint32 i;
		for(i = 0; i < m_numMembers; i++) {
			if(m_members[i]) {
				delete m_members[i];
			}
		}
		delete [] m_members;
	}
	for
	(
		std::vector<Member *>::iterator	p = m_accessors.begin();
		p < m_accessors.end();
		++p
	)
	{
		delete *p;
	}

	m_accessors.clear();
}

//----------------------------------------------------------------------------
//
// Name       : SlicStructDescription::AddAccessor
//
// Description: Add an accessor member to a Slic structure.
//
// Parameters : name	: name of accessor
//				symbol	: information of the accessor
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : In contrast to members, accessors will not be saved to file.
//
//----------------------------------------------------------------------------

void SlicStructDescription::AddAccessor
(
	char const *			name,
	SlicStructMemberData *	symbol
)
{
	m_accessors.push_back(new Member(this, name, symbol));
}

//----------------------------------------------------------------------------
//
// Name       : SlicStructDescription::GetMemberSymbol
//
// Description: Get Slic structure symbol for a member.
//
// Parameters : index	: member number
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : When the member at the index is not found or NULL,
//              NULL is returned.
//
//----------------------------------------------------------------------------

SlicStructMemberData * SlicStructDescription::GetMemberSymbol(sint32 index) const
{
	Member * memberAtIndex = NULL;

	if (index < 0)
	{
		// Invalid index
	}
	else if (index < m_numMembers)
	{
		memberAtIndex = m_members[index];
	}
	else if ((unsigned) index < (m_numMembers + m_accessors.size()))
	{
		memberAtIndex = m_accessors[index - m_numMembers];
	}
	else
	{
		// Invalid index
	}

	return (memberAtIndex) ? memberAtIndex->m_symbol : NULL;
};

//----------------------------------------------------------------------------
//
// Name       : SlicStructDescription::GetMemberSymbolIndex
//
// Description: Get member index of a Slic structure symbol.
//
// Parameters : symbol	: Slic structure symbol
//
// Globals    : -
//
// Returns    : sint32	: member index
//
// Remark(s)  : - The index of the first matching member or accessor is
//                returned.
//              - When the name is not found, -1 is returned.
//              - The members and accessors are ordered on time of adding,
//                but the accessors are ordered behind the members. So,
//                members have indices starting from 0, and accessors have
//                indices starting from m_numMembers.
//
//----------------------------------------------------------------------------

sint32 SlicStructDescription::GetMemberSymbolIndex(SlicStructMemberData * symbol) const
{
	sint32	i;
	for (i = 0; i < m_numMembers; ++i)
	{
		if (symbol == m_members[i]->m_symbol)
		{
			return i;
		}
	}

	// Continue counting while checking the accessors
	for
	(
		std::vector<Member *>::const_iterator	p = m_accessors.begin();
		p < m_accessors.end();
		++p
	)
	{
		if (symbol == (*p)->m_symbol)
		{
			return i;
		}
		else
		{
			++i;	// not the symbol we are looking for
		}
	}

	return -1;
}

void SlicStructDescription::AddMember(SlicStructDescription::Member *member)
{

	m_numMembers++;
	if(!m_members) {

		Assert(m_numMembers == 1);
		m_members = new SlicStructDescription::Member *[m_numMembers];
	} else {

		SlicStructDescription::Member **oldmembers = m_members;
		m_members = new SlicStructDescription::Member *[m_numMembers];
		memcpy(m_members, oldmembers, (m_numMembers - 1) * sizeof(SlicStruct *));
		delete [] oldmembers;
	}

	m_members[m_numMembers - 1] = member;
}

void SlicStructDescription::AddMember(char const * name, SLIC_SYM type)
{
	AddMember(new SlicStructDescription::Member(this, name, type));
}

void SlicStructDescription::AddMember(char const * name, SlicStructMemberData *sym)
{
	AddMember(new SlicStructDescription::Member(this, name, sym));
}

//----------------------------------------------------------------------------
//
// Name       : SlicStructDescription::GetMemberIndex
//
// Description: Get index of member or accessor.
//
// Parameters : name	: name of member or accessor.
//
// Globals    : -
//
// Returns    : sint32	: index of member or accessor with given name.
//
// Remark(s)  : - The index of the first matching member or accessor is
//                returned.
//              - When the name is not found, -1 is returned.
//              - The members and accessors are ordered on time of adding,
//                but the accessors are ordered behind the members. So,
//                members have indices starting from 0, and accessors have
//                indices starting from m_numMembers.
//
//----------------------------------------------------------------------------
sint32 SlicStructDescription::GetMemberIndex(char const * name) const
{
	sint32 i;
	for(i = 0; i < m_numMembers; i++) {
		if(!stricmp(name, m_members[i]->m_name))
			return i;
	}

	// Continue counting while checking the accessors
	for
	(
		std::vector<Member *>::const_iterator	p = m_accessors.begin();
		p < m_accessors.end();
		++p
	)
	{
		if (stricmp(name, (*p)->m_name))
		{
			++i;	// not the name we are looking for
		}
		else
		{
			return i;
		}
	}

	return -1;
}

SlicSymbolData *SlicStructDescription::CreateInstance(SS_TYPE type, SlicStackValue value)
{
	SlicSymbolData *sym = new SlicSymbolData(new SlicStructInstance(this));
	sym->SetValueFromStackValue(type, value);
	return sym;
}

SlicSymbolData *SlicStructDescription::CreateInstance()
{
	SlicSymbolData *sym = new SlicSymbolData(new SlicStructInstance(this));
	return sym;
}

SlicSymbolData *SlicStructDescription::CreateDataSymbol()
{

	return NULL;
}

//----------------------------------------------------------------------------
//
// Name       : SlicStructDescription::GetMemberName
//
// Description: Get the name of a member or accessor.
//
// Parameters : index	: index of member or accessor.
//
// Globals    : -
//
// Returns    : const char *	: name of member or accessor with given index.
//
// Remark(s)  : - When the index is invalid, "<Error>" is returned.
//              - See GetMemberIndex for the numbering of the members and
//                accessors.
//
//----------------------------------------------------------------------------

const char * SlicStructDescription::GetMemberName(sint32 index) const
{
	Assert(index >= 0);
	if (index >= 0)
	{
		if (index < m_numMembers)
		{
			return m_members[index]->m_name;
		}
		else
		{
			size_t const	accessorIndex	= index - m_numMembers;
			if (accessorIndex < m_accessors.size())
			{
				return m_accessors[accessorIndex]->m_name;
			}
		}
	}

	return "<Error>";
}




SlicStructInstance::SlicStructInstance(SlicStructDescription *description, SlicSymbolData *dataSym)
{
	m_description = description;

	m_validIndexCount	=
		m_description->GetNumMembers() + m_description->GetNumAccessors();
	m_members			= new SlicStructMemberData *[m_validIndexCount];
	std::fill(m_members, m_members + m_validIndexCount, (SlicStructMemberData *) NULL);

	if(!dataSym) {
		m_dataSymbol = m_description->CreateDataSymbol();
		m_createdData = true;
	} else {
		m_dataSymbol = dataSym;
		m_createdData = false;
	}

	m_dataSymbolIndex = -1;
}

SlicStructInstance::SlicStructInstance(CivArchive &archive)
{
	Serialize(archive);
}

//----------------------------------------------------------------------------
//
// Name       : ~SlicStructInstance
//
// Description: Destructor
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
SlicStructInstance::~SlicStructInstance()
{
	if (m_createdData)
    {
		delete m_dataSymbol;
	}

	if(m_members) {
		for (size_t i = 0; i < m_validIndexCount; ++i)
		{
			delete m_members[i];
		}
		delete [] m_members;
	}

}

void SlicStructInstance::Serialize(CivArchive &archive)
{
	sint32 i;
	uint8 haveData;
	if(archive.IsStoring()) {
		archive.PutUINT8(m_description->GetType());
		// Store the members. Don't store the accessors.
		for(i = 0; i < m_description->GetNumMembers(); i++) {
			haveData = m_members[i] != NULL;
			archive << haveData;
			if(haveData) {
				m_members[i]->SlicSymbolData::Serialize(archive);
			}
		}
		haveData = m_dataSymbol != NULL;
		archive << haveData;
		if(haveData) {
			archive.PutUINT8((uint8)m_createdData);
			if(m_createdData) {
				m_dataSymbol->SlicSymbolData::Serialize(archive);
			} else {

				SlicNamedSymbol *sym = (SlicNamedSymbol *)m_dataSymbol;
				archive.PutSINT32(sym->GetIndex());
			}
		}
		archive << m_dataSymbolIndex;
	} else {
		m_description = g_slicEngine->GetStructDescription((SLIC_BUILTIN)archive.GetUINT8());
		m_validIndexCount	=
			m_description->GetNumMembers() + m_description->GetNumAccessors();
		m_members			= new SlicStructMemberData *[m_validIndexCount];
		std::fill(m_members, m_members + m_validIndexCount, (SlicStructMemberData *) NULL);
		for(i = 0; i < m_description->GetNumMembers(); i++) {
			SlicStructDescription::Member *memDesc = m_description->m_members[i];
			archive >> haveData;
			if(haveData) {
				CreateMember(i);
				(SlicStructMemberData *)slicsymbol_Load(archive, m_members[i]);
				m_members[i]->SetParent(this);
			}
		}
		archive >> haveData;
		if(haveData) {
			m_createdData = archive.GetUINT8() != 0;
			if(m_createdData) {
				m_dataSymbol = slicsymbol_Load(archive, NULL);
			} else {
				m_dataSymbolIndex = archive.GetSINT32();
				m_dataSymbol = NULL;
			}
		} else {
			m_dataSymbol = NULL;
		}

		if(g_saveFileVersion >= 64) {
			archive >> m_dataSymbolIndex;
		} else {
			m_dataSymbolIndex = -1;
		}

		if (!m_dataSymbol && (m_dataSymbolIndex < 0))
        {
			m_dataSymbol    = m_description->CreateDataSymbol();
            m_createdData   = true;
		}

	}
}

//----------------------------------------------------------------------------
//
// Name       : SlicStructInstance::CreateMember
//
// Description: Create a new member instance when not already existing.
//
// Parameters : index	: member number
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Assumption: index is valid.
//
//				Cache-like functionality: a clone of the descriptor data will
//				be created upon first entry. If the clone has already been
//              created before, this function does nothing.
//
//----------------------------------------------------------------------------

void SlicStructInstance::CreateMember(sint32 index)
{
	if(m_members[index])
		return;	// No action: cache exists.

	SlicStructDescription::Member *	memDesc	=
		(index < m_description->GetNumMembers())
		? m_description->m_members[index]
		: m_description->m_accessors[index - m_description->GetNumMembers()];

	if(memDesc->m_symbol) {
		m_members[index] = memDesc->m_symbol->MakeCopy(this);
	} else {
		m_members[index] = new SlicStructMemberData(this, memDesc->m_type);
	}
}

SlicSymbolData *SlicStructInstance::GetMemberSymbol(sint32 index)
{
	Assert(index >= 0);
	Assert((unsigned) index < m_validIndexCount);
	if ((index < 0) || ((unsigned) index >= m_validIndexCount))
	{
		return NULL;
	}
	else
	{
		CreateMember(index);
	}

	return m_members[index];
}

sint32 SlicStructInstance::GetMemberSymbolIndex(SlicStructMemberData *memb)
{
	for (size_t i = 0; i < m_validIndexCount; ++i)
	{
		if(m_members[i] == memb)
			return i;
	}

	return -1;
}

SlicSymbolData *SlicStructInstance::GetMemberSymbolByName(char *name)
{
	sint32 const	index	= m_description->GetMemberIndex(name);
	if ((index < 0) || ((unsigned) index >= m_validIndexCount))
	{
		return NULL;
	}

	CreateMember(index);
	return m_members[index];
}

SlicSymbolData *SlicStructInstance::GetDataSymbol()
{
	if(!m_dataSymbol) {
		if(m_dataSymbolIndex >= 0) {
			m_dataSymbol = g_slicEngine->GetSymbol(m_dataSymbolIndex);
			m_dataSymbolIndex = -1;
		}
	}
	return m_dataSymbol;
}
void SlicStructMemberData::SerializeMemberReference(CivArchive &archive)
{
	Assert(archive.IsStoring());
	archive.PutSINT32((sint32)m_parent->GetDescription()->GetType());
	sint32 index = m_parent->GetMemberSymbolIndex(this);
	Assert(index >= 0);
	archive.PutSINT32(index);
}
