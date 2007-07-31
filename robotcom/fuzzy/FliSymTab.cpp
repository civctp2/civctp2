

#include "c3.h"
#include "FliSymTab.h"
#include "fliif.h"
#include "FliSymbol.h"
#include "FliEngine.h"

#define k_FLI_SYM_HASH_SIZE 512

FliSymTab::FliSymTab(sint32 initsize) :
	StringHash<FliSymbol>(k_FLI_SYM_HASH_SIZE)
{
	m_arraySize = initsize;
	m_array = new FliSymbol *[initsize];
	m_numEntries = 0;

    m_max_num_fuzzy_set = 0; 
    m_fuzzy_set_buff=NULL;

}

FliSymTab::~FliSymTab()
{
	if(m_array) {
		delete [] m_array;
        m_array = NULL; 
	}

    sint32 i; 
    if (m_max_num_fuzzy_set) { 
        for (i=0; i<m_max_num_fuzzy_set; i++) { 
            delete[] m_fuzzy_set_buff[i]; 
            m_fuzzy_set_buff[i] = NULL; 
        }
        delete[] m_fuzzy_set_buff; 
        m_fuzzy_set_buff=NULL; 
    }

}

FliSymbol *FliSymTab::Add(char *name, double *internal, double initvalue, FLI_SYM type)
{
	FliSymbol *newsym = new FliSymbol(name, internal, initvalue, type);
	newsym->SetIndex(m_numEntries);
	StringHash<FliSymbol>::Add(newsym);
	if(m_numEntries >= m_arraySize) {
		FliSymbol **newarray = new FliSymbol *[m_arraySize * 2];
		memcpy(newarray, m_array, m_arraySize * sizeof(FliSymbol *));
		delete [] m_array;
		m_array = newarray;
		m_arraySize *= 2;
	}
	m_array[m_numEntries] = newsym;
	m_numEntries++;
	return newsym;
}

void FliSymTab::Init()
{
	for(sint32 i = 0; i < m_numEntries; i++) {
		m_array[i]->Init();
	}
}


void FliSymTab::InitGraphics()
{ 
    CalcNumFuzzyVariables();
    AllocateFuzzySetBuff ();
}

void FliSymTab::InitForSection(sint32 module)
{
	Init();
	for(sint32 i = 0; i < m_numEntries; i++) {
		m_array[i]->InitForSection(module);
	}
}

FliSymbol *FliSymTab::Get(sint32 index)
{
	Assert(index >=0 && index < m_numEntries);
	return m_array[index];
}


sint32 FliSymTab::GetNumFuzzyVariables(const sint32 idx_section) 
{ 
    Assert(0 <= idx_section); 
    Assert(idx_section < FLI_SECT_MAX); 

    return m_num_variables[idx_section]; 
} 

void FliSymTab::CalcNumFuzzyVariables()
{
    sint32 idx_symbol, count, idx_section; 

    for (idx_section=0; idx_section<FLI_SECT_MAX; idx_section++) { 
        count = 0; 
        for (idx_symbol=0; idx_symbol < m_numEntries; idx_symbol++) { 
            if (m_array[idx_symbol]->UsedInSection(idx_section)) { 
                count++; 
            } 
        }
        m_num_variables[idx_section]=count; 
    }
} 

void FliSymTab::AllocateFuzzySetBuff ()
{ 
    
    
    















    m_max_num_fuzzy_set = 8;  
    m_fuzzy_set_buff = new double *[m_max_num_fuzzy_set]; 
    sint32 i; 
    for (i=0; i<m_max_num_fuzzy_set; i++) { 
        m_fuzzy_set_buff[i] = new double[sint32(k_NUM_DEFUZZ_SAMPLES)];
    } 
}


FliSymbol* FliSymTab::FindSymbol(const sint32 idx_section, 
   const sint32 idx_variable)
{
    sint32 i; 
    sint32 count=0; 
    for (i=0; i<m_numEntries; i++) { 
        if (m_array[i]->UsedInSection(idx_section)) { 
            if (count == idx_variable) { 
                return m_array[i]; 
            } 
            count++;
        }
    }
    Assert(0); 
    return NULL; 
}

void FliSymTab::GetFuzzyGraph(sint32 idx_section, sint32 idx_variable, 
        char **label, double *minx, double *maxx, double *miny, double *maxy, 
        sint32 *num_graphs, sint32 *num_x, double ***height, double *defuzz_val)

{
    FliSymbol *symbol = FindSymbol(idx_section, idx_variable); 

    *miny = 0.0; 
    *maxy = 1.0; 

    symbol->GetFuzzyGraph(idx_section, label, minx, maxx, num_graphs, m_max_num_fuzzy_set, 
        m_fuzzy_set_buff, defuzz_val);

    *height = m_fuzzy_set_buff; 
    *num_x = sint32 (k_NUM_DEFUZZ_SAMPLES); 
}


MBCHAR	s_inputComment[_MAX_PATH];
FILE	*s_inputLogFile;

#ifdef DO_FZLOG

#define FZINPUTLOGFILENAME		"fzinputlog"

void FliSymTab::DumpFZInputHeaders(sint32 which)
{
	MBCHAR fname[_MAX_PATH];

	sprintf(fname, "logs\\%s%d.txt", FZINPUTLOGFILENAME, which);
	s_inputLogFile = fopen(fname, "a");
	Assert(s_inputLogFile);

    sint32 i; 
    for (i=0; i<m_numEntries; i++) { 
        if (m_array[i]->GetType() == FLI_SYM_INPUT) {
			fprintf(s_inputLogFile, "%s\t", m_array[i]->GetName());
        }
    }
	
	fprintf(s_inputLogFile, "\n");
    
	fclose(s_inputLogFile);
	s_inputLogFile = NULL;

    return ; 
}

void FliSymTab::DumpFZInputs(sint32 which)
{
	MBCHAR fname[_MAX_PATH];

	sprintf(fname, "logs\\%s%d.txt", FZINPUTLOGFILENAME, which);
	s_inputLogFile = fopen(fname, "a");

    sint32 i; 
    for (i=0; i<m_numEntries; i++) { 
        if (m_array[i]->GetType() == FLI_SYM_INPUT) {
			fprintf(s_inputLogFile, "%f\t", m_array[i]->GetValue());
        }
    }
	
	if (strlen(s_inputComment) > 0) {
		fprintf(s_inputLogFile, "%s", s_inputComment);
		s_inputComment[0] = '\0';
	}

	fprintf(s_inputLogFile, "\n");
    
	fclose(s_inputLogFile);
	s_inputLogFile = NULL;

    return ; 
}

#endif
