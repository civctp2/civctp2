%{
#include "ctp2_config.h"
#include "ctp2_inttypes.h"

#include "ctpdb.h"
#include "ydb.tab.h"
#include <stdlib.h>
#include <string.h>

extern int g_line_number;

%}

name		[a-zA-Z]+[a-zA-Z0-9_]*
string      \"[^\"]*\"

%%
int			  { return INT; }
float		  { return FLOAT; }
record		  { return RECORD; }
struct		  { return STRUCT; }
bit			  { return BIT; }
bits          { return BITS; }
exclusivebits { return EXCLUSIVEBITS; }
file          { return FILENAME; }
string        { return STRING; }
stringid      { return STRINGID; }
aka           { return AKA; }

{name}        {
			      yydblval.name = (char*)strdup(yytext);
				  return NAME;
			  }
{string}      {
				  /* strips quotes */
				  yydblval.name = (char*)strdup(yytext+1);
				  yydblval.name[strlen(yydblval.name) - 1] = 0;
				  return STRINGVALUE;
			  }
-?[0-9]+\.[0-9]+ {
				  yydblval.fval = atof(yytext);
				  return FLOATVALUE;
			  }
-?[0-9]+	  {
				  yydblval.val = atoi(yytext);
				  return NUMBER;
			  }
\.\.          { return RANGE; }					
#.*\n         { g_line_number++; }
\/\/.*\n	  { g_line_number++; }
[ \t]         {}
\n            { g_line_number++; }
\r            {}
.             { return yytext[0]; }

%%
int yywrap() { return 1; }
