#pragma once

#ifndef __FLI_SYM_TAB_H__
#define __FLI_SYM_TAB_H__

#define COM_INTERFACE

#include "StringHash.h"
#include "FliSymbol.h"
#include "fliif.h"

class FliSymTab : public StringHash<FliSymbol>
{

	sint32 m_arraySize;
	sint32 m_numEntries;


	FliSymbol **m_array;
    sint32 m_num_variables[FLI_SECT_MAX];
    sint32 m_max_num_fuzzy_set;
    double **m_fuzzy_set_buff;

public:

	FliSymTab(sint32 initsize);
	~FliSymTab();

	FliSymbol *Add(char *name, double *internal, double initvalue, FLI_SYM type);
	void Init();
	void InitForSection(sint32 module);
	FliSymbol *Get(sint32 index);

    void InitGraphics();

    sint32 GetNumFuzzyVariables(const sint32 idx_section);
    void CalcNumFuzzyVariables();
    void AllocateFuzzySetBuff ();

    FliSymbol* FindSymbol(const sint32 idx_section, const sint32 idx_variable);
    void GetFuzzyGraph(sint32 idx_section, sint32 idx_variable,
        char **label, double *minx, double *maxx, double *miny, double *maxy,
        sint32 *num_graphs, sint32 *num_x, double ***height, double *defuzz_val);
#ifdef DO_FZLOG
	void DumpFZInputHeaders(sint32 which);
	void DumpFZInputs(sint32 which);
#endif

};

#endif
