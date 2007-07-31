

#ifndef __ATTRACT_WINDOW_H__
#define __ATTRACT_WINDOW_H__

#include "c3window.h"
#include "PointerList.h"

struct AttractRegion {
	aui_Region *m_region;
	uint32 m_startTime;
};

class AttractWindow : public C3Window {
public:
	static void Initialize(void);
	static void Cleanup(void);

public:
	AttractWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_POPUP);

	virtual ~AttractWindow();

	virtual AUI_ERRCODE InitCommon(void);

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );
	virtual AUI_ERRCODE Idle(void);
	void AppIdle();

public:
	void DrawAttractiveStuff(void);
	void ClearWindow(void);
	void HighlightControl(MBCHAR *ldlName);
	void RemoveControl(MBCHAR *ldlName);

	void RemoveRegion(aui_Region *region);
	void AddRegion(aui_Region *region);

private:
	RECT		m_screenAttractRect;
	POINT		m_attractPoint;
	sint32		m_attractStage;
	uint32		m_finishTime;

	PointerList<AttractRegion> m_regions;
};

extern AttractWindow *g_attractWindow;

#endif
