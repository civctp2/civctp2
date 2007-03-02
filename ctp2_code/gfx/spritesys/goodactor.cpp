//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Good actor
// Id           : $Id$
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
// _ACTOR_DRAW_OPTIMIZATION
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - None
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "GoodActor.h"

#include "aui.h"
#include "pixelutils.h"
#include "tileutils.h"
#include "Unit.h"

#include "SelItem.h"                // g_selected_item

#include "FacedSprite.h"
#include "GoodSpriteGroup.h"
#include "SpriteState.h"
#include "Actor.h"
#include "SpriteGroupList.h"
#include "tiledmap.h"               // g_tiledMap
#include "Anim.h"

#include "ActorPath.h"
#include "Action.h"
#include "director.h"               // g_director
#include "maputils.h"
#include "civarchive.h"

extern SpriteGroupList	*   g_goodSpriteGroupList;

namespace
{
    /// Default transparency value
    /// @todo Move to better location
    uint16 const        TRANSPARENCY_DEFAULT    = 15;
}

GoodActor::GoodActor(sint32 index, const MapPoint &pos)
:
    Actor               (),
    m_facing            (0),
    m_frame             (0),
    m_transparency      (TRANSPARENCY_DEFAULT),
    m_index             (index),
	m_pos               (pos),
    m_goodSpriteGroup   (NULL),
    m_curAction         (NULL),
    m_curGoodAction     (GOODACTION_IDLE),     
    m_actionQueue       (k_MAX_ACTION_QUEUE_SIZE),
    m_loadType          (LOADTYPE_BASIC)
{
	Assert(g_goodSpriteGroupList);
    if (g_goodSpriteGroupList)
    {
	    m_goodSpriteGroup = (GoodSpriteGroup *)
            g_goodSpriteGroupList->GetSprite(index, GROUPTYPE_GOOD, LOADTYPE_BASIC, (GAME_ACTION) 0);
    }

	AddIdle();
}

GoodActor::GoodActor(GoodActor const & a_Original)
:
    Actor               (a_Original),
    m_facing            (a_Original.m_facing),
    m_frame             (a_Original.m_frame),
    m_transparency      (a_Original.m_transparency),
    m_index             (a_Original.m_index),
	m_pos               (a_Original.m_pos),
    m_goodSpriteGroup   (NULL),
    m_curAction         (NULL),
    m_curGoodAction     (a_Original.m_curGoodAction),     
    m_actionQueue       (k_MAX_ACTION_QUEUE_SIZE),
    m_loadType          (LOADTYPE_BASIC)
{
    Assert(g_goodSpriteGroupList);

    if (a_Original.m_curAction)
    {
        m_curAction = new Action(*a_Original.m_curAction);
    }

    for (size_t i = 0; i < a_Original.m_actionQueue.GetNumItems(); ++i)
    {
        Action *    data = NULL;
        if (a_Original.m_actionQueue.GetQueueItem(i, data))
        {
            m_actionQueue.Enqueue(data ? new Action(*data) : NULL);
        }
    }

    if (g_goodSpriteGroupList)
    {
        m_loadType = a_Original.m_loadType;
	    m_goodSpriteGroup = (GoodSpriteGroup *)
            g_goodSpriteGroupList->GetSprite(m_index, GROUPTYPE_GOOD, m_loadType, (GAME_ACTION) 0);
    }
}

GoodActor & GoodActor::operator = (GoodActor const & a_Original)
{
    if (this != &a_Original)
    {
        Assert(g_goodSpriteGroupList);

        Actor::operator     = (a_Original);
        m_facing            = a_Original.m_facing;
        m_frame             = a_Original.m_frame;
        m_transparency      = a_Original.m_transparency;
        m_index             = a_Original.m_index;
	    m_pos               = a_Original.m_pos;
 
        if (g_goodSpriteGroupList)
        {
            if (m_goodSpriteGroup)
            {
	            g_goodSpriteGroupList->ReleaseSprite(m_index, GetLoadType());
            }

            m_loadType = a_Original.m_loadType;
	        m_goodSpriteGroup = (GoodSpriteGroup *)
                g_goodSpriteGroupList->GetSprite(m_index, GROUPTYPE_GOOD, m_loadType, (GAME_ACTION) 0);
        }
        else
        {
            m_goodSpriteGroup   = NULL;
            m_loadType          = LOADTYPE_BASIC;
        }

        delete m_curAction;
        m_curAction     = a_Original.m_curAction ? new Action(*a_Original.m_curAction) : NULL;
        m_curGoodAction = a_Original.m_curGoodAction;
        
      	while (m_actionQueue.GetNumItems() > 0) 
        {
            Action *    action  = NULL;
		    m_actionQueue.Dequeue(action);
		    delete action;
        }
        for (size_t i = 0; i < a_Original.m_actionQueue.GetNumItems(); ++i)
        {
            Action *    data = NULL;
            if (a_Original.m_actionQueue.GetQueueItem(i, data))
            {
                m_actionQueue.Enqueue(data ? new Action(*data) : NULL);
            }
        }
    }

    return *this;
}

/// @todo Replace with proper copy constructor
GoodActor::GoodActor(GoodActor *copy)
{
	*this = *copy;

    if (m_curAction)
    {
	    m_curAction = new Action(m_curAction);
    }

	m_goodSpriteGroup = (GoodSpriteGroup *)g_goodSpriteGroupList->GetSprite(m_index, GROUPTYPE_GOOD, GetLoadType(),(GAME_ACTION)0);

	m_actionQueue.CopyQueue();
}


GoodActor::GoodActor(CivArchive &archive)
:
    Actor               (),
    m_facing            (0),
    m_frame             (0),
    m_transparency      (TRANSPARENCY_DEFAULT),
    m_index             (0),
	m_pos               (),
    m_goodSpriteGroup   (NULL),
    m_curAction         (NULL),
    m_curGoodAction     (GOODACTION_IDLE),     
    m_actionQueue       (k_MAX_ACTION_QUEUE_SIZE),
    m_loadType          (LOADTYPE_BASIC)
{
	Serialize(archive);

	Assert(g_goodSpriteGroupList);
    if (g_goodSpriteGroupList)
    {
	    m_goodSpriteGroup = (GoodSpriteGroup *)
            g_goodSpriteGroupList->GetSprite(m_index, GROUPTYPE_GOOD, LOADTYPE_BASIC, (GAME_ACTION) 0);
    }

	AddIdle();
}

GoodActor::~GoodActor()
{
    delete m_curAction;

	while (m_actionQueue.GetNumItems() > 0) 
    {
        Action *    action  = NULL;
		m_actionQueue.Dequeue(action);
		delete action;
	}

    if (g_goodSpriteGroupList && m_goodSpriteGroup)
    {
	    g_goodSpriteGroupList->ReleaseSprite(m_index, GetLoadType());
    }
}

void GoodActor::FullLoad(void)
{ // Doesn't seem to do much
	if (!m_goodSpriteGroup) return;
	if (m_loadType == LOADTYPE_FULL) return;

	SpriteGroup *group = g_goodSpriteGroupList->GetSprite(m_index, GROUPTYPE_GOOD, LOADTYPE_FULL,(GAME_ACTION)0);
	Assert(group == m_goodSpriteGroup);
	
	m_loadType = LOADTYPE_FULL;
	m_frame = 0;
}

void GoodActor::DumpFullLoad(void)
{
	if (!m_goodSpriteGroup) return;
	if (m_loadType != LOADTYPE_FULL) return;

	bool purged = g_goodSpriteGroupList->ReleaseSprite(m_index, LOADTYPE_FULL);

	if (purged) {
		m_goodSpriteGroup = NULL;
	} else {
		m_loadType = LOADTYPE_BASIC;
	}

	m_frame = 0;
}


void GoodActor::PositionActor(MapPoint &pos)
{
	sint32 pixelX, pixelY;
	maputils_MapXY2PixelXY(pos.x, pos.y, &pixelX, &pixelY);
    Actor::SetPos(pixelX, pixelY);
	SetPos(pos);
}

void GoodActor::AddIdle(void)
{
	if (m_curAction) return;

	m_curAction = new Action(GOODACTION_IDLE, ACTIONEND_ANIMEND);

	Anim * anim = CreateAnim(GOODACTION_IDLE);
	if (anim) 
    {
	    m_curAction->SetAnim(anim);
	    m_curAction->SetDelay(0);
	    m_curGoodAction = GOODACTION_IDLE;
    }
}

void GoodActor::Process(void)
{
	sint32		tickCount = GetTickCount();

	if (m_curAction) 
    {
		m_curAction->Process();

		
		if (m_curAction->Finished()) 
        {
			if (m_curAction->GetDelay() > 0 &&
				tickCount > m_curAction->GetDelay()
               ) 
            {
				MapPoint  end;
				m_curAction->GetEndMapPoint(end);
				if (end.x != 0 || end.y != 0) {
					m_pos = end;
				}

				GetNextAction();
			} else {
				if (m_curAction->GetDelay() == 0) {
					m_curAction->SetDelay(tickCount + 8000 + rand() % 5000);
				} else {
					return;
				}
			}
		}
	}

	if (m_curAction) 
    {
		sint32 x, y;
		maputils_MapXY2PixelXY(m_pos.x, m_pos.y, &x, &y);
        Actor::SetPos(x, y);

		
		m_frame = m_curAction->GetSpriteFrame();

		
		m_transparency = m_curAction->GetTransparency();

		if (m_curAction->GetPath()) 
        {
            Actor::SetPos(m_curAction->GetPosition());
		}
		
		m_facing = m_curAction->GetFacing();
	}
}

void GoodActor::GetNextAction(void)
{
	delete m_curAction;
	m_curAction = NULL;

	if (m_actionQueue.GetNumItems() > 0) {
		m_actionQueue.Dequeue(m_curAction);
		if (m_curAction) {
			m_curGoodAction = (GOODACTION)m_curAction->GetActionType();
		} else {
			Assert(FALSE);
		}
	} else {
		
		AddIdle();
	}
}

void GoodActor::AddAction(Action *actionObj)
{
	Assert(m_goodSpriteGroup && actionObj);
	if (!m_goodSpriteGroup || !actionObj) return;
	
	m_actionQueue.Enqueue(actionObj);

	if (m_curAction) {
		if (m_curAction->GetAnim()->GetType() == ANIMTYPE_LOOPED) {
			m_curAction->SetFinished(TRUE);
		}
	}
}

Anim *GoodActor::CreateAnim(GOODACTION action)
{
	Assert(m_goodSpriteGroup);
	if (!m_goodSpriteGroup) return NULL;
	
	Anim	*origAnim = m_goodSpriteGroup->GetAnim((GAME_ACTION)action);
	if (origAnim == NULL) 
	{
		origAnim = m_goodSpriteGroup->GetAnim((GAME_ACTION)GOODACTION_IDLE);
	}

    return origAnim ? new Anim(*origAnim) : NULL;
}

void GoodActor::DrawSelectionBrackets(void)
{

	TileSet		*tileSet = g_tiledMap->GetTileSet();

	RECT		rect;
	SetRect(&rect, 0, 0, 1, 1);
	

	
 	OffsetRect(&rect,	m_x + (sint32)(k_TILE_PIXEL_WIDTH * g_tiledMap->GetScale())/2, 
						m_y + (sint32)(k_TILE_GRID_HEIGHT * g_tiledMap->GetScale())/2);

	
	InflateRect(&rect, 25, 25);

	g_tiledMap->AddDirtyRectToMix(rect);

	POINT iconDim = tileSet->GetMapIconDimensions(MAPICON_BRACKET1);

	rect.right -= (iconDim.x+1);
	rect.bottom -= (iconDim.y+1);

	Pixel16 * topLeft = tileSet->GetMapIconData(MAPICON_BRACKET1);
	Assert(topLeft); if (!topLeft) return;
	Pixel16 * topRight = tileSet->GetMapIconData(MAPICON_BRACKET2);
	Assert(topRight); if (!topRight) return;
	Pixel16 * botRight = tileSet->GetMapIconData(MAPICON_BRACKET3);
	Assert(botRight); if (!botRight) return;
	Pixel16 * botLeft = tileSet->GetMapIconData(MAPICON_BRACKET4);
	Assert(botLeft); if (!botLeft) return;

	g_tiledMap->DrawColorizedOverlayIntoMix(topLeft, rect.left, rect.top);
	g_tiledMap->DrawColorizedOverlayIntoMix(topRight, rect.right, rect.top);
	g_tiledMap->DrawColorizedOverlayIntoMix(botRight, rect.right, rect.bottom);
	g_tiledMap->DrawColorizedOverlayIntoMix(botLeft, rect.left, rect.bottom);
}

bool GoodActor::Draw(bool fogged)
{
	uint16			flags = k_DRAWFLAGS_NORMAL;
	Pixel16			color = 0x0000;
	sint32			xoffset = (sint32)((double)k_ACTOR_CENTER_OFFSET_X * g_tiledMap->GetScale());
	sint32			yoffset = (sint32)((double)k_ACTOR_CENTER_OFFSET_Y * g_tiledMap->GetScale());

	

	






#ifdef _ACTOR_DRAW_OPTIMIZATION
	
	if ( ( m_frame == m_oldFrame ) && 
		( m_x+xoffset == m_oldOffsetX ) && ( m_y+yoffset == m_oldOffsetY ) )
	{
		if (m_paintTwice < 2)
		{
		    m_paintTwice++;
		}
		return false;
	}

	m_paintTwice = 0;

	m_oldFrame = m_frame;
	m_oldOffsetX = m_x + xoffset;
	m_oldOffsetY = m_y + yoffset;
#endif

	if (fogged)
		flags |= k_BIT_DRAWFLAGS_FOGGED;

	m_goodSpriteGroup->Draw(m_curGoodAction, m_frame, m_x+xoffset, m_y+yoffset, m_facing, 
							g_tiledMap->GetScale(), m_transparency, color, flags);

	if (g_selected_item->GetState() == SELECT_TYPE_GOOD) {
		if (m_pos == g_selected_item->GetCurSelectPos()) {
			DrawSelectionBrackets();
		}
	}

	return true;
}

void GoodActor::DrawDirect(aui_Surface *surf, sint32 x, sint32 y, double scale)
{
	uint16			flags = k_DRAWFLAGS_NORMAL;
	Pixel16			color = 0x0000;


	sint32			xoffset = (sint32)((double)k_ACTOR_CENTER_OFFSET_X * scale);
	sint32			yoffset = (sint32)((double)k_ACTOR_CENTER_OFFSET_Y * scale);

	m_goodSpriteGroup->DrawDirect(surf, GOODACTION_IDLE, m_frame, x+xoffset, y+yoffset, m_facing, scale, 
							m_transparency, color, flags);
}

void GoodActor::DrawText(sint32 x, sint32 y, MBCHAR const * goodText)
{
	m_goodSpriteGroup->DrawText(x, y, goodText);
}

POINT GoodActor::GetHotpoint(void) const
{
	POINT pt = {0,0};

	if (!m_goodSpriteGroup) return pt;
	if (!m_goodSpriteGroup->GetGroupSprite((GAME_ACTION)GOODACTION_IDLE)) return pt;

	return m_goodSpriteGroup->GetGroupSprite((GAME_ACTION)GOODACTION_IDLE)->GetHotPoint();
}

bool GoodActor::IsAnimating(void) const
{
	return false;
}

uint16 GoodActor::GetWidth(void) const
{
	Assert(m_goodSpriteGroup);
	if (!m_goodSpriteGroup) return 0;

	Sprite	*   theSprite = m_goodSpriteGroup->GetGroupSprite((GAME_ACTION)m_curGoodAction);
    return (theSprite) ? theSprite->GetWidth() : 0;
}

uint16 GoodActor::GetHeight(void) const
{
	Assert(m_goodSpriteGroup);
	if (!m_goodSpriteGroup) return 0;

	Sprite *    theSprite = m_goodSpriteGroup->GetGroupSprite((GAME_ACTION)m_curGoodAction);
    return (theSprite) ? theSprite->GetHeight() : 0;
}

void GoodActor::GetBoundingRect(RECT *rect) const
{
	Assert(m_goodSpriteGroup && rect);
	if (!m_goodSpriteGroup || !rect) return;

	POINT	hotPoint = m_goodSpriteGroup->GetHotPoint(m_curGoodAction);
	double	scale = g_tiledMap->GetScale();
	sint32	xoff = (sint32)((double)(k_ACTOR_CENTER_OFFSET_X - hotPoint.x) * scale), 
			yoff = (sint32)((double)(k_ACTOR_CENTER_OFFSET_Y - hotPoint.y) * scale);

	rect->left = 0;
	rect->top = 0;
	rect->right = (sint32)((double)GetWidth() * scale);
	rect->bottom = (sint32)((double)GetHeight() * scale);

	OffsetRect(rect, m_x+xoff, m_y+yoff);
}

void GoodActor::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

	if (archive.IsStoring()) 
    {
		archive << m_index;
		m_pos.Serialize(archive);
	} 
    else 
    {
		archive >> m_index;
		m_pos.Serialize(archive);
	}
}
