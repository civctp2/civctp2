

























#ifndef _SDL_endian_h
#define _SDL_endian_h




#define SDL_LIL_ENDIAN	1234
#define SDL_BIG_ENDIAN	4321






#if defined(i386) || defined(WIN32) || defined(__alpha__)
#define SDL_BYTEORDER	SDL_LIL_ENDIAN
#else
#define SDL_BYTEORDER	SDL_BIG_ENDIAN
#endif


#define SDL_Swap16(X)  ((X<<8)|(X>>8))
#define SDL_Swap32(X)  ((X<<24)|((X<<8)&0x00FF0000)|((X>>8)&0x0000FF00)|(X>>24))
#define SDL_Swap64(X)  "How do we swap a 64-bit value?"


#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define SDL_SwapLE16(X)	(X)
#define SDL_SwapLE32(X)	(X)
#define SDL_SwapLE64(X)	(X)
#define SDL_SwapBE16(X)	SDL_Swap16(X)
#define SDL_SwapBE32(X)	SDL_Swap32(X)
#define SDL_SwapBE64(X)	SDL_Swap64(X)
#else
#define SDL_SwapLE16(X)	SDL_Swap16(X)
#define SDL_SwapLE32(X)	SDL_Swap32(X)
#define SDL_SwapLE64(X)	SDL_Swap64(X)
#define SDL_SwapBE16(X)	(X)
#define SDL_SwapBE32(X)	(X)
#define SDL_SwapBE64(X)	(X)
#endif

#endif 
