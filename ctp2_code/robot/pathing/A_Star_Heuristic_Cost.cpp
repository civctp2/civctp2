//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Heuristic cost for the A* pathing algorithm
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

#include "c3.h"
#include "c3errors.h"
#include "Globals.h"
#include "DebugMemory.h"
#include <Limits>

#include "semi_dynamic_array.h"

#include "XY_Coordinates.h"
#include "World.h"
#include "RandGen.h"
#include "Cell.h"
#include "A_Star_Heuristic_Cost.h"
























#include <algorithm>

namespace
{

//----------------------------------------------------------------------------
//
// Name       : DivCeil
//
// Description: Divide and round up to nearest integer
//
// Parameters : a_Value			: value to divide
//				a_Divisor		: divisor
//
// Globals    : -
//
// Returns    : Rounded up division result
//
// Remark(s)  : Does not test for division by 0.
//
//----------------------------------------------------------------------------
inline size_t	DivCeil
(
	size_t const	a_Value, 
	size_t const	a_Divisor
)
{
	return (a_Value + a_Divisor - 1) / a_Divisor;
};

//----------------------------------------------------------------------------
//
// Name       : IsInRange
//
// Description: Check
//
// Parameters : a_Test			: value to check
//				a_Begin		    : start of range
//              a_End			: one past end of range
//              a_MayWrap   	: allow wrap around
//
// Globals    : -
//
// Returns    : bool			: test value is valid
//              a_Real			: real value after wrap around - if any
//
// Remark(s)  : Assumes the test value is only slightly out of bounds.
//
//----------------------------------------------------------------------------
inline bool		IsInRange
(
	int	const		a_Test, 
	int const		a_Begin,
	int const		a_End,
	bool const		a_MayWrap,
	int &			a_Real
)
{
	if (a_Test < a_Begin)
	{
		a_Real	= a_Test + a_End;
	}
	else if (a_Test < a_End)
	{
		a_Real	= a_Test;
	}
	else
	{
		a_Real	= a_Test - a_End;
	}

	return a_MayWrap || (a_Test == a_Real);
};

} // namespace

//----------------------------------------------------------------------------
//
// Name       : A_Star_Heuristic_Cost::A_Star_Heuristic_Cost
//
// Description: Constructor
//
// Parameters : a_RowCount		: number of map rows
//				a_ColumnCount	: number of map columns
//				a_HasYWrap		: map wraps at top and bottom
//				a_HasXWrap		: map wraps at sides
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
A_Star_Heuristic_Cost::A_Star_Heuristic_Cost
(
	size_t const	a_RowCount,
	size_t const	a_ColumnCount,
	bool const		a_HasYWrap,
	bool const		a_HasXWrap
)
:	rows(DivCeil(a_RowCount, HEURISTIC_TILES_PER_GRID)),
	columns(DivCeil(2 * a_ColumnCount, HEURISTIC_TILES_PER_GRID)),
	world_rows(a_RowCount),
	world_columns(2 * a_ColumnCount),
	x_wrap(a_HasXWrap),
	y_wrap(a_HasYWrap)
{
	size_t const	blockCount	= rows * columns;

	raw_min_movement_costs		= new double[blockCount];
	relaxed_min_movement_costs	= new double[blockCount];
}

//----------------------------------------------------------------------------
//
// Name       : A_Star_Heuristic_Cost::~A_Star_Heuristic_Cost
//
// Description: Destructor
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
A_Star_Heuristic_Cost::~A_Star_Heuristic_Cost()
{
	delete [] raw_min_movement_costs;
	delete [] relaxed_min_movement_costs;
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
	std::fill(raw_min_movement_costs, 
		      raw_min_movement_costs + (rows * columns),
			  DBL_MAX
			 );
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

		odd_row = !odd_row;

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













//----------------------------------------------------------------------------
//
// Name       : A_Star_Heuristic_Cost::Relax_One_Cost_Grid
//
// Description: Sets a (relaxed) block value to the minimum (raw) value of 
//              itself and its direct neighbours.
//
// Parameters : a_Row			: block row number
//				a_Column		: block column number
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Assumes that input is valid.
//
//----------------------------------------------------------------------------
void A_Star_Heuristic_Cost::Relax_One_Cost_Grid
(
	int		a_Row,
	int		a_Column
)
{
	int const	radius		= 1;	// how direct a neighbour should be
	int const	ownIndex	= a_Column + (a_Row * columns);

	// Initialise with own value (just to have something)
	double &	relaxedMin	= relaxed_min_movement_costs[ownIndex];
	relaxedMin = raw_min_movement_costs[ownIndex];

	for (int i = a_Row - radius; i <= a_Row + radius; ++i)
	{
		int row;
		if (IsInRange(i, 0, rows, y_wrap, row))
		{
			for (int j = a_Column - radius; j <= a_Column + radius; ++j)
			{
				int column;
				if (IsInRange(j, 0, columns, x_wrap, column))
				{
					double const &	neighbourValue	= 
						raw_min_movement_costs[column + (row * columns)];

					if (neighbourValue < relaxedMin)
					{
						relaxedMin = neighbourValue;
					}
				}
			}
		}
	}
}



