#ifndef __AUIDEBUG_H__
#define __AUIDEBUG_H__

#ifdef _DEBUG

#ifndef Assert
#if defined(WIN32)
#include <crtdbg.h>
#endif
#define Assert(x) _ASSERTE(x)
#endif

#else

#ifndef Assert
#define Assert(x) (void)0
#endif

#endif

#endif
