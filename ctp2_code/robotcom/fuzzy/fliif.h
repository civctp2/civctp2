
#pragma once

#ifndef __FLIIF_H__
#define __FLIIF_H__

#ifdef __cplusplus
extern "C" {
#endif

enum  {
    SFTYPE_RIGHT,
    SFTYPE_LEFT,
    SFTYPE_TRI, 
    SFTYPE_SPIKE
};
typedef int SFTYPE;

enum {
	FLI_OK,
	FLI_CANT_OPEN_FILE,
	FLI_PARSER_ERROR,
	FLI_LEX_INCLUDE_ERROR,
	FLI_TOO_MANY_FILES
};

enum FLOP_TYPE{
	FLOP_NOP,
	FLOP_PUSH,
	FLOP_AND,
	FLOP_OR,
	FLOP_NOT,
	FLOP_VERY,
	FLOP_KINDA,
	FLOP_ENDIF,
    FLOP_STARTELSE,
	FLOP_ENDELSE,
	FLOP_ENDELSEIF,
	FLOP_OUTPUT,
	FLOP_ENDEXP,
	FLOP_STOP
	
};

enum {
	BFLOP_LT,
	BFLOP_EQ,
	BFLOP_GT,
	BFLOP_NEQ
};

typedef int BFLOP_TYPE;




enum {
	FLI_SECT_EVAL,
    FLI_SECT_INIT, 
	FLI_SECT_BEGINTURN,
    FLI_SECT_ENDTURN, 
	FLI_SECT_PRE_INCOMING_DIPLOMACY,
	FLI_SECT_POST_INCOMING_DIPLOMACY,
	FLI_SECT_PRE_OUTGOING_DIPLOMACY,
	FLI_SECT_POST_OUTGOING_DIPLOMACY,
	FLI_SECT_BEGIN_DIPLOMACY,
	FLI_SECT_REJECTED_MESSAGE,
	FLI_SECT_ACCEPTED_MESSAGE,
	FLI_SECT_MAX 
};

#ifdef PARSER_INTERFACE
typedef void FliSymbol;
#else
class FliSymbol;
#endif

extern int fli_parse_error;
extern int fli_parse_done;
extern int fli_lineNumber;

void fli_AddInputSymbol(char *name);
void fli_AddInitializedInputSymbol(char *name, double value);
void fli_AddOutputSymbol(char *name, double minValue, double maxValue);
void fli_AddInitializedOutputSymbol(char *name, double value, double min, double max);
FliSymbol *fli_LookupSymbol(char *name);
void fli_AddSetFunction(char *range_name, char *symbolname, 
						SFTYPE sftype, double v1, double v2);

int fli_RunParser(const char *topdir, const char *file);
void fli_Init();
int fli_include_file(char *file);
int fli_open_first_file(const char *topdir, const char *file);
const char *fli_current_file();

void fli_AddOp(int op, ...);
void fli_EndRule();

void fli_SetSection(int section);

void fli_StartAction(const char *name);
void fli_EndAction();
void fli_NextArgument(const char *str);
void fli_SetBoolExp(const char *varname, BFLOP_TYPE op, double value);

void fli_StartWhen();
void fli_WhenExpression();
void fli_EndWhen();




void fli_ReportError(const char *s);


void fli_DumpCode();
void fli_DebugPrintf(char *fmt, ...);
BOOL fli_GetFliLoggingEnabled(int owner);
void fli_SetFliLoggingEnabled(int owner, BOOL enabled);
void fli_SetCurrentPlayer(int owner);
#define FLPRINTF(x) fli_DebugPrintf x




#ifdef __cplusplus
}
#endif



#endif 
