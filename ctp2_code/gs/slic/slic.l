%{
//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Defines what valid slic commands are.
//                This file slic.l is compiled to lex.yy.c
//                and therefore you find this text in both files.
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Exponetiation operator '**' added.
//
//----------------------------------------------------------------------------
#include "ctp2_config.h"
#include "ctp2_inttypes.h"
#include "y.tab.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "slicif.h" 

int state;
int g_slicLineNumber = 1;

YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
extern char *include_filename_stack[MAX_INCLUDE_DEPTH];
int include_line_number_stack[MAX_INCLUDE_DEPTH];

extern int include_stack_ptr;
extern int g_slicWarnUndeclared;

%}
string 		\"[^\"\n]*[\"\n]
slicid      \'[^\'\n]*[\'\n]
stringid    ID_[a-zA-Z_0-9]+
name		[a-zA-Z]+[a-zA-Z0-9_]*

%x incl
%x fname

%%
^#include	{ BEGIN(incl); }
^#warnings   { g_slicWarnUndeclared = 1; }
<incl>[ \t]* {}
<incl>\"    { BEGIN(fname); }
<fname>[a-zA-Z0-9_.\\]+\" { /* Got the include file name */
  			if ( include_stack_ptr >= MAX_INCLUDE_DEPTH ) {
				slic_parse_error = SLIC_ERROR_INCLUDE;
				yyterminate();
            } else {

				char buf[MAX_PATH];
				char filename[MAX_PATH];
				strcpy(filename, yytext);
				filename[strlen(filename)-1] = 0; /* Strip trailing quote */
				if(!slicif_find_file(filename, buf)) {
					slic_parse_error = SLIC_ERROR_CANT_OPEN_FILE;
					yyterminate();
				}
				include_line_number_stack[include_stack_ptr] = g_slicLineNumber;
			    include_stack[include_stack_ptr++] =
						YY_CURRENT_BUFFER;
				include_filename_stack[include_stack_ptr] = malloc(strlen(buf)+1);
				strcpy(include_filename_stack[include_stack_ptr],
					   buf);
			    yyin = fopen( buf, "r" );
			  
			    if ( ! yyin ) {
				    slic_parse_error = SLIC_ERROR_CANT_OPEN_FILE;
					yyterminate();
				} else {
					yy_switch_to_buffer(
						yy_create_buffer( yyin, YY_BUF_SIZE ) );
					
					g_slicLineNumber = 1;
					BEGIN(INITIAL);	
				}
			}
		}
<fname>.		{
				    slic_parse_error = SLIC_ERROR_INCLUDE;
				    yyterminate();
				}
<<EOF>> {
			if ( --include_stack_ptr < 0 ) {
				if(include_stack_ptr == -1) {
					return END_OF_INPUT;
				}
				yyterminate();
			} else {
                fclose(yyin);
                yyin = 0;
				yy_delete_buffer( YY_CURRENT_BUFFER );
				free(include_filename_stack[include_stack_ptr + 1]);
				yy_switch_to_buffer(
					include_stack[include_stack_ptr] );
				g_slicLineNumber = include_line_number_stack[include_stack_ptr];
			}
        }

						
MessageBox	{ return KW_MESSAGEBOX;}
AlertBox    { return KW_ALERTBOX; }
HelpBox     { return KW_HELPBOX; }
Trigger		{ return KW_TRIGGER; }
HandleEvent { return KW_HANDLEEVENT; }
Function    { return KW_FUNCTION; }
When		{ return KW_WHEN; }
If          { return KW_IF; }
Else        { return KW_ELSE; }
ElseIf      { return KW_ELSEIF; }
While       { return KW_WHILE; }
For         { return KW_FOR; }
Button      { return KW_BUTTON; }
Region		{ return KW_REGION; }
On          { return KW_ON; }
OnClose     { return KW_ONCLOSE; }
Pre         { return KW_PRE; }
Post        { return KW_POST; }
Event       { return KW_EVENT; }

Unit_t      { return KW_UNIT; }
City_t      { return KW_CITY; }
Location_t  { return KW_LOCATION; }
Int_t       { return KW_INT; }
Void_t      { return KW_VOID; }
Army_t      { return KW_ARMY; }

Int_f       { return KW_INT_FUNC; }
Void_f      { return KW_VOID_FUNC; }

Const       { return KW_CONST; }

Return      { return KW_RETURN; }

{stringid}  { yylval.val = slicif_find_string(yytext); return STRINGID; }
{name}		{ 
			    int value;
				void *dbptr;
				if(slicif_find_const(yytext, &value)) {
				    yylval.val = value;
					return NUMBER;
				} else if(slicif_find_db(yytext, &dbptr)) {
					yylval.dbptr = dbptr;
					return DBREF;
				} else {
				    yylval.name = (char*)strdup(yytext); 
				    return NAME;
				}
			}
[0-9]+		{ yylval.val = atoi(yytext); return NUMBER; }
{slicid}	{ 
				yylval.name = malloc(strlen(yytext) - 1);
				strncpy(yylval.name, yytext + 1, strlen(yytext) - 2);
				yylval.name[strlen(yytext) - 2] = 0;
				return IDENTIFIER; 
			}
{string}    { 
                yylval.name = malloc(strlen(yytext) - 1);
				strncpy(yylval.name, yytext + 1, strlen(yytext) - 2);
				yylval.name[strlen(yytext) - 2] = 0;
				return STRING;
			}

\/\/.*\n	{ 
              g_slicLineNumber++; 
              slicif_register_line(g_slicLineNumber, -1);
            }
\{ 			{ /*slicif_start_block();*/ return yytext[0]; }
\} 			{ /*slicif_end_block();*/ return yytext[0]; }

\.       { return REF; }
[ \t]		{}
==		{ return EQ; }
\<		{ return LT; }
\>		{ return GT; }
\<=		{ return LTE; }
\>=		{ return GTE; }
!=      { return NEQ; }
&&      { return AND; }
\*\*    { 
		return EXP;
		}
\|\|    { return OR; }
\n		{ 
            g_slicLineNumber++;
            slicif_register_line(g_slicLineNumber, -1);
        }
.		{ return yytext[0]; }

%%
int yywrap() { return 1; }

int slicif_open_first_file(char *file)
{
	char buf[MAX_PATH];

	include_stack_ptr = 0;

	if(!slicif_find_file(file, buf))
		return 0;

	include_filename_stack[0] = malloc(strlen(buf) + 1);
	strcpy(include_filename_stack[0], buf);

	yyin = fopen(buf, "r");
	if(!yyin) {
		return 0;
	}

	return 1;
}

const char *slicif_current_file()
{
	return include_filename_stack[include_stack_ptr];
}
