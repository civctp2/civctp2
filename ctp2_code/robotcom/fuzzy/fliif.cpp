#include "c3.h"
#include "fliif.h"
#include <stdio.h>
#include <stdarg.h>
#include "FliEngine.h"
#include "FliSymbol.h"
#include "FliSetFunc.h"
#include "FliWhen.h"

#include "pointerlist.h"

#define k_FLI_MAX_CODE_SIZE 32768

static uint8 s_code[k_FLI_MAX_CODE_SIZE];
static uint8 *s_codePtr;

static sint32 s_currentSection;

static BOOL		s_fliLoggingEnabled[32];

PointerList<FliSymbol> *s_currentInputSymbols = NULL;
PointerList<FliSymbol> *s_currentOutputSymbols = NULL;

FliEngine *current_fli_engine;
static FliWhen *s_currentWhen = NULL;

sint32 s_currentPlayer = 0;

void SetCurrentEngine(FliEngine *e)
{
    current_fli_engine = e;
}

extern "C" {
void fli_Init()
{

	s_codePtr = s_code;
	s_currentInputSymbols = new PointerList<FliSymbol>;
	s_currentOutputSymbols = new PointerList<FliSymbol>;
	sint32 i;
	for(i = 0; i < 32; i++) {
		s_fliLoggingEnabled[i] = 0;
	}
}

void fli_AddInputSymbol(char *name)
{
    FLPRINTF(("Add input sym %s\n", name));

	FliSymbol *sym = current_fli_engine->GetInputSymbol(name);

	if(sym == NULL) {
		current_fli_engine->ReportDBError("FLI_UNKNOWN_INPUT",
								   fli_current_file(), fli_lineNumber,
								   name);
		return;
	}
	sym->SetDeclared();
}

void fli_AddInitializedInputSymbol(char *name, double value)
{
	FLPRINTF(("Add sym %s, val=%f\n", name, value));

	FliSymbol *sym = current_fli_engine->GetInputSymbol(name);
	if(!sym) {
		current_fli_engine->ReportDBError("FLI_UNKNOWN_INPUT",
								   fli_current_file(), fli_lineNumber,
								   name);
		return;
	}

	sym->SetInitialValue(s_currentSection, value);
	sym->SetDeclared();
	sym->RegisterInit(s_currentSection);
}

void fli_AddOutputSymbol(char *name, double minValue, double maxValue)
{
	FLPRINTF(("Add output sym %s\n", name));

	FliSymbol *sym = current_fli_engine->GetSymbol(name);
	if(sym == NULL) {
		sym = current_fli_engine->AddExternalSymbol(name, 0.0);
	}
	Assert(sym);
	if(!sym) {
		current_fli_engine->ReportDBError("FLI_INTERNAL_ERROR",
								   fli_current_file(), fli_lineNumber);
		return;
	}
	sym->SetMinMax(minValue, maxValue);
	sym->SetDeclared();
}

void fli_AddInitializedOutputSymbol(char *name, double value,
									double minValue, double maxValue)
{
	FLPRINTF(("Add output sym %s, val=%f\n", name, value));
	FliSymbol *sym = current_fli_engine->GetSymbol(name);
	if(sym == NULL) {
		sym = current_fli_engine->AddExternalSymbol(name, value);
		Assert(sym);
		if(!sym) {
			current_fli_engine->ReportDBError("FLI_INTERNAL_ERROR",
									   fli_current_file(), fli_lineNumber);
			return;
		}
	}

    if ((value < minValue) || (maxValue < value)) {
			current_fli_engine->ReportDBError("FLI_INTERNAL_ERROR",
									   fli_current_file(), fli_lineNumber);
            return;
    }

	sym->SetInitialValue(s_currentSection, value);
	sym->SetMinMax(minValue, maxValue);
	sym->SetDeclared();
}

void fli_AddSetFunction(char *set_func_name, char *symbolname,
						SFTYPE sftype, double v1, double v2)
{
	FLPRINTF(("Add set func %s on var %s, type %d, (%f,%f)\n",
		   set_func_name, symbolname,
		   sftype, v1, v2));
	FliSymbol *sym = current_fli_engine->GetSymbol(symbolname);
	if(!sym || !sym->IsDeclared()) {
		current_fli_engine->ReportDBError("FLI_UNDECLARED_VARIABLE",
								 fli_current_file(), fli_lineNumber,
								 symbolname);
		return;
	}

	char actualname[1024];
	sprintf(actualname, "s%02d_%s", s_currentSection, set_func_name);
	current_fli_engine->AddFunction(actualname, sym, sftype, v1, v2);
}

void fli_EndRule()
{
	FLPRINTF(("fli_EndRule:\n"));
	fli_AddOp(FLOP_STOP);

#ifdef DBGAILOG
	fli_DumpCode();
#endif

	current_fli_engine->AddRule(s_code, s_codePtr - s_code, s_currentInputSymbols,
								s_currentOutputSymbols,
								s_currentSection,
								s_currentWhen);




	s_currentInputSymbols = new PointerList<FliSymbol>;
	s_currentOutputSymbols = new PointerList<FliSymbol>;

	s_codePtr = s_code;
}

void fli_AddOp(int op, ...)
{
	va_list vl;
	char *name;
	FliSetFunc *func;
	char actualname[1024];

	va_start(vl, op);
	*s_codePtr = (uint8)op;
	s_codePtr++;
	switch(op) {
		case FLOP_NOP:
			break;
		case FLOP_PUSH:
			name = va_arg(vl, char*);
			sprintf(actualname, "s%02d_%s", s_currentSection, name);
			func = current_fli_engine->GetFunction(actualname);
			if(func) {
				if(!s_currentInputSymbols->Find(func->GetVariable())) {
					s_currentInputSymbols->AddTail(func->GetVariable());
				}
				*((int *)s_codePtr) = func->GetIndex();
				s_codePtr += sizeof(int);
			} else {
				current_fli_engine->ReportDBError("FLI_UNKNOWN_SET_FUNCTION",
										   fli_current_file(), fli_lineNumber,
										   name);
				s_code[0] = FLOP_STOP;
				*((int *)s_codePtr) = -1;
				s_codePtr += sizeof(int);
			}
			break;
		case FLOP_OUTPUT:
			name = va_arg(vl, char *);
			sprintf(actualname, "s%02d_%s", s_currentSection, name);
			func = current_fli_engine->GetFunction(actualname);
			if(func) {
				if(!s_currentOutputSymbols->Find(func->GetVariable())) {
					s_currentOutputSymbols->AddTail(func->GetVariable());
					func->GetVariable()->AddDependance(s_currentSection);
				}
				*((int *)s_codePtr) = func->GetIndex();
				s_codePtr += sizeof(int);
			} else {
				current_fli_engine->ReportDBError("FLI_UNKNOWN_SET_FUNCTION",
										   fli_current_file(), fli_lineNumber,
										   name);
				s_code[0] = FLOP_STOP;
				*((int *)s_codePtr) = -1;
				s_codePtr += sizeof(int);
			}
			break;
		case FLOP_AND:
		case FLOP_OR:
		case FLOP_NOT:
		case FLOP_VERY:
		case FLOP_KINDA:
		case FLOP_ENDIF:
        case FLOP_STARTELSE:
		case FLOP_ENDELSE:
		case FLOP_ENDELSEIF:
		case FLOP_ENDEXP:
			break;

		case FLOP_STOP:
			break;
		default:
			current_fli_engine->ReportDBError("FLI_INTERNAL_ERROR",
									   fli_current_file(), fli_lineNumber);
			break;
	}
	va_end(vl);
}

void fli_SetSection(int section)
{
	s_currentSection = section;
}

void fli_ReportError(const MBCHAR *s)
{
	current_fli_engine->ReportError(s);
}




void fli_StartAction(const char *name)
{
	current_fli_engine->StartAction(name);
}

void fli_EndAction()
{
	current_fli_engine->EndAction(s_currentSection);
}

void fli_NextArgument(const char *str)
{
	current_fli_engine->AddArgument(str);
}

void fli_SetBoolExp(const char *varname, BFLOP_TYPE op, double value)
{
	if(s_currentWhen) {
		s_currentWhen->Set(current_fli_engine->GetSymbol(varname), op, value);
	} else {
		current_fli_engine->SetBoolExp(varname, op, value);
	}
}

void fli_StartWhen()
{
	Assert(!s_currentWhen);
	s_currentWhen = new FliWhen();
	s_currentWhen->AddRef();
}

void fli_WhenExpression()
{
}

void fli_EndWhen()
{
	s_currentWhen->Release();
	s_currentWhen = NULL;
}

void fli_DumpCode()
{
	uint8 *codePtr;
	uint8 op;
	sint32 funcindex;
	FliSetFunc *func;
	for(codePtr = s_code; codePtr < s_codePtr;) {
		FLPRINTF(("0x%04x: ", codePtr - s_code));
		op = *codePtr;
		codePtr++;
		switch(op) {
			case FLOP_NOP:
				FLPRINTF(("NOP\n"));
				break;
			case FLOP_PUSH:
				funcindex = *((sint32 *)codePtr);
				if(funcindex >= 0) {
					func = current_fli_engine->GetFunction(funcindex);
					if(func) {
						FLPRINTF(("PUSH %s\n", func->GetName()));
					} else {
						FLPRINTF(("PUSH <<bad>>\n"));
					}
				} else {
					FLPRINTF(("PUSH <<unknown>>\n"));
				}
				codePtr += sizeof(sint32);
				break;
			case FLOP_AND:
				FLPRINTF(("AND\n"));
				break;
			case FLOP_OR:
				FLPRINTF(("OR\n"));
				break;
			case FLOP_NOT:
				FLPRINTF(("NOT\n"));
				break;
			case FLOP_VERY:
				FLPRINTF(("VERY\n"));
				break;
			case FLOP_KINDA:
				FLPRINTF(("KINDA\n"));
				break;
			case FLOP_STOP:
				FLPRINTF(("STOP\n"));
				return;
				break;
			case FLOP_ENDIF:
				FLPRINTF(("ENDIF\n"));
				break;
            case FLOP_STARTELSE:
                FLPRINTF(("STARTELSE\n"));
                break;
			case FLOP_ENDELSE:
				FLPRINTF(("ENDELSE\n"));
				break;
			case FLOP_ENDELSEIF:
				FLPRINTF(("ENDELSEIF\n"));
				break;
			case FLOP_OUTPUT:
				funcindex = *((sint32 *)codePtr);
				if(funcindex >= 0) {
					func = current_fli_engine->GetFunction(funcindex);
					FLPRINTF(("OUTPUT %s\n", func->GetName()));
				} else {
					FLPRINTF(("OUTPUT <<unknown>>\n"));
				}
				codePtr += sizeof(sint32);
				break;
			case FLOP_ENDEXP:
				FLPRINTF(("ENDEXP\n"));
				break;
			default:
				FLPRINTF(("???\n"));
				break;
		}
	}
}

void fli_DebugPrintf(char *fmt, ...)
{

	if (!s_fliLoggingEnabled[s_currentPlayer]) return;

	va_list list;
	FILE *f;
	static int firstcall = 1;
	if(firstcall) {
		f = fopen("logs\\flidbg.txt", "w");
		firstcall = 0;
	} else {
		f = fopen("logs\\flidbg.txt", "a");
	}
	if(!f)
		return;

	va_start(list, fmt);
	vfprintf(f, fmt, list);
	va_end(list);
	fclose(f);
}

BOOL fli_GetFliLoggingEnabled(int owner)
{
	return s_fliLoggingEnabled[owner];
}

void fli_SetFliLoggingEnabled(int owner, BOOL enabled)
{
	s_fliLoggingEnabled[owner] = enabled;
}

void fli_SetCurrentPlayer(int owner)
{
	s_currentPlayer = owner;
}


}
