//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : SLIC functions
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
// Compiler flags
// 
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
// _MSC_VER		
// - Compiler version (for the Microsoft C++ compiler only)
//
// Note: For the blocks with _MSC_VER preprocessor directives, the following
//       is implied: the (_MSC_VER) preprocessor directive lines, and the blocks
//       that are inactive for _MSC_VER value 1200 are modified Apolyton code. 
//       The blocks that are active for _MSC_VER value 1200 are the original 
//       Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Readded all the slicfunctions of the patch, maybe some of them belong 
//   better into slicfuncai.h, by Martin Gühmann.
// - New slic functions added by Martin Gühmann:
//   - CargoCapacity     Gets number of additional units a unit can carry.
//   - MaxCargoSize      Gets the maximum number of units a unit can carry.
//   - CargoSize         Gets the current number of units a unit is carrying.
//   - GetUnitFromCargo  Gets the i'th unit a unit is carrying.
//   - GetContinent      Gets the continent ID of an location.
//   - IsWater           Gets whether a location is water.
// - IsVeteran return type corrected.
// - New slic function by Solver: IsOnSameContinent - Checks whether two
//   locations are on the same continent.
// - Added AddSlaves function modelled after the AddPops function.
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#ifndef __SLICFUNC_H__
#define __SLICFUNC_H__


#define k_MAX_SLIC_ARGS 8

class CivArchive;
class SlicSegment;
class SlicSymbolData;
class Unit;
class MapPoint;
class GameEventArgList;
class Army;

typedef sint32 StringId;

enum GAME_EVENT;

enum SA_TYPE {
	SA_TYPE_INT,
	SA_TYPE_INT_VAR,
	SA_TYPE_RECIP,
	SA_TYPE_STRING,
	SA_TYPE_OBJECT,
	SA_TYPE_LIST,
	SA_TYPE_SEGMENT,
	SA_TYPE_REGION,
	SA_TYPE_COMPLEX_REGION,
	SA_TYPE_BUILTIN,
	SA_TYPE_HARD_STRING,
};
	
enum SFN_ERROR {
	SFN_ERROR_OK,
	SFN_ERROR_NUM_ARGS,     
	SFN_ERROR_TYPE_ARGS,    
	SFN_ERROR_BAD_FUNCTION, 
	SFN_ERROR_INTERNAL,     
	SFN_ERROR_TYPE_BUILTIN, 
	SFN_ERROR_NOT_ADVANCE,  
	SFN_ERROR_NOT_UNIT_FLAG, 
	SFN_ERROR_NOT_IN_BUTTON, 
	SFN_ERROR_NOT_DIP_MESSAGE, 
	SFN_ERROR_NOT_MESSAGE_TYPE, 
	SFN_ERROR_NOT_SEGMENT,    
	SFN_ERROR_NOT_TRADE_BID, 
	SFN_ERROR_NOT_SINGLE_RECIPIENT, 
	SFN_ERROR_NOT_ADJACENT,  
	SFN_ERROR_UNKNOWN_UNIT_TYPE, 
	SFN_ERROR_UNKNOWN_BUILDING, 
	SFN_ERROR_OUT_OF_RANGE, 
	SFN_ERROR_DEAD_PLAYER, 
	SFN_ERROR_EFFECT_NOT_FOUND, 
	SFN_ERROR_SOUND_NOT_FOUND, 
	SFN_ERROR_CIV_NOT_FOUND, 
	SFN_ERROR_NOT_A_FUNCTION, 
};

enum SLIC_FUNC_RET_TYPE {
	SFR_VOID,
	SFR_INT,
	SFR_UNIT,
	SFR_CITY,
	SFR_ARMY
};

struct SlicArg {
	sint32 m_int;
	SlicSegment* m_segment;
	SlicSymbolData *m_symbol;
};

class SlicStringToToken {
private:
	const char *m_name;
	sint32 m_token;

public:
	SlicStringToToken(const char *name, sint32 token) {
		m_name = name;
		m_token = token;
	}
	SlicStringToToken(CivArchive &archive) {
		Serialize(archive);
	}
	const char *GetName() { return m_name; }
	sint32 GetToken() const { return m_token; }

	void Serialize(CivArchive &archive) {};
};

class SlicArgList {
public:
	SlicArgList();
	void AddArg(SA_TYPE type, sint32 value);
	void AddArg(SlicSegment *segment, SlicSymbolData *symbol);
	void AddArg(SA_TYPE type, SlicSymbolData *symbol);
	void Clear() { m_numArgs = 0; }
	BOOL GetInt(sint32 arg, sint32 &value);
	BOOL GetPlayer(sint32 arg, sint32 &value);
	BOOL GetString(sint32 arg, const char *&value);
	BOOL GetUnit(sint32 arg, Unit &u);
	BOOL GetCity(sint32 arg, Unit &city);
	BOOL GetPos(sint32 arg, MapPoint &pos);
	BOOL GetArmy(sint32 arg, Army &a);
	BOOL GetStringId(sint32 arg, StringId &id);

	GameEventArgList *CreateGameEventArgs(GAME_EVENT ev);

	SA_TYPE m_argType[k_MAX_SLIC_ARGS];
	SlicArg m_argValue[k_MAX_SLIC_ARGS];
	sint32 m_numArgs;
};

union SlicFuncResult {
	sint32 m_int;
	uint32 m_unit_id;
	uint32 m_city_id;
	uint32 m_army_id;
};

class SlicFunc {
private:
	char *m_name;
protected:
	SLIC_FUNC_RET_TYPE m_type;
	SlicFuncResult m_result;

public:
	SlicFunc(char *name, SLIC_FUNC_RET_TYPE type);
	SlicFunc(CivArchive &archive);
	~SlicFunc();
	void Serialize(CivArchive &archive);
	SlicFuncResult GetResult() { return m_result; }
	SLIC_FUNC_RET_TYPE GetReturnType() { return m_type; }

	const char *GetName() const { return m_name; }
	virtual SFN_ERROR Call(SlicArgList *args) 
	{
		DPRINTF(k_DBG_SLIC, ("Hey you!  Stop that!\n"));
		return SFN_ERROR_BAD_FUNCTION;
	}
};

#define SLICFUNC(type, x) \
class Slic_##x : public SlicFunc { \
public:\
	Slic_##x() : SlicFunc(#x, type) {} \
	SFN_ERROR Call(SlicArgList *args); \
};












SLICFUNC(SFR_VOID, PrintInt);
SLICFUNC(SFR_VOID, PrintText);
SLICFUNC(SFR_VOID, Return1);
SLICFUNC(SFR_VOID, Return0);


SLICFUNC(SFR_VOID, Text);				
SLICFUNC(SFR_VOID, Message);			
SLICFUNC(SFR_VOID, AddMessage);		
SLICFUNC(SFR_VOID, MessageAll);		
SLICFUNC(SFR_VOID, MessageAllBut);	
SLICFUNC(SFR_VOID, EyePoint);			
SLICFUNC(SFR_VOID, DisableTrigger);	
SLICFUNC(SFR_VOID, EnableTrigger);	
SLICFUNC(SFR_INT,  HasAdvance); 		
SLICFUNC(SFR_INT,  IsContinentBiggerThan);    
SLICFUNC(SFR_INT,  IsHostile);		
SLICFUNC(SFR_INT,  TradePoints);      
SLICFUNC(SFR_INT,  TradePointsInUse);      
SLICFUNC(SFR_INT,  TradeRoutes);      
SLICFUNC(SFR_INT,  HasSameGoodAsTraded);
SLICFUNC(SFR_VOID, AddCity);          
SLICFUNC(SFR_INT,  IsSecondRowUnit);  
SLICFUNC(SFR_INT,  IsFlankingUnit);   
SLICFUNC(SFR_INT,  IsBombardingUnit); 

SLICFUNC(SFR_INT,  IsWormholeProbe);  
SLICFUNC(SFR_INT,  IsUnderseaCity);   
SLICFUNC(SFR_INT,  IsSpaceCity);      
SLICFUNC(SFR_INT,  IsSpaceUnit);      
SLICFUNC(SFR_INT,  IsWonderType);     
SLICFUNC(SFR_INT,  IsCounterBombardingUnit); 
SLICFUNC(SFR_INT,  IsCleric);         
SLICFUNC(SFR_INT,  IsSlaver);         
SLICFUNC(SFR_INT,  IsActiveDefender); 
SLICFUNC(SFR_INT,  IsDiplomat);       
SLICFUNC(SFR_INT,  IsInRegion);       
SLICFUNC(SFR_INT,  UnitHasFlag);      
SLICFUNC(SFR_INT,  UnitsInCell);      
SLICFUNC(SFR_INT,  PlayerCityCount);  
SLICFUNC(SFR_INT,  RegardLevel);      
SLICFUNC(SFR_VOID, ChangeRegardLevel);
SLICFUNC(SFR_VOID, Kill);             
SLICFUNC(SFR_VOID, DeactivateTutorial);   
SLICFUNC(SFR_INT,  ControlsRegion);    
SLICFUNC(SFR_VOID, DemandWarFromAllies); 

SLICFUNC(SFR_INT,  KnowledgeRank);     
SLICFUNC(SFR_INT,  MilitaryRank);      

SLICFUNC(SFR_INT,  TradeRank);         
SLICFUNC(SFR_INT,  GoldRank);          
SLICFUNC(SFR_INT,  PopulationRank);    
SLICFUNC(SFR_INT,  CitiesRank);        
SLICFUNC(SFR_INT,  GeographicRank);    
SLICFUNC(SFR_INT,  SpaceRank);         
SLICFUNC(SFR_INT,  UnderseaRank);      


SLICFUNC(SFR_VOID, Accept);            
SLICFUNC(SFR_VOID, Reject);            


SLICFUNC(SFR_VOID, EyeDropdown);       


SLICFUNC(SFR_VOID, CaptureCity);       
SLICFUNC(SFR_VOID, CaptureRegion);     
SLICFUNC(SFR_VOID, LeaveRegion);       
SLICFUNC(SFR_VOID, Surrender);         


SLICFUNC(SFR_VOID, Research);

SLICFUNC(SFR_VOID, MessageType);       
SLICFUNC(SFR_VOID, Caption);           

SLICFUNC(SFR_VOID, Duration);          
SLICFUNC(SFR_VOID, BreakAgreement);    
SLICFUNC(SFR_VOID, AcceptTradeOffer);  
SLICFUNC(SFR_VOID, DontAcceptTradeOffer); 
SLICFUNC(SFR_VOID, SetGovernment);     
SLICFUNC(SFR_VOID, StealRandomAdvance); 
SLICFUNC(SFR_VOID, StealSpecificAdvance); 

SLICFUNC(SFR_VOID, DisableTileImprovementButton); 
SLICFUNC(SFR_VOID, DisableScreensButton); 
SLICFUNC(SFR_VOID, EnableTileImprovementButton); 
SLICFUNC(SFR_VOID, EnableScreensButton); 

SLICFUNC(SFR_VOID, OpenCiv); 
SLICFUNC(SFR_VOID, OpenCity); 
SLICFUNC(SFR_VOID, OpenUnit); 
SLICFUNC(SFR_VOID, OpenScience); 
SLICFUNC(SFR_VOID, OpenDiplomacy); 
SLICFUNC(SFR_VOID, OpenTrade); 
SLICFUNC(SFR_VOID, OpenInfo); 
SLICFUNC(SFR_VOID, OpenOptions); 
SLICFUNC(SFR_VOID, OpenCivTab); 
SLICFUNC(SFR_VOID, OpenMaxTab); 
SLICFUNC(SFR_VOID, OpenLaborTab); 
SLICFUNC(SFR_VOID, OpenProductionTab); 
SLICFUNC(SFR_VOID, OpenCityTab); 
SLICFUNC(SFR_VOID, ExitToShell); 

SLICFUNC(SFR_VOID, SendTradeBid); 
SLICFUNC(SFR_VOID, AcceptTradeBid); 
SLICFUNC(SFR_VOID, RejectTradeBid); 

SLICFUNC(SFR_VOID, BreakAlliance); 
SLICFUNC(SFR_VOID, AddOrder); 

SLICFUNC(SFR_VOID, EndTurn); 
SLICFUNC(SFR_VOID, FinishBuilding); 
SLICFUNC(SFR_VOID, Abort);          
SLICFUNC(SFR_VOID, Show);           
SLICFUNC(SFR_VOID, DoAutoUnload);                 
SLICFUNC(SFR_VOID, DoLandInOcean);                
SLICFUNC(SFR_VOID, DoOutOfFuel);                  
SLICFUNC(SFR_VOID, DoPillageOwnLand);             
SLICFUNC(SFR_VOID, DoSellImprovement);            
SLICFUNC(SFR_VOID, DoCertainRevolution);          
SLICFUNC(SFR_VOID, DoFreeSlaves);                 
SLICFUNC(SFR_VOID, DoCannotAffordMaintenance);    
SLICFUNC(SFR_VOID, DoCannotAffordSupport);        
SLICFUNC(SFR_VOID, DoCityWillStarve);             
SLICFUNC(SFR_VOID, DoYouWillBreakRoute);          

SLICFUNC(SFR_INT,  TerrainType);                  

SLICFUNC(SFR_VOID, LibraryUnit);                  
SLICFUNC(SFR_VOID, LibraryBuilding);              
SLICFUNC(SFR_VOID, LibraryWonder);                
SLICFUNC(SFR_VOID, LibraryAdvance);               
SLICFUNC(SFR_VOID, LibraryTerrain);               
SLICFUNC(SFR_VOID, LibraryConcept);               
SLICFUNC(SFR_VOID, LibraryGovernment);            
SLICFUNC(SFR_VOID, LibraryTileImprovement);       

SLICFUNC(SFR_INT,  UnitCount);                    

SLICFUNC(SFR_INT,  UnitType);                   
SLICFUNC(SFR_VOID, KillMessages);                 

SLICFUNC(SFR_VOID, MessageClass);                 
SLICFUNC(SFR_VOID, KillClass);                    

SLICFUNC(SFR_INT,  CityHasBuilding);              

SLICFUNC(SFR_VOID, Title);                        

SLICFUNC(SFR_VOID, NetworkAccept);                
SLICFUNC(SFR_VOID, NetworkEject);                 

SLICFUNC(SFR_VOID, Attract);                      
SLICFUNC(SFR_VOID, StopAttract);                  

SLICFUNC(SFR_VOID, DontSave);                     

SLICFUNC(SFR_INT,  IsUnitSelected)                
SLICFUNC(SFR_INT,  IsCitySelected)                

SLICFUNC(SFR_INT,  BuildingType)                  

SLICFUNC(SFR_INT,  IsHumanPlayer)                 

SLICFUNC(SFR_VOID, DisableClose)                  

SLICFUNC(SFR_VOID, EnableCloseMessage)            
SLICFUNC(SFR_VOID, EnableCloseClass)              

SLICFUNC(SFR_VOID, AddGoods)                      
SLICFUNC(SFR_INT,  GoodType)                      
SLICFUNC(SFR_INT,  GoodCount)                     
SLICFUNC(SFR_INT,  GoodCountTotal)                
SLICFUNC(SFR_INT,  GoodVisibutik)                 

SLICFUNC(SFR_VOID, StartTimer)                    
SLICFUNC(SFR_VOID, StopTimer)                     

SLICFUNC(SFR_VOID, DisableMessageClass)           
SLICFUNC(SFR_VOID, EnableMessageClass)            

SLICFUNC(SFR_VOID, CreateUnit)                    
SLICFUNC(SFR_INT,  Random)                        
SLICFUNC(SFR_VOID, AddCityByIndex)                
SLICFUNC(SFR_VOID, DetachRobot)                   
SLICFUNC(SFR_VOID, AttachRobot)                   

SLICFUNC(SFR_VOID, ForceRegard)                   
SLICFUNC(SFR_INT,  Cities)                        
SLICFUNC(SFR_INT,  AddPops)                       

SLICFUNC(SFR_VOID, KillUnit)                      
SLICFUNC(SFR_VOID, PlaySound)                     

SLICFUNC(SFR_INT,  CreateCity)                    
SLICFUNC(SFR_INT,  CreateCoastalCity)             
SLICFUNC(SFR_VOID, ExtractLocation)               

SLICFUNC(SFR_INT,  FindCoastalCity)               
SLICFUNC(SFR_VOID, Terraform)                     
SLICFUNC(SFR_VOID, PlantGood)                     
SLICFUNC(SFR_VOID, GetRandomNeighbor)             

SLICFUNC(SFR_VOID, GrantAdvance)                  
SLICFUNC(SFR_VOID, AddUnit)                       

SLICFUNC(SFR_VOID, AddExpelOrder)                 
SLICFUNC(SFR_VOID, AllUnitsCanBeExpelled)         
SLICFUNC(SFR_VOID, GetMessageClass)               
SLICFUNC(SFR_VOID, SetPlayer)                     

SLICFUNC(SFR_INT,  CityCollectingGood)            
SLICFUNC(SFR_INT,  GetNearestWater)               

SLICFUNC(SFR_INT,  IsPlayerAlive)                 

SLICFUNC(SFR_INT,  CityFoodDelta)                
SLICFUNC(SFR_INT,  PlayerWagesExp)                
SLICFUNC(SFR_INT,  PlayerWorkdayExp)                
SLICFUNC(SFR_INT,  PlayerRationsExp)                
SLICFUNC(SFR_INT,  PlayerWagesLevel)                
SLICFUNC(SFR_INT,  PlayerWorkdayLevel)                
SLICFUNC(SFR_INT,  PlayerRationsLevel)                
SLICFUNC(SFR_INT,  CityStarvationTurns)                

SLICFUNC(SFR_INT,  GameOver)                      

SLICFUNC(SFR_VOID, SaveGame)                      
SLICFUNC(SFR_VOID, LoadGame)                      

SLICFUNC(SFR_INT,  HasRiver)                      
SLICFUNC(SFR_VOID, SetScience)                    

SLICFUNC(SFR_VOID, SetResearching)                

SLICFUNC(SFR_INT,  IsInZOC)                       
SLICFUNC(SFR_VOID, DisableChooseResearch)         
SLICFUNC(SFR_VOID, EnableChooseResearch)          
SLICFUNC(SFR_VOID, SetTimerGranularity)           

SLICFUNC(SFR_VOID, QuitToLobby);                  
SLICFUNC(SFR_VOID, KillEyepointMessage);          

SLICFUNC(SFR_VOID, ClearBuildQueue);              

SLICFUNC(SFR_VOID, BreakLeaveOurLands);           
SLICFUNC(SFR_VOID, BreakNoPiracy);                
SLICFUNC(SFR_VOID, UseDirector);                  
SLICFUNC(SFR_VOID, ClearOrders);                  


SLICFUNC(SFR_VOID, SetUnit);                      
SLICFUNC(SFR_VOID, SetUnitByIndex);               

SLICFUNC(SFR_VOID, SetCity);                      
SLICFUNC(SFR_VOID, SetCityByIndex);               

SLICFUNC(SFR_VOID, SetLocation);                  
SLICFUNC(SFR_VOID, MakeLocation);                 

SLICFUNC(SFR_VOID, SetOrder);                     

SLICFUNC(SFR_VOID, Flood);                        
SLICFUNC(SFR_VOID, Ozone);                        
SLICFUNC(SFR_VOID, GodMode);                      
SLICFUNC(SFR_VOID, ExecuteAllOrders);             
SLICFUNC(SFR_VOID, CatchUp);                      
SLICFUNC(SFR_VOID, Deselect);                     

SLICFUNC(SFR_INT,  Preference);                   
SLICFUNC(SFR_INT,  SetPreference);                

SLICFUNC(SFR_VOID, AddMovement);                  
SLICFUNC(SFR_VOID, ToggleVeteran); 
SLICFUNC(SFR_INT,  IsVeteran);
SLICFUNC(SFR_VOID, CantAttackUnit);               
SLICFUNC(SFR_VOID, CantAttackCity);               
SLICFUNC(SFR_VOID, CityCantRiotOrRevolt);         
SLICFUNC(SFR_VOID, SelectUnit);                   
SLICFUNC(SFR_VOID, SelectCity);                   

SLICFUNC(SFR_VOID, CantEndTurn);                  
SLICFUNC(SFR_VOID, Heal);                         
SLICFUNC(SFR_VOID, AddGold);                      
SLICFUNC(SFR_VOID, SetActionKey);                 
SLICFUNC(SFR_INT,  GetCityByLocation);            

SLICFUNC(SFR_INT,  GetNeighbor);                  

SLICFUNC(SFR_VOID, DamageUnit);                   

SLICFUNC(SFR_INT,  IsUnitInBuildList)             
SLICFUNC(SFR_INT,  IsBuildingInBuildList)         
SLICFUNC(SFR_INT,  IsWonderInBuildList)           
SLICFUNC(SFR_INT,  IsEndgameInBuildList)          
SLICFUNC(SFR_INT,  IsBuildingAtHead)	          
SLICFUNC(SFR_INT,  IsWonderAtHead)		          

SLICFUNC(SFR_VOID, AddUnitToBuildList)            
SLICFUNC(SFR_VOID, AddBuildingToBuildList)        
SLICFUNC(SFR_VOID, AddWonderToBuildList)          
SLICFUNC(SFR_VOID, AddEndgameToBuildList)         
SLICFUNC(SFR_VOID, KillUnitFromBuildList)         
SLICFUNC(SFR_VOID, KillBuildingFromBuildList)     
SLICFUNC(SFR_VOID, KillWonderFromBuildList)       
SLICFUNC(SFR_VOID, KillEndgameFromBuildList)      
SLICFUNC(SFR_VOID, SetPW)                         
SLICFUNC(SFR_VOID, Stacked)                       

SLICFUNC(SFR_VOID, SetString)                     
SLICFUNC(SFR_VOID, SetStringByDBIndex)            
SLICFUNC(SFR_VOID, GetStringDBIndex)              

SLICFUNC(SFR_VOID, UnitHasUserFlag)               

SLICFUNC(SFR_VOID, BlankScreen)                   
SLICFUNC(SFR_VOID, AddCenter)                     
SLICFUNC(SFR_VOID, AddEffect)                     

SLICFUNC(SFR_INT,  PlayerCivilization)            
SLICFUNC(SFR_INT,  CivilizationIndex)             
SLICFUNC(SFR_VOID, ExitToDesktop)                 

SLICFUNC(SFR_INT,  Import)                        
SLICFUNC(SFR_VOID, Export)                        

SLICFUNC(SFR_INT, GetUnitFromArmy)               
SLICFUNC(SFR_INT, GetUnitByIndex)                
SLICFUNC(SFR_INT, GetArmyByIndex)                
SLICFUNC(SFR_INT, GetCityByIndex)                

SLICFUNC(SFR_INT, GetUnitsAtLocation)            
SLICFUNC(SFR_INT, GetUnitFromCell)              


SLICFUNC(SFR_INT, IsRoad)                        


SLICFUNC(SFR_INT, GetMapHeight)                  
SLICFUNC(SFR_INT, GetMapWidth)                   

SLICFUNC(SFR_VOID, AddFeat)                      


SLICFUNC(SFR_INT, IsFortress)                     

SLICFUNC(SFR_INT, Distance)                      
SLICFUNC(SFR_INT, SquaredDistance)               
SLICFUNC(SFR_INT, HasGood)                       


SLICFUNC(SFR_INT, GetRiotLevel)                  
SLICFUNC(SFR_INT, GetRevolutionLevel)            


SLICFUNC(SFR_INT, GetCurrentRound)				 
SLICFUNC(SFR_INT, GetCurrentYear)				 

SLICFUNC(SFR_INT, CityIsValid)                   

SLICFUNC(SFR_INT, CellOwner)                     

SLICFUNC(SFR_INT, CityIsNamed)					 


SLICFUNC(SFR_INT, StringCompare)				
SLICFUNC(SFR_INT, CityNameCompare)				
SLICFUNC(SFR_VOID, ChangeGlobalRegard)			 

SLICFUNC(SFR_VOID, SetAllCitiesVisible)			
SLICFUNC(SFR_VOID, SetCityVisible)				


SLICFUNC(SFR_INT, IsCivilian)					

SLICFUNC(SFR_VOID, FinishImprovements)         
SLICFUNC(SFR_VOID, RemoveAdvance)              
SLICFUNC(SFR_INT, PlayerGold)                  
SLICFUNC(SFR_INT, GetArmyFromUnit)				
SLICFUNC (SFR_INT, ClearBattleFlag)				
SLICFUNC(SFR_VOID, MinimizeAction)				
SLICFUNC(SFR_INT, IsUnitAtHead)					
SLICFUNC(SFR_VOID, OpenScenarioEditor)

//New Slicfunctions of CTP2.1 readded by Martin Gühmann
SLICFUNC(SFR_VOID, DestroyBuilding)
SLICFUNC(SFR_VOID, OpenBuildQueue)
SLICFUNC(SFR_INT, TileHasImprovement)
SLICFUNC(SFR_INT, PlayerHasWonder)
SLICFUNC(SFR_INT, WonderOwner)
SLICFUNC(SFR_INT, CityHasWonder)
SLICFUNC(SFR_INT, ArmyIsValid)
SLICFUNC(SFR_INT, GetRoundsToNextDisaster)
SLICFUNC(SFR_INT, GetCurrentPollutionLevel)
//New slicfunctions by The Big Mc
SLICFUNC(SFR_VOID, FreeAllSlaves);
SLICFUNC(SFR_VOID, AddSlaves);
//New slicfunctions by MrBaggins
SLICFUNC(SFR_VOID, PlantSpecificGood);
SLICFUNC(SFR_VOID, RemoveGood);
//New slicfunctions by Martin Gühmann
SLICFUNC(SFR_INT, CargoCapacity);
SLICFUNC(SFR_INT, MaxCargoSize);
SLICFUNC(SFR_INT, CargoSize);
SLICFUNC(SFR_INT, GetUnitFromCargo) 
SLICFUNC(SFR_INT, GetContinent)
SLICFUNC(SFR_INT, IsWater)
//New slicfunctions by Solver
SLICFUNC(SFR_INT, IsOnSameContinent)



#endif // __SLICFUNC_H__

