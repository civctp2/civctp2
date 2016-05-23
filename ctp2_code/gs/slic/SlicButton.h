#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __SLIC_BUTTON_H__
#define __SLIC_BUTTON_H__

typedef sint32 StringId;
class SlicSegment;
class SlicObject;
class CivArchive;
class Message;

class SlicButton
{
private:

	StringId m_name;
	sint32 m_isCloseEvent;
	sint32 m_codeOffset;


	Message *m_message;
	SlicObject *m_context;
	SlicSegment *m_segment;
	char *m_segmentName;

public:
	SlicButton(StringId name, SlicSegment *segment,
			   sint32 codeOffset, SlicObject *context);
	SlicButton(sint32 codeOffset);
	SlicButton(CivArchive &archive);
	SlicButton(SlicButton*);
	~SlicButton();
	void Serialize(CivArchive &archive);

	const MBCHAR *GetName() const;
	SlicSegment *GetSegment() const { return m_segment; }
	sint32 GetOffset() const { return m_codeOffset; }
	SlicObject *GetContext() const { return m_context; }
	void SetMessage(const Message &message);
	void GetMessage(Message &message) const;
	BOOL IsCloseEvent();
	void Callback();
};

#endif
