










#ifndef __NS_PLAYER_H__
#define __NS_PLAYER_H__

#include "ns_object.h"


class ns_Player : public ns_Object<NETFunc::Player, ns_Player>
{
public:
	
	ns_Player( NETFunc::Player *player ) : ns_Object<NETFunc::Player, ns_Player>(player) {};
	
	void Update( NETFunc::Player *player );

	char *m_mute;
	char *m_name;
	int m_ping;
};


#endif 
