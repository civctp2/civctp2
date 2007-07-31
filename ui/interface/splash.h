

#pragma once

#ifndef __SPLASH_H__
#define __SPLASH_H__

#define k_SPLASH_FIRST_X		0
#define k_SPLASH_FIRST_Y		18
#define	k_SPLASH_TEXT_INC		15

#ifdef _DEBUG
#define SPLASH_STRING(x) { if (g_splash) { \
g_splash_cur = GetTickCount(); \
sprintf (g_splash_buf, " %4.2f secs  ", double(g_splash_cur - g_splash_old) * 0.001); \
g_splash_old = g_splash_cur; \
g_splash->AddTextNL(g_splash_buf); \
g_splash->AddText(x); \
}\
}

#define SPLASH_STRING_SIMPLE(x) { g_splash->AddHilitedTextNL(x); }

#else
	#define SPLASH_STRING(x)		;
	#define SPLASH_STRING_SIMPLE(x)	;
#endif

class Splash
{
public:
	static void Initialize(void);
	static void Cleanup(void);

	Splash();
	~Splash();

	void AddText(MBCHAR *text);
	void AddTextNL(MBCHAR *text);
	void AddHilitedTextNL(MBCHAR *text);

private:
	sint32		m_textX, m_textY;
};

#endif