#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef ___BMH_ZOOM_PAD_HEADER
#define ___BMH_ZOOM_PAD_HEADER


class aui_Control;
class ctp2_Button;


class ZoomPad {
public:
	
	ZoomPad(MBCHAR *ldlBlock);

	
	
	void Update();

private:
	
	static void ZoomInButtonActionCallback(aui_Control *control, uint32 action,
		uint32 data, void *cookie);

	
	static void ZoomOutButtonActionCallback(aui_Control *control, uint32 action,
		uint32 data, void *cookie);

	
	ctp2_Button	*m_zoomIn;

	
	ctp2_Button *m_zoomOut;
};

#endif 
