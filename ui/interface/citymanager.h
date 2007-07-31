
#ifndef CITY_MANAGER_H__
#define CITY_MANAGER_H__

class CityManagerWindow : public aui_Window
{
private:
	ctp2_Button *m_ok, *m_cancel;
	aui_Image *m_bg;

public:
	CityManagerWindow(AUI_ERRCODE *retval,
					  uint32 id,
					  MBCHAR *ldlBlock);
	~CityManagerWindow();

	AUI_ERRCODE InitCommonLdl(MBCHAR *ldlBlock);
	AUI_ERRCODE DrawThis(aui_Surface *surface, sint32 x, sint32 y);

	static void Open();
	static void Cleanup();
};

#endif
