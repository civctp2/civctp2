%{
#include "ldlyaccdefs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ldlif.h"

/* Avoid silly warnings */
#ifdef _MSC_VER
#pragma warning( disable : 4013 )
#endif
#define lint

void yyerror(char* err);

int ldl_parser_done = 0;
int ldl_parse_error;

char *ldl_include_filename_stack[MAX_INCLUDE_DEPTH];
int ldl_include_stack_ptr = 0;

%}

%union {
	int intval;
	double floatval;
	char *nameval;
	char *stringval;
	void *names;
	void *block;
}

%token VINT VNAME VFLOAT VSTRING TINT TDOUBLE TBOOL TSTRING END_OF_INPUT
%token VTRUE VFALSE

%%
ldl: blocks END_OF_INPUT { ldl_parser_done = 1; }
     ;

blocks: blocks block
     | block
     ;

block: blocknames '{' { ldlif_start_block($1.names); } attributes '}' { $$.block = ldlif_end_block($1.names); }
     | blocknames '{' '}' { $$.block = ldlif_add_empty_block($1.names); }
       ;

blocknames: VNAME ':' blocknames { ldlif_add_name(&$$.names, $1.nameval, $3.names); }
          | VNAME { ldlif_add_name(&$$.names, $1.nameval, NULL); }
          ;

attributes: attributes attribute
            | attribute
            ;

attribute: block
           | TBOOL VNAME bool { ldlif_add_bool_attribute($2.nameval, $3.intval); }
           | TINT VNAME VINT { ldlif_add_int_attribute($2.nameval, $3.intval); }
           | TDOUBLE VNAME VFLOAT { ldlif_add_float_attribute($2.nameval, $3.floatval); }
           | TSTRING VNAME VSTRING { ldlif_add_string_attribute($2.nameval, $3.stringval); }
           ;

bool: VTRUE { $$.intval = 1; } | VFALSE { $$.intval = 0; } ;

%%

extern FILE *yyin;
int yyparse();

int ldl_open_first_file(const char *filename);

int ldlif_parse(const char *filename)
{
	ldlif_allocate_stuff();

	g_ldlLineNumber = 1;

	ldlif_init_log();

	if(!ldl_open_first_file(filename)) {
		return -1;
	}

	ldl_parse_error = LDL_ERROR_OK;

	do {
		yyparse();
	} while(ldl_parse_error == LDL_ERROR_OK && !ldl_parser_done);
	return ldl_parse_error;
}

const char *ldlif_get_filename()
{
	return ldl_include_filename_stack[ldl_include_stack_ptr];
}

extern void ldlif_report_error(char *text);
void yyerror(char *s)
{
	char buf[1024];
	sprintf(buf, "%s:%d: %s", ldlif_get_filename(), g_ldlLineNumber, s);
	ldlif_report_error(buf);
}
