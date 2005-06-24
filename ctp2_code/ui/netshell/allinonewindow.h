









 
#ifndef __ALLINONEWINDOW_H__
#define __ALLINONEWINDOW_H__


#include "ns_aiplayersetup.h"
#include "ns_window.h"
#include "aui_action.h"
#include "ns_string.h"
#include "ns_gamesetup.h"
#include "ns_units.h"
#include "ns_improvements.h"
#include "ns_wonders.h"
#include "gamefile.h"

class aui_Switch;
class aui_StringTable;
class ns_HPlayerItem;


class DialogBoxWindow;
extern DialogBoxWindow *g_rulesWindow;
extern DialogBoxWindow *g_exclusionsWindow;

class AllinoneWindow;
extern AllinoneWindow *g_allinoneWindow;


#define k_PPT_PUBLIC	0
#define k_PPT_PRIVATE	1



#include "StartingPosition.h"

struct ns_ScenarioInfo {
	uint8 isScenario;
	MBCHAR m_fileName[_MAX_PATH];
	MBCHAR m_gameName[_MAX_PATH];
	uint8 m_civs[k_MAX_START_POINTS];
	uint8 m_numStartPositions;
	uint8 m_startInfoType;
	uint8 m_haveSavedGame;
	sint32 m_legalCivs[k_MAX_PLAYERS];
	MBCHAR m_scenarioName[_MAX_PATH]; 
	                                  
};


class AllinoneWindow : public ns_Window
{
public:
	
	AllinoneWindow( AUI_ERRCODE *retval );
	virtual ~AllinoneWindow();

protected:
	AllinoneWindow() : ns_Window() {}
	AUI_ERRCODE	InitCommon( void );
	AUI_ERRCODE CreateControls( void );

public:
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	sint32 CurNumHumanPlayers( void );
	sint32 CurNumAiPlayers( void );
	sint32 CurNumPlayers( void ); 

	
	MBCHAR m_lname[ 101 ];

	
	
	tech_WLList<nf_AIPlayer *> *m_aiplayerList;

	
	bool m_receivedGuids;
	CivGuid m_civGuids[k_MAX_PLAYERS];

	
	
	
	BOOL WhoHasTribe( sint32 index, uint16 *curKey, BOOL *curIsAI, BOOL *curIsFemale );

	
	
	
	sint32 FindTribe( uint16 key, BOOL isAI, BOOL *isFemale = NULL );

	
	
	BOOL AssignTribe(
		sint32 index,
		uint16 key,
		BOOL isAI,
		BOOL isFemale,
		BOOL unassign );

	
	
	
	
	void RequestTribe( sint32 index );

	
	BOOL IsMine( NETFunc::Player *player );
	ns_HPlayerItem *GetHPlayerFromId( dpid_t id );
	NETFunc::Player *GetPlayerFromKey( uint16 key );
	nf_AIPlayer *GetAIPlayerFromKey( uint16 key );

	enum Mode {
		CREATE,
		JOIN,
		CONTINUE_CREATE,
		CONTINUE_JOIN
	};
	void SetMode(Mode m);
	Mode GetMode() { return m_mode; }

	BOOL IsScenarioGame( void ) const { return m_isScenarioGame; }
	void SetScenarioGame( BOOL s ) { m_isScenarioGame = s; }
	void SetupNewScenario();

	void	Update( void );

	
	void	UpdateGameSetup( bool b = false ); 
	
	void	ReallyUpdateGameSetup( void ); 

	void	UpdatePlayerSetup( void ); 
	void	UpdateAIPlayerSetup( nf_AIPlayer *aiplayer );
	void	DeleteAIPlayer( nf_AIPlayer *aiplayer );
	void	ReallyUpdateAIPlayerSetup( void );

	void	AddAIPlayer( sint32 curCount = 0 );

	
	sint32	OKToAddPlayers( void );


	BOOL    LoadGUIDs(SaveInfo *info);
	BOOL    SetScenarioInfo(SaveInfo *info);

	ns_ScenarioInfo *GetScenarioInfo() { return &m_scenInfo; }


	virtual AUI_ERRCODE Idle( void );
	virtual AUI_ERRCODE SetParent( aui_Region *region );

	void EnableButtonsForUnlaunch();

	

	enum CONTROL
	{
		CONTROL_FIRST = 0,

		CONTROL_GAMENAMESTATICTEXT = CONTROL_FIRST,
		CONTROL_GAMENAMETEXTFIELD,
		CONTROL_PLAYSTYLESTATICTEXT,
		CONTROL_PLAYSTYLEDROPDOWN,
		CONTROL_PLAYSTYLEVALUESTATICTEXT,
		CONTROL_PLAYSTYLEVALUESPINNER,

		CONTROL_CHATBOX,
		CONTROL_LOCKSWITCH,
		CONTROL_INFOBUTTON,
		CONTROL_KICKBUTTON,
		CONTROL_PPTSWITCH,
		CONTROL_REVIEWBUTTON,








		CONTROL_PLAYERSTITLESTATICTEXT,
		CONTROL_PLAYERSSHEET,

		CONTROL_HPLAYERSLISTBOX,
		CONTROL_GPLAYERSLISTBOX,
		CONTROL_AIPLAYERSLISTBOX,

		CONTROL_ADDAIBUTTON,

		CONTROL_RULESBUTTON,
		CONTROL_EXCLUSIONSBUTTON,





		CONTROL_AGESBUTTON,
		CONTROL_MAPSIZEBUTTON,
		CONTROL_WORLDTYPEBUTTON,
		CONTROL_WORLDSHAPEBUTTON,
		CONTROL_DIFFICULTYBUTTON,




		CONTROL_RULESTITLESTATICTEXT,
		CONTROL_RULESTITLESTATICTEXT2,
		CONTROL_RULESSHEET,


		CONTROL_BLOODLUSTSWITCH,



		CONTROL_POLLUTIONSWITCH,

		CONTROL_DYNAMICJOINSWITCH,


		CONTROL_HANDICAPPINGSWITCH,
		CONTROL_GOLDSTATICTEXT,
		CONTROL_PWSTATICTEXT,



		CONTROL_CIVPOINTSBUTTON,
		CONTROL_PWPOINTSBUTTON,

		CONTROL_RULESOKBUTTON,


		CONTROL_EXCLUSIONSTITLESTATICTEXT,
		CONTROL_EXCLUSIONSSHEET,

		CONTROL_SMALLNASTYTABGROUP,
		CONTROL_UNITSTAB,
		CONTROL_UNITSLISTBOX,
		CONTROL_IMPROVEMENTSTAB,
		CONTROL_IMPROVEMENTSLISTBOX,
		CONTROL_WONDERSTAB,
		CONTROL_WONDERSLISTBOX,

		CONTROL_EXCLUSIONSOKBUTTON,








		CONTROL_OKBUTTON,
		CONTROL_CANCELBUTTON,
		CONTROL_LAST,
		CONTROL_MAX = CONTROL_LAST - CONTROL_FIRST
	};

	
	AUI_ERRCODE CreateExclusions( void );

	void UpdatePlayerButtons( void );
	void SpitOutGameSetup( void );

	void	UpdateTribeSwitches( void );



protected:
	void	UpdateConfig( void );
	void	UpdateDisplay( void );

	bool m_createdExclusions;

	Mode m_mode;
	BOOL m_isScenarioGame;

	
	



	
	uint32 m_tickGame;
	uint32 m_tickPlayer;
	uint32 m_tickAIPlayer;

	ns_String	*m_messageRequestDenied;
	ns_String	*m_messageKicked;
	ns_String	*m_messageGameSetup;
	ns_String	*m_messageGameEnter;
	ns_String	*m_messageGameHost;
	ns_String	*m_messageGameCreate;
	ns_String	*m_messageLaunched;

	aui_StringTable *m_playStyleValueStrings;
	aui_StringTable *m_PPTStrings;

	bool m_shouldUpdateGame;
	bool m_shouldUpdatePlayer;
	bool m_shouldUpdateAIPlayer;
	sint32 m_shouldAddAIPlayer;

	bool m_joinedGame;

	aui_Action *m_dbActionArray[ 1 ]; 

	sint32 m_numAvailUnits;
	aui_Switch *m_units[ k_UNITS_MAX ];
	sint32 m_numAvailImprovements;
	aui_Switch *m_improvements[ k_IMPROVEMENTS_MAX ];
	sint32 m_numAvailWonders;
	aui_Switch *m_wonders[ k_WONDERS_MAX ];
	friend class nf_GameSetup;
	
	ns_ScenarioInfo m_scenInfo;

	

	class GameNameTextFieldAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class PPTSwitchAction : public aui_Action
	{ public: virtual ActionCallback Execute; };
	friend class PPTSwitchAction;

	class KickButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class InfoButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class OKButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class CancelButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class ReviewButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class LockSwitchAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class AddAIButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };







	class DialogBoxPopDownAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class PlayersListBoxAction : public aui_Action
	{ public: virtual ActionCallback Execute; };










	class PlayStyleDropDownAction : public aui_Action
	{ public: virtual ActionCallback Execute; };
	friend class PlayStyleDropDownAction;




	static void PlayStyleValueSpinnerCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
	



	class RulesButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class ExclusionsButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };




	class RulesOKButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class ExclusionsOKButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };


	class DynamicJoinSwitchAction : public aui_Action
	{ public: virtual ActionCallback Execute; };







	class HandicappingSwitchAction : public aui_Action
	{ public: virtual ActionCallback Execute; };
	friend class HandicappingSwitchAction;





	class BloodlustSwitchAction : public aui_Action
	{ public: virtual ActionCallback Execute; };










	class PollutionSwitchAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class CivPointsButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };
	friend class CivPointsButtonAction;

	class PwPointsButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };
	friend class PwPointsButtonAction;









	class AgesButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class MapSizeButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class WorldTypeButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class WorldShapeButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class DifficultyButtonAction : public aui_Action
	{ public: virtual ActionCallback Execute; };

	class UnitExclusionAction : public aui_Action
	{
	public:
		UnitExclusionAction( sint32 index )
			: m_index( index ) {}
		virtual ActionCallback Execute;
	private:
		sint32 m_index;
	};
	
	class ImprovementExclusionAction : public aui_Action
	{
	public:
		ImprovementExclusionAction( sint32 index )
			: m_index( index ) {}
		virtual ActionCallback Execute;
	private:
		sint32 m_index;
	};
	
	class WonderExclusionAction : public aui_Action
	{
	public:
		WonderExclusionAction( sint32 index )
			: m_index( index ) {}
		virtual ActionCallback Execute;
	private:
		sint32 m_index;
	};
};


void AllinoneAgesCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	void* cookie );
void AllinoneMapSizeCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	void* cookie );
void AllinoneWorldShapeCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	void* cookie );
void AllinoneTribeCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	void* cookie );










void AllinoneDifficultyCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	void* cookie );
void AllinoneWorldTypeCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	void* cookie );


#endif 
