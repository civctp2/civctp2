
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
}

BOOL SlicStack::GetUnit(SS_TYPE type, SlicStackValue symVal, Unit &u)
{
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
}

BOOL SlicStack::GetCity(SS_TYPE type, SlicStackValue symVal, Unit &city)
{
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
}


BOOL SlicStack::GetPos(SS_TYPE type, SlicStackValue symVal, MapPoint &pos)
{
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
}

BOOL SlicStack::GetArmy(SS_TYPE type, SlicStackValue symVal, Army &a)
{
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
}
