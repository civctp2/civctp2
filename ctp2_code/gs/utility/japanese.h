/*
	Macros for japanese sjis code and kataka code by t.s. 2003.12
*/

#define IS_SJIS_1ST(c) ( c <= -97  && c >= -127 || c <= -17 && c >= -32 )
#define IS_NOT_SJIS_1ST(c) ( c > -17 || c < -32 && c > -97 || c < -127  )
#define IS_SJIS_2ND(c) ( c <= 126  && c >= 64 || c <= -4 && c >= -128 )
#define IS_NOT_SJIS_2ND(c) ( c > 126 || c < 64 && c > -4 || c < -128  )
#define IS_KATAKANA(c)  ( c >= -95 && c <= -33 )
