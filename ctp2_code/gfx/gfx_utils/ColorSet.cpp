//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Color set
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Reorganised to try to prevent crashes.
// - Number of colors in a ColorsXX.txt must be now either 58 like in the 
//   original game or 74 like in the source code edition. If there are just
//   58 the missing player colors are filled with the map colors at that 
//   like in the original version. (Oct 22nd 2005 Martin Gühmann)
// - Relaxed the above to allow more than 74 color entries in the future.
// - Added check for invalid/unassigned player index in ComputePlayerColor.
//
//----------------------------------------------------------------------------
 
#include "c3.h"
#include "colorset.h"

#include "c3errors.h"
#include "c3files.h"
#include "pixelutils.h"
#include <stdexcept>    // std::exception, std::runtime_error
#include "Token.h"

extern sint32 g_is565Format;

namespace
{

/// Color for invalid players
COLOR const  COLOR_PLAYER_INVALID   = COLOR_BLACK;
/// Number of colors in the original Activision set
size_t const OLD_COLOR_NUMBER       = 58;
/// Expected minimal number of colors in the Apolyton source code edition
size_t const NEW_COLOR_NUMBER_MIN   = 74;

ColorSet     s_theUniqueColorSet;

//----------------------------------------------------------------------------
//
// Name       : ColorRef
//
// Description: Convert 16 (or 15) bit color to 24 bit COLORREF value
//
// Parameters : c               : color in native format
//
// Globals    : g_is565Format   : native format is RGB565 (when false: RGB555)
//                                                  
//
// Returns    : COLORREF        : color in RGB888 format
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
COLORREF ColorRef(Pixel16 const & c)
{
    if (g_is565Format) // 5-6-5 bit pixel format
    {
        return static_cast<COLORREF>
            (((c & 0xF800) >> 8) | ((c & 0x07E0) << 5) | ((c & 0x001F) << 19));
    }
    else // 5-5-5 bit pixel format
    {
        return static_cast<COLORREF>
            (((c & 0x7C00) >> 7) | ((c & 0x03e0) << 6) | ((c & 0x001F) << 19));
    }
}

//----------------------------------------------------------------------------
//
// Name       : ParseNextNumber
//
// Description: Get the next number from a token stream.
//
// Parameters : tokenStream     : the token stream
//
// Globals    : - 
//
// Returns    : <T>             : the next number from the stream.
//
// Remark(s)  : throws when there is no next number in the stream.
//
//----------------------------------------------------------------------------
template <typename T> T ParseNextNumber(Token & tokenStream)
{
    if (TOKEN_NUMBER != tokenStream.Next())
    {
        throw std::runtime_error("Error during parse.");
    }


    sint32  tmp;
    tokenStream.GetNumber(tmp);
    return static_cast<T>(tmp);
}

} // local namespace

ColorSet *	g_colorSet  = &s_theUniqueColorSet; 

ColorSet::ColorSet()
:   m_colors()
{ ; }

ColorSet::~ColorSet()
{
    std::vector<Pixel16>().swap(m_colors);
}

//----------------------------------------------------------------------------
//
// Name       : ColorSet::Cleanup
//
// Description: Release all color data.
//
// Parameters : -
//
// Globals    : s_theUniqueColorSet : cleaned
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void ColorSet::Cleanup(void)
{
    std::vector<Pixel16>().swap(s_theUniqueColorSet.m_colors);
}

//----------------------------------------------------------------------------
//
// Name       : ColorSet::Initialize
//
// Description: Fill the color data from a file.
//
// Parameters : fileNumber          : the ## part of Colors##.txt
//
// Globals    : s_theUniqueColorSet : filled
//
// Returns    : -
//
// Remark(s)  : When the file number is too large, color set 0 (Colors00.txt)
//              is used.
//
//----------------------------------------------------------------------------
void ColorSet::Initialize(uint32 fileNumber)
{
	s_theUniqueColorSet.Import((fileNumber < k_MAX_COLOR_SET) ? fileNumber : 0);
}

//----------------------------------------------------------------------------
//
// Name       : ColorSet::Import
//
// Description: Fill the color data from a file.
//
// Parameters : fileNumber      : the ## part of Colors##.txt
//
// Globals    : g_is565Format   : native format is RGB565 (when false: RGB555)
//
// Returns    : -
//
// Remark(s)  : When anything fails (file can not be found or has invalid 
//              format) the color data is cleared, and all colors are black.
//
//----------------------------------------------------------------------------
void ColorSet::Import(uint32 fileNumber)
{
	MBCHAR		filename[_MAX_PATH];
	sprintf(filename, "Colors%.2ld.txt", fileNumber);
	Token       theToken(filename, C3DIR_GAMEDATA); 

    try
    {
        if (TOKEN_NUMBER != theToken.GetType())
        {
            throw std::runtime_error("Missing number of colors.");
        }

        sint32 fileColorCount = 0;
        theToken.GetNumber(fileColorCount); 
        if (    (fileColorCount != static_cast<sint32>(OLD_COLOR_NUMBER))
		     && (fileColorCount <  static_cast<sint32>(NEW_COLOR_NUMBER_MIN))
           ) 
		{ 
	        throw std::runtime_error("Illegal number of colors.");
        }

        if (!token_ParseKeywordNext(&theToken, TOKEN_COLORSET) ||
	        !token_ParseAnOpenBraceNext(&theToken)
           )
        {
            throw std::runtime_error("Error before open brace."); 
        }

        size_t const    arraySize   = 
            std::max(NEW_COLOR_NUMBER_MIN, static_cast<size_t>(fileColorCount));

        m_colors.resize(arraySize);
        for (size_t i = 0; i < static_cast<size_t>(fileColorCount); ++i) 
        {
	        if (!token_ParseKeywordNext(&theToken, TOKEN_COLORSET_COLOR)) 
            {
		        throw std::runtime_error("Error during parse."); 
            }
	        
	        uint16 const    r       = ParseNextNumber<uint16>(theToken);
            uint16 const    g       = ParseNextNumber<uint16>(theToken);
            uint16 const    b       = ParseNextNumber<uint16>(theToken);

            // The file has RGB888 format: convert to native format.
	        Pixel16 const   rgb565  = static_cast<Pixel16>
                (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3));

			if (   (static_cast<size_t>(fileColorCount) == OLD_COLOR_NUMBER)
			    && (COLOR_PLAYER18 <= static_cast<COLOR>(i))
			   )
            {
                // The playtest made itself incompatible by middle-insertion.
                // The following index shift hack compensates for this.
				size_t const    hackIndex   = 
                    i + static_cast<size_t>(COLOR_TERRAIN_0)
                      - static_cast<size_t>(COLOR_PLAYER18);
                
                m_colors[hackIndex] = pixelutils_Convert565to555(rgb565);
                // And another hack: copy terrain colors to player colors?!
                m_colors[i]         = m_colors[hackIndex];
			}
            else
            {
                // When g_is565Format is true, pixeluitls_Conver565to555 is an
                // identity operation.
		        m_colors[i] = pixelutils_Convert565to555(rgb565);
            }
        }

        if (!token_ParseAnCloseBraceNext(&theToken))
        {
            // Missing closing brace: only generate a warning at this point.
            c3errors_ErrorDialog(theToken.ErrStr(), "Missing close brace.");
        }
    }
    catch (std::exception const & error)
    {
        std::vector<Pixel16>().swap(m_colors);
        c3errors_FatalDialog(theToken.ErrStr(), error.what()); 
    }
}

Pixel16 ColorSet::GetColor(COLOR color) const
{
    size_t const    index = static_cast<size_t>(color);
	Assert(index < m_colors.size());
    return (index < m_colors.size()) ? m_colors[index] : (Pixel16) 0;
}

/// Get the color of a player
/// \param playerNum Index of the player
/// \remarks Validity of the player index is tested partially.
COLOR ColorSet::ComputePlayerColor(sint32 playerNum) const
{
	return (playerNum >= 0)
	       ? static_cast<COLOR>(COLOR_PLAYER1 + playerNum)
	       : COLOR_PLAYER_INVALID;
}

Pixel16 ColorSet::GetPlayerColor(sint32 playerNum) const
{
	return GetColor(ComputePlayerColor(playerNum));
}

COLORREF ColorSet::GetColorRef(COLOR color) const
{
	return ColorRef(GetColor(color));
}

COLORREF ColorSet::GetDarkColorRef(COLOR color) const
{
	return ColorRef(GetDarkColor(color));
}

COLORREF ColorSet::GetLightColorRef(COLOR color) const
{
	return ColorRef(GetLightColor(color));
}

Pixel16 ColorSet::GetDarkColor(COLOR color) const
{
	return pixelutils_Shadow(GetColor(color));
}

Pixel16 ColorSet::GetDarkPlayerColor(sint32 playerNum) const
{
	return pixelutils_Shadow(GetPlayerColor(playerNum));
}

Pixel16 ColorSet::GetLightColor(COLOR color) const
{
	return pixelutils_Lightening(GetColor(color));
}

Pixel16 ColorSet::GetLightPlayerColor(sint32 playerNum) const
{
	return pixelutils_Lightening(GetPlayerColor(playerNum));
}

