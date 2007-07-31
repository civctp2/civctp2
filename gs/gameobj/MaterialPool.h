









#pragma once
#ifndef _MATERIAL_POOL_H_
#define _MATERIAL_POOL_H_

#define k_DEFAULT_MATERIAL_CAP 100
#define k_DEFAULT_MATERIALS_TAX 0.2

class CivArchive;

class MaterialPool
{
private:
	sint32 m_level;
	sint32 m_owner;
	sint32 m_cap;

public:
	MaterialPool(sint32 owner) {
		m_level = 0;
		m_cap = k_DEFAULT_MATERIAL_CAP;
		m_owner = owner;
	}

	void AddMaterials(sint32 amt);
	sint32 GetMaterials() const { return m_level; }
	void SubtractMaterials(sint32 amt) { Assert(m_level >= amt); m_level -= amt;}
	void CheatAddMaterials(sint32 amt);
	sint32 CheatSubtractMaterials(sint32 amt);

	void SetLevel(sint32 level) { m_level = level; }

	void SetCap(sint32 cap) { m_cap = cap; }
	sint32 GetCap() { return m_cap; }
	void Serialize(CivArchive &archive);
};

#endif
