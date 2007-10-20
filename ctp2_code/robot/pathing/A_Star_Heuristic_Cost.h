//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Heuristic cost for the A* pathing algorithm
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
// 
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - X-wrap added, structure cleaned up 
//
//----------------------------------------------------------------------------

#ifndef __A_Star_Heuristic_Cost_h__
#define __A_Star_Heuristic_Cost_h__








struct MapPointData;
#include "MapPoint.h"
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

	
	bool x_wrap;
	bool y_wrap;
	
	
	
	MapPointData xy_pos;				
										
	MapPoint ipos;						
	double *relaxed_min_cost;			




public:



public:

	
	
	
	
	
	A_Star_Heuristic_Cost
	(
		size_t const	a_RowCount,
		size_t const	a_ColumnCount,
		bool const		a_HasYWrap		= false,	// default: earth shape
		bool const		a_HasXWrap		= true
	);
	~A_Star_Heuristic_Cost();
	
	
	
	
	
	
	void Update();
	

	
	
	
	
	
	
	
	void Update_One_Tiles_Cost
	(
		MapPointData &the_tile,			
		double new_cost					
	);
	

	
	
	
	
	
	
	
	
	double Get_Minimum_Nearby_Movement_Cost
	(
		MapPointData const & the_tile			
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
