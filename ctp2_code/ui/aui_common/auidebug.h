










#ifndef __AUIDEBUG_H__
#define __AUIDEBUG_H__


#ifdef _DEBUG

#ifndef Assert
#include <crtdbg.h>
#define Assert(x) _ASSERTE(x)
#endif 

#else

#ifndef Assert
#define Assert(x) (void)0
#endif 

#endif 


#endif 
