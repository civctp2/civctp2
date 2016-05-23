%{
#include <stdlib.h>
#include <string.h>
#include "DBTokens.h"

int g_dblexerLineNumber;
%}

string 		\"[^\"\n]*[\"\n]
name		[a-zA-Z]+[a-zA-Z0-9_]*

%%
=           { return k_Token_Equals; }
\{          { return k_Token_OpenBrace; }
\}          { return k_Token_CloseBrace; }
>			{ return k_Token_Modified; }
,			{ return k_Token_ModifiedDelimiter; }
{name}      { return k_Token_Name; }
{string}    { return k_Token_String; }

-?[0-9]+\.[0-9]* { return k_Token_Float; }
-?[0-9]+      { return k_Token_Int; }

[ \t]     { }
\n        { g_dblexerLineNumber++; }
#.*\n       { g_dblexerLineNumber++; }
\/\/.*\n	{ g_dblexerLineNumber++; }
.           { return yytext[0]; }

%%
int yywrap() { return 1; }
