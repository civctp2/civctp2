










#ifndef __NS_GAME_H__
#define __NS_GAME_H__

#include "ns_object.h"


class ns_Game : public ns_Object<NETFunc::Game, ns_Game>
{
public:
	
	ns_Game( NETFunc::Game *game ) : ns_Object<NETFunc::Game, ns_Game>(game) {};
	
	void Update( NETFunc::Game *game );

	char *m_locked;	
	char *m_closed;
	char *m_launched;
	char *m_name;

	int m_players;
};


#endif 
