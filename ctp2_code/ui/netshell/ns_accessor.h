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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Corrected strange access of non-static members from static data.
//
//----------------------------------------------------------------------------

#ifndef __NS_ACCESSOR_H__
#define __NS_ACCESSOR_H__

#include <vector>























template<class T>
class ns_Accessor {
public:
	typedef void* (T::*Data);
	enum Type {
		INT,
		STRING,
		ICON,
		ERR
	};
	struct Struct 
	{
		Struct(Type t, void * d)
		:	type(t),
			data(d)
		{
		};

		Type	type;
		void *	data;
	};

    std::vector<Struct>	list;

	ns_Accessor()
	:	list()
	{
	};

	Type type(size_t i) const
	{
		return (i < list.size()) ? list[i].type : ERR;
	};

	void * data(size_t i) const
	{
		return (i < list.size()) ? list[i].data : NULL;
	};
	int comp(int i, T *p) {
		Type t = ((T *)this)->type(i);
		void *a = ((T *)this)->data(i);
		void *b = p->data(i);
		switch(t) {
		case INT:
			return (int)a - (int)b;
		case STRING:
		case ICON:
			if(a) {
				if(b)
					return strcmp((char *)a, (char *)b);
				else
					return 1;
			} else {
				if(b)
					return -1;
				else
					return 0;
			}
		}
		return 0;
	}
};


#endif 
