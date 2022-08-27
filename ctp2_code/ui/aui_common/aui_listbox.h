//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface list box
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
// _MSC_VER
// - Use Microsoft C++ extensions when set.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Event handlers declared in a notation that is more standard C++.
// - Always focus on the latest message.
//
//----------------------------------------------------------------------------

#ifndef __AUI_LISTBOX_H__
#define __AUI_LISTBOX_H__

#include "aui_control.h"
#include "aui_header.h"
#include "aui_ranger.h"

class aui_Item;
class aui_DropDown;

#define k_AUI_LISTBOX_LDL_HEADER			"header"
#define k_AUI_LISTBOX_LDL_RANGERX			"rangerx"
#define k_AUI_LISTBOX_LDL_RANGERY			"rangery"
#define k_AUI_LISTBOX_LDL_ALWAYSRANGER		"alwaysranger"
#define k_AUI_LISTBOX_LDL_HEADEROFFSETX		"headeroffsetx"
#define k_AUI_LISTBOX_LDL_HEADEROFFSETY		"headeroffsety"
#define k_AUI_LISTBOX_LDL_RANGERXOFFSETX	"rangerxoffsetx"
#define k_AUI_LISTBOX_LDL_RANGERXOFFSETY	"rangerxoffsety"
#define k_AUI_LISTBOX_LDL_RANGERYOFFSETX	"rangeryoffsetx"
#define k_AUI_LISTBOX_LDL_RANGERYOFFSETY	"rangeryoffsety"
#define k_AUI_LISTBOX_LDL_ITEMWIDTH			"itemwidth"
#define k_AUI_LISTBOX_LDL_ITEMHEIGHT		"itemheight"
#define k_AUI_LISTBOX_LDL_SENDRIGHTCLICKS   "sendrightclicks"

enum AUI_LISTBOX_ACTION
{
	AUI_LISTBOX_ACTION_FIRST = 0,
	AUI_LISTBOX_ACTION_NULL = 0,
	AUI_LISTBOX_ACTION_SELECT,
	AUI_LISTBOX_ACTION_DOUBLECLICKSELECT,
	AUI_LISTBOX_ACTION_RMOUSESELECT,
	AUI_LISTBOX_ACTION_LAST
};

enum AUI_LISTBOX_SELECTSTATUS
{
	AUI_LISTBOX_SELECTSTATUS_FIRST = 0,
	AUI_LISTBOX_SELECTSTATUS_NONE = 0,
	AUI_LISTBOX_SELECTSTATUS_SELECTING,
	AUI_LISTBOX_SELECTSTATUS_DESELECTING,
	AUI_LISTBOX_SELECTSTATUS_INVERTING,
	AUI_LISTBOX_SELECTSTATUS_LAST,
};

class aui_ListBox : public aui_Control
{
public:
	aui_ListBox(
		AUI_ERRCODE *retval,
		uint32 id,
		const MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	aui_ListBox(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~aui_ListBox();

protected:
	aui_ListBox() : aui_Control() {}
	AUI_ERRCODE InitCommonLdl( const MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( void );
	AUI_ERRCODE CreateRangersAndHeader( const MBCHAR *ldlBlock = NULL );

public:
	virtual AUI_ERRCODE	Resize( sint32 width, sint32 height );

	virtual AUI_ERRCODE Show( void );

	aui_Control	*GetPane( void ) const
		{ return m_pane; }
	aui_Header	*GetHeader( void ) const
		{ return m_header; }
	aui_Ranger	*GetVerticalRanger( void ) const
		{ return m_verticalRanger; }
	aui_Ranger	*GetHorizontalRanger( void ) const
		{ return m_horizontalRanger; }

	void		SetAlwaysRanger( BOOL always ) { m_alwaysRanger = always; }

	sint32		GetRangerSize( void ) const { return m_rangerSize; }
	AUI_ERRCODE	SetRangerSize( sint32 rangerSize )
		{ m_rangerSize = rangerSize; return RepositionRangers(); }

	virtual AUI_ERRCODE	AddItem( aui_Item *item );
	virtual AUI_ERRCODE InsertItem( aui_Item *item, sint32 index );
	virtual AUI_ERRCODE	RemoveItem( uint32 itemId );
	aui_Item	*GetItem( uint32 itemId ) const
		{ return (aui_Item *)m_pane->GetChild( itemId ); }

	void RemoveItems( BOOL destroy = FALSE, BOOL destroyAction = FALSE );

	AUI_ERRCODE	RemoveItemByIndex( sint32 index );
	aui_Item	*GetItemByIndex( sint32 index );

	AUI_ERRCODE	AddHeaderSwitch( aui_Switch *theSwitch );
	AUI_ERRCODE	RemoveHeaderSwitch( uint32 switchId );
	aui_Switch	*GetHeaderSwitch( uint32 switchId )
		{ return (aui_Switch *)m_header->GetChild( switchId ); }

	aui_Switch	*GetHeaderSwitchByIndex( sint32 index );

	AUI_ERRCODE	SelectItem( sint32 index, uint32 data = 0 );
	AUI_ERRCODE	SelectItem( aui_Item *item, uint32 data = 0 );
	AUI_ERRCODE	DeselectItem( sint32 index, uint32 data = 0 );
	AUI_ERRCODE	DeselectItem( aui_Item *item, uint32 data = 0 );

	virtual AUI_ERRCODE SortByColumn( sint32 column, BOOL ascending )
	{
		if ( column == -1 ) return AUI_ERRCODE_OK;

		Assert( 0 <= column && column < m_numColumns );
		if ( 0 > column || column >= m_numColumns )
			return AUI_ERRCODE_INVALIDPARAM;

		m_sortColumn = column;
		m_sortAscending = ascending;

		return AUI_ERRCODE_OK;
	}

	tech_WLList<sint32>	*GetSelectedList( void ) const
		{ return m_selectedList; }
	tech_WLList<sint32>	*GetSelectedListLastTime( void ) const
		{ return m_selectedListLastTime; }

	aui_Item	*GetSelectedItem( void ) const;
	sint32		GetSelectedItemIndex( void ) const;

	sint32 ExtractDoubleClickedItem( uint32 data ) const
	{ return (sint32)data; }

	BOOL ExtractEndUserTriggeredEvent( uint32 data ) const
	{ return (data & 0x1) == 0; }

	BOOL		IsMultiSelect( void ) const { return m_multiSelect; }
	BOOL		SetMultiSelect( BOOL multiSelect );

	BOOL		IsForceSelect( void ) const { return m_forceSelect; }
	BOOL		SetForceSelect( BOOL forceSelect );

	BOOL		IsAbsorbant( void ) const { return m_absorbEvents; }
	BOOL		SetAbsorbancy( BOOL absorbEvents );

	sint32		NumItems( void ) const { return m_numRows; }

	BOOL		IsItem( aui_Region *region );

	void WhatsChanged(
		tech_WLList<sint32> &selectedList,
		tech_WLList<sint32> &deselectedList );

	AUI_ERRCODE	RangerMoved( void );

	virtual AUI_ERRCODE Draw(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 ) { return aui_Control::DrawThis( surface, x, y ); }

	sint32 GetSortColumn( void ) const { return m_sortColumn; }
	BOOL GetSortAscending( void ) const { return m_sortAscending; }

	void BuildListStart(void);
	void BuildListEnd(bool isAddBottom = false);

	aui_Item *ConstructAndAddTextItem(const MBCHAR *ldlblock, const MBCHAR *text, void *userData);

	void SetIgnoreOutsideDrops(bool ignore) { m_ignoreOutsideDrops = ignore; }

	virtual void SendKeyboardAction();
	virtual bool HandleKey(uint32 wParam);

	void SetKeyboardActionControl(aui_Control *control);

	static aui_ListBox *ms_mouseFocusListBox;

	static aui_ListBox *GetMouseFocusListBox(void) { return ms_mouseFocusListBox; }
	static void SetMouseFocusListBox(aui_ListBox *box) { ms_mouseFocusListBox = box; }

	void ForceScroll(sint32 deltaX, sint32 deltaY);

protected:
	AUI_ERRCODE	CalculateDimensions( void );
	virtual AUI_ERRCODE	RepositionItems( void );
	virtual AUI_ERRCODE	RepositionHeaderSwitches( void );
	virtual AUI_ERRCODE	RepositionRangers( void );

	virtual sint32 WidthForItems() const { return m_width; }
	virtual sint32 HeightForItems() const { return m_height; }

	virtual aui_DragDropWindow *CreateDragDropWindow( aui_Control *dragDropItem );

	sint32		ItemsPerWidth( sint32 column );
	sint32		ColumnWidth( sint32 column );
	sint32		HorizontalRangerPositionCount( void );

	AUI_ERRCODE SwitchItems( sint32 indexA, sint32 indexB );

	AUI_ERRCODE	CalculateScroll( sint32 x, sint32 y );
	AUI_ERRCODE	ScrollList( void );

	AUI_ERRCODE	DragSelect(sint32 relativeY);

	void SendSelectCallback(
		AUI_LISTBOX_ACTION action = AUI_LISTBOX_ACTION_SELECT,
		uint32 data = 0 );

	AUI_ERRCODE	StartSelecting( void );
	friend class aui_DropDown;

	static aui_DragDropWindow *m_dragDropWindow;

	aui_Control	*m_pane;
	aui_Header	*m_header;
	aui_Ranger	*m_verticalRanger;
	aui_Ranger	*m_horizontalRanger;

	POINT		m_headerOffset;
	POINT		m_verticalRangerOffset;
	POINT		m_horizontalRangerOffset;
	sint32		m_itemWidth;
	sint32		m_itemHeight;

	tech_WLList<sint32> *m_widthList;

	sint32		m_rangerSize;

	BOOL		m_multiSelect;
	BOOL		m_forceSelect;
	BOOL		m_absorbEvents;
	BOOL		m_absorbed;

	AUI_LISTBOX_SELECTSTATUS
				m_selectStatus;

	sint32		m_dragIndex;

	tech_WLList<sint32>
				*m_selectedList;
	tech_WLList<sint32>
				*m_selectedListLastTime;
	tech_WLList<sint32>
				*m_visualSelectedList;

	BOOL		m_scrolling;
	sint32		m_scrollDx;
	sint32		m_scrollDy;

	sint32		m_numRows;
	sint32		m_numColumns;
	sint32		m_maxItemWidth;
	sint32		m_maxItemHeight;
	sint32		m_itemsPerWidth;
	sint32		m_itemsPerHeight;

	BOOL		m_alwaysRanger;

	sint32		m_sortColumn;
	BOOL		m_sortAscending;

	BOOL		m_buildingTheList;

	BOOL		m_savedForceSelect;
	bool        m_ignoreOutsideDrops;

	aui_Control *m_keyboardActionControl;

	bool        m_sendRightClicks;

	virtual void	PreChildrenCallback    (aui_MouseEvent * mouseData);
	virtual void	PostChildrenCallback   (aui_MouseEvent * mouseData);

	virtual void	MouseMoveOver          (aui_MouseEvent * mouseData);
	virtual void	MouseMoveInside        (aui_MouseEvent * mouseData);
	virtual void	MouseMoveAway          (aui_MouseEvent * mouseData);

	virtual void	MouseLDragOver         (aui_MouseEvent * mouseData);
	virtual void	MouseLDragAway         (aui_MouseEvent * mouseData);
	virtual void	MouseLDragInside       (aui_MouseEvent * mouseData);
	virtual void	MouseLDragOutside      (aui_MouseEvent * mouseData);

	virtual void	MouseLGrabInside       (aui_MouseEvent * mouseData);
	virtual void	MouseLDropInside       (aui_MouseEvent * mouseData);
	virtual void	MouseLDropOutside      (aui_MouseEvent * mouseData);

	virtual void	MouseLDoubleClickInside(aui_MouseEvent * mouseData);

	virtual void	MouseRGrabInside       (aui_MouseEvent * mouseData);
	virtual void	MouseRDropInside       (aui_MouseEvent * mouseData);
	virtual void	MouseRDragAway         (aui_MouseEvent * mouseData);
	virtual void	MouseRDragOver         (aui_MouseEvent * mouseData);

private:
	sint32 CalculateRelativeY(sint32 y) const {
		return y - m_y - (m_pane ? m_pane->Y() : 0);
	}

	sint32 CalculateItemIndexByRelativeY(sint32 relativeY) const {
		return (relativeY / (m_maxItemHeight != 0 ? m_maxItemHeight : 1))
				+ (m_verticalRanger ? m_verticalRanger->GetValueY() : 0);
	}
};

aui_Control::ControlActionCallback ListBoxRangerActionCallback;

#endif
