%{
#include <crtdbg.h>
#include <windows.h>
#include "c3debug.h"
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

#include "y.tab.h"
#define PARSER_INTERFACE
#include "fliif.h"


int fli_lineNumber;

static const char *s_topdir;

#define MAX_INCLUDE_DEPTH 64
YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
char *include_filename_stack[MAX_INCLUDE_DEPTH];
int include_line_number_stack[MAX_INCLUDE_DEPTH];

int include_stack_ptr = 0;

%}

/* the "incl" state is used for picking up the name
 * of an include file
 */
%x incl
%x fname

number	-?(([0-9]+)|([0-9]*\.[0-9]+))
name	[a-zA-Z_][a-zA-Z_0-9]*
string  \"[^\"\n]*[\"\n]

%%
#include            {FLPRINTF(("Got #include\n"));
						BEGIN(incl);}

<incl>[ \t]*      /* eat the whitespace */
<incl>\"			{ FLPRINTF(("Begin fname\n")); BEGIN(fname);}
<fname>[a-zA-Z0-9_.\\]+\" { /* got the include file name */
			FLPRINTF(("fname: %s\n", yytext));
  			if ( include_stack_ptr >= MAX_INCLUDE_DEPTH ) {
				FLPRINTF(("Includes nested too deeply\n"));
				fli_parse_error = FLI_LEX_INCLUDE_ERROR;
				yyterminate();
            } else {

				char buf[MAX_PATH];
				sprintf(buf, "%s\\%s", s_topdir, yytext);
				buf[strlen(buf)-1] = 0; /* Strip trailing quote */

				include_line_number_stack[include_stack_ptr] = fli_lineNumber;
			    include_stack[include_stack_ptr++] =
						YY_CURRENT_BUFFER;
				include_filename_stack[include_stack_ptr] = malloc(strlen(buf)+1);
				strcpy(include_filename_stack[include_stack_ptr],
					   buf);
			    yyin = fopen( buf, "r" );
			  
			    if ( ! yyin ) {
				    fli_parse_error = FLI_CANT_OPEN_FILE;
					yyterminate();
				} else {
				
						yy_switch_to_buffer(
								yy_create_buffer( yyin, YY_BUF_SIZE ) );
			  
						fli_lineNumber = 1;
						BEGIN(INITIAL);	
				}
			}
		}
<fname>.		{
				    fli_parse_error = FLI_LEX_INCLUDE_ERROR;
				    yyterminate();
				}
<<EOF>> {
	 	
			if ( --include_stack_ptr < 0 ) {
				if(include_stack_ptr == -1) {
					return END_OF_INPUT;
				}
				FLPRINTF(("Doing yyterminate\n"));
				yyterminate();
			} else {
				fclose(yyin);
				yyin = 0; 
				yy_delete_buffer( YY_CURRENT_BUFFER );
				free(include_filename_stack[include_stack_ptr + 1]);
				yy_switch_to_buffer(
					include_stack[include_stack_ptr] );
				fli_lineNumber = include_line_number_stack[include_stack_ptr];

			}
        }

{string}    { 
				yylval.name = malloc(strlen(yytext) - 1);
				strncpy(yylval.name, yytext + 1, strlen(yytext + 1) - 1);
				yylval.name[strlen(yytext + 1) - 1] = 0;
				return STRING;
			}
elseif		{ return ELSEIF; }
else		{ return ELSE; }
if			{ return IF; }
input		{ return INPUT; }
output		{ return OUTPUT; }
not			{ return NOT; }
and			{ return AND; }
or			{ return OR; }
left		{ return LEFT; }
right		{ return RIGHT; }
tri			{ return TRI; }
spike		{ return SPIKE; } 
fuzzy_eval 	{ return FUZZY_EVAL; }
fuzzy_init  { return FUZZY_INIT; } 
fuzzy_beginturn { return FUZZY_BEGINTURN; }
fuzzy_endturn  { return FUZZY_ENDTURN; }
fuzzy_begin_diplomacy 	{ return FUZZY_DIP; }
fuzzy_pre_incoming_diplomacy { return FUZZY_PRE_INC_DIP; }
fuzzy_post_incoming_diplomacy { return FUZZY_POST_INC_DIP; }
fuzzy_pre_outgoing_diplomacy { return FUZZY_PRE_OUT_DIP; }
fuzzy_post_outgoing_diplomacy { return FUZZY_POST_OUT_DIP; }
fuzzy_rejected_message { return FUZZY_REJECTED_MESSAGE; }
fuzzy_accepted_message { return FUZZY_ACCEPTED_MESSAGE; }
very	    { return VERY; }
kinda		{ return KINDA; }

action      { return ACTION; }
when        { return WHEN; }

{number} 	{ yylval.value = atof(yytext); return NUMBER; }
{name}		{ yylval.name = (char *)strdup(yytext); return NAME; }
\/\/.*\n	{ fli_lineNumber++; }
\n			{ fli_lineNumber++; }
[ \t]		{ }
;			{ /* semicolons are silently ignored for the benefit of
				 C programmers. */ }
.			{ return yytext[0]; }
%%

int yywrap() 
{ 
	return 1;
}

int fli_open_first_file(const char *topdir, const char *file)
{
	char buf[MAX_PATH];

	include_stack_ptr=0; 
	sprintf(buf, "%s\\%s", topdir, file);
	s_topdir = topdir;
	include_filename_stack[0] = malloc(strlen(buf) + 1);
	strcpy(include_filename_stack[0], buf);

	yyin = fopen(buf, "r");
	if(!yyin) {
		return 0;
	}

	return 1;
}

const char *fli_current_file()
{
    char *tmp=NULL; 
	char *str=NULL; 

	// get rid of the full path	
//	Assert(0 <= include_stack_ptr);

	str = include_filename_stack[include_stack_ptr];

	if (str) { 
	    for (tmp = str; *tmp; tmp++) { 
			if (*tmp == '\\') { 
				str = tmp+1; 
			} 
		}
	} 
	return str; 
}

