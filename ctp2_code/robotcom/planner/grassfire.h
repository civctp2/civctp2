










#ifndef __Grasfire_h__
#define __Grasfire_h__

#define BORDER_IS_TARGET 1
#define BORDER_IS_NOT_TARGET 0
#define DISTANCE_INSIDE 1
#define DISTANCE_OUTSIDE 0
#define FOUR_CONNECTED 4
#define EIGHT_CONNECTED 8







template <class DATA_TYPE>
class Grassfire
{
	private:
		int rows;
		int columns;
		int * current_row;
		int * other;
		matrix<int> * first_pass_results;


	public:
		Grassfire
		(
			int i_rows, 
			int i_columns
		);
 
		~Grassfire();

		void Xform
		(
			matrix<DATA_TYPE> * in,
			matrix<int> * out,
			DATA_TYPE target,
			int inside,
			int bkgd,
			int connect,
			int x_wrap,
			int y_wrap
		);

};













template <class DATA_TYPE>
Grassfire<DATA_TYPE>::Grassfire
(
	int i_rows, 
	int i_columns
)
{
	
	rows = i_rows;
	columns = i_columns;

	
	first_pass_results = new matrix<int> (rows, columns);

	
	current_row = new int[columns + 2];
	Assert(current_row);

	other = new int[columns + 2];
	Assert(other);
}













template <class DATA_TYPE>
Grassfire<DATA_TYPE>::~Grassfire()
{
	
	delete first_pass_results;
    first_pass_results=NULL; 

	delete [] current_row;
    current_row = NULL; 

	delete [] other;
    other = NULL; 
}













template <class DATA_TYPE>
void Grassfire<DATA_TYPE>::Xform
(
	matrix<DATA_TYPE> * in,
	matrix<int> * out,
	DATA_TYPE target,
	int inside,
	int bkgd,
	int connect,
	int x_wrap,
	int y_wrap
)
{
	
	int infinity;						
	int y_infinity, x_infinity;			
	int *buftemp, *row_above, *row_below;
	int r,c, min, x;
	int bkgdval;
	bool changed_value_on_horizontal;	
										
	bool changed_value_on_vertical;		
										
	bool crossed_end_column;			
	int this_one_cell;					
	int old_rightmost_value;			
	

	
	
	
	x_infinity = columns - 1;	
	y_infinity = rows-1;

	
	if (y_wrap) y_infinity /= 2;
	if (x_wrap) x_infinity /= 2;

	
	if (connect is EIGHT_CONNECTED)
	{
		
		if (y_infinity > x_infinity) infinity = y_infinity;
		else infinity = x_infinity;
	} 
	else
	{
		
		infinity = x_infinity + y_infinity;
	} 

	
	infinity++;

	
	if (inside)	
		bkgdval = (bkgd) ? infinity : 0;
	else		
		bkgdval = (bkgd) ? 0 : infinity;

	
	for (c=0; c <= columns + 1; c++)  
	  current_row[c] = bkgdval;	

	other[0] = bkgdval;					
	other[columns+1] = bkgdval;

	
	row_above = other;

	
	for (r=0; r<=rows-1; r++) 
	{

		buftemp = row_above;				
		row_above = current_row;				
		current_row = buftemp;				

		
		if (x_wrap)
		{
			row_above[0] = row_above[columns];
			row_above[columns+1] = row_above[1];
		} 

		current_row[0] = bkgdval;			

		
		crossed_end_column = false;

		
		c = 0;

		
		changed_value_on_vertical = false;

		
		while ( (not crossed_end_column) 
			or ( (changed_value_on_horizontal) and (x_wrap)))
		{

			
			
			
			
			
			
			if (inside == ((*in)(r,c) != target)) 
			{
				this_one_cell = 0;
			}
			else 
			{
				
				
				min = current_row[c] + 1;
				x = row_above[c+1] + 1;  
				
				if (x<min) min = x;

				
				if (connect == 8) 
				{
				  x = row_above[c] + 1;  
				  if (x<min) min = x;

				  x = row_above[c+2] + 1;  
				  if (x<min) min = x;
				}
				if (min > infinity)  min = infinity;

				this_one_cell = min;

			}

			
			if (this_one_cell isnt current_row[c+1])
			{
				
				current_row[c+1] = this_one_cell;

				
				changed_value_on_horizontal = true;
				changed_value_on_vertical = true;

			} 
			else 
				changed_value_on_horizontal = false;

			
			(*first_pass_results)(r,c) = current_row[c+1];

			
			c++;

			
			if (c is columns)
			{
				
				crossed_end_column = true;

				
				
				if ((current_row[columns] <current_row[0]) and x_wrap)
				{
					
					current_row[0] = current_row[columns];

					
					c = 0;

					
					changed_value_on_horizontal = true;
				}
				else changed_value_on_horizontal = false;

			} 

		} 

	} 

	
	for (c=0; c<=columns; c++)  current_row[c] = bkgdval;

	
	row_below = row_above;

	
	
	for (r=rows-1; r>=0; --r) 
	{

		buftemp = row_below;				
		row_below = current_row;				
		current_row = buftemp;				
	
		
		if (x_wrap)
		{
			row_below[0] = row_below[columns];
			row_below[columns+1] = row_below[1];
		} 

		
		crossed_end_column = false;

		
		c = columns-1;

		
		changed_value_on_vertical = false;

		
		old_rightmost_value = (*first_pass_results)(r,columns-1);

		
		while ( (not crossed_end_column) 
			or ( (changed_value_on_horizontal) and (x_wrap)))
		{
			
			if ((*first_pass_results)(r,c) == 0) 
			{
				this_one_cell = 0;
			}
			else 
			{
				
				
				min = (*first_pass_results)(r,c);
				x = current_row[c+2] + 1;  if (x<min) min = x;
				x = row_below[c+1] + 1;  if (x<min) min = x;
				
				
				if (connect == 8) 
				{
				  x = row_below[c+2] + 1;  if (x<min) min = x;
				  x = row_below[c] + 1;  if (x<min) min = x;
				}

				if (min > infinity)  min = infinity;
				this_one_cell = min;

			} 


			
			if (this_one_cell isnt current_row[c+1])
			{
				
				current_row[c+1] = this_one_cell;

				
				changed_value_on_horizontal = true;
				changed_value_on_vertical = true;

			} 
			else 
				changed_value_on_horizontal = false;

			(*out)(r,c) = current_row[c+1];

			
			c--;

			
			if (c < 0)
			{
				
				crossed_end_column = true;

				
				if ((x_wrap) and (current_row[1] < current_row[columns+1]))
				{
					
					current_row[columns+1] = current_row[1];

					
					c = columns-1;

					
					changed_value_on_horizontal = true;

				} 
				else 
					changed_value_on_horizontal = false;

			} 

		} 

		
		if ((old_rightmost_value isnt current_row[columns])
			and (current_row[0] > current_row[columns])
			and (x_wrap))
		{
			
			current_row[0] = current_row[columns];

			
			c = 0;

			
			changed_value_on_horizontal = true;

			
			while (changed_value_on_horizontal)
			{
				
				x = current_row[c] + 1;

				
				if (x < current_row[c+1])
				{
					
					current_row[c+1] = x;

					
					(*out)(r,c) = x;

				} 
				else changed_value_on_horizontal = false;
			} 

		} 

	} 


	
	
	
	
	if (y_wrap)
	{
		
		
		for (c=0; c < columns; c++)  
			current_row[c+1] = (*out)(rows-1,c);
		
		
		current_row[0] = current_row[columns];
		current_row[columns+1] = current_row[1];		

		changed_value_on_vertical = true;

		
		for (r=0; (r<=rows-1) and (changed_value_on_vertical); r++) 
		{
			current_row[0] = current_row[columns];
			current_row[columns+1] = current_row[1];

			buftemp = other;				
			other = current_row;				
			current_row = buftemp;				

			changed_value_on_vertical = false;

			
			for (c=0; c<columns; c++) 
			{

				
				min = (*out)(r,c);
				x = other[c+1] + 1;
				
				if (x<min) min = x;

				
				if (connect == 8) 
				{
				  x = other[c] + 1;  
				  if (x<min) min = x;

				  x = other[c+2] + 1;  
				  if (x<min) min = x;
				}
				if (min > infinity)  min = infinity;

				if ((*out)(r,c) isnt min) 
				{
					changed_value_on_vertical = true;
					(*out)(r,c) = min;
				}
				current_row[c+1] = min;

			} 

		} 

		
		
		for (c=0; c < columns; c++)  
			current_row[c+1] = (*out)(0,c);	

		current_row[0] = current_row[columns];
		current_row[columns+1] = current_row[1];

		changed_value_on_vertical = true;

		
		for (r=rows-1; (r>=0) and (changed_value_on_vertical); --r) 
		{

			buftemp = other;				
			other = current_row;				
			current_row = buftemp;				
		
			current_row[0] = current_row[columns];
			current_row[columns+1] = current_row[1];

			changed_value_on_vertical = false;

			for (c=columns-1; c>=0; --c) 
			{
				
				min = (*out)(r,c);
				x = other[c+1] + 1;
				
				if (x<min) min = x;

				
				if (connect == 8) 
				{
				  x = other[c] + 1;  
				  if (x<min) min = x;

				  x = other[c+2] + 1;  
				  if (x<min) min = x;
				}
				if (min > infinity)  min = infinity;

				if ((*out)(r,c) isnt min) 
				{
					changed_value_on_vertical = true;
					(*out)(r,c) = min;
				}
				current_row[c+1] = min;

			} 

		} 

	} 

}












#endif 
