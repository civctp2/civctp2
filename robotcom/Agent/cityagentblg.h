



    double UtilityBlgSilo(AiMain *ai,  double effect[]);
    double UtilityBlgCapitol(AiMain *ai,  double effect[]);
    double UtilityBlgHappy(AiMain *ai,  double effect[]);

    double UtilityBlgWaste(AiMain *ai,  double effect[]);
    double UtilityBlgCrime(AiMain *ai,  double effect[]);

    double UtilityBlgProductionInc(AiMain *ai,  double effect[]);
    double UtilityBlgFoodInc(AiMain *ai,  double effect[]);
    double UtilityGoldInc(AiMain *ai,  double effect[]);
    double UtilityKnowledgeInc(AiMain *ai,  double effect[]);
    double UtilityProdEntertainInc(AiMain *ai,  double effect[]);

    double UtilityProdPercent(AiMain *ai,  double effect[]);
    double UtilityFoodPercent(AiMain *ai,  double effect[]);
    double UtilityGoldPercent(AiMain *ai,  double effect[]);
    double UtilityKnowledgePercent(AiMain *ai,  double effect[]);
    double UtilityEntertainPercent(AiMain *ai,  double effect[]);

    
    double UtilityBlgDefense(AiMain *ai,  double effect[]);
    double UtilityBlgWall(AiMain *ai,  double effect[]);

    double UtilityBlgSpaceLauncher(AiMain*ai, double effect[]);


    double ActionBidBuild(AiMain *ai,  GoalBuilding *goal_obj);     

    void SetHasSpaceLauncher(const BOOL val); 
    BOOL HasSpaceLauncher() const;
    BOOL HasAirport() const;
    BOOL HasGranary() const;

    void ResetBuildingGoal(const sint32 blg_idx); 



