
#pragma once
#ifndef __UNIT_UTIL_H__
#define __UNIT_UTIL_H__

class MapPoint;
class SpecialAttackInfoRecord;
class MapPoint;
class Army;
class CellUnitList;
class Unit;
enum SPECATTACK;

void unitutil_Initialize();
sint32 unitutil_MaxActiveDefenseRange();
sint32 unitutil_GetMaxVisionRange();
sint32 unitutil_GetLandCity();
sint32 unitutil_GetSeaCity();
sint32 unitutil_GetCityTypeFor(const MapPoint &pos);
const SpecialAttackInfoRecord *unitutil_GetSpecialAttack(SPECATTACK attack);

void unitutil_GetAverageDefenseBonus(const MapPoint &pos, const Army &attackers, const CellUnitList &defenders, double & city_bonus, double & entrenched_bonus);
bool unitutil_GetCityInfo(MapPoint &pos, char * city_name, sint32 & image_index);


void unitutil_ExecuteMadLaunch(Unit & unit);


#endif
