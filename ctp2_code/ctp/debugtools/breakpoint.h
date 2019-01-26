#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include "ctp2_config.h" // Figures out if _DEBUG is defined on Linux

#ifdef _DEBUG

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
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
