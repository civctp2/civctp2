












#ifndef __IC3_CIVARCHIVE_H__
#define __IC3_CIVARCHIVE_H__ 1

#undef INTERFACE
#define INTERFACE IC3CivArchive

DEFINE_GUID(CLSID_IC3CivArchive, 
    0x151bb9c0,
    0xd098,
    0x11d1,
    0x85, 0xa7, 0x00, 0x60, 0x08, 0x3a, 0x40, 0x82
);

DECLARE_INTERFACE_(IC3CivArchive, IUnknown) { 

STDMETHOD_ (void, Store) (THIS_ uint8 *pbData, uint32 ulLen) PURE;
STDMETHOD_ (void, Load) (THIS_ uint8 *pbData, uint32 ulLen) PURE ;					
STDMETHOD_ (BOOL, IsStoring)(THIS) PURE ; 
} ;

#endif
