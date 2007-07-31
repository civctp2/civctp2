


#include "c3.h"
#include "SlicStruct.h"
#include "SlicSymbol.h"
#include "SlicEngine.h"
#include "SlicNamedSymbol.h"
#include "CivArchive.h"
#include "GameFile.h"

SlicStructDescription::Member::Member(SlicStructDescription *parent, char *name, SLIC_SYM type)
{
	m_parent = parent;
	m_name = new char[strlen(name) + 1];
	strcpy(m_name, name);

	m_type = type;
	m_symbol = NULL;
}

SlicStructDescription::Member::Member(SlicStructDescription *parent, char *name, SlicStructMemberData *sym)
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

SlicStructDescription::SlicStructDescription(char *name, SLIC_BUILTIN type)
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

void SlicStructDescription::AddMember(char *name, SLIC_SYM type)
{
	AddMember(new SlicStructDescription::Member(this, name, type));
}

void SlicStructDescription::AddMember(char *name, SlicStructMemberData *sym)
{
	AddMember(new SlicStructDescription::Member(this, name, sym));
}
	
sint32 SlicStructDescription::GetMemberIndex(char *name)
{
	sint32 i;
	for(i = 0; i < m_numMembers; i++) {
		if(!stricmp(name, m_members[i]->m_name))
			return i;
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

const char *SlicStructDescription::GetMemberName(sint32 index)
{
	Assert(index >= 0);
	Assert(index < m_numMembers);
	if(index < 0 || index >= m_numMembers)
		return "<Error>";
	
	return m_members[index]->m_name;
}
	



SlicStructInstance::SlicStructInstance(SlicStructDescription *description, SlicSymbolData *dataSym)
{
	m_description = description;

	
	m_members = new SlicStructMemberData *[m_description->m_numMembers];
	sint32 i;
	for(i = 0; i < m_description->m_numMembers; i++) {
		m_members[i] = NULL;
	}

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

SlicStructInstance::~SlicStructInstance()
{
	if(m_dataSymbol && m_createdData) {
		delete m_dataSymbol;
		m_dataSymbol = NULL;
	}

	if(m_members) {
		sint32 i;
		for(i = 0; i < m_description->GetNumMembers(); i++) {
			if(m_members[i]) {
				delete m_members[i];
				m_members[i] = NULL;
			}
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
		m_members = new SlicStructMemberData *[m_description->GetNumMembers()];
		for(i = 0; i < m_description->GetNumMembers(); i++) {
			SlicStructDescription::Member *memDesc = m_description->m_members[i];
			m_members[i] = NULL;
			archive >> haveData;
			if(haveData) {
				CreateMember(i);
				(SlicStructMemberData *)slicsymbol_Load(archive, m_members[i]);
				m_members[i]->SetParent(this);
			} else {
				m_members[i] = NULL;
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
			if(!m_dataSymbol) {
				m_dataSymbol = m_description->CreateDataSymbol();
			}
		}
		if(!m_dataSymbol && m_dataSymbolIndex < 0) {
			m_dataSymbol = m_description->CreateDataSymbol();
		}

	}
}

void SlicStructInstance::CreateMember(sint32 index)
{
	if(m_members[index])
		return;

	SlicStructDescription::Member *memDesc = m_description->m_members[index];
	if(memDesc->m_symbol) {
		m_members[index] = memDesc->m_symbol->MakeCopy(this);
	} else {
		m_members[index] = new SlicStructMemberData(this, memDesc->m_type);
	}
}

SlicSymbolData *SlicStructInstance::GetMemberSymbol(sint32 index)
{
	Assert(index >= 0);
	Assert(index < m_description->m_numMembers);
	if(index < 0 || index >= m_description->m_numMembers)
		return NULL;

	if(!m_members[index]) {
		CreateMember(index);
	}

	return m_members[index];
}

sint32 SlicStructInstance::GetMemberSymbolIndex(SlicStructMemberData *memb)
{
	sint32 i;
	for(i = 0; i < m_description->m_numMembers; i++) {
		if(m_members[i] == memb)
			return i;
	}
	return -1;
}

SlicSymbolData *SlicStructInstance::GetMemberSymbolByName(char *name)
{
	sint32 i;
	for(i = 0; i < m_description->m_numMembers; i++) {
		if(!stricmp(m_description->m_members[i]->m_name, name)) {
			if(!m_members[i]) {
				CreateMember(i);
			}
			return m_members[i];
		}
	}
	return NULL;
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

