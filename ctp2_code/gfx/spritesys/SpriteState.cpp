#include "c3.h"

#include "SpriteState.h"
#include "CivArchive.h"












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
