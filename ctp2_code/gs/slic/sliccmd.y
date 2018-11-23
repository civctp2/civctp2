%{
//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Defines what valid slic commands are.
//                This file sliccmd.y is compiled to sc.tab.c
//                and therefore you find this text in both files.
// Id           : $Id:$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Exponetiation operator '**' added.
// - Added database array access. (Sep 16th 2005 Martin G�hmann)
//
//----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sliccmd.h"
#include "slicif.h"
#include <math.h>

/* Avoid silly warnings */
#ifdef _MSC_VER
#pragma warning( disable : 4013 )
#endif
#define lint

void yyerror(const char* err);
int yysclex(void);

char *sliccmd_output;
int sliccmd_output_len;
const char *sliccmd_input;
const char *sliccmd_input_ptr;
int sliccmd_input_len;
int sliccmd_first_token;
int sliccmd_done;
int sliccmd_parse_failed;
int sliccmd_use_dialogs;
const char *sliccmd_cat_string;

%}

%union {
	struct sliccmdExpValue v;
}

%token NAME DBREF STRINGID IDENTIFIER
%token NUMBER GT LT GTE LTE EQ REF
%token AND OR
%token STRING
%token SLICCMD_WATCH SLICCMD_ASSN SLICCMD_EVAL SLICCMD_REPLACE
%token EXP

%left REF
%left AND OR
%left GT LT GTE LTE EQ NEQ
%left '-' '+'
%left '*' '/' EXP
%nonassoc UMINUS '!'

%%
command: SLICCMD_WATCH expression
         {
			 sliccmd_parse_done(&$2.v, SLICCMD_WATCH);
			 sliccmd_done = 1;
		 }
         | SLICCMD_EVAL expression
         {
			 sliccmd_parse_done(&$2.v, SLICCMD_EVAL);
			 sliccmd_done = 1;
		 }
		 | SLICCMD_ASSN lvalue '=' expression
		 {
			 sliccmd_assign($2.v.sym, &$4.v);
			 sliccmd_done = 1;
		 }
         | SLICCMD_REPLACE expression
		 {
			 sliccmd_parse_done(&$2.v, SLICCMD_REPLACE);
			 sliccmd_done = 1;
		 }
         ;

lvalue: NAME
		{
			$$.v.type = EXP_VAL_SYM;
			$$.v.sym = sliccmd_lookup_sym($1.v.name);
		}
		| NAME REF NAME
		{
			$$.v.type = EXP_VAL_SYM;
			$$.v.sym = sliccmd_get_ref_sym($1.v.name, $3.v.name);
		}
		| NAME '[' expression ']'
		{
			$$.v.type = EXP_VAL_SYM;
			$$.v.sym = sliccmd_array_lookup_sym($1.v.name, $3.v.value);
		}
		| NAME '[' expression ']' REF NAME
		{
			if($3.v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			    $$.v.type = EXP_VAL_SYM;
			    $$.v.sym = sliccmd_array_lookup_reference($1.v.name, $3.v.value, $6.v.name);
			}
		}
		;

arguments: argument
	| argument ',' arguments
	|
	;

argument: expression { sliccmd_arg_exp($1.v.value); }
	| IDENTIFIER { sliccmd_arg_id($1.v.name); }
	| STRINGID { sliccmd_arg_stringid($1.v.value); }
	| STRING { sliccmd_arg_string($1.v.name); }
	;

expression: expression '+' expression
    {
		if($1.v.type != EXP_VAL_INT ||
		   $3.v.type != EXP_VAL_INT) {
			sliccmd_type_error();
		} else {
			$$.v.type = EXP_VAL_INT;
			$$.v.value = $1.v.value + $3.v.value;
		}
	}
	|   expression '-' expression
        {
			if($1.v.type != EXP_VAL_INT ||
			   $3.v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			$$.v.type = EXP_VAL_INT;
				$$.v.value = $1.v.value - $3.v.value;
			}
		}
	|   expression '*' expression
        {
			if($1.v.type != EXP_VAL_INT ||
			   $3.v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			$$.v.type = EXP_VAL_INT;
				$$.v.value = $1.v.value * $3.v.value;
			}
		}
	|   expression '/' expression
        {
			if($1.v.type != EXP_VAL_INT ||
			   $3.v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			$$.v.type = EXP_VAL_INT;
				$$.v.value = $1.v.value / $3.v.value;
			}
		}
	|   expression EXP expression
        {
			if($1.v.type != EXP_VAL_INT ||
			   $3.v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			$$.v.type = EXP_VAL_INT;
				$$.v.value = (int)pow($1.v.value, $3.v.value);
			}
		}
	|   expression LT  expression
        {
			if($1.v.type != EXP_VAL_INT ||
			   $3.v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			$$.v.type = EXP_VAL_INT;
				$$.v.value = $1.v.value < $3.v.value;
			}
		}
	|   expression GT  expression
        {
			if($1.v.type != EXP_VAL_INT ||
			   $3.v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			$$.v.type = EXP_VAL_INT;
				$$.v.value = $1.v.value > $3.v.value;
			}
		}
	|   expression GTE expression
        {
			if($1.v.type != EXP_VAL_INT ||
			   $3.v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			$$.v.type = EXP_VAL_INT;
				$$.v.value = $1.v.value >= $3.v.value;
			}
		}
	|   expression LTE expression
        {
			if($1.v.type != EXP_VAL_INT ||
			   $3.v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			$$.v.type = EXP_VAL_INT;
				$$.v.value = $1.v.value <= $3.v.value;
			}
		}
	|   expression EQ  expression
        {
			if($1.v.type != EXP_VAL_INT ||
			   $3.v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			$$.v.type = EXP_VAL_INT;
				$$.v.value = $1.v.value == $3.v.value;
			}
		}
	|   expression NEQ expression
        {
			if($1.v.type != EXP_VAL_INT ||
			   $3.v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			$$.v.type = EXP_VAL_INT;
				$$.v.value = $1.v.value != $3.v.value;
			}
		}
	|	expression AND expression
        {
			if($1.v.type != EXP_VAL_INT ||
			   $3.v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			$$.v.type = EXP_VAL_INT;
				$$.v.value = $1.v.value && $3.v.value;
			}
		}
	|	expression OR  expression
        {
			if($1.v.type != EXP_VAL_INT ||
			   $3.v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			$$.v.type = EXP_VAL_INT;
				$$.v.value = $1.v.value || $3.v.value;
			}
		}
	|  	'!' expression %prec '!'
        {
			if($2.v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			$$.v.type = EXP_VAL_INT;
				$$.v.value = !($2.v.value);
			}
		}
	|   '-' expression %prec UMINUS
        {
			if($2.v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			$$.v.type = EXP_VAL_INT;
				$$.v.value = -($2.v.value);
			}
		}
	|   '(' expression ')'
        {
			if($2.v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			$$.v.type = EXP_VAL_INT;
				$$.v.value = $2.v.value;
			}
		}
	|   NUMBER
        {
			$$.v.type = EXP_VAL_INT;
			$$.v.value = $1.v.value;
		}
	|   NAME
        {
			if(sliccmd_has_int_value($1.v.name)) {
				$$.v.type = EXP_VAL_INT;
				$$.v.value = sliccmd_get_int_value($1.v.name);
			} else if(($$.v.sym = sliccmd_maybe_lookup_sym($1.v.name)) != NULL) {
				$$.v.type = EXP_VAL_SYM;
				$$.v.sym = sliccmd_lookup_sym($1.v.name);
			} else {
				$$.v.type = EXP_VAL_SYM;
				strcpy($$.v.name, $1.v.name);
			}
		}
	|   NAME '(' arguments ')'
		{
		    $$.v.value = sliccmd_call($1.v.name);
		}
	|   NAME REF NAME
        {
			if(sliccmd_ref_has_int_value($1.v.name, $3.v.name)) {
				$$.v.type = EXP_VAL_INT;
				$$.v.value = sliccmd_get_ref_value($1.v.name, $3.v.name);
			} else {
				$$.v.type = EXP_VAL_SYM;
				$$.v.sym = sliccmd_get_ref_sym($1.v.name, $3.v.name);
			}
		}
    |   NAME '[' expression ']'
		{
			if($3.v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
				if(sliccmd_array_has_int_value($1.v.name, $3.v.value)) {
					$$.v.type = EXP_VAL_INT;
					$$.v.value = sliccmd_array_lookup($1.v.name, $3.v.value);
				} else {
					$$.v.type = EXP_VAL_SYM;
					$$.v.sym = sliccmd_array_lookup_sym($1.v.name, $3.v.value);
				}
			}
		}
    |   NAME '[' expression ']' REF NAME
		{
			if($3.v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
				int val;
				$$.v.type = EXP_VAL_SYM;
				$$.v.sym = sliccmd_array_lookup_reference($1.v.name, $3.v.value, $6.v.name);
				if(($$.v.sym) && (sliccmd_sym_has_int_value($$.v.sym, &val))) {
					$$.v.type = EXP_VAL_INT;
					$$.v.value = val;
					$$.v.sym = NULL;
				}
			}
		}
    |   DBREF '(' expression ')'
        {
			if($3.v.type == EXP_VAL_INT) {
				$$.v.type = EXP_VAL_TEMP_SYM;
				$$.v.sym = sliccmd_get_db_name_sym_by_index($1.v.dbptr, $3.v.value);
			} else {
				$$.v.type = EXP_VAL_TEMP_SYM;
				$$.v.sym = sliccmd_get_db_name_sym($1.v.dbptr, $3.v.name);
			}
		}
    |   DBREF '(' expression ')' REF NAME
        {
			if($3.v.type == EXP_VAL_INT) {
				$$.v.type = EXP_VAL_INT;
				$$.v.value = slicif_find_db_value_by_index($1.v.dbptr, $3.v.value, $6.v.name);
				$$.v.sym = NULL;
			} else {

				$$.v.type = EXP_VAL_INT;
				$$.v.value = slicif_find_db_value($1.v.dbptr, $3.v.name, $6.v.name);
				$$.v.sym = NULL;
			}
		}
    |   DBREF '(' expression ')' REF NAME '[' expression ']'
        {
			if($3.v.type == EXP_VAL_INT) {
				$$.v.type = EXP_VAL_INT;
				$$.v.value = slicif_find_db_array_value_by_index($1.v.dbptr, $3.v.value, $6.v.name, $8.v.value);
				$$.v.sym = NULL;
			} else {

				$$.v.type = EXP_VAL_INT;
				$$.v.value = slicif_find_db_array_value($1.v.dbptr, $3.v.name, $6.v.name, $8.v.value);
				$$.v.sym = NULL;
			}
		}
	;

%%

int yyparse();

extern char *yysctext;

int
sliccmd_parse(int action, const char *cmd, char *output, int outputlen, int useDialogs, const char *catString)
{
	sliccmd_output = output;
	sliccmd_output_len = outputlen;
	sliccmd_cat_string = catString;

	sliccmd_input = cmd;
	sliccmd_input_ptr = sliccmd_input;
	sliccmd_input_len = strlen(sliccmd_input);
	sliccmd_done = 0;
	sliccmd_first_token = action;
	sliccmd_parse_failed = 0;

	sliccmd_use_dialogs = useDialogs;

	sliccmd_clear_symbols();

    do {
		yyparse();
    } while(!sliccmd_done);

	if(yysctext)
		yysctext[0] = 0;

	return sliccmd_parse_failed;
}

void yyerror(const char *s)
{
	sliccmd_error(s);
	sliccmd_parse_failed = 1;
	sliccmd_done = 1;
}
