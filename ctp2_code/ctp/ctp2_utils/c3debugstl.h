//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Debug allocator for the STL classes.
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
//       is implied: the (_MSC_VER) preprocessor directive lines and the blocks 
//       between #else and #endif are modified Apolyton code. The blocks that
//       are active for _MSC_VER value 1200 are the original Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Marked MS version specific code.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __C3DEBUGSTL_H__
#define __C3DEBUGSTL_H__

#include <memory>


#if defined(_MSC_VER) && (_MSC_VER < 1300)
template<class _Ty>
class dbgallocator : public std::allocator<_Ty> 
{
public:
    typedef std::allocator<_Ty> BaseAllocator;
    
	pointer allocate(size_type _N, const void *buf)
	{ 
        if (_N > 0)
        {
            return BaseAllocator::allocate(_N, buf);
        }
	    else
	    {
            // MSVC6 std::allocator crashes when passing 0 as size
		    buf = NULL;
		    return (pointer) buf;
	    }
	};
	
       void deallocate(void _FARQ *_P, size_type _N)
       {
        if (_P)
        {
            BaseAllocator::deallocate(_P, _N);
        }
    }
};
#else
// TODO: Make the debug allocator std::allocator compliant.
// Workaround: use the standard allocator.
#define dbgallocator std::allocator
#endif


#endif // __C3DEBUGSTL_H__
