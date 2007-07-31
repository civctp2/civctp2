

#include "c3.h"

#include "c3errors.h"

#include "c3ui.h"

#include "aui_region.h"
#include "aui_control.h"
#include "aui_uniqueid.h"
#include "aui_ldl.h"

#include "pixelutils.h"
#include "primitives.h"

#include "ColorSet.h"

#include "AttractWindow.h"

extern C3UI			*g_c3ui;
extern ColorSet		*g_colorSet;

AttractWindow		*g_attractWindow;

#define k_ATTRACT_FINAL_STAGE		20
#define k_ATTRACT_DURATION			120000

void AttractWindow::Initialize(void)
{
	AUI_ERRCODE		retval;

	g_attractWindow = new AttractWindow(&retval, aui_UniqueId(), "AttractWindow", 16);
	Assert(g_attractWindow);
	if (!g_attractWindow)
		return;

	g_attractWindow->SetTransparent(TRUE);
}

void AttractWindow::Cleanup(void)
{

	if (g_attractWindow) {
		g_attractWindow->Hide();
		g_c3ui->RemoveWindow(g_attractWindow->Id());
		delete g_attractWindow;
		g_attractWindow = NULL;
	}
}

AttractWindow::AttractWindow(AUI_ERRCODE *retval, uint32 id, MBCHAR *ldlBlock, 
							 sint32 bpp, AUI_WINDOW_TYPE type)
							 :
C3Window(retval, id, ldlBlock, bpp, type)
{
	*retval = InitCommon();	
}

AttractWindow::~AttractWindow()
{
	m_regions.DeleteAll();
}

AUI_ERRCODE AttractWindow::InitCommon(void)
{
	m_attractStage = -1;
	m_finishTime = 0;
	
	

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE AttractWindow::DrawThis(aui_Surface *surface, sint32 x, sint32 y)
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if (!surface) 
		surface = TheSurface();

	DrawAttractiveStuff();

	RECT rect = {0, 0, Width(), Height()};

	
	m_dirtyList->AddRect( &rect );

	return AUI_ERRCODE_OK;
}






AUI_ERRCODE AttractWindow::Idle(void)
{
	if (m_attractStage != -1) {
		m_attractStage++;

		DrawThis();
		if (m_attractStage == k_ATTRACT_FINAL_STAGE) {
			if (GetTickCount() > m_finishTime) {
				m_attractStage = -1;
				Hide();
				g_c3ui->RemoveWindow(this->Id());
			} else {
				m_attractStage = 0;
			}
		}
	}

	return AUI_ERRCODE_OK;
}

void AttractWindow::AppIdle()
{
	if(m_regions.GetCount() > 0) {
		PointerList<AttractRegion>::Walker walk(&m_regions);
		for(; walk.IsValid(); walk.Next()) {
			AttractRegion *ar = walk.GetObj();
			ar->m_region->SetAttract(true, GetTickCount() - ar->m_startTime);
		}
	}




}




void AttractWindow::DrawAttractiveStuff(void)
{
	static RECT		lastRect;

	if (m_attractStage == -1) return;

	ClearWindow();

	sint32 x1, y1, x2, y2;

	x1 = (m_attractPoint.x)/k_ATTRACT_FINAL_STAGE * m_attractStage;
	y1 = (m_attractPoint.y)/k_ATTRACT_FINAL_STAGE * m_attractStage;
	x2 = Width() - (Width() - m_attractPoint.x)/k_ATTRACT_FINAL_STAGE * m_attractStage;
	y2 = Height() - (Height() - m_attractPoint.y)/k_ATTRACT_FINAL_STAGE * m_attractStage;

	RECT	attractRect = {x1, y1, x2, y2},
			tempRect;

	if (attractRect.left < 0) OffsetRect(&attractRect, -attractRect.left, 0);
	if (attractRect.top < 0) OffsetRect(&attractRect, 0, -attractRect.top);
	if (attractRect.right > Width()) OffsetRect(&attractRect, Width()-attractRect.right, 0);
	if (attractRect.bottom > Height()) OffsetRect(&attractRect, 0, Height() - attractRect.bottom);

	tempRect = attractRect;
	primitives_FrameRect16(m_surface, &tempRect, g_colorSet->GetColor(COLOR_GREEN));
	InflateRect(&tempRect, -1, -1);
	primitives_FrameRect16(m_surface, &tempRect, g_colorSet->GetColor(COLOR_DARK_GREEN));

	if (m_attractStage > 1) {
		tempRect = lastRect;
		primitives_FrameRect16(m_surface, &tempRect, g_colorSet->GetDarkColor(COLOR_GREEN));
		InflateRect(&tempRect, -1, -1);
		primitives_FrameRect16(m_surface, &tempRect, g_colorSet->GetColor(COLOR_GREEN));
	}
	lastRect = attractRect;
}




void AttractWindow::ClearWindow(void)
{
	RECT rect = {0,0,Width(),Height()};

	primitives_PaintRect16(m_surface, &rect, 0x0000);
}







void AttractWindow::HighlightControl(MBCHAR *ldlName)
{
	Assert(g_c3ui);
	if (!g_c3ui) return;
	Assert(g_c3ui->GetLdl());
	if (!g_c3ui->GetLdl()) return;

	
	aui_Region *region = (aui_Region *)g_c3ui->GetLdl()->GetObject(ldlName);

	Assert(region);
	if (!region) {
		c3errors_ErrorDialog("Attract", "Control %s not found by AttractWindow.", ldlName);
		return;
	}

	if(region->CanAttract()) {		
		RemoveRegion(region);
		AddRegion(region);
		return;
	}
	RECT rect = {region->X(), region->Y(), 
					region->X() + region->Width(),
					region->Y() + region->Height() };

	
	if ( region->GetParent() != g_c3ui )
		(( aui_Control *)region)->ToScreen( &rect );

	m_screenAttractRect = rect;

	
	sint32		x, y;

	x = m_screenAttractRect.left + (m_screenAttractRect.right - m_screenAttractRect.left)/2;
	y = m_screenAttractRect.top + (m_screenAttractRect.bottom - m_screenAttractRect.top)/2;
	
	m_attractPoint.x = x;
	m_attractPoint.y = y;

	x -= Width()/2;
	y -= Height()/2;

	
	if (x < 0) x = 0;
	
	if (y < 0) y = 0;
	if (x + Width() > g_c3ui->Width())
		x = g_c3ui->Width() - Width();

	if (y + Height() > g_c3ui->Height())
		y = g_c3ui->Height() - Height();

	
	Move(x, y);

	
	m_attractPoint.x -= X();
	m_attractPoint.y -= Y();

	m_attractStage = 0;

	
	ClearWindow();

	
	Show();

	g_c3ui->AddWindow(this);

	m_finishTime = GetTickCount() + k_ATTRACT_DURATION;
}

void AttractWindow::RemoveControl(MBCHAR *ldlName)
{
	aui_Region *region = (aui_Region *)g_c3ui->GetLdl()->GetObject(ldlName);
	if(region) {
		RemoveRegion(region);
	}
}

void AttractWindow::RemoveRegion(aui_Region *region)
{
	PointerList<AttractRegion>::Walker walk(&m_regions);
	while(walk.IsValid()) {
		if(walk.GetObj()->m_region == region) {
			delete walk.Remove();
		} else {
			walk.Next();
		}
	}
}

void AttractWindow::AddRegion(aui_Region *region)
{
	AttractRegion *ar = new AttractRegion;
	ar->m_region = region;
	ar->m_startTime = GetTickCount();
	m_regions.AddTail(ar);
}
