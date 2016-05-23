//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Unit
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
// CTP1_TRADE
// - Creates an executable with trade like in CTP1. Currently broken.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #pragma marked as Microsoft extension.
// - IsValid marked as const.
// - GetDisplayName added.
// - Made GetFuel method const - April 24th 2005 Martin Gühmann
// - Added NeedsRefueling method to remove code duplications.
//   - April 24th 2005 Martin Gühmann
// - Moved UnitValidForOrder from ArmyData to be able to access the Unit
//   properties as well. - April 24th 2005 Martin Gühmann
// - Removed some unsused method to removed some unused in methods in
//   CityData.. - Aug 6th 2005 Martin Gühmann
// - Removed another unused and unecessary function. (Aug 12th 2005 Martin Gühmann)
// - Moved sinking and upgrade functionality from ArmyData. (Dec 24th 2006 Martin Gühmann)
// - Modified sink to take a unit so the Slic identifies what sank - E 5-24-2007
// - Added an IsInVisionRange test. (25-Jan-2008 Martin Gühmann)
// - Added check move points option to CanAtLeastOneCargoUnloadAt (8-Feb-2008 Martin Gühmann).
// - Separated the Settle event drom the Settle in City event. (19-Feb-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __UNITENUMS_H__
#define __UNITENUMS_H__ 1

enum OPTIMISE_STATE
{
	OPTIMISE_STATE_NONE,
	OPTIMISE_STATE_FOOD,
	OPTIMISE_STATE_PRODUCTION,
	OPTIMISE_STATE_GOLD,
	OPTIMISE_STATE_HAPPINESS,
	OPTIMISE_STATE_SCIENCE,
	OPTIMISE_STATE_INVALID,
} ;

enum CITY_ATTITUDE
{
	CITY_ATTITUDE_CONTENT,
	CITY_ATTITUDE_WE_LOVE_THE_KING,
	CITY_ATTITUDE_HAPPY,
	CITY_ATTITUDE_DISORDER,
};

enum RADIUS_OP
{
	RADIUS_OP_UKNOWN = -1,
	RADIUS_OP_REMOVE_IMPROVEMENTS = 0,
	RADIUS_OP_KILL_UNITS,
	RADIUS_OP_RESET_OWNER,
	RADIUS_OP_KILL_TILE,
	RADIUS_OP_ADD_GOODS,
	RADIUS_OP_COUNT_GOODS,
};

enum CONVERTED_BY
{
	CONVERTED_BY_CLERIC,
	CONVERTED_BY_TELEVANGELIST,
	CONVERTED_BY_NOTHING
};

enum UPRISING_CAUSE
{
	UPRISING_CAUSE_NONE,
	UPRISING_CAUSE_SLAVE_STARVED,
	UPRISING_CAUSE_UNGUARDED_SLAVES,
	UPRISING_CAUSE_INCITED,
	UPRISING_CAUSE_INTERNAL,
};

enum POP_TYPE
{
	POP_WORKER,
	POP_SCIENTIST,
	POP_ENTERTAINER,
	POP_FARMER,
	POP_LABORER,
	POP_MERCHANT,
	POP_SLAVE,
	POP_MAX
};

enum UNIT_COMMAND
{
	UNIT_COMMAND_NEAREST_CITY,
	UNIT_COMMAND_NEAREST_FORT,
	UNIT_COMMAND_NEAREST_AIRFIELD,
	UNIT_COMMAND_ENTRENCH,
	UNIT_COMMAND_SLEEP
};

enum UNIT_ORDER_TYPE
{
	UNIT_ORDER_NONE,                         //  0
	UNIT_ORDER_MOVE,                         //  1
	UNIT_ORDER_PATROL,                       //  2
	UNIT_ORDER_CIRCULAR_PATROL,              //  3
	UNIT_ORDER_ENTRENCH,                     //  4
	UNIT_ORDER_SLEEP,                        //  5
	UNIT_ORDER_DETRENCH,                     //  6
	UNIT_ORDER_UNLOAD,                       //  7
	UNIT_ORDER_MOVE_TO,                      //  8
	UNIT_ORDER_TELEPORT_TO,                  //  9

	UNIT_ORDER_EXPEL_TO,                     // 10
	UNIT_ORDER_GROUP,                        // 11
	UNIT_ORDER_UNGROUP,                      // 12
	UNIT_ORDER_INVESTIGATE_CITY,             // 13
	UNIT_ORDER_NULLIFY_WALLS,                // 14
	UNIT_ORDER_STEAL_TECHNOLOGY,             // 15
	UNIT_ORDER_INCITE_REVOLUTION,            // 16
	UNIT_ORDER_ASSASSINATE,                  // 17
	UNIT_ORDER_INVESTIGATE_READINESS,        // 18
	UNIT_ORDER_BOMBARD,                      // 19

	UNIT_ORDER_SUE,                          // 20
	UNIT_ORDER_FRANCHISE,                    // 21
	UNIT_ORDER_SUE_FRANCHISE,                // 22
	UNIT_ORDER_EXPEL,                        // 23
	UNIT_ORDER_ESTABLISH_EMBASSY,            // 24
	UNIT_ORDER_THROW_PARTY,                  // 25
	UNIT_ORDER_CAUSE_UNHAPPINESS,            // 26
	UNIT_ORDER_PLANT_NUKE,                   // 27
	UNIT_ORDER_SLAVE_RAID,                   // 28
	UNIT_ORDER_ENSLAVE_SETTLER,              // 29

	UNIT_ORDER_UNDERGROUND_RAILWAY,          // 30
	UNIT_ORDER_INCITE_UPRISING,              // 31
	UNIT_ORDER_BIO_INFECT,                   // 32
	UNIT_ORDER_NANO_INFECT,                  // 33
	UNIT_ORDER_CONVERT,                      // 34
	UNIT_ORDER_REFORM,                       // 35
	UNIT_ORDER_INDULGENCE,                   // 36
	UNIT_ORDER_SOOTHSAY,                     // 37
	UNIT_ORDER_CREATE_PARK,                  // 38
	UNIT_ORDER_PILLAGE,                      // 39

	UNIT_ORDER_INJOIN,                       // 40
	UNIT_ORDER_INTERCEPT_TRADE,              // 41
	UNIT_ORDER_PARADROP_MOVE,                // 42
	UNIT_ORDER_SET_UNLOAD_MOVEMENT_POINTS,   // 43
	UNIT_ORDER_GROUP_UNIT,                   // 44
	UNIT_ORDER_DISBAND,                      // 45
	UNIT_ORDER_FINISH_ATTACK,                // 46
	UNIT_ORDER_UNLOAD_ONE_UNIT,              // 47
	UNIT_ORDER_BOARD_TRANSPORT,              // 48
	UNIT_ORDER_WAKE_UP,                      // 49

	UNIT_ORDER_PILLAGE_UNCONDITIONALLY,      // 50
	UNIT_ORDER_MOVE_THEN_UNLOAD,             // 51
	UNIT_ORDER_ADVERTISE,                    // 52
	UNIT_ORDER_INFORM_AI_CAPTURE_CITY,       // 53
	UNIT_ORDER_UNLOAD_SELECTED_STACK,        // 54
	UNIT_ORDER_ADD_EVENT,                    // 55
	UNIT_ORDER_SETTLE,                       // 56
	UNIT_ORDER_LAUNCH,                       // 57
	UNIT_ORDER_TARGET,                       // 58
	UNIT_ORDER_CLEAR_TARGET,                 // 59

	UNIT_ORDER_PLAGUE,                       // 60
	UNIT_ORDER_VICTORY_MOVE,                 // 61
	UNIT_ORDER_SETTLE_IN_CITY,               // 62
	UNIT_ORDER_UPGRADE,                      // 63

	// Add new orders above this line

	UNIT_ORDER_MAX                           // 64
};

enum SPECATTACK
{
	SPECATTACK_NONE = -1,

	SPECATTACK_NUKE,
	SPECATTACK_HEARGOSSIP,
	SPECATTACK_THROWPARTY,
	SPECATTACK_ESTABLISHEMBASSY,
	SPECATTACK_INCITEREVOLUTION,
	SPECATTACK_BOMBCABINET,
	SPECATTACK_CREATEFRANCHISE,
	SPECATTACK_CAUSEUNHAPPINESS,
	SPECATTACK_CONDUCTHIT,
	SPECATTACK_BIOTERROR,
	SPECATTACK_NANOTERROR,
	SPECATTACK_SLAVERAID,
	SPECATTACK_ENSLAVESETTLER,
	SPECATTACK_SLAVEUPRISING,
	SPECATTACK_FREESLAVES,
	SPECATTACK_SELLINDULGENCE,
	SPECATTACK_CONVERTCITY,
	SPECATTACK_PLANTNUKE,
	SPECATTACK_SOOTHSAY,
	SPECATTACK_CREATEPARK,
	SPECATTACK_INJOIN,
	SPECATTACK_SPY,
	SPECATTACK_STEALTECH,
	SPECATTACK_REVOLUTION,
	SPECATTACK_REFORMCITY,

	SPECATTACK_MAX
};

#endif
