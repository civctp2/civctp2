
#pragma once 


#ifndef __INSTALLATION_BID_LIST_H__ 
#define __INSTALLATION_BID_LIST_H__ 1


class GoalInstallation; 
class CivArchive; 
class CityAgent;
template <class T> class BSet;
struct MapPointData; 
class AiMain; 

class InstallationBidListFlat
{ 
protected:
         sint32 m_nInstallations; 
public:
    InstallationBidListFlat(); 
    void Serialize(CivArchive &archive); 
};

class InstallationBidList : public InstallationBidListFlat{ 

    sint32 *m_ngoals; 
	GoalInstallation ** m_inst; 

public:
    InstallationBidList(sint32 ni); 
    InstallationBidList(BSet<CityAgent> *bs, CivArchive &archive);
    ~InstallationBidList(); 
    void Serialize(BSet<CityAgent> *bs, CivArchive &archive); 

    void Clear(); 

    void InsertGoal (GoalInstallation *goal_obj); 

    void CalcDesiredPW(AiMain *ai); 

    BOOL GetHighestProfit(AiMain *ai, GoalInstallation *&max_goal); 

    void RegisterMyRemoveCity(const uint32 uid);


}; 

#endif __INSTALLATION_BID_LIST_H__ 