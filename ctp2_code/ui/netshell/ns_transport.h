










#ifndef __NS_TRANSPORT_H__
#define __NS_TRANSPORT_H__

#include "ns_object.h"


class ns_Transport : public ns_Object<NETFunc::Transport, ns_Transport>
{
public:
	
	ns_Transport( NETFunc::Transport *transport ) : ns_Object<NETFunc::Transport, ns_Transport>(transport) {};
	
	void Update( NETFunc::Transport *transport );

	char *m_name;
};


#endif 
