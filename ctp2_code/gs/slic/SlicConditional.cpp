
#include "c3.h"
#include "SlicConditional.h"
#include "sliccmd.h"
#include "ysc.tab.h"

SlicConditional::SlicConditional(const char *expression)
{
	strncpy(m_expression, expression, k_MAX_SLIC_CONDITIONAL);
}

SlicConditional::~SlicConditional()
{
}

sint32 SlicConditional::Eval()
{
	char output[1024];
	char catString = 0;
	
	sliccmd_int_result = 0;
	sliccmd_parse(SLICCMD_EVAL, m_expression, output, 1024, 0, &catString);
	return sliccmd_int_result;
}


void SlicConditional::SetExpression(const char *expression)
{
	strncpy(m_expression, expression, k_MAX_SLIC_CONDITIONAL);
}
