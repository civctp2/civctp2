

#ifndef __GRABITEM_H__
#define __GRABITEM_H__

enum GrabItemType {
	GRABITEMTYPE_NONE,

	GRABITEMTYPE_UNIT,
	GRABITEMTYPE_CITY,
	GRABITEMTYPE_TRADEROUTE,

	GRABITEM_TYPE_MAX
};

class Unit;
class TradeRoute;

class GrabItem 
{
public:
					GrabItem();
					~GrabItem();
	
	static void		Init(void);
	static void		Cleanup(void);

	BOOL			SomethingIsGrabbed(void) { return m_isGrabbed; }

	GrabItemType	GetItemType(void) { return m_grabbedItemType; }

	void			SetGrabbedItem(Unit *unit);
	void			GetGrabbedItem(Unit **unit);

	void			SetGrabbedItem(TradeRoute *route);
	void			GetGrabbedItem(TradeRoute **route);

	void			Release(void) { m_isGrabbed = FALSE; m_grabbedItem = NULL; m_grabbedItemType = GRABITEMTYPE_NONE; }

private:
	BOOL			m_isGrabbed;	
	
	void			*m_grabbedItem;
	GrabItemType	m_grabbedItemType;

};


#endif
