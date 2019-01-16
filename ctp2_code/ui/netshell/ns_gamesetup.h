//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  :
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Corrected strange access of non-static members from static data.
// - Make the Linux version loading and producing Windows compatible
//   savegames. (16-Jan-2019 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifndef __NS_GAMESETUP_H__
#define __NS_GAMESETUP_H__

#include "ns_object.h"
#include "ns_tribes.h"
#include "ns_units.h"
#include "ns_improvements.h"
#include "ns_wonders.h"

#define k_NS_MAX_HUMANS		4
#define k_NS_MAX_PLAYERS	8

struct TribeSlot
{
	sint8	isFemale;
	sint8	isAI;
	uint16	key;
	sint32	tribe;
};

class nf_GameSetup : public NETFunc::GameSetup
{
public:

	static sint32 m_version;

	nf_GameSetup(NETFunc::KeyStruct *k);
	nf_GameSetup(NETFunc::Game *g);
	nf_GameSetup(void);
	void Init( void );
	void SetKey(NETFunc::KeyStruct *k);
	void Pack();
	void Unpack();

	void Reset() {}

	sint32 GetPlayStyle     ()                      const { return m_playstyle;                   }
	void   SetPlayStyle     (sint32 playstyle)            {        m_playstyle = playstyle;       }

	sint32 GetPlayStyleValue()                      const { return m_playstylevalue;              }
	void   SetPlayStyleValue(sint32 playstylevalue)       { m_playstylevalue = playstylevalue;    }

	sint8  GetHandicapping  ()                      const { return m_handicapping;                }
	void   SetHandicapping  (sint8  handicapping)         {        m_handicapping = handicapping; }

	sint8  GetDynamicJoin   ()                      const { return m_dynamicJoin;                 }
	void   SetDynamicJoin   (sint8 dynamicJoin)           {        m_dynamicJoin = dynamicJoin;   }

	sint8  GetBloodlust     ()                      const { return m_bloodlust;                   }
	void   SetBloodlust     (sint8  bloodlust)            {        m_bloodlust = bloodlust;       }

	sint8  GetPollution     ()                      const { return m_pollution;                   }
	void   SetPollution     (sint8  pollution)            {        m_pollution = pollution;       }

	sint32 GetCivPoints     ()                      const { return m_civPoints;                   }
	void   SetCivPoints     (sint32 civPoints)            {        m_civPoints = civPoints;       }

	sint32 GetPwPoints      ()                      const { return m_pwPoints;                    }
	void   SetPwPoints      (sint32 pwPoints)             {        m_pwPoints = pwPoints;         }

	sint8  GetStartAge      ()                      const { return m_startAge;                    }
	void   SetStartAge      (sint8  startAge)             {        m_startAge = startAge;         }

	sint8  GetEndAge        ()                      const { return m_endAge;                      }
	void   SetEndAge        (sint8  endAge)               {        m_endAge = endAge;             }

	sint8  GetMapSize       ()                      const { return m_mapSize;                     }
	void   SetMapSize       (sint8  mapSize)              {        m_mapSize = mapSize;           }

	sint8  GetWorldType1    ()                      const { return m_worldType1;                  }
	void   SetWorldType1    (sint8  worldType1)           {        m_worldType1 = worldType1;     }

	sint8  GetWorldType2    ()                      const { return m_worldType2;                  }
	void   SetWorldType2    (sint8  worldType2)           {        m_worldType2 = worldType2;     }

	sint8  GetWorldType3    ()                      const { return m_worldType3;                  }
	void   SetWorldType3    (sint8  worldType3)           {        m_worldType3 = worldType3;     }

	sint8  GetWorldType4    ()                      const { return m_worldType4;                  }
	void   SetWorldType4    (sint8  worldType4)           {        m_worldType4 = worldType4;     }

	sint8  GetWorldType5    ()                      const { return m_worldType5;                  }
	void   SetWorldType5    (sint8  worldType5)           {        m_worldType5 = worldType5;     }

	sint8  GetWorldType6    ()                      const { return m_worldType6;                  }
	void   SetWorldType6    (sint8  worldType6)           {        m_worldType6 = worldType6;     }

	sint8  GetWorldShape    ()                      const { return m_worldShape;                  }
	void   SetWorldShape    (sint8  worldShape)           {        m_worldShape = worldShape;     }

	sint8  GetDifficulty1   ()                      const { return m_difficulty1;                 }
	void   SetDifficulty1   (sint8  difficulty1)          {        m_difficulty1 = difficulty1;   }

	sint8  GetDifficulty2   ()                      const { return m_difficulty2;                 }
	void   SetDifficulty2   (sint8  difficulty2)          {        m_difficulty2 = difficulty2;   }

	TribeSlot *GetTribeSlots() const { return (TribeSlot *)m_tribeSlots; }
	void SetTribeSlot(sint32 slot, BOOL isFemale, BOOL isAI, uint16 key, sint32 tribe)
	{
		m_tribeSlots[ slot ].isFemale = (sint8)isFemale;
		m_tribeSlots[ slot ].isAI = (sint8)isAI;
		m_tribeSlots[ slot ].key = key;
		m_tribeSlots[ slot ].tribe = tribe;
	}

	TribeSlot *GetSavedTribeSlots() const
	{ return (TribeSlot *)m_savedTribeSlots; }
	void SetSavedTribeSlot(sint32 slot, BOOL isFemale, BOOL isAI, uint16 key, sint32 tribe)
	{
		m_savedTribeSlots[ slot ].isFemale = (sint8)isFemale;
		m_savedTribeSlots[ slot ].isAI = (sint8)isAI;
		m_savedTribeSlots[ slot ].key = key;
		m_savedTribeSlots[ slot ].tribe = tribe;
	}

	sint8 GetUnit(sint32 i) const
	{ return static_cast<sint8>(m_excludedUnits[ i >> 3 ] & ( 1 << ( (uint32)i % 8 ) )); }
	void SetUnit(sint8 unit, int i )
	{ unit ?
		m_excludedUnits[ i >> 3 ] |= ( 1 << ( (uint32)i % 8 ) ) :
		m_excludedUnits[ i >> 3 ] &= ~( 1 << ( (uint32)i % 8 ) ); }

	sint32  GetNumAvailUnits( void ) const { return m_numAvailUnits; }
	void    SetNumAvailUnits( sint32 numAvailUnits )
	{ m_numAvailUnits = numAvailUnits; }

	sint8 GetImprovement( sint32 i ) const
	{ return static_cast<sint8>(m_excludedImprovements[ i >> 3 ] & ( 1 << ( (uint32)i % 8 ) )); }
	void SetImprovement(sint8 improvement, sint32 i )
	{ improvement ?
		m_excludedImprovements[ i >> 3 ] |= ( 1 << ( (uint32)i % 8 ) ) :
		m_excludedImprovements[ i >> 3 ] &= ~( 1 << ( (uint32)i % 8 ) ); }

	sint32  GetNumAvailImprovements()                            const {return m_numAvailImprovements;                        }
	void    SetNumAvailImprovements(sint32 numAvailImprovements)       {       m_numAvailImprovements = numAvailImprovements; }

	sint8 GetWonder( sint32 i ) const
	{ return static_cast<sint8>(m_excludedWonders[ i >> 3 ] & ( 1 << ( (uint32)i % 8 ) )); }
	void SetWonder(sint8 wonder, sint32 i )
	{ wonder ?
		m_excludedWonders[ i >> 3 ] |= ( 1 << ( (uint32)i % 8 ) ) :
		m_excludedWonders[ i >> 3 ] &= ~( 1 << ( (uint32)i % 8 ) ); }

	sint32 GetNumAvailWonders()                       const { return m_numAvailWonders;                   }
	void   SetNumAvailWonders(sint32 numAvailWonders)       {        m_numAvailWonders = numAvailWonders; }

	void   SetSavedId( uint32 savedId );
	uint32 GetSavedId( void );

	void   WriteToFile(FILE *saveFile) const;
	void   ReadFromFile(FILE *saveFile);

private:
	sint32 m_playstyle;
	sint32 m_playstylevalue;

	sint8 m_handicapping;
	sint8 m_dynamicJoin;

	sint8 m_bloodlust;

	sint8 m_pollution;
	sint32 m_civPoints;
	sint32 m_pwPoints;

	sint8 m_startAge;
	sint8 m_endAge;
	sint8 m_mapSize;
	sint8 m_worldType1;
	sint8 m_worldType2;
	sint8 m_worldType3;
	sint8 m_worldType4;
	sint8 m_worldType5;
	sint8 m_worldType6;
	sint8 m_worldShape;
	sint8 m_difficulty1;
	sint8 m_difficulty2;

	TribeSlot m_tribeSlots[ k_NS_MAX_PLAYERS ];
	TribeSlot m_savedTribeSlots[ k_NS_MAX_PLAYERS ];

	sint32 m_numAvailUnits;
	sint8 m_excludedUnits[ ( k_UNITS_MAX >> 3 ) + 1 ];
	sint32 m_numAvailImprovements;
	sint8 m_excludedImprovements[ ( k_IMPROVEMENTS_MAX >> 3 ) + 1 ];
	sint32 m_numAvailWonders;
	sint8 m_excludedWonders[ ( k_WONDERS_MAX >> 3 ) + 1 ];
};

class ns_GameSetup : public ns_Object<nf_GameSetup, ns_GameSetup>
{
public:
	ns_GameSetup(nf_GameSetup * gamesetup);
	void Update( nf_GameSetup *gamesetup );

	MBCHAR *m_name;
};

#endif
