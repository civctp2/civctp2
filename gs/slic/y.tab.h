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
typedef union {
	int val;
	char* name;
    char* text;
	void *dbptr;
} YYSTYPE;
extern YYSTYPE yylval;
