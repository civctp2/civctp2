
#ifndef __DB_TOKENS_H__
#define __DB_TOKENS_H__

#define k_Token_Equals      '='
#define k_Token_OpenBrace   '{'
#define k_Token_CloseBrace  '}'

#define k_First_Special_Token 256
#define k_Token_Name          ((k_First_Special_Token) + 1)
#define k_Token_String        ((k_First_Special_Token) + 2)
#define k_Token_Int           ((k_First_Special_Token) + 3)
#define k_Token_Float         ((k_First_Special_Token) + 4)

#define k_Token_Custom_Base 500

#endif
