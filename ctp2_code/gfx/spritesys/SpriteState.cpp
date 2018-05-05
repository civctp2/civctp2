#include "ctp/c3.h"

#include "gfx/spritesys/SpriteState.h"
#include "robot/aibackdoor/civarchive.h"












SpriteState::SpriteState(CivArchive &archive)
	{
	Serialize(archive) ;
	}












void SpriteState::Serialize(CivArchive &archive)
	{
	if (archive.IsStoring())
		archive<<m_index ;
	else
		archive>>m_index ;

	}
