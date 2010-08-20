//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
// Description  :
// Id           : $Id$
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
//----------------------------------------------------------------------------
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif
#ifndef DIRECTIONS_H__
#define DIRECTIONS_H__

enum WORLD_DIRECTION
{
  NORTH,
  NORTHEAST,
  EAST,
  NORTHWEST,
  SOUTHEAST,
  WEST,
  SOUTHWEST,
  SOUTH,
#if !defined(_SMALL_MAPPOINTS)
  DOWN,
  UP,
#endif
  NOWHERE
};

#endif
