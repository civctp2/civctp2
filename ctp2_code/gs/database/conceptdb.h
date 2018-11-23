#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __CONCEPTDB_H__
#define __CONCEPTDB_H__

class CivArchive;
class Token;
#include "dbtypes.h"

struct ConceptInfo {
	StringId	m_name;
	sint32		m_iconDBIndex;
	sint32		m_noIndex;
};

class ConceptDB {

public:
	sint32		m_nConcepts;
	ConceptInfo	*m_conceptInfo;




	sint32 *m_indexToAlpha;
	sint32 *m_alphaToIndex;

	ConceptDB();
	ConceptDB(CivArchive &archive);
	~ConceptDB();

	sint32 ParseAConcept(Token *conceptToken, ConceptInfo *conceptInfo);
	sint32 ParseConceptDatabase(MBCHAR *filename);

	BOOL ParseOptional( Token *conceptToken, ConceptInfo *info );

		void Initialize(void);
	sint32 Init(MBCHAR *filename);

	sint32 GetNumConcepts() const { return m_nConcepts; }
	ConceptInfo *GetConceptInfo( sint32 index ) const { return &m_conceptInfo[index]; }
	const MBCHAR *GetNameStr (sint32 at) const;

	void Serialize(CivArchive &archive);

};

#endif
