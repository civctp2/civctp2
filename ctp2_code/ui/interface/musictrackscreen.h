#ifndef __MUSICTRACKSCREEN_H__
#define __MUSICTRACKSCREEN_H__

sint32 musictrackscreen_displayMyWindow();
sint32 musictrackscreen_removeMyWindow(uint32 action);
AUI_ERRCODE musictrackscreen_Initialize( void );
AUI_ERRCODE musictrackscreen_Cleanup();

void musictrackscreen_acceptPress(aui_Control *control, uint32 action, uint32 data, void *cookie );

class MusicTrackListItem : public c3_ListItem
{
public:

	MusicTrackListItem(AUI_ERRCODE *retval, sint32 trackNum, const MBCHAR *name, const MBCHAR *ldlBlock);

	virtual void Update(void) {}

	sint32 GetTrackNum(void) { return m_trackNum; }
protected:
	MusicTrackListItem() : c3_ListItem() {}
	AUI_ERRCODE InitCommonLdl(sint32 trackNum, const MBCHAR *name, const MBCHAR *ldlBlock);

private:
	sint32 m_trackNum;
};

#endif
