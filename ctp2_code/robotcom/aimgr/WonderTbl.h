



#ifndef __WONDER_H__
#define __WONDER_H__ 1

class AiMain;
class CivArchive; 
class WonderBlg; 
class CityAgent;

class BSetID; 

enum WONDER_STATE;
typedef sint32 PLAYER_INDEX;


class WonderTableFlat{ 
protected:
    sint32 big_dumb_var; 
public:
	WonderTableFlat(); 
	void Serialize(CivArchive &archive); 
};


class WonderTablePtr { 
protected:

    sint32 m_num_wonders; 
    WonderBlg *m_array; 
	BOOL m_emancipation;

public:
	WonderTablePtr(); 
    ~WonderTablePtr(); 
	void Serialize(CivArchive &archive);
	void Store(CivArchive &archive);
    void Load(CivArchive &archive, sint32 pcount, uint8 nmask[]);
};


class WonderTable : public WonderTableFlat , public WonderTablePtr { 

public:


    WonderTable(AiMain *ai); 
    WonderTable(CivArchive &archive); 
    ~WonderTable(); 
    void Serialize(CivArchive &archive); 

   
	
	
	
	




	BOOL GetEmancipation() { return m_emancipation;}
    double * GetEffects(const sint32 idx_wonder);
	bool CanInsertWonderGoalForCity
	(
		AiMain *ai,
		CityAgent *the_city,
		sint32 which_wonder
	);
	void SetWonderState(sint32 which_wonder, WONDER_STATE state);

    double GetProductionCost(const sint32 idx_wonder) const; 
    BOOL CanBuild(AiMain *ai, const sint32 idx_wonder) const;
    void SetIamBuilding(const sint32 idx_wonder); 

    sint32 GetNumWonders() const; 
    void RegisterNewScience(const sint32  idx_sci);
    BOOL RegisterCreateWonder(uint32 city_id, const sint32 wonder_type, 
       const PLAYER_INDEX  my_idx,  const PLAYER_INDEX owner);
    void RegisterCapturedWonder(const sint32 idx_wonder); 

    WONDER_STATE GetMyState(const sint32 idx_wonder) const; 
    WONDER_STATE GetGlobalState(const sint32 idx_wonder) const; 
    BOOL IOwnIt(const sint32 idx_wonder) const; 
	BSetID *GetHomeCity(const sint32 idx_wonder); 
	
};

#endif __WONDER_H__

