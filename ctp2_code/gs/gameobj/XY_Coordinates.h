
#ifndef __XY_Coordinates_h__
#define __XY_Coordinates_h__

struct MapPointData;









class XY_Coordinates
{


private:
	sint32 max_rows;					
	sint32 max_columns;
	sint32 num_columns;					

	sint32 max_X;						
	sint32 max_Y;						
	sint32 num_X_columns;				



public:

	

private:

	
	
	
	
	
	
	
	
	inline void Calculate_Possible_X_Y_Deltas
	(
		const MapPointData &A,					
		const MapPointData &B,					
		sint32 &delta_x_unwrapped,			
		sint32 &delta_x_wrapped,			
		sint32 &delta_y_unwrapped,			
		sint32 &delta_y_wrapped				
	);




public:

	
	
	
	
	
	
	void Init
	(
		sint32 i_max_rows,
		sint32 i_max_columns
	);
		


	
	
	
	
	
	
	
	
	bool RC_to_XY
	(
		const MapPointData &RC,
		MapPointData &XY
	);
		


	
	
	
	
	
	
	
	
	bool XY_to_RC
	(
		const MapPointData &XY,
		MapPointData &RC
	);
		


	
	
	
	
	
	
	
	sint32 Get_XY_Travel_Distance
	(
		const MapPointData &A,
		const MapPointData &B
	);
		

	
	
	
	
	
	
	
	sint32 Get_RC_Travel_Distance
	(
		const MapPointData &A,
		const MapPointData &B
	);
		

	
	
	
	
	
	
	
	double Get_XY_Euclidean_Distance_Squared
	(
		const MapPointData &A,
		const MapPointData &B
	);


	
	
	
	
	
	
	
	double Get_RC_Euclidean_Distance_Squared
	(
		const MapPointData &A,
		const MapPointData &B
	);


	
	
	
	
	
	
	
	sint32 Get_XY_Euclidean_Distance
	(
		const MapPointData &A,
		const MapPointData &B
	);
		

	
	
	
	
	
	
	
	sint32 Get_RC_Euclidean_Distance
	(
		const MapPointData &A,
		const MapPointData &B
	);
		

	
	
	
	
	
	
	
	double Get_Double_XY_Euclidean_Distance
	(
		const MapPointData &A,
		const MapPointData &B
	);

	
	
	
	
	
	
	
	
	double Get_Double_RC_Euclidean_Distance
	(
		const MapPointData &A,
		const MapPointData &B
	);

	

};





#endif 
