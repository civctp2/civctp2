//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : 
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
// 
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
// _MSC_VER		
// - When defined, allows Microsoft C++ extensions.
// - When not defined, generates standard C++.
//
// Note: For the blocks with _MSC_VER preprocessor directives, the following
//       is implied: the (_MSC_VER) preprocessor directive lines and the blocks 
//       between #else and #endif are modified Apolyton code. The blocks 
//       between #if and #else are the original Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #pragma once marked as Microsoft extension.
// - Added utility function for the double to int percentage conversions.
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __CIV3_MATH_H__
#define __CIV3_MATH_H__

#define ABS(x) (((x) > 0) ? (x) : -(x))
#define SGN(x) (((x) >= 0) ? 1 : -1)
#define MAX(x,y) (((x) < (y)) ? (y) : (x))
#define MIN(x,y) (((x) < (y)) ? (x) : (y))

#if !defined(ACTIVISION_ORIGINAL)

//----------------------------------------------------------------------------
//
// Name       : AsPercentage
//
// Description: Convert a floating point factor to an integer percentage.
//
// Parameters : a_Factor		: factor to convert
//
// Globals    : -
//
// Returns    : int				: percentage
//
// Remark(s)  : Direct floating point to integer conversion discards the 
//              fraction, which makes it unreliable around the unit mark 
//              (0.9999999 -> 0). Adding 0.5 makes it round, and moves the 
//              unreliable spot to the 0.5 mark - which is not used for the
//              sliders.
// 
// Example	  :	AsPercentage(0.1) = 10, AsPercentage(0.008) = 1.
//              AsPercentage(0.005) may be 0 or 1, dependent on machine and
//              compiler.
//
//----------------------------------------------------------------------------

inline int AsPercentage(double const a_Factor)
{
	return static_cast<int>((a_Factor * 100.0) + 0.5);
}

#endif	// ACTIVISION_ORIGINAL

#endif
