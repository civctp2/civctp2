
#ifndef FEAT_TRACKER_H__
#define FEAT_TRACKER_H__

class CivArchive;
template <class T> class PointerList;
class Unit;



enum FEAT_EFFECT {
	FEAT_EFFECT_NONE = -1,
	FEAT_EFFECT_BOAT_MOVEMENT,  
	FEAT_EFFECT_CITY_DEFENSE_BONUS,
	FEAT_EFFECT_REDUCE_CITY_WALLS,
	FEAT_EFFECT_INCREASE_CITY_VISION,
	FEAT_EFFECT_INCREASE_PRODUCTION, 
	FEAT_EFFECT_INCREASE_COMMERCE,   
	FEAT_EFFECT_INCREASE_HAPPINESS,  
	FEAT_EFFECT_ELIMINATE_DISTANCE_PENALTY,
	FEAT_EFFECT_INCREASE_BOAT_VISION,
	FEAT_EFFECT_INCREASE_SCIENCE, 
	FEAT_EFFECT_GIVE_MAPS,
	FEAT_EFFECT_INCREASE_HIT_POINTS,
	FEAT_EFFECT_SCRIPTED_TURN,
	FEAT_EFFECT_SCRIPTED_CITY,

	
	FEAT_EFFECT_MAX
};

	
class Feat {
  public:
	Feat(sint32 type, sint32 player);

	Feat(CivArchive &archive);
	~Feat();
	void Serialize(CivArchive &archive);

	sint32 GetPlayer() { return m_player; }
	sint32 GetType() { return m_type; }
	sint32 GetRound() { return m_round; }

  private:

	sint32 m_type; 
	sint32 m_player; 
	sint32 m_round; 
	
};

class FeatTracker {
  public:
	FeatTracker();
	FeatTracker(CivArchive &archive);
	~FeatTracker();
	void Serialize(CivArchive &archive);

	static void Initialize();
	static void Cleanup();

	void AddFeatToEffectLists(Feat *feat);
	void RemoveFeatFromEffectLists(Feat *feat);
	void AddFeat(sint32 type, sint32 player);
	void AddFeat(const MBCHAR *name, sint32 player);

	sint32 GetEffect(FEAT_EFFECT effect, sint32 player, bool getTotal);
	sint32 GetAdditiveEffect(FEAT_EFFECT effect, sint32 player);
	sint32 GetMaxEffect(FEAT_EFFECT effect, sint32 player);

	void BeginTurn(sint32 player);

	void FindBuildingFeats();
	void CheckBuildingFeat(Unit &city, sint32 building);
	void CheckConquerFeat(sint32 defeated, sint32 defeatedByWhom);

	static void InitializeEvents();
	static void CleanupEvents();

  private:
	PointerList<Feat> *m_activeList;  
	PointerList<Feat> *m_effectList[FEAT_EFFECT_MAX]; 
	bool *m_achieved; 
	bool *m_buildingFeat; 
};

extern FeatTracker *g_featTracker;

#endif

