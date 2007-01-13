









#pragma once
#ifndef _DIPLOMATICREQUESTPOOL_H_
#define _DIPLOMATICREQUESTPOOL_H_

#include "ObjPool.h"

#include "DiplomaticRequest.h"

enum REQUEST_TYPE ;

class DiplomaticRequestData;
class MessageDynamicArray ;

class DiplomaticRequestPool : public ObjPool
	{
		
		

		
		

		
		
		

		
		

	public:
		DiplomaticRequestData* AccessDiplomaticRequest(const DiplomaticRequest id)
			{
			return ((DiplomaticRequestData*)Access(id)) ;
			}

		DiplomaticRequestData* GetDiplomaticRequest(const DiplomaticRequest id) const
			{
			return ((DiplomaticRequestData*)Get(id)) ;
			}

		DiplomaticRequestPool() ;
		DiplomaticRequestPool(CivArchive &archive) ;

		DiplomaticRequest Create(PLAYER_INDEX owner, PLAYER_INDEX recipient, REQUEST_TYPE request) ;
		DiplomaticRequestData *CreateData();

		void EndTurn(const PLAYER_INDEX sender) ;
		void EndTurn(DiplomaticRequestData *top, const PLAYER_INDEX sender,
					 MessageDynamicArray &msgExpired);
		void Serialize(CivArchive &archive) ;

	} ;

extern DiplomaticRequestPool* g_theDiplomaticRequestPool ;
#else

class DiplomaticRequestPool ;

#endif
