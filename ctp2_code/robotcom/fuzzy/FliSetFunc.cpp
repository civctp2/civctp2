
#include "c3.h"
#include "FliSetFunc.h"
#include "fliif.h"
#include "FliSymbol.h"

FliSetFunc::FliSetFunc(char *name, FliSymbol *variable, SFTYPE type,
					   double center, double width)
{
	m_name = new char[strlen(name) + 1];
	strcpy(m_name, name);
	m_variable = variable;
	m_type = (uint8)type;
	m_center = center;
	m_width = width;

	
    if (SFTYPE_SPIKE == type) { 
        m_width = 0.0; 
    } else { 
	    Assert(m_width != 0);
        if(m_width == 0){
		    m_width = 0.00001;
        }
    }

	m_left_edge = GetLeftEdge();
	m_right_edge = GetRightEdge();

	
	
	
	
}

FliSetFunc::~FliSetFunc()
{
	delete [] m_name;
    m_name = NULL; 
}

BOOL FliSetFunc::IsInputFunction()
{
	return m_variable->IsInputVariable();
}

double FliSetFunc::Evaluate(double dx)
{
	sint32 insideLeft;
	BOOL insideRight;
	return GetMembership(m_variable->GetValue(), dx, insideLeft, insideRight);
}

double FliSetFunc::GetMembership(double x, double dx,
								 sint32 &insideLeftSteps,
								 BOOL &insideRight)
{
	insideLeftSteps = 0;
	insideRight = FALSE;
	switch(m_type) {
        case SFTYPE_SPIKE:
            if (fabs(x - m_center) < (dx * 0.5)) { 
                return 1.0; 
            } else { 
                return 0.0; 
            } 
		case SFTYPE_LEFT:			
			if(x <= m_center) {
				insideLeftSteps = (sint32)((m_center - x) / dx);
				return 1.0;
			} else if(x >= m_center + m_width)
				return 0.0;
			return 1.0 - ((x - m_center) / m_width);

		case SFTYPE_RIGHT:
			if(x >= m_center) {
				insideRight = TRUE;
				return 1.0;
			} else if(x <= m_center - m_width)
				return 0.0;
			return 1.0 - ((m_center - x) / m_width);

		case SFTYPE_TRI:
			if(x >= m_center + (m_width) ||
			   x <= m_center - (m_width))
				return 0;
			else if(x >= m_center) {
				return 1.0 - ((x - m_center) / (m_width));
			} else {
				return 1.0 - ((m_center - x) / (m_width));
			}
		default:
			Assert(FALSE);
			return 0.0;
	}
}

double FliSetFunc::GetLeftEdge()
{
	switch(m_type) {
		case SFTYPE_SPIKE:
			return m_center - GetRecommendedDX();
		case SFTYPE_LEFT:
			return -1;
		case SFTYPE_RIGHT:
		case SFTYPE_TRI:
			return m_center - m_width;
		default:
			Assert(FALSE);
			return -1;
	}
}

double FliSetFunc::GetRightEdge()
{
	switch(m_type) {
		case SFTYPE_SPIKE:
			return m_center + GetRecommendedDX();
		case SFTYPE_LEFT:
		case SFTYPE_TRI:
			return m_center + m_width;
		case SFTYPE_RIGHT:
			return -1;
		default:
			Assert(FALSE);
			return -1;
	}
}

double FliSetFunc::GetRecommendedDX()
{
    return m_width * 0.02; 
} 


