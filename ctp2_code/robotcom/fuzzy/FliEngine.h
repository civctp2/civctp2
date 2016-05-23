#pragma once

#ifndef __FLI_ENGINE_H__
#define __FLI_ENGINE_H__

class FliSymTab;
class FliSymbol;
class FliFuncTab;
class FliSetFunc;
template <class T> class PointerList;
class FliRule;

enum FLI_SYM;
typedef int SFTYPE;
interface IC3GameState;
class AiMain;
class FliAction;
typedef int BFLOP_TYPE;
class FliWhen;

#define k_NUM_DEFUZZ_SAMPLES 100.0

class FliEngine {
	FliSymTab *m_symTab;
	FliFuncTab *m_funcTab;

	PointerList<FliRule> **m_rawRuleLists;
	PointerList<FliRule> **m_ruleLists;
	PointerList<FliAction> **m_actionLists;

	FliAction *m_currentAction;

	IC3GameState *m_gs;

public:
	AiMain *m_ai;

	FliEngine(IC3GameState *gs, AiMain *ai);
	~FliEngine();

	FliSymbol *GetSymbol(sint32 index);
	FliSymbol *GetSymbol(const char *name);
	FliSymbol *GetInputSymbol(char *name);

	void AddSymbol(char *name, double *internal, double value, FLI_SYM type);
	FliSymbol *AddExternalSymbol(char *name, double initialvalue);

	void Init();

	FliSetFunc *GetFunction(sint32 index);
	FliSetFunc *GetFunction(const char *name);
	void AddFunction(char *name, FliSymbol *variable,
					 SFTYPE type, double v1, double v2);
	void AddRule(uint8 *code, sint32 codelen,
				 PointerList<FliSymbol> *inputsyms,
				 PointerList<FliSymbol> *outputsyms,
				 sint32 ruletype,
				 FliWhen *when);
	void SortAllRules();
	void SortRules(PointerList<FliRule> *inlist,
				   PointerList<FliRule> *outlist);
	void FireRules(sint32 module);
	void Defuzzify(const sint32 module, FliSymbol *variable, double nSamples);

	void ReportDBError(const MBCHAR *stringId, ...);
	void ReportError(const MBCHAR *text, ...);




	void StartAction(const char *name);
	void EndAction(sint32 section);
	void AddArgument(const char *str);
	void SetBoolExp(const char *varname, BFLOP_TYPE op, double value);

    void InitGraphics();
    sint32 GetNumFuzzySections();
    sint32 GetNumFuzzyVariables(sint32 idx_section);
    void GetFuzzyGraph(sint32 idx_section, sint32 idx_variable,
        char **label, double *minx, double *maxx, double *miny, double *maxy,
        sint32 *num_graphs, sint32 *num_x, double ***height, double *defuzz_val);

    void ReloadFuzzyLogic();
    void ResetFuzzyInput(sint32 idx_section, sint32 idx_variable,
        double new_defuzz_val);
	char *GetSectionName(sint32 module);
#ifdef DO_FZLOG
	void DumpFZInputHeaders(sint32 which);
	void DumpFZInputs(sint32 which);
#endif

};

#endif
