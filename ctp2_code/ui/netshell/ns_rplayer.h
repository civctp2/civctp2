










#ifndef __NS_RPLAYER_H__
#define __NS_RPLAYER_H__

#include "ns_object.h"


class ns_RPlayer : public ns_Object<NETFunc::Player, ns_RPlayer>
{
public:
	
	ns_RPlayer( NETFunc::Player *player ) : ns_Object<NETFunc::Player, ns_RPlayer>(player) {};
	
	void Update( NETFunc::Player *player );

	char *m_name;
	int m_ping;
};

#endif 
