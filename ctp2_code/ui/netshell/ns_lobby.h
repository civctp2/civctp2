










#ifndef __NS_LOBBY_H__
#define __NS_LOBBY_H__

#include "ns_object.h"


class ns_Lobby : public ns_Object<NETFunc::Lobby, ns_Lobby>
{
public:
	
	ns_Lobby( NETFunc::Lobby *lobby ) : ns_Object<NETFunc::Lobby, ns_Lobby>(lobby) {};
	
	void Update( NETFunc::Lobby *lobby );

	char *m_closed;
	char *m_name;
	int m_players;
};


#endif __NS_LOBBY_H__
