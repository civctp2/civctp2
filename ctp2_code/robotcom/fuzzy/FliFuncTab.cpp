
#include "c3.h"
#include "FliFuncTab.h"
#include "fliif.h"

#define k_FLI_FUNC_HASH_SIZE 512

FliFuncTab::FliFuncTab(sint32 initsize) :
	StringHash<FliSetFunc>(k_FLI_FUNC_HASH_SIZE)
{
	m_arraySize = initsize;
	m_array = new FliSetFunc *[initsize];
	m_numEntries = 0;
}

FliFuncTab::~FliFuncTab()
{
	if(m_array) {
		for(sint32 i = 0; i < m_numEntries; i++) {
			
            m_array[i] = NULL; 
		}
		delete [] m_array;
        m_array = NULL; 
	}
}

FliSetFunc *FliFuncTab::Add(char *name, FliSymbol *variable,
							SFTYPE type,
							double v1, double v2)
{
	FliSetFunc *newfunc = new FliSetFunc(name, variable,
										type, v1, v2);
	newfunc->SetIndex(m_numEntries);
	StringHash<FliSetFunc>::Add(newfunc);
	if(m_numEntries >= m_arraySize) {
		FliSetFunc **newarray = new FliSetFunc *[m_arraySize * 2];
		memcpy(newarray, m_array, m_arraySize * sizeof(FliSetFunc *));
		delete [] m_array;
        m_array = NULL; 
		m_array = newarray;
		m_arraySize *= 2;
	}
	m_array[m_numEntries] = newfunc;
	m_numEntries++;
	return newfunc;
}

FliSetFunc *FliFuncTab::Get(sint32 index) const
{
	Assert(index >=0 && index < m_numEntries);
	if(index >= 0 && index < m_numEntries) {
		return m_array[index];
	} else {
		return NULL;
	}
}
