










#ifndef __NS_AIPLAYERSETUP_H__
#define __NS_AIPLAYERSETUP_H__

#include "ns_object.h"

class nf_AIPlayer:public NETFunc::AIPlayer {
public:

	static sint32 m_version;

	nf_AIPlayer(void);
	nf_AIPlayer(NETFunc::AIPlayer *p);

	void SetKey(NETFunc::KeyStruct *k);

	void Pack();
	void Unpack();

	
	void Reset();

	int	GetTribe() const { return m_tribe; }
	void SetTribe( int tribe )
	{ m_tribe = tribe; }

	int	GetCivPoints() const { return m_civPoints; }
	void SetCivPoints( int civPoints )
	{ m_civPoints = civPoints; }

	int	GetPwPoints() const { return m_pwPoints; }
	void SetPwPoints( int pwPoints )
	{ m_pwPoints = pwPoints; }

protected:
	int m_tribe;
	int m_civPoints;
	int m_pwPoints;
};


class ns_AIPlayerSetup : public ns_Object<NETFunc::AIPlayer, ns_AIPlayerSetup>
{
public:
	
	ns_AIPlayerSetup( NETFunc::AIPlayer *aiplayer ) : ns_Object<NETFunc::AIPlayer, ns_AIPlayerSetup>(aiplayer) {};
	
	void Update( NETFunc::AIPlayer *aiplayer );

	char *m_name;
};

#endif 
