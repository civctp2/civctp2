
#pragma once

#ifndef __IC3EndGameDB_H__
#define __IC3EndGameDB_H__ 1


#include <OBJBASE.H>  


DEFINE_GUID(CLSID_IC3EndGameDB, 
    0xd52face0,
    0xb6ab,
    0x11d2,
    0xb7, 0x12, 0x00, 0x08, 0xc7, 0x89, 0xa8, 0x06
);

#undef INTERFACE
#define INTERFACE IC3EndGameDB

class MapPoint;
struct MapPointData; 
DECLARE_INTERFACE_(IC3EndGameDB, IUnknown)
{
 	

    STDMETHOD_(sint32, GetNumObjectTypes)(THIS_) PURE;    

    STDMETHOD_ (sint32, End_Game_Object_String_To_Int)(THIS_ char * objecttype_string) PURE;
    STDMETHOD_ (char *, End_Game_Object_Int_To_String)(THIS_ sint32 index) PURE;
	
	STDMETHOD_ (BOOL, RequiresProbeRecovery)(THIS_ sint32 object_id)  PURE;
	STDMETHOD_ (BOOL, ControlsSpeed)(THIS_ sint32 object_id)  PURE;
	STDMETHOD_ (BOOL, ExactlyOneRequired)(THIS_ sint32 object_id)  PURE;

	
	STDMETHOD_ (double, RushBuyModifier)(THIS_ sint32 object_id) PURE ;
	STDMETHOD_ (sint32, GetCataclysmNum)(THIS_ sint32 object_id) PURE ;
	STDMETHOD_ (sint32, GetCataclysmPercent)(THIS_ sint32 object_id) PURE ;
	STDMETHOD_ (sint32, NotifyLabBuilt)(THIS_ sint32 object_id) PURE ;
	STDMETHOD_ (sint32, GetSoundID)(THIS_ sint32 object_id) PURE ;
	STDMETHOD_ (sint32, StartsSequence)(THIS_ sint32 object_id) PURE ;
	STDMETHOD_ (sint32, RequiresLab)(THIS_ sint32 object_id) PURE ;
	STDMETHOD_ (sint32, GetCost)(THIS_ sint32 object_id) PURE ;

	
	STDMETHOD_ (sint32, GetTurnsPerStage)(THIS_ sint32 object_id, sint32 numBuilt) PURE;
	STDMETHOD_ (sint32, RequiredForStage)(THIS_ sint32 object_id, sint32 stage) PURE;
	STDMETHOD_ (sint32, RequiredToAdvanceFromStage)(THIS_ sint32 object_id, sint32 stage) PURE;
};

#endif __IC3EndGameDB_H__




