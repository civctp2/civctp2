
#pragma once

#ifndef __CIVSCENARIOS_H__
#define __CIVSCENARIOS_H__

#define k_SCENARIO_PACK_NAME_MAX		_MAX_PATH
#define k_SCENARIO_PACK_DESC_MAX		512
#define k_SCENARIO_NAME_MAX				_MAX_PATH
#define k_SCENARIO_DESC_MAX				512

#define k_SCENARIO_PACK_LIST_FILENAME	"packlist.txt"
#define k_SCENARIO_FOLDER_PREFIX		"scen"
#define k_SCENARIO_INFO_FILENAME		"scenario.txt"

#define k_SCENARIO_DEFAULT_SAVED_GAME_NAME	"savegame.c2g"

struct Scenario {
	MBCHAR		m_path[_MAX_PATH];
	MBCHAR		m_name[k_SCENARIO_NAME_MAX];
	MBCHAR		m_description[k_SCENARIO_DESC_MAX];
};

struct ScenarioPack {
	sint32      m_index;
	MBCHAR		m_path[_MAX_PATH];
	MBCHAR		m_name[k_SCENARIO_PACK_NAME_MAX];
	MBCHAR		m_description[k_SCENARIO_PACK_DESC_MAX];
	sint32		m_numScenarios;
	Scenario	*m_scenarios;
};

class SaveInfo;

enum CIV_SCEN_ERR {
	CIV_SCEN_OK,
	CIV_SCEN_DIR_EXISTS,
	CIV_SCEN_CANT_CREATE_FILE,
	CIV_SCEN_UNKNOWN
};

class CivScenarios 
{
public:
	static void Initialize(void);
	static void Cleanup(void);

	CivScenarios();
	~CivScenarios();

	void	LoadScenarioData(Scenario *scenario, MBCHAR *scenPath);
	void	LoadScenarioPackData(ScenarioPack *pack, MBCHAR *packPath);
	void	LoadData(void);
	
	void	ClearData(void);

	void	ReloadData(void);

	
	sint32			GetNumScenarioPacks(void) const { return m_numScenarioPacks; }
	ScenarioPack	*GetScenarioPack(sint32 which);
	ScenarioPack    *GetScenarioPackByPath(const MBCHAR *path);

	
	
	
	
	BOOL FindScenario(MBCHAR *scenarioName, ScenarioPack **pack, Scenario **scen);

	BOOL FindScenarioFromSaveFile(MBCHAR *saveFile, ScenarioPack **pack, Scenario **scen);

	
	
	BOOL ScenarioHasSavedGame(Scenario *scen);

	
	
	
	SaveInfo *LoadSaveInfo(Scenario *scen);

	CIV_SCEN_ERR MakeNewPack(MBCHAR *dirName, MBCHAR *packName, MBCHAR *packDesc);
	CIV_SCEN_ERR UpdatePacklist(ScenarioPack *pack);
	CIV_SCEN_ERR MakeNewScenario(ScenarioPack *pack, MBCHAR *scenName, MBCHAR *scenDesc);

private:
	sint32			m_numScenarioPacks;
	ScenarioPack	*m_scenarioPacks;
};

extern CivScenarios		*g_civScenarios;

#endif
