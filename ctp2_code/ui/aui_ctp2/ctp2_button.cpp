#include "c3.h"
#include "ctp2_button.h"

#include <string>

#include "aui_imagelist.h"
#include "aui_ldl.h"
#include "aui_window.h"
#include "c3ui.h"

#include "AttractWindow.h"

#include "colorset.h"           // g_colorSet
#include "primitives.h"

#include "ldl_data.hpp"

extern C3UI	*g_c3ui;

static const MBCHAR *k_CTP2_BUTTON_LDL_LAYER_UP			= "layerup";
static const MBCHAR *k_CTP2_BUTTON_LDL_LAYER_DOWN		= "layerdown";
static const MBCHAR *k_CTP2_BUTTON_LDL_LAYER_UP_ON      = "layerupon";

const sint32 ctp2_Button::k_CTP2_BUTTON_LAYER_FLAG_UP			= 0x10;
const sint32 ctp2_Button::k_CTP2_BUTTON_LAYER_FLAG_DOWN			= 0x20;
const sint32 ctp2_Button::k_CTP2_BUTTON_LAYER_FLAG_UP_ON        = 0x40;

#define k_CTP2_BUTTON_ATTRACT_MS 250

ctp2_Button::~ctp2_Button()
{
}










AUI_ERRCODE ctp2_Button::InitCommonLdl(MBCHAR *ldlBlock, MBCHAR *ldlTemplate,
									   sint32 x, sint32 y,
									   sint32 width, sint32 height)
{
    ldl_datablock * theBlock = aui_Ldl::FindDataBlock(ldlBlock);

    if (!theBlock)
	{
        theBlock = aui_Ldl::FindDataBlock(ldlTemplate);
		if (!theBlock)
        {
            return AUI_ERRCODE_HACK;
		}
	}

	m_isToggle = theBlock->GetBool("togglebutton") ? true : false;
	if(m_isToggle) {
		m_toggleState = false;
	}

	for(sint32 layerIndex = 0; layerIndex < GetNumberOfLayers(); layerIndex++) {




		char layerIndexString[20];
		sprintf(layerIndexString, "%d", layerIndex);

		InitializeLayerFlag(theBlock, layerIndex, k_CTP2_BUTTON_LDL_LAYER_UP,
							k_CTP2_BUTTON_LAYER_FLAG_UP, layerIndexString);

		InitializeLayerFlag(theBlock, layerIndex, k_CTP2_BUTTON_LDL_LAYER_DOWN,
							k_CTP2_BUTTON_LAYER_FLAG_DOWN, layerIndexString);

		if(m_isToggle) {
			InitializeLayerFlag(theBlock, layerIndex, k_CTP2_BUTTON_LDL_LAYER_UP_ON,
								k_CTP2_BUTTON_LAYER_FLAG_UP_ON, layerIndexString);
		}
	}

	m_ignoreHighlight = theBlock->GetBool("nohighlight") ? true : false;
	if(m_ignoreHighlight) {
		m_drawMask &= ~(k_AUI_REGION_DRAWFLAG_MOUSEMOVEOVER | k_AUI_REGION_DRAWFLAG_MOUSEMOVEAWAY);
	}

	m_isDropdownButton = theBlock->GetBool("dropdownbutton") ? true : false;
	if(m_isDropdownButton) {

		m_drawMask = k_AUI_REGION_DRAWFLAG_UPDATE;
	}


	if(!IsDisabled()) {
		if(!m_isToggle || !m_toggleState) {
			AddRenderFlags(k_CTP2_BUTTON_LAYER_FLAG_UP);
			RemoveRenderFlags(k_CTP2_BUTTON_LAYER_FLAG_UP_ON);
		} else {
			AddRenderFlags(k_CTP2_BUTTON_LAYER_FLAG_UP_ON);
			RemoveRenderFlags(k_CTP2_BUTTON_LAYER_FLAG_UP);
		}
	}

	m_attract = false;

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE ctp2_Button::DrawThis(aui_Surface *surface,
								  sint32 x,
								  sint32 y)
{

	if(IsHidden())
		return AUI_ERRCODE_OK;

	if(!surface)
		surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	DrawLayers(surface, &rect);

	DrawThisText(surface, &rect);


	if(surface == m_window->TheSurface())
		m_window->AddDirtyRect(&rect);

	if(m_window->GetFocusControl() == this) {
		primitives_FrameThickRect16(surface, &rect, g_colorSet->GetColor(COLOR_UI_BOX), 3);
	}

	return AUI_ERRCODE_OK;
}

void ctp2_Button::MouseLGrabInside(aui_MouseEvent *mouseData)
{

	RenderDown();

	aui_Button::MouseLGrabInside(mouseData);
}

void ctp2_Button::MouseLDropInside(aui_MouseEvent *mouseData)
{

	RenderDown(false);

	aui_Button::MouseLDropInside(mouseData);
}

void ctp2_Button::MouseLDragOver(aui_MouseEvent *mouseData)
{

	if(GetMouseOwnership() == this) {

		RenderDown();
	}

	aui_Button::MouseLDragOver(mouseData);
}

void ctp2_Button::MouseLDragAway(aui_MouseEvent *mouseData)
{

	if(GetMouseOwnership() == this) {

		RenderDown(false);
	}

	aui_Button::MouseLDragAway(mouseData);
}

void ctp2_Button::ResetCurrentRenderFlags() {

	aui_Button::ResetCurrentRenderFlags();


	if(!IsDisabled())
		if(!m_isToggle || !m_toggleState) {
			AddRenderFlags(k_CTP2_BUTTON_LAYER_FLAG_UP);
			RemoveRenderFlags(k_CTP2_BUTTON_LAYER_FLAG_UP_ON);
		} else {
			AddRenderFlags(k_CTP2_BUTTON_LAYER_FLAG_UP_ON);
			RemoveRenderFlags(k_CTP2_BUTTON_LAYER_FLAG_UP);
		}
}

void ctp2_Button::SetAttract(bool on, uint32 ticksSinceStart)
{
	m_attract = on;
	m_attractTicks = ticksSinceStart;

	if(m_attract) {
		if((m_attractTicks / k_CTP2_BUTTON_ATTRACT_MS) & 1) {
			Highlight(true);
		} else {
			Highlight(false);
		}
	} else {
		Highlight(false);
		g_attractWindow->RemoveRegion(this);
	}
}

void ctp2_Button::RenderDown(bool status)
{

	if(m_isDropdownButton)
		return;

	if(IsDisabled())
		RemoveRenderFlags(k_CTP2_BUTTON_LAYER_FLAG_UP |
						  k_CTP2_BUTTON_LAYER_FLAG_DOWN |
						  k_CTP2_BUTTON_LAYER_FLAG_UP_ON);
	else {

		if(status) {
			AddRenderFlags(k_CTP2_BUTTON_LAYER_FLAG_DOWN);
			RemoveRenderFlags(k_CTP2_BUTTON_LAYER_FLAG_UP | k_CTP2_BUTTON_LAYER_FLAG_UP_ON);
		} else {
			if(!m_isToggle || !m_toggleState) {
				AddRenderFlags(k_CTP2_BUTTON_LAYER_FLAG_UP);
				RemoveRenderFlags(k_CTP2_BUTTON_LAYER_FLAG_UP_ON);
			} else {
				AddRenderFlags(k_CTP2_BUTTON_LAYER_FLAG_UP_ON);
				RemoveRenderFlags(k_CTP2_BUTTON_LAYER_FLAG_UP);
			}
			RemoveRenderFlags(k_CTP2_BUTTON_LAYER_FLAG_DOWN);
		}
	}

	ShouldDraw();
}

void ctp2_Button::SetToggleState(bool state)
{
	m_toggleState = state;
	RenderDown(GetMouseOwnership() == this);
}
