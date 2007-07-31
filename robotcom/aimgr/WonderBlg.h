

#ifndef __WONDER_BLG_H__
#define __WONDER_BLG_H__ 1

class CivArchive; 
class AiMain; 

enum WONDER_STATE { 
    WONDER_STATE_CANT_BUILD, 
    WONDER_STATE_UNBUILT, 
    WONDER_STATE_SCHEDULED, 
    WONDER_STATE_BUILDING, 
    WONDER_STATE_DONE, 
    WONDER_STATE_OBSOLETE
};


class BSetID; 

class WonderBlgFlat{ 
protected:

    sint32 m_idx_wonder; 
    double m_production_cost; 
    WONDER_STATE m_global_state; 
    double m_global_percent_done; 
    WONDER_STATE m_my_state; 
    double m_my_pecent_done; 

    sint32 m_enabling; 
    sint32 m_obsolete; 

    BOOL m_i_own_it; 
    PLAYER_INDEX m_wonder_owner; 

public:
	WonderBlgFlat(); 
	void Serialize(CivArchive &archive); 
};


class WonderBlgPtr { 
protected:
    double *m_effect; 
    BSetID *m_home_city; 
public:

	WonderBlgPtr(); 
    ~WonderBlgPtr(); 
	void Serialize(CivArchive &archive);
	void Store(CivArchive &archive);
    void Load(CivArchive &archive, sint32 pcount, uint8 nmask[]);
};


class WonderBlg : public WonderBlgFlat , public WonderBlgPtr { 

public:


    WonderBlg(); 
    WonderBlg(CivArchive &archive); 
    ~WonderBlg(); 
    void Serialize(CivArchive &archive); 



    void Init(AiMain *ai, const sint32 iw);

    BOOL CanBuild(AiMain *ai); 
    double* GetEffects();

    double GetProductionCost(); 

    void SetIamBuilding();

    void RegisterNewScience(const sint32  idx_sci);
    BOOL RegisterCreateWonder(const uint32 city_id, const PLAYER_INDEX  my_idx, 
        PLAYER_INDEX owner); 
    void RegisterCapturedWonder();


    void SetMyState(const WONDER_STATE s); 
    WONDER_STATE GetMyState() const { return m_my_state; } 
    WONDER_STATE GetGlobalState() const { return m_global_state; }
    BOOL IOwnIt() const  { return m_i_own_it; }
	BSetID *GetHomeCity() { return m_home_city; }
	PLAYER_INDEX GetOwner();
};

#endif __WONDER_BLG_H__




