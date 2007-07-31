














#if defined(__WATCOMC__)    

#pragma pack()

#elif defined(__MWERKS__)   

#pragma options align=reset

#elif defined(_M_IX86)      

#pragma pack(pop)

#endif
