//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Slic segment handling
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
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Cleanup added
//
//----------------------------------------------------------------------------

#ifndef __SLIC_SEGMENT_H__
#define __SLIC_SEGMENT_H__

#include "slicif.h"
#include "StringHash.h"
#include "GameEventHook.h"
#include "GameEventHook.h"
#include "SlicError.h"
#include "SlicFunc.h"
#include "gstypes.h"

class SlicSegment;
class SlicStack;
class CivArchive;
class SlicSymbolData;
class SlicObject;
class SlicArgList;
class SlicConditional;
class SlicUITrigger;

#define SLICLS_FLAG 0x80000000

enum SPECIAL_VAR {
	SPECIAL_VAR_SELECTED_UNIT,
	SPECIAL_VAR_ALL_UNITS_MOVED,
	SPECIAL_VAR_NO_CITY_VIEW,
	SPECIAL_VAR_NO_WORK_VIEW,
	SPECIAL_VAR_NO_PRODUCTION_CONTROLS,
};

class SlicSegment : public GameEventHookCallback {
private:
	
	
	SLIC_OBJECT m_type;
	int m_codeSize;
	sint32 m_num_trigger_symbols;
	sint32 m_num_parameters;
	BOOL m_enabled;
	uint32 m_specialVariables;
	BOOL m_isAlert;
	BOOL m_isHelp;
	GAME_EVENT m_event;
	GAME_EVENT_PRIORITY m_priority;
	sint32 m_fromFile;
    sint32 m_lastShown[k_MAX_PLAYERS];
	sint32 m_firstLineNumber;
	

	
	char *m_id;
	unsigned char *m_code;
	char *m_uiComponent;
	char *m_filename;

	sint32 *m_trigger_symbols_indices;
	SlicSymbolData **m_trigger_symbols;

	sint32 *m_parameter_indices;
	SlicSymbolData **m_parameter_symbols;
	

	int m_poolIndex; 

	friend class SlicFrame;

public:
	SlicSegment();
	SlicSegment(sint32 slicifIndex);
	SlicSegment(CivArchive &archive);
	~SlicSegment();
	void Serialize(CivArchive &archive);

	void *operator new(size_t size);
	void operator delete(void *ptr);

    void SetPoolIndex(int index) { m_poolIndex = index; }
	int GetPoolIndex() const { return m_poolIndex; }

	const char* GetName() const { return m_id; }
	SLIC_OBJECT GetType() const { return m_type; }
	void LinkTriggerSymbols();
	void LinkParameterSymbols();
	BOOL IsEnabled() const { return m_enabled; }
	void Disable() { m_enabled = FALSE; }
	void Enable() { m_enabled = TRUE; }
	void AddSpecialVariable(SPECIAL_VAR which);
	void AddSpecials(SlicObject *obj);
	sint32 GetFilenum() const { return m_fromFile; }
	BOOL IsAlert() const { return m_isAlert; }
	BOOL IsHelp() const { return m_isHelp; }
    void SetLastShown(sint32 player, sint32 turn) { m_lastShown[player] = (turn | SLICLS_FLAG); }
    sint32 GetLastShown(sint32 player) { return (m_lastShown[player] & ~SLICLS_FLAG); }
    BOOL TestLastShown(sint32 player, sint32 turn);
    BOOL HasBeenShown(sint32 player) {
        Assert(m_lastShown); 
        if (!m_lastShown) return FALSE; 
        return(m_lastShown[player] != 0); 
    }

	const char *GetUIComponent() const { return m_uiComponent; }

	SFN_ERROR Call(SlicArgList *args, SlicObject *&obj);
	
	GAME_EVENT_HOOK_DISPOSITION GEVHookCallback(GAME_EVENT type, GameEventArgList *args);
	void GetDescription(char *str, sint32 maxsize);

	const char *GetFilename() { return m_filename; }
	uint8 *FindNextLine(uint8 *start);
	bool GetSourceLines(sint32 &firstLineNum, sint32 &firstLineOffset, sint32 &lastLineNum);
	sint32 FindLineNumber(sint32 offset);
	bool LineHasBreak(sint32 line, bool &conditional);

	void SetBreak(sint32 lineNumber, bool setBreak);

	uint8 *GetCodePointer(sint32 line);

	void RemoveConditional(sint32 line);
	SlicConditional *GetConditional(sint32 line);
	SlicConditional *NewConditional(sint32 line, const char *expression);

    static void Cleanup(void);
};

class SlicSegmentHash : public StringHash<SlicSegment>
{
private:
	sint32 m_numSegments;
	SlicSegment **m_segments;
	sint32 m_nextSegment;
	friend class SegmentList; 

public:
	SlicSegmentHash(sint32 size) : StringHash<SlicSegment>(size) {
		m_numSegments = 0;
		m_segments = NULL;
		m_nextSegment = 0;
	}
	~SlicSegmentHash() {
		delete [] m_segments;
	}

	void SetSize(sint32 size);
	void LinkTriggerSymbols(StringHash<SlicUITrigger> *uiHash);
	void Add(const char *str, SlicSegment *obj);
};

class SlicUITrigger {
  private:
	char *m_name;
	SlicSegment *m_segment;

  public:
	SlicUITrigger(char *name, SlicSegment *seg) {
		m_name = new char[strlen(name) + 1];
		strcpy(m_name, name);
		m_segment = seg;
	}
	~SlicUITrigger() {
		delete [] m_name;
	}

	const MBCHAR *GetName() { return m_name; }
	SlicSegment *GetSegment() { return m_segment; }
};

#endif
