






#pragma once
#ifndef ___BMH_SHORTCUT_PAD_HEADER
#define ___BMH_SHORTCUT_PAD_HEADER


class aui_Control;
class ctp2_Button;


class ShortcutPad {
public:
	
	class ShortcutObject {
	public:
		virtual void operator()() = 0;
	};

	
	template<class return_type> class ShortcutCallback : public ShortcutObject {
	public:
		
		ShortcutCallback(return_type (*function)()) :
		m_function(function) { Assert(m_function); }

		
		virtual void operator()() { m_function(); }
	private:
		
		return_type (*m_function)();
	};

	
	ShortcutPad(MBCHAR *ldlBlock);

private:
	
	static void ShortcutButtonActionCallback(aui_Control *control, uint32 action,
		uint32 data, void *cookie);

	
	ctp2_Button *m_cityManagementButton;
	ctp2_Button *m_civilizationManagementButton;
	ctp2_Button *m_nationalManagementButton;
	ctp2_Button *m_unitManagementButton;
	ctp2_Button *m_diplomacyButton;
	ctp2_Button *m_scienceButton;
	ctp2_Button *m_tradeButton;
	ctp2_Button *m_greatLibraryButton;
};

#endif 
