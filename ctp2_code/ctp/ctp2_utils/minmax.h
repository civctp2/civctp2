//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Contains the definitions of std::min and std::max which are
//                missing in MSVC6
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
// Modifications from the original Activision code:
//
// - Created file (31-Mar-2007 Martin Gühmann)
// - Moved min and max from c3.h so that we can use it also in the other 
//   projects so that we don't have to include everything from c3,h.
//
//----------------------------------------------------------------------------
//
//  Remarks
//
// - I do not know whether the location of this file is right, maybe it should
//   be put into the compiler subdirectory.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __MINMAX__H__
#define __MINMAX__H__ 1

#if defined(_MSC_VER) && (_MSC_VER < 1300)
	// MSVC 6.0 does not have std::min and std::max.
	namespace std
	{
		template <typename T>
		inline T const & min(T const & a, T const & b)
		{
			return (a < b) ? a : b;
		};

		template <typename T>
		inline T const & max(T const & a, T const & b)
		{
			return (a < b) ? b : a;
		};
	};	// namespace std
#endif	// _MSC_VER < 1300

#endif

