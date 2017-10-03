#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef PIXEL_TYPES_H__
#define PIXEL_TYPES_H__

#ifdef __linux__
#include <stdint.h>
typedef uint32_t	Pixel32;
typedef uint16_t	Pixel16;
typedef uint8_t		Pixel8;
#else
typedef unsigned long  int	Pixel32;
typedef unsigned short int	Pixel16;
typedef unsigned char		Pixel8;
#endif

#endif
