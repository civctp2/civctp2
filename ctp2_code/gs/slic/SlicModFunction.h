
#ifndef SLIC_MOD_FUNC_H__
#define SLIC_MOD_FUNC_H__

#define k_MAX_MOD_FUNC_ARGS 8

#include "SlicModFuncEnum.h"
#include "SlicEngine.h"

class SlicModFunc {
public:
	SlicModFunc(const MBCHAR *name, ...);
	~SlicModFunc();

	SlicSegment *GetSegment();
	sint32 GetNumArgs() { return m_numArgs; }
	SLIC_TAG GetArg(sint32 arg);

private:
	SLIC_TAG m_argTypes[k_MAX_MOD_FUNC_ARGS];
	SlicSegment *m_segment;
	sint32 m_numArgs;
};

#endif
