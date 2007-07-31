











#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <iostream>
using namespace std;


string ID_LOST_BATTLE("ID_LOST_BATTLE");
string ID_ATTACKED_CIVILIANS("ID_ATTACKED_CIVILIANS");
string ID_UNPROVOKED_ATTACK("ID_UNPROVOKED_ATTACK");
string ID_ACCIDENTAL_ATTACK("ID_ACCIDENTAL_ATTACK");


string ID_EXPLAIN_TRADE_CITY_FOR_PEACE("ID_EXPLAIN_TRADE_CITY_FOR_PEACE");
string ID_ADVICE_TRADE_CITY_FOR_PEACE("ID_ADVICE_TRADE_CITY_FOR_PEACE");
string ID_NEWS_TRADE_CITY_FOR_PEACE("ID_NEWS_TRADE_CITY_FOR_PEACE");
string ID_EXPLAIN_PEACE_TREATY("ID_EXPLAIN_PEACE_TREATY");
string ID_ADVICE_PEACE_TREATY("ID_ADVICE_PEACE_TREATY");
string ID_NEWS_PEACE_TREATY("ID_NEWS_PEACE_TREATY");
string ID_EXPLAIN_THREATEN_ATTACK_CITY("ID_EXPLAIN_THREATEN_ATTACK_CITY");
string ID_ADVICE_THREATEN_ATTACK_CITY("ID_ADVICE_THREATEN_ATTACK_CITY");
string ID_NEWS_THREATEN_ATTACK_CITY("ID_NEWS_THREATEN_ATTACK_CITY");
string ID_EXPLAIN_SCENARIO_FORCE_PEACE("ID_EXPLAIN_SCENARIO_FORCE_PEACE");
string ID_ADVICE_SCENARIO_FORCE_PEACE("ID_ADVICE_SCENARIO_FORCE_PEACE");
string ID_NEWS_SCENARIO_FORCE_PEACE("ID_NEWS_SCENARIO_FORCE_PEACE");
string ID_EXPLAIN_FEAR_LEAVE_BORDERS("ID_EXPLAIN_FEAR_LEAVE_BORDERS");
string ID_ADVICE_FEAR_LEAVE_BORDERS("ID_ADVICE_FEAR_LEAVE_BORDERS");
string ID_NEWS_FEAR_LEAVE_BORDERS("ID_NEWS_FEAR_LEAVE_BORDERS");
string ID_EXPLAIN_BUY_SURPLUS_HARDWARE("ID_EXPLAIN_BUY_SURPLUS_HARDWARE");
string ID_ADVICE_BUY_SURPLUS_HARDWARE("ID_ADVICE_BUY_SURPLUS_HARDWARE");
string ID_NEWS_BUY_SURPLUS_HARDWARE("ID_NEWS_BUY_SURPLUS_HARDWARE");
string ID_EXPLAIN_GENERIC_ACCEPT("ID_EXPLAIN_GENERIC_ACCEPT");
string ID_ADVICE_GENERIC_ACCEPT("ID_ADVICE_GENERIC_ACCEPT");
string ID_NEWS_GENERIC_ACCEPT("ID_NEWS_GENERIC_ACCEPT");
string ID_EXPLAIN_GENERIC_REJECT("ID_EXPLAIN_GENERIC_REJECT");
string ID_ADVICE_GENERIC_REJECT("ID_ADVICE_GENERIC_REJECT");
string ID_NEWS_GENERIC_REJECT("ID_NEWS_GENERIC_REJECT");
string ID_EXPLAIN_CAVE_IN_TO_THREAT("ID_EXPLAIN_CAVE_IN_TO_THREAT");
string ID_ADVICE_CAVE_IN_TO_THREAT("ID_ADVICE_CAVE_IN_TO_THREAT");
string ID_NEWS_CAVE_IN_TO_THREAT("ID_NEWS_CAVE_IN_TO_THREAT");
string ID_EXPLAIN_REJECT_THREAT("ID_EXPLAIN_REJECT_THREAT");
string ID_ADVICE_REJECT_THREAT("ID_ADVICE_REJECT_THREAT");
string ID_NEWS_REJECT_THREAT("ID_NEWS_REJECT_THREAT");
string ID_EXPLAIN_NO_THREAT("ID_EXPLAIN_NO_THREAT");
string ID_ADVICE_NO_THREAT("ID_ADVICE_NO_THREAT");
string ID_NEWS_NO_THREAT("ID_NEWS_NO_THREAT");
string ID_EXPLAIN_REJECT_COUNTER("ID_EXPLAIN_REJECT_COUNTER");
string ID_ADVICE_REJECT_COUNTER("ID_ADVICE_REJECT_COUNTER");
string ID_NEWS_REJECT_COUNTER("ID_NEWS_REJECT_COUNTER");
string ID_EXPLAIN_ACCEPT_COUNTER("ID_EXPLAIN_ACCEPT_COUNTER");
string ID_ADVICE_ACCEPT_COUNTER("ID_ADVICE_ACCEPT_COUNTER");
string ID_NEWS_ACCEPT_COUNTER("ID_NEWS_ACCEPT_COUNTER");


string ID_SPY_FULL_ATTACK_SS("ID_SPY_FULL_ATTACK_SS");
string ID_ADVICE_FULL_ATTACK_SS("ID_ADVICE_FULL_ATTACK_SS");
string ID_NEWS_FULL_ATTACK_SS("ID_NEWS_FULL_ATTACK_SS");
string ID_SPY_REGROUP_SS("ID_SPY_REGROUP_SS");
string ID_ADVICE_REGROUP_SS("ID_ADVICE_REGROUP_SS");
string ID_NEWS_REGROUP_SS("ID_NEWS_REGROUP_SS");


string ID_SPY_PROVOKE_WAR_DS("ID_SPY_PROVOKE_WAR_DS");
string ID_ADVICE_PROVOKE_WAR_DS("ID_ADVICE_PROVOKE_WAR_DS");
string ID_NEWS_PROVOKE_WAR_DS("ID_NEWS_PROVOKE_WAR_DS");
string ID_SPY_MAKE_FRIEND_DS("ID_SPY_MAKE_FRIEND_DS");
string ID_ADVICE_MAKE_FRIEND_DS("ID_ADVICE_MAKE_FRIEND_DS");
string ID_NEWS_MAKE_FRIEND_DS("ID_NEWS_MAKE_FRIEND_DS");

#include "CTP2.h"
#include "Diplomat.h"
#include "MapAnalysis.h"


vector<Player> Player::s_players(g_numPlayers);
vector<Scheduler> Scheduler::s_scheduler(g_numPlayers);
vector<AIP> AIP::s_AIP(g_numPlayers);


sint32 MapAnalysis::m_atRiskCitiesValue = 10;
sint32 MapAnalysis::m_cityValue = 11;
sint32 MapAnalysis::m_militaryPower = 12;
sint32 MapAnalysis::m_totalCityValue = 13;

sint32 g_turn = 0;
sint32 g_numPlayers = 3;

int main(int argc, char* argv[])
{
	static PLAYER_INDEX humanId = 0;
	static PLAYER_INDEX robotId = 1;

	
	Diplomat &robot = Diplomat::GetDiplomat(robotId);
	robot.SetPlayerId(robotId);
	AIP::GetAIP(robotId).SetPersonality("Homicidal_Scoundral");
	robot.IsAiControlled(false);

	Diplomat &human = Diplomat::GetDiplomat(humanId );
	human.SetPlayerId(humanId );
	AIP::GetAIP(humanId ).SetPersonality("Homicidal_Scoundral");
	human.IsAiControlled(false);

	
	robot.InitStrategicState();
	robot.InitDiplomaticState(humanId);

	human.InitStrategicState();
	human.InitDiplomaticState(robotId);

	
	MapAnalysis::m_atRiskCitiesValue = 20;
	MapAnalysis::m_militaryPower = 10;


	
	
	Diplomat::DebugStatusAll();

	string choice;
	GameEventArgList args;
	Unit u;
	PLAYER_INDEX current_player = robotId;
	while ( choice != "q") {

		

		cout << endl;
		choice = "";
		cout << "Player " << static_cast<sint32>(current_player);
		cout << " command? (next, propose, event, help, quit)";
		cin >> choice;
		switch (tolower(choice[0])) {
		case 'n': 
			current_player = !current_player;
			Diplomat::GetDiplomat(current_player).BeginTurn();
		
			
			Diplomat::DebugStatusAll();

			break;
		case 'p':
			Diplomat::GetDiplomat(current_player).BeginTurn();

			
			Diplomat::DebugStatusAll();

			break;
		case 'e':
			cout << endl;
			cout << "Player " << (int) current_player;
			cout << " has attacked a unit owned by " << (int) !current_player;
			cout << endl << endl;

			
			u = Unit(!current_player,1,10); 
			args.SetUnit(0,u); 
			args.SetInt(0,static_cast<sint32>(CAUSE_REMOVE_ARMY_ATTACKED)); 
			args.SetPlayer(0,current_player); 
			
			
			Diplomat::GetDiplomat(!current_player).LogEvent(GEV_KillUnit, &args);
			Diplomat::GetDiplomat(!current_player).RecomputeRegard();

			
			Diplomat::DebugStatusAll();

			break;
		case 'h':
			cout << endl;
			cout << " Next: begin turn for other player." << endl;
			cout << " Propose: begin turn for current player." << endl;
			cout << " Event: trigger event on other player." << endl;
			cout << " Help: this file." << endl;
			cout << " Quit: exit program." << endl << endl;
		}
	}

	return 0;
}
