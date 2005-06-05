//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : City ressource lists
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added GetNum and Resize methods for loading of savegames with different 
//   number of goods in than in the database. - May 28th 2005 Martin Gühmann
//
//----------------------------------------------------------------------------

#pragma once
#ifndef _RESOURCES_H_
#define _RESOURCES_H_

class CivArchive;

class Resources {
private:
	sint32 m_numGoods;
	sint32 *m_supply;
	sint32 m_totalResources;

	friend class NetCity;

public:
	Resources();
	Resources(const Resources &copyme);
	~Resources() {
		if(m_supply)
			delete [] m_supply;
	}

	void Clear()
	{
		memset(m_supply, 0, m_numGoods * sizeof(sint32));
		m_totalResources = 0;
	}

	void Resize(sint32 newSize);

	const sint32 & operator [] (const sint32 index) const
	{
		Assert(index >= 0 && index < m_numGoods);
		return m_supply[index];
	}

	Resources &operator = (Resources &copyme);

	sint32 GetResourceCount(sint32 index)
	{
		return m_supply[index];
	}

	sint32 AddResource(sint32 index, sint32 amt = 1)
	{
		m_totalResources += amt;
		Assert(m_totalResources >= 0);
		return (m_supply[index] += amt);
	}

	void SetResourceCount(sint32 index, sint32 amt)
	{
		m_totalResources -= m_supply[index] - amt;
		Assert(m_totalResources >= 0);
		m_supply[index] = amt;
	}

	sint32 GetTotalResources() {
		return m_totalResources;
	}

	void Serialize(CivArchive &archive);

	sint32 FindMatch(const Resources *r2) const {
		for(sint32 i = 0; i < m_numGoods; i++) {
			if(m_supply[i] && r2->m_supply[i])
				return i;
		}
		return -1;
	}

	//----------------------------------------------------------------------------
	//
	// Name       : Resources::GetNum
	//
	// Description: Gets the number of elements of the underlying m_supply array.
	//
	// Parameters : -
	//
	// Globals    : -
	//
	// Returns    : The size of the underlying m_supply arry.
	//
	// Remark(s)  : Added for valid check so that savegames with modified 
	//              ressource database can be loaded.
	//
	//----------------------------------------------------------------------------
	sint32 GetNum() {
		return m_numGoods;
	}

	friend class NetCityResources;
};

#endif


