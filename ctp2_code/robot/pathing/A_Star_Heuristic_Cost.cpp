












#include "c3.h"
#include "c3errors.h"
#include "Globals.h"
#include "DebugMemory.h"
#include <Limits>

#include "semi_dynamic_array.h"

#include "common.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "RandGen.h"
#include "Cell.h"
#include "A_Star_Heuristic_Cost.h"
























void A_Star_Heuristic_Cost::Init
(
	sint32 i_max_rows,
	sint32 i_max_columns,
	bool i_y_wrap
)
{
	
	int bytes_to_allocate;				
	

	
	
	world_rows = i_max_rows;
	world_columns = 2 * i_max_columns;
	rows = (sint32) ceil(world_rows/(double)HEURISTIC_TILES_PER_GRID);
	columns = (sint32) ceil(world_columns/(double)HEURISTIC_TILES_PER_GRID);

	
	bytes_to_allocate = rows * columns * sizeof(double);

	
	raw_min_movement_costs = (double *) MALLOC(bytes_to_allocate);
	relaxed_min_movement_costs = (double *) MALLOC(bytes_to_allocate);

	
	y_wrap = i_y_wrap;

}













void A_Star_Heuristic_Cost::Trash()
{
	
	FREE(raw_min_movement_costs);
	FREE(relaxed_min_movement_costs);
}













void A_Star_Heuristic_Cost::Update()
{
	
	
	Update_Raw_Movement_Costs();

	
	Relax_Raw_Movement_Costs();

}














void A_Star_Heuristic_Cost::Update_One_Tiles_Cost
(
	MapPointData &the_tile,			
	double new_cost					
)
{
}

































































void A_Star_Heuristic_Cost::Clear_Raw_Movement_Costs()
{
	
	int row, column;					
	double * grids_min_tile_cost;		
	

	
	for (row = 0; row < rows; row++)
	{
		
		for (column = 0; column < columns; column++)
		{
			
			grids_min_tile_cost = Get_Raw_Cost_Grid_Pointer(row, column);

			
			
			*grids_min_tile_cost = DBL_MAX;

		} 

	} 
}













void A_Star_Heuristic_Cost::Update_Raw_Movement_Costs()
{
	


	double a_tiles_cost;				

	double *raw_min_cost;				
	int odd_row = true;					
	

	
	Clear_Raw_Movement_Costs();

	
	for (xy_pos.y = 0; xy_pos.y < world_rows; xy_pos.y++)
	{
		
		if (odd_row) xy_pos.x = 0;
		
		else xy_pos.x = 1;

		
		
		for (; xy_pos.x < world_columns; xy_pos.x += 2)
		{
			
			g_theWorld->XY_Coords.XY_to_RC(xy_pos, ipos);

			
			a_tiles_cost = g_theWorld->GetMoveCost(ipos);

			Assert(a_tiles_cost > 0);

			
			raw_min_cost = Get_Raw_Cost_Grid_Pointer(xy_pos);

			
			if (a_tiles_cost < (*raw_min_cost))
			{
				
				(*raw_min_cost) = a_tiles_cost;

			} 

		} 

		
		odd_row = not odd_row;

	} 

}





#ifdef SUPER_DEBUG_HEURISTIC
void A_Star_Heuristic_Cost::test_terrain_costs()
{
	


	double a_tiles_cost;				
	int odd_row = true;					
    MapPoint norm_pos; 
	

	


	
	for (xy_pos.y = 0; xy_pos.y < world_rows; xy_pos.y++)
	{
		
		if (odd_row) xy_pos.x = 0;
		
		else xy_pos.x = 1;

		
		
		for (; xy_pos.x < world_columns; xy_pos.x += 2)
		{
			
			g_theWorld->XY_Coords.XY_to_RC(xy_pos, ipos);

			
			a_tiles_cost = g_theWorld->GetMoveCost(ipos);

			Assert(a_tiles_cost > 0);

			norm_pos.Iso2Norm(ipos); 
			g_theWorld->SetColor(ipos, (long) a_tiles_cost); 

		} 

		
		odd_row = not odd_row;

	} 

















  

}
#endif









void A_Star_Heuristic_Cost::Relax_Raw_Movement_Costs()
{
	
	int row, column;					
	

	
	for (row = 0; row < rows; row++)
	{
		
		for (column = 0; column < columns; column++)
		{
			
			Relax_One_Cost_Grid(row, column);

		} 

	} 
}














void A_Star_Heuristic_Cost::Relax_One_Cost_Grid
(
	int row,
	int column
)
{
	
	double *our_min;					
	double *relaxed_min;				
	double *other_min;					

	
	int row_above, row_below, right_column, left_column;
	

	
	our_min = Get_Raw_Cost_Grid_Pointer(row, column);
	relaxed_min = Get_Relaxed_Cost_Grid_Pointer(row, column);

	
	*relaxed_min = *our_min;

	
	row_above = row - 1;
	row_below = row + 1;
	right_column = column + 1;
	left_column = column - 1;

	
	if ((y_wrap) and (row_above < 0)) row_above = rows - 1;
	if ((y_wrap) and (row_below > rows - 1)) row_below = 0;
	if (left_column < 0) left_column = columns - 1;
	if (right_column > columns - 1) right_column = 0;

	
	if (row_above > 0)
	{
		
		other_min = Get_Raw_Cost_Grid_Pointer(row_above, left_column);
		if (*other_min < *relaxed_min) *relaxed_min = *other_min;

		
		other_min = Get_Raw_Cost_Grid_Pointer(row_above, column);
		if (*other_min < *relaxed_min) *relaxed_min = *other_min;

		
		other_min = Get_Raw_Cost_Grid_Pointer(row_above, right_column);
		if (*other_min < *relaxed_min) *relaxed_min = *other_min;

	} 
	
	
	other_min = Get_Raw_Cost_Grid_Pointer(row, left_column);
	if (*other_min < *relaxed_min) *relaxed_min = *other_min;

	
	other_min = Get_Raw_Cost_Grid_Pointer(row, right_column);
	if (*other_min < *relaxed_min) *relaxed_min = *other_min;

	
	if (row_below < rows - 1)
	{
		
		other_min = Get_Raw_Cost_Grid_Pointer(row_below, left_column);
		if (*other_min < *relaxed_min) *relaxed_min = *other_min;

		
		other_min = Get_Raw_Cost_Grid_Pointer(row_below, column);
		if (*other_min < *relaxed_min) *relaxed_min = *other_min;

		
		other_min = Get_Raw_Cost_Grid_Pointer(row_below, right_column);
		if (*other_min < *relaxed_min) *relaxed_min = *other_min;

	} 
	
}




