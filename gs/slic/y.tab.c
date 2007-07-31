#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.8 (Berkeley) 01/20/90";
#endif
#define YYBYACC 1
#line 2 "..\\gs\\slic\\slic.y"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "slicif.h"

/* Avoid silly warnings */
#pragma warning( disable : 4013 )
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

#line 27 "..\\gs\\slic\\slic.y"
typedef union {
	int val;
	char* name;
    char* text;
	void *dbptr;
} YYSTYPE;
#line 37 "..\\gs\\slic\\y.tab.c"
#define NAME 257
#define STRINGID 258
#define IDENTIFIER 259
#define KW_MESSAGEBOX 260
#define KW_ALERTBOX 261
#define KW_TRIGGER 262
#define KW_WHEN 263
#define KW_IF 264
#define KW_BUTTON 265
#define KW_REGION 266
#define KW_ON 267
#define KW_HELPBOX 268
#define KW_ELSE 269
#define KW_ELSEIF 270
#define KW_WHILE 271
#define KW_FOR 272
#define NUMBER 273
#define GT 274
#define LT 275
#define GTE 276
#define LTE 277
#define EQ 278
#define REF 279
#define AND 280
#define OR 281
#define END_OF_INPUT 282
#define STRING 283
#define KW_ONCLOSE 284
#define KW_UNIT 285
#define KW_CITY 286
#define KW_LOCATION 287
#define KW_INT 288
#define KW_ARMY 289
#define KW_FUNCTION 290
#define KW_VOID 291
#define KW_RETURN 292
#define KW_INT_FUNC 293
#define KW_VOID_FUNC 294
#define KW_CONST 295
#define KW_HANDLEEVENT 296
#define KW_PRE 297
#define KW_POST 298
#define KW_EVENT 299
#define DBREF 300
#define NEQ 301
#define UMINUS 302
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    1,    1,    2,    2,    2,    2,    2,    2,    2,
    2,    7,    7,    7,   10,   10,   13,    3,   14,    3,
   15,    3,   16,    4,   18,    4,   20,   21,    5,   22,
   23,    5,   19,   19,    6,   25,    6,   26,   24,   24,
   27,    9,    8,   28,   28,   29,   29,   29,   30,   11,
   11,   11,   11,   11,   32,   12,   33,   12,   31,   31,
   37,   35,   35,   35,   35,   35,   34,   34,   34,   39,
   41,   34,   42,   34,   34,   43,   44,   45,   46,   34,
   47,   34,   34,   40,   48,   40,   40,   36,   36,   36,
   49,   49,   49,   49,   17,   38,   38,   38,   38,   38,
   38,   38,   38,   38,   38,   38,   38,   38,   38,   38,
   38,   38,   38,   50,   38,   38,   38,   38,   38,   38,
   38,   38,   52,   51,
};
short yylen[] = {                                         2,
    2,    2,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    3,    5,    6,    5,    6,    0,    4,    0,    4,
    0,    4,    0,    6,    0,    8,    0,    0,    8,    0,
    0,    9,    1,    1,   12,    0,    6,    0,    4,    1,
    2,    5,    5,    1,    1,    1,    3,    0,    2,    1,
    1,    1,    1,    1,    0,    4,    0,    3,    1,    2,
    0,    5,    3,    6,    5,    8,    2,    5,    2,    0,
    0,    8,    0,    6,    3,    0,    0,    0,    0,   13,
    0,    8,    1,    2,    0,    7,    0,    1,    3,    0,
    1,    1,    1,    1,    3,    3,    3,    3,    3,    3,
    3,    3,    3,    3,    3,    3,    3,    3,    2,    2,
    3,    1,    1,    0,    5,    3,    1,    3,    3,    4,
    6,    6,    0,    5,
};
short yydefred[] = {                                      0,
    0,    0,    0,    0,    0,   50,   53,   54,   52,   51,
   44,   45,    0,    0,    0,    0,    3,    4,    5,    6,
    7,    8,    9,   10,   11,    0,    0,    0,   17,   19,
    0,    0,   21,    0,    0,    1,    2,    0,    0,   41,
    0,    0,    0,    0,    0,   36,    0,    0,    0,   12,
    0,    0,    0,   46,    0,   18,   20,    0,    0,    0,
    0,   22,    0,    0,    0,    0,    0,   49,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   15,    0,
    0,    0,    0,   13,   42,   43,   47,    0,   70,    0,
   73,    0,    0,    0,    0,   83,    0,    0,    0,   58,
    0,  112,    0,    0,    0,    0,    0,    0,   24,    0,
    0,    0,   37,   16,   33,   34,   28,    0,   14,    0,
    0,    0,   61,    0,    0,    0,   76,   69,    0,    0,
   56,   60,   67,    0,  114,    0,    0,  110,  109,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   95,    0,    0,    0,    0,    0,   31,
    0,    0,    0,    0,    0,    0,    0,    0,   75,    0,
  116,  119,    0,    0,    0,    0,  111,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   98,   99,  100,
  118,   26,    0,   39,   29,    0,    0,    0,   93,   92,
   94,    0,    0,    0,    0,    0,    0,    0,   81,    0,
    0,    0,    0,    0,   32,    0,    0,    0,   62,    0,
    0,   68,    0,   77,    0,  115,  124,    0,    0,    0,
    0,    0,   89,   71,   74,    0,    0,  121,  122,    0,
    0,    0,    0,    0,   35,    0,    0,    0,   72,   78,
   82,   84,    0,    0,    0,    0,    0,   79,   85,    0,
    0,   80,   86,
};
short yydgoto[] = {                                      15,
   16,   17,   18,   19,   20,   21,   22,   23,   24,   25,
   26,   56,   41,   42,   47,   43,   74,   44,  117,   81,
  159,   82,  196,   78,   61,  112,   27,   28,   53,   54,
   97,   71,   72,   98,   99,  202,  164,  203,  124,  249,
  242,  126,  168,  236,  254,  260,  225,  261,  204,  173,
  108,  136,
};
short yysindex[] = {                                    554,
 -216, -214, -197, -167, -162,    0,    0,    0,    0,    0,
    0,    0, -149,   74,    0,  261,    0,    0,    0,    0,
    0,    0,    0,    0,    0, -138,   84, -131,    0,    0,
    0,  -24,    0,   69, -116,    0,    0,  -20, -106,    0,
   25,   25, -114, -115, -119,    0,   25,  -44,  117,    0,
  -87,  -98,   55,    0,    0,    0,    0,  120, -120,  121,
  -89,    0,  110, -103,    0,   81,  126,    0,  -31, -106,
  370,   51,  -30,   25,  -76,  -85,    0,  132,    0,  134,
 -222,  -64,  137,    0,    0,    0,    0,   -6,    0,  158,
    0,  159,   25,  -30,  142,    0,   79,  370,  149,    0,
  -38,    0,  170,  -30,  -30,  -30,   16,  -70,    0,  120,
  171,  174,    0,    0,    0,    0,    0, -222,    0,  -39,
  -30,  -30,    0,  173,  -37,  179,    0,    0,   35,  -35,
    0,    0,    0,  -22,    0,  129,  -29,    0,    0,   46,
  -30,  -30,  -30,  -30,  -30,  -30,  -30,  -30,  -30,  -30,
  -30,  -30,  -30,    0,  -27,   25,  -42,  -89,   25,    0,
  172,   64,  160,  -33,  -30,  191,  -30,  -21,    0,  194,
    0,    0,  -33,  -30,  -32,   75,    0,  453,  453,  453,
  453,  453,  169,  169,  453,   68,   68,    0,    0,    0,
    0,    0,  193,    0,    0,   25,  -30,  -56,    0,    0,
    0,  198,  160,  201,   86,   25,   97,  183,    0,  205,
  108,  -19,   -7,  -17,    0,  160,   -9,  -30,    0,  -33,
   25,    0,   25,    0,  -33,    0,    0,   -8,    2,  168,
  213,  160,    0,    0,    0,  -30,  234,    0,    0,  217,
  -30, -133,  119,  220,    0,  160,   25,  246,    0,    0,
    0,    0,  -30,  -21,  130,  257,   25,    0,    0,   25,
 -133,    0,    0,
};
short yyrindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
 -142,    0,    0,    0,    0,    0,    0,    0,   91,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  288,    0,    0,    0,    0,    0,
    0,    0,    0,    0, -233,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   -3,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  175,    0,    0,
  -23,    0,    0,    0,    0,    0,    0,  -12,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  -18,  258,    0,    0,    0,    0,    0,    0,
    0,    0,  258,    0,  147,    0,    0,  236,  408,  416,
  425,  438,   54,  427,  447,  287,  371,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  102,  260,    0,    0,    0,    0,    0,    0,
    0,    7,    0,    0,    0,    1,    0,    0,    0,  258,
    0,    0,    0,    0,  258,    0,    0,    0,    0,    0,
    0,   43,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  334,    0,    0,    0,   45,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  334,    0,    0,
};
short yygindex[] = {                                      0,
    0,  286,    0,    0,    0,    0,  -25,    0,    0,    0,
   -1,  539,    0,    0,    0,    0,  195,    0,  185,    0,
    0,    0,    0,  146,    0,    0,    0,    0,    0,  237,
  208,    0,    0,    0, -151, -146,    0,  660,    0,   53,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,
};
#define YYTABLESIZE 913
short yytable[] = {                                     105,
   64,  135,  105,  105,  218,   67,  106,  135,  212,  106,
  106,  104,  172,  113,  104,  104,  208,  113,  113,  113,
  113,  113,   63,  113,  117,   30,  210,   85,  117,  117,
  117,  117,  117,  123,  117,  113,   46,   52,   50,   38,
   63,   65,   29,  120,   30,   96,  117,  120,  120,  120,
  120,  120,  153,  120,  122,   40,  154,  151,  150,   65,
  149,   31,  152,   27,   27,  120,   45,  123,   52,  113,
   51,  153,   96,  233,  115,  116,  151,  150,  237,  149,
  117,  152,  153,   64,  121,   66,  177,  151,  150,   32,
  149,   55,  152,  169,  107,   69,   33,  107,   70,  120,
  153,   64,  256,   66,  153,  151,  150,   34,  149,  151,
  152,  153,  107,   35,  152,  213,  151,  150,   38,  149,
   23,  152,  153,   39,   25,   40,  221,  151,  150,   48,
  149,   48,  152,  153,   48,  247,  248,  223,  151,  150,
   49,  149,   91,  152,  153,   91,  107,   55,   58,  151,
  150,   59,  149,   60,  152,  153,  198,   65,   68,   73,
  151,  150,   75,  149,   76,  152,  153,   77,   79,   80,
  257,  151,  150,   83,  149,  100,  152,  250,    6,    7,
    8,    9,   10,  113,   84,   66,  110,  111,  113,  113,
  113,  113,  114,  113,  118,  119,  153,  125,  127,  130,
  227,  151,  150,  131,  149,  153,  152,  133,  155,  137,
  151,  150,  165,  149,  157,  152,  158,  161,  167,  174,
  166,  170,  217,  101,  199,  200,  101,  175,   63,  191,
  193,  206,  197,  209,  171,   88,  214,  123,  219,  102,
  134,  224,  102,  102,  220,  226,  134,  231,  238,  201,
  113,  113,  113,  113,  113,  230,  113,  113,  239,  228,
  240,  117,  117,  117,  117,  117,  103,  117,  117,  103,
  103,  229,  120,  241,  244,  245,  102,  113,  251,  102,
  120,  120,  120,  120,  120,  253,  120,  120,  117,  141,
  142,  143,  144,  145,  102,  146,  147,  258,   90,   59,
   88,   37,  160,  194,  156,  132,   87,  120,  141,  142,
  143,  144,  145,  263,  146,  147,  148,    0,    0,  141,
  142,  143,  144,  145,    0,  146,  147,   97,  102,   97,
   97,   97,    0,  107,  107,  148,    0,  141,  142,  143,
  144,  145,    0,  146,  147,   97,  148,    0,  141,  142,
  143,  144,  145,    0,  146,  147,    0,    0,    0,  141,
  142,  143,  144,  145,  148,  146,  147,    0,    0,    0,
  141,  142,  143,  144,  145,  148,  146,  147,    0,   97,
    0,  141,  142,  143,  144,  145,  148,  146,  147,    0,
    0,    0,  141,  142,  143,  144,  145,  148,  146,  147,
    0,    0,    0,  141,  142,  143,  144,  145,  148,  146,
  147,   96,   57,   96,   96,   96,    0,    0,    0,  148,
  113,  113,  113,  113,  113,    0,  113,  113,    0,   96,
  148,    0,    0,  141,  142,  143,  144,  145,    0,  146,
  147,    0,  141,  142,  143,  144,  145,  113,  101,    0,
    0,  101,    0,    0,    0,    0,  103,    0,   87,  103,
  148,    0,    0,   96,    0,  104,  101,  108,  104,  148,
  108,    0,    0,    0,  103,    0,    0,    0,  105,    0,
    0,  105,    0,  104,    0,  108,    0,  106,    0,  153,
  106,    0,    0,    0,  151,  150,  105,  149,    0,  152,
  101,    0,    0,    0,    0,  106,    0,    0,  103,  102,
  102,  102,  102,  102,    0,  102,  102,  104,    0,  108,
    1,    2,    3,    0,    0,    0,    4,    0,    5,    0,
  105,    0,    0,    0,    0,    0,  102,    0,    0,  106,
    0,    0,   36,    0,   55,    6,    7,    8,    9,   10,
    0,   55,   55,   11,   12,   13,   14,    0,   55,   55,
   97,   97,   97,   97,   97,    0,   97,   97,    0,    0,
    0,   55,   55,   55,   55,   55,   55,    0,    0,   55,
   57,    0,    0,    0,    0,   62,   55,   97,    0,    0,
   87,    0,    0,    0,    0,    0,    0,   87,   87,    0,
    0,    0,    0,    0,   87,   87,    0,   86,    0,    0,
    0,    0,  109,    0,    0,    0,    0,   87,   87,   87,
   87,   87,   87,    0,    0,   87,   88,    0,    0,    0,
    0,  128,   87,   89,   90,    0,    0,    0,    0,    0,
   91,   92,    0,    0,   96,   96,   96,   96,   96,    0,
   96,   96,    0,   93,    6,    7,    8,    9,   10,    0,
    0,   94,    0,    0,    0,    0,    0,    0,   95,    0,
    0,   96,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  101,  101,  101,  101,  101,    0,  101,  101,  103,
  103,  103,  103,  103,  192,  103,  103,  195,  104,  104,
  104,  104,  104,    0,  104,  104,  108,  108,  101,    0,
    0,  105,  105,  105,  105,  105,  103,  105,  105,    0,
  106,  106,  106,  106,  106,  104,  106,  106,    0,    0,
    0,    0,  107,    0,  215,    0,    0,    0,  105,    0,
    0,    0,    0,    0,  222,    0,    0,  106,    0,    0,
    0,    0,    0,  129,    0,    0,    0,    0,    0,  234,
    0,  235,    0,  138,  139,  140,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  162,  163,    0,    0,    0,  252,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  259,  176,    0,  262,    0,
  178,  179,  180,  181,  182,  183,  184,  185,  186,  187,
  188,  189,  190,    1,    2,    3,    0,    0,    0,    4,
    0,    5,    0,    0,  205,    0,  207,    0,    0,    0,
    0,    0,    0,  211,    0,    0,    0,    0,    6,    7,
    8,    9,   10,    0,    0,    0,   11,   12,   13,   14,
    0,    0,    0,    0,    0,    0,  216,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  232,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  243,    0,    0,    0,    0,
  246,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  255,
};
short yycheck[] = {                                      33,
   45,   40,   33,   33,   61,   93,   40,   40,   41,   40,
   40,   45,   35,   37,   45,   45,  168,   41,   42,   43,
   44,   45,   41,   47,   37,  259,  173,   59,   41,   42,
   43,   44,   45,   40,   47,   59,   61,   39,   59,   43,
   59,   41,  259,   37,  259,   71,   59,   41,   42,   43,
   44,   45,   37,   47,   61,   59,   41,   42,   43,   59,
   45,  259,   47,  297,  298,   59,   91,   91,   70,   93,
   91,   37,   98,  220,  297,  298,   42,   43,  225,   45,
   93,   47,   37,   41,   91,   41,   41,   42,   43,  257,
   45,  123,   47,   59,   41,   41,  259,   44,   44,   93,
   37,   59,  254,   59,   37,   42,   43,  257,   45,   42,
   47,   37,   59,   40,   47,   41,   42,   43,  257,   45,
  263,   47,   37,   40,  267,  257,   41,   42,   43,   61,
   45,   41,   47,   37,   44,  269,  270,   41,   42,   43,
  257,   45,   41,   47,   37,   44,   93,  123,  263,   42,
   43,  267,   45,  273,   47,   37,   93,   41,  257,   40,
   42,   43,  283,   45,   44,   47,   37,  257,   59,  273,
   41,   42,   43,   93,   45,  125,   47,   59,  285,  286,
  287,  288,  289,   37,   59,  273,  263,  273,   42,   43,
   59,   45,   59,   47,  259,   59,   37,   40,   40,   58,
   93,   42,   43,  125,   45,   37,   47,   59,  279,   40,
   42,   43,   40,   45,   44,   47,   43,  257,   40,   91,
  258,  257,  279,  257,  258,  259,  257,  257,  273,  257,
  273,   41,   61,   40,  257,  257,   44,   91,   41,  273,
  279,   59,  273,  273,   44,   41,  279,  257,  257,  283,
  274,  275,  276,  277,  278,  273,  280,  281,  257,  279,
   93,  274,  275,  276,  277,  278,  300,  280,  281,  300,
  300,  279,  279,   61,   41,   59,   41,  301,   59,   44,
  274,  275,  276,  277,  278,   40,  280,  281,  301,  274,
  275,  276,  277,  278,   59,  280,  281,   41,   41,  125,
   41,   16,  118,  158,  110,   98,   70,  301,  274,  275,
  276,  277,  278,  261,  280,  281,  301,   -1,   -1,  274,
  275,  276,  277,  278,   -1,  280,  281,   41,   93,   43,
   44,   45,   -1,  280,  281,  301,   -1,  274,  275,  276,
  277,  278,   -1,  280,  281,   59,  301,   -1,  274,  275,
  276,  277,  278,   -1,  280,  281,   -1,   -1,   -1,  274,
  275,  276,  277,  278,  301,  280,  281,   -1,   -1,   -1,
  274,  275,  276,  277,  278,  301,  280,  281,   -1,   93,
   -1,  274,  275,  276,  277,  278,  301,  280,  281,   -1,
   -1,   -1,  274,  275,  276,  277,  278,  301,  280,  281,
   -1,   -1,   -1,  274,  275,  276,  277,  278,  301,  280,
  281,   41,  125,   43,   44,   45,   -1,   -1,   -1,  301,
  274,  275,  276,  277,  278,   -1,  280,  281,   -1,   59,
  301,   -1,   -1,  274,  275,  276,  277,  278,   -1,  280,
  281,   -1,  274,  275,  276,  277,  278,  301,   41,   -1,
   -1,   44,   -1,   -1,   -1,   -1,   41,   -1,  125,   44,
  301,   -1,   -1,   93,   -1,   41,   59,   41,   44,  301,
   44,   -1,   -1,   -1,   59,   -1,   -1,   -1,   41,   -1,
   -1,   44,   -1,   59,   -1,   59,   -1,   41,   -1,   37,
   44,   -1,   -1,   -1,   42,   43,   59,   45,   -1,   47,
   93,   -1,   -1,   -1,   -1,   59,   -1,   -1,   93,  274,
  275,  276,  277,  278,   -1,  280,  281,   93,   -1,   93,
  260,  261,  262,   -1,   -1,   -1,  266,   -1,  268,   -1,
   93,   -1,   -1,   -1,   -1,   -1,  301,   -1,   -1,   93,
   -1,   -1,  282,   -1,  257,  285,  286,  287,  288,  289,
   -1,  264,  265,  293,  294,  295,  296,   -1,  271,  272,
  274,  275,  276,  277,  278,   -1,  280,  281,   -1,   -1,
   -1,  284,  285,  286,  287,  288,  289,   -1,   -1,  292,
   42,   -1,   -1,   -1,   -1,   47,  299,  301,   -1,   -1,
  257,   -1,   -1,   -1,   -1,   -1,   -1,  264,  265,   -1,
   -1,   -1,   -1,   -1,  271,  272,   -1,   69,   -1,   -1,
   -1,   -1,   74,   -1,   -1,   -1,   -1,  284,  285,  286,
  287,  288,  289,   -1,   -1,  292,  257,   -1,   -1,   -1,
   -1,   93,  299,  264,  265,   -1,   -1,   -1,   -1,   -1,
  271,  272,   -1,   -1,  274,  275,  276,  277,  278,   -1,
  280,  281,   -1,  284,  285,  286,  287,  288,  289,   -1,
   -1,  292,   -1,   -1,   -1,   -1,   -1,   -1,  299,   -1,
   -1,  301,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  274,  275,  276,  277,  278,   -1,  280,  281,  274,
  275,  276,  277,  278,  156,  280,  281,  159,  274,  275,
  276,  277,  278,   -1,  280,  281,  280,  281,  301,   -1,
   -1,  274,  275,  276,  277,  278,  301,  280,  281,   -1,
  274,  275,  276,  277,  278,  301,  280,  281,   -1,   -1,
   -1,   -1,   73,   -1,  196,   -1,   -1,   -1,  301,   -1,
   -1,   -1,   -1,   -1,  206,   -1,   -1,  301,   -1,   -1,
   -1,   -1,   -1,   94,   -1,   -1,   -1,   -1,   -1,  221,
   -1,  223,   -1,  104,  105,  106,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  121,  122,   -1,   -1,   -1,  247,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  257,  137,   -1,  260,   -1,
  141,  142,  143,  144,  145,  146,  147,  148,  149,  150,
  151,  152,  153,  260,  261,  262,   -1,   -1,   -1,  266,
   -1,  268,   -1,   -1,  165,   -1,  167,   -1,   -1,   -1,
   -1,   -1,   -1,  174,   -1,   -1,   -1,   -1,  285,  286,
  287,  288,  289,   -1,   -1,   -1,  293,  294,  295,  296,
   -1,   -1,   -1,   -1,   -1,   -1,  197,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  218,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  236,   -1,   -1,   -1,   -1,
  241,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  253,
};
#define YYFINAL 15
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 302
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"'!'",0,"'#'",0,"'%'",0,0,"'('","')'","'*'","'+'","','","'-'",0,"'/'",0,0,0,0,0,
0,0,0,0,0,"':'","';'",0,"'='",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,"'['",0,"']'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"'{'",0,"'}'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"NAME","STRINGID","IDENTIFIER",
"KW_MESSAGEBOX","KW_ALERTBOX","KW_TRIGGER","KW_WHEN","KW_IF","KW_BUTTON",
"KW_REGION","KW_ON","KW_HELPBOX","KW_ELSE","KW_ELSEIF","KW_WHILE","KW_FOR",
"NUMBER","GT","LT","GTE","LTE","EQ","REF","AND","OR","END_OF_INPUT","STRING",
"KW_ONCLOSE","KW_UNIT","KW_CITY","KW_LOCATION","KW_INT","KW_ARMY","KW_FUNCTION",
"KW_VOID","KW_RETURN","KW_INT_FUNC","KW_VOID_FUNC","KW_CONST","KW_HANDLEEVENT",
"KW_PRE","KW_POST","KW_EVENT","DBREF","NEQ","UMINUS",
};
char *yyrule[] = {
"$accept : slic",
"slic : objects END_OF_INPUT",
"objects : objects object",
"objects : object",
"object : messagebox",
"object : trigger",
"object : eventhandler",
"object : region",
"object : typedef",
"object : function",
"object : functionproto",
"object : const",
"typedef : vartype NAME ';'",
"typedef : vartype NAME '[' ']' ';'",
"typedef : vartype NAME '[' NUMBER ']' ';'",
"const : KW_CONST NAME '=' NUMBER ';'",
"const : KW_CONST NAME '=' '-' NUMBER ';'",
"$$1 :",
"messagebox : KW_MESSAGEBOX IDENTIFIER $$1 body",
"$$2 :",
"messagebox : KW_ALERTBOX IDENTIFIER $$2 body",
"$$3 :",
"messagebox : KW_HELPBOX IDENTIFIER $$3 body",
"$$4 :",
"trigger : KW_TRIGGER IDENTIFIER $$4 KW_WHEN triggercondition body",
"$$5 :",
"trigger : KW_TRIGGER IDENTIFIER $$5 KW_ON STRING KW_WHEN triggercondition body",
"$$6 :",
"$$7 :",
"eventhandler : KW_HANDLEEVENT '(' NAME ')' $$6 priority $$7 body",
"$$8 :",
"$$9 :",
"eventhandler : KW_HANDLEEVENT '(' NAME ')' $$8 IDENTIFIER priority $$9 body",
"priority : KW_PRE",
"priority : KW_POST",
"region : KW_REGION NAME '[' NUMBER ',' NUMBER ',' NUMBER ',' NUMBER ']' ';'",
"$$10 :",
"region : KW_REGION NAME '=' $$10 regionlist ';'",
"$$11 :",
"regionlist : NAME $$11 '+' regionlist",
"regionlist : NAME",
"functionhead : returntype NAME",
"functionproto : functionhead '(' functionarglist ')' ';'",
"function : functionhead '(' functionarglist ')' body",
"returntype : KW_INT_FUNC",
"returntype : KW_VOID_FUNC",
"functionarglist : functionargument",
"functionarglist : functionarglist ',' functionargument",
"functionarglist :",
"functionargument : vartype NAME",
"vartype : KW_UNIT",
"vartype : KW_ARMY",
"vartype : KW_INT",
"vartype : KW_CITY",
"vartype : KW_LOCATION",
"$$12 :",
"body : '{' $$12 statements '}'",
"$$13 :",
"body : '{' $$13 '}'",
"statements : statement",
"statements : statement statements",
"$$14 :",
"simplestatement : NAME '(' $$14 arguments ')'",
"simplestatement : NAME '=' expression",
"simplestatement : NAME '[' expression ']' '=' expression",
"simplestatement : NAME REF NAME '=' expression",
"simplestatement : NAME '[' expression ']' REF NAME '=' expression",
"statement : simplestatement ';'",
"statement : KW_BUTTON '(' STRINGID ')' body",
"statement : KW_ONCLOSE body",
"$$15 :",
"$$16 :",
"statement : KW_IF $$15 '(' expression ')' body $$16 elsestatements",
"$$17 :",
"statement : KW_WHILE $$17 '(' expression ')' body",
"statement : KW_RETURN expression ';'",
"$$18 :",
"$$19 :",
"$$20 :",
"$$21 :",
"statement : KW_FOR '(' $$18 simplestatement ';' $$19 expression ';' $$20 simplestatement ')' $$21 body",
"$$22 :",
"statement : KW_EVENT ':' NAME '(' $$22 arguments ')' ';'",
"statement : typedef",
"elsestatements : KW_ELSE body",
"$$23 :",
"elsestatements : KW_ELSEIF '(' expression ')' body $$23 elsestatements",
"elsestatements :",
"arguments : argument",
"arguments : argument ',' arguments",
"arguments :",
"argument : expression",
"argument : IDENTIFIER",
"argument : STRINGID",
"argument : STRING",
"triggercondition : '(' expression ')'",
"expression : expression '+' expression",
"expression : expression '-' expression",
"expression : expression '*' expression",
"expression : expression '/' expression",
"expression : expression '%' expression",
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
"$$24 :",
"expression : NAME '(' $$24 arguments ')'",
"expression : NAME REF NAME",
"expression : arrayref",
"expression : arrayref REF NAME",
"expression : NAME REF '#'",
"expression : DBREF '(' NAME ')'",
"expression : DBREF '(' NAME ')' REF NAME",
"expression : DBREF '(' expression ')' REF NAME",
"$$25 :",
"arrayref : NAME $$25 '[' expression ']'",
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
#line 341 "..\\gs\\slic\\slic.y"

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

#line 643 "..\\gs\\slic\\y.tab.c"
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
#line 60 "..\\gs\\slic\\slic.y"
{ slic_parser_done = 1; }
break;
case 12:
#line 77 "..\\gs\\slic\\slic.y"
{ slicif_declare_sym(yyvsp[-1].name, (SLIC_SYM)yyvsp[-2].val); }
break;
case 13:
#line 78 "..\\gs\\slic\\slic.y"
{ slicif_declare_array(yyvsp[-3].name, (SLIC_SYM)yyvsp[-4].val); }
break;
case 14:
#line 79 "..\\gs\\slic\\slic.y"
{ slicif_declare_fixed_array(yyvsp[-4].name, (SLIC_SYM)yyvsp[-5].val, yyvsp[-2].val); }
break;
case 15:
#line 82 "..\\gs\\slic\\slic.y"
{ slicif_add_const(yyvsp[-3].name, yyvsp[-1].val); }
break;
case 16:
#line 83 "..\\gs\\slic\\slic.y"
{ slicif_add_const(yyvsp[-4].name, -yyvsp[-2].val); }
break;
case 17:
#line 86 "..\\gs\\slic\\slic.y"
{ slicif_start_segment(yyvsp[0].name); }
break;
case 18:
#line 87 "..\\gs\\slic\\slic.y"
{ 
		struct PSlicObject *obj = malloc(sizeof(struct PSlicObject));
#ifdef _DEBUG
		fprintf(debuglog, "Parsed MessageBox %s\n", yyvsp[-2]); 
#endif
		obj->m_type = SLIC_OBJECT_MESSAGEBOX;
		obj->m_id = yyvsp[-2].name;
		obj->m_is_alert = 0;
		obj->m_is_help = 0;
		slicif_add_object(obj);
	}
break;
case 19:
#line 98 "..\\gs\\slic\\slic.y"
{ slicif_start_segment(yyvsp[0].name); }
break;
case 20:
#line 99 "..\\gs\\slic\\slic.y"
{
		struct PSlicObject *obj = malloc(sizeof(struct PSlicObject));
#ifdef _DEBUG
		fprintf(debuglog, "Parsed AlertBox %s\n", yyvsp[-2]);
#endif
		obj->m_type = SLIC_OBJECT_MESSAGEBOX;
		obj->m_id = yyvsp[-2].name;
		obj->m_is_alert = 1;
		obj->m_is_help = 0;
		slicif_add_object(obj);
	}
break;
case 21:
#line 110 "..\\gs\\slic\\slic.y"
{ slicif_start_segment(yyvsp[0].name); }
break;
case 22:
#line 111 "..\\gs\\slic\\slic.y"
{
		struct PSlicObject *obj = malloc(sizeof(struct PSlicObject));
#ifdef _DEBUG
		fprintf(debuglog, "Parsed AlertBox %s\n", yyvsp[-2]);
#endif
		obj->m_type = SLIC_OBJECT_MESSAGEBOX;
		obj->m_id = yyvsp[-2].name;
		obj->m_is_alert = 0;
		obj->m_is_help = 1;
		slicif_add_object(obj);
	}
break;
case 23:
#line 124 "..\\gs\\slic\\slic.y"
{ slicif_start_segment(yyvsp[0].name); }
break;
case 24:
#line 125 "..\\gs\\slic\\slic.y"
{
		struct PSlicObject *obj = malloc(sizeof(struct PSlicObject));
#ifdef _DEBUG
		fprintf(debuglog, "Parsed Trigger %s\n", yyvsp[-4]);
#endif

		obj->m_is_alert = 0;
		obj->m_is_help = 0;
		obj->m_type = SLIC_OBJECT_TRIGGER;
		obj->m_id = yyvsp[-4].name;
		obj->m_ui_component = NULL;
		slicif_add_object(obj);
	}
break;
case 25:
#line 138 "..\\gs\\slic\\slic.y"
{ slicif_start_segment(yyvsp[0].name); }
break;
case 26:
#line 139 "..\\gs\\slic\\slic.y"
{
		struct PSlicObject *obj = malloc(sizeof(struct PSlicObject));
#ifdef _DEBUG
		fprintf(debuglog, "Parsed Trigger %s\n", yyvsp[-6]);
#endif
		obj->m_is_alert = 0;
		obj->m_is_help = 0;
		obj->m_type = SLIC_OBJECT_TRIGGER;
		obj->m_id = yyvsp[-6].name;
		obj->m_ui_component = yyvsp[-3].name;
		slicif_add_object(obj);
	}
break;
case 27:
#line 154 "..\\gs\\slic\\slic.y"
{ 
			      slicif_check_event_exists(yyvsp[-1].name);
				  slicif_set_event_checking(yyvsp[-1].name);
		      }
break;
case 28:
#line 158 "..\\gs\\slic\\slic.y"
{ slicif_start_segment(slicif_create_name(yyvsp[-3].name)); }
break;
case 29:
#line 159 "..\\gs\\slic\\slic.y"
{
		struct PSlicObject *obj = malloc(sizeof(struct PSlicObject));
#ifdef _DEBUG
	    fprintf(debuglog, "Parsed HandleEvent %s\n", yyvsp[-5].name);
#endif	
		obj->m_is_alert = 0;
		obj->m_is_help = 0;
		obj->m_type = SLIC_OBJECT_HANDLEEVENT;
		obj->m_id = slicif_get_segment_name_copy();
		obj->m_ui_component = NULL;
		obj->m_event_name = yyvsp[-5].name;
		obj->m_priority = slicif_get_priority();
		slicif_add_object(obj);
	}
break;
case 30:
#line 174 "..\\gs\\slic\\slic.y"
{ 
	      slicif_check_event_exists(yyvsp[-1].name);
		  slicif_set_event_checking(yyvsp[-1].name);
	  }
break;
case 31:
#line 179 "..\\gs\\slic\\slic.y"
{ 
		  slicif_start_segment(yyvsp[-1].name); 
	  }
break;
case 32:
#line 183 "..\\gs\\slic\\slic.y"
{
		struct PSlicObject *obj = malloc(sizeof(struct PSlicObject));
#ifdef _DEBUG
	    fprintf(debuglog, "Parsed HandleEvent %s '%s'\n", yyvsp[-6].name, yyvsp[-3].name);
#endif	
		obj->m_is_alert = 0;
		obj->m_is_help = 0;
		obj->m_type = SLIC_OBJECT_HANDLEEVENT;
		obj->m_id = yyvsp[-3].name;
		obj->m_ui_component = NULL;
		obj->m_event_name = yyvsp[-6].name;
		obj->m_priority = slicif_get_priority();
		slicif_add_object(obj);
    }
break;
case 33:
#line 200 "..\\gs\\slic\\slic.y"
{ slicif_set_priority(SLIC_PRI_PRE); }
break;
case 34:
#line 202 "..\\gs\\slic\\slic.y"
{ slicif_set_priority(SLIC_PRI_POST); }
break;
case 35:
#line 206 "..\\gs\\slic\\slic.y"
{
		    slicif_add_region(yyvsp[-10].name, yyvsp[-8].val, yyvsp[-6].val, yyvsp[-4].val, yyvsp[-2].val);
		}
break;
case 36:
#line 209 "..\\gs\\slic\\slic.y"
{ slicif_start_complex_region(yyvsp[-1].name); }
break;
case 37:
#line 210 "..\\gs\\slic\\slic.y"
{ slicif_finish_complex_region(); }
break;
case 38:
#line 213 "..\\gs\\slic\\slic.y"
{slicif_add_region_to_complex(yyvsp[0].name); }
break;
case 40:
#line 214 "..\\gs\\slic\\slic.y"
{ slicif_add_region_to_complex(yyvsp[0].name); }
break;
case 41:
#line 218 "..\\gs\\slic\\slic.y"
{ yyval.name = yyvsp[0].name; slicif_start_segment(yyvsp[0].name); }
break;
case 42:
#line 221 "..\\gs\\slic\\slic.y"
{ slicif_add_prototype(yyvsp[-4].name); }
break;
case 43:
#line 225 "..\\gs\\slic\\slic.y"
{
		struct PSlicObject *obj = malloc(sizeof(struct PSlicObject));
#ifdef _DEBUG
		fprintf(debuglog, "Parsed Function %s\n", yyvsp[-4].name); 
#endif
		obj->m_type = SLIC_OBJECT_FUNCTION;
		obj->m_id = yyvsp[-4].name;
		obj->m_is_alert = 0;
		obj->m_is_help = 0;
		slicif_add_object(obj);
	}
break;
case 44:
#line 238 "..\\gs\\slic\\slic.y"
{ slicif_function_return(SF_RET_INT); }
break;
case 45:
#line 239 "..\\gs\\slic\\slic.y"
{ slicif_function_return(SF_RET_VOID); }
break;
case 49:
#line 247 "..\\gs\\slic\\slic.y"
{ slicif_add_parameter((SLIC_SYM)yyvsp[-1].val, yyvsp[0].name); }
break;
case 50:
#line 250 "..\\gs\\slic\\slic.y"
{ yyval.val = SLIC_SYM_UNIT; }
break;
case 51:
#line 251 "..\\gs\\slic\\slic.y"
{ yyval.val = SLIC_SYM_ARMY; }
break;
case 52:
#line 252 "..\\gs\\slic\\slic.y"
{ yyval.val = SLIC_SYM_IVAR; }
break;
case 53:
#line 253 "..\\gs\\slic\\slic.y"
{ yyval.val = SLIC_SYM_CITY; }
break;
case 54:
#line 254 "..\\gs\\slic\\slic.y"
{ yyval.val = SLIC_SYM_LOCATION; }
break;
case 55:
#line 257 "..\\gs\\slic\\slic.y"
{ slicif_start_block(); }
break;
case 56:
#line 257 "..\\gs\\slic\\slic.y"
{ slicif_end_block(); }
break;
case 57:
#line 258 "..\\gs\\slic\\slic.y"
{ slicif_start_block(); }
break;
case 58:
#line 258 "..\\gs\\slic\\slic.y"
{ slicif_end_block(); }
break;
case 61:
#line 265 "..\\gs\\slic\\slic.y"
{slicif_add_op(SOP_SARGS);}
break;
case 62:
#line 265 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_CALL, yyvsp[-4].name); }
break;
case 63:
#line 266 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_ASSN, yyvsp[-2].name); }
break;
case 64:
#line 267 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_ASSNA, yyvsp[-5].name); }
break;
case 65:
#line 268 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_ASSNM, yyvsp[-4].name, yyvsp[-2].name); }
break;
case 66:
#line 269 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_ASSNAM, yyvsp[-7].name, yyvsp[-2].name); }
break;
case 68:
#line 273 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_BUTN, yyvsp[-2].val); }
break;
case 69:
#line 274 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_OCLS); }
break;
case 70:
#line 275 "..\\gs\\slic\\slic.y"
{ slicif_start_if(); }
break;
case 71:
#line 276 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_BNT); }
break;
case 72:
#line 277 "..\\gs\\slic\\slic.y"
{ slicif_end_if(); }
break;
case 73:
#line 278 "..\\gs\\slic\\slic.y"
{ slicif_start_while(); }
break;
case 74:
#line 279 "..\\gs\\slic\\slic.y"
{ slicif_end_while(); }
break;
case 75:
#line 280 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_RET); }
break;
case 76:
#line 281 "..\\gs\\slic\\slic.y"
{ slicif_start_for(); }
break;
case 77:
#line 282 "..\\gs\\slic\\slic.y"
{ slicif_for_expression(); }
break;
case 78:
#line 283 "..\\gs\\slic\\slic.y"
{ slicif_for_continue(); }
break;
case 79:
#line 284 "..\\gs\\slic\\slic.y"
{ slicif_start_for_body(); }
break;
case 80:
#line 284 "..\\gs\\slic\\slic.y"
{ slicif_end_for(); }
break;
case 81:
#line 285 "..\\gs\\slic\\slic.y"
{slicif_start_event(yyvsp[-1].name); }
break;
case 82:
#line 285 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_EVENT, yyvsp[-5].name); }
break;
case 84:
#line 291 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_BNEV); }
break;
case 85:
#line 292 "..\\gs\\slic\\slic.y"
{slicif_add_op(SOP_BNT); }
break;
case 91:
#line 301 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_ARGE); }
break;
case 92:
#line 302 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_ARGID, yyvsp[0].name); }
break;
case 93:
#line 303 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_ARGS, yyvsp[0].val); }
break;
case 94:
#line 304 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_ARGST, yyvsp[0].name); }
break;
case 95:
#line 307 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_TRIG); }
break;
case 96:
#line 310 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_ADD); }
break;
case 97:
#line 311 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_SUB); }
break;
case 98:
#line 312 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_MULT); }
break;
case 99:
#line 313 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_DIV); }
break;
case 100:
#line 314 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_MOD); }
break;
case 101:
#line 315 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_LT); }
break;
case 102:
#line 316 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_GT); }
break;
case 103:
#line 317 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_GTE); }
break;
case 104:
#line 318 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_LTE); }
break;
case 105:
#line 319 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_EQ); }
break;
case 106:
#line 320 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_NEQ); }
break;
case 107:
#line 321 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_AND); }
break;
case 108:
#line 322 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_OR); }
break;
case 109:
#line 323 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_NOT); }
break;
case 110:
#line 324 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_NEG); }
break;
case 111:
#line 325 "..\\gs\\slic\\slic.y"
{ }
break;
case 112:
#line 326 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_PUSHI, yyvsp[0].val); }
break;
case 113:
#line 327 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_PUSHV, yyvsp[0].name); }
break;
case 114:
#line 328 "..\\gs\\slic\\slic.y"
{slicif_add_op(SOP_SARGS); }
break;
case 115:
#line 328 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_CALLR, yyvsp[-4].name); }
break;
case 116:
#line 329 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_PUSHM, yyvsp[-2].name, yyvsp[0].name); }
break;
case 117:
#line 330 "..\\gs\\slic\\slic.y"
{slicif_add_op(SOP_AINDX); }
break;
case 118:
#line 331 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_PUSHAM, yyvsp[-2].name, yyvsp[0].name); }
break;
case 119:
#line 332 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_ASIZE, yyvsp[-2].name); }
break;
case 120:
#line 333 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_PUSHI, slicif_find_db_index(yyvsp[-3].dbptr, yyvsp[-1].name)); }
break;
case 121:
#line 334 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_PUSHI, slicif_find_db_value(yyvsp[-5].dbptr, yyvsp[-3].name, yyvsp[0].name)); }
break;
case 122:
#line 335 "..\\gs\\slic\\slic.y"
{ slicif_add_op(SOP_PUSHI, slicif_find_db_value_by_index(yyvsp[-5].dbptr, yyvsp[-3].val, yyvsp[0].name)); }
break;
case 123:
#line 338 "..\\gs\\slic\\slic.y"
{ yyval.name = yyvsp[0].name; slicif_add_op(SOP_PUSHA, yyvsp[0].name); }
break;
#line 1282 "..\\gs\\slic\\y.tab.c"
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
