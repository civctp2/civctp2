







#pragma once
#ifndef ___BMH_SCIENCE_VICTORY_DIALOG_HEADER
#define ___BMH_SCIENCE_VICTORY_DIALOG_HEADER


class aui_Control;
class aui_Region;
class aui_Surface;
class ctp2_Button;
class ctp2_ListBox;
class ctp2_ListItem;
class ctp2_Static;
class ctp2_Window;
class GaiaController;
class RadarMap;
class Unit;


class ScienceVictoryDialog {
public:
	
	static void Open();

	
	static void Close();

	
	static void Cleanup();

	
	ScienceVictoryDialog();

	
	~ScienceVictoryDialog();

	
	void Update();

private:
	
	void Show();

	
	void Hide();

	
	
	void SetSwitchState(sint32 switchState);

	
	void UpdateStartButton(const GaiaController *gaiaController);

	
	void UpdateConstructionInformation(
		const GaiaController *gaiaController);

	
	ctp2_Static *GetListItemColumn(ctp2_ListItem *item,
		sint32 column);

	
	ctp2_ListItem *CreateMainframeItem(const Unit &city,
		sint32 timeRemaining);

	
	void UpdateConstructionList(
		const GaiaController *gaiaController);

	
	void UpdateConstructionMap(
		const GaiaController *gaiaController);

	
	
	void UpdateStatus(const GaiaController *gaiaController);

	
	static AUI_ERRCODE StatusBarActionCallback(ctp2_Static *control,
		aui_Surface *surface, RECT &rect, void *cookie);

	
	
	static void SwitchShowCallback(aui_Region *region,
		void *userData);

	
	static sint32 CompareMainframeCities(ctp2_ListItem *item1,
		ctp2_ListItem *item2, sint32 column);

	
	static void StartButtonActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	
	static void BuildButtonActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	
	static void CloseButtonActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	
	ctp2_Window		*m_window;

	
	
	ctp2_Static		*m_switch;

	
	sint32			m_switchState;

	
	ctp2_Button		*m_startButton;

	
	RadarMap		*m_map;

	
	ctp2_ListBox	*m_mainframeList;

	
	ctp2_Static		*m_towerRadius;			
	ctp2_Static		*m_coverage;			
	ctp2_Static		*m_mainframeReq;		
	ctp2_Static		*m_towerReq;			
	ctp2_Static		*m_satReq;				
	ctp2_Static		*m_mainframeCom;		
	ctp2_Static		*m_towerCom;			
	ctp2_Static		*m_satCom;				

	
	ctp2_Static		*m_statusText;			
	ctp2_Static		*m_statusBar;			

	
	ctp2_Button		*m_adviseButton;

	
	ctp2_Button		*m_buildQueueButton;

	
	ctp2_Button		*m_closeButton;
};


extern ScienceVictoryDialog *g_scienceVictoryDialog;

#endif 
