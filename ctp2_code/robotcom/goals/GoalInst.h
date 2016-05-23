#ifndef __GOALS_INSTALLATION_H__
#define __GOALS_INSTALLATION_H__ 1

class CivArchive;
struct MapPointData;
class AiMain;
class CityAgent;
template <class T> class BSet;

class GoalInstallationFlat {
protected:
    sint32 m_type;
    sint32 m_extra_data;
    double m_utility;
    sint32 m_cost;

public:
    GoalInstallationFlat();
    void Serialize(CivArchive &archive);
};

class GoalInstallationPtr {
protected:
    MapPointData *m_pos;
    CityAgent *m_home_city_agent;
public:
    void Serialize(CivArchive &archive);
    void Store(CivArchive &archive);
    void Load (CivArchive &archive, sint32 pcount, uint8 *nmask);
};

class GoalInstallation : public GoalInstallationFlat , public GoalInstallationPtr
{

public:
   GoalInstallation *m_next;

   GoalInstallation();
   GoalInstallation(sint32 t, sint32 extra_data, MapPointData &pos,
        double u, CityAgent *ca, sint32 cost);

   GoalInstallation ( BSet<CityAgent> *bs, CivArchive &archive);

    ~GoalInstallation();
    void Serialize( BSet<CityAgent> *bs, CivArchive &archive);

    void Clear();
    virtual sint32 GetInstallationType() const;
    double GetUtility() const;
    void SetUtility(const double u);
    virtual void GetPos(MapPointData &pos) const;
    virtual sint32 GetCost() const;
    virtual void Construct(AiMain *ai, sint32 &stored);

    BOOL SamePosition(GoalInstallation *test_goal);
    CityAgent *GetHomeCity();

};

#endif __GOALS_INSTALLATION_H__
