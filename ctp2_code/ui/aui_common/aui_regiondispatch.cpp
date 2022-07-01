#include <iostream>
#include "c3.h"

#include "aui_region.h"

#include "SelItem.h"

typedef void (aui_Region::*MouseFunction)(aui_MouseEvent * mouseData);
class MouseDispatcher
{
public:
	MouseDispatcher(
			MouseFunction mouseNoChange,
			MouseFunction mouseMoveOver,
			MouseFunction mouseMoveOverEdit,
			MouseFunction mouseMoveAway,
			MouseFunction mouseMoveAwayEdit,
			MouseFunction mouseMoveInside,
			MouseFunction mouseMoveInsideEdit,
			MouseFunction mouseMoveOutside,
			MouseFunction mouseMoveOutsideEdit,
			MouseFunction mouseLeftDragOver,
			MouseFunction mouseLeftDragOverEdit,
			MouseFunction mouseLeftDragAway,
			MouseFunction mouseLeftDragAwayEdit,
			MouseFunction mouseLeftDragInside,
			MouseFunction mouseLeftDragInsideEdit,
			MouseFunction mouseLeftDragOutside,
			MouseFunction mouseLeftDragOutsideEdit,
			MouseFunction mouseLeftGrabInside,
			MouseFunction mouseLeftGrabInsideEdit,
			MouseFunction mouseLeftGrabOutside,
			MouseFunction mouseLeftGrabOutsideEdit,
			MouseFunction mouseLeftDropInside,
			MouseFunction mouseLeftDropInsideEdit,
			MouseFunction mouseLeftDropOutside,
			MouseFunction mouseLeftDropOutsideEdit,
			MouseFunction mouseLeftDoubleClickInside,
			MouseFunction mouseLeftDoubleClickInsideEdit,
			MouseFunction mouseLeftDoubleClickOutside,
			MouseFunction mouseLeftDoubleClickOutsideEdit,
			MouseFunction mouseRightDragOver,
			MouseFunction mouseRightDragOverEdit,
			MouseFunction mouseRightDragAway,
			MouseFunction mouseRightDragAwayEdit,
			MouseFunction mouseRightDragInside,
			MouseFunction mouseRightDragInsideEdit,
			MouseFunction mouseRightDragOutside,
			MouseFunction mouseRightDragOutsideEdit,
			MouseFunction mouseRightGrabInside,
			MouseFunction mouseRightGrabInsideEdit,
			MouseFunction mouseRightGrabOutside,
			MouseFunction mouseRightGrabOutsideEdit,
			MouseFunction mouseRightDropInside,
			MouseFunction mouseRightDropInsideEdit,
			MouseFunction mouseRightDropOutside,
			MouseFunction mouseRightDropOutsideEdit,
			MouseFunction mouseRightDoubleClickInside,
			MouseFunction mouseRightDoubleClickInsideEdit,
			MouseFunction mouseRightDoubleClickOutside,
			MouseFunction mouseRightDoubleClickOutsideEdit);

	void Dispatch(aui_Region & region, aui_MouseEvent & event, bool handleIt, bool edit);

public:
	bool           m_noChange;
	uint32         m_noChangeTime;
	bool           m_isMouseInside;
	aui_MouseEvent m_mouseEvent;
	bool           m_doubleClickingInside;
	uint32         m_doubleClickTimeOut;
	POINT          m_doubleClickOldPos;

private:
	static const uint32 BUTTON_NONE         = 0;
	static const uint32 BUTTON_LEFT         = 0;
	static const uint32 BUTTON_RIGHT        = 1;
	static const uint32 BUTTON_MAX          = 2;

	static const uint32 ACTION_MOVE         = 0;
	static const uint32 ACTION_DRAG         = 1;
	static const uint32 ACTION_GRAB         = 2;
	static const uint32 ACTION_DROP         = 3;
	static const uint32 ACTION_DOUBLE_CLICK = 4;
	static const uint32 ACTION_MAX          = 5;

	static const uint32 AREA_OVER           = 0;
	static const uint32 AREA_AWAY           = 1;
	static const uint32 AREA_INSIDE         = 2;
	static const uint32 AREA_OUTSIDE        = 3;
	static const uint32 AREA_MAX            = 4;

	static const uint32 GROUP_NORMAL        = 0;
	static const uint32 GROUP_EDIT          = 1;
	static const uint32 GROUP_MAX           = 2;

	void HandleClick(aui_Region & region, aui_MouseEvent & event, uint32 button, uint32 area, uint32 group);
	void HandleMouseMoved(aui_Region & region, aui_MouseEvent & event, uint32 area, uint32 group);
	void HandleMouseStatic(aui_Region & region, aui_MouseEvent & event, uint32 area, uint32 group);
	bool MovedLessThanClickDistance(const aui_MouseEvent & event) const;
	void ExecuteMouseFunction(aui_Region & region, aui_MouseEvent & event, uint32 button, uint32 action, uint32 area, uint32 group) const;

	MouseFunction m_mouseNoChange;
	MouseFunction m_mouseFunctions[BUTTON_MAX][ACTION_MAX][AREA_MAX][GROUP_MAX];
	uint32        m_doubleClickStartWaitTime[BUTTON_MAX];
	sint32        m_xLastTime;
	sint32        m_yLastTime;
};

MouseDispatcher::MouseDispatcher(
		MouseFunction mouseNoChange,
		MouseFunction mouseMoveOver,
		MouseFunction mouseMoveOverEdit,
		MouseFunction mouseMoveAway,
		MouseFunction mouseMoveAwayEdit,
		MouseFunction mouseMoveInside,
		MouseFunction mouseMoveInsideEdit,
		MouseFunction mouseMoveOutside,
		MouseFunction mouseMoveOutsideEdit,
		MouseFunction mouseLeftDragOver,
		MouseFunction mouseLeftDragOverEdit,
		MouseFunction mouseLeftDragAway,
		MouseFunction mouseLeftDragAwayEdit,
		MouseFunction mouseLeftDragInside,
		MouseFunction mouseLeftDragInsideEdit,
		MouseFunction mouseLeftDragOutside,
		MouseFunction mouseLeftDragOutsideEdit,
		MouseFunction mouseLeftGrabInside,
		MouseFunction mouseLeftGrabInsideEdit,
		MouseFunction mouseLeftGrabOutside,
		MouseFunction mouseLeftGrabOutsideEdit,
		MouseFunction mouseLeftDropInside,
		MouseFunction mouseLeftDropInsideEdit,
		MouseFunction mouseLeftDropOutside,
		MouseFunction mouseLeftDropOutsideEdit,
		MouseFunction mouseLeftDoubleClickInside,
		MouseFunction mouseLeftDoubleClickInsideEdit,
		MouseFunction mouseLeftDoubleClickOutside,
		MouseFunction mouseLeftDoubleClickOutsideEdit,
		MouseFunction mouseRightDragOver,
		MouseFunction mouseRightDragOverEdit,
		MouseFunction mouseRightDragAway,
		MouseFunction mouseRightDragAwayEdit,
		MouseFunction mouseRightDragInside,
		MouseFunction mouseRightDragInsideEdit,
		MouseFunction mouseRightDragOutside,
		MouseFunction mouseRightDragOutsideEdit,
		MouseFunction mouseRightGrabInside,
		MouseFunction mouseRightGrabInsideEdit,
		MouseFunction mouseRightGrabOutside,
		MouseFunction mouseRightGrabOutsideEdit,
		MouseFunction mouseRightDropInside,
		MouseFunction mouseRightDropInsideEdit,
		MouseFunction mouseRightDropOutside,
		MouseFunction mouseRightDropOutsideEdit,
		MouseFunction mouseRightDoubleClickInside,
		MouseFunction mouseRightDoubleClickInsideEdit,
		MouseFunction mouseRightDoubleClickOutside,
		MouseFunction mouseRightDoubleClickOutsideEdit)
:
	m_noChange(false),
	m_noChangeTime(0),
	m_isMouseInside(false),
	m_mouseEvent(),
	m_doubleClickingInside(true),
	m_doubleClickTimeOut(0),
	m_doubleClickOldPos(),
	m_xLastTime(0),
	m_yLastTime(0)
{
	m_mouseNoChange = mouseNoChange;
	m_mouseFunctions[BUTTON_NONE][ACTION_MOVE][AREA_OVER][GROUP_NORMAL] = mouseMoveOver;
	m_mouseFunctions[BUTTON_NONE][ACTION_MOVE][AREA_OVER][GROUP_EDIT] = mouseMoveOverEdit;
	m_mouseFunctions[BUTTON_NONE][ACTION_MOVE][AREA_AWAY][GROUP_NORMAL] = mouseMoveAway;
	m_mouseFunctions[BUTTON_NONE][ACTION_MOVE][AREA_AWAY][GROUP_EDIT] = mouseMoveAwayEdit;
	m_mouseFunctions[BUTTON_NONE][ACTION_MOVE][AREA_INSIDE][GROUP_NORMAL] = mouseMoveInside;
	m_mouseFunctions[BUTTON_NONE][ACTION_MOVE][AREA_INSIDE][GROUP_EDIT] = mouseMoveInsideEdit;
	m_mouseFunctions[BUTTON_NONE][ACTION_MOVE][AREA_OUTSIDE][GROUP_NORMAL] = mouseMoveOutside;
	m_mouseFunctions[BUTTON_NONE][ACTION_MOVE][AREA_OUTSIDE][GROUP_EDIT] = mouseMoveOutsideEdit;

	m_mouseFunctions[BUTTON_LEFT][ACTION_DRAG][AREA_OVER][GROUP_NORMAL] = mouseLeftDragOver;
	m_mouseFunctions[BUTTON_LEFT][ACTION_DRAG][AREA_OVER][GROUP_EDIT] = mouseLeftDragOverEdit;
	m_mouseFunctions[BUTTON_LEFT][ACTION_DRAG][AREA_AWAY][GROUP_NORMAL] = mouseLeftDragAway;
	m_mouseFunctions[BUTTON_LEFT][ACTION_DRAG][AREA_AWAY][GROUP_EDIT] = mouseLeftDragAwayEdit;
	m_mouseFunctions[BUTTON_LEFT][ACTION_DRAG][AREA_INSIDE][GROUP_NORMAL] = mouseLeftDragInside;
	m_mouseFunctions[BUTTON_LEFT][ACTION_DRAG][AREA_INSIDE][GROUP_EDIT] = mouseLeftDragInsideEdit;
	m_mouseFunctions[BUTTON_LEFT][ACTION_DRAG][AREA_OUTSIDE][GROUP_NORMAL] = mouseLeftDragOutside;
	m_mouseFunctions[BUTTON_LEFT][ACTION_DRAG][AREA_OUTSIDE][GROUP_EDIT] = mouseLeftDragOutsideEdit;

	m_mouseFunctions[BUTTON_LEFT][ACTION_GRAB][AREA_INSIDE][GROUP_NORMAL] = mouseLeftGrabInside;
	m_mouseFunctions[BUTTON_LEFT][ACTION_GRAB][AREA_INSIDE][GROUP_EDIT] = mouseLeftGrabInsideEdit;
	m_mouseFunctions[BUTTON_LEFT][ACTION_GRAB][AREA_OUTSIDE][GROUP_NORMAL] = mouseLeftGrabOutside;
	m_mouseFunctions[BUTTON_LEFT][ACTION_GRAB][AREA_OUTSIDE][GROUP_EDIT] = mouseLeftGrabOutsideEdit;

	m_mouseFunctions[BUTTON_LEFT][ACTION_DROP][AREA_INSIDE][GROUP_NORMAL] = mouseLeftDropInside;
	m_mouseFunctions[BUTTON_LEFT][ACTION_DROP][AREA_INSIDE][GROUP_EDIT] = mouseLeftDropInsideEdit;
	m_mouseFunctions[BUTTON_LEFT][ACTION_DROP][AREA_OUTSIDE][GROUP_NORMAL] = mouseLeftDropOutside;
	m_mouseFunctions[BUTTON_LEFT][ACTION_DROP][AREA_OUTSIDE][GROUP_EDIT] = mouseLeftDropOutsideEdit;

	m_mouseFunctions[BUTTON_LEFT][ACTION_DOUBLE_CLICK][AREA_INSIDE][GROUP_NORMAL] = mouseLeftDoubleClickInside;
	m_mouseFunctions[BUTTON_LEFT][ACTION_DOUBLE_CLICK][AREA_INSIDE][GROUP_EDIT] = mouseLeftDoubleClickInsideEdit;
	m_mouseFunctions[BUTTON_LEFT][ACTION_DOUBLE_CLICK][AREA_OUTSIDE][GROUP_NORMAL] = mouseLeftDoubleClickOutside;
	m_mouseFunctions[BUTTON_LEFT][ACTION_DOUBLE_CLICK][AREA_OUTSIDE][GROUP_EDIT] = mouseLeftDoubleClickOutsideEdit;

	m_mouseFunctions[BUTTON_RIGHT][ACTION_DRAG][AREA_OVER][GROUP_NORMAL] = mouseRightDragOver;
	m_mouseFunctions[BUTTON_RIGHT][ACTION_DRAG][AREA_OVER][GROUP_EDIT] = mouseRightDragOverEdit;
	m_mouseFunctions[BUTTON_RIGHT][ACTION_DRAG][AREA_AWAY][GROUP_NORMAL] = mouseRightDragAway;
	m_mouseFunctions[BUTTON_RIGHT][ACTION_DRAG][AREA_AWAY][GROUP_EDIT] = mouseRightDragAwayEdit;
	m_mouseFunctions[BUTTON_RIGHT][ACTION_DRAG][AREA_INSIDE][GROUP_NORMAL] = mouseRightDragInside;
	m_mouseFunctions[BUTTON_RIGHT][ACTION_DRAG][AREA_INSIDE][GROUP_EDIT] = mouseRightDragInsideEdit;
	m_mouseFunctions[BUTTON_RIGHT][ACTION_DRAG][AREA_OUTSIDE][GROUP_NORMAL] = mouseRightDragOutside;
	m_mouseFunctions[BUTTON_RIGHT][ACTION_DRAG][AREA_OUTSIDE][GROUP_EDIT] = mouseRightDragOutsideEdit;

	m_mouseFunctions[BUTTON_RIGHT][ACTION_GRAB][AREA_INSIDE][GROUP_NORMAL] = mouseRightGrabInside;
	m_mouseFunctions[BUTTON_RIGHT][ACTION_GRAB][AREA_INSIDE][GROUP_EDIT] = mouseRightGrabInsideEdit;
	m_mouseFunctions[BUTTON_RIGHT][ACTION_GRAB][AREA_OUTSIDE][GROUP_NORMAL] = mouseRightGrabOutside;
	m_mouseFunctions[BUTTON_RIGHT][ACTION_GRAB][AREA_OUTSIDE][GROUP_EDIT] = mouseRightGrabOutsideEdit;

	m_mouseFunctions[BUTTON_RIGHT][ACTION_DROP][AREA_INSIDE][GROUP_NORMAL] = mouseRightDropInside;
	m_mouseFunctions[BUTTON_RIGHT][ACTION_DROP][AREA_INSIDE][GROUP_EDIT] = mouseRightDropInsideEdit;
	m_mouseFunctions[BUTTON_RIGHT][ACTION_DROP][AREA_OUTSIDE][GROUP_NORMAL] = mouseRightDropOutside;
	m_mouseFunctions[BUTTON_RIGHT][ACTION_DROP][AREA_OUTSIDE][GROUP_EDIT] = mouseRightDropOutsideEdit;

	m_mouseFunctions[BUTTON_RIGHT][ACTION_DOUBLE_CLICK][AREA_INSIDE][GROUP_NORMAL] = mouseRightDoubleClickInside;
	m_mouseFunctions[BUTTON_RIGHT][ACTION_DOUBLE_CLICK][AREA_INSIDE][GROUP_EDIT] = mouseRightDoubleClickInsideEdit;
	m_mouseFunctions[BUTTON_RIGHT][ACTION_DOUBLE_CLICK][AREA_OUTSIDE][GROUP_NORMAL] = mouseRightDoubleClickOutside;
	m_mouseFunctions[BUTTON_RIGHT][ACTION_DOUBLE_CLICK][AREA_OUTSIDE][GROUP_EDIT] = mouseRightDoubleClickOutsideEdit;

	m_doubleClickStartWaitTime[BUTTON_LEFT] = 0;
	m_doubleClickStartWaitTime[BUTTON_RIGHT] = 0;
}

void MouseDispatcher::ExecuteMouseFunction(aui_Region & region, aui_MouseEvent & event, uint32 button, uint32 action, uint32 area,
                                           uint32 group) const
{
	// std::cout << "Dispatch for region:: (" << region.Id() << ") :: (" << button << ", " << action << ", " << area << ", " << group << ")" << std::endl;
	(region.*m_mouseFunctions[button][action][area][group])(&event);
}

bool MouseDispatcher::MovedLessThanClickDistance(const aui_MouseEvent & event) const
{
	const sint32 k_DOUBLE_CLICK_TRAVEL_DISTANCE = 5;

	sint32 xDistance = abs(event.position.x - m_doubleClickOldPos.x);
	sint32 yDistance = abs(event.position.y - m_doubleClickOldPos.y);

	return (xDistance < k_DOUBLE_CLICK_TRAVEL_DISTANCE && yDistance < k_DOUBLE_CLICK_TRAVEL_DISTANCE);
}

void MouseDispatcher::HandleClick(
		aui_Region & region,
		aui_MouseEvent & event,
		uint32 button,
		uint32 area,
		uint32 group)
{
	if (
			((button == BUTTON_RIGHT) || (area == AREA_OUTSIDE) || (group == GROUP_EDIT) || MovedLessThanClickDistance(event))
			&& (m_doubleClickingInside ^ (area == AREA_OUTSIDE)) // (m_doubleClickingInside and !outside) or (!m_doubleClickingInside and outside)
			&& event.time - m_doubleClickStartWaitTime[button] < m_doubleClickTimeOut
	)
	{
		ExecuteMouseFunction(region, event, button, ACTION_DOUBLE_CLICK, area, group);
	}
	else
	{
		m_doubleClickingInside = (area != AREA_OUTSIDE);
		m_doubleClickStartWaitTime[button] = event.time;
		if (button == BUTTON_LEFT && group == GROUP_NORMAL)
		{
			m_doubleClickOldPos = event.position;
		}
		ExecuteMouseFunction(region, event, button, ACTION_GRAB, area, group);
	}
}

void MouseDispatcher::HandleMouseMoved(
		aui_Region & region,
		aui_MouseEvent & event,
		uint32 area,
		uint32 group)
{
	uint32 inOutArea = ((area == AREA_INSIDE || area == AREA_OVER) ? AREA_INSIDE : AREA_OUTSIDE);
	if (event.lbutton)
	{
		if (m_mouseEvent.lbutton)
		{
			ExecuteMouseFunction(region, event, BUTTON_LEFT, ACTION_DRAG, area, group);
			if (event.rbutton)
			{
				if (m_mouseEvent.rbutton)
				{
					ExecuteMouseFunction(region, event, BUTTON_RIGHT, ACTION_DRAG, area, group);
				}
				else
				{
					HandleClick(region, event, BUTTON_RIGHT, inOutArea, group);
				}
			}
			else
			{
				if (m_mouseEvent.rbutton)
				{
					ExecuteMouseFunction(region, event, BUTTON_RIGHT, ACTION_DRAG, area, group);
					ExecuteMouseFunction(region, event, BUTTON_RIGHT, ACTION_DROP, inOutArea, group);
				}
			}
		}
		else
		{
			if (event.rbutton)
			{
				if (m_mouseEvent.rbutton)
				{
					ExecuteMouseFunction(region, event, BUTTON_RIGHT, ACTION_DRAG, area, group);
					HandleClick(region, event, BUTTON_LEFT, inOutArea, group);
				}
				else
				{
					ExecuteMouseFunction(region, event, BUTTON_NONE, ACTION_MOVE, area, group);
					HandleClick(region, event, BUTTON_LEFT, inOutArea, group);
					HandleClick(region, event, BUTTON_RIGHT, inOutArea, group);
				}
			}
			else
			{
				if (m_mouseEvent.rbutton)
				{
					ExecuteMouseFunction(region, event, BUTTON_RIGHT, ACTION_DRAG, area, group);
					HandleClick(region, event, BUTTON_LEFT, inOutArea, group);
					ExecuteMouseFunction(region, event, BUTTON_RIGHT, ACTION_DROP, inOutArea, group);
				}
				else
				{
					ExecuteMouseFunction(region, event, BUTTON_NONE, ACTION_MOVE, area,  group);
					HandleClick(region, event, BUTTON_LEFT, inOutArea, group);
				}
			}
		}
	}
	else
	{
		if (m_mouseEvent.lbutton)
		{
			if (inOutArea == AREA_INSIDE && group == GROUP_NORMAL && g_selected_item)
			{
				g_selected_item->RegisterUIClick();
			}

			ExecuteMouseFunction(region, event, BUTTON_LEFT, ACTION_DRAG, area, group);
			if (event.rbutton)
			{
				if (m_mouseEvent.rbutton)
				{
					ExecuteMouseFunction(region, event, BUTTON_RIGHT, ACTION_DRAG, area, group);
				}
				else
				{
					HandleClick(region, event, BUTTON_RIGHT, inOutArea, group);
				}
				ExecuteMouseFunction(region, event, BUTTON_LEFT, ACTION_DROP, inOutArea, group);
			}
			else
			{
				if (m_mouseEvent.rbutton)
				{
					ExecuteMouseFunction(region, event, BUTTON_RIGHT, ACTION_DRAG, area, group);
					ExecuteMouseFunction(region, event, BUTTON_LEFT, ACTION_DROP, inOutArea, group);
					ExecuteMouseFunction(region, event, BUTTON_RIGHT, ACTION_DROP, inOutArea, group);
				}
				else
				{
					ExecuteMouseFunction(region, event, BUTTON_LEFT, ACTION_DROP, inOutArea, group);
				}
			}
		}
		else
		{
			if (event.rbutton)
			{
				if (m_mouseEvent.rbutton)
				{
					ExecuteMouseFunction(region, event, BUTTON_RIGHT, ACTION_DRAG, area, group);
				}
				else
				{
					ExecuteMouseFunction(region, event, BUTTON_NONE, ACTION_MOVE, area, group);
					HandleClick(region, event, BUTTON_RIGHT, inOutArea, group);
				}
			}
			else
			{
				if (m_mouseEvent.rbutton)
				{
					ExecuteMouseFunction(region, event, BUTTON_RIGHT, ACTION_DRAG, area, group);
					ExecuteMouseFunction(region, event, BUTTON_RIGHT, ACTION_DROP, inOutArea, group);
				}
				else
				{
					ExecuteMouseFunction(region, event, BUTTON_NONE, ACTION_MOVE, area, group);
				}
			}
		}
	}
}

void MouseDispatcher::HandleMouseStatic(aui_Region & region, aui_MouseEvent & event, uint32 area, uint32 group)
{
	if (event.lbutton )
	{
		if (m_mouseEvent.lbutton)
		{
			if (event.rbutton)
			{
				if (m_mouseEvent.rbutton)
				{
					m_noChange = true;
					(region.*m_mouseNoChange)(&event);
				}
				else
				{
					HandleClick(region, event, BUTTON_RIGHT, area, group);
				}
			}
			else
			{
				if (m_mouseEvent.rbutton)
				{
					ExecuteMouseFunction(region, event, BUTTON_RIGHT, ACTION_DROP, area, group);
				}
				else
				{
					m_noChange = true;
					(region.*m_mouseNoChange)(&event);
				}
			}
		}
		else
		{
			HandleClick(region, event, BUTTON_LEFT, area, group);
			if (event.rbutton)
			{
				if (!m_mouseEvent.rbutton)
				{
					HandleClick(region, event, BUTTON_RIGHT, area, group);
				}
			}
			else
			{
				if (m_mouseEvent.rbutton)
				{
					ExecuteMouseFunction(region, event, BUTTON_RIGHT, ACTION_DROP, area, group);
				}
			}
		}
	}
	else
	{
		if (m_mouseEvent.lbutton)
		{
			if (area == AREA_INSIDE && group == GROUP_NORMAL && g_selected_item)
			{
				g_selected_item->RegisterUIClick();
			}

			if (event.rbutton)
			{
				if (!m_mouseEvent.rbutton)
				{
					HandleClick(region, event, BUTTON_RIGHT, area, group);
				}
				ExecuteMouseFunction(region, event, BUTTON_LEFT, ACTION_DROP, area, group);
			}
			else
			{
				ExecuteMouseFunction(region, event, BUTTON_LEFT, ACTION_DROP, area, group);
				if (m_mouseEvent.rbutton)
				{
					ExecuteMouseFunction(region, event, BUTTON_RIGHT, ACTION_DROP, area, group);
				}
			}
		}
		else
		{
			if (event.rbutton)
			{
				if (m_mouseEvent.rbutton)
				{
					m_noChange = true;
					(region.*m_mouseNoChange)(&event);
				}
				else
				{
					HandleClick(region, event, BUTTON_RIGHT, area, group);
				}
			}
			else
			{
				if (m_mouseEvent.rbutton)
				{
					ExecuteMouseFunction(region, event, BUTTON_RIGHT, ACTION_DROP, area, group);
				}
				else
				{
					m_noChange = true;
					(region.*m_mouseNoChange)(&event);
				}
			}
		}
	}
}

void MouseDispatcher::Dispatch(aui_Region & region, aui_MouseEvent & event, bool handleIt, bool edit)
{
	uint32 group = edit ? GROUP_EDIT : GROUP_NORMAL;
	if (group == GROUP_NORMAL)
	{
		if (!m_noChange) {
			m_noChangeTime = event.time;
		} else {
			m_noChange = false;
		}
	}

	if (event.position.x != m_mouseEvent.position.x
		|| event.position.y != m_mouseEvent.position.y
		|| region.X() != m_xLastTime
		|| region.Y() != m_yLastTime)
	{
		bool wasMouseInside = m_isMouseInside;

		m_isMouseInside = region.IsInside(&event.position);
		if (handleIt)
		{
			uint32 area = m_isMouseInside
					? (wasMouseInside ? AREA_INSIDE : AREA_OVER)
					: (wasMouseInside ? AREA_AWAY : AREA_OUTSIDE);
			HandleMouseMoved(region, event, area, group);
		}
	}
	else
	{
		if (handleIt)
		{
			HandleMouseStatic(region, event, m_isMouseInside ? AREA_INSIDE : AREA_OUTSIDE, group);
		}
	}
	memcpy(&m_mouseEvent, &event, sizeof(aui_MouseEvent));
	m_xLastTime = region.X();
	m_yLastTime = region.Y();
}

MouseDispatcher * aui_Region::createMouseDispatcher() {
	auto * mouseDispatcher = new MouseDispatcher(
		&aui_Region::MouseNoChange,
		&aui_Region::MouseMoveOver,
		&aui_Region::MouseNoOperation, // No operation
		&aui_Region::MouseMoveAway,
		&aui_Region::MouseNoOperation, // No operation
		&aui_Region::MouseMoveInside,
		&aui_Region::MouseNoOperation, // No operation
		&aui_Region::MouseMoveOutside,
		&aui_Region::MouseNoOperation, // No operation
		&aui_Region::MouseLDragOver,
		&aui_Region::MouseLDragEditMode, // General LDrag-edit-mode
		&aui_Region::MouseLDragAway,
		&aui_Region::MouseLDragEditMode, // General LDrag-edit-mode
		&aui_Region::MouseLDragInside,
		&aui_Region::MouseLDragEditMode, // General LDrag-edit-mode
		&aui_Region::MouseLDragOutside,
		&aui_Region::MouseLDragEditMode, // General LDrag-edit-mode
		&aui_Region::MouseLGrabInside,
		&aui_Region::MouseLGrabEditMode, // General LGrab-edit-mode
		&aui_Region::MouseLGrabOutside,
		&aui_Region::MouseLGrabEditMode, // General LGrab-edit-mode
		&aui_Region::MouseLDropInside,
		&aui_Region::MouseLDropEditMode, // General LDrop-edit-mode
		&aui_Region::MouseLDropOutside,
		&aui_Region::MouseLDropEditMode, // General LDrop-edit-mode
		&aui_Region::MouseLDoubleClickInside,
		&aui_Region::MouseNoOperation, // No operation
		&aui_Region::MouseLDoubleClickOutside,
		&aui_Region::MouseNoOperation, // No operation
		&aui_Region::MouseRDragOver,
		&aui_Region::MouseNoOperation, // No operation
		&aui_Region::MouseRDragAway,
		&aui_Region::MouseNoOperation, // No operation
		&aui_Region::MouseRDragInside,
		&aui_Region::MouseNoOperation, // No operation
		&aui_Region::MouseRDragOutside,
		&aui_Region::MouseNoOperation, // No operation
		&aui_Region::MouseRGrabInside,
		&aui_Region::MouseRGrabInsideEdit,
		&aui_Region::MouseRGrabOutside,
		&aui_Region::MouseRGrabOutsideEdit,
		&aui_Region::MouseRDropInside,
		&aui_Region::MouseRDropInsideEdit,
		&aui_Region::MouseRDropOutside,
		&aui_Region::MouseRDropOutsideEdit,
		&aui_Region::MouseRDoubleClickInside,
		&aui_Region::MouseNoOperation, // No operation
		&aui_Region::MouseRDoubleClickOutside,
		&aui_Region::MouseNoOperation); // No operation

#ifdef __AUI_USE_DIRECTX__
	mouseDispatcher->m_doubleClickTimeOut = GetDoubleClickTime();
#else
	mouseDispatcher->m_doubleClickTimeOut = 375;
#endif
	memset(&mouseDispatcher->m_mouseEvent, 0, sizeof(mouseDispatcher->m_mouseEvent));
	return mouseDispatcher;
}

void aui_Region::MouseDispatch(MouseDispatcher & mouseDispatcher, aui_MouseEvent & event, bool handleIt, bool edit)
{
	mouseDispatcher.Dispatch(*this, event, handleIt, edit);
}

bool aui_Region::IsMouseInside() const
{
	return m_mouseDispatcher->m_isMouseInside;
}

uint32 aui_Region::GetNoChangeTime() const
{
	return m_mouseDispatcher->m_noChangeTime;
}

void aui_Region::SetMouseEvent(aui_MouseEvent & mouseEvent)
{
	m_mouseDispatcher->m_mouseEvent = mouseEvent;
}

void aui_Region::MouseNoChange(aui_MouseEvent * mouseData)
{
	if (IsDisabled()) {
		return;
	}
	if (m_mouseDispatcher->m_isMouseInside && !GetWhichSeesMouse()) {
		SetWhichSeesMouse(this );
	}
}
