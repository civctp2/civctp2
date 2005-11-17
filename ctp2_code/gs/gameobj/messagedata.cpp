







#include "c3.h"
#include "c3errors.h"

#include "Civilisation.h"
#include "Globals.h"
#include "player.h"
#include "Unit.h"
#include "MessageData.h"
#include "message.h"
#include "TurnCnt.h"
#include "UnitDynArr.h"
#include "TurnCnt.h"
#include "network.h"
#include "net_action.h"
#include "StrDB.h"
#include "SlicObject.h"
#include "SlicEngine.h"
#include "UnitData.h"
#include "Unit.h"
#include "SlicButton.h"
#include "SlicEyePoint.h"
#include "AdvanceRecord.h"
#include "UnitPool.h"
#include "pointerlist.h"
#include "WonderRecord.h"
#include "TerrainRecord.h"
#include "filenamedb.h"
#include "DiplomaticRequestPool.h"
#include "controlpanelwindow.h"


#include "aui.h"
#include "messagewin.h"
#include "messagemodal.h"

extern FilenameDB *g_theMessageIconFileDB;
extern DiplomaticRequestPool *g_theDiplomaticRequestPool;


#define AND_I_AM_STEVE


	extern	TurnCount	*g_turn ;

	extern	Player	**g_player ;

	extern	StringDB	*g_theStringDB ;
 
extern MessageModal *g_modalMessage;









MessageData::MessageData(CivArchive &archive) : GAMEOBJ(0)
{
    sint32 i; 
    for (i=0; i<k_MAX_MSG_LEN; i++) { 
        m_caption[i] = 0; 
    } 

    m_advance = -1; 
    m_cityList = new UnitDynamicArray ;
	m_buttonList = new PointerList<SlicButton>;
	m_eyePoints = new PointerList<SlicEyePoint>;
	m_window = NULL;  
	m_slicSegment = NULL; 
    Serialize(archive) ; 
}

MessageData::MessageData(const ID id) : GAMEOBJ(id.m_id)
	{
	
    sint32 i; 
    for (i=0; i<k_MAX_MSG_LEN; i++) { 
        m_caption[i] = 0; 
    } 

    m_advance = -1; 
	m_owner = PLAYER_INDEX_INVALID ;
	m_sender = PLAYER_INDEX_INVALID ;
	m_msgType = 0;
	m_msgSelectedType = 1;
	m_isRead = FALSE;
	m_cityList = new UnitDynamicArray ;
	m_text = NULL ;
	m_request = DiplomaticRequest() ;
	if (g_turn)
		m_timestamp = g_turn->GetYear() ;
	else
		m_timestamp = 0;
	m_buttonList = new PointerList<SlicButton>;
	m_eyePoints = new PointerList<SlicEyePoint>;
	m_advanceSet = FALSE;
	m_expiration = 0x10000000;  
	m_window = NULL;  
	m_isHelpBox = FALSE;
	m_isAlertBox = FALSE;
	m_isInstant = FALSE;
	m_slicSegment = NULL;
	m_class = -1;
	m_title = NULL;
	m_closeDisabled = FALSE;
	m_isDiplomaticResponse = FALSE;
	m_useDirector = FALSE;
	}










MessageData::MessageData(const ID id, const PLAYER_INDEX owner, const PLAYER_INDEX sender, const MESSAGE_TYPE type, MBCHAR *s) : GAMEOBJ(id.m_id)
	{
    sint32 i; 
    for (i=0; i<k_MAX_MSG_LEN; i++) { 
        m_caption[i] = 0; 
    } 

	m_owner = owner ;
	m_sender = sender ;
	m_msgType = type ;
	m_msgSelectedType = type; 
	m_cityList = new UnitDynamicArray ;
	m_text = new char[strlen(s) + 1];
	strcpy(m_text, s) ;
	m_request = DiplomaticRequest() ;
	m_timestamp = g_turn->GetYear() ;
	m_advanceSet = FALSE;
	m_window = NULL;  
	m_isHelpBox = FALSE;
    m_advance = -1; 
	m_isInstant = FALSE;
	m_slicSegment = NULL;
	m_class = -1;
	m_title = NULL;
	m_buttonList = new PointerList<SlicButton>;
	m_eyePoints = new PointerList<SlicEyePoint>;
	m_isAlertBox = FALSE;
	m_closeDisabled = FALSE;
	m_isDiplomaticResponse = FALSE;
	m_useDirector = FALSE;
	}


MessageData::MessageData(const ID id, MessageData *copy) : GAMEOBJ(id.m_id)
{
    m_advance = -1; 
    sint32 i; 
    for (i=0; i<k_MAX_MSG_LEN; i++) { 
        m_caption[i] = 0; 
    } 


	m_window = NULL;	
						

	
	
	
	
	
	
	

	memcpy(this, copy, (uint8*)&m_text - (uint8*)this);
	m_id = id;
	m_cityList = new UnitDynamicArray;
	for(i = 0; i < copy->m_cityList->Num(); i++) {
		m_cityList->Insert(copy->m_cityList->Access(i));
	}
	m_request = copy->m_request;

	m_buttonList = new PointerList<SlicButton>;
	PointerList<SlicButton>::Walker bwalk(copy->m_buttonList);
	while(bwalk.IsValid()) {
		m_buttonList->AddTail(new SlicButton(bwalk.GetObj()));
		m_buttonList->GetTail()->SetMessage(Message(m_id));
		bwalk.Next();
	}
		
	m_eyePoints = new PointerList<SlicEyePoint>;
	PointerList<SlicEyePoint>::Walker ewalk(copy->m_eyePoints);
	while(ewalk.IsValid()) {
		m_eyePoints->AddTail(new SlicEyePoint(ewalk.GetObj()));
		m_eyePoints->GetTail()->SetMessage(Message(m_id));
		ewalk.Next();
	}

	m_advanceSet = copy->m_advanceSet;

	if(copy->m_text) {
		m_text = new char[strlen(copy->m_text) + 1];
		strcpy(m_text, copy->m_text);
	} else {
		m_text = NULL;
	}
	m_msgType = copy->m_msgType;
	m_msgSelectedType = copy->m_msgSelectedType;
	m_isRead = copy->m_isRead;

	m_isHelpBox = copy->m_isHelpBox;
	m_isInstant = copy->m_isInstant;
	m_isAlertBox = copy->m_isAlertBox;
	m_closeDisabled = copy->m_closeDisabled;
	m_isDiplomaticResponse = copy->m_isDiplomaticResponse;
	m_useDirector = copy->m_useDirector;

	m_slicSegment = copy->m_slicSegment;

	strcpy(m_caption, copy->m_caption);
	m_class = copy->m_class;

	if(copy->m_title) {
		m_title = new MBCHAR[strlen(copy->m_title) + 1];
		strcpy(m_title, copy->m_title);
	} else {
		m_title = NULL;
	}

	m_tradeOffer = copy->m_tradeOffer;
}

#ifdef _BAD_BUTTON
extern PointerList<SlicButton> s_deletedButtons;
#endif

MessageData::~MessageData()
{
	if(m_cityList)
		delete m_cityList;
	
	if(m_buttonList) {
		SlicButton *butt;
		while(butt = m_buttonList->RemoveHead()) {
#ifdef _BAD_BUTTON
			memset(butt, 0xbe, sizeof(SlicButton));
			s_deletedButtons.AddTail(butt);
#else
			delete butt;
#endif
		}
		delete m_buttonList;
	}

	if(m_eyePoints) {
		SlicEyePoint *ep;
		while(ep = m_eyePoints->RemoveHead())
			delete ep;
		delete m_eyePoints;
	}

	if(m_window) {
		KillMessageWindow();
	}

	if(m_advanceSet)
		g_player[m_owner]->StartResearching(m_advance);

	if(m_text)
		delete [] m_text;
	if(m_title)
		delete [] m_title;
}











void MessageData::SetMsgText(MBCHAR *s)
	{
	if(m_text) 
		{
		delete [] m_text;
		}

	m_text = new char[strlen(s) + 1];
	strcpy(m_text, s) ;
	}












void MessageData::Serialize(CivArchive &archive)
	{
	sint32		hasChild;
	sint32 count, i;
	if (archive.IsStoring())
	{
		GAMEOBJ::Serialize(archive);
		
		archive.StoreChunk((uint8 *)&m_owner, ((uint8 *)&m_caption)+sizeof(m_caption));

		if(m_text) {
			count = strlen(m_text) + 1;
			archive << count;
			archive.Store((uint8*)m_text, count * sizeof(MBCHAR));
		} else {
			count = 0;
			archive << count;
		}

		if(m_title) {
			count = strlen(m_title) + 1;
		} else {
			count = 0;
		}
		archive << count;
		if(count > 0) {
			archive.Store((uint8*)m_title, count * sizeof(MBCHAR));
		}
		count = m_buttonList->GetCount();
		archive << count;
		PointerList<SlicButton>::Walker bwalk(m_buttonList);
		for(i = 0; i < count && bwalk.IsValid(); i++, bwalk.Next()) {
			bwalk.GetObj()->Serialize(archive);
		}

		count = m_eyePoints->GetCount();
		archive << count;
		PointerList<SlicEyePoint>::Walker ewalk(m_eyePoints);
		for(i = 0; i < count && ewalk.IsValid(); i++, ewalk.Next()) {
			ewalk.GetObj()->Serialize(archive);
		}
		

		hasChild = m_lesser != NULL;
		archive << hasChild;
		if (m_lesser)
			((MessageData *)(m_lesser))->Serialize(archive) ;

		hasChild = m_greater != NULL;
		archive << hasChild;
		if (m_greater)
			((MessageData *)(m_greater))->Serialize(archive) ;

		}
	else 
	{
		GAMEOBJ::Serialize(archive);

		archive.LoadChunk((uint8 *)&m_owner, ((uint8 *)&m_caption)+sizeof(m_caption));


		archive >> count;
		if(count > 0) {
			m_text = new MBCHAR[count];
			archive.Load((uint8*)m_text, count * sizeof(MBCHAR));
		} else {
			m_text = NULL;
		}

		archive >> count;
		if(count <= 0) {
			m_title = NULL;
		} else {
			m_title = new MBCHAR[count];
			archive.Load((uint8*)m_title, count * sizeof(MBCHAR));
		}

		archive >> count;
		for(i = 0; i < count; i++) {
			m_buttonList->AddTail(new SlicButton(archive));
		}

		archive >> count;
		for(i = 0; i < count; i++) {
			m_eyePoints->AddTail(new SlicEyePoint(archive));
		}






		archive >> hasChild;
		if(hasChild) {
			m_lesser = new MessageData(archive);
		} else {
			m_lesser = NULL;
		}

		archive >> hasChild;
		if(hasChild) {
			m_greater = new MessageData(archive);
		} else {
			m_greater = NULL;
		}

	}

	m_cityList->Serialize(archive) ;
	m_request.Serialize(archive) ;
	m_tradeOffer.Serialize(archive);

	}











void MessageData::Dump(const sint32 i)
	{
	DPRINTF(k_DBG_INFO, ("%d -- %d %d %d %s\n", i, m_owner, m_sender, m_msgType, m_text)) ;
	}









MESSAGE_RESPONSE_TYPE MessageData::Reject(void)
	{
	return (MESSAGE_RESPONSE_TYPE_REJECT) ;
	}









MESSAGE_RESPONSE_TYPE MessageData::Accept(void)
{
	return (MESSAGE_RESPONSE_TYPE_ACCEPT) ;
}











































































































































































































































































































































































































































































































































































































































































































void MessageData::AddButton(SlicButton *button)
{
	




	button->SetMessage(Message(m_id));
	m_buttonList->AddTail(button);
}

sint32 MessageData::GetNumButtons() const
{
	PointerList<SlicButton>::Walker walk(m_buttonList);
	sint32 count = 0;
	while(walk.IsValid()) {
		if(!walk.GetObj()->IsCloseEvent())
			count++;
		walk.Next();
	}
	return count;
}

SlicButton *MessageData::GetButton(sint32 index)
{
	if(index >= m_buttonList->GetCount())
		return NULL;

	PointerList<SlicButton>::Walker walk(m_buttonList);
	sint32 count = 0;
	while(walk.IsValid()) {
		if(!walk.GetObj()->IsCloseEvent()) {
			if(count == index)
				return walk.GetObj();
			count++;
		}
		walk.Next();
	}

	return NULL;
}

SlicButton *MessageData::GetCloseEvent()
{
	PointerList<SlicButton>::Walker walk(m_buttonList);
	while(walk.IsValid()) {
		if(walk.GetObj()->IsCloseEvent()) {
			return walk.GetObj();
		}
		walk.Next();
	}
	return NULL;
}

void MessageData::AddEyePoint(SlicEyePoint *eyePoint)
{
	m_eyePoints->AddTail(eyePoint);
}

SlicEyePoint *MessageData::GetEyePoint(sint32 index)
{
	if(index >= m_eyePoints->GetCount() || index < 0)
		return NULL;

	PointerList<SlicEyePoint>::Walker walk(m_eyePoints);
	sint32 count = 0;
	while(walk.IsValid() && count < index) {
		walk.Next();
		count++;
	}
	Assert(walk.IsValid());
	if(!walk.IsValid())
		return NULL;
	return walk.GetObj();
}


BOOL MessageData::GetEyePointMapPosition( sint32 index, MapPoint &point )
{
	SlicEyePoint *ep = GetEyePoint(index);
	if(!ep) {
		point.Set(1,1);
		return FALSE;
	} else {
		ep->GetPoint(point);
		return TRUE;
	}
}



const MBCHAR *MessageData::GetEyePointName( sint32 index )
{
	SlicEyePoint *ep = GetEyePoint(index);
	if(!ep)
		return NULL;
	if(!ep->GetName()) {
		static MBCHAR empty = 0;
		return &empty;
	}

	return ep->GetName();
}

void MessageData::EyePointCallback(sint32 index)
{
	SlicEyePoint *ep = GetEyePoint(index);
	Assert(ep);
	if(!ep)
		return;
	ep->Callback();
}

void MessageData::EyeDropdownCallback(sint32 index)
{
	SlicEyePoint *ep = GetEyePoint(index);
	Assert(ep);
	if(!ep)
		return;
	ep->Callback();
}

void MessageData::KillMessageWindow( void )
{
	if(m_window) {

		messagewin_PrepareDestroyWindow( m_window );
	} else if(g_modalMessage && g_modalMessage->GetMessage()->m_id == m_id) {
		messagemodal_PrepareDestroyWindow();		
		
	}

	m_window = NULL;
}

void MessageData::IgnoreMessage( void )
{
	
	if (m_window) {
		messagewin_PrepareDestroyWindow( m_window );
	}
	m_window = NULL;
}

sint32 MessageData::GetNumEyePoints() const
{
	return m_eyePoints->GetCount();
}

MESSAGE_EYEPOINT_STYLE MessageData::GetEyePointStyle( void )
{
	sint32 count = GetNumEyePoints();

	if ( count == 0 )
		return MESSAGE_EYEPOINT_STYLE_NONE;
	else if ( count == 1 )
		return MESSAGE_EYEPOINT_STYLE_STANDARD;
	else if ( GetEyePointName( 0 ) )
		return MESSAGE_EYEPOINT_STYLE_DROPDOWN;
	else
		return MESSAGE_EYEPOINT_STYLE_LIST;

}


MESSAGE_RESPONSE_STYLE MessageData::GetResponseStyle( void )
{
	
	
	
	sint32 count = GetNumButtons();
	
	if ( count == 0 )
		return MESSAGE_RESPONSE_STYLE_NONE;
	else if ( count > 4 )
		return MESSAGE_RESPONSE_STYLE_SUBMIT;
	else 
		return MESSAGE_RESPONSE_STYLE_SELECT;
}

void MessageData::SetSelectedAdvance(AdvanceType adv)
{
	BOOL m_advanceSet = TRUE;
	m_advance = adv;
}

AdvanceType MessageData::GetSelectedAdvance() const
{
	return m_advance;
}

void MessageData::SetMsgCaption(const MBCHAR *caption)
{
	strcpy(m_caption, caption);
}

void MessageData::SetDuration(sint32 duration)
{
	m_expiration = g_turn->GetRound() + duration;
}

sint32 MessageData::GetExpiration() const
{
	return m_expiration;
}











void MessageData::ToString(MBCHAR *s)
	{

	}


void MessageData::SetTitle(MBCHAR *title) 
{
	if(m_title) {
		delete [] m_title;
	}
	m_title = new MBCHAR[strlen(title) + 1];
	strcpy(m_title, title);
}

void MessageData::NotifySlicReload()
{
	while(m_buttonList->GetHead())
		m_buttonList->RemoveHead();
	if(m_lesser) {
		((MessageData *)m_lesser)->NotifySlicReload();
	}

	if(m_greater) {
		((MessageData *)m_greater)->NotifySlicReload();
	}
}

void MessageData::SetRead()
{ 
	m_isRead = TRUE; 
	g_controlPanel->SetMessageRead(Message(m_id));
}
