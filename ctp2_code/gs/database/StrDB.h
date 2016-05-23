//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  :
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
// _MSC_VER
// - Compiler version (for the Microsoft C++ compiler only)
//
// Note: For the blocks with _MSC_VER preprocessor directives, the following
//       is implied: the (_MSC_VER) preprocessor directive lines, and the blocks
//       that are inactive for _MSC_VER value 1200 are modified Apolyton code.
//       The blocks that are inactiThe blocks that are active for _MSC_VER value
//       1200 are the original Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Reimplemented containers as vectors, to make it less error prone.
// - Load default strings if they are missing in the database so that mods
//   also have a full set of strings. (Jan 30th 2006 Martin Gühmann)
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __STRING_DB_H__
#define __STRING_DB_H__ 1

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

#include <vector>       // std::vector

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class	StringDB;

extern	StringDB *	g_theStringDB;

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "c3types.h"	// MBCHAR
#include "dbtypes.h"	// StringId
#include "StrRec.h"		// StringRecord
#include "Token.h"		// Token

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class StringDB
{
public:
	StringDB();
    virtual ~StringDB();

	// Modifiers
	bool					InsertStr
	(
		MBCHAR const *			add_id,
		MBCHAR const *			new_text
	);
	bool					Parse(MBCHAR * filename);

	// Accessors
	MBCHAR *				GetIdStr
	(
		StringId const &		index
	) const;
	MBCHAR const *			GetNameStr(StringId const & n) const;
	MBCHAR const *			GetNameStr(MBCHAR const * s) const;
	bool					GetStringID
	(
		MBCHAR const *			str_id,
		StringId &				index
	) const;
	bool					GetText
	(
		MBCHAR const *			get_id,
		MBCHAR **				new_text
	) const;

	void Export(MBCHAR * file);

private:
	std::vector<StringRecord *>	m_all;	// a flattened list version of m_head
	std::vector<StringRecord *> m_head;
		// hash vector of B-trees of lexicographically ordered strings

	bool					AddStrNode
	(
		StringRecord * &		ptr,
		MBCHAR const *			add_id,
		MBCHAR const *			new_text,
		StringRecord * &		newPtr
	);
	void					AssignIndex(StringRecord * & ptr);
	void					Btree2Array(void);
	StringRecord * &		GetHead(MBCHAR const * id);
	StringRecord const * const &
							GetHead(MBCHAR const * id) const;
	bool					GetIndexNode
	(
		StringRecord const *	ptr,
		MBCHAR const *			str_id,
		StringId &				index
	) const;
	bool					GetStrNode
	(
		StringRecord const *	ptr,
		MBCHAR const *			add_id,
		MBCHAR **				new_text
	) const;

	bool					ParseAStringEntry            (Token *strToken);
	bool					ParseAStringEntryNoDuplicates(Token *strToken);
};

#endif	// Multiple include guard
