#ifndef __A_Star_Heuristic_Cost_h__
#define __A_Star_Heuristic_Cost_h__








struct MapPointData;
#include "MapPoint.h"

#include "common.h"
#include "XY_Coordinates.h"
#include "World.h"



#define HEURISTIC_TILES_PER_GRID 4



extern World * g_theWorld;














class A_Star_Heuristic_Cost
{


private:

	
	double * raw_min_movement_costs;

	
	
	double * relaxed_min_movement_costs;

	
	sint32 rows;
	sint32 columns;

	
	sint32 world_rows;
	sint32 world_columns;

	
	bool y_wrap;

	
	
	
	MapPointData xy_pos;				
										
	MapPoint ipos;						
	double *relaxed_min_cost;			




public:



public:

	
	
	
	
	
	
	void Init
	(
		sint32 i_max_rows,
		sint32 i_max_columns,
		bool i_y_wrap
	);
	

	
	
	
	
	
	
	void Trash();
	

	
	
	
	
	
	
	void Update();
	

	
	
	
	
	
	
	
	void Update_One_Tiles_Cost
	(
		MapPointData &the_tile,			
		double new_cost					
	);
	

	
	
	
	
	
	
	
	
	inline double Get_Minimum_Nearby_Movement_Cost
	(
		MapPointData &the_tile			
	)
	{
		




		

		
		
		

		



		
		g_theWorld->XY_Coords.RC_to_XY(the_tile, xy_pos);

		
		relaxed_min_cost = Get_Relaxed_Cost_Grid_Pointer(xy_pos);

		
		return *relaxed_min_cost;

	}


		



private:

	
	
	
	
	
	
	void Clear_Raw_Movement_Costs();


	
	
	
	
	
	
	void Update_Raw_Movement_Costs();


	
	
	
	
	
	
	
	void Relax_Raw_Movement_Costs();


	
	
	
	
	
	
	
	void Relax_One_Cost_Grid
	(
		int row,
		int column
	);


	
	
	
	
	
	
	inline double *Get_Raw_Cost_Grid_Pointer
	(
		MapPointData &the_tile			
	)
	{
		return &(raw_min_movement_costs[
				(the_tile.x/HEURISTIC_TILES_PER_GRID)
				+ ((the_tile.y/HEURISTIC_TILES_PER_GRID) * columns)]);
	}


	
	
	
	
	
	
	inline double *Get_Raw_Cost_Grid_Pointer
	(
		int row, int column					
	)
	{

		return &(raw_min_movement_costs[column + (row * columns)]);
	
	}


	
	
	
	
	
	
	inline double *Get_Relaxed_Cost_Grid_Pointer
	(
		MapPointData &the_tile			
	)
	{
		return &(relaxed_min_movement_costs[
				(the_tile.x/HEURISTIC_TILES_PER_GRID)
				+ ((the_tile.y/HEURISTIC_TILES_PER_GRID) * columns)]);
	}



	
	
	
	
	
	
	inline double *Get_Relaxed_Cost_Grid_Pointer
	(
		int row, int column					
	)
	{

		return &(relaxed_min_movement_costs[column + (row * columns)]);
	
	}

#ifdef SUPER_DEBUG_HEURISTIC
	public:
	void test_terrain_costs();
#endif

};





#endif 
