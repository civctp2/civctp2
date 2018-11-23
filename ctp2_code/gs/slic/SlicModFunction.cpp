#include "c3.h"

#include "SlicEngine.h"
#include "SlicModFunction.h"

SlicModFunc::SlicModFunc(const MBCHAR *name, ...)
{
	m_numArgs = 0;

	m_segment = g_slicEngine->GetSegment(name);
	if(!m_segment) return;

	va_list vl;
	va_start(vl, name);

	bool done = false, abort = false;
	SLIC_TAG tag;

	m_numArgs = 0;
	do {
		tag = (SLIC_TAG) va_arg(vl, VA_ARG_SLIC_TAG);
		switch(tag) {
			case ST_END:
				done = true;
				break;
			case ST_UNIT:
			case ST_CITY:
			case ST_PLAYER:
			case ST_GOOD:
			case ST_GOLD:
			case ST_LOCATION:
			case ST_ACTION:
			case ST_ADVANCE:
			case ST_INT:
				m_argTypes[m_numArgs++] = tag;
				break;
			default:
			{
				BOOL Unknown_Tag = false;
				Assert(Unknown_Tag);
				abort = true;
				break;
			}
		}
	} while(!done && !abort && m_numArgs < k_MAX_MOD_FUNC_ARGS);

	va_end(vl);
}

SlicModFunc::~SlicModFunc()
{
}

SlicSegment *SlicModFunc::GetSegment()
{
	return m_segment;
}

SLIC_TAG SlicModFunc::GetArg(sint32 arg)
{
	Assert(arg >= 0);
	Assert(arg < m_numArgs);
	if(arg >= 0 && arg < m_numArgs)
		return m_argTypes[arg];
	else
		return ST_END;
}
