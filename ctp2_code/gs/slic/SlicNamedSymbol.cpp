//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Guarded against crash with NULL source argument in strcpy.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "SlicNamedSymbol.h"

#include "SlicSymbol.h"
#include "SlicArray.h"
#include "SlicObject.h"
#include "SlicEngine.h"
#include "SlicFunc.h"
#include "civarchive.h"
#include "profileDB.h"


namespace
{
//  Some value that is different from k_NORMAL_FILE (0) and k_TUTORIAL_FILE (1).
    uint8 const k_GENERATED_BY_EXECUTABLE   = (unsigned) -1;
}

SlicNamedSymbol::SlicNamedSymbol(const char *name, SLIC_SYM type) :
	SlicSymbolData(type)
{
	Init(name);
}

SlicNamedSymbol::SlicNamedSymbol(const char *name) :
	SlicSymbolData(SLIC_SYM_UNDEFINED)
{
	Init(name);
}

SlicNamedSymbol::SlicNamedSymbol(const char *name, SlicArray *array) :
	SlicSymbolData(array)
{
	Init(name);
}

SlicNamedSymbol::SlicNamedSymbol(const char *name, SlicStructDescription *structDesc) :
	SlicSymbolData(structDesc)
{
	Init(name);
}

SlicNamedSymbol::~SlicNamedSymbol()
{
	delete [] m_name;
}


void SlicNamedSymbol::Serialize(CivArchive &archive)
{
	uint16 len;

	if(archive.IsStoring()) {
		archive.StoreChunk((uint8*)&m_index, (uint8*)&m_fromFile + sizeof(m_fromFile));
		len = strlen(m_name);
		archive << len;
		archive.Store((uint8*)m_name, len);
	} else {
		archive.LoadChunk((uint8*)&m_index, (uint8*)&m_fromFile + sizeof(m_fromFile));
		archive >> len;
		m_name = new char[len + 1];
		archive.Load((uint8*)m_name, len);
		m_name[len] = 0;
	}
}

void SlicNamedSymbol::Init(const char *name)
{
    if (name)
    {
	    m_name = new char[strlen(name) + 1];
	    strcpy(m_name, name);
    }
    else
    {
        m_name = NULL;
    }

	m_fromFile = k_GENERATED_BY_EXECUTABLE;
}

const char *SlicNamedSymbol::GetName() const
{
	return m_name;
}

SlicParameterSymbol::SlicParameterSymbol(const char *name, sint32 index) :
	SlicNamedSymbol(name)
{
	m_parameterIndex = index;
}

void SlicParameterSymbol::Serialize(CivArchive &archive)
{
	if(archive.IsStoring()) {
		archive << m_parameterIndex;
	} else {
		archive >> m_parameterIndex;
	}

	SlicNamedSymbol::Serialize(archive);
}
	
BOOL SlicParameterSymbol::GetIntValue(sint32 &value) const
{
	SlicObject *context = g_slicEngine->GetContext();
	SlicArgList *argList = context->GetArgList();
	if(!argList) {
		if(g_theProfileDB && g_theProfileDB->IsDebugSlic()) {
			c3errors_ErrorDialog("Slic", "Parameter %s used outside function call", GetName());
		}
		return FALSE;
	}
	return argList->GetInt(m_parameterIndex, value);
}

BOOL SlicParameterSymbol::GetPlayer(sint32 &value) const
{
	SlicObject *context = g_slicEngine->GetContext();
	SlicArgList *argList = context->GetArgList();
	if(!argList) {
		if(g_theProfileDB && g_theProfileDB->IsDebugSlic()) {
			c3errors_ErrorDialog("Slic", "Parameter %s used outside function call", GetName());
		}
		return FALSE;
	}
	return argList->GetPlayer(m_parameterIndex, value);
}

BOOL SlicParameterSymbol::GetPos(MapPoint &pos) const
{
	SlicObject *context = g_slicEngine->GetContext();
	SlicArgList *argList = context->GetArgList();
	if(!argList) {
		if(g_theProfileDB && g_theProfileDB->IsDebugSlic()) {
			c3errors_ErrorDialog("Slic", "Parameter %s used outside function call", GetName());
		}
		return FALSE;
	}
	return argList->GetPos(m_parameterIndex, pos);
}
	
BOOL SlicParameterSymbol::GetUnit(Unit &u) const
{
	SlicObject *context = g_slicEngine->GetContext();
	SlicArgList *argList = context->GetArgList();
	if(!argList) {
		if(g_theProfileDB && g_theProfileDB->IsDebugSlic()) {
			c3errors_ErrorDialog("Slic", "Parameter %s used outside function call", GetName());
		}
		return FALSE;
	}
	return argList->GetUnit(m_parameterIndex, u);
}
	
BOOL SlicParameterSymbol::GetArmy(Army &a) const
{
	SlicObject *context = g_slicEngine->GetContext();
	SlicArgList *argList = context->GetArgList();
	if(!argList) {
		if(g_theProfileDB && g_theProfileDB->IsDebugSlic()) {
			c3errors_ErrorDialog("Slic", "Parameter %s used outside function call", GetName());
		}
		return FALSE;
	}
	return argList->GetArmy(m_parameterIndex, a);
}

BOOL SlicParameterSymbol::GetCity(Unit &c) const
{
	SlicObject *context = g_slicEngine->GetContext();
	SlicArgList *argList = context->GetArgList();
	if(!argList) {
		if(g_theProfileDB && g_theProfileDB->IsDebugSlic()) {
			c3errors_ErrorDialog("SLIC Parameter %s used outside function call", GetName());
		}
		return FALSE;
	}
	return argList->GetCity(m_parameterIndex, c);
}

void SlicBuiltinNamedSymbol::Serialize(CivArchive &archive)
{
	if(archive.IsStoring()) {
		archive.PutUINT8(m_builtin);
	} else {
		m_builtin = (SLIC_BUILTIN)archive.GetUINT8();
	}

	SlicNamedSymbol::Serialize(archive);

	if(!archive.IsStoring()) {
		g_slicEngine->AddBuiltinSymbol(this);
	}
}
