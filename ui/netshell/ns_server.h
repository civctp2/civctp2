










#ifndef __NS_SERVER_H__
#define __NS_SERVER_H__

#include "ns_object.h"


class ns_Server : public ns_Object<NETFunc::Server, ns_Server>
{
public:
	
	ns_Server( NETFunc::Server *server ) : ns_Object<NETFunc::Server, ns_Server>(server) {};
	
	void Update( NETFunc::Server *server );

	char *m_name;
	int m_players;
	int m_ping;
};


#endif 
