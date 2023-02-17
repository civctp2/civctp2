//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : AI cause
// Id           : $Id$
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
//----------------------------------------------------------------------------
//
// Compiler flags
//
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - None
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __AI_CAUSE_H__
#define __AI_CAUSE_H__ 1

enum CAUSE_REMOVE_ARMY
{
	CAUSE_REMOVE_ARMY_UNKNOWN,                     //  0
	CAUSE_REMOVE_ARMY_FLOOD,                       //  1

	CAUSE_REMOVE_ARMY_ATTACKED,                    //  2
	CAUSE_REMOVE_ARMY_CHEAT,                       //  3

	CAUSE_REMOVE_ARMY_TRANSPORTED,                 //  4
	CAUSE_REMOVE_ARMY_GROUPING,                    //  5
	CAUSE_REMOVE_ARMY_UNGROUPING,                  //  6
	CAUSE_REMOVE_ARMY_RAIL_LAUNCH,                 //  7
	CAUSE_REMOVE_ARMY_PARADROP,                    //  8
	CAUSE_REMOVE_ARMY_SETTLE,                      //  9
	CAUSE_REMOVE_ARMY_STARVATION,                  // 10
	CAUSE_REMOVE_ARMY_PARKRANGER,                  // 11
	CAUSE_REMOVE_ARMY_NUKE,                        // 12
	CAUSE_REMOVE_ARMY_BOMBARD,                     // 13
	CAUSE_REMOVE_ARMY_SUE,                         // 14
	CAUSE_REMOVE_ARMY_POLLUTION,                   // 15
	CAUSE_REMOVE_ARMY_UPRISING,                    // 16
	CAUSE_REMOVE_ARMY_SLAVE_UPRISING,              // 17
	CAUSE_REMOVE_ARMY_ENSLAVED,                    // 18
	CAUSE_REMOVE_ARMY_DIPLOMACY,                   // 19

	CAUSE_REMOVE_ARMY_NO_MAT_SUPPORT,              // 20

	CAUSE_REMOVE_ARMY_DIED_IN_ATTACK,              // 21
	CAUSE_REMOVE_ARMY_DIED_IN_ATTACK_ON_TOP,       // 22
	CAUSE_REMOVE_ARMY_DIED_IN_SPYING,              // 23
	CAUSE_REMOVE_ARMY_DIED_IN_SLAVERAID,           // 24
	CAUSE_REMOVE_ARMY_DIED_IN_UNDERGROUND_RR_RAID, // 25
	CAUSE_REMOVE_ARMY_DIED_IN_CONVERT_RAID,        // 26
	CAUSE_REMOVE_ARMY_DIED_IN_REFORMATION,         // 27
	CAUSE_REMOVE_ARMY_COUNTERBOMBARD,              // 28
	CAUSE_REMOVE_ARMY_OUTOFFUEL,                   // 29
	CAUSE_REMOVE_ARMY_REENTRY,                     // 30
	CAUSE_REMOVE_ARMY_GOVERNMENT_CHANGE,           // 31
	CAUSE_REMOVE_ARMY_TOE,                         // 32
	CAUSE_REMOVE_ARMY_RUSTLE,                      // 33
	CAUSE_REMOVE_ARMY_LAND_MINES,                  // 34
	CAUSE_REMOVE_ARMY_TRANSPORT_DIED,              // 35
	CAUSE_REMOVE_ARMY_DIDNT_NEED_IT_AFTER_ALL,     // 36
	CAUSE_REMOVE_ARMY_DISBANDED,                   // 37

	CAUSE_REMOVE_ARMY_PARADROP_COMPLETE,           // 38
	CAUSE_REMOVE_ARMY_NUKES_ELIMINATED,            // 39
	CAUSE_REMOVE_ARMY_EMANCIPATION,                // 40
	CAUSE_REMOVE_ARMY_ILLEGAL_CELL,                // 41
	CAUSE_REMOVE_ARMY_EXPELLED_NO_CITIES,          // 42
	CAUSE_REMOVE_ARMY_SCENARIO,                    // 43
	CAUSE_REMOVE_ARMY_PROBE_RECOVERED,             // 44
	CAUSE_REMOVE_ARMY_SINGLE_USE,                  // 45
	CAUSE_REMOVE_ARMY_ACTIVE_DEFENSE,              // 46
	CAUSE_REMOVE_ARMY_TRADE                        // 47
};

enum CAUSE_NEW_ARMY
{
	CAUSE_NEW_ARMY_UNKNOWN,                        //  0
	CAUSE_NEW_ARMY_INITIAL,                        //  1
	CAUSE_NEW_ARMY_TRANSPORTED,                    //  2
	CAUSE_NEW_ARMY_GROUPING,                       //  3
	CAUSE_NEW_ARMY_UNGROUPING,                     //  4
	CAUSE_NEW_ARMY_BUILT,                          //  5
	CAUSE_NEW_ARMY_PARADROP,                       //  6
	CAUSE_NEW_ARMY_SPACE_LAUNCH,                   //  7
	CAUSE_NEW_ARMY_UPRISING,                       //  8
	CAUSE_NEW_ARMY_GOODY_HUT,                      //  9
	CAUSE_NEW_ARMY_TRADER,                         // 10
	CAUSE_NEW_ARMY_RUSTLE,                         // 11
	CAUSE_NEW_ARMY_DIPLOMACY,                      // 12
	CAUSE_NEW_ARMY_NETWORK,                        // 13
	CAUSE_NEW_ARMY_CARGO_POD,                      // 14
	CAUSE_NEW_ARMY_EXPELLED,                       // 15
	CAUSE_NEW_ARMY_ENTERED_WORMHOLE,               // 16
	CAUSE_NEW_ARMY_CHEAT,                          // 17
	CAUSE_NEW_ARMY_AI_TRANSPORT_CHEAT,             // 18
	CAUSE_NEW_ARMY_DISBANDED_CITY,                 // 19
	CAUSE_NEW_ARMY_SCENARIO,                       // 20
	CAUSE_NEW_ARMY_UNGROUPING_ORDER,               // 21
	CAUSE_NEW_ARMY_REMOTE_GROUPING,                // 22
	CAUSE_NEW_ARMY_REMOTE_UNGROUPING               // 23
};

enum CAUSE_REMOVE_CITY
{
	CAUSE_REMOVE_CITY_UNKNOWN,                     //  0
	CAUSE_REMOVE_CITY_FLOOD,                       //  1
	CAUSE_REMOVE_CITY_HAPPINESS_REVOLT,            //  2
	CAUSE_REMOVE_CITY_SLAVE_UPRISING,              //  3
	CAUSE_REMOVE_CITY_ATTACK,                      //  4
	CAUSE_REMOVE_CITY_DIPLOMACY,                   //  5
	CAUSE_REMOVE_CITY_CHEAT,                       //  6
	CAUSE_REMOVE_CITY_AI_ENTITY_REVOLT,            //  7
	CAUSE_REMOVE_CITY_NO_PEOPLE                    //  8
};

enum CAUSE_NEW_CITY
{
	CAUSE_NEW_CITY_UNKNOWN,                        //  0
	CAUSE_NEW_CITY_SETTLE,                         //  1
	CAUSE_NEW_CITY_GOODY_HUT,                      //  2
	CAUSE_NEW_CITY_REVOLT,                         //  3
	CAUSE_NEW_CITY_CONQUEST,                       //  4
	CAUSE_NEW_CITY_SPACE_LADDER,                   //  5
	CAUSE_NEW_CITY_CHEAT,                          //  6
	CAUSE_NEW_CITY_DIPLOMACY,                      //  7
	CAUSE_NEW_CITY_SLAVE_UPRISING,                 //  8
	CAUSE_NEW_CITY_UPRISING,                       //  9
	CAUSE_NEW_CITY_INITIAL,                        // 10
	CAUSE_NEW_CITY_SCENARIO                        // 11
};

enum CAUSE_SCI
{
	CAUSE_SCI_UNKNOWN,                             //  0
	CAUSE_SCI_RESEARCH,                            //  1
	CAUSE_SCI_GOODY,                               //  2
	CAUSE_SCI_DIPLOMACY,                           //  3
	CAUSE_SCI_COMBAT,                              //  4
	CAUSE_SCI_WONDER,                              //  5
	CAUSE_SCI_CHEAT,                               //  6
	CAUSE_SCI_INITIAL                              //  7
};

enum CAUSE_NEW_PLAYER
{
	CAUSE_NEW_PLAYER_REVOLT,                       //  0
	CAUSE_NEW_PLAYER_NETWORK,                      //  1
	CAUSE_NEW_PLAYER_UNKNOWN                       //  2
};

enum ERR_BUILD_INST
{
	ERR_BUILD_INST_OK,                             //  0
	ERR_BUILD_INST_OVERWRITE,                      //  1
	ERR_BUILD_INST_UPGRADE,                        //  2
	ERR_BUILD_INST_BLOCKED,                        //  3
	ERR_BUILD_INST_REDUNDENT                       //  4
};

enum CAUSE_SPECIAL_ATTACK
{
	CAUSE_SPECIAL_ATTACK_BOMBARD,                  //  0
	CAUSE_SPECIAL_ATTACK_SUE,                      //  1
	CAUSE_SPECIAL_ATTACK_SPYING,                   //  2
	CAUSE_SPECIAL_ATTACK_SLAVERAID,                //  3
	CAUSE_SPECIAL_ATTACK_UNDERGROUND_RR_RAID,      //  4
	CAUSE_SPECIAL_ATTACK_INCITE_REVOLUTION,        //  5
	CAUSE_SPECIAL_ATTACK_STEAL_TECHNOLOGY,         //  6
	CAUSE_SPECIAL_ATTACK_INJOIN,                   //  7
	CAUSE_SPECIAL_ATTACK_ASSISINATE,               //  8
	CAUSE_SPECIAL_ATTACK_CONVERT,                  //  9
	CAUSE_SPECIAL_ATTACK_BIO_INFECT,               // 10
	CAUSE_SPECIAL_ATTACK_NANO_INFECT               // 11
};

enum CAUSE_HIDDEN_DETECT
{
	CAUSE_HIDDEN_DETECT_MINES,                     //  0
	CAUSE_HIDDEN_DETECT_STEALTH,                   //  1
	CAUSE_HIDDEN_DETECT_UNKNOWN                    //  2
};

#endif // __AI_CAUSE_H__
