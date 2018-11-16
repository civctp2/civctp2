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

#include "tiffutils.h"
#include "pixelutils.h"

#include "aui_directsurface.h"
#include "primitives.h"
#include "screenmanager.h"

#include "CitySpriteGroup.h"
#include "FacedSprite.h"
#include "Sprite.h"

#include "CivPaths.h"
#include "c3files.h"

extern CivPaths *g_civPaths;
extern ScreenManager *g_screenManager;

CitySpriteGroup::CitySpriteGroup(GROUPTYPE type)
:
SpriteGroup(type)
{
	m_currentSize = CITYSIZE_1;
}

CitySpriteGroup::~CitySpriteGroup()
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

void CitySpriteGroup::DrawText(sint32 x, sint32 y, char *s)
{
	primitives_DrawText(g_screenManager->GetSurface(), x, y, (MBCHAR *)s, 0, 0);
}
