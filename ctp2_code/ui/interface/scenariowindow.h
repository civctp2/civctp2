

#pragma once
#ifndef SCENARIOWINDOW_FLAG
#define	SCENARIOWINDOW_FLAG

#include "c3_popupwindow.h"
#include "spnewgamewindow.h"
#include "aui_control.h"
#include "aui_action.h"

class ctp2_Window;
class ctp2_Button;
class ctp2_Static;

class ctp2_ListBox;


class ScenarioRulesBox;
class ScenarioBox;
class AvailableBox;


struct Scenario;
struct ScenarioPack;




sint32 scenarioscreen_displayMyWindow();
sint32 scenarioscreen_removeMyWindow(uint32 action);


sint32 scenarioscreen_SetExitCallback(aui_Control::ControlActionCallback *callback);


AUI_ERRCODE scenarioscreen_Initialize( aui_Control::ControlActionCallback
									   *callback = NULL );
AUI_ERRCODE scenarioscreen_Cleanup();




enum SCENARIO_WINDOW_MODE {
	SCENARIO_WINDOW_MODE_NONE = -1,

	SCENARIO_WINDOW_MODE_LOAD_PACK,
	SCENARIO_WINDOW_MODE_LOAD_SCEN,
	SCENARIO_WINDOW_MODE_SAVE_PACK,
	SCENARIO_WINDOW_MODE_SAVE_SCEN,

	SCENARIO_WINDOW_MODE_MAX
};

class ScenarioWindow
{
public:
	ScenarioWindow(
		AUI_ERRCODE *retval,
		MBCHAR *ldlBlock);
	virtual ~ScenarioWindow();

	static void            Initialize();
	static void            Display(bool load = true);
	static void            Hide();
	static void            Cleanup();

	void			SetScenario(Scenario *scenario);
	Scenario		*GetScenario(void) const { return m_scenario; }

	void			SetScenarioPack(ScenarioPack *scenarioPack) { m_scenarioPack = scenarioPack; }
	ScenarioPack	*GetScenarioPack(void) const { return m_scenarioPack; }

	SCENARIO_WINDOW_MODE 	GetMode(void) { return m_mode; }
	void			SetMode(SCENARIO_WINDOW_MODE mode);

	void			FillListWithScenarios(ctp2_ListBox *available);

	
	void			FillListWithScenarioPacks(ctp2_ListBox *available, bool hideOriginalScenarios=false);

	
	static void			SetExitCallback(aui_Control::ControlActionCallback *callback);
	aui_Control::ControlActionCallback *GetExitCallback(void) { return m_exitCallback; }

	static void ScenarioSelect(aui_Control *control, uint32 action, uint32 data, void *cookie );
	static void CancelPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
	static void OkPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
	static void SavePress(aui_Control *control, uint32 action, uint32 data, void *cookie );
	static void NewPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
	static void BackPress(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void NewPackOk(aui_Control *control, uint32 action, uint32 data, void *cookie );
	static void NewPackCancel(aui_Control *control, uint32 action, uint32 data, void *cookie );

	static void NewScenOk(aui_Control *control, uint32 action, uint32 data, void *cookie );
	static void NewScenCancel(aui_Control *control, uint32 action, uint32 data, void *cookie );

	static void SetProfileFromScenario( void );
	static void LoadScenarioGame( void );


public:	
	
	Scenario		*m_scenario;
	ScenarioPack	*m_scenarioPack;	
	sint32			m_scenarioPackIndex;
	
	SCENARIO_WINDOW_MODE	m_mode;

	ctp2_Window *m_window;
	ctp2_Static *m_scenInstructions;
	ctp2_Static *m_packInstructions;

	
	ctp2_Button     *m_LoadButton;
	ctp2_Button     *m_SaveButton;
	ctp2_Button     *m_NewButton;
	ctp2_Button     *m_BackButton;

	ctp2_ListBox		*m_available;
	
	ctp2_Window *m_newPackWindow;
	ctp2_Window *m_newScenWindow;

	
	aui_Control::ControlActionCallback	*m_exitCallback;
};





class CloseScenarioScreenAction : public aui_Action
{
public:
	virtual ActionCallback Execute;
};

#endif
