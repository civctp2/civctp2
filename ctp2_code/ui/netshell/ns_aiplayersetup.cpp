









#include "c3.h"
#include "ns_aiplayersetup.h"


sint32 nf_AIPlayer::m_version = 102;


nf_AIPlayer::nf_AIPlayer(void):NETFunc::AIPlayer() {
	SetTribe(0);
	SetCivPoints(0);
	SetPwPoints(0);
}

nf_AIPlayer::nf_AIPlayer(NETFunc::AIPlayer *p):NETFunc::AIPlayer(*p) {
	SetTribe(0);
	SetCivPoints(0);
	SetPwPoints(0);

}

void nf_AIPlayer::SetKey(NETFunc::KeyStruct *k) {
	memcpy(&key, k, sizeof(NETFunc::KeyStruct));
	SetTribe(0);
	SetCivPoints(0);
	SetPwPoints(0);
}

void nf_AIPlayer::Pack()
{
	AIPlayer::Pack();
	Push( m_tribe );
	Push( m_civPoints );
	Push( m_pwPoints );
}

void nf_AIPlayer::Unpack()
{
	AIPlayer::Unpack();
	Pop( m_tribe );
	Pop( m_civPoints );
	Pop( m_pwPoints );
}

void nf_AIPlayer::Reset()
{
	SetTribe( 0 );
	SetCivPoints( 0 );
	SetPwPoints( 0 );
}



int ns_AIPlayerSetup::count = 1;
ns_AIPlayerSetup::Struct ns_AIPlayerSetup::list[] = {
	{STRING,	(Data)&m_name}
};


void ns_AIPlayerSetup::Update( NETFunc::AIPlayer *aiplayer ) {
	m_name = aiplayer->GetName();
}
