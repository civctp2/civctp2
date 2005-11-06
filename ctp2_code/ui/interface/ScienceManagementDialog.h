





#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef ___BMH_SCIENCE_MANAGEMENT_DIALOG_HEADER
#define ___BMH_SCIENCE_MANAGEMENT_DIALOG_HEADER


class AdvanceRecord;
class aui_Control;
class aui_Surface;
class aui_Switch;
class ctp2_Button;
class ctp2_HyperTextBox;
class ctp2_ListBox;
class ctp2_ListItem;
class ctp2_Static;
class ctp2_Window;


class ScienceManagementDialog {
public:
	
	static void Open();

	
	static void Close();

	
	static void Cleanup();

	
	ScienceManagementDialog();

	
	~ScienceManagementDialog();

	
	void Update();

private:
	
	void Show();

	
	void Hide();

	
	void UpdateScience();

	
	void ClearScience();

	
	void UpdateAdvanceList();

	
	ctp2_ListItem *CreateAdvanceItem(const AdvanceRecord *advance);

	
	BOOL UpdateAdvanceItem(ctp2_ListItem *item,
		const AdvanceRecord *advance);

	
	ctp2_Static *GetListItemColumn(ctp2_ListItem *item, sint32 column);

	
	static void CloseButtonActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	
	
	static void HyperlinkActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	
	static void EditResearchButtonActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	
	static void AdvanceListCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	
	static AUI_ERRCODE ColorBoxActionCallback(ctp2_Static *control,
		aui_Surface *surface, RECT &rect, void *cookie);

	
	static AUI_ERRCODE ColorHeaderActionCallback(aui_Switch *control,
		aui_Surface *surface, RECT &rect, void *cookie);

	
	static sint32 CompareAdvance(ctp2_ListItem *item1, ctp2_ListItem *item2,
		sint32 column);

	
	ctp2_Window		*m_window;

	
	ctp2_Button		*m_closeButton;

	
	ctp2_Button		*m_changeButton;

	
	ctp2_Static *m_scienceLabel;		
	ctp2_Button *m_scienceIconButton;	
	ctp2_Button *m_scienceTurnButton;	
	ctp2_Static *m_scienceTurnValue;	
	ctp2_Static *m_scienceCurrentValue;	
	ctp2_Static *m_scienceTotalValue;	

	
	ctp2_HyperTextBox	*m_scienceDescription;

	
	ctp2_ListBox		*m_advanceList;
};


extern ScienceManagementDialog *g_scienceManagementDialog;

#endif 
