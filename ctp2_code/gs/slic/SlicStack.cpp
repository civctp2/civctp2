//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Slic stack handling
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
// - Prevented random memory access and NULL-dereferencing crashes.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "SlicStack.h"
#include "Unit.h"
#include "SlicEngine.h"
#include "SlicSymbol.h"
#include "SlicNamedSymbol.h"
#include "Army.h"
#include "profileDB.h"

SlicStack::SlicStack()
{
	m_sptr = 0;
	m_stack = new uint8[k_INITIAL_STACK_SIZE];
	m_stackSize = k_INITIAL_STACK_SIZE;
}

SlicStack::~SlicStack()
{
	delete [] m_stack;
}

void SlicStack::Push(SS_TYPE type, SlicStackValue value)
{
	if (static_cast<size_t>(m_sptr) + sizeof(SlicStackValue) + 1 > m_stackSize)
    {
		uint8* newStack = new uint8[m_stackSize * 2];
		memcpy(newStack, m_stack, m_stackSize);
		delete [] m_stack;
		m_stack = newStack;
		m_stackSize *= 2;
	}

	m_stack[m_sptr++] = static_cast<uint8>(type);
	*((SlicStackValue *)&m_stack[m_sptr]) = value;
	m_sptr += sizeof(SlicStackValue);
}

sint32 SlicStack::Pop(SS_TYPE &type, SlicStackValue &value)
{
	Assert(m_sptr >= (1 + sizeof(SlicStackValue)));
	if (static_cast<size_t>(m_sptr) < 1 + sizeof(SlicStackValue))
    {
		return -1;
	}

	m_sptr -= sizeof(SlicStackValue);
	value = *((SlicStackValue *)&m_stack[m_sptr]);
	m_sptr--;
	type = SS_TYPE(m_stack[m_sptr]);
	return m_sptr;
}

SlicSymbolData *SlicStack::GetSymbol(SS_TYPE symType, SlicStackValue symVal)
{
	if(symType == SS_TYPE_VAR) {
		return g_slicEngine->GetSymbol(symVal.m_int);
	} else if(symType == SS_TYPE_SYM) {
		return symVal.m_sym;
	} else {
		return NULL;
	}
}

sint32 SlicStack::Eval(SS_TYPE type, SlicStackValue value)
{
	if (SS_TYPE_INT == type)
	{
		return value.m_int;
	}

	SlicSymbolData const *	sym = GetSymbol(type, value);

	if (sym)
	{
		sint32	retValue;

		if (sym->GetIntValue(retValue))
		{
			return retValue;
		}

		if(g_theProfileDB
		&& g_theProfileDB->IsDebugSlic()
		){
			c3errors_ErrorDialog("Slic", "%s: is not an integer.\nLine: %i\nFile: %s",
				                 sym->GetName(),
								 g_slicEngine->GetCurrentLine(),
								 g_slicEngine->GetFileName());
		}
	}

//	Assert(false);  // To heavy for debugung Cradle Frenzy
	return 0;		// Questionable, but what else?
}

BOOL SlicStack::GetUnit(SS_TYPE type, SlicStackValue symVal, Unit &u)
{
	SlicSymbolData const *	sym = GetSymbol(type, symVal);
	return (sym) ? sym->GetUnit(u) : FALSE;
}

BOOL SlicStack::GetCity(SS_TYPE type, SlicStackValue symVal, Unit &city)
{
	SlicSymbolData const *	sym = GetSymbol(type, symVal);
	return (sym) ? sym->GetCity(city) : FALSE;
}

BOOL SlicStack::GetPos(SS_TYPE type, SlicStackValue symVal, MapPoint &pos)
{
	SlicSymbolData const *	sym = GetSymbol(type, symVal);
	return (sym) ? sym->GetPos(pos) : FALSE;
}

BOOL SlicStack::GetArmy(SS_TYPE type, SlicStackValue symVal, Army &a)
{
	SlicSymbolData const *	sym = GetSymbol(type, symVal);
	return (sym) ? sym->GetArmy(a) : FALSE;
}
