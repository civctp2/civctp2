

#include "c3.h"
#include "fliif.h"
#include "FliSymbol.h"
#include "civarchive.h"
#include "dynarr.h"
#include "FliOutputProc.h"
#include "FliEngine.h"
#include "FliSetFunc.h"
#include "pointerlist.h"
#include "FliRule.h"

FliSymbol::FliSymbol(char *name, double *internal, double initvalue, FLI_SYM type)
{
	m_name = new char[strlen(name) + 1];
	strcpy(m_name, name);
	m_internal = internal;
    m_external = 0.0; 

	
	
	m_minValue = 0;
	m_maxValue = -1;

	m_flags = (uint8)type;

	if(m_internal) {
		*m_internal = initvalue;
	} else {
		m_external = initvalue;
	}


	m_hasInitValue = 0;
	m_initialized = 0;

	for(sint32 i = 0; i < FLI_SECT_MAX; i++) {
		
		m_numRules[i] = 0;
		m_firedRules[i] = 0;
		m_dependancies[i] = 0;
    }
	m_outputProcs = NULL;
	

#ifdef _DEBUG
	m_inputStrings = new PointerList<InputString>;
#endif
}

FliSymbol::~FliSymbol()
{
    if(m_name) { 
		delete [] m_name;
        m_name = NULL; 
    } 

	if(m_outputProcs) { 
		delete m_outputProcs;
		m_outputProcs = NULL; 
    }
#ifdef _DEBUG
	if(m_inputStrings) {
		delete m_inputStrings;
        m_inputStrings = NULL; 
	}
#endif
}

void FliSymbol::Serialize(IC3CivArchive *archive)
{
}

void FliSymbol::SetInitialValue(sint32 section, double value)
{
	m_hasInitValue |= (1 << section);
	m_initValue[section] = value;
}

const char *FliSymbol::GetName() const
{
	return m_name;
}

double FliSymbol::GetValue() const
{
	if(m_internal)
		return *m_internal;
	return m_external;
}

void FliSymbol::SetValue(double value)
{
	if(GetType() == FLI_SYM_OUTPUT) {
		Assert(value >= m_minValue && value <= m_maxValue);
		if(value < m_minValue || value > m_maxValue)
			return;
	}

	if(m_internal)
		*m_internal = value;
	else
		m_external = value;
}

void FliSymbol::Init()
{
	
}

BOOL FliSymbol::IsInputVariable()
{
	return GetType() == FLI_SYM_INPUT;
}

BOOL FliSymbol::DependanciesResolved(sint32 module)
{
	return m_dependancies[module] == 0;
}

void FliSymbol::AddDependance(sint32 module)
{
	++m_dependancies[module];
}

void FliSymbol::RemoveDependance(sint32 module)
{
	Assert(m_dependancies[module] > 0);
	if(m_dependancies[module] <= 0)
		return;

	--m_dependancies[module];
}

void FliSymbol::InitForSection(sint32 module)
{
	m_firedRules[module] = 0;

	if(!m_outputProcs) {
		m_outputProcs = new DynamicArray<FliOutputProc>;
	}
	m_outputProcs->Clear();

	if(!(m_hasInitValue & (1 << module)))
		return;

	if(m_internal)
		*m_internal = m_initValue[module];
	else
		m_external = m_initValue[module];
}

sint32 FliSymbol::AddRule(sint32 module)
{
	return ++m_numRules[module];
}

sint16 FliSymbol::NumRules(sint32 module)
{
	return m_numRules[module];
}

sint16 FliSymbol::FireRule(sint32 module)
{
	return ++m_firedRules[module];
}

FliOutputProc *FliSymbol::NewOutputProc(const sint32 module)
{
	if(!m_outputProcs) {
		m_outputProcs = new DynamicArray<FliOutputProc>;
	}
	if(m_outputProcs->m_nElements >= m_outputProcs->m_maxElements) {
		m_outputProcs->Resize(m_outputProcs->m_maxElements * 2);
	}
	m_outputProcs->m_nElements++;
	return &m_outputProcs->Access(m_outputProcs->Num() - 1);
}

void FliSymbol::RemoveUnusedOutputProcs(sint32 module)
{
	DynamicArray<FliOutputProc> *outputProcs = m_outputProcs;
	if(!outputProcs) {
		m_outputProcs = new DynamicArray<FliOutputProc>;
		outputProcs = m_outputProcs;
	}
	FliOutputProc *procsArray = outputProcs->m_array;
	sint32 i, n = outputProcs->m_nElements;
	for(i = n - 1; i >= 0; i--) {
		if(outputProcs->Access(i).m_scalar < 0.00000001) {
			outputProcs->DelIndex(i);
		}
	}
}

sint32 m_count; 
double FliSymbol::GetMaxMembership(const sint32 module, double &x, double dx,
								   double xmax)
{
	sint32 insideLeftSteps;
	BOOL insideRight;
	sint32 maxInsideLeftSteps = 0;
	BOOL maxIsInsideRight = FALSE;

m_count++; 

	DynamicArray<FliOutputProc> *outputProcs = m_outputProcs;
	if(!outputProcs) {
		x = xmax;
		return 0;
	}

	FliOutputProc *procsArray = outputProcs->m_array;
	sint32 i, n = outputProcs->m_nElements;
	double max = 0, cur;
	for(i = n - 1; i >= 0; i--) {
		
		if(x >= procsArray[i].m_function->m_left_edge) {
			cur = procsArray[i].Membership(x, dx, insideLeftSteps, insideRight);
			if(cur > max) {
				max = cur;
				maxInsideLeftSteps = insideLeftSteps;
				maxIsInsideRight = insideRight;
			}
		}

		
		if(x > procsArray[i].m_function->m_right_edge && procsArray[i].m_function->m_right_edge >= 0) {
			outputProcs->DelIndex(i);
			
			procsArray = outputProcs->m_array;
		}
	}

    
    if(max < 0.0000001) { 
        double xhop = xmax;
        double leftEdge; 

		for(i = n - 1; i >= 0; i--) {
			leftEdge = procsArray[i].m_function->m_left_edge;
			if ((x < leftEdge) && (leftEdge < xhop)) {
				
				
				xhop = leftEdge;
			}
		}

        x = xhop; 
	}
	return max;
}

double FliSymbol::GetRecommendedDX(const sint32 module)
{
    double dx_min = 10000000.0, dx; 

	if(m_outputProcs) {
		sint32 i, n = m_outputProcs->Num();
		
		for(i = 0; i < n; i++) {
			dx = m_outputProcs->Access(i).GetRecommendedDX();
			
			if (dx < dx_min) { 
				dx_min = dx; 
			} 
		}
	}

	return dx_min;
}


BOOL FliSymbol::UsedInSection(const sint32 module) 
{ 
    return ((m_hasInitValue & (1 << module)) || 
			(0 < m_dependancies[module]) || 
			(0 < m_numRules[module]));
}        

BOOL FliSymbol::IsInitializedForSection(sint32 module)
{
	return m_initialized & (1 << module);
}

void FliSymbol::RegisterInit(sint32 module)
{
	m_initialized |= (1 << module);
}

sint32 FliSymbol::GetNumFuzzySets(const sint32 module) const
{ 
	if(!m_outputProcs)
		return 0;
    return m_outputProcs->Num(); 
} 

void FliSymbol::DoubleFuzzySetBuff(sint32 &max_num_fuzzy_set, 
    double**& fuzzy_set_buff)
{
    double **tmp; 

    tmp = new double*[2 * max_num_fuzzy_set]; 
    sint32 i; 

    for (i=0; i<max_num_fuzzy_set; i++) { 
        tmp[i] = fuzzy_set_buff[i]; 
        fuzzy_set_buff[i] = NULL; 
    } 


    for (i=max_num_fuzzy_set; i<(2*max_num_fuzzy_set); i++) { 
        tmp[i] = new double[sint32(k_NUM_DEFUZZ_SAMPLES)]; 
    } 

    delete[] fuzzy_set_buff; 
    fuzzy_set_buff = tmp; 
    max_num_fuzzy_set *= 2;

}


void FliSymbol::GetFuzzyGraph(const sint32 idx_section,  char** label, double* minx, double* maxx, 
   sint32* num_graphs, sint32 &max_num_fuzzy_set, double**& fuzzy_set_buff, 
   double* defuzz_val)
{

    *defuzz_val = GetValue();  
    *label = m_name;
	GetMinMax(*minx, *maxx);
    
    sint32 i; 
    if (*maxx < *minx) {
        *num_graphs = 1; 
        *minx = min(0.0, *defuzz_val); 
        *maxx = max(1.0, *defuzz_val); 
        for (i=0; i<k_NUM_DEFUZZ_SAMPLES; i++) { 
            fuzzy_set_buff[0][i] = 0.0; 
        } 
    } else { 

        double dx = (*maxx - *minx) / k_NUM_DEFUZZ_SAMPLES; 
    	sint32 j, n;
		if(!m_outputProcs)
			n = 0;
		else
			n = m_outputProcs->Num();
        *num_graphs = n; 

        if (n < 1) { 
            *num_graphs = 1; 
            for (i=0; i<k_NUM_DEFUZZ_SAMPLES; i++) { 
                fuzzy_set_buff[0][i] = 0.0; 
            } 
            return;
        } 

        if (max_num_fuzzy_set < n) { 
            DoubleFuzzySetBuff(max_num_fuzzy_set, fuzzy_set_buff); 
        }


	    FliOutputProc* cur=NULL;
        double x, y; 

    	for(i=0; i < n; i++) {
	    	cur = &(m_outputProcs->Access(i)); 
            for(x = *minx, j=0; x <= *maxx; x += dx, j++) {
				sint32 insideLeft;
				BOOL insideRight;
                y = cur->Membership(x, dx, insideLeft, insideRight);
                Assert(0.0 <= y); 
                Assert(y <= 1.0); 
                fuzzy_set_buff[i][j] = y; 
            } 
	    }
    }
}


#ifdef _DEBUG
void FliSymbol::CatLogStrings(PointerList<InputString> *strings)
{
	PointerList<InputString>::Walker walk(strings);
	for(; walk.IsValid(); walk.Next()) {
		m_inputStrings->AddTail(walk.GetObj());
	}
}

void FliSymbol::ClearLogStrings()
{
	while(m_inputStrings->GetCount() > 0) {
		m_inputStrings->RemoveHead();
	}
}

void FliSymbol::DumpLogStrings()
{
	PointerList<InputString>::Walker walk(m_inputStrings);
	for(; walk.IsValid(); walk.Next()) {
		FLPRINTF(("%s", walk.GetObj()->m_string));
	}
}
#endif

	

