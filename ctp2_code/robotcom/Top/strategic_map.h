#ifndef __Strategic_Map_h__
#define __Strategic_Map_h__










#define DEFAULT_AI_MAP_CONFIG_FILE "strategic_AI_config.txt"


#define RELAX_SHARED_SIDE (1.0)
#define RELAX_DIAGONAL (1.41)


#define INSIDE_EMPIRE_DISTANCE_DIVISOR (0.5)

#define MAX_POWER_RATIO (10.0)

#define MIN_THREAT_VALUE (25.0)









struct MapPointData;


typedef float threat_type;








class Strategic_Map_Flat_Data
{

public:

	sint32 threat_map_resolution;
	sint32 death_map_resolution;
	sint32 empire_map_resolution;
	sint32 exploration_map_resolution;
	sint32 goal_map_resolution;

	double relaxation_coefficient;
	sint32 relaxation_cycles;


	sint32 our_city_count;
	sint32 free_city_count;

	double summed_threat;
	double our_summed_power;

	double m_max_power;
	double m_max_value;
	double m_max_threat;
	double m_max_deaths;
	double m_min_deaths;

	int rough_count_of_explored_cells;
	float m_space_threat;

protected:
    Strategic_Map_Flat_Data();
    void Serialize(CivArchive &archive);


private:
};
















class Strategic_Map : public Strategic_Map_Flat_Data
{

public:


private:

	matrix<double> * death_map;
	matrix<sint8> * exploration_map;

	threat_type dest_threat;
	threat_type adjacent_threat;
	threat_type new_threat;



































	AiMain *ai;


public:









	Strategic_Map
	(
		AiMain *init_ai,
		char * strategic_AI_config_file_name = NULL
	);









	~Strategic_Map();

    Strategic_Map(AiMain *ai, CivArchive &archive);
    void Serialize(CivArchive &archive);








	sint32 Get_Our_City_Count();








	void Update_Death_Differential();
	double Get_Max_Danger() const { return m_max_deaths;}
	double Get_Min_Danger() const { return m_min_deaths;}







	void Update_Threats();
	double Get_Max_Threat() const { return m_max_threat;}







	void Update_Power_Map();
	void Clear_Power_Map();
	void Relax_Power_Map();
	double Get_Max_Power() const { return m_max_power;}







	void Update_Value_Map();
	void Clear_Value_Map();
	void Relax_Value_Map();
	double Get_Max_Value() const { return m_max_value;}

	float Find_Map_Max(matrix<float> *map);
	float Find_Map_Min(matrix<float> *map);
	double Find_Map_Max(matrix<double> *map);
	double Find_Map_Min(matrix<double> *map);






	double Get_Power_Bonus(AiMain *ai, MapPointData xyPos);
	double Get_Value_Bonus(AiMain *ai, MapPointData xyPos);
	double Get_Danger_Bonus(AiMain *ai, MapPointData xyPos);
	double Get_Threat_Bonus(AiMain *ai, MapPointData xyPos);







	void Update_Overall_Threat();









	void Update_Power_Ratio();








	void Update_Our_Empire(AiMain *ai);









	void Update_Enemy_Empire(AiMain *ai);








	void Update_Explored_Territory(AiMain *ai);








	inline void XY_to_MapgridRC
	(
		sint32 x,
		sint32 y,
		sint32 &row,
		sint32 &column,
		sint32 mapgrid_resolution
	)
	{

		row = y/mapgrid_resolution;
		column = x/mapgrid_resolution;
	}









	inline void MapgridRC_to_XY
	(
		sint32 row,
		sint32 column,
		sint32 &x,
		sint32 &y,
		sint32 mapgrid_resolution
	)
	{

		y = row * mapgrid_resolution;
		x = column * mapgrid_resolution;
	}







	void Adjust_Death_Differential
	(
		double adjustment,
		MapPointData &query_pos
	);








	double Get_Death_Differential
	(
		MapPointData &query_pos
	);








	double Get_Threat(MapPointData &query_pos);

	double Get_Power(MapPointData &query_pos);

	double Get_Value(MapPointData &query_pos);








	int Get_Distance_To_Our_Empire
	(
		MapPointData &query_pos
	);








	int Get_Distance_To_Enemy_Empire
	(
		MapPointData &query_pos
	);








	int Get_Distance_To_Enemy_Empire
	(
		sint32 row,
		sint32 column
	);








	void Get_Empire_Map_Dimensions
	(
		sint32 & rows,
		sint32 & columns
	);








	sint32 Get_Distance_To_Empire_Mapgrid_Value
	(
		sint32 row,
		sint32 column
	);








	void Get_Empire_Mapgrid_Center_Tile
	(
		sint32 row,
		sint32 column,
		MapPointData &norm_pos
	);








	void Get_Empire_Mapgrid_Random_Tile
	(
		sint32 row,
		sint32 column,
		MapPointData &norm_pos
	);








	void Get_Explore_Map_Dimensions
	(
		sint32 & rows,
		sint32 & columns
	);








	sint32 Must_We_Explored_Here
	(
		sint32 row,
		sint32 column
	);








	void Get_Explore_Mapgrid_Center_Tile
	(
		sint32 row,
		sint32 column,
		MapPointData &norm_pos
	);

	BOOL Strategic_Map::Get_Unexplore_Mapgrid_Neighbor_Tile
	(
		sint32 row,
		sint32 column,
		MapPointData &norm_pos,
		uint32 corner,
		BOOL x_wrap,
		BOOL y_wrap
	);








	GoalMapTarget *GetAreaGoal
	(
		const MapPointData &norm_pos,
		const GOAL_TYPE &type
	);








	void SetAreaGoal
	(
		const MapPointData &norm_pos,
		const GOAL_TYPE &type,
		GoalMapTarget *tmp_goal
	);


private:








	void Load_Map_Config
	(
		char * strategic_AI_config_file_name
	);








	void Init_Maps();








	void Clean_Up();








	void Clear_Death_Differential();








	void Clear_Threats();









	inline void Relax_Two_Cells
	(
		sint32 row,
		sint32 column,
		sint32 into_row,
		sint32 into_column,
		double relaxation_divisor,
		matrix<threat_type> *source_map,
		matrix<threat_type> *dest_map,
		threat_type this_cell_value,
		sint32 cycle
	)
	{

		if (into_row < 0) into_row = source_map->Get_Rows() - 1;
		if (into_column < 0) into_column = source_map->Get_Columns() - 1;
		if (into_row >= source_map->Get_Rows()) into_row = 0;
		if (into_column >= source_map->Get_Columns()) into_column = 0;









		dest_threat = (*dest_map)(into_row, into_column);

		adjacent_threat = (threat_type) (
			(relaxation_coefficient * this_cell_value /
			 relaxation_divisor));


		new_threat = max(dest_threat, adjacent_threat);

		if (new_threat < MIN_THREAT_VALUE)
			new_threat = 0.0;

		(*dest_map)(into_row, into_column) = new_threat;






	}









	void Relax_Threats();








	void Log_Threats();








	void Log_Death_Differential();








	void Clear_Our_Empire();








	void Log_Our_Empire();








	void Clear_Enemy_Empire();








	void Log_Enemy_Empire();








	void Clear_Exploration_Map();








	void Clear_Goal_Map();








	void Log_Exploration_Map();

};






#endif
