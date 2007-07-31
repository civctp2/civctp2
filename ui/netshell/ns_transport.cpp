










#include "c3.h"

#include "ns_transport.h"



int ns_Transport::count = 1;
ns_Transport::Struct ns_Transport::list[] = {
	{STRING,	(Data)&m_name},
};


void ns_Transport::Update( NETFunc::Transport *transport ) {
	SetMine(false);
	m_name = transport->GetName();
}

