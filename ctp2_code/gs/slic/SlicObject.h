#ifndef __SLIC_OBJECT_H__
#define __SLIC_OBJECT_H__

#include "SlicContext.h"

class ID;
class CivArchive ;
class SlicButton;
class SlicEyePoint;
class DiplomaticRequest;
class SlicArgList;
class GameEventArgList;

typedef sint32 MESSAGE_TYPE;

class SlicObject : public SlicContext
{
private:
	sint32 m_refCount;

	char *m_id;
	SlicSegment *m_segment;
	SlicFrame *m_frame;
	sint32 m_seconds;

	sint32 *m_recipientList;
	sint32 m_numRecipients;
	ID *m_request;

	sint32 m_defaultAdvanceSet;
	sint32 m_defaultAdvance;

	sint32 m_aborted;
	sint32 m_instantMessage;

	sint32 m_index;

	sint32 m_class;
	sint32 m_dontSave;

	sint32 m_closeDisabled;
	sint32 m_isDiplomaticResponse;

	sint32 m_useDirector;

	sint32 m_result;

	SlicArgList *m_argList;
	void Execute();
#ifdef _DEBUG
	void Dump();
#endif

	friend class SlicEngine;

public:
	sint32 AddRef();
	sint32 Release();
	SlicObject();
	SlicObject(char const * id);
	SlicObject(SlicSegment *segment);
	SlicObject(SlicSegment *segment, SlicObject *copy);
    SlicObject(char const * id, SlicContext *copy);
	SlicObject(CivArchive &archive) ;
	virtual ~SlicObject();
	void Serialize(CivArchive &archive) ;

	sint32 GetIndex() const { return m_index; }
	void SetIndex(sint32 index) { m_index = index; }

	BOOL IsValid();

	void AddRecipient(const PLAYER_INDEX recip);
	void AddAllRecipients();
	void AddAllRecipientsBut(const PLAYER_INDEX loser1,
                             const PLAYER_INDEX loser2 = -1);

	sint32 GetNumRecipients() const { return m_numRecipients; }
	sint32 GetRecipient(const PLAYER_INDEX index) const;

	void SetIdle(sint32 idle) { m_seconds = idle; }
	sint32 GetIdle() const { return m_seconds; }
	void SetMessageText(const MBCHAR *text);
	void SetMessageType(MESSAGE_TYPE type, MESSAGE_TYPE selected);
	void SetMessageCaption(const MBCHAR *text);
	void SetMessageDuration(sint32 duration);
	void SetMessageTitle(const MBCHAR *text);

	void AddButton(SlicButton *button);
	void AddEyePoint(SlicEyePoint *eyepoint);

	SlicSegment *GetSegment() { return m_segment; }

	void SetDefaultAdvance(sint32 adv);

	BOOL ConcernsPlayer(PLAYER_INDEX player);

	void Abort() { m_aborted = TRUE; }
	void SetInstant() { m_instantMessage = TRUE; }

	void SetClass(sint32 mclass) { m_class = mclass; }

	void SetDontSave() { m_dontSave = TRUE; }
	void DisableClose() { m_closeDisabled = TRUE; }

	void SetIsDiplomaticResponse() { m_isDiplomaticResponse = TRUE; }
	void SetUseDirector() { m_useDirector = TRUE; }

	void SetArgList(SlicArgList *argList) { m_argList = argList; }
	SlicArgList *GetArgList() { return m_argList; }

	void SetResult(sint32 result) { m_result = result; }
	sint32 GetResult() { return m_result; }

	void SetFrame(SlicFrame *frame);
	SlicFrame *GetFrame() { return m_frame; }

	void Continue();
	void Finish();

};

#endif
