//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C header
// Description  : 
// Id           : $Id$
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications since addition:
//
// - Moved CIV_INDEX and GENDER into this file to resolve
//   cyclic header dependencies
// - Replaced CIV_INDEX by sint32. (2-Jan-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------
#ifndef __GS_GAMEOBJ__GAMEOBJ_TYPES__H__
#define __GS_GAMEOBJ__GAMEOBJ_TYPES__H__ 1

#define CIV_INDEX_VANDALS    0
#define CIV_INDEX_INVALID   -1
#define CIV_INDEX_RANDOM  1024

/**
 * Gender of the civilization's leader
 */
enum GENDER {
	GENDER_MALE,
	GENDER_FEMALE,

	GENDER_RANDOM,
	
	GENDER_MAX
};

#endif
