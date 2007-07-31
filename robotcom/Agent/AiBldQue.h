

#pragma once

#ifndef __AI_BUILD_QUEUE_H__
#define __AI_BUILD_QUEUE_H__ 1

class Agent; 
class AiBuildNode; 
class AiBuildQueue; 
class BSetID; 
class CivArchive; 
class BSetID; 
class AiMain; 

enum BQ_CATEGORY { 
    BQ_CATEGORY_ARMY, 
    BQ_CATEGORY_BUILDING,
    BQ_CATEGORY_FREIGHT,
    BQ_CATEGORY_WONDER,
    BQ_CATEGORY_END
}; 

class AiBuildNode { 

    sint32 m_cost; 
    BQ_CATEGORY m_category; 
    sint32 m_type; 
    double m_utility; 
    BSetID *m_id; 
    Agent *m_data; 
    AiBuildNode * m_next; 
   

public:

    friend AiBuildQueue; 

     AiBuildNode();
    AiBuildNode(BQ_CATEGORY cat, sint32 type, sint32 cost, Agent *aa, const BSetID &id, 
        const double utility);
    ~AiBuildNode();
    BQ_CATEGORY GetCategory() const { return m_category; }
};

class AiBuildQueue { 
	
    sint32 m_len; 
    AiBuildNode *m_head, *m_tail; 

public: 

	AiBuildQueue();
    ~AiBuildQueue(); 
    AiBuildQueue(CivArchive &archive); 
    void Serialize(CivArchive &archive); 

    void Enqueue(BQ_CATEGORY cat, sint32 type, sint32 cost, Agent *aa, 
        double utility);
    void ChangeBuildHead(AiMain *ai, BQ_CATEGORY cat, sint32 item_type,
        sint32 cost); 

    AiBuildNode* Dequeue(); 
    sint32 GetLen() const { return m_len; }

    void HookBldQueue(AiMain *ai);  

    BSetID PeekHeadID() const; 

    sint32 EstimateRoundsToFinished(AiMain *ai, BSetID id,
        sint32 production_store, const sint32 net_production) const;
    sint32 EstimateRoundsToEmptyQueue(AiMain *ai, 
         const sint32 production_store, sint32 net_production) const; 
	sint32 AiBuildQueue::TurnsToFinishFront(sint32 production_store, sint32 net_production);

	
	BOOL AiBuildQueue::FrontIsImprovement();
	double AiBuildQueue::EstimatePercentItem(sint32 production_store); 

    BOOL GetHead(BQ_CATEGORY &cat, sint32 &type); 

    double GetFrontUtility() const; 
};

#endif __BUILD_QUEUE_H__