#pragma once

#ifndef __FLI_SYMBOL_H__
#define __FLI_SYMBOL_H__

#include "fliif.h"

enum FLI_SYM {
	FLI_SYM_INPUT,
	FLI_SYM_OUTPUT
};

#define k_FLI_SYM_FLAG_TYPE_MASK  0x01
#define k_FLI_SYM_FLAG_REINIT     0x02
#define k_FLI_SYM_FLAG_ISDECLARED 0x04

template <class T> class DynamicArray;
class FliOutputProc;
template <class T> class PointerList;
class InputString;

interface IC3CivArchive;

class FliSymbol {
private:
	double *m_internal;
	double m_external;
	double m_initValue[FLI_SECT_MAX];

	uint16 m_hasInitValue;

	uint8 m_flags;
	uint8 pad;

	sint32 m_index;
	double m_minValue;
	double m_maxValue;





	sint16 m_numRules[FLI_SECT_MAX];
	sint16 m_firedRules[FLI_SECT_MAX];
	sint16 m_dependancies[FLI_SECT_MAX];
	uint16 m_initialized;
	DynamicArray<FliOutputProc> *m_outputProcs;
	char *m_name;
#ifdef _DEBUG
	PointerList<InputString> *m_inputStrings;
#endif

public:
	FliSymbol(char *name, double *internal, double initvalue, FLI_SYM type);
	~FliSymbol();
	void Serialize(IC3CivArchive *archive);

	FLI_SYM GetType() { return (FLI_SYM)(m_flags & k_FLI_SYM_FLAG_TYPE_MASK); }

	const char *GetName() const;
	double GetValue() const;
	void SetValue(double value);
	void SetInitialValue(sint32 section, double value);
	void SetMinMax(double min, double max) {
		m_minValue = min;
		m_maxValue = max;
	}
	void GetMinMax(double &min, double &max) const {
		min = m_minValue;
		max = m_maxValue;
	}

    sint32 GetNumFuzzySets(const sint32 module) const;
	void SetIndex(sint32 index) { m_index = index; }
	sint32 GetIndex() const { return m_index; }

	void Init();
	void InitForSection(sint32 module);

	BOOL IsInputVariable();
	BOOL DependanciesResolved(sint32 module);
	void AddDependance(sint32 module);
	void RemoveDependance(sint32 module);
	sint32 AddRule(sint32 module);
	sint16 NumRules(sint32 module);
	sint16 FireRule(sint32 module);
	FliOutputProc *NewOutputProc(const sint32 module);
	void RemoveUnusedOutputProcs(sint32 module);
	double GetMaxMembership(const sint32 module, double &x, double dx,
							double xmax);
	BOOL IsDeclared() const { return m_flags & k_FLI_SYM_FLAG_ISDECLARED; }
	void SetDeclared() { m_flags |= k_FLI_SYM_FLAG_ISDECLARED; }

    BOOL UsedInSection(const sint32 module);
	BOOL IsInitializedForSection(sint32 module);
	void RegisterInit(sint32 module);

    void DoubleFuzzySetBuff(sint32 &max_num_fuzzy_set, double**& fuzzy_set_buf);
    void GetFuzzyGraph(const sint32 idx_section, char **label, double *minx, double *maxx,
        sint32 *num_graphs, sint32 &max_num_fuzzy_set, double **&fuzzy_set_buff,
        double *defuzz_val);

    double GetRecommendedDX(const sint32 module);

#ifdef _DEBUG
	void CatLogStrings(PointerList<InputString> *strings);
	void ClearLogStrings();
	void DumpLogStrings();
#endif

};

#endif
