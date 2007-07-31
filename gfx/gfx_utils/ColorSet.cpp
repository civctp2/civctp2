










 
#include "c3.h"

#include "Token.h"

#include "pixelutils.h"
#include "ColorSet.h"

#include "c3errors.h"
#include "c3files.h"

ColorSet	*g_colorSet = NULL;
uint32		g_colorSetNum = 0;
extern		sint32 g_is565Format;

ColorSet::ColorSet()
{
	m_numColors = 0;

	sint32		i;

	for (i=0; i<k_MAX_COLOR_SET; i++)
		m_colors[i] = NULL;
}


ColorSet::~ColorSet()
{
}

void ColorSet::Initialize(void)
{
	g_colorSet = new ColorSet;

	g_colorSet->Import(g_colorSetNum);
}

void ColorSet::Import(uint32 index)
{
	sint32		tmp; 
	Token		*theToken;
	MBCHAR		filename[_MAX_PATH];
	Pixel16		color;
	sint32		i;
	uint16		r, g, b;

	sprintf(filename, "Colors%#.2ld.txt", index);

	theToken = new Token(filename, C3DIR_GAMEDATA); 
	Assert(theToken); 	
	if (!theToken) return; 
	
	if (theToken->GetType() != TOKEN_NUMBER) { 
		c3errors_FatalDialog(theToken->ErrStr(), "Missing number of colors.");
	} else { 
		theToken->GetNumber(tmp); 
		if (tmp < 0 || tmp >= k_MAX_COLOR_SET) { 
			c3errors_FatalDialog(theToken->ErrStr(), "Illegal number of colors."); 
		}
	}

	m_numColors = (uint32)tmp;
	
	if (!token_ParseKeywordNext(theToken, TOKEN_COLORSET)) goto Error;

	if (!token_ParseAnOpenBraceNext(theToken)) goto Error; 

	for (i=0; i<(sint32)m_numColors; i++) {
		if (!token_ParseKeywordNext(theToken, TOKEN_COLORSET_COLOR)) goto Error;
		
		if (theToken->Next() == TOKEN_NUMBER) theToken->GetNumber(tmp); 
		else goto Error;
		r = (uint16)tmp;

		if (theToken->Next() == TOKEN_NUMBER) theToken->GetNumber(tmp); 
		else goto Error;
		g = (uint16)tmp;

		if (theToken->Next() == TOKEN_NUMBER) theToken->GetNumber(tmp); 
		else goto Error;
		b = (uint16)tmp;

		color = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
		color = pixelutils_Convert565to555(color);
		
		m_colors[i] = color;
	}

	if (!token_ParseAnCloseBraceNext(theToken)) goto Error; 

	delete theToken;

	return;

Error:
	c3errors_FatalDialog(theToken->ErrStr(), "Error during parse."); 
}

Pixel16 ColorSet::GetColor(COLOR color)
{
	Assert(color >= COLOR_BLACK && color < COLOR_MAX);
	if (color < COLOR_BLACK || color >= COLOR_MAX) return 0;

	return m_colors[color];
}

COLOR ColorSet::ComputePlayerColor(sint32 playerNum)
{
	return (COLOR)(COLOR_PLAYER1 + playerNum);
}

Pixel16 ColorSet::GetPlayerColor(sint32 playerNum)
{
	return GetColor((COLOR)(COLOR_PLAYER1 + playerNum));
}

COLORREF ColorSet::GetColorRef(COLOR color)
{
	uint32		c = (uint32)GetColor(color);
	
	if (g_is565Format) {
		return (COLORREF)((c & 0xF800) >> 8) | ((c & 0x07E0) << 5) | ((c & 0x001F) << 19);
	} else {
		return (COLORREF)((c & 0x7C00) >> 7) | ((c & 0x03e0) << 6) | ((c & 0x001F) << 19);
	}

}

COLORREF ColorSet::GetDarkColorRef(COLOR color)
{
	uint32		c = (uint32)GetDarkColor(color);
	
	if (g_is565Format) {
		return (COLORREF)((c & 0xF800) >> 8) | ((c & 0x07E0) << 5) | ((c & 0x001F) << 19);
	} else {
		return (COLORREF)((c & 0x7C00) >> 7) | ((c & 0x03e0) << 6) | ((c & 0x001F) << 19);
	}
}

COLORREF ColorSet::GetLightColorRef(COLOR color)
{
	uint32		c = (uint32)GetLightColor(color);
	
	if (g_is565Format) {
		return (COLORREF)((c & 0xF800) >> 8) | ((c & 0x07E0) << 5) | ((c & 0x001F) << 19);
	} else {
		return (COLORREF)((c & 0x7C00) >> 7) | ((c & 0x03e0) << 6) | ((c & 0x001F) << 19);
	}
}

Pixel16 ColorSet::GetDarkColor(COLOR color)
{
	return pixelutils_Shadow(GetColor(color));
}

Pixel16 ColorSet::GetDarkPlayerColor(sint32 playerNum)
{
	return pixelutils_Shadow(GetPlayerColor(playerNum));
}

Pixel16 ColorSet::GetLightColor(COLOR color)
{
	return pixelutils_Lightening(GetColor(color));
}

Pixel16 ColorSet::GetLightPlayerColor(sint32 playerNum)
{
	return pixelutils_Lightening(GetPlayerColor(playerNum));
}

