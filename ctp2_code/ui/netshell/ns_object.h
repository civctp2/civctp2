










#ifndef __NS_OBJECT_H__
#define __NS_OBJECT_H__

#include "netfunc.h"
#include "ns_accessor.h"

template <class T, class NetShellT>

class ns_Object : public ns_Accessor<NetShellT>, public NETFunc::Key
{
	T *nf_object;
	bool mine;
public:
	
	ns_Object( T *t ):Key(t) {
		nf_object = t;
		mine = false;
		((NetShellT *)this)->Update(t);
	}







	bool IsMine() {
		return mine;
	}

	void SetMine(bool m) {
		mine = m;
	}

	
	T *GetNETFuncObject(void) {
		return nf_object;
	}
	
	void Update(T *t) {
		mine = false;
	};
	virtual ~ns_Object() { 
		nf_object = 0;
	}

	
	void SetKey( void ) {}
};


#endif 
