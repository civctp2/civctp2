






















#include "c3.h"


#ifndef TEST_XY_COORDS

#include "MapPoint.h"

#endif


#include <math.h>


#ifdef TEST_XY_COORDS


#include <stdio.h>
#include <stdlib.h>

typedef int sint32;

class MapPointData
{
public:
	sint32 x;
	sint32 y;
	sint32 z;
};

#endif

#include "XY_Coordinates.h"








void XY_Coordinates::Init
(
	sint32 i_max_rows,
	sint32 i_max_columns
)
{
	
	max_rows = i_max_rows - 1;
	max_columns = i_max_columns - 1;

	
	num_columns = max_columns + 1;

	
	max_X = (2*num_columns) - 1;

	
	max_Y = max_rows;

	
	num_X_columns = max_X + 1;
}




	










bool XY_Coordinates::RC_to_XY
(
	const MapPointData &RC,
	MapPointData &XY
)
{
	
	XY.x = (sint16)(((2*RC.x) + RC.y) % num_X_columns);

	
	XY.y = RC.y;

	
	return true;
}




	










bool XY_Coordinates::XY_to_RC
(
	const MapPointData &XY,
	MapPointData &RC
)
{
	
	sint32 c1;							
	

	
	c1 = ((XY.x - XY.y)/2) % num_columns;

	
	if (c1 < 0)
		RC.x = (sint16)(num_columns + c1);
	else
		RC.x = (sint16)c1;

	
	RC.y = XY.y;

	
	return true;
}




	










inline void XY_Coordinates::Calculate_Possible_X_Y_Deltas
(
	const MapPointData &A,					
	const MapPointData &B,					
	sint32 &delta_x_unwrapped,			
	sint32 &delta_x_wrapped,			
	sint32 &delta_y_unwrapped,			
	sint32 &delta_y_wrapped				
)
{
	

	
	if (A.x > B.x)
	{
		
		delta_x_unwrapped = A.x - B.x;

		
		
		
		delta_x_wrapped = B.x + (max_X - A.x) + 1; 

	} 
	else
	{
		
		delta_x_unwrapped = B.x - A.x;

		
		
		
		delta_x_wrapped = A.x + (max_X - B.x) + 1; 

	} 

	

	
	if (A.y > B.y)
	{
		
		delta_y_unwrapped = A.y - B.y;

		
		
		
		delta_y_wrapped = B.y + (max_Y - A.y) + 1; 

	} 
	else
	{
		
		delta_y_unwrapped = B.y - A.y;

		
		
		
		delta_y_wrapped = A.y + (max_Y - B.y) + 1; 

	} 


}














sint32 XY_Coordinates::Get_XY_Travel_Distance
(
	const MapPointData &A,
	const MapPointData &B
)
{
	
	sint32 delta_x_unwrapped;			
	sint32 delta_x_wrapped;				
	sint32 delta_y_unwrapped;			
	sint32 delta_y_wrapped;				
	sint32 smallest_x_delta;			
	sint32 smallest_y_delta;			
	sint32 trip_length = 0;				
	

	
	Calculate_Possible_X_Y_Deltas(A, B, delta_x_unwrapped, delta_x_wrapped,
										delta_y_unwrapped, delta_y_wrapped);

	
	if (delta_x_unwrapped < delta_x_wrapped)
		smallest_x_delta = delta_x_unwrapped;
	else
		smallest_x_delta = delta_x_wrapped;

	
	if (delta_y_unwrapped < delta_y_wrapped)
		smallest_y_delta = delta_y_unwrapped;
	else
		smallest_y_delta = delta_y_wrapped;

	
	
	
	
	
	
	if (smallest_x_delta > smallest_y_delta)
	{
		
		trip_length = smallest_y_delta;

		
		
		trip_length += (smallest_x_delta - smallest_y_delta) / 2;

	} 
	else
	
	{
		
		trip_length = smallest_x_delta;

		
		
		trip_length += (smallest_y_delta - smallest_x_delta) / 2;

	} 

	
	return trip_length;

}




	









sint32 XY_Coordinates::Get_RC_Travel_Distance
(
	const MapPointData &A,
	const MapPointData &B
)
{
	
	MapPointData A_XY;						
	MapPointData B_XY;
	sint32 smallest_delta;				
	

	
	RC_to_XY(A, A_XY);
	RC_to_XY(B, B_XY);

	
	smallest_delta = Get_XY_Travel_Distance(A_XY, B_XY);

	
	return smallest_delta;

}




	









double XY_Coordinates::Get_XY_Euclidean_Distance_Squared
(
	const MapPointData &A,
	const MapPointData &B
)
{
	
	sint32 delta_x_unwrapped;			
	sint32 delta_x_wrapped;				
	sint32 delta_y_unwrapped;			
	sint32 delta_y_wrapped;				
	sint32 straight, x_wrap_only, y_wrap_only, both_wrap;	
	double shortest_distance;			
	sint32 t1, t2;						
	

	
	Calculate_Possible_X_Y_Deltas(A, B, delta_x_unwrapped, delta_x_wrapped,
										delta_y_unwrapped, delta_y_wrapped);

	
	straight =  (delta_x_unwrapped * delta_x_unwrapped) +
				(delta_y_unwrapped * delta_y_unwrapped);

	
	x_wrap_only =   (delta_x_wrapped * delta_x_wrapped) +
					(delta_y_unwrapped * delta_y_unwrapped);

	
	y_wrap_only =   (delta_x_unwrapped * delta_x_unwrapped) +
					(delta_y_wrapped * delta_y_wrapped);

	
	both_wrap = (delta_x_wrapped * delta_x_wrapped) +
				(delta_y_wrapped * delta_y_wrapped);

	
	t1 = (straight < x_wrap_only) ? straight : x_wrap_only;
	t2 = (t1 < y_wrap_only) ? t1 : y_wrap_only;
	shortest_distance = (t2 < both_wrap) ? t2 : both_wrap;

	
	
	
	shortest_distance /= 4.0;

	
	return (shortest_distance);
}




	









double XY_Coordinates::Get_RC_Euclidean_Distance_Squared
(
	const MapPointData &A,
	const MapPointData &B
)
{
	
	MapPointData A_XY;						
	MapPointData B_XY;
	double smallest_distance;			
	

	
	RC_to_XY(A, A_XY);
	RC_to_XY(B, B_XY);

	
	smallest_distance = Get_XY_Euclidean_Distance_Squared(A_XY, B_XY);

	
	return smallest_distance;
}














sint32 XY_Coordinates::Get_XY_Euclidean_Distance
(
	const MapPointData &A,
	const MapPointData &B
)
{
	
	double shortest_distance_squared;	
	

	
	shortest_distance_squared = 
		Get_XY_Euclidean_Distance_Squared(A, B);

	
	return (sint32) ((sqrt((double) shortest_distance_squared)) + 0.5);
}




	









sint32 XY_Coordinates::Get_RC_Euclidean_Distance
(
	const MapPointData &A,
	const MapPointData &B
)
{
	
	MapPointData A_XY;						
	MapPointData B_XY;
	sint32 smallest_distance;			
	

	
	RC_to_XY(A, A_XY);
	RC_to_XY(B, B_XY);

	
	smallest_distance = Get_XY_Euclidean_Distance(A_XY, B_XY);

	
	return smallest_distance;
}




	









double XY_Coordinates::Get_Double_XY_Euclidean_Distance
(
	const MapPointData &A,
	const MapPointData &B
)
{
	
	double shortest_distance_squared;	
	

	
	shortest_distance_squared = 
		Get_XY_Euclidean_Distance_Squared(A, B);

	
	return sqrt((double) shortest_distance_squared);

}




	









double XY_Coordinates::Get_Double_RC_Euclidean_Distance
(
	const MapPointData &A,
	const MapPointData &B
)
{
	
	MapPointData A_XY;						
	MapPointData B_XY;
	double smallest_distance;			
	

	
	RC_to_XY(A, A_XY);
	RC_to_XY(B, B_XY);

	
	smallest_distance = Get_Double_XY_Euclidean_Distance(A_XY, B_XY);

	
	return smallest_distance;
}







#ifdef TEST_XY_COORDS







void main()
{
	
	MapPointData A;							
	MapPointData B;
	MapPointData RCA;						
	MapPointData RCB;
	MapPointData XYA;						
	MapPointData XYB;						

	int travel_distance;				
	double euclid_distanced_squared;
	int int_euclid_distance;
	double d_real;
	
	int t = 100;						

	int MAX_C, MAX_R;					

	XY_Coordinates my_coords;			
	


	
	printf("MAX C & R: ");
	scanf("%d %d", &MAX_C, &MAX_R);

	
	my_coords.Init(MAX_R, MAX_C);

	
	
	B.x = 0;
	B.y = 0;

	
	
	while (t != 0)
	{
		
		printf("\n\n**************************\n");
		printf("\nEnter x & y: ");
		scanf("%d %d", &(A.x), &(A.y));
		printf("\nA: (%d, %d)\n\n", A.x, A.y);

		
		travel_distance = my_coords.Get_XY_Travel_Distance(A, B);
		euclid_distanced_squared = my_coords.Get_XY_Euclidean_Distance_Squared(A, B);
		int_euclid_distance = my_coords.Get_XY_Euclidean_Distance(A, B);
		d_real = my_coords.Get_Double_XY_Euclidean_Distance(A, B);

		
		printf("XY: Travel %d  Squared %f  Int %d  Real %f\n\n", travel_distance, euclid_distanced_squared, int_euclid_distance, d_real);

		
		my_coords.XY_to_RC(A, RCA);
		my_coords.XY_to_RC(B, RCB);

		printf("\nRCA: (%d, %d)\n\n", RCA.x, RCA.y);

		
		my_coords.RC_to_XY(RCA, XYA);
		my_coords.RC_to_XY(RCB, XYB);

		printf("\nXYA: (%d, %d)\n\n", XYA.x, XYA.y);

		
		travel_distance = my_coords.Get_RC_Travel_Distance(RCA, RCB);
		euclid_distanced_squared = my_coords.Get_RC_Euclidean_Distance_Squared(RCA, RCB);
		int_euclid_distance = my_coords.Get_RC_Euclidean_Distance(RCA, RCB);
		d_real = my_coords.Get_Double_RC_Euclidean_Distance(RCA, RCB);

		
		printf("RC: Travel %d  Squared %f  Int %d  Real %f\n\n", travel_distance, euclid_distanced_squared, int_euclid_distance, d_real);

		B.x = A.x;
		B.y = A.y;

		printf("Enter 0 to quit (any other num to continue): ");
		scanf("%d", &t);	
	}

}





#endif 
