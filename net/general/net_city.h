









#pragma once
#ifndef _NET_CITY_H_
#define _NET_CITY_H_

#include "net_packet.h"

class UnitData;
class CityData;
class Resources;

class NetCity : public Packetizer
{
public:
	NetCity(UnitData*, BOOL isInitialPacket = FALSE);
	NetCity() {}

	void Packetize(uint8* buf, uint16& size);
	void Unpacketize(uint16 id, uint8* buf, uint16 size);
	void PacketizeResources(const Resources &resources, uint8 *buf, uint16 &size);
	void UnpacketizeResources(Resources &resources, uint8 *buf, sint32 &pos);

private:
	UnitData* m_unitData;
	uint8 m_isInitialPacket;
};

class NetCity2 : public Packetizer
{
public:
	NetCity2(CityData *cd, uint8 isInitialPacket = 0) 
	{ 
		m_data = cd; 
		m_isInitialPacket = isInitialPacket;
	}
	NetCity2() {}

	void Packetize(uint8* buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
private:
	CityData *m_data;
	uint8 m_isInitialPacket;
};

class NetCityName : public Packetizer
{
public:
	NetCityName(CityData* cd) { m_cityData = cd;}
	NetCityName() {}
	void Packetize(uint8* buf, uint16 &size);
	void Unpacketize(uint16 id, uint8* buf, uint16 size);
private:
	CityData *m_cityData;
};

class NetCityBuildQueue : public Packetizer
{
public:
	NetCityBuildQueue(CityData *cd) { m_cityData = cd;}
	NetCityBuildQueue() {}

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
private:
	CityData *m_cityData;
};

class NetCityResources : public Packetizer
{
public:
	NetCityResources(CityData *cd) { m_cityData = cd; }
	NetCityResources() {}

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
private:
	CityData *m_cityData;
};

#else
class NetCity;
#endif
