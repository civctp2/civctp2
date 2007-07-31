










#ifndef __NS_AIPLAYER_H__
#define __NS_AIPLAYER_H__

#include "ns_object.h"


class ns_AIPlayer : public ns_Object<NETFunc::AIPlayer, ns_AIPlayer>
{
public:
	
	ns_AIPlayer( NETFunc::AIPlayer *aiplayer ) : ns_Object<NETFunc::AIPlayer, ns_AIPlayer>(aiplayer) {};
	
	void Update( NETFunc::AIPlayer *aiplayer );

	char *m_name;

	char *m_tribe;

	int m_civpoints;
	int m_pwpoints;
};

#endif 
