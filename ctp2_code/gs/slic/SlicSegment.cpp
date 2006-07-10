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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added extra case statement to facilitate '**' operator
// - Added stuff for '&' operator
// - Prevented memory leaks
// - Prevented crash
// - Reuse SlicSegment pool between SlicEngine sessions.
//
//----------------------------------------------------------------------------

#include "c3.h"                 // Pre-compiled header
#include "SlicSegment.h"        // Own declarations: consistency check

#include "SlicError.h"
#include "slicif.h"
#include "SlicStack.h"
#include "SlicFrame.h"
#include "civarchive.h"
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
#include "profileDB.h"
#include "pool.h"

namespace
{
    sint32 const                NOT_IN_USE    = -1;
    GAME_EVENT_PRIORITY const   PRIORITY_DEFAULT    = GEV_PRI_Pre;
    SLIC_OBJECT const           TYPE_DEFAULT        = SLIC_OBJECT_MESSAGEBOX;

    size_t const                SEGMENT_POOL_SIZE   = 100;
	Pool<SlicSegment> *         s_segmentPond       = NULL;
}

//----------------------------------------------------------------------------
//
// Name       : SlicSegment::SlicSegment
//
// Description: Default constructor
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
SlicSegment::SlicSegment()
:	GameEventHookCallback       (),
    m_type                      (TYPE_DEFAULT),
	m_codeSize                  (0),
	m_num_trigger_symbols       (0),
	m_num_parameters            (0),
	m_enabled                   (FALSE),
	m_specialVariables          (0),
	m_isAlert                   (FALSE),
	m_isHelp                    (FALSE),
	m_event                     (GEV_MAX),
	m_priority                  (PRIORITY_DEFAULT),
	m_fromFile                  (FALSE),
	m_firstLineNumber           (NOT_IN_USE),
	m_id                        (NULL),
	m_code                      (NULL),
	m_uiComponent               (NULL),
	m_filename                  (NULL),
    m_trigger_symbols_indices   (NULL),
	m_trigger_symbols           (NULL),
	m_parameter_indices         (NULL),
	m_parameter_symbols         (NULL),
    m_poolIndex                 (NOT_IN_USE) 
{
    std::fill(m_lastShown, m_lastShown + k_MAX_PLAYERS, 0);
}

//----------------------------------------------------------------------------
//
// Name       : SlicSegment::SlicSegment
//
// Description: Constructor from stored object
//
// Parameters : slicifIndex         : index of stored object
//
// Globals    : g_slicObjectArray   : stored objects
//              g_slicEngine        : game engine
//              g_gevManager        : game event manager
//
// Returns    : -
//
// Remark(s)  : - The stored object will be deleted after copying its data.
//              - Event handlers and Slic user functions will be registered. 
//
//----------------------------------------------------------------------------
SlicSegment::SlicSegment(sint32 slicifIndex)
:	GameEventHookCallback       (),
    m_type                      (TYPE_DEFAULT),
	m_codeSize                  (0),
	m_num_trigger_symbols       (0),
	m_num_parameters            (0),
	m_enabled                   (TRUE),
	m_specialVariables          (0),
	m_isAlert                   (FALSE),
	m_isHelp                    (FALSE),
	m_event                     (GEV_MAX),
	m_priority                  (PRIORITY_DEFAULT),
	m_fromFile                  (FALSE),
	m_firstLineNumber           (NOT_IN_USE),
	m_id                        (NULL),
	m_code                      (NULL),
	m_uiComponent               (NULL),
	m_filename                  (NULL),
    m_trigger_symbols_indices   (NULL),
	m_trigger_symbols           (NULL),
	m_parameter_indices         (NULL),
	m_parameter_symbols         (NULL),
    m_poolIndex                 (slicifIndex) 
{
    std::fill(m_lastShown, m_lastShown + k_MAX_PLAYERS, 0);

	Assert(slicifIndex < g_slicNumEntries);
	struct PSlicObject *pobj = g_slicObjectArray[slicifIndex];

	m_type = pobj->m_type;
	m_id = pobj->m_id;
	m_code = pobj->m_code;
	m_codeSize = pobj->m_codeSize;
	m_fromFile = pobj->m_from_file;
	m_isAlert = pobj->m_is_alert;
	m_isHelp  = pobj->m_is_help;
	m_uiComponent = pobj->m_ui_component;
	m_filename = pobj->m_filename;

	if (m_type == SLIC_OBJECT_TRIGGER) 
    {
		free(pobj->m_trigger_symbols);  // never used
	}

	if (m_type == SLIC_OBJECT_FUNCTION) 
    {
		if (pobj->m_num_parameters > 0) 
        {
			m_parameter_indices = new sint32[pobj->m_num_parameters];
		    memcpy(m_parameter_indices, pobj->m_parameters, pobj->m_num_parameters * sizeof(sint32));
		}
		m_num_parameters = pobj->m_num_parameters;
		free(pobj->m_parameters);
	}

	if (m_type == SLIC_OBJECT_HANDLEEVENT) 
    {
		switch (pobj->m_priority) 
        {
			case SLIC_PRI_PRE:     m_priority = GEV_PRI_Pre;         break;
			case SLIC_PRI_POST:    m_priority = GEV_PRI_Post;        break;
			case SLIC_PRI_PRIMARY: m_priority = GEV_PRI_Primary;     break;
			default:
				m_priority = GEV_PRI_Pre;
				Assert(FALSE);
				break;
		}

        if (g_gevManager)
        {
		    m_event = g_gevManager->GetEventIndex(pobj->m_event_name);
		    g_gevManager->AddCallback(m_event, m_priority, this);
	    }
	}

	g_slicObjectArray[slicifIndex] = NULL;

	m_enabled = TRUE;

    if (g_slicEngine)
    {
	    SlicSymbolData *sym = g_slicEngine->GetSymbol(m_id);
		
	    if (sym) 
        {
		    if((sym->GetType() == SLIC_SYM_ID) ||
	    	   (sym->GetType() == SLIC_SYM_UFUNC)
              ) 
            {
			    sym->SetSegment(this);
		    }
	    }
	}

	free(pobj);
}

//----------------------------------------------------------------------------
//
// Name       : SlicSegment::SlicSegment
//
// Description: Constructor from serialized archive
//
// Parameters : archive : archive to read from
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
SlicSegment::SlicSegment(CivArchive &archive)
:	GameEventHookCallback       (),
	m_type                      (TYPE_DEFAULT),
	m_codeSize                  (0),
	m_num_trigger_symbols       (0),
	m_num_parameters            (0),
	m_enabled                   (TRUE),
	m_specialVariables          (0),
	m_isAlert                   (FALSE),
	m_isHelp                    (FALSE),
	m_event                     (GEV_MAX),
	m_priority                  (PRIORITY_DEFAULT),
	m_fromFile                  (FALSE),
	m_firstLineNumber           (NOT_IN_USE),
	m_id                        (NULL),
	m_code                      (NULL),
	m_uiComponent               (NULL),
	m_filename                  (NULL),
	m_trigger_symbols_indices   (NULL),
	m_trigger_symbols           (NULL),
	m_parameter_indices         (NULL),
	m_parameter_symbols         (NULL)
	// m_poolIndex              (filled when retrieving from the pool) 
{
	std::fill(m_lastShown, m_lastShown + k_MAX_PLAYERS, 0);
	Serialize(archive);
}

//----------------------------------------------------------------------------
//
// Name       : SlicSegment::~SlicSegment
//
// Description: Destructor
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
SlicSegment::~SlicSegment()
{
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

	delete [] m_trigger_symbols_indices;
	delete [] m_trigger_symbols;
	delete [] m_parameter_indices;
	delete [] m_parameter_symbols;
}

//----------------------------------------------------------------------------
//
// Name       : SlicSegment::operator new
//
// Description: Memory allocator
//
// Parameters : (size_t: unused)
//
// Globals    : s_segmentPond   : pool to retrieve a segment from
//
// Returns    : void *          : pointer to pool segment
//
// Remark(s)  : Slic segments are from a specialised pool, instead of from the
//              default heap.
//
//----------------------------------------------------------------------------
void * SlicSegment::operator new(size_t)
{
	if (!s_segmentPond)
	{
		s_segmentPond = new Pool<SlicSegment>(SEGMENT_POOL_SIZE);
	}

	int index;
	SlicSegment *   seg = static_cast<SlicSegment *>
		(s_segmentPond->Get_Next_Pointer(index));

	if (seg)
	{
		seg->SetPoolIndex(index);
	}

	return seg;
}

//----------------------------------------------------------------------------
//
// Name       : SlicSegment::operator delete
//
// Description: Memory deallocator
//
// Parameters : ptr             : pointer to delete
//
// Globals    : s_segmentPond   : pool to restore the pointer to
//
// Returns    : -
//
// Remark(s)  : Slic segments are from a specialised pool, instead of from the
//              default heap.
//
//----------------------------------------------------------------------------
void SlicSegment::operator delete(void *ptr)
{
	SlicSegment * seg = static_cast<SlicSegment *>(ptr);
	if (seg)
	{
        if ((SLIC_OBJECT_HANDLEEVENT == seg->m_type) && 
            (GEV_MAX != seg->m_event) &&
            g_gevManager
           )
        {
            g_gevManager->RemoveCallback(seg->m_event, seg);
        }

		int const poolIndex = seg->GetPoolIndex();
		if ((poolIndex >= 0) && s_segmentPond)
		{
			s_segmentPond->Release_Pointer(poolIndex);
		}
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
		m_parameter_indices = (m_num_parameters > 0) ? new sint32[m_num_parameters] : NULL;
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

	delete [] m_parameter_symbols;	
    m_parameter_symbols = (m_num_parameters > 0) ? new SlicSymbolData *[m_num_parameters] : NULL;
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
	delete [] m_segments;
	m_numSegments = size;
	m_nextSegment = 0;
	m_segments = new SlicSegment *[m_numSegments ? m_numSegments : 1];
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
	if (IsEnabled()) 
    {
	    SlicObject *so = new SlicObject(this);
	    so->AddRef();
        so->Snarf(args);
        so->SetResult((sint32)GEV_HD_Continue); 
        g_slicEngine->Execute(so);

	    GAME_EVENT_HOOK_DISPOSITION disp = (GAME_EVENT_HOOK_DISPOSITION) so->GetResult();
	    so->Release();

	    Assert(disp >= GEV_HD_Continue && disp < GEV_HD_MAX);
	    if (g_slicEngine->AtBreak())
		    return GEV_HD_NeedUserInput;

        switch(disp) 
        {
		default:
			if (g_theProfileDB && g_theProfileDB->IsDebugSlic()) 
            {
				c3errors_ErrorDialog("Slic", "Bad return from event handler");
            }
            break;
		case SLIC_CONST_CONTINUE:
			break;
		case SLIC_CONST_GETINPUT:
			return GEV_HD_NeedUserInput;
		case SLIC_CONST_STOP:
			return GEV_HD_Stop;
		}
	}

	return GEV_HD_Continue;
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

void SlicSegment::Cleanup(void)
{
	delete s_segmentPond;
	s_segmentPond = NULL;
}

