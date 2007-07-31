#ifdef _DEBUG





















#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#ifdef __cplusplus
extern "C" {
#endif












#define Breakpoint() __asm { int 03h }



#ifdef __cplusplus
}
#endif

#endif  

#endif 
