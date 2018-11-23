#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __GW_RECORD_H__
#define __GW_RECORD_H__

#include "Rec.h"
#include "gstypes.h"

class Token;

#define MAX_CHANGES		64





























class CivArchive ;

class Token ;

class GWRecord : public Record
	{
	public :


		sint32	m_changes ;

		TERRAIN_TYPES	m_original[MAX_CHANGES],
						m_flip[MAX_CHANGES] ;

		double	m_prob[MAX_CHANGES],
				m_next_to_ocean_bonus,
				m_neighbour_bonus,
				m_max_bonus ;












		GWRecord() ;

		sint32 GetChanges(void) const { return (m_changes) ; }

		sint32 ParseGWRecord(Token *token) ;
		void Serialize(CivArchive &archive) ;

	} ;

#endif
