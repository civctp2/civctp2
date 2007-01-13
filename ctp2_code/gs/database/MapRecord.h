#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#include "Rec.h"

#define k_MAP_LEVELS 11
#define k_NUM_MAP_VARS 14

class Token;

class MapRecord : public Record
{
private:
	sint32 m_numLevels;
	double m_settings[k_MAP_LEVELS][k_NUM_MAP_VARS];
	sint32 m_width, m_height;

public:
	BOOL Parse(Token *token);
	const double *GetSettings(sint32 level, sint32 &numSettings) const;
	void SetSize(sint32 width, sint32 height) { m_width = width; m_height = height; }
	sint32 GetWidth() const { return m_width; }
	sint32 GetHeight() const { return m_height; }
};
