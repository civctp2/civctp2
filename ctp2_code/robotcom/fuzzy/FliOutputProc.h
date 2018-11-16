#ifndef __FLI_OUTPUT_PROC_H__
#define __FLI_OUTPUT_PROC_H__

#define k_MAX_MODIFIERS 2

class FliSetFunc;
class CivArchive;

class FliOutputProc {
private:
	double m_scalar;

	FliSetFunc *m_function;
	uint8 m_modifierPtr;
	uint8 m_modifiers[k_MAX_MODIFIERS];
	friend class FliSymbol;

public:
	void Init(double scalar, FliSetFunc *function);
	void AddModifier(uint8 op);
	double Membership(double x, double dx, sint32 &insideLeftSteps, BOOL &insideRight);
    double GetRecommendedDX();
	double GetScalar() { return m_scalar; }
	double GetLeftEdge();
	FliSetFunc *GetFunction() const { return m_function; }

	void Castrate() {}
	void DelPointers() {}
	void Serialize(CivArchive &archive);
};

#endif
