//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
// _MSC_VER		
// - Compiler version (for the Microsoft C++ compiler only)
//
// Note: For the blocks with _MSC_VER preprocessor directives, the following
//       is implied: the (_MSC_VER) preprocessor directive lines, and the blocks
//       that are inactive for _MSC_VER value 1200 are modified Apolyton code. 
//       The blocks that are active for _MSC_VER value 1200 are the original 
//       Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Moved common SpriteGroup member handling to SpriteGroup.
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#ifndef __PROJECTILESPRITEGROUP_H__
#define __PROJECTILESPRITEGROUP_H__

#include "FacedSpriteWshadow.h"
#include "SpriteGroup.h"

#define k_NUM_FIREPOINTS		8

enum PROJECTILEACTION {
	PROJECTILEACTION_NONE = -1,
	
	PROJECTILEACTION_MOVE,

	PROJECTILEACTION_MAX
};

class aui_Surface;
class aui_DirectSurface;

class Sprite;
class FacedSpriteWshadow;

class Anim;

class ProjectileSpriteGroup : public SpriteGroup {
public:
	ProjectileSpriteGroup(GROUPTYPE type);
	virtual ~ProjectileSpriteGroup();

	void			Load(char *filename);
	void			Save(char *filename);

	void			Draw(PROJECTILEACTION action, sint32 frame, sint32 drawX, sint32 drawY, sint32 SdrawX, sint32 SdrawY, sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags, BOOL specialDelayProcess, BOOL directionalAttack = FALSE);

	void			DrawText(sint32 x, sint32 y, char *s);

	void			RunBenchmark(aui_Surface *surf);

	sint32			GetWidth(void) { return m_width; };
	sint32			GetHeight(void) { return m_height; };

	sint32			GetNumFrames(PROJECTILEACTION action);



	POINT			*GetFirePoints(uint16 which) { return m_firePoints[which]; }

	POINT			*GetMoveOffsets(void) { return m_moveOffsets; }

	
	uint16			GetNumFirePoints(void) { return m_numFirePoints; }
	void			SetNumFirePoints(uint16 num) { m_numFirePoints = num; }

	sint32			Parse(uint16 id);

	POINT			GetHotPoint(PROJECTILEACTION action, sint32 facing);

private:
	uint16				m_numFirePoints;
	POINT				m_firePoints[k_NUM_FIREPOINTS][k_NUM_FACINGS];
	
	POINT				m_moveOffsets[k_NUM_FACINGS];



};


#endif