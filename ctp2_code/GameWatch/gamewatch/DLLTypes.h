#ifndef _BMH_GW_DLL_TYPES_H_
#define _BMH_GW_DLL_TYPES_H_

#ifdef _MSC_VER
#pragma warning(disable:4251)
#endif




#ifndef DllImport
#define DllImport	__declspec( dllimport )
#endif
#ifndef DllExport
#define DllExport	__declspec( dllexport )
#endif









#ifdef __GW_USE_IMPORT
#define GWDllHeader DllImport
#else
#ifdef __GW_USE_EXPORT
#define GWDllHeader DllExport
#else
#define GWDllHeader
#endif
#endif

#endif
