#include "c3.h"

#include "aui_region.h"

#include "SelItem.h"

extern SelectedItem		*g_selected_item;




#define k_DOUBLE_CLICK_TRAVEL_DISTANCE		5

#define LEGALTRAVELDISTANCE					(	(xDistance < k_DOUBLE_CLICK_TRAVEL_DISTANCE) && \
												(yDistance < k_DOUBLE_CLICK_TRAVEL_DISTANCE))





void aui_Region::MouseDispatch( aui_MouseEvent *input, BOOL handleIt )
{

	if ( !m_noChange )
		m_noChangeTime = input->time;
	else
		m_noChange = FALSE;

	sint32 xDistance = abs(input->position.x - m_doubleClickOldPos.x);
	sint32 yDistance = abs(input->position.y - m_doubleClickOldPos.y);

	if ( input->position.x != m_mouseState.position.x
	||   input->position.y != m_mouseState.position.y
	||   m_x != m_xLastTime
	||   m_y != m_yLastTime )
	{

		BOOL wasMouseInside = m_isMouseInside;

		m_isMouseInside = IsInside( &input->position );


		if ( handleIt )

		if ( m_isMouseInside )
		{
			if ( wasMouseInside )
			{
				if ( input->lbutton )
				{
					if ( m_mouseState.lbutton )
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragInside( input );
								MouseRDragInside( input );
							}
							else
							{
								MouseLDragInside( input );
								if (m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickInside( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabInside( input );
								}
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragInside( input );
								MouseRDragInside( input );
								MouseRDropInside( input );
							}
							else
							{
								MouseLDragInside( input );
							}
						}
					}
					else
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragInside( input );
								if (LEGALTRAVELDISTANCE && m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickInside( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleLClickStartWaitTime = input->time;
									m_doubleClickOldPos = input->position;
									MouseLGrabInside( input );
								}
							}
							else
							{
								MouseMoveInside( input );
								if (LEGALTRAVELDISTANCE && m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickInside( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleLClickStartWaitTime = input->time;
									m_doubleClickOldPos = input->position;
									MouseLGrabInside( input );
								}
								if (m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickInside( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabInside( input );
								}
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragInside( input );
								if (LEGALTRAVELDISTANCE && m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickInside( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleLClickStartWaitTime = input->time;
									m_doubleClickOldPos = input->position;
									MouseLGrabInside( input );
								}
								MouseRDropInside( input );
							}
							else
							{
								MouseMoveInside( input );
								if (LEGALTRAVELDISTANCE && m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickInside( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleLClickStartWaitTime = input->time;
									m_doubleClickOldPos = input->position;
									MouseLGrabInside( input );
								}
							}
						}
					}
				}
				else
				{
					if ( m_mouseState.lbutton )
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								if (g_selected_item)
									g_selected_item->RegisterUIClick();

								MouseLDragInside( input );
								MouseRDragInside( input );
								MouseLDropInside( input );
							}
							else
							{
								if (g_selected_item)
									g_selected_item->RegisterUIClick();

								MouseLDragInside( input );
								if (m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickInside( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabInside( input );
								}
								MouseLDropInside( input );
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								if (g_selected_item)
									g_selected_item->RegisterUIClick();

								MouseLDragInside( input );
								MouseRDragInside( input );
								MouseLDropInside( input );
								MouseRDropInside( input );
							}
							else
							{
								if (g_selected_item)
									g_selected_item->RegisterUIClick();

								MouseLDragInside( input );
								MouseLDropInside( input );
							}
						}
					}
					else
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragInside( input );
							}
							else
							{
								MouseMoveInside( input );
								if (m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickInside( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabInside( input );
								}
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragInside( input );
								MouseRDropInside( input );
							}
							else
							{
								MouseMoveInside( input );
							}
						}
					}
				}
			}
			else
			{
				if ( input->lbutton )
				{
					if ( m_mouseState.lbutton )
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragOver( input );
								MouseRDragOver( input );
							}
							else
							{
								MouseLDragOver( input );
								if (m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickInside( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabInside( input );
								}
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragOver( input );
								MouseRDragOver( input );
								MouseRDropInside( input );
							}
							else
							{
								MouseLDragOver( input );
							}
						}
					}
					else
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragOver( input );
								if (LEGALTRAVELDISTANCE && m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickInside( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleLClickStartWaitTime = input->time;
									m_doubleClickOldPos = input->position;
									MouseLGrabInside( input );
								}
							}
							else
							{
								MouseMoveOver( input );
								if (LEGALTRAVELDISTANCE && m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickInside( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleLClickStartWaitTime = input->time;
									m_doubleClickOldPos = input->position;
									MouseLGrabInside( input );
								}
								if (m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickInside( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabInside( input );
								}
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragOver( input );
								if (LEGALTRAVELDISTANCE && m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickInside( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleLClickStartWaitTime = input->time;
									m_doubleClickOldPos = input->position;
									MouseLGrabInside( input );
								}
								MouseRDropInside( input );
							}
							else
							{
								MouseMoveOver( input );
								if (LEGALTRAVELDISTANCE && m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickInside( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleLClickStartWaitTime = input->time;
									m_doubleClickOldPos = input->position;
									MouseLGrabInside( input );
								}
							}
						}
					}
				}
				else
				{
					if ( m_mouseState.lbutton )
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								if (g_selected_item)
									g_selected_item->RegisterUIClick();
								MouseLDragOver( input );
								MouseRDragOver( input );
								MouseLDropInside( input );
							}
							else
							{
								if (g_selected_item)
									g_selected_item->RegisterUIClick();
								MouseLDragOver( input );
								if (m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickInside( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabInside( input );
								}
								MouseLDropInside( input );
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								if (g_selected_item)
									g_selected_item->RegisterUIClick();
								MouseLDragOver( input );
								MouseRDragOver( input );
								MouseLDropInside( input );
								MouseRDropInside( input );
							}
							else
							{
								if (g_selected_item)
									g_selected_item->RegisterUIClick();
								MouseLDragOver( input );
								MouseLDropInside( input );
							}
						}
					}
					else
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragOver( input );
							}
							else
							{
								MouseMoveOver( input );
								if (m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickInside( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabInside( input );
								}
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragOver( input );
								MouseRDropInside( input );
							}
							else
							{
								MouseMoveOver( input );
							}
						}
					}
				}
			}
		}
		else
		{
			if ( wasMouseInside )
			{
				if ( input->lbutton )
				{
					if ( m_mouseState.lbutton )
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragAway( input );
								MouseRDragAway( input );
							}
							else
							{
								MouseLDragAway( input );
								if ( !m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickOutside( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabOutside( input );
								}
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragAway( input );
								MouseRDragAway( input );
								MouseRDropOutside( input );
							}
							else
							{
								MouseLDragAway( input );
							}
						}
					}
					else
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragAway( input );
								if ( !m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickOutside( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleLClickStartWaitTime = input->time;
									m_doubleClickOldPos = input->position;
									MouseLGrabOutside( input );
								}
							}
							else
							{
								MouseMoveAway( input );
								if ( !m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickOutside( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleLClickStartWaitTime = input->time;
									m_doubleClickOldPos = input->position;
									MouseLGrabOutside( input );
								}
								if ( !m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickOutside( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabOutside( input );
								}
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragAway( input );
								if ( !m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickOutside( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleLClickStartWaitTime = input->time;
									m_doubleClickOldPos = input->position;
									MouseLGrabOutside( input );
								}
								MouseRDropOutside( input );
							}
							else
							{
								MouseMoveAway( input );
								if ( !m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickOutside( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleLClickStartWaitTime = input->time;
									m_doubleClickOldPos = input->position;
									MouseLGrabOutside( input );
								}
							}
						}
					}
				}
				else
				{
					if ( m_mouseState.lbutton )
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragAway( input );
								MouseRDragAway( input );
								MouseLDropOutside( input );
							}
							else
							{
								MouseLDragAway( input );
								if ( !m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickOutside( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabOutside( input );
								}
								MouseLDropOutside( input );
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragAway( input );
								MouseRDragAway( input );
								MouseLDropOutside( input );
								MouseRDropOutside( input );
							}
							else
							{
								MouseLDragAway( input );
								MouseLDropOutside( input );
							}
						}
					}
					else
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragAway( input );
							}
							else
							{
								MouseMoveAway( input );
								if ( !m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickOutside( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabOutside( input );
								}
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragAway( input );
								MouseRDropOutside( input );
							}
							else
							{
								MouseMoveAway( input );
							}
						}
					}
				}
			}
			else
			{
				if ( input->lbutton )
				{
					if ( m_mouseState.lbutton )
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragOutside( input );
								MouseRDragOutside( input );
							}
							else
							{
								MouseLDragOutside( input );
								if ( !m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickOutside( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabOutside( input );
								}
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragOutside( input );
								MouseRDragOutside( input );
								MouseRDropOutside( input );
							}
							else
							{
								MouseLDragOutside( input );
							}
						}
					}
					else
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragOutside( input );
								if ( !m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickOutside( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleLClickStartWaitTime = input->time;
									m_doubleClickOldPos = input->position;
									MouseLGrabOutside( input );
								}
							}
							else
							{
								MouseMoveOutside( input );
								if ( !m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickOutside( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleLClickStartWaitTime = input->time;
									m_doubleClickOldPos = input->position;
									MouseLGrabOutside( input );
								}
								if ( !m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickOutside( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabOutside( input );
								}
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragOutside( input );
								if ( !m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickOutside( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleLClickStartWaitTime = input->time;
									m_doubleClickOldPos = input->position;
									MouseLGrabOutside( input );
								}
								MouseRDropOutside( input );
							}
							else
							{
								MouseMoveOutside( input );
								if ( !m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickOutside( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleLClickStartWaitTime = input->time;
									m_doubleClickOldPos = input->position;
									MouseLGrabOutside( input );
								}
							}
						}
					}
				}
				else
				{
					if ( m_mouseState.lbutton )
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragOutside( input );
								MouseRDragOutside( input );
								MouseLDropOutside( input );
							}
							else
							{
								MouseLDragOutside( input );
								if ( !m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickOutside( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabOutside( input );
								}
								MouseLDropOutside( input );
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragOutside( input );
								MouseRDragOutside( input );
								MouseLDropOutside( input );
								MouseRDropOutside( input );
							}
							else
							{
								MouseLDragOutside( input );
								MouseLDropOutside( input );
							}
						}
					}
					else
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragOutside( input );
							}
							else
							{
								MouseMoveOutside( input );
								if ( !m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickOutside( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabOutside( input );
								}
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragOutside( input );
								MouseRDropOutside( input );
							}
							else
							{
								MouseMoveOutside( input );
							}
						}
					}
				}
			}
		}
	}


	else
	{

		if ( handleIt )

		if ( m_isMouseInside )
		{
			if ( input->lbutton )
			{
				if ( m_mouseState.lbutton )
				{
					if ( input->rbutton )
					{
						if ( m_mouseState.rbutton )
						{
							m_noChange = TRUE;
							MouseNoChange( input );
						}
						else
						{
							if (m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseRDoubleClickInside( input );
							}
							else
							{
								m_doubleClickingInside = TRUE;
								m_doubleRClickStartWaitTime = input->time;
								MouseRGrabInside( input );
							}
						}
					}
					else
					{
						if ( m_mouseState.rbutton )
						{
							MouseRDropInside( input );
						}
						else
						{
							m_noChange = TRUE;
							MouseNoChange( input );
						}
					}
				}
				else
				{
					if ( input->rbutton )
					{
						if ( m_mouseState.rbutton )
						{
							if (LEGALTRAVELDISTANCE && m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseLDoubleClickInside( input );
							}
							else
							{
								m_doubleClickingInside = TRUE;
								m_doubleLClickStartWaitTime = input->time;
								m_doubleClickOldPos = input->position;
								MouseLGrabInside( input );
							}
						}
						else
						{
							if (LEGALTRAVELDISTANCE && m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseLDoubleClickInside( input );
							}
							else
							{
								m_doubleClickingInside = TRUE;
								m_doubleLClickStartWaitTime = input->time;
								m_doubleClickOldPos = input->position;
								MouseLGrabInside( input );
							}
							if (m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseRDoubleClickInside( input );
							}
							else
							{
								m_doubleClickingInside = TRUE;
								m_doubleRClickStartWaitTime = input->time;
								MouseRGrabInside( input );
							}
						}
					}
					else
					{
						if ( m_mouseState.rbutton )
						{
							if (LEGALTRAVELDISTANCE && m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseLDoubleClickInside( input );
							}
							else
							{
								m_doubleClickingInside = TRUE;
								m_doubleLClickStartWaitTime = input->time;
								m_doubleClickOldPos = input->position;
								MouseLGrabInside( input );
							}
							MouseRDropInside( input );
						}
						else
						{
							if (LEGALTRAVELDISTANCE && m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseLDoubleClickInside( input );
							}
							else
							{
								m_doubleClickingInside = TRUE;
								m_doubleLClickStartWaitTime = input->time;
								m_doubleClickOldPos = input->position;
								MouseLGrabInside( input );
							}
						}
					}
				}
			}
			else
			{
				if ( m_mouseState.lbutton )
				{
					if ( input->rbutton )
					{
						if ( m_mouseState.rbutton )
						{
							if (g_selected_item)
								g_selected_item->RegisterUIClick();
							MouseLDropInside( input );
						}
						else
						{
							if (g_selected_item)
								g_selected_item->RegisterUIClick();
							if (m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseRDoubleClickInside( input );
							}
							else
							{
								m_doubleClickingInside = TRUE;
								m_doubleRClickStartWaitTime = input->time;
								MouseRGrabInside( input );
							}
							MouseLDropInside( input );
						}
					}
					else
					{
						if ( m_mouseState.rbutton )
						{
							if (g_selected_item)
								g_selected_item->RegisterUIClick();
							MouseLDropInside( input );
							MouseRDropInside( input );
						}
						else
						{
							if (g_selected_item)
								g_selected_item->RegisterUIClick();
							MouseLDropInside( input );
						}
					}
				}
				else
				{
					if ( input->rbutton )
					{
						if ( m_mouseState.rbutton )
						{
							m_noChange = TRUE;
							MouseNoChange( input );
						}
						else
						{
							if (m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseRDoubleClickInside( input );
							}
							else
							{
								m_doubleClickingInside = TRUE;
								m_doubleRClickStartWaitTime = input->time;
								MouseRGrabInside( input );
							}
						}
					}
					else
					{
						if ( m_mouseState.rbutton )
						{
							MouseRDropInside( input );
						}
						else
						{
							m_noChange = TRUE;
							MouseNoChange( input );
						}
					}
				}
			}
		}
		else
		{
			if ( input->lbutton )
			{
				if ( m_mouseState.lbutton )
				{
					if ( input->rbutton )
					{
						if ( m_mouseState.rbutton )
						{
							m_noChange = TRUE;
							MouseNoChange( input );
						}
						else
						{
							if ( !m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseRDoubleClickOutside( input );
							}
							else
							{
								m_doubleClickingInside = FALSE;
								m_doubleRClickStartWaitTime = input->time;
								MouseRGrabOutside( input );
							}
						}
					}
					else
					{
						if ( m_mouseState.rbutton )
						{
							MouseRDropOutside( input );
						}
						else
						{
							m_noChange = TRUE;
							MouseNoChange( input );
						}
					}
				}
				else
				{
					if ( input->rbutton )
					{
						if ( m_mouseState.rbutton )
						{
							if ( !m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseLDoubleClickOutside( input );
							}
							else
							{
								m_doubleClickingInside = FALSE;
								m_doubleLClickStartWaitTime = input->time;
								m_doubleClickOldPos = input->position;
								MouseLGrabOutside( input );
							}
						}
						else
						{
							if ( !m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseLDoubleClickOutside( input );
							}
							else
							{
								m_doubleClickingInside = FALSE;
								m_doubleClickOldPos = input->position;
								m_doubleLClickStartWaitTime = input->time;
								MouseLGrabOutside( input );
							}
							if ( !m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseRDoubleClickOutside( input );
							}
							else
							{
								m_doubleClickingInside = FALSE;
								m_doubleRClickStartWaitTime = input->time;
								MouseRGrabOutside( input );
							}
						}
					}
					else
					{
						if ( m_mouseState.rbutton )
						{
							if ( !m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseLDoubleClickOutside( input );
							}
							else
							{
								m_doubleClickingInside = FALSE;
								m_doubleClickOldPos = input->position;
								m_doubleLClickStartWaitTime = input->time;
								MouseLGrabOutside( input );
							}
							MouseRDropOutside( input );
						}
						else
						{
							if ( !m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseLDoubleClickOutside( input );
							}
							else
							{
								m_doubleClickingInside = FALSE;
								m_doubleClickOldPos = input->position;
								m_doubleLClickStartWaitTime = input->time;
								MouseLGrabOutside( input );
							}
						}
					}
				}
			}
			else
			{
				if ( m_mouseState.lbutton )
				{
					if ( input->rbutton )
					{
						if ( m_mouseState.rbutton )
						{
							MouseLDropOutside( input );
						}
						else
						{
							if ( !m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseRDoubleClickOutside( input );
							}
							else
							{
								m_doubleClickingInside = FALSE;
								m_doubleRClickStartWaitTime = input->time;
								MouseRGrabOutside( input );
							}
							MouseLDropOutside( input );
						}
					}
					else
					{
						if ( m_mouseState.rbutton )
						{
							MouseLDropOutside( input );
							MouseRDropOutside( input );
						}
						else
						{
							MouseLDropOutside( input );
						}
					}
				}
				else
				{
					if ( input->rbutton )
					{
						if ( m_mouseState.rbutton )
						{
							m_noChange = TRUE;
							MouseNoChange( input );
						}
						else
						{
							if ( !m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseRDoubleClickOutside( input );
							}
							else
							{
								m_doubleClickingInside = FALSE;
								m_doubleRClickStartWaitTime = input->time;
								MouseRGrabOutside( input );
							}
						}
					}
					else
					{
						if ( m_mouseState.rbutton )
						{
							MouseRDropOutside( input );
						}
						else
						{
							m_noChange = TRUE;
							MouseNoChange( input );
						}
					}
				}
			}
		}
	}
}

void aui_Region::MouseDispatchEdit( aui_MouseEvent *input, BOOL handleIt )
{

	if ( input->position.x != m_mouseState.position.x
	||   input->position.y != m_mouseState.position.y
	||   m_x != m_xLastTime
	||   m_y != m_yLastTime )
	{

		BOOL wasMouseInside = m_isMouseInside;

		m_isMouseInside = IsInside( &input->position );

		if ( handleIt )

		if ( m_isMouseInside )
		{
			if ( wasMouseInside )
			{
				if ( input->lbutton )
				{
					if ( m_mouseState.lbutton )
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragInsideEdit( input );
								MouseRDragInsideEdit( input );
							}
							else
							{
								MouseLDragInsideEdit( input );
								if ( m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickInsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabInsideEdit( input );
								}
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragInsideEdit( input );
								MouseRDragInsideEdit( input );
								MouseRDropInsideEdit( input );
							}
							else
							{
								MouseLDragInsideEdit( input );
							}
						}
					}
					else
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragInsideEdit( input );
								if ( m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickInsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleLClickStartWaitTime = input->time;
									MouseLGrabInsideEdit( input );
								}
							}
							else
							{
								MouseMoveInsideEdit( input );
								if ( m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickInsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleLClickStartWaitTime = input->time;
									MouseLGrabInsideEdit( input );
								}
								if ( m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickInsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabInsideEdit( input );
								}
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragInsideEdit( input );
								if ( m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickInsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleLClickStartWaitTime = input->time;
									MouseLGrabInsideEdit( input );
								}
								MouseRDropInsideEdit( input );
							}
							else
							{
								MouseMoveInsideEdit( input );
								if ( m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickInsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleLClickStartWaitTime = input->time;
									MouseLGrabInsideEdit( input );
								}
							}
						}
					}
				}
				else
				{
					if ( m_mouseState.lbutton )
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragInsideEdit( input );
								MouseRDragInsideEdit( input );
								MouseLDropInsideEdit( input );
							}
							else
							{
								MouseLDragInsideEdit( input );
								if ( m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickInsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabInsideEdit( input );
								}
								MouseLDropInsideEdit( input );
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragInsideEdit( input );
								MouseRDragInsideEdit( input );
								MouseLDropInsideEdit( input );
								MouseRDropInsideEdit( input );
							}
							else
							{
								MouseLDragInsideEdit( input );
								MouseLDropInsideEdit( input );
							}
						}
					}
					else
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragInsideEdit( input );
							}
							else
							{
								MouseMoveInsideEdit( input );
								if ( m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickInsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabInsideEdit( input );
								}
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragInsideEdit( input );
								MouseRDropInsideEdit( input );
							}
							else
							{
								MouseMoveInsideEdit( input );
							}
						}
					}
				}
			}
			else
			{
				if ( input->lbutton )
				{
					if ( m_mouseState.lbutton )
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragOverEdit( input );
								MouseRDragOverEdit( input );
							}
							else
							{
								MouseLDragOverEdit( input );
								if ( m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickInsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabInsideEdit( input );
								}
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragOverEdit( input );
								MouseRDragOverEdit( input );
								MouseRDropInsideEdit( input );
							}
							else
							{
								MouseLDragOverEdit( input );
							}
						}
					}
					else
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragOverEdit( input );
								if ( m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickInsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleLClickStartWaitTime = input->time;
									MouseLGrabInsideEdit( input );
								}
							}
							else
							{
								MouseMoveOverEdit( input );
								if ( m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickInsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleLClickStartWaitTime = input->time;
									MouseLGrabInsideEdit( input );
								}
								if ( m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickInsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabInsideEdit( input );
								}
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragOverEdit( input );
								if ( m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickInsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleLClickStartWaitTime = input->time;
									MouseLGrabInsideEdit( input );
								}
								MouseRDropInsideEdit( input );
							}
							else
							{
								MouseMoveOverEdit( input );
								if ( m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickInsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleLClickStartWaitTime = input->time;
									MouseLGrabInsideEdit( input );
								}
							}
						}
					}
				}
				else
				{
					if ( m_mouseState.lbutton )
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragOverEdit( input );
								MouseRDragOverEdit( input );
								MouseLDropInsideEdit( input );
							}
							else
							{
								MouseLDragOverEdit( input );
								if ( m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickInsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabInsideEdit( input );
								}
								MouseLDropInsideEdit( input );
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragOverEdit( input );
								MouseRDragOverEdit( input );
								MouseLDropInsideEdit( input );
								MouseRDropInsideEdit( input );
							}
							else
							{
								MouseLDragOverEdit( input );
								MouseLDropInsideEdit( input );
							}
						}
					}
					else
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragOverEdit( input );
							}
							else
							{
								MouseMoveOverEdit( input );
								if ( m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickInsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = TRUE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabInsideEdit( input );
								}
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragOverEdit( input );
								MouseRDropInsideEdit( input );
							}
							else
							{
								MouseMoveOverEdit( input );
							}
						}
					}
				}
			}
		}
		else
		{
			if ( wasMouseInside )
			{
				if ( input->lbutton )
				{
					if ( m_mouseState.lbutton )
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragAwayEdit( input );
								MouseRDragAwayEdit( input );
							}
							else
							{
								MouseLDragAwayEdit( input );
								if ( !m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickOutsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabOutsideEdit( input );
								}
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragAwayEdit( input );
								MouseRDragAwayEdit( input );
								MouseRDropOutsideEdit( input );
							}
							else
							{
								MouseLDragAwayEdit( input );
							}
						}
					}
					else
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragAwayEdit( input );
								if ( !m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickOutsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleLClickStartWaitTime = input->time;
									MouseLGrabOutsideEdit( input );
								}
							}
							else
							{
								MouseMoveAwayEdit( input );
								if ( !m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickOutsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleLClickStartWaitTime = input->time;
									MouseLGrabOutsideEdit( input );
								}
								if ( !m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickOutsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabOutsideEdit( input );
								}
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragAwayEdit( input );
								if ( !m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickOutsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleLClickStartWaitTime = input->time;
									MouseLGrabOutsideEdit( input );
								}
								MouseRDropOutsideEdit( input );
							}
							else
							{
								MouseMoveAwayEdit( input );
								if ( !m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickOutsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleLClickStartWaitTime = input->time;
									MouseLGrabOutsideEdit( input );
								}
							}
						}
					}
				}
				else
				{
					if ( m_mouseState.lbutton )
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragAwayEdit( input );
								MouseRDragAwayEdit( input );
								MouseLDropOutsideEdit( input );
							}
							else
							{
								MouseLDragAwayEdit( input );
								if ( !m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickOutsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabOutsideEdit( input );
								}
								MouseLDropOutsideEdit( input );
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragAwayEdit( input );
								MouseRDragAwayEdit( input );
								MouseLDropOutsideEdit( input );
								MouseRDropOutsideEdit( input );
							}
							else
							{
								MouseLDragAwayEdit( input );
								MouseLDropOutsideEdit( input );
							}
						}
					}
					else
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragAwayEdit( input );
							}
							else
							{
								MouseMoveAwayEdit( input );
								if ( !m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickOutsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabOutsideEdit( input );
								}
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragAwayEdit( input );
								MouseRDropOutsideEdit( input );
							}
							else
							{
								MouseMoveAwayEdit( input );
							}
						}
					}
				}
			}
			else
			{
				if ( input->lbutton )
				{
					if ( m_mouseState.lbutton )
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragOutsideEdit( input );
								MouseRDragOutsideEdit( input );
							}
							else
							{
								MouseLDragOutsideEdit( input );
								if ( !m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickOutsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabOutsideEdit( input );
								}
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragOutsideEdit( input );
								MouseRDragOutsideEdit( input );
								MouseRDropOutsideEdit( input );
							}
							else
							{
								MouseLDragOutsideEdit( input );
							}
						}
					}
					else
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragOutsideEdit( input );
								if ( !m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickOutsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleLClickStartWaitTime = input->time;
									MouseLGrabOutsideEdit( input );
								}
							}
							else
							{
								MouseMoveOutsideEdit( input );
								if ( !m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickOutsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleLClickStartWaitTime = input->time;
									MouseLGrabOutsideEdit( input );
								}
								if ( !m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickOutsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabOutsideEdit( input );
								}
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragOutsideEdit( input );
								if ( !m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickOutsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleLClickStartWaitTime = input->time;
									MouseLGrabOutsideEdit( input );
								}
								MouseRDropOutsideEdit( input );
							}
							else
							{
								MouseMoveOutsideEdit( input );
								if ( !m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseLDoubleClickOutsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleLClickStartWaitTime = input->time;
									MouseLGrabOutsideEdit( input );
								}
							}
						}
					}
				}
				else
				{
					if ( m_mouseState.lbutton )
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragOutsideEdit( input );
								MouseRDragOutsideEdit( input );
								MouseLDropOutsideEdit( input );
							}
							else
							{
								MouseLDragOutsideEdit( input );
								if ( !m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickOutsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabOutsideEdit( input );
								}
								MouseLDropOutsideEdit( input );
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseLDragOutsideEdit( input );
								MouseRDragOutsideEdit( input );
								MouseLDropOutsideEdit( input );
								MouseRDropOutsideEdit( input );
							}
							else
							{
								MouseLDragOutsideEdit( input );
								MouseLDropOutsideEdit( input );
							}
						}
					}
					else
					{
						if ( input->rbutton )
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragOutsideEdit( input );
							}
							else
							{
								MouseMoveOutsideEdit( input );
								if ( !m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
								{
									MouseRDoubleClickOutsideEdit( input );
								}
								else
								{
									m_doubleClickingInside = FALSE;
									m_doubleRClickStartWaitTime = input->time;
									MouseRGrabOutsideEdit( input );
								}
							}
						}
						else
						{
							if ( m_mouseState.rbutton )
							{
								MouseRDragOutsideEdit( input );
								MouseRDropOutsideEdit( input );
							}
							else
							{
								MouseMoveOutsideEdit( input );
							}
						}
					}
				}
			}
		}
	}


	else
	{

		if ( handleIt )

		if ( m_isMouseInside )
		{
			if ( input->lbutton )
			{
				if ( m_mouseState.lbutton )
				{
					if ( input->rbutton )
					{
						if ( m_mouseState.rbutton )
						{
							m_noChange = TRUE;
							MouseNoChange( input );
						}
						else
						{
							if ( m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseRDoubleClickInsideEdit( input );
							}
							else
							{
								m_doubleClickingInside = TRUE;
								m_doubleRClickStartWaitTime = input->time;
								MouseRGrabInsideEdit( input );
							}
						}
					}
					else
					{
						if ( m_mouseState.rbutton )
						{
							MouseRDropInsideEdit( input );
						}
						else
						{
							m_noChange = TRUE;
							MouseNoChange( input );
						}
					}
				}
				else
				{
					if ( input->rbutton )
					{
						if ( m_mouseState.rbutton )
						{
							if ( m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseLDoubleClickInsideEdit( input );
							}
							else
							{
								m_doubleClickingInside = TRUE;
								m_doubleLClickStartWaitTime = input->time;
								MouseLGrabInsideEdit( input );
							}
						}
						else
						{
							if ( m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseLDoubleClickInsideEdit( input );
							}
							else
							{
								m_doubleClickingInside = TRUE;
								m_doubleLClickStartWaitTime = input->time;
								MouseLGrabInsideEdit( input );
							}
							if ( m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseRDoubleClickInsideEdit( input );
							}
							else
							{
								m_doubleClickingInside = TRUE;
								m_doubleRClickStartWaitTime = input->time;
								MouseRGrabInsideEdit( input );
							}
						}
					}
					else
					{
						if ( m_mouseState.rbutton )
						{
							if ( m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseLDoubleClickInsideEdit( input );
							}
							else
							{
								m_doubleClickingInside = TRUE;
								m_doubleLClickStartWaitTime = input->time;
								MouseLGrabInsideEdit( input );
							}
							MouseRDropInsideEdit( input );
						}
						else
						{
							if ( m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseLDoubleClickInsideEdit( input );
							}
							else
							{
								m_doubleClickingInside = TRUE;
								m_doubleLClickStartWaitTime = input->time;
								MouseLGrabInsideEdit( input );
							}
						}
					}
				}
			}
			else
			{
				if ( m_mouseState.lbutton )
				{
					if ( input->rbutton )
					{
						if ( m_mouseState.rbutton )
						{
							MouseLDropInsideEdit( input );
						}
						else
						{
							if ( m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseRDoubleClickInsideEdit( input );
							}
							else
							{
								m_doubleClickingInside = TRUE;
								m_doubleRClickStartWaitTime = input->time;
								MouseRGrabInsideEdit( input );
							}
							MouseLDropInsideEdit( input );
						}
					}
					else
					{
						if ( m_mouseState.rbutton )
						{
							MouseLDropInsideEdit( input );
							MouseRDropInsideEdit( input );
						}
						else
						{
							MouseLDropInsideEdit( input );
						}
					}
				}
				else
				{
					if ( input->rbutton )
					{
						if ( m_mouseState.rbutton )
						{
							m_noChange = TRUE;
							MouseNoChange( input );
						}
						else
						{
							if ( m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseRDoubleClickInsideEdit( input );
							}
							else
							{
								m_doubleClickingInside = TRUE;
								m_doubleRClickStartWaitTime = input->time;
								MouseRGrabInsideEdit( input );
							}
						}
					}
					else
					{
						if ( m_mouseState.rbutton )
						{
							MouseRDropInsideEdit( input );
						}
						else
						{
							m_noChange = TRUE;
							MouseNoChange( input );
						}
					}
				}
			}
		}
		else
		{
			if ( input->lbutton )
			{
				if ( m_mouseState.lbutton )
				{
					if ( input->rbutton )
					{
						if ( m_mouseState.rbutton )
						{
							m_noChange = TRUE;
							MouseNoChange( input );
						}
						else
						{
							if ( !m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseRDoubleClickOutsideEdit( input );
							}
							else
							{
								m_doubleClickingInside = FALSE;
								m_doubleRClickStartWaitTime = input->time;
								MouseRGrabOutsideEdit( input );
							}
						}
					}
					else
					{
						if ( m_mouseState.rbutton )
						{
							MouseRDropOutsideEdit( input );
						}
						else
						{
							m_noChange = TRUE;
							MouseNoChange( input );
						}
					}
				}
				else
				{
					if ( input->rbutton )
					{
						if ( m_mouseState.rbutton )
						{
							if ( !m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseLDoubleClickOutsideEdit( input );
							}
							else
							{
								m_doubleClickingInside = FALSE;
								m_doubleLClickStartWaitTime = input->time;
								MouseLGrabOutsideEdit( input );
							}
						}
						else
						{
							if ( !m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseLDoubleClickOutsideEdit( input );
							}
							else
							{
								m_doubleClickingInside = FALSE;
								m_doubleLClickStartWaitTime = input->time;
								MouseLGrabOutsideEdit( input );
							}
							if ( !m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseRDoubleClickOutsideEdit( input );
							}
							else
							{
								m_doubleClickingInside = FALSE;
								m_doubleRClickStartWaitTime = input->time;
								MouseRGrabOutsideEdit( input );
							}
						}
					}
					else
					{
						if ( m_mouseState.rbutton )
						{
							if ( !m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseLDoubleClickOutsideEdit( input );
							}
							else
							{
								m_doubleClickingInside = FALSE;
								m_doubleLClickStartWaitTime = input->time;
								MouseLGrabOutsideEdit( input );
							}
							MouseRDropOutsideEdit( input );
						}
						else
						{
							if ( !m_doubleClickingInside && input->time - m_doubleLClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseLDoubleClickOutsideEdit( input );
							}
							else
							{
								m_doubleClickingInside = FALSE;
								m_doubleLClickStartWaitTime = input->time;
								MouseLGrabOutsideEdit( input );
							}
						}
					}
				}
			}
			else
			{
				if ( m_mouseState.lbutton )
				{
					if ( input->rbutton )
					{
						if ( m_mouseState.rbutton )
						{
							MouseLDropOutsideEdit( input );
						}
						else
						{
							if ( !m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseRDoubleClickOutsideEdit( input );
							}
							else
							{
								m_doubleClickingInside = FALSE;
								m_doubleRClickStartWaitTime = input->time;
								MouseRGrabOutsideEdit( input );
							}
							MouseLDropOutsideEdit( input );
						}
					}
					else
					{
						if ( m_mouseState.rbutton )
						{
							MouseLDropOutsideEdit( input );
							MouseRDropOutsideEdit( input );
						}
						else
						{
							MouseLDropOutsideEdit( input );
						}
					}
				}
				else
				{
					if ( input->rbutton )
					{
						if ( m_mouseState.rbutton )
						{
							m_noChange = TRUE;
							MouseNoChange( input );
						}
						else
						{
							if ( !m_doubleClickingInside && input->time - m_doubleRClickStartWaitTime < m_doubleClickTimeOut )
							{
								MouseRDoubleClickOutsideEdit( input );
							}
							else
							{
								m_doubleClickingInside = FALSE;
								m_doubleRClickStartWaitTime = input->time;
								MouseRGrabOutsideEdit( input );
							}
						}
					}
					else
					{
						if ( m_mouseState.rbutton )
						{
							MouseRDropOutsideEdit( input );
						}
						else
						{
							m_noChange = TRUE;
							MouseNoChange( input );
						}
					}
				}
			}
		}
	}
}
