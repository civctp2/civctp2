













#ifndef eclock_h  
#define eclock_h

#ifdef UNIX

#ifdef SPARC


#include <sys/times.h>
#include <limits.h>

#define ECLOCKS_PER_SEC CLK_TCK

clock_t eclock_sparc(void);
#define eclock() eclock_sparc()

#else 


#include <time.h>
#include <sys/times.h>
#define ECLOCKS_PER_SEC CLK_TCK

#define eclock() times(NULL)

#endif 


#elif defined(__MWERKS__) 

#include <OSUtils.h>
#include <time.h>
#define ECLOCKS_PER_SEC 1000 

#else 


#if defined(_WIN32)
#include <time.h>
#define ECLOCKS_PER_SEC 1000 
clock_t eclock_win32(void);
#define eclock() eclock_win32()
#else
#include <time.h>
#define ECLOCKS_PER_SEC CLOCKS_PER_SEC
#define eclock() clock()
#endif  

#endif

#endif  

