







#include "c3.h"
#include "Unit.h"
#include "Player.h"
#include "MessageData.h"
#include "Message.h"
#include "MessagePool.h"

#include "network.h"
#include "CivArchive.h"
#include "Globals.h"









MessagePool::MessagePool() : ObjPool(k_BIT_GAME_OBJ_TYPE_MESSAGE)
	{
	}









MessagePool::MessagePool(CivArchive &archive) : ObjPool(k_BIT_GAME_OBJ_TYPE_MESSAGE)
	{
	Serialize(archive) ;
	}












void MessagePool::Serialize(CivArchive &archive)
	{
	MessageData	*newData ;

	sint32	i,
			count = 0 ;

#define MSGPOOL_MAGIC 0xAF439F4E

    CHECKSERIALIZE

    if (archive.IsStoring()) { 
		archive.PerformMagic(MSGPOOL_MAGIC) ;
		ObjPool::Serialize(archive);
        for (i=0; i<k_OBJ_POOL_TABLE_SIZE; i++) { 
            if(m_table[i]) { 
				count++;
            }
        }

		archive<<count;
        for(i = 0; i < k_OBJ_POOL_TABLE_SIZE; i++) { 
            if(m_table[i]) { 
				((MessageData *)(m_table[i]))->Serialize(archive) ;
            }
        }
    } else {
		archive.TestMagic(MSGPOOL_MAGIC) ;
		ObjPool::Serialize(archive);

		archive>>count;
		for (i=0; i<count; i++)
			{
			newData = new MessageData(archive) ;
			Insert(newData) ;
			}

		}

	}

	







Message MessagePool::Create(PLAYER_INDEX owner, PLAYER_INDEX sender, MESSAGE_TYPE type, MBCHAR *msg)
	{
	MessageData* newData;

	Message newRequest(NewKey(k_BIT_GAME_OBJ_TYPE_MESSAGE));
	
	newData = new MessageData(newRequest, owner, sender, type, msg) ;
	Insert(newData) ;

	g_player[owner]->AddMessage(newRequest) ;						
	DoNetwork(newData);

	return (newRequest) ;
	}












Message MessagePool::Create(PLAYER_INDEX owner, MBCHAR *msg)
	{
	MessageData* newData;

	Message newMessage(NewKey(k_BIT_GAME_OBJ_TYPE_MESSAGE));
	
	newData = new MessageData(newMessage, owner, PLAYER_INDEX_INVALID, 0, msg) ;
	Insert(newData) ;

	g_player[owner]->AddMessage(newMessage) ;						

	DoNetwork(newData);

	return (newMessage) ;
	}

Message MessagePool::Recreate(PLAYER_INDEX owner, MBCHAR *msg, MBCHAR *title)
	{
	MessageData* newData;

	Message newMessage(NewKey(k_BIT_GAME_OBJ_TYPE_MESSAGE));
	
	newData = new MessageData(newMessage, owner, PLAYER_INDEX_INVALID, 0, msg) ;
	Insert(newData) ;

	if(title)
		newData->SetTitle(title);

	g_player[owner]->AddMessage(newMessage) ;						

	DoNetwork(newData);

	return (newMessage) ;
	}


Message MessagePool::Create(PLAYER_INDEX owner, MessageData *copy)
{
	MessageData *newData;
	Message newMessage(NewKey(k_BIT_GAME_OBJ_TYPE_MESSAGE));
	newData = new MessageData(newMessage, copy);
	newData->SetOwner(owner);
	Insert(newData);
	g_player[owner]->AddMessage(newMessage);

	
	
	
	return newMessage;
}











Message MessagePool::ServerCreate()
{
	MessageData *newData;
	Message newRequest(NewKey(k_BIT_GAME_OBJ_TYPE_MESSAGE));
	newData = new MessageData(newRequest);
	Insert(newData);
	return newRequest;
}

void MessagePool::DoNetwork(MessageData *newData)
{
	
	return;

	if(g_network.IsClient()) {
		Assert(newData->GetSender() == g_network.GetPlayerIndex() ||
			   newData->GetSender() == PLAYER_INDEX_INVALID);
		if(newData->GetSender() == g_network.GetPlayerIndex() ||
			newData->GetSender() == PLAYER_INDEX_INVALID) {
			g_network.AddCreatedObject(newData);
			g_network.SendMessage(newData);
		}
	} else if(g_network.IsHost()) {
		if(newData->GetSender() != PLAYER_INDEX_INVALID) {
			g_network.Block(newData->GetSender());
		}
		g_network.Enqueue(newData);
		if(newData->GetSender() != PLAYER_INDEX_INVALID) {
			g_network.Unblock(newData->GetSender());
		}
	}
}

void MessagePool::NotifySlicReload()
{
	sint32 i;
	for(i = 0; i < k_OBJ_POOL_TABLE_SIZE; i++) { 
		if(m_table[i]) { 
			((MessageData *)(m_table[i]))->NotifySlicReload();
		}
	}
}
