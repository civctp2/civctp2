//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Database lexer (Lexical Analyzer Generator) token header
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
// 
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Implemented GovernmentsModified subclass (allowing cdb files including
//   a GovernmentsModified record to produce parsers capable of reading and
//   storing subrecords for Government types.)
//   See http://apolyton.net/forums/showthread.php?s=&threadid=107916 for
//   more details  _____ by MrBaggins Jan-04
//
//   * Added Modified and Modified delimiter tokens (chars '>' and ',') to
//     assist in lexical analysis of new additional GovernmentsModified
//     record syntax
//
//----------------------------------------------------------------------------

#ifndef __DB_TOKENS_H__
#define __DB_TOKENS_H__

#define k_Token_Equals      '='
#define k_Token_OpenBrace   '{'
#define k_Token_CloseBrace  '}'

#if !defined(ACTIVISION_ORIGINAL)//GovMod-See DBLexer.l also
#define k_Token_Modified '>'
#define k_Token_ModifiedDelimiter ','
//EndGovMod
#endif

#define k_First_Special_Token 256
#define k_Token_Name          ((k_First_Special_Token) + 1)
#define k_Token_String        ((k_First_Special_Token) + 2)
#define k_Token_Int           ((k_First_Special_Token) + 3)
#define k_Token_Float         ((k_First_Special_Token) + 4)

#define k_Token_Custom_Base 500

#endif
