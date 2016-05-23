/*----------------------------------------------------------------------------
 *
 * Project      : Call To Power 2
 * File type    : Yacc input file.
 * Description  : Slic syntax parser
 * Id           : $Id:$
 *
 *----------------------------------------------------------------------------
 *
 * Disclaimer
 *
 * THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
 *
 * This material has been developed at apolyton.net by the Apolyton CtP2
 * Source Code Project. Contact the authors at ctp2source@apolyton.net.
 *
 *----------------------------------------------------------------------------
 *
 * Modifications from the original Activision code:
 *
 * - Addition by Martin Gühmann to allow:
 *   - Slic database access
 *   - Slic database size access
 * - Exponentiation operator '**' added.
 * - Bitwise operators ('&', '|', '^', '~') added.
 * - Added database array access. (Sep 16th 2005 Martin Gühmann)
 *
 *----------------------------------------------------------------------------
 */
%{
/*----------------------------------------------------------------------------
 *
 * Project      : Call To Power 2
 * File type    : C source
 * Description  : Slic syntax parser
 * CAUTION      : This file is generated. Do not edit it. Edit slic.y instead.
 *
 *----------------------------------------------------------------------------
 *
 * Disclaimer
 *
 * THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
 *
 * This material has been developed at apolyton.net by the Apolyton CtP2
 * Source Code Project. Contact the authors at ctp2source@apolyton.net.
 *
 *----------------------------------------------------------------------------
 *
 * Compiler flags
 *
 * _DEBUG
 * Set when generating the debug version.
 *
 *----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "slicif.h"

/* Avoid silly warnings */
#ifdef _MSC_VER
#pragma warning( disable : 4013 )
#endif
#define lint

void yyerror(char* err);

#ifdef _DEBUG
FILE *debuglog;
#endif

int slic_parser_done = 0;
int slic_parse_error;

char slic_parser_error_text[1024];
char *include_filename_stack[MAX_INCLUDE_DEPTH];

int include_stack_ptr = 0;

%}

%union {
	int val;
	char* name;
    char* text;
	void *dbptr;
}

%token NAME STRINGID IDENTIFIER
%token KW_MESSAGEBOX KW_ALERTBOX KW_TRIGGER KW_WHEN KW_IF KW_BUTTON KW_REGION KW_ON
%token KW_HELPBOX KW_ELSE KW_ELSEIF KW_WHILE KW_FOR
%token NUMBER GT LT GTE LTE EQ REF
%token AND OR
%token END_OF_INPUT
%token STRING
%token KW_ONCLOSE
%token KW_UNIT KW_CITY KW_LOCATION KW_INT KW_ARMY
%token KW_FUNCTION KW_VOID
%token KW_RETURN
%token KW_INT_FUNC KW_VOID_FUNC
%token KW_CONST
%token KW_HANDLEEVENT
%token KW_PRE KW_POST
%token KW_EVENT
%token DBREF
%token EXP

%left REF
%left AND OR
// The following are the precedences for bitwise operators to agree with C++
// conventions:
%left '^'
%left '|'
%left '&'
%left GT LT GTE LTE EQ NEQ
%left '-' '+'
%left '*' '/' '%'
%right EXP
%nonassoc UMINUS '!' '~'

%%
slic: objects END_OF_INPUT { slic_parser_done = 1; }
	;

objects: objects object
	| object
	;

object: messagebox
	| trigger
    | eventhandler
	| region
    | typedef
	| function
	| functionproto
	| const
	;

typedef: vartype NAME ';' { slicif_declare_sym($2.name, (SLIC_SYM)$1.val); }
	   | vartype NAME '[' ']' ';' { slicif_declare_array($2.name, (SLIC_SYM)$1.val); }
	   | vartype NAME '[' NUMBER ']' ';' { slicif_declare_fixed_array($2.name, (SLIC_SYM)$1.val, $4.val); }
	   ;

const: KW_CONST NAME '=' NUMBER ';' { slicif_add_const($2.name, $4.val); }
	 | KW_CONST NAME '=' '-' NUMBER ';' { slicif_add_const($2.name, -$4.val); }
	   ;

messagebox: KW_MESSAGEBOX IDENTIFIER { slicif_start_segment($2.name); } body
	{
		struct PSlicObject *obj = malloc(sizeof(struct PSlicObject));
#ifdef _DEBUG
		fprintf(debuglog, "Parsed MessageBox %s\n", $2);
#endif
		obj->m_type = SLIC_OBJECT_MESSAGEBOX;
		obj->m_id = $2.name;
		obj->m_is_alert = 0;
		obj->m_is_help = 0;
		slicif_add_object(obj);
	}
	| KW_ALERTBOX IDENTIFIER { slicif_start_segment($2.name); } body
	{
		struct PSlicObject *obj = malloc(sizeof(struct PSlicObject));
#ifdef _DEBUG
		fprintf(debuglog, "Parsed AlertBox %s\n", $2);
#endif
		obj->m_type = SLIC_OBJECT_MESSAGEBOX;
		obj->m_id = $2.name;
		obj->m_is_alert = 1;
		obj->m_is_help = 0;
		slicif_add_object(obj);
	}
	| KW_HELPBOX IDENTIFIER { slicif_start_segment($2.name); } body
	{
		struct PSlicObject *obj = malloc(sizeof(struct PSlicObject));
#ifdef _DEBUG
		fprintf(debuglog, "Parsed HelpBox %s\n", $2);
#endif
		obj->m_type = SLIC_OBJECT_MESSAGEBOX;
		obj->m_id = $2.name;
		obj->m_is_alert = 0;
		obj->m_is_help = 1;
		slicif_add_object(obj);
	}
	;

trigger: KW_TRIGGER IDENTIFIER { slicif_start_segment($2.name); } KW_WHEN triggercondition body
	{
		struct PSlicObject *obj = malloc(sizeof(struct PSlicObject));
#ifdef _DEBUG
		fprintf(debuglog, "Parsed Trigger %s\n", $2);
#endif

		obj->m_is_alert = 0;
		obj->m_is_help = 0;
		obj->m_type = SLIC_OBJECT_TRIGGER;
		obj->m_id = $2.name;
		obj->m_ui_component = NULL;
		slicif_add_object(obj);
	}
	| KW_TRIGGER IDENTIFIER { slicif_start_segment($2.name); } KW_ON STRING KW_WHEN triggercondition body
	{
		struct PSlicObject *obj = malloc(sizeof(struct PSlicObject));
#ifdef _DEBUG
		fprintf(debuglog, "Parsed Trigger %s\n", $2);
#endif
		obj->m_is_alert = 0;
		obj->m_is_help = 0;
		obj->m_type = SLIC_OBJECT_TRIGGER;
		obj->m_id = $2.name;
		obj->m_ui_component = $5.name;
		slicif_add_object(obj);
	}
	;

eventhandler: KW_HANDLEEVENT '(' NAME ')'
			  {
			      slicif_check_event_exists($3.name);
				  slicif_set_event_checking($3.name);
		      }
			  priority { slicif_start_segment(slicif_create_name($3.name)); } body
    {
		struct PSlicObject *obj = malloc(sizeof(struct PSlicObject));
#ifdef _DEBUG
	    fprintf(debuglog, "Parsed HandleEvent %s\n", $3.name);
#endif
		obj->m_is_alert = 0;
		obj->m_is_help = 0;
		obj->m_type = SLIC_OBJECT_HANDLEEVENT;
		obj->m_id = slicif_get_segment_name_copy();
		obj->m_ui_component = NULL;
		obj->m_event_name = $3.name;
		obj->m_priority = slicif_get_priority();
		slicif_add_object(obj);
	}
    | KW_HANDLEEVENT '(' NAME ')'
	  {
	      slicif_check_event_exists($3.name);
		  slicif_set_event_checking($3.name);
	  }
      IDENTIFIER priority
      {
		  slicif_start_segment($6.name);
	  }
      body
    {
		struct PSlicObject *obj = malloc(sizeof(struct PSlicObject));
#ifdef _DEBUG
	    fprintf(debuglog, "Parsed HandleEvent %s '%s'\n", $3.name, $6.name);
#endif
		obj->m_is_alert = 0;
		obj->m_is_help = 0;
		obj->m_type = SLIC_OBJECT_HANDLEEVENT;
		obj->m_id = $6.name;
		obj->m_ui_component = NULL;
		obj->m_event_name = $3.name;
		obj->m_priority = slicif_get_priority();
		slicif_add_object(obj);
    }
    ;

priority: KW_PRE
          { slicif_set_priority(SLIC_PRI_PRE); }
        | KW_POST
		  { slicif_set_priority(SLIC_PRI_POST); }
        ;

region: KW_REGION NAME '[' NUMBER ',' NUMBER ',' NUMBER ',' NUMBER ']' ';'
		{
		    slicif_add_region($2.name, $4.val, $6.val, $8.val, $10.val);
		}
		| KW_REGION NAME '=' { slicif_start_complex_region($2.name); } regionlist ';'
		{ slicif_finish_complex_region(); }
		;

regionlist: NAME {slicif_add_region_to_complex($1.name); } '+' regionlist
		|	NAME { slicif_add_region_to_complex($1.name); }
		;

functionhead: returntype NAME
              { $$.name = $2.name; slicif_start_segment($2.name); }
              ;

functionproto: functionhead '(' functionarglist ')' ';' { slicif_add_prototype($1.name); }
			   ;

function: functionhead '(' functionarglist ')' body
	{
		struct PSlicObject *obj = malloc(sizeof(struct PSlicObject));
#ifdef _DEBUG
		fprintf(debuglog, "Parsed Function %s\n", $1.name);
#endif
		obj->m_type = SLIC_OBJECT_FUNCTION;
		obj->m_id = $1.name;
		obj->m_is_alert = 0;
		obj->m_is_help = 0;
		slicif_add_object(obj);
	}
	;

returntype: KW_INT_FUNC { slicif_function_return(SF_RET_INT); }
		  | KW_VOID_FUNC { slicif_function_return(SF_RET_VOID); }
		  ;

functionarglist: functionargument
			   | functionarglist ',' functionargument
			   | /* empty */
			   ;

functionargument: vartype NAME { slicif_add_parameter((SLIC_SYM)$1.val, $2.name); }
				  ;

vartype: KW_UNIT { $$.val = SLIC_SYM_UNIT; }
	| KW_ARMY { $$.val = SLIC_SYM_ARMY; }
	| KW_INT { $$.val = SLIC_SYM_IVAR; }
	| KW_CITY { $$.val = SLIC_SYM_CITY; }
	| KW_LOCATION { $$.val = SLIC_SYM_LOCATION; }
	   ;

body: '{' { slicif_start_block(); } statements '}' { slicif_end_block(); }
	| '{' { slicif_start_block(); } '}' { slicif_end_block(); }
	;

statements: statement
	| statement statements
	;

simplestatement: NAME '(' {slicif_add_op(SOP_SARGS);} arguments ')' { slicif_add_op(SOP_CALL, $1.name); }
	| NAME '=' expression { slicif_add_op(SOP_ASSN, $1.name); }
	| NAME '[' expression ']' '=' expression { slicif_add_op(SOP_ASSNA, $1.name); }
	| NAME REF NAME '=' expression { slicif_add_op(SOP_ASSNM, $1.name, $3.name); }
	| NAME '[' expression ']' REF NAME '=' expression { slicif_add_op(SOP_ASSNAM, $1.name, $6.name); }
    ;

statement: simplestatement ';'
	| KW_BUTTON '(' STRINGID ')' body { slicif_add_op(SOP_BUTN, $3.val); }
	| KW_ONCLOSE body { slicif_add_op(SOP_OCLS); }
	| KW_IF                   { slicif_start_if(); }
	  '(' expression ')' body { slicif_add_op(SOP_BNT); }
	  elsestatements          { slicif_end_if(); }
	| KW_WHILE                { slicif_start_while(); }
	  '(' expression ')' body { slicif_end_while(); }
	| KW_RETURN expression ';' { slicif_add_op(SOP_RET); }
	| KW_FOR '(' { slicif_start_for(); } simplestatement ';'
                 { slicif_for_expression(); } expression ';'
                 { slicif_for_continue(); } simplestatement ')'
				 { slicif_start_for_body(); } body { slicif_end_for(); }
	| KW_EVENT ':' NAME '(' {slicif_start_event($3.name); } arguments ')' ';'{ slicif_add_op(SOP_EVENT, $3.name); }
	| typedef
/*	| KW_STRUCT NAME NAME ';' { slicif_add_local_struct($2.name, $3.name); }*/
	;

elsestatements: KW_ELSE body { slicif_add_op(SOP_BNEV); }
		| KW_ELSEIF '(' expression ')' body {slicif_add_op(SOP_BNT); } elsestatements
		| /* nothing */
		;

arguments: argument
	| argument ',' arguments
	|
	;

argument: expression { slicif_add_op(SOP_ARGE); }
	| IDENTIFIER { slicif_add_op(SOP_ARGID, $1.name); }
	| STRINGID { slicif_add_op(SOP_ARGS, $1.val); }
	| STRING { slicif_add_op(SOP_ARGST, $1.name); }
	;

triggercondition: '(' expression ')' { slicif_add_op(SOP_TRIG); }
	;

expression:
	// Arithmetic operations
	    expression '+' expression { slicif_add_op(SOP_ADD); }
	|   expression '-' expression { slicif_add_op(SOP_SUB); }
	|   expression '*' expression { slicif_add_op(SOP_MULT); }
	|   expression '/' expression { slicif_add_op(SOP_DIV); }
	|   expression '%' expression { slicif_add_op(SOP_MOD); }
	|   expression EXP expression { slicif_add_op(SOP_EXP); }
	// Bitwise operations
	|   expression '&' expression { slicif_add_op(SOP_BAND); }
	|   expression '|' expression { slicif_add_op(SOP_BOR); }
	|   expression '^' expression { slicif_add_op(SOP_BXOR); }
	|   '~' expression            { slicif_add_op(SOP_BNOT); }
	// Comparisons
	|   expression LT  expression { slicif_add_op(SOP_LT); }
	|   expression GT  expression { slicif_add_op(SOP_GT); }
	|   expression GTE expression { slicif_add_op(SOP_GTE); }
	|   expression LTE expression { slicif_add_op(SOP_LTE); }
	|   expression EQ  expression { slicif_add_op(SOP_EQ); }
	|   expression NEQ expression { slicif_add_op(SOP_NEQ); }
	// Boolean operations
	|	expression AND expression { slicif_add_op(SOP_AND); }
	|	expression OR  expression { slicif_add_op(SOP_OR); }
	|  	'!' expression %prec '!' { slicif_add_op(SOP_NOT); }
	|   '-' expression %prec UMINUS { slicif_add_op(SOP_NEG); }
	|   '(' expression ')' { }
	|   NUMBER { slicif_add_op(SOP_PUSHI, $1.val); }
	|   NAME { slicif_add_op(SOP_PUSHV, $1.name); }
	|   NAME '(' {slicif_add_op(SOP_SARGS); } arguments ')' { slicif_add_op(SOP_CALLR, $1.name); }
	|   NAME REF NAME { slicif_add_op(SOP_PUSHM, $1.name, $3.name); }
	|   arrayref {slicif_add_op(SOP_AINDX); }
	|   arrayref REF NAME { slicif_add_op(SOP_PUSHAM, $1.name, $3.name); }
	|   NAME REF '#' { slicif_add_op(SOP_ASIZE, $1.name); }
	|   DBREF '(' NAME ')'
	    {
	        if (slicif_is_name($1.dbptr, $3.name) < 0)
			{
		        slicif_add_op(SOP_DBNAME, $1.dbptr, $3.name);
	        }
	        else
			{
		        slicif_add_op(SOP_PUSHI, slicif_find_db_index($1.dbptr, $3.name));
	        }
        }
	|   DBREF '(' NAME ')' REF NAME
	    {
            if (slicif_is_name($1.dbptr, $3.name) < 0)
			{
		        slicif_add_op(SOP_DBNAMEREF, $1.dbptr, $3.name, $6.name);
	        }
	        else
			{
		        slicif_add_op(SOP_PUSHI, slicif_find_db_value($1.dbptr, $3.name, $6.name));
	        }
        }
	|   DBREF '(' NAME ')' REF NAME '[' expression ']'
	    {
            if (slicif_is_name($1.dbptr, $3.name) < 0)
			{
		        slicif_add_op(SOP_DBNAMEARRAY, $1.dbptr, $3.name, $6.name);
	        }
	        else
			{
				slicif_add_op(SOP_DBNAMECONSTARRAY, $1.dbptr, slicif_find_db_index($1.dbptr, $3.name), $6.name);
	        }
        }
	|   DBREF '(' expression ')' { slicif_add_op(SOP_DB, $1.dbptr); }
	|   DBREF '(' expression ')' REF NAME { slicif_add_op(SOP_DBREF, $1.dbptr, $6.name); }
	|   DBREF '(' expression ')' REF NAME '[' expression ']' { slicif_add_op(SOP_DBARRAY, $1.dbptr, $6.name); }
	|   DBREF '(' ')' {	slicif_add_op(SOP_DBSIZE, $1.dbptr); }
/*	|   NAME '[' expression ']' REF NAME { slicif_add_op(SOP_PUSHAM, $1.name, $6.name); }*/
	;

arrayref: NAME { $$.name = $1.name; slicif_add_op(SOP_PUSHA, $1.name); } '[' expression ']'
	;
%%

extern FILE *yyin;
int yyparse();

SLIC_ERROR slicif_run_parser(char* filename, int symStart)
{
#ifdef _DEBUG
	static int debug_opened = 0;
	if(!debug_opened) {
		debuglog = fopen("slicdbg.txt", "w");
		debug_opened = 1;
	} else {
		debuglog = fopen("slicdbg.txt", "a");
	}
#endif
	slicif_set_start(symStart);

	g_slicLineNumber = 1;

	if(!slicif_open_first_file(filename)) {
		return slic_parse_error = SLIC_ERROR_CANT_OPEN_FILE;
	}

	slic_parse_error = SLIC_ERROR_OK;
	slic_parser_done = 0;

	slicif_start();

    do {
		yyparse();
    } while(!feof(yyin) && slic_parse_error == SLIC_ERROR_OK && !slic_parser_done);

#ifdef _DEBUG
	fclose(debuglog);
	debuglog = NULL;
#endif
	fclose(yyin);
	yyin = NULL;
	return slic_parse_error;
}

const char *slicif_get_filename()
{
	return include_filename_stack[include_stack_ptr];
}

void yyerror(char *s)
{
    extern int line;
#ifdef _DEBUG
    if(debuglog)
		fprintf(debuglog, "line %d: %s\n", g_slicLineNumber, s);
#endif
	sprintf(slic_parser_error_text, "%s:%d: %s",
			slicif_get_filename(), g_slicLineNumber, s);
	slicif_report_error(slic_parser_error_text);
	/*slic_parse_error = SLIC_ERROR_SYNTAX;*/
}
