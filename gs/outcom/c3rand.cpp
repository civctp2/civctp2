
#include "c3.h"
#include "C3Rand.h"
#include "RandGen.h"
#include "CivArchive.h"

extern RandomGenerator *g_rand;

STDMETHODIMP C3Rand::QueryInterface(REFIID riid, void **obj)
{
	*obj = NULL;
	if(IsEqualIID(riid, IID_IUnknown)) {
		*obj = (IUnknown *)this;
		AddRef();
		return S_OK;
	} else if(IsEqualIID(riid, CLSID_IC3Rand)) {
		*obj = (IC3Rand*)this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) C3Rand::AddRef()
{
	return ++m_refCount;
}

STDMETHODIMP_(ULONG) C3Rand::Release()
{
	if(--m_refCount)
		return m_refCount;
	delete this;
	return 0;
}

C3Rand::C3Rand(BOOL ownGenerator)
{ 
	m_refCount = 0; 
	m_ownGenerator = TRUE; 
	if(m_ownGenerator) {
		m_rand = new RandomGenerator(*g_rand);
	} else {
		m_rand = g_rand; 
	}
}


C3Rand::~C3Rand()
{
	if(m_ownGenerator) {
		delete m_rand;
		m_rand = NULL;
	}
}

STDMETHODIMP_(sint32) C3Rand::Next(sint32 range)
{
	return m_rand->Next(range);
}


C3Rand::C3Rand(CivArchive &archive)
{
	m_rand = NULL;
    Serialize(archive); 
}

void C3Rand::Serialize(CivArchive &archive)
{
    if (archive.IsStoring()) { 
        archive << m_refCount; 
        archive.PutSINT32(m_ownGenerator);

        if (m_ownGenerator) { 
            m_rand->Serialize(archive); 
        } 
      
    } else { 
        archive >> m_refCount; 
        m_ownGenerator = archive.GetSINT32();
        if (m_ownGenerator) { 

			
			if (m_rand)
				delete m_rand;

            m_rand = new RandomGenerator(archive); 
        } else { 
            m_rand = g_rand; 
        } 
    } 
} 
