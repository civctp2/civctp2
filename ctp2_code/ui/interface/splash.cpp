

#include "c3.h"
#include "splash.h"

#include "aui.h"
#include "aui_surface.h"
#include "c3ui.h"
#include "colorset.h"               // g_colorSet
#include "Globals.h"
#include "pixelutils.h"
#include "primitives.h"
#include "profileDB.h"

extern C3UI	*   g_c3ui;

Splash *        g_splash    = NULL;

void Splash::Initialize(void)
{
    allocated::reassign(g_splash, new Splash());
}

void Splash::Cleanup(void)
{
    allocated::clear(g_splash);
}

Splash::Splash()
:
    m_textX     (k_SPLASH_FIRST_X),
    m_textY     (k_SPLASH_FIRST_Y)
{
}

void Splash::AddText(MBCHAR const * text)
{
	if (g_c3ui)
	{
		primitives_DrawText(g_c3ui->Secondary(), 
		                    m_textX, m_textY, 
		                    text, 
		                    g_colorSet->GetColorRef(COLOR_WHITE), 
		                    true
		                   );

		if (g_theProfileDB && g_theProfileDB->IsUseDirectXBlitter())
		{
			g_c3ui->BltSecondaryToPrimary(k_AUI_BLITTER_FLAG_COPY);
		}
		else
		{
			g_c3ui->BltSecondaryToPrimary(k_AUI_BLITTER_FLAG_COPY | k_AUI_BLITTER_FLAG_FAST);
		}
	}
}

void Splash::AddTextNL(MBCHAR const * text)
{
	if (g_c3ui)
	{
		aui_Surface * surface = g_c3ui->Secondary();
		if (!surface) return;

		primitives_DrawText(surface, 
		                    m_textX + 325, m_textY, 
		                    text, 
		                    g_colorSet->GetColorRef(COLOR_WHITE), 
		                    true
		                   );

		m_textY += k_SPLASH_TEXT_INC;
		if (m_textY >= surface->Height()) m_textY = k_SPLASH_FIRST_Y;

		if (g_theProfileDB && g_theProfileDB->IsUseDirectXBlitter())
		{
			g_c3ui->BltSecondaryToPrimary(k_AUI_BLITTER_FLAG_COPY);
		}
		else
		{
			g_c3ui->BltSecondaryToPrimary(k_AUI_BLITTER_FLAG_COPY | k_AUI_BLITTER_FLAG_FAST);
		}
	}
}

void Splash::AddHilitedTextNL(MBCHAR const *text)
{
	if (g_c3ui)
	{
		aui_Surface * surface = g_c3ui->Secondary();

		if (!surface) return;

		primitives_DrawText(surface, 
		                    m_textX, m_textY, 
		                    text, 
		                    g_colorSet->GetColorRef(COLOR_YELLOW), 
		                    true
		                   );

		m_textY += k_SPLASH_TEXT_INC;
		if (m_textY >= surface->Height()) m_textY = k_SPLASH_FIRST_Y;

		if (g_theProfileDB && g_theProfileDB->IsUseDirectXBlitter())
		{
			g_c3ui->BltSecondaryToPrimary(k_AUI_BLITTER_FLAG_COPY);
	}
		else
		{
			g_c3ui->BltSecondaryToPrimary(k_AUI_BLITTER_FLAG_COPY | k_AUI_BLITTER_FLAG_FAST);
		}
	}
}
