#include "c3.h"
#include "SlicSymTab.h"
#include "SlicEngine.h"
#include "civarchive.h"

SlicSymTab::SlicSymTab(sint32 size) :
	StringHash<SlicNamedSymbol>(k_SLIC_SYM_TAB_HASH_SIZE)
{
	m_arraySize = m_numEntries = size;
	if(m_arraySize == 0)
		m_arraySize = 1;
	m_array = new SlicNamedSymbol *[m_arraySize];
	for(sint32 i = 0; i < m_arraySize; i++) {
		m_array[i] = 0;
	}
}

SlicSymTab::SlicSymTab(CivArchive &archive) :
	StringHash<SlicNamedSymbol>(k_SLIC_SYM_TAB_HASH_SIZE)
{
	Serialize(archive);
}

SlicSymTab::~SlicSymTab()
{
	if(m_array) {
#if 0


		for(sint32 i = 0; i < m_numEntries; i++) {
			if(m_array[i])
				delete m_array[i];
		}
#endif
		delete [] m_array;
	}
}

void SlicSymTab::Serialize(CivArchive &archive)
{
	uint8 isPresent;
	if(archive.IsStoring()) {
		archive << m_arraySize;
		archive << m_numEntries;
		for(sint32 i = 0; i < m_numEntries; i++) {
			isPresent = (m_array[i] != NULL);
			archive << isPresent;
			if(isPresent) {
				((SlicSymbolData *)m_array[i])->SlicSymbolData::Serialize(archive);
			}
		}
	} else {
		archive >> m_arraySize;
		archive >> m_numEntries;
		m_array = new SlicNamedSymbol *[m_arraySize];
		for(sint32 i = 0; i < m_numEntries; i++) {
			archive >> isPresent;
			if(isPresent) {
				m_array[i] = (SlicNamedSymbol *)slicsymbol_Load(archive, NULL);
				StringHash<SlicNamedSymbol>::Add(m_array[i]);

				if(m_array[i]->IsBuiltin()) {
					g_slicEngine->AddBuiltinSymbol((SlicBuiltinNamedSymbol *)m_array[i]);
				}
			} else {
				m_array[i] = NULL;
			}
		}
	}
}

void SlicSymTab::PostSerialize()
{
	for(sint32 i = 0; i < m_numEntries; i++) {
		if(m_array[i]) {

		}
	}
}

void SlicSymTab::Add(SlicNamedSymbol *sym)
{
	StringHash<SlicNamedSymbol>::Add(sym);

#if 0
	PSlicNamedSymbol *psym = slicif_find_sym(sym->GetName(), 0);
	if(psym) {
		Assert(psym->m_index < m_numEntries);
		sym->SetIndex(psym->m_index);
		m_array[sym->GetIndex()] = sym;
		sint32 index = psym->m_index;
		free(g_slicSymTab[index]->m_name);
		if(g_slicSymTab[index]->m_region) {
			free(g_slicSymTab[index]->m_region);
		}
		free(g_slicSymTab[index]);

		g_slicSymTab[index] = NULL;
	} else {
#endif
		if(m_numEntries >= m_arraySize) {
			SlicNamedSymbol **newArray = new SlicNamedSymbol *[m_arraySize * 2];
			memcpy(newArray, m_array, m_arraySize * sizeof(SlicNamedSymbol *));
			delete [] m_array;
			m_array = newArray;
			m_arraySize *= 2;
		}
		sym->SetIndex(m_numEntries);
		m_array[m_numEntries] = sym;
		m_numEntries++;

}

void SlicSymTab::Add(sint32 index)
{
#if 0
	Assert(index >= 0);
	Assert(index < g_slicSymTabNumEntries);
	PSlicNamedSymbol *psym = g_slicSymTab[index];
	if(!psym)
		return;
	Assert(index < m_numEntries);

	SlicNamedSymbol *newSym = new SlicNamedSymbol(index);
	m_array[index] = newSym;
	free(g_slicSymTab[index]->m_name);
	if(g_slicSymTab[index]->m_region) {
		free(g_slicSymTab[index]->m_region);
	}
	free(g_slicSymTab[index]);
	g_slicSymTab[index] = NULL;

	switch(newSym->GetType()) {
		case SLIC_SYM_FUNC:
			newSym->SetFunction(g_slicEngine->GetFunction(newSym->GetName()));
			newSym->DelName();
			break;
		case SLIC_SYM_IVAR:
			break;
		default:
			break;
	}

	StringHash<SlicNamedSymbol>::Add(newSym);
#endif
	Assert(FALSE);
}

const SlicNamedSymbol *SlicSymTab::Get(sint32 index) const
{
	Assert(index >= 0);
	Assert(index < m_numEntries);
	if(index < 0 || index >= m_numEntries)
		return NULL;

	return m_array[index];
}

SlicNamedSymbol *SlicSymTab::Access(sint32 index)
{
	Assert(index >= 0);
	Assert(index < m_numEntries);
	if(index < 0 || index >= m_numEntries)
		return NULL;

	return m_array[index];
}

void SlicSymTab::Resize(sint32 num)
{
	m_numEntries = num;
	if(num <= m_arraySize)
		return;
	SlicNamedSymbol **oldarray = m_array;
	m_array = new SlicNamedSymbol*[num];
	memcpy(m_array, oldarray, m_arraySize * sizeof (SlicNamedSymbol *));
	delete [] oldarray;
	m_arraySize = num;
}
