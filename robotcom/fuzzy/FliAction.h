

#pragma once
#ifndef __FLI_ACTION_H__
#define __FLI_ACTION_H__

class FliSymbol;

enum FLI_ACTION_TYPE {
	FLI_ACTION_LOAD_AIP,
};

template <class T> class PointerList;
class AiMain;
class FliEngine;

#include "fliif.h"

class FliAction {
private:
	FliEngine *m_engine;
	FLI_ACTION_TYPE m_action;
	PointerList<char> *m_arguments;
	FliSymbol *m_symbol;
	BFLOP_TYPE m_op;
	double m_value;

	void Execute(AiMain *ai);

public:
	FliAction(const char *name, FliEngine *engine);
	~FliAction();

	void AddArgument(const char *str);
	void SetBoolExp(FliSymbol *sym, BFLOP_TYPE op, double value);

	void Evaluate(AiMain *ai);
};

#endif
