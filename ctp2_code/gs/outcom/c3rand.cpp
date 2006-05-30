
#include "c3.h"
#include "C3Rand.h"
#include "RandGen.h"
#include "civarchive.h"

extern RandomGenerator *g_rand;

#ifndef USE_COM_REPLACEMENT
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
#else
uint32 C3Rand::AddRef()
#endif
{
	return ++m_refCount;
}

#ifndef USE_COM_REPLACEMENT
STDMETHODIMP_(ULONG) C3Rand::Release()
#else
uint32 C3Rand::Release()
#endif
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

#ifndef USE_COM_REPLACEMENT
STDMETHODIMP_(sint32) C3Rand::Next(sint32 range)
#else
sint32 C3Rand::Next(sint32 range)
#endif
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
        archive << static_cast<uint32>(m_refCount); 
        archive.PutSINT32(m_ownGenerator);

        if (m_ownGenerator) { 
            m_rand->Serialize(archive); 
        } 
      
    } else { 
        uint32  l_refCount;
        archive >> l_refCount; 
        m_refCount  = l_refCount;
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
