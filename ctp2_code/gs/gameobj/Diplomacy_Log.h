#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __DIPLOMACY_LOG_H__
#define __DIPLOMACY_LOG_H__ 1

class Diplomacy_Log;

#include "AgreementTypes.h"
#include "player.h"
#include "DiplomaticTypes.h"
class MapPoint; 


#define k_REGARD_STACK_MAX 10

class Diplomacy_Log {

	char m_filename[80]; 
	uint32 m_player_bit_mask;

    sint32 m_regard_stack_idx; 
    sint32 m_regard_stack[k_REGARD_STACK_MAX][2]; 

public:

	Diplomacy_Log(); 
	~Diplomacy_Log(); 

	void LogAllPlayers(); 
	void UnlogAllPlayers(); 
	void LogPlayer(const sint32 player_idx);
	void UnlogPlayer(const sint32 player_idx);
    BOOL IsPlayerLogged(const sint32 player_idx);

    void BeginRound();
	void BeginTurn(); 
	void EndTurn(); 

    void LogStr(const sint32 player_idx, char *str);
    void LogHostileAction(const UNIT_ORDER_TYPE order_type, const sint32 target_player, 
        const sint32 target_id,  const sint32 owner, const uint32 unit_id, const MapPoint &norm_pos);

    void LogRegard(const sint32 the_regarder, const sint32 the_regardee); 
    void PushRegardReqest(const sint32 regarder, const sint32 regardee); 
    void PopRegardRequest(); 

    void LogMakeAgreement(const sint32 owner, const sint32 recipient, const sint32 thirdParty,
                const AGREEMENT_TYPE agreement); 
    void LogBrokenAgreement(const sint32 owner, const sint32 recipient, const sint32 thirdParty,
                const AGREEMENT_TYPE agreement); 

    void LogRequestCreated(const sint32 owner, const sint32 recipient, const REQUEST_TYPE request); 

    void Request2String(REQUEST_TYPE request, char astr[80]);
    void LogTone(const sint32 o, const sint32 r, const ATTITUDE_TYPE att);

    void LogEnact(const sint32 sender, const sint32 reciever, const REQUEST_TYPE request);
    void LogReject(const sint32 sender, const sint32 reciever, const REQUEST_TYPE request);

};

#endif // __DIPLOMACY_LOG_H__
