
#include "c3.h"



#include "Globals.h"
#include "C3GameState.h"

#include "C3EndGameDB.h"


#include "Player.h"
#include "EndGameDB.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "CivArchive.h"
#include "MapPoint.h"

#include "StrDB.h"

extern EndGameDatabase *g_theEndGameDB; 
extern World *g_theWorld; 
extern StringDB *g_theStringDB;

#include "ConstDB.h"
extern ConstDB *g_theConstDB; 

#include "Readiness.h"

STDMETHODIMP C3EndGameDB::QueryInterface(REFIID riid, void **obj)
{
	*obj = NULL;

	if(IsEqualIID(riid, IID_IUnknown)) {
		*obj = (IUnknown *)this;
		AddRef();
		return S_OK;
	} else if(IsEqualIID(riid, CLSID_IC3EndGameDB)) {
		*obj = (IC3EndGameDB*)this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) C3EndGameDB::AddRef()
{
	return ++m_refCount;
}

STDMETHODIMP_(ULONG) C3EndGameDB::Release()
{
	if(--m_refCount)
		return m_refCount;
	delete this;
	return 0;
}


C3EndGameDB::C3EndGameDB(Player *p)
{
   	m_refCount = 0;
    m_player = p;
} 

C3EndGameDB::C3EndGameDB(Player *p, CivArchive &archive)
{
    m_player = p;
    Serialize(archive); 
} 

void C3EndGameDB::Serialize(CivArchive &archive) 
{
    CHECKSERIALIZE

    if (archive.IsStoring()) { 
        archive << m_refCount; 
    } else { 
        archive >> m_refCount; 
    } 
}

STDMETHODIMP_ (sint32) C3EndGameDB::GetNumObjectTypes ()
{
    
	return g_theEndGameDB->GetNumRec();
}

STDMETHODIMP_ (sint32) C3EndGameDB::End_Game_Object_String_To_Int(char *object_string)
{
	StringId str_id;
	sint32 index;

	
	if (g_theStringDB->GetStringID(object_string, str_id))
	{
		
		if (g_theEndGameDB->GetNamedItem(str_id, index))
		{
			
			return index;
		} 
	} 

	Assert(false);

	return -1;
}

STDMETHODIMP_ (char *) C3EndGameDB::End_Game_Object_Int_To_String(sint32 index)
{
	StringId str_id;
	char * end_game_string;

	
	if (g_theEndGameDB->GetNamedItemID(index, str_id))
	{
		
		end_game_string = (char *) g_theStringDB->GetNameStr(str_id);
	}

	return end_game_string;
}


STDMETHODIMP_ (BOOL) C3EndGameDB::RequiresProbeRecovery(sint32 object_id)
{
	return g_theEndGameDB->Get(object_id)->RequiresProbeRecovery();
}

STDMETHODIMP_ (BOOL) C3EndGameDB::ControlsSpeed(sint32 object_id)
{
	return g_theEndGameDB->Get(object_id)->ControlsSpeed();
}

STDMETHODIMP_ (BOOL) C3EndGameDB::ExactlyOneRequired(sint32 object_id)
{
	return g_theEndGameDB->Get(object_id)->ExactlyOneRequired();
}



STDMETHODIMP_ (double) C3EndGameDB::RushBuyModifier(sint32 object_id)
{
	return g_theEndGameDB->Get(object_id)->RushBuyModifier();
}

STDMETHODIMP_ (sint32) C3EndGameDB::GetCataclysmNum(sint32 object_id)
{
	return g_theEndGameDB->Get(object_id)->GetCataclysmNum();
}

STDMETHODIMP_ (sint32) C3EndGameDB::GetCataclysmPercent(sint32 object_id)
{
	return g_theEndGameDB->Get(object_id)->GetCataclysmPercent();
}

STDMETHODIMP_ (sint32) C3EndGameDB::NotifyLabBuilt(sint32 object_id)
{
	return g_theEndGameDB->Get(object_id)->NotifyLabBuilt();
}

STDMETHODIMP_ (sint32) C3EndGameDB::GetSoundID(sint32 object_id)
{
	return g_theEndGameDB->Get(object_id)->GetSoundID();
}

STDMETHODIMP_ (sint32) C3EndGameDB::StartsSequence(sint32 object_id)
{
	return g_theEndGameDB->Get(object_id)->StartsSequence();
}

STDMETHODIMP_ (sint32) C3EndGameDB::RequiresLab(sint32 object_id)
{
	return g_theEndGameDB->Get(object_id)->RequiresLab();
}

STDMETHODIMP_ (sint32) C3EndGameDB::GetCost(sint32 object_id)
{
	return g_theEndGameDB->Get(object_id)->GetCost();
}


STDMETHODIMP_ (sint32) C3EndGameDB::GetTurnsPerStage(sint32 object_id, sint32 numBuilt)
{
	return g_theEndGameDB->Get(object_id)->GetTurnsPerStage(numBuilt);
}

STDMETHODIMP_ (sint32) C3EndGameDB::RequiredForStage(sint32 object_id, sint32 stage)
{
	return g_theEndGameDB->Get(object_id)->GetTurnsPerStage(stage);
}

STDMETHODIMP_ (sint32) C3EndGameDB::RequiredToAdvanceFromStage(sint32 object_id, sint32 stage)
{
	return g_theEndGameDB->Get(object_id)->RequiredToAdvanceFromStage(stage);
}
