//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Great library
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
// HAVE_PRAGMA_ONCE
// - Uses #pragma once preprocessor derective
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Microsoft extensions marked.
// - Increased maximum library text size to support the German version.
// - Exported database name size max.
// - Added function to look up an item name on creation index.
// - Added alpha <-> index functions. (Sep 13th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------
//
/// \file   GreatLibrary.h
/// \brief  Great library handling

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __GREATLIBRARYTYPES_H__
#define __GREATLIBRARYTYPES_H__

enum DATABASE {
	DATABASE_DEFAULT,
	DATABASE_UNITS,
	DATABASE_BUILDINGS,
	DATABASE_WONDERS,
	DATABASE_ADVANCES,
	DATABASE_TERRAIN,
	DATABASE_CONCEPTS,
	DATABASE_GOVERNMENTS,
	DATABASE_TILE_IMPROVEMENTS,
	DATABASE_RESOURCE,
	DATABASE_ORDERS,
	DATABASE_SEARCH,

	DATABASE_MAX
};

enum LIB_STRING {
	LIB_STRING_INDEX,
	LIB_STRING_TREE
};

#endif
