#pragma once

#ifndef __FLI_SET_FUNC_H__
#define __FLI_SET_FUNC_H__

typedef int SFTYPE;
class FliSymbol;
#include "fliif.h"

class FliSetFunc {
private:
	double m_center, m_width;
	sint32 m_index;
	uint8 m_type;

	char *m_name;
	FliSymbol *m_variable;

public:
	double m_left_edge;
	double m_right_edge;

	FliSetFunc(char *name, FliSymbol *variable, SFTYPE type,
			   double center, double width);
	~FliSetFunc();

	const char *GetName() const { return m_name; };
	SFTYPE GetType() const { return (SFTYPE)m_type; }
    double GetCenter() const { return m_center; }
    double GetWidth() const { return m_width; }

	sint32 GetIndex() const { return m_index; }
	void SetIndex(sint32 index) { m_index = index; }
	FliSymbol *GetVariable() { return m_variable; }

	BOOL IsInputFunction();

	double Evaluate(double dx);
	double GetMembership(double x, double dx, sint32 &insideLeftSteps, BOOL &insideRight);
	double GetLeftEdge();
	double GetRightEdge();

    double GetRecommendedDX();

};

#endif
