
#ifndef __FLI_STACK_H__
#define __FLI_STACK_H__

#define k_FLI_MAX_STACK_SIZE 16384

class FliStack {
private:
	double m_stack[k_FLI_MAX_STACK_SIZE];
	sint32 m_stackPtr;
    
public:
	void Clear();
	void Push(double value);
	double And();
	double Or();
	double Not();
	double Very();
	double Kinda();
	double GetScalar();
};

#endif
