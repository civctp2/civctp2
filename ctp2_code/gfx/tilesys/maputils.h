











#pragma once
#ifndef __MAPUTILS_H__
#define __MAPUTILS_H__

void	maputils_WrapPoint(sint32 x,sint32 y,sint32 *wrapX,sint32 *wrapY);
BOOL	maputils_TilePointInTileRect(sint32 x, sint32 y, RECT *tileRect);

sint32	maputils_TileX2MapX(sint32 tileX,sint32 tileY);
void	maputils_MapX2TileX(sint32 mapX,sint32 mapY,sint32 *tileX);
void	maputils_MapXY2PixelXY(sint32 mapX, sint32 mapY,sint32 *pixelX,sint32 *pixelY);
void	maputils_MapXY2PixelXY(sint32 mapX, sint32 mapY,sint32 *pixelX,sint32 *pixelY, RECT *mapViewRect);


void	maputils_TileX2MapXAbs(sint32 tileX,sint32 tileY,sint32 *mapX);

#endif