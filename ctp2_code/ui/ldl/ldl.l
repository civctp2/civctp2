%{
#include "ldlyaccdefs.h"
#include "ldl.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef WIN32
#include <windows.h>
#else
#include <sys/param.h>
#include <sys/types.h>
#include "windows.h"
#include "cifm.h"
#define fopen(a, b) ci_fopen(a, b)
#endif
#include "ldlif.h"

int state;
int g_ldlLineNumber = 1;

YY_BUFFER_STATE ldl_include_stack[MAX_INCLUDE_DEPTH];
extern char *ldl_include_filename_stack[MAX_INCLUDE_DEPTH];
int ldl_include_line_number_stack[MAX_INCLUDE_DEPTH];

extern int ldl_include_stack_ptr;
extern int g_ldlWarnUndeclared;
extern int ldl_parse_error;

%}

string		\"[^\"\n]*[\"\r\n]
name		[a-zA-Z]+[a-zA-Z0-9_]*

%x incl
%x fname
%x inclL
%x fnameL

%%

^import	{ BEGIN(incl); }
<incl>[ \t\r]* {}
<incl>\"    { BEGIN(fname); }
<fname>[a-zA-Z0-9_.\\]+\" { /* Got the include file name */
			if ( ldl_include_stack_ptr >= MAX_INCLUDE_DEPTH ) {
				ldl_parse_error = LDL_ERROR_INCLUDE;
				yyterminate();
			} else {

				char buf[MAX_PATH];
				char filename[MAX_PATH];
				strcpy(filename, yytext);
				filename[strlen(filename)-1] = 0; /* Strip trailing quote */
				if(!ldlif_find_file(filename, buf)) {
					ldl_parse_error = LDL_ERROR_CANT_OPEN_FILE;
					yyterminate();
				}
				ldl_include_line_number_stack[ldl_include_stack_ptr] = g_ldlLineNumber;
				ldl_include_stack[ldl_include_stack_ptr++] =
						YY_CURRENT_BUFFER;
				ldl_include_filename_stack[ldl_include_stack_ptr] = malloc(strlen(buf)+1);
				strcpy(ldl_include_filename_stack[ldl_include_stack_ptr],
				       buf);
				yyin = fopen( buf, "r" );

				if ( ! yyin ) {
					ldl_parse_error = LDL_ERROR_CANT_OPEN_FILE;
					yyterminate();
				} else {
					yy_switch_to_buffer(
						yy_create_buffer( yyin, YY_BUF_SIZE ) );

					g_ldlLineNumber = 1;
					BEGIN(INITIAL);
				}
			}
		}
<fname>.		{
					ldl_parse_error = LDL_ERROR_INCLUDE;
					yyterminate();
				}

^importLoc	{ BEGIN(inclL); }
<inclL>[ \t\r]* {}
<inclL>\"    { BEGIN(fnameL); }
<fnameL>[a-zA-Z0-9_.\\]+\" { /* Got the include file name */
			if ( ldl_include_stack_ptr >= MAX_INCLUDE_DEPTH ) {
				ldl_parse_error = LDL_ERROR_INCLUDE;
				yyterminate();
			} else {

				char buf[MAX_PATH];
				char filename[MAX_PATH];
				strcpy(filename, yytext);
				filename[strlen(filename)-1] = 0; /* Strip trailing quote */
				if(!ldlif_find_localized_file(filename, buf)) {
					ldl_parse_error = LDL_ERROR_CANT_OPEN_FILE;
					yyterminate();
				}
				ldl_include_line_number_stack[ldl_include_stack_ptr] = g_ldlLineNumber;
				ldl_include_stack[ldl_include_stack_ptr++] =
						YY_CURRENT_BUFFER;
				ldl_include_filename_stack[ldl_include_stack_ptr] = malloc(strlen(buf)+1);
				strcpy(ldl_include_filename_stack[ldl_include_stack_ptr],
				       buf);
				yyin = fopen( buf, "r" );

				if ( ! yyin ) {
					ldl_parse_error = LDL_ERROR_CANT_OPEN_FILE;
					yyterminate();
				} else {
					yy_switch_to_buffer(
						yy_create_buffer( yyin, YY_BUF_SIZE ) );

					g_ldlLineNumber = 1;
					BEGIN(INITIAL);
				}
			}
		}
<fnameL>.		{
					ldl_parse_error = LDL_ERROR_INCLUDE;
					yyterminate();
				}

<<EOF>> {
			if ( --ldl_include_stack_ptr < 0 ) {
				if(ldl_include_stack_ptr == -1) {
					return END_OF_INPUT;
				}
				yyterminate();
			} else {
				fclose(yyin);
				yyin = 0;
				yy_delete_buffer( YY_CURRENT_BUFFER );
				free(ldl_include_filename_stack[ldl_include_stack_ptr + 1]);
				yy_switch_to_buffer(
					ldl_include_stack[ldl_include_stack_ptr] );
				g_ldlLineNumber = ldl_include_line_number_stack[ldl_include_stack_ptr];
			}
		}

template		{ }
int				{
					return TINT;
				}
double			{
					return TDOUBLE;
				}
bool			{
					return TBOOL;
				}
string			{
					return TSTRING;
				}
true			{ return VTRUE; }
false			{ return VFALSE; }
{name}			{
					yylval.nameval = ldlif_getnameptr(yytext);
					return VNAME;
				}
-?[0-9]+		{
					yylval.intval = atoi(yytext);
					return VINT;
				}
-?[0-9]*\.[0-9]+ {
					yylval.floatval = atof(yytext);
					return VFLOAT;
				}
{string}		{
					yytext[strlen(yytext) - 1] = 0;
					yylval.stringval = ldlif_getstringptr(&yytext[1]);
					return VSTRING;
				}
#.*\n			{ g_ldlLineNumber++; }
[ \t\r]			{ }
\n				{ g_ldlLineNumber++; }
\$				{ }
.				{ return yytext[0]; }

%%
int yywrap() { return 1; }

int ldl_open_first_file(char *file)
{
	char buf[MAX_PATH];

	ldl_include_stack_ptr = 0;

	if(!ldlif_find_file(file, buf))
		return 0;

	ldl_include_filename_stack[0] = malloc(strlen(buf) + 1);
	strcpy(ldl_include_filename_stack[0], buf);

	yyin = fopen(buf, "r");
	if(!yyin) {
		return 0;
	}

	return 1;
}

const char *ldlif_current_file()
{
	return ldl_include_filename_stack[ldl_include_stack_ptr];
}
