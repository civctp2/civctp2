#ifndef _BMH_GW_CIV_DLL_TYPES_H_
#define _BMH_GW_CIV_DLL_TYPES_H_


#pragma warning(disable:4251)





#ifndef DllImport
#define DllImport	__declspec( dllimport )
#endif
#ifndef DllExport
#define DllExport	__declspec( dllexport )
#endif









#ifdef __GWCIV_USE_IMPORT
#define GWCivDllHeader DllImport
#else
#ifdef __GWCIV_USE_EXPORT
#define GWCivDllHeader DllExport
#else
#define GWCivDllHeader
#endif
#endif

#endif
