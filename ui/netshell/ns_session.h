










#ifndef __NS_SESSION_H__
#define __NS_SESSION_H__

#include "ns_object.h"


class ns_Session : public ns_Object<NETFunc::Session, ns_Session>
{
public:
	
	ns_Session( NETFunc::Session *session ) : ns_Object<NETFunc::Session, ns_Session>(session) {};
	
	void Update( NETFunc::Session *session );

	char *m_locked;	
	char *m_closed;
	char *m_name;
	int m_players;
};


#endif 
