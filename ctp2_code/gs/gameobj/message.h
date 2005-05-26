






#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __MESSAGE_H__ 
#define __MESSAGE_H__

#include "ID.h"
#include "MessageData.h"

enum MESSAGE_RESPONSE_TYPE ;
typedef sint32 AdvanceType;

class Message : public ID
	{
	public:

		
		
		Message() : ID() { return ; }
		Message(sint32 val) : ID(val) { return ; }
		Message(uint32 val) : ID(val) { return ; }
#ifdef _MSC_VER
		Message(const int val) : ID(val) { return ; }
		Message(const unsigned int val) : ID(val) { return ; }
#endif

		void KillMessage() ;
		void Kill() { KillMessage() ; }
		void MessageRead() { KillMessage() ; }

		void RemoveAllReferences() ;

		void MinimizeMessage();
		void Minimize() {MinimizeMessage();}

		const MessageData* GetData() const ;
		MessageData* AccessData() ;

		void Castrate() {} ;
		PLAYER_INDEX GetOwner(void) const { return (GetData()->GetOwner()) ; }
		PLAYER_INDEX GetSender(void) const { return (GetData()->GetSender()) ; }
		MESSAGE_TYPE GetMsgType(void) const { return (GetData()->GetMsgType()) ; }
		MBCHAR *GetText(void) { return (AccessData()->GetMsgText()) ; }
		void ToString(MBCHAR *s) { AccessData()->ToString(s) ; }
		MESSAGE_RESPONSE_TYPE Reject(void) { return (AccessData()->Reject()) ; }
		MESSAGE_RESPONSE_TYPE Accept(void) { return (AccessData()->Accept()) ; }
		void Dump(const sint32 i) { AccessData()->Dump(i) ; }
		void Show();

		void SetSelectedAdvance(AdvanceType advance);
		AdvanceType GetSelectedAdvance() const;

		void SetDuration(sint32 duration);
		sint32 GetExpiration() const;

		void SetIsHelpBox() { AccessData()->SetIsHelpBox(); }
		BOOL IsHelpBox() const { return GetData()->IsHelpBox(); }

		void SetIsAlertBox() { AccessData()->SetIsAlertBox(); }
		BOOL IsAlertBox() const { return GetData()->IsAlertBox(); }

		void SetIsInstantMessage() { AccessData()->SetIsInstantMessage(); }
		BOOL IsInstantMessage() const { return GetData()->IsInstantMessage(); }

		BOOL IsRead() const { return GetData()->IsRead(); }
		void SetRead() { ((MessageData*)GetData())->SetRead(); }

		BOOL IsDiplomaticResponse() const { return GetData()->IsDiplomaticResponse(); }

		void SetClass(sint32 mclass) { AccessData()->SetClass(mclass); }
		sint32 GetClass() const { return GetData()->GetClass(); }

		void SetUseDirector() { AccessData()->SetUseDirector(); }
		BOOL UseDirector() const { return GetData()->UseDirector(); }
	} ;

#else

class Message ;

#endif
