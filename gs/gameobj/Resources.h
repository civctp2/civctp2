








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

	friend class NetCityResources;
};

#endif


