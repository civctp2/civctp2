
















#if defined(__WATCOMC__)    

#pragma pack()

#elif defined(__MWERKS__)   

#pragma options align=native

#elif defined(_M_IX86)      

#pragma pack(push)

#endif




#ifndef PACK
#define PACK
#endif
