

#include "c3.h"
#include "FliStack.h"

void FliStack::Clear()
{
	m_stackPtr = 0;
}

void FliStack::Push(double value)
{
	m_stack[m_stackPtr++] = value;
}

double FliStack::And()
{
	Assert(m_stackPtr >= 2);
	if(m_stackPtr < 2)
		return 0.0;
	double v1 = m_stack[--m_stackPtr];
	double v2 = m_stack[--m_stackPtr];
    double result = min(v1, v2);
	Push(result);
    return result; 
}

double FliStack::Or()
{
	Assert(m_stackPtr >= 2);
	if(m_stackPtr < 2)
		return 0.0;
	double v1 = m_stack[--m_stackPtr];
	double v2 = m_stack[--m_stackPtr];
    double result = max(v1, v2);
	Push(result);
    return result;
}

double FliStack::Not()
{
	Assert(m_stackPtr >= 1);
	if(m_stackPtr < 1)
		return 0.0;
	double v = m_stack[--m_stackPtr];
    double result = 1.0 - v;
	Push(result);
    return result;
}

double FliStack::Very()
{
	Assert(m_stackPtr >= 1);
	if(m_stackPtr < 1)
		return 0.0;
	double v = m_stack[--m_stackPtr];
    double result = v * v;
	Push(result);
    return result;
}

double FliStack::Kinda()
{
	Assert(m_stackPtr >= 1);
	if(m_stackPtr < 1)
		return 0.0;
	double v = m_stack[--m_stackPtr];
    double result = sqrt(v);
	Push(result);
    return result;
}

double FliStack::GetScalar()
{
	
	
	

	double ret = 1.0;

	for(sint32 i = 0; i < m_stackPtr - 1; i++){
		ret = min(1.0 - m_stack[i], ret);
	}
	ret = min(ret, m_stack[m_stackPtr - 1]);
	return ret;
}

