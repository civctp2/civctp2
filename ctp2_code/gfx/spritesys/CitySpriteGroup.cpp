










 


#include "c3.h"

#include "tiffutils.h"
#include "pixelutils.h"

#include "aui_directsurface.h"
#include "primitives.h"
#include "ScreenManager.h"

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

	for (int i = CITYSIZE_1; i<CITYSIZE_MAX; i++) {
		m_sprites[i] = NULL;
	}

	m_width = 0;
	m_height = 0;
}

CitySpriteGroup::~CitySpriteGroup()
{
	for (int i = CITYSIZE_1; i<CITYSIZE_MAX; i++) {
		if (m_sprites[i]) {
			delete m_sprites[i];
			m_sprites[i] = NULL;
		}
	}
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
	primitives_DrawText((aui_DirectSurface *)g_screenManager->GetSurface(), x, y, (MBCHAR *)s, 0, 0);
}


