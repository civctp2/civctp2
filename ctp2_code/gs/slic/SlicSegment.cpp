//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Slic - reading files (?)
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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added extra case statement to facilitate '**' operator
// - Added stuff for '&' operator
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "SlicSegment.h"
#include "SlicError.h"
#include "slicif.h"
#include "SlicStack.h"
#include "SlicFrame.h"
#include "CivArchive.h"
#include "SlicEngine.h"
#include "SlicSymbol.h"
#include "SlicObject.h"
#include "Unit.h"
#include "SelItem.h"

#include "TurnCnt.h"

#include "GameEventManager.h"
#include "SlicNamedSymbol.h"
#include "SlicConditional.h"
#include "SlicConst.h"
#include "ProfileDB.h"

SlicSegment::SlicSegment()
{
	m_id = NULL;
	m_code = NULL;
	m_uiComponent = NULL;
	m_trigger_symbols = NULL;
	m_trigger_symbols_indices = NULL;
	m_parameter_indices = NULL;
	m_parameter_symbols = NULL;
    m_isAlert = FALSE;
	m_isHelp = FALSE;
	m_filename = NULL;
	m_firstLineNumber = -1;

    int i;
    for(i=0; i<k_MAX_PLAYERS; i++)
        m_lastShown[i] = 0;
}

SlicSegment::SlicSegment(sint32 slicifIndex)
{
	Assert(slicifIndex < g_slicNumEntries);
	struct PSlicObject *pobj = g_slicObjectArray[slicifIndex];

	m_firstLineNumber = -1;

    m_isAlert = FALSE;
	m_isHelp = FALSE;

	m_type = pobj->m_type;
	m_id = pobj->m_id;
	m_code = pobj->m_code;
	m_codeSize = pobj->m_codeSize;
	m_specialVariables = 0;
	m_fromFile = pobj->m_from_file;
	m_isAlert = pobj->m_is_alert;
	m_isHelp  = pobj->m_is_help;
	m_uiComponent = pobj->m_ui_component;
	m_filename = pobj->m_filename;

	




	m_trigger_symbols = NULL;
	m_parameter_symbols = NULL;

	if(m_type == SLIC_OBJECT_TRIGGER) {
		
		
		
		
		free(pobj->m_trigger_symbols);
		m_num_trigger_symbols = 0;
	} else {
		m_trigger_symbols_indices = NULL;
		m_num_trigger_symbols = 0;
	}

	if(m_type == SLIC_OBJECT_FUNCTION) {
		if(pobj->m_num_parameters == 0) {
			m_parameter_indices = new sint32[1];
		} else {
			m_parameter_indices = new sint32[pobj->m_num_parameters];
		}
		memcpy(m_parameter_indices, pobj->m_parameters, pobj->m_num_parameters * sizeof(sint32));
		m_num_parameters = pobj->m_num_parameters;
		free(pobj->m_parameters);
	} else {
		m_parameter_indices = NULL;
		m_num_parameters = 0;
	}

	if(m_type == SLIC_OBJECT_HANDLEEVENT && g_gevManager) {
		m_event = g_gevManager->GetEventIndex(pobj->m_event_name);
		switch(pobj->m_priority) {
			case SLIC_PRI_PRE:     m_priority = GEV_PRI_Pre;         break;
			case SLIC_PRI_POST:    m_priority = GEV_PRI_Post;        break;
			case SLIC_PRI_PRIMARY: m_priority = GEV_PRI_Primary;     break;
			default:
				m_priority = GEV_PRI_Pre;
				Assert(FALSE);
				break;
		}

		g_gevManager->AddCallback(m_event, m_priority, this);
	} else {
		m_event = GEV_MAX;
	}

	g_slicObjectArray[slicifIndex] = NULL;

	m_enabled = TRUE;

    int i;
    for(i=0; i<k_MAX_PLAYERS; i++)
        m_lastShown[i] = 0;

	SlicSymbolData *sym = g_slicEngine->GetSymbol(m_id);
	if(sym) {
		
		
		if((sym->GetType() == SLIC_SYM_ID) ||
		   (sym->GetType() == SLIC_SYM_UFUNC)) {
			sym->SetSegment(this);
		}
	}

	free(pobj);
}

SlicSegment::SlicSegment(CivArchive &archive)
{
	Serialize(archive);
}


SlicSegment::~SlicSegment()
{
	if(m_type == SLIC_OBJECT_HANDLEEVENT && g_gevManager && m_event != GEV_MAX) {
		g_gevManager->RemoveCallback(m_event, this);
	}

	
	
	if(m_id) {
		free(m_id);
		m_id = NULL;
	}

	if(m_code) {
		free(m_code);
		m_code = NULL;
	}

	if(m_uiComponent) {
		free(m_uiComponent);
		m_uiComponent = NULL;
	}

	if(m_filename) {
		free(m_filename);
		m_filename = NULL;
	}

	if(m_trigger_symbols_indices) {
		delete [] m_trigger_symbols_indices;
		m_trigger_symbols_indices = NULL;
	}

	if(m_trigger_symbols) {
		delete [] m_trigger_symbols;
		m_trigger_symbols = NULL;
	}

	if(m_parameter_indices) {
		delete [] m_parameter_indices;
		m_parameter_indices = NULL;
	}

	if(m_parameter_symbols) {
		delete [] m_parameter_symbols;
		m_parameter_symbols = NULL;
	}
}

void *SlicSegment::operator new(size_t size)
{
	Assert(g_slicEngine);
	if(g_slicEngine) {
		SlicSegment *seg = g_slicEngine->GetNewSegment();
		return seg;
	} else {
		SlicSegment *seg = ::new SlicSegment;
		seg->m_poolIndex = -1;
		return seg;
	}
}

void SlicSegment::operator delete(void *ptr)
{
	SlicSegment *seg = (SlicSegment *)ptr;
	if(seg->m_poolIndex >= 0) {
		g_slicEngine->ReleaseSegment(seg);
	} else {
		::delete(ptr);
	}
}


BOOL SlicSegment::TestLastShown(sint32 player, sint32 turn)
{
    int ls = GetLastShown(player);

    return(!HasBeenShown(player) || ((g_turn->GetRound() - ls) >= turn));
}

void SlicSegment::Serialize(CivArchive &archive)
{
	sint32 l;
	if(archive.IsStoring()) {
		archive.StoreChunk((uint8 *)&m_type, ((uint8 *)&m_fromFile)+sizeof(m_fromFile));

		if(m_id) {
			l = strlen(m_id) + 1;
			archive << l;
			archive.Store((uint8*)m_id, l);
		} else {
			l = 1;
			archive << l;
			archive.Store((uint8*)"", l);
		}
		archive.Store((uint8*)m_code, m_codeSize);
		archive.Store((uint8*)m_trigger_symbols_indices, m_num_trigger_symbols * sizeof(sint32));
		archive.Store((uint8*)m_lastShown, k_MAX_PLAYERS * sizeof(sint32));
		if(m_uiComponent) {
			l = strlen(m_uiComponent) + 1;
			archive << l;
			archive.Store((uint8*)m_uiComponent, l);
		} else {
			l = 0;
			archive << l;
		}
		archive << m_num_parameters;
		sint32 i;
		for(i = 0; i < m_num_parameters; i++) {
			archive.PutSINT32(((SlicParameterSymbol *)m_parameter_symbols[i])->GetIndex());
		}
		if(!m_filename) {
			archive.PutSINT32(0);
		} else {
			archive.PutSINT32(strlen(m_filename));
			archive.Store((uint8*)m_filename, strlen(m_filename));
		}
	} else {
		archive.LoadChunk((uint8 *)&m_type, ((uint8 *)&m_fromFile)+sizeof(m_fromFile));

		archive >> l;
		m_id = (char *)malloc(l);
		archive.Load((uint8*)m_id, l);
		m_code = (uint8*)malloc(m_codeSize);
		archive.Load((uint8*)m_code, m_codeSize);

        if (m_num_trigger_symbols < 1) {  
            m_trigger_symbols_indices = NULL; 
            m_trigger_symbols = NULL; 
        } else { 
    		m_trigger_symbols_indices = new sint32[m_num_trigger_symbols];
	    	archive.Load((uint8*)m_trigger_symbols_indices, m_num_trigger_symbols * sizeof(sint32));
        }

		archive.Load((uint8*)m_lastShown, k_MAX_PLAYERS * sizeof(sint32));

		archive >> l;
		if(l > 0) {
			m_uiComponent = (char *)malloc(l);
			archive.Load((uint8*)m_uiComponent, l);
		} else {
			m_uiComponent = NULL;
		}

		archive >> m_num_parameters;
		m_parameter_indices = new sint32[m_num_parameters > 0 ? m_num_parameters : 1];
		sint32 i;
		for(i = 0; i < m_num_parameters; i++) {
			m_parameter_indices[i] = archive.GetSINT32();
		}
		l = archive.GetSINT32();
		if(l > 0) {
			m_filename = (char *)malloc(l + 1);
			archive.Load((uint8*)m_filename, l);
			m_filename[l] = 0;
		} else {
			m_filename = NULL;
		}

		if(m_type == SLIC_OBJECT_HANDLEEVENT) {
			g_gevManager->AddCallback(m_event, m_priority, this);
		}
		m_parameter_symbols = NULL;
	}
}

void SlicSegment::AddSpecialVariable(SPECIAL_VAR which)
{
	m_specialVariables |= (1 << which);
}

void SlicSegment::AddSpecials(SlicObject *obj)
{
	if(m_specialVariables & (1 << SPECIAL_VAR_SELECTED_UNIT)) {
		PLAYER_INDEX	player ;
		ID	item ;
		SELECT_TYPE	state ;
		
		g_selected_item->GetTopCurItem(player, item, state);
		if(state == SELECT_TYPE_LOCAL_ARMY) {
			Army army = Army(item);
			obj->AddUnit(army.GetTopVisibleUnit(g_selected_item->GetVisiblePlayer()));
		}
	}
}


void SlicSegment::LinkTriggerSymbols()
{
#if 0
	if(m_type != SLIC_OBJECT_TRIGGER)
		return;

	if(m_num_trigger_symbols <= 0) {
		
		
		
		Assert(FALSE);
		return;
	}

	if(m_uiComponent != NULL) {
		
		
		
		return;
	}

	m_trigger_symbols = new SlicSymbol *[m_num_trigger_symbols];
	for(sint32 i = 0; i < m_num_trigger_symbols; i++) {
		m_trigger_symbols[i] = g_slicEngine->GetSymbol(m_trigger_symbols_indices[i]);
		Assert(m_trigger_symbols[i]);
		if(m_trigger_symbols[i]) {
			m_trigger_symbols[i]->AddTrigger(this);
		}
	}
#endif
}

void SlicSegment::LinkParameterSymbols()
{
	if(m_type != SLIC_OBJECT_FUNCTION)
		return;

	
	m_parameter_symbols = new SlicSymbolData *[m_num_parameters ? m_num_parameters : 1];
	for(sint32 i = 0; i < m_num_parameters; i++) {
		m_parameter_symbols[i] = g_slicEngine->GetSymbol(m_parameter_indices[i]);
		Assert(m_parameter_symbols[i]);
	}
}

void SlicSegmentHash::LinkTriggerSymbols(StringHash<SlicUITrigger> *uiHash)
{
	for(sint32 i = 0; i < m_table_size; i++) {
		StringHashNode<SlicSegment> *node = m_table[i];
		while(node) {
			node->m_obj->LinkTriggerSymbols();
			node->m_obj->LinkParameterSymbols();
			if(node->m_obj->GetUIComponent()) {
				SlicUITrigger *trig = new SlicUITrigger((char *)node->m_obj->GetUIComponent(), node->m_obj);
				uiHash->Add(trig->GetName(), trig);
			}
			node = node->m_next;
		}
	}
}

void SlicSegmentHash::SetSize(sint32 size)
{
	if(m_segments) {
		delete [] m_segments;
	}
	m_numSegments = size;
	m_nextSegment = 0;
	m_segments = new SlicSegment *[m_numSegments];
	memset(m_segments, 0, sizeof(SlicSegment *) * m_numSegments);
}
	
void SlicSegmentHash::Add(const char *name, SlicSegment *seg)
{
	if(m_segments) {
		m_segments[m_nextSegment++] = seg;
	}
	StringHash<SlicSegment>::Add(name, seg);
}

SFN_ERROR SlicSegment::Call(SlicArgList *args, SlicObject *&obj)
{
	Assert(m_type == SLIC_OBJECT_FUNCTION);
	if(m_type != SLIC_OBJECT_FUNCTION)
		return SFN_ERROR_NOT_A_FUNCTION;

	obj = new SlicObject(this);	
	obj->SetArgList(args);
	obj->AddRef(); 
	obj->CopyFromBuiltins();

	g_slicEngine->Execute(obj);

	
	
	

	return SFN_ERROR_OK;
}

void SlicSegment::GetDescription(char *str, sint32 maxsize)
{
	if(m_firstLineNumber < 0) {
		sint32 offset, lastline;
		GetSourceLines(m_firstLineNumber, offset, lastline);
	}

	sprintf(str, "'%s': %s@%d", m_id, m_filename, m_firstLineNumber);
}

GAME_EVENT_HOOK_DISPOSITION SlicSegment::GEVHookCallback(GAME_EVENT type, GameEventArgList *args)
{
	if(!IsEnabled()) return GEV_HD_Continue;

	SlicObject *so = new SlicObject(this);
	so->AddRef();
	so->Snarf(args);
	so->SetResult((sint32)GEV_HD_Continue); 
	g_slicEngine->Execute(so);
	GAME_EVENT_HOOK_DISPOSITION disp = (GAME_EVENT_HOOK_DISPOSITION)so->GetResult();
	so->Release();
	Assert(disp >= GEV_HD_Continue && disp < GEV_HD_MAX);
	if(g_slicEngine->AtBreak())
		return GEV_HD_NeedUserInput;

	switch(disp) {
		case SLIC_CONST_CONTINUE:
			return GEV_HD_Continue;
		case SLIC_CONST_GETINPUT:
			return GEV_HD_NeedUserInput;
		case SLIC_CONST_STOP:
			return GEV_HD_Stop;
		default:
			if(g_theProfileDB && g_theProfileDB->IsDebugSlic()) {

				c3errors_ErrorDialog("Slic", "Bad return from event handler");
			}
			return GEV_HD_Continue;
	}
}


uint8 *SlicSegment::FindNextLine(uint8 *start)
{
	uint8* codePtr = start;
	Assert(start >= m_code);
	Assert(start < m_code + m_codeSize);

	if((start < m_code) || (start >= m_code + m_codeSize)) {
		return NULL;
	}

	bool atEnd = false;

	while(codePtr < m_code + m_codeSize) {
		SOP op = (SOP)*codePtr;
		codePtr++;
		switch(op) {
			case SOP_PUSHI:
			case SOP_PUSHV:
			case SOP_PUSHA:
			case SOP_ARGID:
			case SOP_ARGS: 
			case SOP_ARGST:
			case SOP_CALL:
			case SOP_CALLR:
			case SOP_JMP:
			case SOP_BNT:
			case SOP_BNEV:
			case SOP_OCLS:
			case SOP_ASSN:
			case SOP_ASSNA:
			case SOP_EVENT:
			case SOP_ASIZE:
				
				codePtr += sizeof(int);
				break;
			case SOP_PUSHD:
				codePtr += sizeof(double);
				break;
			case SOP_PUSHM:
			case SOP_PUSHAM:
			case SOP_BUTN:
				
				codePtr += sizeof(int);
				codePtr += sizeof(int);
				break;
			case SOP_STOP:
				
				atEnd = TRUE;
				break;
			case SOP_END:
			case SOP_AINDX:
			case SOP_ADD:
			case SOP_SUB:
			case SOP_MULT:
			case SOP_EXP:
			case SOP_BAND:
			case SOP_DIV: 
			case SOP_EQ:  
			case SOP_GT:  
			case SOP_LT:  
			case SOP_GTE: 
			case SOP_LTE: 
			case SOP_POP: 
			case SOP_TRIG:
			case SOP_ARGE:
			case SOP_NEQ: 
			case SOP_AND:
			case SOP_OR: 
			case SOP_NOT:
			case SOP_SARGS:
			case SOP_RET:
			case SOP_NEG:
			case SOP_SBLK:
				
				break;
			case SOP_LINE: 
			case SOP_LBRK:
				
				codePtr--;
				return codePtr;

			default:
				break;
		}
	}
	return NULL;
}

bool SlicSegment::GetSourceLines(sint32 &firstLineNum, sint32 &firstLineOffset, sint32 &lastLineNum)
{
	bool found = false;
	uint8 *codePtr = m_code;
	sint32 line = 0, offset;

	firstLineNum = -1;
	firstLineOffset = -1;
	lastLineNum = -1;

	while(1) {
		codePtr = FindNextLine(codePtr);
		if(!codePtr) {
			if(firstLineNum >= 0) {
				lastLineNum = line;
				return true;
			} else {
				return false;
			}
		}

		SOP op = (SOP)*codePtr;
		Assert(op == SOP_LINE || op == SOP_LBRK);
		if(op != SOP_LINE && op != SOP_LBRK)
			return false;
		codePtr++;

		line = *((sint32 *)codePtr);
		codePtr += sizeof(sint32);
	
		offset = *((sint32 *)codePtr);
		
		
		
		if(offset < 0) {
			offset = SlicFrame::FindFileOffset(m_filename, line);
			*((sint32 *)codePtr) = offset;
		}
		codePtr += sizeof(int);
		
		codePtr += sizeof(SlicConditional*);

		
		if(firstLineNum < 0) {
			firstLineNum = line;
			firstLineOffset = offset;
		}
	}
}

sint32 SlicSegment::FindLineNumber(sint32 offset)
{
	sint32 curLine = -1;
	bool foundOffset = false;
	uint8 *codePtr = m_code;

	while(!foundOffset) {
		codePtr = FindNextLine(codePtr);

		Assert(codePtr);
		if(!codePtr)
			break;

		SOP op = (SOP)*codePtr;
		Assert(op == SOP_LINE || op == SOP_LBRK);
		if(op != SOP_LINE && op != SOP_LBRK)
			return curLine;
			
		codePtr++;
		curLine = *(sint32 *)codePtr;
		
		
		if((codePtr - m_code) >= (sint32)(offset - ((sizeof(int) * 2) + sizeof(SlicConditional*) + 1))) {
			return curLine;
		}
		codePtr += sizeof(int);
		codePtr += sizeof(int);
		codePtr += sizeof(SlicConditional*);
	}
	return curLine;
}

uint8 *SlicSegment::GetCodePointer(sint32 lineNumber)
{
	bool foundLine = false;
	sint32 curLine = -1;
	uint8 *codePtr = m_code;

	while(!foundLine) {
		codePtr = FindNextLine(codePtr);
		Assert(codePtr);
		if(!codePtr)
			return NULL;
		
		SOP op = (SOP)*codePtr;
		Assert(op == SOP_LINE || op == SOP_LBRK);
		if(op != SOP_LINE && op != SOP_LBRK)
			return NULL;

		codePtr++;
		curLine = *(sint32 *)codePtr;
		if(curLine == lineNumber) {
			codePtr--;
			return codePtr;
		}
		codePtr += sizeof(int);
		codePtr += sizeof(int);
		codePtr += sizeof(SlicConditional *);
	}
	return NULL;
}

bool SlicSegment::LineHasBreak(sint32 lineNumber, bool &conditional)
{
	uint8 *codePtr = GetCodePointer(lineNumber);
	conditional = false;
	if(codePtr) {
		SOP op = (SOP)*codePtr;
		Assert(op == SOP_LINE || op == SOP_LBRK);
		if(op != SOP_LINE && op != SOP_LBRK)
			return false;

		codePtr++;
		codePtr += sizeof(int);
		codePtr += sizeof(int);
		if(*((SlicConditional**)codePtr) != NULL) {
			conditional = true;
		}

		if(op == SOP_LBRK)
			return true;
		else
			return false;
	}
	return false;
}
	
void SlicSegment::SetBreak(sint32 lineNumber, bool setBreak)
{
	uint8 *codePtr = GetCodePointer(lineNumber);
	if(codePtr) {
		if(setBreak)
			*codePtr = SOP_LBRK;
		else
			*codePtr = SOP_LINE;
	}
}

void SlicSegment::RemoveConditional(sint32 line)
{
	uint8 *codePtr = GetCodePointer(line);

	if(codePtr) {
		codePtr++;
		codePtr += sizeof(int);
		codePtr += sizeof(int);
		*((SlicConditional **)codePtr) = NULL;
	}
}

SlicConditional *SlicSegment::GetConditional(sint32 line)
{
	uint8 *codePtr = GetCodePointer(line);

	if(codePtr) {
		codePtr++;
		codePtr += sizeof(int);
		codePtr += sizeof(int);
		return *((SlicConditional **)codePtr);
	}
	return NULL;
}

SlicConditional *SlicSegment::NewConditional(sint32 line, const char *expression)
{
	uint8 *codePtr = GetCodePointer(line);
	if(codePtr) {
		codePtr++;
		codePtr += sizeof(int);
		codePtr += sizeof(int);
		*((SlicConditional **)codePtr) = new SlicConditional(expression);
		return *((SlicConditional **)codePtr);
	}
	return NULL;
}


