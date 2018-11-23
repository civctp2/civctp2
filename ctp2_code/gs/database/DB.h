#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __DB_H__
#define __DB_H__

typedef sint32 StringId;

class CivArchive ;

template <class T> class Database {

public:


	sint32 m_nRec;
	sint32 m_max_nRec;








	T *m_rec;




	sint32 *m_indexToAlpha;
	sint32 *m_alphaToIndex;




   Database();
   Database(CivArchive &archive) ;
  ~Database();
   void SetSize(const sint32 n);
   void SetSizeAll(const sint32 n);

   void AddRec(const StringId sid, sint32 &i);
   sint32 GetNumRec() const {return m_nRec; };
   const T * Get(const sint32 i) const;
   T * Access(const sint32 i);

   void SetEnabling (const sint32 i, const sint32 e);
   void SetObsolete (const sint32 i, const sint32 e, sint32 index);

   sint32 GetEnabling (const sint32 i) const;
   sint32 GetObsolete (const sint32 i, sint32 index) const;

   StringId GetName(const sint32 i) const {
       Assert(0 <= i);
       Assert(i < m_nRec);
       return m_rec[i].GetName();
   }

   sint32 GetNamedItem (const StringId id, sint32 &index) const;

	sint32 GetNamedItemID (sint32 index, StringId &id ) const;

	void Serialize(CivArchive &archive) ;
};

#if 0

template  class Database<UnitRecord>;
template  class Database<WonderRecord>;
template  class Database<ImproveRecord>;
template  class Database<GWRecord>;
template  class Database<PollutionRecord>;
template  class Database<CivilisationRecord>;
template  class Database<GovernmentRecord>;
template  class Database<TerrainRecord>;
template  class Database<InstallationRecord>;
#endif

#else

template <class T> class Database;

#endif
