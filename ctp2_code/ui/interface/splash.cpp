

#include "c3.h"
#include "splash.h"

#include "aui.h"
#include "aui_directsurface.h"
#include "c3ui.h"
#include "colorset.h"               // g_colorSet
#include "Globals.h"
#include "pixelutils.h"
#include "primitives.h"

extern C3UI			*g_c3ui;

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
       primitives_DrawText(g_c3ui->Primary(),
                            m_textX, m_textY,
                            text,
                            g_colorSet->GetColorRef(COLOR_WHITE),
                            true
                           );
    }
}



void Splash::AddTextNL(MBCHAR const * text)
{
       if (g_c3ui)
    {
           aui_Surface * surface = g_c3ui->Primary();
           if (!surface) return;

           primitives_DrawText(surface,
                            m_textX + 325, m_textY,
                            text,
                            g_colorSet->GetColorRef(COLOR_WHITE),
                            true
                           );

           m_textY += k_SPLASH_TEXT_INC;
           if (m_textY >= surface->Height()) m_textY = k_SPLASH_FIRST_Y;
    }
}

void Splash::AddHilitedTextNL(MBCHAR const *text)
{
       if (g_c3ui)
    {
           aui_Surface * surface = g_c3ui->Primary();

	   if (!surface) return;

	   primitives_DrawText(surface,
                            m_textX, m_textY,
                            text,
                            g_colorSet->GetColorRef(COLOR_YELLOW),
                            true
                           );

	   m_textY += k_SPLASH_TEXT_INC;
	   if (m_textY >= surface->Height()) m_textY = k_SPLASH_FIRST_Y;
    }
}

