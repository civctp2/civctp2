











#ifndef _BMH_GWARCHIVE_DLL_TYPES_H_
#define _BMH_GWARCHIVE_DLL_TYPES_H_



#pragma warning(disable:4251)





#ifndef DllImport
#define DllImport	__declspec( dllimport )
#endif
#ifndef DllExport
#define DllExport	__declspec( dllexport )
#endif









#ifdef __GWARCHIVE_USE_IMPORT
#define GWArchiveDllHeader DllImport
#else 
#ifdef __GWARCHIVE_USE_EXPORT
#define GWArchiveDllHeader DllExport
#else 
#define GWArchiveDllHeader
#endif 
#endif 

#endif 
