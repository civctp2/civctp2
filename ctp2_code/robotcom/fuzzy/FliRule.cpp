

#include "c3.h"
#include "FliRule.h"
#include "FliSetFunc.h"
#include "PointerList.h"
#include "FliSymbol.h"
#include "FliEngine.h"
#include "FliStack.h"
#include "FliOutputProc.h"
#include "FliWhen.h"
#include "Globals.h"
#include "AiMain.h"

FliRule::FliRule(uint8 *code, sint32 codelen,
				 PointerList<FliSymbol> *inputsyms,
				 PointerList<FliSymbol> *outputsyms,
				 sint32 ruletype,
				 FliWhen *when)
{
	m_inputSymbols = inputsyms;
	m_outputSymbols = outputsyms;
	m_code = new uint8[codelen];
	memcpy(m_code, code, codelen);
	m_codelen = codelen;
	m_type = ruletype;
	m_when = when;
	if(m_when)
		m_when->AddRef();

	PointerList<FliSymbol>::Walker walk(m_outputSymbols);
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    while(walk.IsValid()) {
Assert(++finite_loop < 100000); 
		walk.GetObj()->AddRule(m_type);
		walk.Next();
	}

#ifdef _DEBUG
	m_inputStrings = new PointerList<InputString>;
#endif
}

FliRule::~FliRule()
{
    if(m_inputSymbols) { 
		delete m_inputSymbols;
        m_inputSymbols = NULL; 
    } 

    if(m_outputSymbols) { 
		delete m_outputSymbols;
        m_outputSymbols = NULL; 
    } 

    if(m_code) { 
		delete [] m_code;
        m_code = NULL; 
    } 

	if(m_when) {
		m_when->Release();
		m_when = NULL;
	}
#ifdef _DEBUG
	if(m_inputStrings) {
		m_inputStrings->DeleteAll();
		delete m_inputStrings;
        m_inputStrings = NULL;
	}
#endif
}

BOOL FliRule::DependanciesResolved()
{
	PointerList<FliSymbol>::Walker walk(m_inputSymbols);
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    while(walk.IsValid()) {
Assert(++finite_loop < 100000); 
		if(!walk.GetObj()->DependanciesResolved(m_type)) {
			return FALSE;
		}
		walk.Next();
	}
	return TRUE;
}

void FliRule::ResolveDependancies()
{
	PointerList<FliSymbol>::Walker walk(m_outputSymbols);
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    while(walk.IsValid()) {
Assert(++finite_loop < 100000); 
		walk.GetObj()->RemoveDependance(m_type);
		walk.Next();
	}
}

sint32 int_time; 
sint32 def_time; 
void FliRule::Evaluate(FliEngine *fli_engine, const sint32 module)
{
#ifdef _DEBUG
	if(fli_GetFliLoggingEnabled(fli_engine->m_ai->my_player_index)) {
		m_inputStrings->DeleteAll();
	}
#endif
sint32 start_time = 0; 
 start_time = 0; 

start_time = GetTickCount(); 

	
	if(!m_when || m_when->Evaluate()) {
		Interpret(fli_engine, module);
	}

int_time += GetTickCount() - start_time; 

	
	
	PointerList<FliSymbol>::Walker walk(m_outputSymbols);
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

start_time = GetTickCount(); 

    while(walk.IsValid()) {
Assert(++finite_loop < 100000); 
		if(walk.GetObj()->FireRule(m_type) == 
		   walk.GetObj()->NumRules(m_type)) {
			fli_engine->Defuzzify(module, walk.GetObj(), k_NUM_DEFUZZ_SAMPLES);
#ifdef _DEBUG
			if(fli_GetFliLoggingEnabled(fli_engine->m_ai->my_player_index)) {
				walk.GetObj()->ClearLogStrings();
			}
#endif
		}
		walk.Next();
	}
def_time += GetTickCount() - start_time; 
}

sint32 g_str_stack_idx; 
char g_str_stack[10][512]; 

void PushStr(char *str)
{ 
    Assert(g_str_stack_idx < 10); 
    strcpy(g_str_stack[g_str_stack_idx], str); 
    g_str_stack_idx++; 
} 
void PopStr(char *str)
{
    g_str_stack_idx--;
    if (g_str_stack_idx  < 0) { 
        strcpy(str, " "); 
        g_str_stack_idx=0; 
    } else { 
        strcpy(str, g_str_stack[g_str_stack_idx]); 
    }
    
} 


void FliRule::Interpret(FliEngine *fli_engine, const sint32 module)
{
	uint8 *codePtr = m_code;
	sint32 intarg;
	static FliStack stack;
	BOOL inInput = TRUE;
	FliOutputProc *outputproc = NULL;
	FliSetFunc *func;
	double tmpval;

	BOOL error = FALSE;
    BOOL executing = TRUE; 
	double scalar=0.0;
    double oldscalar = 1.0; 
    double dx = 0.000001; 
#ifdef _DEBUG
	char tmpstr[2048];
    char rule_str[2048];
    char str1[512];
    char str2[512];
    g_str_stack_idx=0;
	if(fli_GetFliLoggingEnabled(fli_engine->m_ai->my_player_index)) {
        rule_str[0]=0; 
    }
#endif

	stack.Clear();

    FLOP_TYPE op;
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif
	while((codePtr < m_code + m_codelen) && !error && executing) {
Assert(++finite_loop < 100000); 
		op = FLOP_TYPE(*codePtr++);
		switch(op) {
			case FLOP_NOP:
				break;
			case FLOP_PUSH:
				intarg = *((sint32 *)codePtr);
				codePtr += sizeof(sint32);
				func = fli_engine->GetFunction(intarg);
				Assert(func);
				if(!func) {
					error = TRUE;
					break;
				}
				if(func->IsInputFunction() && 
				   !func->GetVariable()->IsInitializedForSection(module)) {
					fli_engine->ReportDBError("FLI_NOT_INITIALIZED_WARNING", 
											  func->GetVariable()->GetName(),
											  func->GetName() + 4, 
											  fli_engine->GetSectionName(module));
				}
				tmpval = func->Evaluate(dx);
				stack.Push(tmpval);
#ifdef _DEBUG
				if(fli_GetFliLoggingEnabled(fli_engine->m_ai->my_player_index)) {

                    switch (func->GetType()) { 
                    case SFTYPE_RIGHT: sprintf (str1, "right"); break; 
                    case SFTYPE_LEFT: sprintf (str1, "left"); break; 
                    case SFTYPE_TRI: sprintf (str1, "tri"); break; 
                    case SFTYPE_SPIKE: sprintf (str1, "spike"); break; 
                    default:
                        Assert(0); 
                    } 

					sprintf(tmpstr, "Input %40s(%5s,%4.2f,%4.2f)=%4.2lf because %s=%4.2lf\n",
							func->GetName()+4,
                            str1, 
                            func->GetCenter(), 
                            func->GetWidth(),
							tmpval,
							func->GetVariable()->GetName(), 
							func->GetVariable()->GetValue());
					m_inputStrings->AddTail(new InputString(tmpstr));

                    sprintf(tmpstr, "%s==%3.3lf", func->GetName()+4, tmpval);
                    PushStr (tmpstr);
				}
#endif
				break;
			case FLOP_OUTPUT:
				intarg = *((sint32 *)codePtr);
				codePtr += sizeof(sint32);
				func = fli_engine->GetFunction(intarg);
				Assert(func);
				if(!func) {
					error = TRUE;
					break;
				}
				outputproc = func->GetVariable()->NewOutputProc(module);
				outputproc->Init(min(oldscalar, scalar), func); 
#ifdef _DEBUG
				if(fli_GetFliLoggingEnabled(fli_engine->m_ai->my_player_index)) {

                    sprintf (tmpstr, "(%s=%3.3lf)\n", func->GetName()+4, tmpval);
                    strcat (rule_str, tmpstr); 
					func->GetVariable()->CatLogStrings(m_inputStrings);
				}
#endif
				break;
			case FLOP_AND:
				tmpval = stack.And();
#ifdef _DEBUG
				if(fli_GetFliLoggingEnabled(fli_engine->m_ai->my_player_index)) {

                    PopStr(str1); 
                    PopStr(str2); 
                    sprintf (tmpstr, "(%s AND %s)=%3.3lf\n", str1, str2, tmpval); 
                    PushStr(tmpstr);
                }
#endif

				break;
			case FLOP_OR:
				tmpval = stack.Or();
#ifdef _DEBUG
				if(fli_GetFliLoggingEnabled(fli_engine->m_ai->my_player_index)) {
                    PopStr(str1); 
                    PopStr(str2); 
                    sprintf (tmpstr, "(%s OR %s)=%3.3lf\n", str1, str2, tmpval); 
                    PushStr(tmpstr);
                }
#endif
				break;
			case FLOP_NOT:
                if(inInput) {
					tmpval = stack.Not();
#ifdef _DEBUG
				if(fli_GetFliLoggingEnabled(fli_engine->m_ai->my_player_index)) {
                    PopStr(str1); 
                    sprintf (tmpstr, "NOT(%s)=%3.3lf\n", str1, tmpval); 
                    PushStr(tmpstr);
                }
#endif
                } else { 
                    outputproc->AddModifier(FLOP_NOT);
#ifdef _DEBUG
				if(fli_GetFliLoggingEnabled(fli_engine->m_ai->my_player_index)) {                    
                    sprintf (tmpstr, "%sNOT ", rule_str); 
                    strcpy(rule_str, tmpstr);
                }
#endif           
                }

				break;
			case FLOP_VERY:
                if(inInput) {
					tmpval = stack.Very();
#ifdef _DEBUG
				if(fli_GetFliLoggingEnabled(fli_engine->m_ai->my_player_index)) {
                    PopStr(str1); 
                    sprintf (tmpstr, "VERY(%s)=%3.3lf\n", str1, tmpval); 
                    PushStr(tmpstr);
                }
#endif
                }
                else { 
					outputproc->AddModifier(FLOP_VERY);
#ifdef _DEBUG
				if(fli_GetFliLoggingEnabled(fli_engine->m_ai->my_player_index)) {                    
                    sprintf (tmpstr, "%sVERY ", rule_str); 
                    strcpy(rule_str, tmpstr);
                }
#endif           
                }

				break;
			case FLOP_KINDA:
                if(inInput)  { 
					tmpval = stack.Kinda();
#ifdef _DEBUG
				if(fli_GetFliLoggingEnabled(fli_engine->m_ai->my_player_index)) {
                    PopStr(str1); 
                    sprintf (tmpstr, "KINDA(%s)=%3.3lf\n", str1, tmpval); 
                    PushStr(tmpstr);
                }
#endif

                } else { 
					outputproc->AddModifier(FLOP_KINDA);
#ifdef _DEBUG
				if(fli_GetFliLoggingEnabled(fli_engine->m_ai->my_player_index)) {                    
                    sprintf (tmpstr, "%sKINDA ", rule_str); 
                    strcpy(rule_str, tmpstr);
                }
#endif           
                }

				break;
			case FLOP_ENDEXP:
				inInput = FALSE;
				scalar = stack.GetScalar();

#ifdef _DEBUG
                if(fli_GetFliLoggingEnabled(fli_engine->m_ai->my_player_index)) {
                    PopStr(str1);
                    sprintf (rule_str, "%s\nTHEN\n", str1);
                }
#endif
				break;

            case FLOP_ENDIF:
				inInput = TRUE;
                oldscalar = min(oldscalar, 1.0 - scalar); 
				break;
            case FLOP_STARTELSE:
                scalar = 1.0 - scalar; 
#ifdef _DEBUG
                if(fli_GetFliLoggingEnabled(fli_engine->m_ai->my_player_index)) {
                    sprintf (tmpstr, "ELSE\n");
                    strcat (rule_str, tmpstr); 
                }
#endif
                break; 
            case FLOP_ENDELSE:
				inInput = TRUE;
             	break;
			case FLOP_ENDELSEIF:
#ifdef _DEBUG
                if(fli_GetFliLoggingEnabled(fli_engine->m_ai->my_player_index)) {
                    sprintf (tmpstr, "ELSEIF\n");
                    strcat (rule_str, tmpstr); 
                }
#endif
				inInput = TRUE;  
                oldscalar = min(oldscalar, 1.0 - scalar); 
                break;				
			case FLOP_STOP:
				executing = FALSE;
				break;
			default:
				Assert(FALSE);
				break;
		}
	}

#ifdef _DEBUG
    if(fli_GetFliLoggingEnabled(fli_engine->m_ai->my_player_index)) {
        sprintf (tmpstr, "\nIF\n%s", rule_str); 
        FLPRINTF((tmpstr));
    }
#endif

    Assert(error == FALSE); 
}
