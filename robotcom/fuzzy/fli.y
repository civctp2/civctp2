%{
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

/* Avoid silly warnings */
#pragma warning( disable : 4013 )
#define lint

#define PARSER_INTERFACE
#include "fliif.h"

void yyerror(char *s);
int yyparse();

int fli_parse_error;
int fli_parse_done;

extern int fli_lineNumber;

#define YYSTACKSIZE 2000

%}


%union {
	double value;
	int sftype;
	char *name;
}



%token NAME NUMBER IF INPUT OUTPUT NOT AND OR ELSE ELSEIF LEFT RIGHT TRI SPIKE
%token FUZZY_EVAL FUZZY_INIT FUZZY_DIP FUZZY_BEGINTURN FUZZY_ENDTURN VERY KINDA END_OF_INPUT
%token FUZZY_PRE_INC_DIP FUZZY_POST_INC_DIP FUZZY_PRE_OUT_DIP FUZZY_POST_OUT_DIP
%token FUZZY_REJECTED_MESSAGE FUZZY_ACCEPTED_MESSAGE
%token ACTION WHEN
%token STRING

%left OR
%left AND
%nonassoc VERY KINDA
%nonassoc NOT

%%
fli: 		rulesections END_OF_INPUT { fli_parse_done = 1; FLPRINTF(("End of input\n")); }
			;

rulesections: rulesection
			| rulesection rulesections
			;

rulesection:  sectiontype NAME '{' statements '}'
			| sectiontype '{' statements '}'
		    | sectiontype NAME '{' '}'
			| sectiontype '{' '}'
			 ;

sectiontype:	FUZZY_EVAL { fli_SetSection(FLI_SECT_EVAL); }
				| FUZZY_INIT { fli_SetSection(FLI_SECT_INIT); }
				| FUZZY_BEGINTURN { fli_SetSection(FLI_SECT_BEGINTURN); }
				| FUZZY_ENDTURN { fli_SetSection(FLI_SECT_ENDTURN); }
				| FUZZY_DIP { fli_SetSection(FLI_SECT_BEGIN_DIPLOMACY); }
				| FUZZY_PRE_INC_DIP { fli_SetSection(FLI_SECT_PRE_INCOMING_DIPLOMACY); }
				| FUZZY_POST_INC_DIP { fli_SetSection(FLI_SECT_POST_INCOMING_DIPLOMACY); }
				| FUZZY_PRE_OUT_DIP { fli_SetSection(FLI_SECT_PRE_OUTGOING_DIPLOMACY); }
				| FUZZY_POST_OUT_DIP { fli_SetSection(FLI_SECT_POST_OUTGOING_DIPLOMACY); }
				| FUZZY_REJECTED_MESSAGE { fli_SetSection(FLI_SECT_REJECTED_MESSAGE); }
				| FUZZY_ACCEPTED_MESSAGE { fli_SetSection(FLI_SECT_ACCEPTED_MESSAGE); }
				; 

statements:	statement
			| statement statements
			;

statement:	inputdef
			| outputdef
			| setfuncdef
			| rule
			| boolsect
			| action
			;

inputdef:	INPUT NAME  { fli_AddInputSymbol($2.name); }
			| INPUT NAME '=' NUMBER  
			{ fli_AddInitializedInputSymbol($2.name, $4.value); }
			;

outputdef:	OUTPUT NAME '[' NUMBER ',' NUMBER ']' 
			{ 
				fli_AddOutputSymbol($2.name, $4.value, $6.value);
			}
			| OUTPUT NAME '[' NUMBER ',' NUMBER ']' '=' NUMBER  
			{ 
				fli_AddInitializedOutputSymbol($2.name, $9.value, $4.value, $6.value);
			}
			;

setfuncdef:	setfunctype NAME '(' NAME ',' NUMBER ',' NUMBER ')' 
			{
				fli_AddSetFunction($2.name, $4.name, $1.sftype,
								   $6.value, $8.value);
			}   
			;

setfunctype:  RIGHT { $$.sftype = SFTYPE_RIGHT; }
			| LEFT { $$.sftype = SFTYPE_LEFT; }
			| TRI { $$.sftype = SFTYPE_TRI; }
			| SPIKE { $$.sftype = SFTYPE_SPIKE; }
			;

boolsect:   WHEN { fli_StartWhen(); } '(' boolexp ')' whenbody
			;

whenbody:   '{' rules '}' { fli_EndWhen(); }
			| rulebody { fli_AddOp(FLOP_ENDIF); fli_EndRule(); fli_EndWhen(); }
			;

rules:      rules rule
			| rule
			;

rule:		ifclause { fli_EndRule(); }
			| ifclause elseclauses { fli_EndRule(); }
			;

ifclause:	IF '(' inputexp ')' { fli_AddOp(FLOP_ENDEXP); } rulebody { fli_AddOp(FLOP_ENDIF); }
			;

elseclauses:	elseclause
				| elseifclauses elseclause
				| elseifclauses
				;
 
elseclause:	ELSE { fli_AddOp(FLOP_ENDEXP); }   { fli_AddOp(FLOP_STARTELSE); }  rulebody { fli_AddOp(FLOP_ENDELSE); }
			;

elseifclauses:	elseifclause
				| elseifclause elseifclauses
				;

elseifclause:	ELSEIF '(' inputexp ')' { fli_AddOp(FLOP_ENDEXP); }
						rulebody { fli_AddOp(FLOP_ENDELSEIF); }
				;

rulebody:	outputexp
			| '{' outputexps '}'
			;

outputexps:	outputexp
			| outputexp outputexps
			;

outputexp:	NAME  { fli_AddOp(FLOP_OUTPUT, $1.name); }
			| NOT outputexp { fli_AddOp(FLOP_NOT); }
			| VERY outputexp { fli_AddOp(FLOP_VERY); }
			| KINDA outputexp { fli_AddOp(FLOP_KINDA); }
			;

inputexp:	NAME { fli_AddOp(FLOP_PUSH, $1.name); }
			| inputexp AND inputexp { fli_AddOp(FLOP_AND); }
			| inputexp OR inputexp { fli_AddOp(FLOP_OR); }
			| '(' inputexp ')' { }
			| NOT inputexp { fli_AddOp(FLOP_NOT); }
			| VERY inputexp { fli_AddOp(FLOP_VERY); }
			| KINDA inputexp { fli_AddOp(FLOP_KINDA); }
			;

action:		ACTION NAME {fli_StartAction($2.name);} 
				'(' arguments ')' WHEN '(' boolexp ')'
				{ fli_EndAction(); }
			;

arguments:	argument
			| argument ',' argument
			;

argument:   STRING { fli_NextArgument($1.name); }
			;

boolexp:    NAME '<' NUMBER { fli_SetBoolExp($1.name, BFLOP_LT, $3.value); }
			| NAME '=' NUMBER { fli_SetBoolExp($1.name, BFLOP_EQ, $3.value); }
			| NAME '>' NUMBER { fli_SetBoolExp($1.name, BFLOP_GT, $3.value); }
			| NAME { fli_SetBoolExp($1.name, BFLOP_NEQ, 0); fli_AddOp(FLOP_PUSH, "alwaysone"); }
			;

%%

extern FILE *yyin;

int fli_RunParser(const char *topdir, const char *file)
{
    char my_errstr[200]; 
    
	fli_lineNumber = 1;

	if(!fli_open_first_file(topdir, file)) {
		fli_parse_error = FLI_CANT_OPEN_FILE;
       
        sprintf (my_errstr, "Can not open file %s %s", topdir, file);
        yyerror(my_errstr); 

		return 0;
	}

	fli_parse_done = 0;
	fli_parse_error = FLI_OK;

	fli_Init();
	do {
		yyparse();
	} while(!fli_parse_done && (fli_parse_error == FLI_OK));
	if(fli_parse_error == FLI_OK)
		return 1;
	else
		return 0;
}

void yyerror(char *s)
{
	char buf[1024];
	fli_parse_error = FLI_PARSER_ERROR;
	
	sprintf(buf, "%s:%d: %s", fli_current_file(), fli_lineNumber, s);
	FLPRINTF(("%s\n", buf));
	
	fli_ReportError(buf);
}

