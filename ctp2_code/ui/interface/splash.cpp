

#include "c3.h"
#include "aui.h"

#include "aui_directsurface.h"

#include "c3ui.h"

#include "pixelutils.h"
#include "primitives.h"
#include "colorset.h"               // g_colorSet

#include "splash.h"

extern C3UI			*g_c3ui;
Splash				*g_splash;

void Splash::Initialize(void)
{
	g_splash = new Splash();
}

void Splash::Cleanup(void)
{
	delete g_splash;
	g_splash = NULL;
}



Splash::Splash()
{
	m_textX = k_SPLASH_FIRST_X;
	m_textY = k_SPLASH_FIRST_Y;
}



Splash::~Splash()
{
	
}



void Splash::AddText(MBCHAR *text)
{
	if (!g_c3ui) return;
	
	aui_DirectSurface * surface = (aui_DirectSurface *) g_c3ui->Primary();

	if (!surface) return;

	primitives_DrawText(surface, m_textX, m_textY, text, g_colorSet->GetColorRef(COLOR_WHITE), TRUE);
}

void Splash::AddTextNL(MBCHAR *text)
{
	if (!g_c3ui) return;
	
	aui_DirectSurface * surface = (aui_DirectSurface *)g_c3ui->Primary();

	if (!surface) return;

	primitives_DrawText(surface, m_textX + 325, m_textY, text, g_colorSet->GetColorRef(COLOR_WHITE), TRUE);

	m_textY += k_SPLASH_TEXT_INC;

	if (m_textY >= surface->Height()) m_textY = k_SPLASH_FIRST_Y;
}

void Splash::AddHilitedTextNL(MBCHAR *text)
{
	if (!g_c3ui) return;
	
	aui_DirectSurface * surface = (aui_DirectSurface *) g_c3ui->Primary();

	if (!surface) return;

	primitives_DrawText(surface, m_textX, m_textY, text, g_colorSet->GetColorRef(COLOR_YELLOW), TRUE);

	m_textY += k_SPLASH_TEXT_INC;

	if (m_textY >= surface->Height()) m_textY = k_SPLASH_FIRST_Y;
}

