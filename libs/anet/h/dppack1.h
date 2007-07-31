



















#if defined(__WATCOMC__)    

#pragma pack(1)

#elif defined(__MWERKS__)   

#pragma options align=packed

#elif defined(_M_IX86)      

#pragma warning( disable : 4103 )
#pragma pack(push, 1)

#endif




#ifndef PACK
#define PACK
#endif
