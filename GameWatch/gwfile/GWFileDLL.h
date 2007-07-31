











#ifndef _BMH_GWFILE_DLL_TYPES_H_
#define _BMH_GWFILE_DLL_TYPES_H_



#pragma warning(disable:4251)





#ifndef DllImport
#define DllImport	__declspec( dllimport )
#endif
#ifndef DllExport
#define DllExport	__declspec( dllexport )
#endif









#ifdef __GWFILE_USE_IMPORT
#define GWFileDllHeader DllImport
#else 
#ifdef __GWFILE_USE_EXPORT
#define GWFileDllHeader DllExport
#else 
#define GWFileDllHeader
#endif 
#endif 

#endif 
