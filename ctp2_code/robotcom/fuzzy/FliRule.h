

#pragma once
#ifndef __FLI_RULE_H__
#define __FLI_RULE_H__

template <class T> class PointerList;
class FliSetFunc;
class FliSymbol;
class FliEngine; 
class FliWhen;

class InputString
{
public:
	char *m_string;
	InputString(const char *str) {
		m_string = new char[strlen(str) + 1];
		strcpy(m_string, str);
	}
	~InputString() {
		delete [] m_string;
        m_string = NULL; 
	}
};

class FliRule {
private:
	uint8 *m_code;
	sint32 m_codelen;
	sint32 m_type;
	PointerList<FliSymbol> *m_inputSymbols;
	PointerList<FliSymbol> *m_outputSymbols;
	FliWhen *m_when;
#ifdef _DEBUG
	PointerList<InputString> *m_inputStrings;
#endif

public:
	FliRule(uint8 *code, sint32 codelen,
			PointerList<FliSymbol> *inputsyms,
			PointerList<FliSymbol> *outputsyms,
			sint32 ruletype,
			FliWhen *when);
	~FliRule();

	sint32 GetType() const { return m_type; }

	BOOL DependanciesResolved();
	void ResolveDependancies();
	void Evaluate(FliEngine *fli_engine, const sint32 module);
	void Interpret(FliEngine *fli_engine, const sint32 module);
};

#endif
