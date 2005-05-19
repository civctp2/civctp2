

#include "c3.h"
#include <stdarg.h>

#include "FliEngine.h"
#include "FliSymTab.h"
#include "FliFuncTab.h"
#include "FliRule.h"
#include "pointerlist.h"
#include "fliif.h"
#include "FliAction.h"

#include "ic3GameState.h"
#include "ic3ErrorReport.h"
#include "ic3String.h"
#include "aimain.h"

#define k_INITIAL_SYMTAB_SIZE 64
double fz_always_one_var=1.0;

FliEngine::FliEngine(IC3GameState *gs, AiMain *ai)
{
	m_symTab = new FliSymTab(k_INITIAL_SYMTAB_SIZE);
	m_funcTab = new FliFuncTab(k_INITIAL_SYMTAB_SIZE);

	m_rawRuleLists = new PointerList<FliRule> *[FLI_SECT_MAX];
	m_ruleLists = new PointerList<FliRule> *[FLI_SECT_MAX];
	m_actionLists = new PointerList<FliAction> *[FLI_SECT_MAX];

	AddSymbol("always_one_var", &fz_always_one_var, 1.0, FLI_SYM_INPUT);
	GetSymbol("always_one_var")->SetDeclared();

	sint32 i;
	for(i = 0; i < FLI_SECT_MAX; i++) {
		m_rawRuleLists[i] = new PointerList<FliRule>;
		m_ruleLists[i] = NULL;
		m_actionLists[i] = new PointerList<FliAction>;


		GetSymbol("always_one_var")->RegisterInit(i);
		char actualname[1024];
		sprintf(actualname, "s%02d_alwaysone", i);
		AddFunction(actualname, GetSymbol("always_one_var"), SFTYPE_RIGHT, 1, 0.5);
	}

	m_gs = gs;
	m_ai = ai;

	m_currentAction = NULL;

}

FliEngine::~FliEngine()
{
    if(m_symTab) { 
		delete m_symTab;
        m_symTab = NULL; 
    } 

    if(m_funcTab) {
		delete m_funcTab;
        m_funcTab = NULL; 
    } 

	sint32 i;
	for(i = 0; i < sint32(FLI_SECT_MAX); i++) {
		if(m_rawRuleLists[i]) {
			m_rawRuleLists[i]->DeleteAll();
			delete m_rawRuleLists[i];
            m_rawRuleLists[i] = NULL; 
		}

		if(m_ruleLists[i]) {
			m_ruleLists[i]->DeleteAll();
			delete m_ruleLists[i];
            m_ruleLists[i] = NULL; 
		}

		if(m_actionLists[i]) {
			m_actionLists[i]->DeleteAll();
			delete m_actionLists[i];
            m_actionLists[i] = NULL; 
		}
	}

	delete [] m_rawRuleLists;
    m_rawRuleLists = NULL; 
	delete [] m_ruleLists;
    m_ruleLists = NULL; 
	delete [] m_actionLists;
    m_actionLists = NULL; 

}

FliSymbol *FliEngine::GetSymbol(sint32 index)
{
	return m_symTab->Get(index);
}

FliSymbol *FliEngine::GetSymbol(const char *name)
{
	return m_symTab->Access(name);
}

FliSymbol *FliEngine::GetInputSymbol(char *name)
{
	FliSymbol *sym = m_symTab->Access(name);
	if(!sym)
		return NULL;

	if(sym->GetType() != FLI_SYM_INPUT)
		return NULL;
	return sym;
}

void FliEngine::AddSymbol(char *name, double *internal, double initvalue, FLI_SYM type)
{
	m_symTab->Add(name, internal, initvalue, type);
}

FliSymbol *FliEngine::AddExternalSymbol(char *name, double initialvalue)
{
	return m_symTab->Add(name, NULL, initialvalue, FLI_SYM_OUTPUT);
}

void FliEngine::Init()
{
	m_symTab->Init();
	SortAllRules();
}

FliSetFunc *FliEngine::GetFunction(sint32 index)
{
	return m_funcTab->Get(index);
}

FliSetFunc *FliEngine::GetFunction(const char *name)
{
	return m_funcTab->Access(name);
}

void FliEngine::AddFunction(char *name, FliSymbol *variable, SFTYPE type,
							double v1, double v2)
{
	m_funcTab->Add(name, variable, type, v1, v2);
}

void FliEngine::AddRule(uint8 *code, sint32 codelen, 
						PointerList<FliSymbol> *inputsyms,
						PointerList<FliSymbol> *outputsyms,
						sint32 ruletype,
						FliWhen *when)
{
	FliRule *newrule = new FliRule(code, codelen, inputsyms,
								   outputsyms, ruletype,
								   when);
	m_rawRuleLists[ruletype]->AddTail(newrule);
}

void FliEngine::SortAllRules()
{
	sint32 i;
	for(i = 0; i < sint32(FLI_SECT_MAX); i++) {
		m_ruleLists[i] = new PointerList<FliRule>;
		SortRules(m_rawRuleLists[i], m_ruleLists[i]);
		delete m_rawRuleLists[i];
		m_rawRuleLists[i] = NULL;
	}
}

void FliEngine::SortRules(PointerList<FliRule>*inlist,
						  PointerList<FliRule>*outlist)
{
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

	while(!inlist->IsEmpty()) {
Assert(++finite_loop < 100000); 
		sint32 startrules = inlist->GetCount();
		
		PointerList<FliRule>::Walker walk(inlist);
		while(walk.IsValid()) {
Assert(++finite_loop < 100000); 
			FliRule *rule = walk.GetObj();
			if(rule->DependanciesResolved()) {
				rule->ResolveDependancies();
				outlist->AddTail(rule);
				walk.Remove();
			} else {
				walk.Next();
			}
		}

		
		if(inlist->GetCount() == startrules) {
			
			ReportDBError("FLI_FUZZY_CYCLE");
			return;
		}
	}
}

sint32 rules_time; 
sint32 act_time; 

void FliEngine::FireRules(sint32 module)
{
	fli_SetCurrentPlayer(m_ai->my_player_index);

	FLPRINTF(("BEGIN %s\n", GetSectionName(module)));
	m_symTab->InitForSection(module);

	Assert(module >= 0 && module < FLI_SECT_MAX);
	if(module < 0 || module >= FLI_SECT_MAX)
		return;

	PointerList<FliRule> *ruleList = m_ruleLists[module];
	PointerList<FliRule>::Walker walk(ruleList);
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

sint32 start_time  = 0; 

start_time = 0;

start_time = GetTickCount(); 

	while(walk.IsValid()) {
Assert(++finite_loop < 100000); 
		FliRule *rule = walk.GetObj();
		rule->Evaluate(this, module);
		walk.Next();
	}
rules_time += GetTickCount() - start_time; 

start_time = GetTickCount(); 
	PointerList<FliAction>::Walker actionWalk(m_actionLists[module]);
	while(actionWalk.IsValid()) {
Assert(++finite_loop < 100000); 
		FliAction *action = actionWalk.GetObj();
		action->Evaluate(m_ai);
		actionWalk.Next();
	}
act_time += GetTickCount() - start_time;

	FLPRINTF(("END %s\n", GetSectionName(module)));
}

void FliEngine::Defuzzify(const sint32 module, FliSymbol *variable, double nSamples)
{
	double x, y;
	double min, max;
	double weightedAverage = 0.0, area = 0.0;

	FLPRINTF(("%s:\n", variable->GetName()));
#ifdef _DEBUG
	if(fli_GetFliLoggingEnabled(m_ai->my_player_index)) {
		variable->DumpLogStrings();
	}
#endif

	variable->RemoveUnusedOutputProcs(module);

    Assert(0.0 < nSamples); 
	variable->GetMinMax(min, max);
    double vdx = variable->GetRecommendedDX(module);  
    double sdx = (max - min) / nSamples; 
    double dx = min(sdx, vdx); 

	for(x = min; x <= max; x += dx) {		
		y = variable->GetMaxMembership(module, x, dx, max);
		weightedAverage += y * (x - min);
		area += y;
	}

	if(area < 0.0000001) {

		
		
		
	} else {
		variable->SetValue((weightedAverage + (min * area))  / area);
	}
	FLPRINTF(("  result: %s=%lf\n\n", variable->GetName(), variable->GetValue()));
}

void FliEngine::ReportDBError(const MBCHAR *stringId, ...)
{
	const MBCHAR *fmt;
	va_list list;
	char buf[1024];

	fmt = m_gs->GetStringDB()->GetString(stringId);

	if(!fmt) {
		sprintf(buf, "String '%s' not in DB", stringId);
	} else {
		va_start(list, stringId);
		vsprintf(buf, fmt, list);
		va_end(list);
	}
	m_gs->GetErrorReport()->ReportError(buf);
}

void FliEngine::ReportError(const MBCHAR *text, ...)
{
	char buf[1024];
	va_list list;

	va_start(list, text);
	vsprintf(buf, text, list);
	va_end(list);
	m_gs->GetErrorReport()->ReportError(buf);
}

void FliEngine::InitGraphics()
{
    m_symTab->InitGraphics(); 
} 

sint32 FliEngine::GetNumFuzzySections()
{
    return FLI_SECT_MAX;
} 

sint32 FliEngine::GetNumFuzzyVariables(sint32 idx_section)
{
    return m_symTab->GetNumFuzzyVariables(idx_section); 
} 

void FliEngine::GetFuzzyGraph(sint32 idx_section, sint32 idx_variable, 
        char **label, double *minx, double *maxx, double *miny, double *maxy, 
        sint32 *num_graphs, sint32 *num_x, double ***height, double *defuzz_val)
{
    m_symTab->GetFuzzyGraph(idx_section, idx_variable, 
        label, minx, maxx, miny, maxy, 
        num_graphs, num_x, height, defuzz_val);
    
}

void FliEngine::ReloadFuzzyLogic()
{
}
void FliEngine::ResetFuzzyInput(sint32 idx_section, sint32 idx_variable, 
        double new_defuzz_val)
{
}


#ifdef DO_FZLOG
void FliEngine::DumpFZInputHeaders(sint32 which) 
{ 
	if (m_symTab) m_symTab->DumpFZInputHeaders(which); 
}
void FliEngine::DumpFZInputs(sint32 which) 
{ 
	if (m_symTab) m_symTab->DumpFZInputs(which); 
}
#endif

void FliEngine::StartAction(const char *name)
{
	Assert(m_currentAction == NULL);
	if(m_currentAction != NULL) {
		delete m_currentAction;
        m_currentAction = NULL; 
	}
	m_currentAction = new FliAction(name, this);
}

void FliEngine::EndAction(sint32 section)
{
	m_actionLists[section]->AddTail(m_currentAction);
	m_currentAction = NULL;
}

void FliEngine::AddArgument(const char *str)
{
	Assert(m_currentAction != NULL);
	if(m_currentAction == NULL)
		return;
	m_currentAction->AddArgument(str);
}

void FliEngine::SetBoolExp(const char *varname,
						   BFLOP_TYPE op, double value)
{
	m_currentAction->SetBoolExp(GetSymbol(varname),
								op,
								value);
}

char *FliEngine::GetSectionName(sint32 module)
{
	switch(module) {
		case FLI_SECT_EVAL: return "eval";
		case FLI_SECT_INIT: return "init";
		case FLI_SECT_BEGINTURN: return "begin_turn";
		case FLI_SECT_ENDTURN: return "end_turn";
		case FLI_SECT_PRE_INCOMING_DIPLOMACY: return "pre_incoming_diplomacy";
		case FLI_SECT_POST_INCOMING_DIPLOMACY: return "post_incoming_diplomacy";
		case FLI_SECT_PRE_OUTGOING_DIPLOMACY: return "pre_outgoing_diplomacy";
		case FLI_SECT_POST_OUTGOING_DIPLOMACY: return "post_outcoming_diplomacy";
		case FLI_SECT_BEGIN_DIPLOMACY: return "begin_diplomacy";
		case FLI_SECT_REJECTED_MESSAGE: return "rejected_message";
		case FLI_SECT_ACCEPTED_MESSAGE: return "accepted_message";
		default: Assert(FALSE); return "unknown";
	}
}
