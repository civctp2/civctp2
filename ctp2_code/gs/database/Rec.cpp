











#include "c3.h"

#include "Rec.h"
#include "CivArchive.h"


Record::Record()
{ 
	sint32 i;
	m_name = -1; 
	m_enable = -1; 
	for(i = 0; i < k_MAX_OBSOLETE; i++) {
		m_obsolete[i] = -1;
	}
	m_nObsolete = 0;
}


void Record::SetName(const StringId string_id)
{
   m_name = string_id; 
}

void Record::SetEnabling(const sint32 e)
{
   m_enable = e;
}


void Record::SetObsolete(const sint32 o, sint32 index)
{
	Assert(index < k_MAX_OBSOLETE);
	Assert(index >= 0);
	if(o < 0)
		return;
	if(index >= 0 && index < k_MAX_OBSOLETE) {
		m_obsolete[index] = o; 
		if(index + 1 > m_nObsolete)
			m_nObsolete = index + 1;
	}
}

sint32 Record::GetEnabling() const
{
    return m_enable; 
} 

sint32 Record::GetObsolete(sint32 index) const
{
    return m_obsolete[index];
}

sint32 Record::GetNumObsolete() const
{
	return m_nObsolete;
}











void Record::Serialize(CivArchive &archive)
{
	if (archive.IsStoring()) {
		archive << m_enable;
		archive << m_nObsolete;
		archive.StoreArray(m_obsolete, m_nObsolete);
	}
	else {
		BOOL ThisDoesntReallyWorkAnyMore = FALSE;
		Assert(ThisDoesntReallyWorkAnyMore);
	}
}
