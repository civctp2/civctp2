//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Slic stack handling
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

SlicStack::SlicStack()
{
	m_sptr = 0;
	m_stack = new uint8[k_INITIAL_STACK_SIZE];
	m_stackSize = k_INITIAL_STACK_SIZE;
}

SlicStack::~SlicStack()
{
	if(m_stack) {
		delete [] m_stack;
	}
}

void SlicStack::Push(SS_TYPE type, SlicStackValue value)
{
	if(m_sptr + sint32(sizeof(SlicStackValue)) + 1 > m_stackSize) {
		uint8* newStack = new uint8[m_stackSize * 2];
		memcpy(newStack, m_stack, m_stackSize);
		delete [] m_stack;
		m_stack = newStack;
		m_stackSize *= 2;
	}

	m_stack[m_sptr++] = uint8(type);
	*((SlicStackValue *)&m_stack[m_sptr]) = value;
	m_sptr += sizeof(SlicStackValue);
}

sint32 SlicStack::Pop(SS_TYPE &type, SlicStackValue &value)
{
	Assert(m_sptr >= (1 + sizeof(SlicStackValue)));
	if(m_sptr < 1 + sizeof(SlicStackValue)) {
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
#if defined(ACTIVISION_ORIGINAL)	// Unitialised & untested GetSymbol return.
	SlicSymbolData *sym;
	sint32 retval;

	switch(type) {
		case SS_TYPE_INT:
			return value.m_int;
		case SS_TYPE_VAR:
			sym = g_slicEngine->GetSymbol(value.m_int);
			sym->GetIntValue(retval);
			return retval;
		case SS_TYPE_SYM:
			sym = value.m_sym;
			sym->GetIntValue(retval);
			return retval;
		default:
			Assert(FALSE);
			return 0;
	}
#else
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
	}

	Assert(FALSE);	// Or issue a SLIC error?
	return 0;		// Questionable, but what else?
#endif
}

BOOL SlicStack::GetUnit(SS_TYPE type, SlicStackValue symVal, Unit &u)
{
#if defined(ACTIVISION_ORIGINAL)	// Uninitialised: may CtD in default case.
	SlicSymbolData *sym;

	switch(type) {
		case SS_TYPE_INT:
			return FALSE;
		case SS_TYPE_VAR:
			sym = g_slicEngine->GetSymbol(symVal.m_int);
			break;
		case SS_TYPE_SYM:
			sym = symVal.m_sym;
			break;
		default:
			Assert(FALSE);
			break;
	}
	if(sym) {
		return sym->GetUnit(u);
	}
	return FALSE;
#else
	SlicSymbolData const *	sym = GetSymbol(type, symVal);
	return (sym) ? sym->GetUnit(u) : FALSE;
#endif
}

BOOL SlicStack::GetCity(SS_TYPE type, SlicStackValue symVal, Unit &city)
{
#if defined(ACTIVISION_ORIGINAL)	// Uninitialised: may CtD in default case.
	SlicSymbolData *sym;

	switch(type) {
		case SS_TYPE_INT:
			return FALSE;
		case SS_TYPE_VAR:
			sym = g_slicEngine->GetSymbol(symVal.m_int);
			break;
		case SS_TYPE_SYM:
			sym = symVal.m_sym;
			break;
		default:
			Assert(FALSE);
			break;
	}
	if(sym) {
		return sym->GetCity(city);
	}
	return FALSE;
#else
	SlicSymbolData const *	sym = GetSymbol(type, symVal);
	return (sym) ? sym->GetCity(city) : FALSE;
#endif
}


BOOL SlicStack::GetPos(SS_TYPE type, SlicStackValue symVal, MapPoint &pos)
{
#if defined(ACTIVISION_ORIGINAL)	// Uninitialised: may CtD in default case.
	SlicSymbolData *sym;

	switch(type) {
		case SS_TYPE_INT:
			return FALSE;
		case SS_TYPE_VAR:
			sym = g_slicEngine->GetSymbol(symVal.m_int);
			break;
		case SS_TYPE_SYM:
			sym = symVal.m_sym;
			break;
		default:
			Assert(FALSE);
			break;
	}
	if(sym) {
		return sym->GetPos(pos);
	}
	return FALSE;
#else
	SlicSymbolData const *	sym = GetSymbol(type, symVal);
	return (sym) ? sym->GetPos(pos) : FALSE;
#endif
}

BOOL SlicStack::GetArmy(SS_TYPE type, SlicStackValue symVal, Army &a)
{
#if defined(ACTIVISION_ORIGINAL)	// Uninitialised: may CtD in default case.
	SlicSymbolData *sym;

	switch(type) {
		case SS_TYPE_INT:
			return FALSE;
		case SS_TYPE_VAR:
			sym = g_slicEngine->GetSymbol(symVal.m_int);
			break;
		case SS_TYPE_SYM:
			sym = symVal.m_sym;
			break;
		default:
			Assert(FALSE);
			break;
	}
	if(sym) {
		return sym->GetArmy(a);
	}
	return FALSE;
#else
	SlicSymbolData const *	sym = GetSymbol(type, symVal);
	return (sym) ? sym->GetArmy(a) : FALSE;
#endif
}
