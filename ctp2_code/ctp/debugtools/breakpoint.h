#ifdef _DEBUG

#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#define Breakpoint() __asm { int 03h }
#else
#define Breakpoint()
#endif

#ifdef __cplusplus
}
#endif

#endif

#endif
