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
typedef union {
	int intval;
	double floatval;
	char *nameval;
	char *stringval;
	void *names;
	void *block;
} YYSTYPE;
extern YYSTYPE yylval;
