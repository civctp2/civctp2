










#ifndef __NS_GPLAYER_H__
#define __NS_GPLAYER_H__

#include "ns_object.h"


class ns_GPlayer : public ns_Object<NETFunc::Player, ns_GPlayer>
{
public:
	
	ns_GPlayer( NETFunc::Player *player ) : ns_Object<NETFunc::Player, ns_GPlayer>(player) {}
	
	void Update( NETFunc::Player *player );

	char *m_host;
	char *m_launched;
	char *m_name;
	int m_ping;
	char *m_tribe;

	int m_civpoints;
	int m_pwpoints;
};

#endif 
