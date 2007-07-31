

#include "c3.h"
#include "slicif.h"
#include "SlicArray.h"
#include "CivArchive.h"
#include "SlicEngine.h"
#include "SlicSymbol.h"
#include "SlicStack.h"
#include "SlicNamedSymbol.h"
#include "SlicStruct.h"
#include "SlicObject.h"
#include "SlicSegment.h"
#include "SlicFrame.h"
#include "ProfileDB.h"

#define k_DEFAULT_SLICARRAY_SIZE 1

SlicArray::SlicArray(SS_TYPE type, SLIC_SYM varType)
{
	m_type = type;
	m_varType = varType;
	m_allocatedSize = k_DEFAULT_SLICARRAY_SIZE;
	m_arraySize = 0;
	m_sizeIsFixed = false;
	m_array = new SlicStackValue[m_allocatedSize];
	memset(m_array, 0, m_allocatedSize * sizeof(SlicStackValue));
	m_structTemplate = NULL;
}

SlicArray::SlicArray(SlicStructDescription *aStruct)
{
	m_type = SS_TYPE_SYM;
	m_varType = SLIC_SYM_STRUCT;
	m_allocatedSize = k_DEFAULT_SLICARRAY_SIZE;
	m_arraySize = 0;
	m_sizeIsFixed = false;
	m_array = new SlicStackValue[m_allocatedSize];
	memset(m_array, 0, m_allocatedSize * sizeof(SlicStackValue));
	m_structTemplate = aStruct;
}

SlicArray::SlicArray(CivArchive &archive)
{
	Serialize(archive);
}

SlicArray::~SlicArray()
{
	if(m_array) {
		delete [] m_array;
	}

}

void SlicArray::FixSize(sint32 size)
{
	if(m_array) {
		delete [] m_array;
	}

	m_allocatedSize = m_arraySize = size; 
	m_array = new SlicStackValue[m_allocatedSize];
	memset(m_array, 0, m_allocatedSize * sizeof(SlicStackValue));
	m_sizeIsFixed = true;
}

void SlicArray::SetType(SS_TYPE type, SLIC_SYM varType)
{
	Assert(m_arraySize == 0 || m_sizeIsFixed);
	m_type = type;
	m_varType = varType;
}

void SlicArray::Serialize(CivArchive &archive)
{
	sint32 i;
	uint8 haveSym;

	if(archive.IsStoring()) {
		archive.PutSINT32(m_type);
		archive.PutSINT32(m_varType);
		archive << m_allocatedSize;
		archive << m_arraySize;
		archive.PutUINT8((uint8)m_sizeIsFixed);

		if(m_varType == SLIC_SYM_STRUCT) {
			archive.PutUINT8(m_structTemplate->GetType());
		}

		if(m_type == SS_TYPE_INT) {
			archive.Store((uint8*)m_array, m_arraySize * sizeof(SlicStackValue));
		} else {
			Assert(m_type == SS_TYPE_SYM);
			for(i = 0; i < m_arraySize; i++) {
				haveSym = m_array[i].m_sym != NULL;
				archive << haveSym;
				if(haveSym) {
					((SlicSymbolData *)m_array[i].m_sym)->SlicSymbolData::Serialize(archive);
				}
			}
		}
	} else {
		m_type = (SS_TYPE)archive.GetSINT32();
		m_varType = (SLIC_SYM)archive.GetSINT32();
		archive >> m_allocatedSize;
		archive >> m_arraySize;
		m_sizeIsFixed = (bool)(archive.GetUINT8() != 0);

		if(m_varType == SLIC_SYM_STRUCT) {
			SLIC_BUILTIN structType = (SLIC_BUILTIN)archive.GetUINT8();		
			m_structTemplate = g_slicEngine->GetStructDescription(structType);
		} else {
			m_structTemplate = NULL;
		}

		m_array = new SlicStackValue[m_allocatedSize];
		memset(m_array, 0, m_allocatedSize * sizeof(SlicStackValue));

		if(m_type == SS_TYPE_INT) {
			archive.Load((uint8 *)m_array, m_arraySize * sizeof(SlicStackValue));
		} else {
			Assert(m_type == SS_TYPE_SYM);
			for(i = 0; i < m_arraySize; i++) {
				archive >> haveSym;
				if(haveSym) {
					m_array[i].m_sym = slicsymbol_Load(archive, NULL);
				}
			}
		}
	}
}

BOOL SlicArray::Lookup(sint32 index, SS_TYPE &type, SlicStackValue &value)
{
	
	type = m_type;

	
	if(index < 0 || index >= m_arraySize) {
		if(g_slicEngine->GetContext() && g_slicEngine->GetContext()->GetSegment() &&
			g_slicEngine->GetContext()->GetFrame()) {
			if(g_theProfileDB && g_theProfileDB->IsDebugSlic()) {
				c3errors_ErrorDialog("SLIC", "%s:%d: Array index %d out of bounds", 
									 g_slicEngine->GetContext()->GetSegment()->GetFilename(),
									 g_slicEngine->GetContext()->GetFrame()->GetCurrentLine(),
									 index);
			}
		} else {
			if(g_theProfileDB && g_theProfileDB->IsDebugSlic()) {
				c3errors_ErrorDialog("SLIC", "Array index %d out of bounds",
									 index);
			}
		}
		return FALSE;
	}

	
	if(m_type == SS_TYPE_SYM && m_array[index].m_sym == NULL) {
		
		if(m_structTemplate) {			
			m_array[index].m_sym = m_structTemplate->CreateInstance();
		} else {
			m_array[index].m_sym = new SlicSymbolData(m_varType);
		}
	}

	value = m_array[index];
	return TRUE;
}

BOOL SlicArray::Insert(sint32 index, SS_TYPE type, SlicStackValue value)
{
	switch(m_type) {
		case SS_TYPE_VAR:
		{
			
			Assert(type == SS_TYPE_VAR || type == SS_TYPE_SYM);
			if(type != SS_TYPE_VAR && type != SS_TYPE_SYM) {
				return FALSE;
			}
			
			
			
			SlicSymbolData *sym;
			if(type == SS_TYPE_VAR)
				sym = g_slicEngine->GetSymbol(value.m_int);
			else
				sym = value.m_sym;

			if(!sym || (sym->GetType() != m_varType)) {
				return FALSE;
			}
			break;
		}
		case SS_TYPE_INT:
			
			if(type != SS_TYPE_INT) {
				SlicSymbolData *sym = value.m_sym;
				if(!sym) {
					return FALSE;
				}
				type = SS_TYPE_INT;
				sym->GetIntValue(value.m_int);
			}
			break;
		case SS_TYPE_SYM:
			
			break;
		default:
			
			Assert(FALSE);
			return FALSE;
	}

	if(index < 0)
		return FALSE;

	
	if(index >= m_allocatedSize) {

		
		sint32 oldAllocated = m_allocatedSize;
		SlicStackValue *newArray;
		while(index >= m_allocatedSize) {
			m_allocatedSize *= 2;
		}

		newArray = new SlicStackValue[m_allocatedSize];
		memset(&newArray[oldAllocated], 0, 
			   (m_allocatedSize - oldAllocated) * sizeof(SlicStackValue));

		memcpy(newArray, m_array, oldAllocated * sizeof(SlicStackValue));
		delete [] m_array;
		m_array = newArray;
	}

	
	if(index >= m_arraySize) {
		if(m_sizeIsFixed) {
			
			return FALSE;
		}

		
		
		if(index > m_arraySize) {
			
			memset(&m_array[m_arraySize], 0, (index - m_arraySize) * sizeof(SlicStackValue));
		}
		m_arraySize = index + 1;
	}

	
	if(m_type == SS_TYPE_SYM) {
		
		if(!m_array[index].m_sym) {
			delete m_array[index].m_sym;
			if(m_structTemplate) {
				
				m_array[index].m_sym = m_structTemplate->CreateInstance(type, value);
			} else {
				
				m_array[index].m_sym = new SlicSymbolData(m_varType);
				m_array[index].m_sym->SetValueFromStackValue(type, value);
			}
		} else {
			m_array[index].m_sym->SetValueFromStackValue(type, value);
		}
	} else {
		m_array[index] = value;
	}

	return TRUE;
}


void SlicArray::Prune(sint32 size)
{
	if(m_sizeIsFixed)
		return;

	sint32 i;
	if(m_type == SS_TYPE_SYM) {
		
		
		for(i = size; i < m_arraySize; i++) {
			delete m_array[i].m_sym;
			m_array[i].m_sym = NULL;
		}
	}

	m_arraySize = size;
}
