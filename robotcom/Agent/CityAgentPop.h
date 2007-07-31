



    void GetCityVariables(AiMain *ai,  
        sint32 &num_field_workers, sint32 &num_slave, sint32 &affect_pop,
        sint32 &happy, double &cop, double &crime, double &musician_happiness, 
        double &num_musician, sint32 &prod_tile, sint32 &food_tile); 

    sint32 GetMusicianEffect()const;
    sint32 GetNumAffectivePop() const; 
    sint32 GetNumCitizens() const; 

    void CalcProjectedStats(
       double prod_effect, double wage, double ration_size, 
       double delta_wgf_happiness, 
       double &net_prod, double net_gold, double &net_food);


	BOOL PopNeedsChanging(AiMain *ai, const double food_coef,                              
    const double prod_coef, const double gold_coef, sint32 idx_city) ;

   
	BOOL PlaceAllPop( AiMain *ai, const double foodCoef, 
        const double productionCoef, const double goldCoef, 
        const double scienceCoef, const double happyCoef, sint32 idx_city, sint32 nCities);

    void FindOpenTiles(AiMain *ai, TileUtility *open_tile[k_NUM_CITY_TILES], 
        TileUtility open_tile_data[k_NUM_CITY_TILES]);

    
    BOOL SatisfyMinimumCityRequirements( AiMain *ai,
        sint32  &idx_slave, sint32 &remaining_slaves, sint32 &idx_pop, 
        sint32 &remaining_pop, const sint32 total_pop, 
        TileUtility *open_list[k_NUM_CITY_TILES], 
        const double foodCoef, const double productionCoef, const double goldCoef, 
        const double scienceCoef, const double happyCoef, double idx_city, double nCities);

    void PlacePopMinimumGold( AiMain *ai,
        sint32  &idx_slave, sint32 &remaining_slaves, sint32 &idx_pop, 
        sint32 &remaining_pop, const sint32 total_pop, 
        TileUtility *open_list[k_NUM_CITY_TILES], const double foodCoef, 
        const double productionCoef, const double max_coef, 
        double &accum_production, double &accum_food);

    void PlacePopMinimumFood( AiMain *ai,
       sint32  &idx_slave, sint32 &remaining_slaves, sint32 &idx_pop, 
       sint32 &remaining_pop, const sint32 total_pop, 
       TileUtility *open_tile[k_NUM_CITY_TILES], const double foodCoef, 
       const double productionCoef, const double max_coef, double &accum_production, 
       double &accum_food, const double percent);

    
    void PlacePopMinimumProduction( AiMain *ai,
        sint32  &idx_slave, sint32 &remaining_slaves, sint32 &idx_pop, 
        sint32 &remaining_pop, const sint32 total_pop, 
        TileUtility *open_tile[k_NUM_CITY_TILES], double &accum_production);

    void PlaceACitizen(    AiMain *ai, 
     const sint32 idx_tile, sint32 &idx_slave, sint32 &remaining_slaves, 
        sint32 &idx_pop, sint32 &remaining_pop, const sint32 total_pop, 
         TileUtility *open_tile[k_NUM_CITY_TILES]);

    void MaximizeCityOutput(AiMain *ai, 
       sint32  &idx_slaves, sint32 &remaining_slaves, sint32 &idx_pop, 
       sint32 &remaining_pop, const sint32 total_pop, 
       double foodCoef, double productionCoef, double goldCoef, 
       double scienceCoef, double happyCoef, 
       TileUtility *open_tile[k_NUM_CITY_TILES]);

    void CalcTileUtility(TileUtility *open_tile[k_NUM_CITY_TILES], 
        const double foodCoef, const double productionCoef, const double goldCoef);
    void SortOpenTiles(TileUtility *open_tile[k_NUM_CITY_TILES]);

    double CalcGruntUtility( AiMain *ai, const double productionCoef);
    double CalcScientistUtility( AiMain *ai, const double scienceCoef);
    double CalcMusicianUtility(  AiMain *ai, const double happyCoef);

    BOOL PutCitizenBestPosition( AiMain *ai, const sint32 idx_pop, 
        const double prof_utility, const POPTYPE prof,
        TileUtility *open_tiles[k_NUM_CITY_TILES], sint32 &idx_tile);

    BOOL PutCitizenBestTile(AiMain *ai, const sint32 idx_slave, 
        TileUtility *open_tiles[k_NUM_CITY_TILES], sint32 &idx_tile);
 
    void SupplyEntertainers( AiMain *ai, sint32 &idx_pop, 
        sint32 &remaining_pop,  const sint32 total_pop, 
        const double min_happiness, const double max_crime);


