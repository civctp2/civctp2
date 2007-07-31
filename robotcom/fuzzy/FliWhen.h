

#pragma once
#ifndef __FLI_WHEN_H__
#define __FLI_WHEN_H__

#include "FliSymbol.h"

class FliWhen
{
	FliSymbol *m_var;
	BFLOP_TYPE m_op;
	double m_value;

	sint32 m_refCount;

public:
	FliWhen() 
	{
		m_var = NULL;
		m_refCount = 0;
	}
	void AddRef() { m_refCount++; }
	void Release() {
		m_refCount--;
		if(m_refCount <= 0)
			delete this;
	}

	BOOL Evaluate() {
		switch(m_op) {
			case BFLOP_EQ:  return m_var->GetValue() == m_value;
			case BFLOP_GT:  return m_var->GetValue() >  m_value;
			case BFLOP_LT:  return m_var->GetValue() <  m_value;
			case BFLOP_NEQ: return m_var->GetValue() != m_value;
			default: Assert(FALSE); return FALSE;
		}
	}

	void Set(FliSymbol *sym, BFLOP_TYPE op, double value) {
		m_var = sym;
		m_op = op;
		m_value = value;
	}
};

#endif
