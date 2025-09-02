#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#ifdef _DEBUG

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) && defined(_X86_)
#define Breakpoint() __asm { int 03h }
#else
#include <csignal>
#define Breakpoint()  std::raise(SIGINT);
#endif

#ifdef __cplusplus
}
#endif

#endif // _DEBUG

#endif // BREAKPOINT_H
