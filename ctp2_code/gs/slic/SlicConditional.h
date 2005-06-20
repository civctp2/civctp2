#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __SLIC_CONDITIONAL_H__
#define __SLIC_CONDITIONAL_H__

#define k_MAX_SLIC_CONDITIONAL 1024

class SlicConditional {
public:
	SlicConditional(const char *expression);
	~SlicConditional();

	sint32 Eval();
	void SetExpression(const char *expression);
	const char *GetExpression() { return m_expression; }

private:
	char m_expression[k_MAX_SLIC_CONDITIONAL];
};

#endif
