











#pragma once
#ifndef __C3DEBUGSTL_H__
#define __C3DEBUGSTL_H__

#include <memory>



template<class _Ty>
class dbgallocator : public std::allocator<_Ty> {
public:
	pointer allocate(size_type _N, const void *buf)
	{ if (_N)
		return allocator<_Ty>::allocate(_N, buf);
	  else
	  {
		  buf = NULL;
		  return (pointer) buf;
	  }
	}
	
	void deallocate(void _FARQ *_P, size_type)
		{if (_P) operator delete(_P); }
};


#endif __C3DEBUGSTL_H__
