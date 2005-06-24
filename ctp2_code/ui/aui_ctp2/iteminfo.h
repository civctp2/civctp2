#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __ITEMINFO_H__
#define __ITEMINFO_H__

class ItemInfo {
public:
	ItemInfo();
	~ItemInfo();

	sint32 GetId(void) { return m_infoId; }
	void SetId(sint32 id) { m_infoId = id; }
	sint32 GetCategory(void) { return m_category; }
	void SetCategory(sint32 category) { m_category = category; }
	sint32 GetItemType(void) { return m_itemType; }
	void SetItemType(sint32 itemType) { m_itemType = itemType; }

protected:
	sint32 m_infoId;
	sint32 m_category;
	sint32 m_itemType;
};

#endif
