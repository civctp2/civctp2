//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  :
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Moved common SpriteGroup member handling to SpriteGroup.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "CitySpriteGroup.h"

#include "primitives.h"
#include "screenmanager.h"
#include "Sprite.h"

extern ScreenManager *  g_screenManager;

CitySpriteGroup::CitySpriteGroup(GROUPTYPE type)
:
    SpriteGroup     (type),
    m_currentSize   (CITYSIZE_1)
{
}

void CitySpriteGroup::Draw(sint32 drawX, sint32 drawY, sint32 facing, double scale,
					  uint16 transparency, Pixel16 outlineColor, uint16 flags)
{
	Assert(m_currentSize >= CITYSIZE_1 &&
			m_currentSize < CITYSIZE_MAX);

	m_sprites[m_currentSize]->Draw(drawX, drawY, facing, scale, transparency, outlineColor, flags);
}

void CitySpriteGroup::Import(uint16 id)
{
}

void CitySpriteGroup::DrawText(sint32 x, sint32 y, MBCHAR const * s)
{
	primitives_DrawText(g_screenManager->GetSurface(), x, y, s, 0, 0);
}
