#pragma once

#ifndef __C3EndGameDB_H__
#define __C3EndGameDB_H__ 1

#include "ic3endgamedb.h"

class Player;
class CivArchive;

class C3EndGameDB : public IC3EndGameDB
{
	ULONG m_refCount;

    Player *m_player;

public:

	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

    C3EndGameDB(Player *p);
    C3EndGameDB(Player *p, CivArchive &archive);
    void Serialize(CivArchive &archive);

    STDMETHODIMP_(sint32) GetNumObjectTypes ();

    STDMETHODIMP_ (sint32) End_Game_Object_String_To_Int(char * object_string);
    STDMETHODIMP_ (char *) End_Game_Object_Int_To_String(sint32 index);

	STDMETHODIMP_ (BOOL) RequiresProbeRecovery(sint32 object_id);
	STDMETHODIMP_ (BOOL) ControlsSpeed(sint32 object_id);
	STDMETHODIMP_ (BOOL) ExactlyOneRequired(sint32 object_id);

	STDMETHODIMP_ (double) RushBuyModifier(sint32 object_id);
	STDMETHODIMP_ (sint32) GetCataclysmNum(sint32 object_id);
	STDMETHODIMP_ (sint32) GetCataclysmPercent(sint32 object_id);
	STDMETHODIMP_ (sint32) NotifyLabBuilt(sint32 object_id);
	STDMETHODIMP_ (sint32) GetSoundID(sint32 object_id);
	STDMETHODIMP_ (sint32) StartsSequence(sint32 object_id);
	STDMETHODIMP_ (sint32) RequiresLab(sint32 object_id);
	STDMETHODIMP_ (sint32) GetCost(sint32 object_id);

	STDMETHODIMP_ (sint32) GetTurnsPerStage(sint32 object_id, sint32 numBuilt);
	STDMETHODIMP_ (sint32) RequiredForStage(sint32 object_id, sint32 stage);
	STDMETHODIMP_ (sint32) RequiredToAdvanceFromStage(sint32 object_id, sint32 stage);
};

#endif
