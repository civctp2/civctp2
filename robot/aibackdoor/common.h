













#ifndef __common_h__
#define __common_h__








#define DARK_REIGN_BUILD




#ifdef __cplusplus
extern "C" {
#endif

#ifdef STRATEGY_DEMO_BUILD
#define DllImport __declspec( dllimport )
#define DllExport __declspec( dllexport )

#ifndef DLL_IMPORT
#define DLLHEADER DllExport
#define DLL_EXPORT
#else
#define DLLHEADER DllImport
#endif
#endif

#ifdef DARK_REIGN_BUILD
#define DLLHEADER
#define DllImport
#define DllExport
#endif 





#define BOGUS 0
#define EXCELLENT 1
#define HEINOUS -1
#define ABSURDLY_BIG_NUMBER 32767
#define A_REALLY_BIG_POSITIVE_REAL_NUMBER 1000000000
#define A_ZILLION A_REALLY_BIG_POSITIVE_REAL_NUMBER
#define A_REALLY_BIG_NEGATIVE_REAL_NUMBER -1000000000

#define is ==
#define are ==
#define isnt !=


#ifdef STRATEGY_DEMO_BUILD
#define RED 0
#define GREEN 1
#define BLUE 2
#endif





#define DEBUG 1

#define BEGIN_DEBUG 
#define END_DEBUG 

#define MAX_STRING 100

#define and &&

#define or ||

#define not !

#define TRUE 1
#define FALSE 0

extern DLLHEADER int DEBUG_CONFIG;
extern DLLHEADER FILE *debug_file;

#define DEBUG_MSG if (DEBUG_CONFIG) fprintf
#define DEBUG_MSG1 if (DEBUG_CONFIG>1) fprintf
#define DEBUG_MSG2 if (DEBUG_CONFIG>2) fprintf
#define DEBUG_MSG3 if (DEBUG_CONFIG>3) fprintf


#ifdef __cplusplus
}
#endif

#endif

