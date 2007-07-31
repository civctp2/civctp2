#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.8 (Berkeley) 01/20/90";
#endif
#define YYBYACC 1
#line 2 "..\\gs\\slic\\sliccmd.y"
/* Change all yy* symbols to sc* for this parser */
#include "scyaccdefs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sliccmd.h"

/* Avoid silly warnings */
#pragma warning( disable : 4013 )
#define lint

void yyerror(char* err);

char *sliccmd_output;
int sliccmd_output_len;
char *sliccmd_input;
char *sliccmd_input_ptr;
int sliccmd_input_len;
int sliccmd_first_token;
int sliccmd_done;
int sliccmd_parse_failed;
int sliccmd_use_dialogs;
char *sliccmd_cat_string;

#line 29 "..\\gs\\slic\\sliccmd.y"
typedef union {
	struct sliccmdExpValue v;
} YYSTYPE;
#line 36 "..\\gs\\slic\\sc.tab.c"
#define NAME 257
#define DBREF 258
#define STRINGID 259
#define IDENTIFIER 260
#define NUMBER 261
#define GT 262
#define LT 263
#define GTE 264
#define LTE 265
#define EQ 266
#define REF 267
#define AND 268
#define OR 269
#define STRING 270
#define SLICCMD_WATCH 271
#define SLICCMD_ASSN 272
#define SLICCMD_EVAL 273
#define SLICCMD_REPLACE 274
#define NEQ 275
#define UMINUS 276
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    0,    0,    0,    2,    2,    2,    2,    3,    3,
    3,    4,    4,    4,    4,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,
};
short yylen[] = {                                         2,
    2,    2,    4,    2,    1,    3,    4,    6,    1,    3,
    0,    1,    1,    1,    1,    3,    3,    3,    3,    3,
    3,    3,    3,    3,    3,    3,    3,    2,    2,    3,
    1,    1,    4,    3,    4,    6,    4,    6,
};
short yydefred[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,   31,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   29,   28,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   34,    0,
   14,   13,   15,    0,    0,    0,    0,   30,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   18,   19,
    6,    0,    0,    0,   33,    0,    0,    0,    0,   10,
    0,    0,   36,   38,    8,
};
short yydgoto[] = {                                       5,
   44,   14,   45,   46,
};
short yysindex[] = {                                   -136,
  127, -254,  127,  127,    0,  -36,  -27,    0,  127,  127,
  127,  106,  -89,  -47,  106,  106, -242,  127,  -33,  127,
    0,    0,  -23,  127,  127,  127,  127,  127,  127,  127,
  127,  127,  127,  127,  127, -236,  127,  127,    0,   -7,
    0,    0,    0,  106,  -18,   -5,  -15,    0,  -37,  -37,
  -37,  -37,  -37,  114,  114,  -37,  -31,  -31,    0,    0,
    0,   98,  106, -238,    0,  -33, -230, -220, -194,    0,
 -190, -186,    0,    0,    0,
};
short yyrindex[] = {                                      0,
    0,    0,    0,    0,    0,    1,    0,    0,    0,    0,
    0,   72,   14,    0,   79,   80,    0,    0,   43,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  -10,    0,   46,    0,    0,   41,   49,
   57,   65,   73,   83,   89,   81,   25,   33,    0,    0,
    0,    0,   88,    9,    0,   43,   17,   30,    0,    0,
    0,    0,    0,    0,    0,
};
short yygindex[] = {                                      0,
  176,    0,   26,    0,
};
#define YYTABLESIZE 389
short yytable[] = {                                      10,
   32,   37,   13,   19,   34,   33,   11,   32,   35,   35,
   34,    9,   20,   38,   39,   35,   37,   48,   34,   33,
   61,   32,   65,   35,   17,   67,   34,   33,   69,   32,
   12,   35,   16,   12,   34,   33,   71,   32,   66,   35,
   21,   32,   32,   32,   32,   32,   72,   32,   20,   35,
   35,   35,   35,   35,   18,   35,   22,   37,   37,   37,
   37,   37,   73,   37,   23,   17,   74,   17,   17,   17,
   75,    1,   24,   16,    5,   16,   16,   16,    2,    4,
   25,   21,   26,   11,   21,   64,    9,    3,   27,   20,
    7,   70,   20,   32,    0,    0,    0,   22,    0,    0,
   22,   35,    0,    0,    0,   23,    0,    0,   23,   37,
    0,    0,    0,   24,    0,    0,   24,   17,    0,    0,
    0,   25,    0,   26,   25,   16,   26,    0,    0,   27,
    0,    0,   27,   21,    1,    2,    3,    4,    0,   34,
   33,   20,   32,    0,   35,    0,    0,   34,   33,   22,
   32,    0,   35,    0,    0,   34,   33,   23,   32,   10,
   35,    0,    0,    0,    0,   24,   11,    0,    0,    0,
    0,    9,    0,   25,    0,   26,   12,   36,   15,   16,
    0,   27,    0,    0,   21,   22,   23,    0,    0,    0,
   68,    0,    0,   40,    0,   47,    0,    0,    0,   49,
   50,   51,   52,   53,   54,   55,   56,   57,   58,   59,
   60,    0,   62,   63,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    6,    7,   41,   42,    8,    0,    0,
   17,    0,    0,    0,    0,    0,   43,    0,   24,   25,
   26,   27,   28,    0,   29,   30,   24,   25,   26,   27,
   28,   31,   29,   30,   24,   25,   26,   27,   28,   31,
   29,   30,   32,   32,   32,   32,   32,   31,   32,   32,
   35,   35,   35,   35,   35,   32,   35,   35,   37,   37,
   37,   37,   37,   35,   37,   37,   17,   17,   17,   17,
   17,   37,   17,   17,   16,   16,   16,   16,   16,   17,
   16,   16,   21,   21,   21,   21,   21,   16,   21,   21,
   20,   20,   20,   20,   20,   21,   20,   20,   22,   22,
   22,   22,   22,   20,   22,   22,   23,   23,   23,   23,
   23,   22,   23,   23,   24,   24,   24,   24,   24,   23,
   24,   24,   25,   25,   25,   25,   25,   24,   25,   25,
   26,   26,    0,    0,    0,   25,   27,   27,    0,   24,
   25,   26,   27,   28,    0,   29,   30,   24,   25,   26,
   27,   28,   31,   29,   30,   24,   25,   26,   27,   28,
   31,    0,    0,    6,    7,    0,    0,    8,   31,
};
short yycheck[] = {                                      33,
    0,   91,  257,   40,   42,   43,   40,   45,    0,   47,
   42,   45,   40,   61,  257,   47,    0,   41,   42,   43,
  257,   45,   41,   47,    0,   41,   42,   43,  267,   45,
   41,   47,    0,   44,   42,   43,  267,   45,   44,   47,
    0,   41,   42,   43,   44,   45,  267,   47,    0,   41,
   42,   43,   44,   45,   91,   47,    0,   41,   42,   43,
   44,   45,  257,   47,    0,   41,  257,   43,   44,   45,
  257,    0,    0,   41,   61,   43,   44,   45,    0,    0,
    0,   41,    0,   41,   44,   93,   41,    0,    0,   41,
   61,   66,   44,   93,   -1,   -1,   -1,   41,   -1,   -1,
   44,   93,   -1,   -1,   -1,   41,   -1,   -1,   44,   93,
   -1,   -1,   -1,   41,   -1,   -1,   44,   93,   -1,   -1,
   -1,   41,   -1,   41,   44,   93,   44,   -1,   -1,   41,
   -1,   -1,   44,   93,  271,  272,  273,  274,   -1,   42,
   43,   93,   45,   -1,   47,   -1,   -1,   42,   43,   93,
   45,   -1,   47,   -1,   -1,   42,   43,   93,   45,   33,
   47,   -1,   -1,   -1,   -1,   93,   40,   -1,   -1,   -1,
   -1,   45,   -1,   93,   -1,   93,    1,  267,    3,    4,
   -1,   93,   -1,   -1,    9,   10,   11,   -1,   -1,   -1,
   93,   -1,   -1,   18,   -1,   20,   -1,   -1,   -1,   24,
   25,   26,   27,   28,   29,   30,   31,   32,   33,   34,
   35,   -1,   37,   38,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  257,  258,  259,  260,  261,   -1,   -1,
  267,   -1,   -1,   -1,   -1,   -1,  270,   -1,  262,  263,
  264,  265,  266,   -1,  268,  269,  262,  263,  264,  265,
  266,  275,  268,  269,  262,  263,  264,  265,  266,  275,
  268,  269,  262,  263,  264,  265,  266,  275,  268,  269,
  262,  263,  264,  265,  266,  275,  268,  269,  262,  263,
  264,  265,  266,  275,  268,  269,  262,  263,  264,  265,
  266,  275,  268,  269,  262,  263,  264,  265,  266,  275,
  268,  269,  262,  263,  264,  265,  266,  275,  268,  269,
  262,  263,  264,  265,  266,  275,  268,  269,  262,  263,
  264,  265,  266,  275,  268,  269,  262,  263,  264,  265,
  266,  275,  268,  269,  262,  263,  264,  265,  266,  275,
  268,  269,  262,  263,  264,  265,  266,  275,  268,  269,
  268,  269,   -1,   -1,   -1,  275,  268,  269,   -1,  262,
  263,  264,  265,  266,   -1,  268,  269,  262,  263,  264,
  265,  266,  275,  268,  269,  262,  263,  264,  265,  266,
  275,   -1,   -1,  257,  258,   -1,   -1,  261,  275,
};
#define YYFINAL 5
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 276
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"'!'",0,0,0,0,0,0,"'('","')'","'*'","'+'","','","'-'",0,"'/'",0,0,0,0,0,0,0,0,0,
0,0,0,0,"'='",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'['",0,
"']'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"NAME","DBREF","STRINGID","IDENTIFIER","NUMBER","GT","LT","GTE",
"LTE","EQ","REF","AND","OR","STRING","SLICCMD_WATCH","SLICCMD_ASSN",
"SLICCMD_EVAL","SLICCMD_REPLACE","NEQ","UMINUS",
};
char *yyrule[] = {
"$accept : command",
"command : SLICCMD_WATCH expression",
"command : SLICCMD_EVAL expression",
"command : SLICCMD_ASSN lvalue '=' expression",
"command : SLICCMD_REPLACE expression",
"lvalue : NAME",
"lvalue : NAME REF NAME",
"lvalue : NAME '[' expression ']'",
"lvalue : NAME '[' expression ']' REF NAME",
"arguments : argument",
"arguments : argument ',' arguments",
"arguments :",
"argument : expression",
"argument : IDENTIFIER",
"argument : STRINGID",
"argument : STRING",
"expression : expression '+' expression",
"expression : expression '-' expression",
"expression : expression '*' expression",
"expression : expression '/' expression",
"expression : expression LT expression",
"expression : expression GT expression",
"expression : expression GTE expression",
"expression : expression LTE expression",
"expression : expression EQ expression",
"expression : expression NEQ expression",
"expression : expression AND expression",
"expression : expression OR expression",
"expression : '!' expression",
"expression : '-' expression",
"expression : '(' expression ')'",
"expression : NUMBER",
"expression : NAME",
"expression : NAME '(' arguments ')'",
"expression : NAME REF NAME",
"expression : NAME '[' expression ']'",
"expression : NAME '[' expression ']' REF NAME",
"expression : DBREF '(' expression ')'",
"expression : DBREF '(' expression ')' REF NAME",
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
#line 340 "..\\gs\\slic\\sliccmd.y"

int yyparse();

extern char *sctext;

int sliccmd_parse(int action, char *cmd, char *output, int outputlen, int useDialogs, char *catString)
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

	if(sctext)
		sctext[0] = 0;

	return sliccmd_parse_failed;
}

void yyerror(char *s)
{
	sliccmd_error(s);
	sliccmd_parse_failed = 1;
	sliccmd_done = 1;
}

#line 315 "..\\gs\\slic\\sc.tab.c"
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
#line 48 "..\\gs\\slic\\sliccmd.y"
{ 			 
			 sliccmd_parse_done(&yyvsp[0].v, SLICCMD_WATCH);
			 sliccmd_done = 1;
		 }
break;
case 2:
#line 53 "..\\gs\\slic\\sliccmd.y"
{
			 sliccmd_parse_done(&yyvsp[0].v, SLICCMD_EVAL);
			 sliccmd_done = 1;
		 }
break;
case 3:
#line 58 "..\\gs\\slic\\sliccmd.y"
{
			 sliccmd_assign(yyvsp[-2].v.sym, &yyvsp[0].v);
			 sliccmd_done = 1;
		 }
break;
case 4:
#line 63 "..\\gs\\slic\\sliccmd.y"
{
			 sliccmd_parse_done(&yyvsp[0].v, SLICCMD_REPLACE);
			 sliccmd_done = 1;
		 }
break;
case 5:
#line 70 "..\\gs\\slic\\sliccmd.y"
{
			yyval.v.type = EXP_VAL_SYM;
			yyval.v.sym = sliccmd_lookup_sym(yyvsp[0].v.name);
		}
break;
case 6:
#line 75 "..\\gs\\slic\\sliccmd.y"
{
			yyval.v.type = EXP_VAL_SYM;
			yyval.v.sym = sliccmd_get_ref_sym(yyvsp[-2].v.name, yyvsp[0].v.name);
		}
break;
case 7:
#line 80 "..\\gs\\slic\\sliccmd.y"
{
			yyval.v.type = EXP_VAL_SYM;
			yyval.v.sym = sliccmd_array_lookup_sym(yyvsp[-3].v.name, yyvsp[-1].v.value);
		}
break;
case 8:
#line 85 "..\\gs\\slic\\sliccmd.y"
{
			if(yyvsp[-3].v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			    yyval.v.type = EXP_VAL_SYM;
			    yyval.v.sym = sliccmd_array_lookup_reference(yyvsp[-5].v.name, yyvsp[-3].v.value, yyvsp[0].v.name);
			}
		}
break;
case 12:
#line 100 "..\\gs\\slic\\sliccmd.y"
{ sliccmd_arg_exp(yyvsp[0].v.value); }
break;
case 13:
#line 101 "..\\gs\\slic\\sliccmd.y"
{ sliccmd_arg_id(yyvsp[0].v.name); }
break;
case 14:
#line 102 "..\\gs\\slic\\sliccmd.y"
{ sliccmd_arg_stringid(yyvsp[0].v.value); }
break;
case 15:
#line 103 "..\\gs\\slic\\sliccmd.y"
{ sliccmd_arg_string(yyvsp[0].v.name); }
break;
case 16:
#line 107 "..\\gs\\slic\\sliccmd.y"
{ 
		if(yyvsp[-2].v.type != EXP_VAL_INT ||
		   yyvsp[0].v.type != EXP_VAL_INT) {
			sliccmd_type_error();
		} else {
			yyval.v.type = EXP_VAL_INT;
			yyval.v.value = yyvsp[-2].v.value + yyvsp[0].v.value; 
		}
	}
break;
case 17:
#line 117 "..\\gs\\slic\\sliccmd.y"
{
			if(yyvsp[-2].v.type != EXP_VAL_INT ||
			   yyvsp[0].v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			yyval.v.type = EXP_VAL_INT;
				yyval.v.value = yyvsp[-2].v.value - yyvsp[0].v.value; 
			}
		}
break;
case 18:
#line 127 "..\\gs\\slic\\sliccmd.y"
{
			if(yyvsp[-2].v.type != EXP_VAL_INT ||
			   yyvsp[0].v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			yyval.v.type = EXP_VAL_INT;
				yyval.v.value = yyvsp[-2].v.value * yyvsp[0].v.value; 
			}
		}
break;
case 19:
#line 137 "..\\gs\\slic\\sliccmd.y"
{
			if(yyvsp[-2].v.type != EXP_VAL_INT ||
			   yyvsp[0].v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			yyval.v.type = EXP_VAL_INT;
				yyval.v.value = yyvsp[-2].v.value / yyvsp[0].v.value; 
			}
		}
break;
case 20:
#line 147 "..\\gs\\slic\\sliccmd.y"
{
			if(yyvsp[-2].v.type != EXP_VAL_INT ||
			   yyvsp[0].v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			yyval.v.type = EXP_VAL_INT;
				yyval.v.value = yyvsp[-2].v.value < yyvsp[0].v.value; 
			}
		}
break;
case 21:
#line 157 "..\\gs\\slic\\sliccmd.y"
{
			if(yyvsp[-2].v.type != EXP_VAL_INT ||
			   yyvsp[0].v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			yyval.v.type = EXP_VAL_INT;
				yyval.v.value = yyvsp[-2].v.value > yyvsp[0].v.value; 
			}
		}
break;
case 22:
#line 167 "..\\gs\\slic\\sliccmd.y"
{
			if(yyvsp[-2].v.type != EXP_VAL_INT ||
			   yyvsp[0].v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			yyval.v.type = EXP_VAL_INT;
				yyval.v.value = yyvsp[-2].v.value >= yyvsp[0].v.value; 
			}
		}
break;
case 23:
#line 177 "..\\gs\\slic\\sliccmd.y"
{
			if(yyvsp[-2].v.type != EXP_VAL_INT ||
			   yyvsp[0].v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			yyval.v.type = EXP_VAL_INT;
				yyval.v.value = yyvsp[-2].v.value <= yyvsp[0].v.value; 
			}
		}
break;
case 24:
#line 187 "..\\gs\\slic\\sliccmd.y"
{
			if(yyvsp[-2].v.type != EXP_VAL_INT ||
			   yyvsp[0].v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			yyval.v.type = EXP_VAL_INT;
				yyval.v.value = yyvsp[-2].v.value == yyvsp[0].v.value; 
			}
		}
break;
case 25:
#line 197 "..\\gs\\slic\\sliccmd.y"
{
			if(yyvsp[-2].v.type != EXP_VAL_INT ||
			   yyvsp[0].v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			yyval.v.type = EXP_VAL_INT;
				yyval.v.value = yyvsp[-2].v.value != yyvsp[0].v.value; 
			}
		}
break;
case 26:
#line 207 "..\\gs\\slic\\sliccmd.y"
{
			if(yyvsp[-2].v.type != EXP_VAL_INT ||
			   yyvsp[0].v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			yyval.v.type = EXP_VAL_INT;
				yyval.v.value = yyvsp[-2].v.value && yyvsp[0].v.value; 
			}
		}
break;
case 27:
#line 217 "..\\gs\\slic\\sliccmd.y"
{
			if(yyvsp[-2].v.type != EXP_VAL_INT ||
			   yyvsp[0].v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			yyval.v.type = EXP_VAL_INT;
				yyval.v.value = yyvsp[-2].v.value || yyvsp[0].v.value; 
			}
		}
break;
case 28:
#line 227 "..\\gs\\slic\\sliccmd.y"
{
			if(yyvsp[0].v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			yyval.v.type = EXP_VAL_INT;
				yyval.v.value = !(yyvsp[0].v.value);
			}
		}
break;
case 29:
#line 236 "..\\gs\\slic\\sliccmd.y"
{
			if(yyvsp[0].v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			yyval.v.type = EXP_VAL_INT;
				yyval.v.value = -(yyvsp[0].v.value);
			}
		}
break;
case 30:
#line 245 "..\\gs\\slic\\sliccmd.y"
{
			if(yyvsp[-1].v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
			yyval.v.type = EXP_VAL_INT;
				yyval.v.value = yyvsp[-1].v.value;
			}
		}
break;
case 31:
#line 254 "..\\gs\\slic\\sliccmd.y"
{ 
			yyval.v.type = EXP_VAL_INT;
			yyval.v.value = yyvsp[0].v.value; 
		}
break;
case 32:
#line 259 "..\\gs\\slic\\sliccmd.y"
{ 			
			if(sliccmd_has_int_value(yyvsp[0].v.name)) {
				yyval.v.type = EXP_VAL_INT;
				yyval.v.value = sliccmd_get_int_value(yyvsp[0].v.name); 
			} else if((yyval.v.sym = sliccmd_maybe_lookup_sym(yyvsp[0].v.name)) != NULL) {
				yyval.v.type = EXP_VAL_SYM;
				yyval.v.sym = sliccmd_lookup_sym(yyvsp[0].v.name);
			} else {
				yyval.v.type = EXP_VAL_SYM;
				strcpy(yyval.v.name, yyvsp[0].v.name);
			}				
		}
break;
case 33:
#line 272 "..\\gs\\slic\\sliccmd.y"
{ 
		    yyval.v.value = sliccmd_call(yyvsp[-3].v.name);
		}
break;
case 34:
#line 276 "..\\gs\\slic\\sliccmd.y"
{ 
			if(sliccmd_ref_has_int_value(yyvsp[-2].v.name, yyvsp[0].v.name)) {
				yyval.v.type = EXP_VAL_INT;
				yyval.v.value = sliccmd_get_ref_value(yyvsp[-2].v.name, yyvsp[0].v.name); 
			} else {
				yyval.v.type = EXP_VAL_SYM;
				yyval.v.sym = sliccmd_get_ref_sym(yyvsp[-2].v.name, yyvsp[0].v.name);
			}
		}
break;
case 35:
#line 286 "..\\gs\\slic\\sliccmd.y"
{ 
			if(yyvsp[-1].v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {
				if(sliccmd_array_has_int_value(yyvsp[-3].v.name, yyvsp[-1].v.value)) {
					yyval.v.type = EXP_VAL_INT;
					yyval.v.value = sliccmd_array_lookup(yyvsp[-3].v.name, yyvsp[-1].v.value); 
				} else {
					yyval.v.type = EXP_VAL_SYM;
					yyval.v.sym = sliccmd_array_lookup_sym(yyvsp[-3].v.name, yyvsp[-1].v.value);
				}
			}
		}
break;
case 36:
#line 300 "..\\gs\\slic\\sliccmd.y"
{ 
			if(yyvsp[-3].v.type != EXP_VAL_INT) {
				sliccmd_type_error();
			} else {				
				int val;
				yyval.v.type = EXP_VAL_SYM;
				yyval.v.sym = sliccmd_array_lookup_reference(yyvsp[-5].v.name, yyvsp[-3].v.value, yyvsp[0].v.name); 
				if((yyval.v.sym) && (sliccmd_sym_has_int_value(yyval.v.sym, &val))) {
					yyval.v.type = EXP_VAL_INT;
					yyval.v.value = val;
					yyval.v.sym = NULL;
				}
			}
		}
break;
case 37:
#line 315 "..\\gs\\slic\\sliccmd.y"
{
			if(yyvsp[-1].v.type == EXP_VAL_INT) {
				yyval.v.type = EXP_VAL_TEMP_SYM;
				yyval.v.sym = sliccmd_get_db_name_sym_by_index(yyvsp[-3].v.dbptr, yyvsp[-1].v.value);
			} else {
				yyval.v.type = EXP_VAL_TEMP_SYM;
				yyval.v.sym = sliccmd_get_db_name_sym(yyvsp[-3].v.dbptr, yyvsp[-1].v.name);
			}
		}
break;
case 38:
#line 325 "..\\gs\\slic\\sliccmd.y"
{
			if(yyvsp[-3].v.type == EXP_VAL_INT) {
				yyval.v.type = EXP_VAL_INT;
				yyval.v.value = slicif_find_db_value_by_index(yyvsp[-5].v.dbptr, yyvsp[-3].v.value, yyvsp[0].v.name);
				yyval.v.sym = NULL;
			} else {

				yyval.v.type = EXP_VAL_INT;
				yyval.v.value = slicif_find_db_value(yyvsp[-5].v.dbptr, yyvsp[-3].v.name, yyvsp[0].v.name);
				yyval.v.sym = NULL;
			}
		}
break;
#line 808 "..\\gs\\slic\\sc.tab.c"
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
