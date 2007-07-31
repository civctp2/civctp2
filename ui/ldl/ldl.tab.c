#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.8 (Berkeley) 01/20/90";
#endif
#define YYBYACC 1
#line 2 "..\\ui\\ldl\\ldl.y"
#include "ldlyaccdefs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ldlif.h"

/* Avoid silly warnings */
#pragma warning( disable : 4013 )
#define lint

void yyerror(char* err);

int ldl_parser_done = 0;
int ldl_parse_error;

char *ldl_include_filename_stack[MAX_INCLUDE_DEPTH];
int ldl_include_stack_ptr = 0;

#line 23 "..\\ui\\ldl\\ldl.y"
typedef union {
	int intval;
	double floatval;
	char *nameval;
	char *stringval;
	void *names;
	void *block;
} YYSTYPE;
#line 35 "..\\ui\\ldl\\ldl.tab.c"
#define VINT 257
#define VNAME 258
#define VFLOAT 259
#define VSTRING 260
#define TINT 261
#define TDOUBLE 262
#define TBOOL 263
#define TSTRING 264
#define END_OF_INPUT 265
#define VTRUE 266
#define VFALSE 267
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    1,    1,    5,    2,    2,    3,    3,    4,    4,
    6,    6,    6,    6,    6,    7,    7,
};
short yylen[] = {                                         2,
    2,    2,    1,    0,    5,    3,    3,    1,    2,    1,
    1,    3,    3,    3,    3,    1,    1,
};
short yydefred[] = {                                      0,
    0,    0,    0,    3,    0,    0,    1,    2,    0,    7,
    6,    0,    0,    0,    0,    0,   11,    0,   10,    0,
    0,    0,    0,    5,    9,   13,   14,   16,   17,   12,
   15,
};
short yydgoto[] = {                                       2,
    3,   17,    5,   18,   12,   19,   30,
};
short yysindex[] = {                                   -251,
  -44,    0, -257,    0, -108, -251,    0,    0, -105,    0,
    0, -252, -237, -236, -235, -234,    0, -125,    0, -232,
 -233, -263, -231,    0,    0,    0,    0,    0,    0,    0,
    0,
};
short yyrindex[] = {                                      0,
  -96,    0,    0,    0,    0,    0,    0,    0, -245,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,
};
short yygindex[] = {                                      0,
    0,    2,   22,    0,    0,   12,    0,
};
#define YYTABLESIZE 139
short yytable[] = {                                      24,
    1,    4,   28,   29,    8,    1,    1,    7,   13,   14,
   15,   16,    4,    6,    9,    4,    4,    4,    4,   11,
   20,   21,   22,   23,   26,   27,    8,   10,   31,   25,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    1,    0,    0,   13,   14,   15,   16,
};
short yycheck[] = {                                     125,
  258,    0,  266,  267,    3,  258,  258,  265,  261,  262,
  263,  264,  258,   58,  123,  261,  262,  263,  264,  125,
  258,  258,  258,  258,  257,  259,  123,    6,  260,   18,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  258,   -1,   -1,  261,  262,  263,  264,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 267
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"':'",0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,"'{'",0,"'}'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"VINT","VNAME",
"VFLOAT","VSTRING","TINT","TDOUBLE","TBOOL","TSTRING","END_OF_INPUT","VTRUE",
"VFALSE",
};
char *yyrule[] = {
"$accept : ldl",
"ldl : blocks END_OF_INPUT",
"blocks : blocks block",
"blocks : block",
"$$1 :",
"block : blocknames '{' $$1 attributes '}'",
"block : blocknames '{' '}'",
"blocknames : VNAME ':' blocknames",
"blocknames : VNAME",
"attributes : attributes attribute",
"attributes : attribute",
"attribute : block",
"attribute : TBOOL VNAME bool",
"attribute : TINT VNAME VINT",
"attribute : TDOUBLE VNAME VFLOAT",
"attribute : TSTRING VNAME VSTRING",
"bool : VTRUE",
"bool : VFALSE",
};
#endif
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#ifdef YYSTACKSIZE
#ifndef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#endif
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH 500
#endif
#endif
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short yyss[YYSTACKSIZE];
YYSTYPE yyvs[YYSTACKSIZE];
#define yystacksize YYSTACKSIZE
#line 65 "..\\ui\\ldl\\ldl.y"

extern FILE *yyin;
int yyparse();

int ldlif_parse(char *filename)
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
#line 213 "..\\ui\\ldl\\ldl.tab.c"
#define YYABORT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse()
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register char *yys;
    extern char *getenv();

    if (yys = getenv("YYDEBUG"))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if (yyn = yydefred[yystate]) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("yydebug: state %d, reading %d (%s)\n", yystate,
                    yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("yydebug: state %d, shifting to state %d\n",
                    yystate, yytable[yyn]);
#endif
        if (yyssp >= yyss + yystacksize - 1)
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#ifdef lint
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#ifdef lint
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("yydebug: state %d, error recovery shifting\
 to state %d\n", *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yyss + yystacksize - 1)
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("yydebug: error recovery discarding state %d\n",
                            *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("yydebug: state %d, error recovery discards token %d (%s)\n",
                    yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("yydebug: state %d, reducing by rule %d (%s)\n",
                yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 1:
#line 36 "..\\ui\\ldl\\ldl.y"
{ ldl_parser_done = 1; }
break;
case 4:
#line 43 "..\\ui\\ldl\\ldl.y"
{ ldlif_start_block(yyvsp[-1].names); }
break;
case 5:
#line 43 "..\\ui\\ldl\\ldl.y"
{ yyval.block = ldlif_end_block(yyvsp[-4].names); }
break;
case 6:
#line 44 "..\\ui\\ldl\\ldl.y"
{ yyval.block = ldlif_add_empty_block(yyvsp[-2].names); }
break;
case 7:
#line 47 "..\\ui\\ldl\\ldl.y"
{ ldlif_add_name(&yyval.names, yyvsp[-2].nameval, yyvsp[0].names); }
break;
case 8:
#line 48 "..\\ui\\ldl\\ldl.y"
{ ldlif_add_name(&yyval.names, yyvsp[0].nameval, NULL); }
break;
case 12:
#line 56 "..\\ui\\ldl\\ldl.y"
{ ldlif_add_bool_attribute(yyvsp[-1].nameval, yyvsp[0].intval); }
break;
case 13:
#line 57 "..\\ui\\ldl\\ldl.y"
{ ldlif_add_int_attribute(yyvsp[-1].nameval, yyvsp[0].intval); }
break;
case 14:
#line 58 "..\\ui\\ldl\\ldl.y"
{ ldlif_add_float_attribute(yyvsp[-1].nameval, yyvsp[0].floatval); }
break;
case 15:
#line 59 "..\\ui\\ldl\\ldl.y"
{ ldlif_add_string_attribute(yyvsp[-1].nameval, yyvsp[0].stringval); }
break;
case 16:
#line 62 "..\\ui\\ldl\\ldl.y"
{ yyval.intval = 1; }
break;
case 17:
#line 62 "..\\ui\\ldl\\ldl.y"
{ yyval.intval = 0; }
break;
#line 401 "..\\ui\\ldl\\ldl.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("yydebug: after reduction, shifting from state 0 to\
 state %d\n", YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("yydebug: state %d, reading %d (%s)\n",
                        YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("yydebug: after reduction, shifting from state %d \
to state %d\n", *yyssp, yystate);
#endif
    if (yyssp >= yyss + yystacksize - 1)
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
