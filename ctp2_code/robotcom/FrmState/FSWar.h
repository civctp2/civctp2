#pragma once

#ifndef __FS_WAR_H__
#define __FS_WAR_H__ 1

#include "robotcom/FrmState/FrameState.h"

FRAMESTATE(FSBeginWar);
FRAMESTATE(FSUpdateThreat);
FRAMESTATE(FSRecruiteDefense);

class ArmyAgent;
class CityAgent;
#endif __FS_WAR_H__
