






#pragma once
#ifndef ___BMH_DOMESTIC_MANAGEMENT_DIALOG_HEADER
#define ___BMH_DOMESTIC_MANAGEMENT_DIALOG_HEADER


#include <memory>


class aui_Control;
class ctp2_Button;
class ctp2_Window;
class CauseAndEffectTab;
class GovernmentTab;


class DomesticManagementDialog {
public:
	
	static void Open();

	
	static void Close();

	
	DomesticManagementDialog();

	
	
	static void Cleanup();

	void Update();

private:
	
	void Show();

	
	void Hide();

	
	static void CloseButtonActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	
	ctp2_Window *m_window;

	
	ctp2_Button *m_closeButton;

	
	std::auto_ptr<CauseAndEffectTab>	m_causeAndEffectTab;	
	std::auto_ptr<GovernmentTab>		m_governmentTab;		
};


extern DomesticManagementDialog *g_domesticManagementDialog;

#endif 
