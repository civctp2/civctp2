#include "c3.h"
#include "Regard.h"
#include "CivArchive.h"
#include "Player.h"
#include "StrDB.h"
extern	StringDB	*g_theStringDB ;





#include "Player.h"
extern Player **g_player; 

#include "network.h"

Regard::Regard()
{

    sint32 player_idx; 
    for (player_idx=0; player_idx<k_MAX_PLAYERS; player_idx++) { 
        m_regard[player_idx]=REGARD_TYPE_NEUTRAL; 
    } 
}


void Regard::SetForPlayer(const PLAYER_INDEX player, const REGARD_TYPE regard)
{
	Assert((player>=0) && (player<=k_MAX_PLAYERS)) ;
	Assert((REGARD_TYPE_INSANE_HATRED <= regard) && (regard<=REGARD_TYPE_LOVE)) ;

   
	m_regard[player] = regard ;
}


REGARD_TYPE Regard::GetForPlayer(const PLAYER_INDEX player)
{
	Assert((player>=0) && (player<=k_MAX_PLAYERS)) ;

	return (m_regard[player]) ;
}


REGARD_TYPE Regard::GetUpdatedRegard(const PLAYER_INDEX me,
    const PLAYER_INDEX him) 
{ 
    Assert(0<= me); 
    Assert(me < k_MAX_PLAYERS); 
    Assert(0<=him); 
    Assert(him<k_MAX_PLAYERS); 

    if (Player::IsThisPlayerARobot(me) && !g_network.IsClient()) {  

        
        
        
		


















        return m_regard[him];
    } else { 
		
		
		
		
        switch (g_player[me]->GetDiplomaticState(him)) { 
        case DIPLOMATIC_STATE_WAR: 
            return REGARD_TYPE_HOTWAR;
        case DIPLOMATIC_STATE_CEASEFIRE: 
            return REGARD_TYPE_NEUTRAL; 
        case DIPLOMATIC_STATE_NEUTRAL: 
            return REGARD_TYPE_NEUTRAL;
        case DIPLOMATIC_STATE_ALLIED:
            return REGARD_TYPE_LOVE; 
        default:
            Assert(0); 
             return REGARD_TYPE_NEUTRAL;
        } 
    }
} 


	












void Regard::Serialize(CivArchive &archive)
	{
	if (archive.IsStoring())
		archive.StoreChunk((uint8 *)&m_regard, ((uint8 *)&m_regard)+sizeof(m_regard));

	else
		archive.LoadChunk((uint8 *)&m_regard, ((uint8 *)&m_regard)+sizeof(m_regard));


	}














uint32 Regard_Regard_GetVersion(void)
	{
	return (k_REGARD_VERSION_MAJOR<<16 | k_REGARD_VERSION_MINOR);
	}
