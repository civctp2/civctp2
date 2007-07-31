

#include "c3.h"
#include "FliOutputProc.h"
#include "FliSetFunc.h"

void FliOutputProc::Init(double scalar, FliSetFunc *function)
{
	memset(m_modifiers, 0, sizeof(m_modifiers));
	m_modifierPtr = 0;
	m_scalar = scalar;
	m_function = function;
}

void FliOutputProc::AddModifier(uint8 op)
{
	Assert(m_modifierPtr < k_MAX_MODIFIERS);
	if(m_modifierPtr < k_MAX_MODIFIERS)
		m_modifiers[m_modifierPtr++] = op;
}


double FliOutputProc::Membership(double x, double dx, sint32 &insideLeftSteps, BOOL &insideRight)
{
	double val = m_function->GetMembership(x, dx, insideLeftSteps, insideRight);
	for(sint32 i = 0; i < (sint32)m_modifierPtr; i++) {
		switch(m_modifiers[i]) {
			case FLOP_NOT:
				val = 1.0 - val;
				break;
			case FLOP_VERY:
				val = val * val;
				break;
			case FLOP_KINDA:
				val = 2 * val - (val * val);
				break;
			default:
				
				
				Assert(FALSE);
				break;
		}
	}
	val *= m_scalar;
	return val;
}

double FliOutputProc::GetLeftEdge()
{
	return m_function->GetLeftEdge();
}

double FliOutputProc::GetRecommendedDX()
{
    return m_function->GetRecommendedDX(); 
}

void FliOutputProc::Serialize(CivArchive &archive)
{
}
