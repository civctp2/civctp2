#include "c3.h"

#include "XY_Coordinates.h"
#include "Cell.h"
#include "World.h"
extern World *g_theWorld;

#include "MapPoint.h"

#include "bset.h"

#include "MoveFlags.h"
































































void AiWrap(
    const MapPoint *size,
    const sint16 x,
    const sint16 y,
    const sint16 z,
    MapPoint &w
    )
{

    if (y < 0) {
        w.y = size->y + y;
    } else if (size->y <= y) {
        w.y = y - size->y;
    } else {
        w.y = y;
    }

    if (x < 0) {
        w.x = size->x + x;
        w.y = w.y - size->y/2;
        if (w.y < 0) {
            w.y += size->y;
        }
    } else if (size->x <= x) {
        w.x = x - size->x;
        w.y = (w.y + size->y/2) % size->y;
    } else {
        w.x = x;
    }

    w.z = z;
    Assert(0 <= z);
    Assert(z < 2);
}
