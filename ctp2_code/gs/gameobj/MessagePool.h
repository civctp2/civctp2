#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _MESSAGEPOOL_H_
#define _MESSAGEPOOL_H_

class MessagePool;
typedef sint32 MESSAGE_TYPE;

#include "civarchive.h"	// CivArchive
#include "message.h"	// MessageData
#include "ObjPool.h"	// ObjPool
#include "player.h"		// PLAYER_INDEX

#define k_HACK_RECONSTITUTED_CLASS 0x72adc754

class MessagePool : public ObjPool
{
public:
		MessageData* AccessMessage(const Message id)
			{
			return ((MessageData*)Access(id)) ;
			}

		MessageData* GetMessage(const Message id) const
			{
			return ((MessageData*)Get(id)) ;
			}

		MessagePool() ;
		MessagePool(CivArchive &archive) ;

		Message Create(PLAYER_INDEX owner, PLAYER_INDEX sender, MESSAGE_TYPE type, MBCHAR *msg) ;
		Message Create(PLAYER_INDEX owner, MBCHAR *msg) ;
		Message Create(PLAYER_INDEX owner, MessageData *copy);
		Message Recreate(PLAYER_INDEX owner, MBCHAR *msg, MBCHAR *title);
		Message ServerCreate();
		void Serialize(CivArchive &archive) ;
		void DoNetwork(MessageData *newData);

		void NotifySlicReload();
	} ;

extern MessagePool *	g_theMessagePool;

#endif
