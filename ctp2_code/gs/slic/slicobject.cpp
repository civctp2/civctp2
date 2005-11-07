

#include "c3.h"
#include "SlicObject.h"
#include "SlicEngine.h"
#include "SlicSegment.h"
#include "dynarr.h"
#include "UnitDynArr.h"
#include "MapPoint.h"
#include "Unit.h"
#include "Civilisation.h"
#include "Advances.h"
#include "SlicFrame.h"
#include "MessageData.h"
#include "MessagePool.h"
#include "player.h"
#include "SelItem.h"
#include "SlicButton.h"
#include "network.h"
#include "civapp.h"
#include "messagewin.h"
#include "TradeBids.h"
#include "messagewindow.h"

#include "stringutils.h"

#include "TurnCnt.h"
#include "Globals.h"

#include "controlpanelwindow.h"
extern ControlPanelWindow *g_controlPanel;

extern TurnCount		*g_turn;

extern Player			**g_player;
extern SelectedItem		*g_selected_item;
extern CivApp			*g_civApp;

#ifdef _PLAYTEST
sint32 g_robotMessages = FALSE;
#endif

sint32 g_hackInstantMessages = 0;	

SlicObject::SlicObject()
{
	m_index = -1;
	m_refCount = 0;
	m_id = NULL;
	m_recipientList = NULL;
	m_frame = NULL;
	m_request = NULL;
    m_seconds = 1; 
    m_defaultAdvance = -1; 
    m_segment = NULL;
    m_request = NULL;
	m_aborted = FALSE;
	m_instantMessage = FALSE;
	m_class = k_NON_TUTORIAL_MESSAGE_CLASS;
	m_dontSave = FALSE;
	m_closeDisabled = FALSE;
	m_isDiplomaticResponse = FALSE;
	m_useDirector = FALSE;
}

SlicObject::SlicObject(char const * id)
{
    m_seconds = 1; 
    m_defaultAdvance = -1; 
    m_request = NULL;

	m_id = new char[strlen(id) + 1];
	strcpy(m_id, id);

	m_recipientList = NULL;
	m_numRecipients = 0;
	m_defaultAdvanceSet = FALSE;
	
	m_segment = g_slicEngine->GetSegment(m_id);
	m_frame = new SlicFrame(m_segment);
	m_refCount = 0;
	m_request = new ID;
	m_aborted = FALSE;
	m_instantMessage = FALSE;
	if(m_segment && !m_segment->IsHelp()) {
		m_class = k_NON_TUTORIAL_MESSAGE_CLASS;
	} else {
		m_class = k_NON_TUTORIAL_HELP_CLASS;
	}

	m_dontSave = FALSE;
	m_closeDisabled = FALSE;
	m_isDiplomaticResponse = FALSE;
	m_useDirector = FALSE;
}

SlicObject::SlicObject(SlicSegment *segment)
{
    m_seconds = 1; 
    m_defaultAdvance = -1; 
	m_id = new char[strlen(segment->GetName()) + 1];
	strcpy(m_id, segment->GetName());
	
	m_recipientList = NULL;
	m_numRecipients = 0;

	m_segment = segment;
	m_frame = new SlicFrame(m_segment);
	m_refCount = 0;
	m_defaultAdvanceSet = FALSE;
	m_request = new ID;
	m_aborted = FALSE;
	m_instantMessage = FALSE;
	if(m_segment && !m_segment->IsHelp()) {
		m_class = k_NON_TUTORIAL_MESSAGE_CLASS;
	} else {
		m_class = k_NON_TUTORIAL_HELP_CLASS;
	}
	m_dontSave = FALSE;
	m_closeDisabled = FALSE;
	m_isDiplomaticResponse = FALSE;
	m_useDirector = FALSE;
}

SlicObject::SlicObject(SlicSegment *segment, SlicObject *copy) 
	: SlicContext(copy)
{
    m_seconds = 1; 
	m_id = new char[strlen(segment->GetName()) + 1];
	strcpy(m_id, segment->GetName());
	
	
	
	
	
	
	
	m_recipientList = NULL;
	m_numRecipients = 0;

	m_segment = segment;
	m_frame = new SlicFrame(m_segment);
	m_refCount = 0;
	m_defaultAdvanceSet = copy->m_defaultAdvanceSet;
	m_defaultAdvance = copy->m_defaultAdvance;
	m_request = new ID(*copy->m_request);

	
	m_aborted = FALSE;
	m_instantMessage = copy->m_instantMessage; 
	m_class = copy->m_class;
	m_dontSave = copy->m_dontSave;
	m_closeDisabled = copy->m_closeDisabled;
	m_isDiplomaticResponse = copy->m_isDiplomaticResponse;
	m_useDirector = copy->m_useDirector;
}

SlicObject::SlicObject(char const * id, SlicContext *copy) 
	: SlicContext(copy)
{
    m_seconds = 1; 
    m_defaultAdvance = -1; 

	m_id = new char[strlen(id) + 1];
	strcpy(m_id, id);

	m_recipientList = NULL;
	m_numRecipients = 0;
	m_defaultAdvanceSet = FALSE;
	
	m_segment = g_slicEngine->GetSegment(m_id);
	m_frame = new SlicFrame(m_segment);
	m_refCount = 0;
	m_request = new ID;
	m_aborted = FALSE;
	m_instantMessage = FALSE;
	if(m_segment && !m_segment->IsHelp()) {
		m_class = k_NON_TUTORIAL_MESSAGE_CLASS;
	} else {
		m_class = k_NON_TUTORIAL_HELP_CLASS;
	}
	m_dontSave = FALSE;
	m_closeDisabled = FALSE;
	m_isDiplomaticResponse = FALSE;
	m_useDirector = FALSE;
}

SlicObject::SlicObject(CivArchive &archive)
{

	
	
    m_segment = NULL;
    m_request = NULL;
	m_id = NULL;
	m_recipientList = NULL;
	m_request = new ID;
	
	
	m_frame = NULL;

	Serialize(archive);
}

SlicObject::~SlicObject()
{
	
	
	m_refCount = 0x7fffffff; 

	delete [] m_id;
	delete [] m_recipientList;
	delete m_frame;
	delete m_request;
}

sint32 SlicObject::AddRef()
{
	Assert(m_refCount >= 0);
	return ++m_refCount;
}

sint32 SlicObject::Release()
{
	--m_refCount;
	if(m_refCount <= 0) {
		Assert(m_refCount == 0);
		if(m_refCount == 0)
			delete this;
		return 0;
	}
	return m_refCount;
}

BOOL SlicObject::IsValid()
{
	return m_segment != NULL;
}

void SlicObject::AddRecipient(const PLAYER_INDEX recip)
{
	m_recipientList = Expand(m_recipientList, m_numRecipients);
	m_recipientList[m_numRecipients++] = recip;
}

void SlicObject::AddAllRecipients()
{
	for(sint32 i = 0; i < k_MAX_PLAYERS; i++) {
		if(g_player[i] && (!g_player[i]->m_isDead)) {
			m_recipientList = Expand(m_recipientList, m_numRecipients);
			m_recipientList[m_numRecipients++] = i;
		}
	}
}

void SlicObject::AddAllRecipientsBut(PLAYER_INDEX loser1, PLAYER_INDEX loser2)
{
	for(sint32 i = 0; i < k_MAX_PLAYERS; i++) {
		if(g_player[i] && (!g_player[i]->m_isDead) &&
           (i != loser1) && (i != loser2)) {
			m_recipientList = Expand(m_recipientList, m_numRecipients);
			m_recipientList[m_numRecipients++] = i;
		}
	}
}

sint32 SlicObject::GetRecipient(const PLAYER_INDEX recip) const
{
	if(!m_recipientList)
		return -1;
	if(recip >= m_numRecipients)
		return -1;
	return m_recipientList[recip];
}

void SlicObject::Execute()
{
	Assert(!g_slicEngine->AtBreak());
	if(g_slicEngine->AtBreak())
		return;

	if(!m_frame) {
		m_frame = new SlicFrame(m_segment);
	}

	if(m_segment->GetType() == SLIC_OBJECT_MESSAGEBOX) {
		m_frame->ClearMessageData();
	} else {
		m_segment->AddSpecials(this);
	}

	if(m_frame->GetMessageData()) {
		char buf[1024];
		sprintf(buf, "%s", m_segment->GetName());
		m_frame->GetMessageData()->SetMsgText(buf);
	}
	m_frame->Run();

	if(g_slicEngine->AtBreak()) {
		
		
		
		return;
	}

	
	Finish();
}

//----------------------------------------------------------------------------
//
// Name       : Finish
//
// Description: Displays any messagebox defined by this SlicObject
//              and then deletes the associated SlicFrame
//
// Parameters : -
//
// Globals    : Unknown
//
// Returns    : -
//
// Remark(s)  : This gets the MessageData object from m_frame, finishes
//              initializing its fields and uses it to construct a
//              Message object which is then added to g_theMessagePool.
//              
//              It appears to do this once for each recipient, and it seems
//              to use the same MessageData object every time, so a great
//              deal of the work could be superfluous, and could be moved
//              out of the for loop.
//
//----------------------------------------------------------------------------
void SlicObject::Finish() 
{
	if(m_segment->GetType() == SLIC_OBJECT_MESSAGEBOX) {
		
		if (m_numRecipients == 0 && !g_civApp->IsGameLoaded()) {
			
			MessageData *messageData = m_frame->GetMessageData();

			if(m_segment->IsHelp()) {
				messageData->SetIsHelpBox();
			}
			
			MessageData *newData;
			Message newMessage(g_theMessagePool->NewKey(k_BIT_GAME_OBJ_TYPE_MESSAGE));
			newData = new MessageData(newMessage, messageData);
			newData->SetOwner(0);
			newData->SetSlicSegment(m_segment);
			g_theMessagePool->Insert(newData);

			if(newMessage.IsAlertBox()) {
				messagewin_CreateModalMessage(newMessage);
			} else {
				messagewin_CreateMessage( newMessage );
			}
		} else {
			for(sint32 i = 0; i < m_numRecipients; i++) {
				m_segment->SetLastShown(m_recipientList[i], g_turn->GetRound());

				if(g_slicEngine->IsMessageClassDisabled(m_class))
					continue;

				if(m_aborted)
					continue;

				if(!g_network.IsActive() || 
				   g_network.IsLocalPlayer(m_recipientList[i]) ||
				   (m_request && m_request->m_id != 0)) {

					if(!g_player[m_recipientList[i]])
						continue;

					if(g_player[m_recipientList[i]]->GetPlayerType() == PLAYER_TYPE_ROBOT &&
					   *m_request == ID(0)
#ifdef _DEBUG
					   && !g_robotMessages
#endif
						) {
						continue;
					}

					MessageData *messageData = m_frame->GetMessageData();

					messageData->SetSlicSegment(m_segment);

					if(m_segment->IsHelp()) {
						messageData->SetIsHelpBox();
					}

					if(m_segment->IsAlert()) {
						messageData->SetIsAlertBox();
					}

					if(m_instantMessage) {
						messageData->SetIsInstantMessage();
					}

					if(m_closeDisabled) {
						messageData->DisableClose(TRUE);
					}

					messageData->SetClass(m_class);

					if(m_isDiplomaticResponse) {
						messageData->SetIsDiplomaticResponse();
					}

					if(1 || m_useDirector) {
						messageData->SetUseDirector();
					}

					if(m_segment->GetFilenum() == k_TUTORIAL_FILE &&
					   !m_dontSave) {
						g_slicEngine->AddTutorialRecord(m_recipientList[i],
														messageData->GetTitle(),	
														messageData->GetMsgText(),
														m_segment);
					}
					Message realMessage;
					realMessage = g_theMessagePool->
						Create(m_recipientList[i], m_frame->GetMessageData());

					if(GetNumTradeBids() > 0) {
						g_theTradeBids->SetMessage(GetTradeBid(0), realMessage);
					}

					if(m_defaultAdvanceSet) {
						realMessage.SetSelectedAdvance(m_defaultAdvance);
					}

					if(m_segment->IsAlert()) {
						
					}
				}
			}
		}
	}
	delete m_frame;
	m_frame = NULL;
}

#ifdef _DEBUG
void SlicObject::Dump()
{
	DPRINTF(k_DBG_INFO, ("SlicObject: ID '%s'\n", m_id));

	SlicContext::Dump();
}
#endif 

void SlicObject::SetMessageText(const MBCHAR *text)
{
	static MBCHAR interpretedText[k_MAX_TEXT_LEN];
	Assert(m_frame);
	if(!m_frame)
		return;


	stringutils_Interpret(text,  *this, interpretedText);

	m_frame->GetMessageData()->SetMsgText(interpretedText);
}

void SlicObject::SetMessageCaption(const MBCHAR *text)
{
	m_frame->GetMessageData()->SetMsgCaption(text);
}

void SlicObject::SetMessageTitle(const MBCHAR *text)
{
	static MBCHAR interpretedText[k_MAX_TEXT_LEN];
	Assert(m_frame);
	if(!m_frame)
		return;


	stringutils_HackColor(FALSE);
	stringutils_Interpret(text,  *this, interpretedText);
	stringutils_HackColor(TRUE);

	m_frame->GetMessageData()->SetTitle(interpretedText);
}

void SlicObject::SetMessageType(MESSAGE_TYPE type,
								MESSAGE_TYPE selectedType)
{
	m_frame->GetMessageData()->SetMsgType(type);
	m_frame->GetMessageData()->SetSelectedMsgType(selectedType);
}

void SlicObject::SetMessageDuration(sint32 duration)
{
	m_frame->GetMessageData()->SetDuration(duration);
}











void SlicObject::Serialize(CivArchive &archive)
{
	sint32	l ;
	
#define SLICLIST_MAGIC 0x25831462
	if (archive.IsStoring()) {
		archive.PerformMagic(SLICLIST_MAGIC) ;
		
		l = strlen(m_id) + 1;
		archive << l;
		archive.Store((uint8 *)m_id, l);
		
		archive<<m_seconds ;
		archive<<m_numRecipients ;
		
		archive.Store((uint8*)m_recipientList, m_numRecipients * sizeof(sint32)) ;
		
		if(m_segment) {
			
			l = strlen(m_segment->GetName()) + 1 ;
		} else {
			l = 0;
		}
		archive << l ;
		if(l > 0) {
			archive.Store((uint8 *)m_segment->GetName(), l) ;
		}
		
		archive << m_defaultAdvanceSet;
		archive << m_defaultAdvance;
		archive << m_aborted;
		archive << m_instantMessage;
		archive << m_class;
		archive << m_dontSave;
		archive << m_closeDisabled;
		archive << m_isDiplomaticResponse;
		archive << m_useDirector;

		
	} else {
		archive.TestMagic(SLICLIST_MAGIC) ;
		
		archive>>l ;
		if (m_id)
			delete m_id ;

		m_refCount = 0;

		m_id = new char[l] ;
		archive.Load((uint8 *)m_id, l) ;
		
		archive>>m_seconds ;
		archive>>m_numRecipients ;
		
		if (m_recipientList)
			delete m_recipientList ;
		
		m_recipientList = new sint32[m_numRecipients] ;
		archive.Load((uint8 *)m_recipientList, m_numRecipients * sizeof(sint32)) ;

		
		archive>>l ;
		if(l > 0) {
			MBCHAR	*tmpID =  new MBCHAR[l] ;
			archive.Load((uint8 *)tmpID, l) ;
			m_segment = g_slicEngine->GetSegment(tmpID) ;
			delete tmpID ;
		} else {
			m_segment = NULL;
		}
		archive >> m_defaultAdvanceSet;
		archive >> m_defaultAdvance;
		archive >> m_aborted;
		archive >> m_instantMessage;
		archive >> m_class;
		archive >> m_dontSave;
		archive >> m_closeDisabled;
		archive >> m_isDiplomaticResponse;
		archive >> m_useDirector;
	}
	SlicContext::Serialize(archive);
	m_request->Serialize(archive);
}

void SlicObject::AddButton(SlicButton *button)
{
	Assert(m_frame);
	if(!m_frame)
		return;
	Assert(m_frame->GetMessageData());
	if(!m_frame->GetMessageData())
		return;
	m_frame->GetMessageData()->AddButton(button);
}

void SlicObject::AddEyePoint(SlicEyePoint *eyepoint)
{
	Assert(m_frame);
	if(!m_frame)
		return;
	m_frame->GetMessageData()->AddEyePoint(eyepoint);
}

void SlicObject::SetDefaultAdvance(sint32 adv)
{
	m_defaultAdvanceSet = TRUE;
	m_defaultAdvance = adv;
}

BOOL SlicObject::ConcernsPlayer(PLAYER_INDEX player)
{
	sint32 i;
	for(i = 0; i < m_numRecipients; i++) {
		if(m_recipientList[i] == player)
			return TRUE;
	}
	return SlicContext::ConcernsPlayer(player);
}

void SlicObject::SetFrame(SlicFrame *frame)
{
	if(m_frame)
		delete m_frame;

	m_frame = frame;
}

void SlicObject::Continue()
{
	m_frame->Run();
	if(g_slicEngine->AtBreak())
		return;

	Finish();
}

