









#ifndef __List_Array_h__
#define __List_Array_h__


#include "semi_dynamic_array.h"




















template <class DATA_TYPE>
class list_array
{
	public:

		
		int count;

		
		
		
		
		
		list_array
		(
			int i_size,					
			int i_chunk_size = -1,	
			int i_max_chunks = -1		
										
		);

		
		
		
		
		
		list_array
		(
            int r_count,
			int r_size,
            char *r_data,
			int i_chunk_size = -1,	
			int i_max_chunks = -1		
										
		);

		
		
		
		
		
		~list_array();

		
		
		
		
		
		void Clear();

		
		
		
		
		
		void Empty();

		
		
		
		
        
		void Get_Raw
        (
            int *r_count,
            int *r_size,                
            char **r_data               
        );

		
		
		
		
		
		inline DATA_TYPE Return_Data_By_Number
		(
			int n
		)
		{
			
			return(data_list[n]);
		}


		
		
		
		
		
		void Delete_Element
		(
			int n
		);

		
		
		
		
		
		void Set_Count
		(
			int n
		);

		
		
		
		
		
		list_array& operator=
		(
			list_array<DATA_TYPE>& the_list
		);

		
		
		
		
		
		
		int Add_Data
		(
			DATA_TYPE the_data
		);

		
		
		
		
		
		
		int Append_Data
		(
			DATA_TYPE the_data
		);

		
		
		
		
		
		
		void Set_Data
		(
			DATA_TYPE the_data,
			int n
		);

		
		
		
		
		
		
		int Get_Max_Size() { return size;}




	protected:

		int chunk_size;					
		int size;						
		int initial_size;
		int max_chunks;					
		int chunks;						

		
		semi_dynamic_array<DATA_TYPE> data_list;

		
		
		
		
		
		void Assign
		(
			list_array<DATA_TYPE>& the_list
		);

		
		
		
		
		
		void Check_Size
		(
			int desired_new_count		
		);

};












template <class DATA_TYPE>
list_array<DATA_TYPE>::list_array
(
	int i_size,					
	int i_chunk_size,			
	int i_max_chunks			
								
) : data_list(i_size)
{
	
	if (i_chunk_size is -1) i_chunk_size = i_size;

	
	chunk_size = i_chunk_size;
	max_chunks = i_max_chunks;

	
	initial_size = i_size;

	
	size = i_size;

	
	chunks = 0;

	
	count = 0;

}












template <class DATA_TYPE>
list_array<DATA_TYPE>::list_array
(
    int r_count,
    int r_size,
    char *r_data,
	int i_chunk_size,			
	int i_max_chunks			
								
) : data_list(r_size, r_data)
{
    int i_size = r_size / sizeof(DATA_TYPE);

    
	if (i_chunk_size is -1) i_chunk_size = i_size;

	
	chunk_size = i_chunk_size;
	max_chunks = i_max_chunks;

	
	initial_size = i_size;

	
	size = i_size;

	
	chunks = 0;

	count = r_count;

}













template <class DATA_TYPE>
list_array<DATA_TYPE>::~list_array() 
{
}












template <class DATA_TYPE>
void list_array<DATA_TYPE>::Clear() 
{
	
	count = 0;
}












template <class DATA_TYPE>
void list_array<DATA_TYPE>::Empty() 
{
	
	count = 0;
}












template <class DATA_TYPE>
void list_array<DATA_TYPE>::Get_Raw
(
    int *r_count,
    int *r_size,
    char **r_data
) 
{
	
	data_list.Get_Raw(r_size, r_data);
    *r_count = count;
    return;
}

































template <class DATA_TYPE>
void list_array<DATA_TYPE>::Delete_Element
(
	int n
) 
{
	
	int i;
	

	for (i = n; i < count-1; i++)
	{
		data_list[i] = data_list[i+1];
	}

	
	count--;

	
	
	
	
	if ((count < (initial_size + ((chunks - 1) * chunk_size)))
		and (chunks >= 2))
	{
		
		chunks--;

		
		size -= chunk_size;

		
		data_list.Resize(size);

	} 
}












template <class DATA_TYPE>
void list_array<DATA_TYPE>::Set_Count
(
	int n
) 
{
	
	

	
	Check_Size(n);

	
	count = n;
}












template <class DATA_TYPE>
list_array<DATA_TYPE>& list_array<DATA_TYPE>::operator=
(
	list_array<DATA_TYPE>& the_list
)
{
	
	chunk_size = the_list.chunk_size;
	max_chunks = the_list.max_chunks;
	chunks = the_list.chunks;

	
	Assign(the_list);

	return this;
}













template <class DATA_TYPE>
int list_array<DATA_TYPE>::Add_Data
(
	DATA_TYPE the_data
)
{
	
	int i;								
	

	
	Check_Size(count+1);

	
	for (i = count - 1; i >= 0; i--)
	{
		
		data_list[i+1] = data_list[i];

	} 

	
	data_list[0] = the_data;

	
	count++;

	
	return 0;

}













template <class DATA_TYPE>
int list_array<DATA_TYPE>::Append_Data
(
	DATA_TYPE the_data
)
{
	
	

	
	Check_Size(count+1);

	
	data_list[count++] = the_data;

	
	return (count - 1);

}













template <class DATA_TYPE>
void list_array<DATA_TYPE>::Set_Data
(
	DATA_TYPE the_data,
	int n
)
{
	
	

	
	_ASSERTE(n < size);

	
	data_list[n] = the_data;

}












template <class DATA_TYPE>
void list_array<DATA_TYPE>::Assign
(
	list_array<DATA_TYPE>& the_list
)
{
	
	

	
	Clean_Up();

	
	size = the_list.size;

	
	data_list = the_list.data_list;


}












template <class DATA_TYPE>
void list_array<DATA_TYPE>::Check_Size		
(
	int desired_new_count			
)

{
	
	int needed_chunks;					
	int extra_chunks;					
	

    _ASSERTE(0 < chunk_size); 
	
	if (desired_new_count > size)
	{
		
		needed_chunks = (desired_new_count - initial_size + chunk_size - 1) 
						/ chunk_size;

		
		if ((needed_chunks >= max_chunks) and (max_chunks isnt -1))
		{
			
			_ASSERTE(needed_chunks < max_chunks);

			return;
		} 
		else 
		{
			
			extra_chunks = needed_chunks - chunks;

			
			if (extra_chunks < 1) return;

			
			chunks = needed_chunks;

			
			size += chunk_size*extra_chunks;

			
			data_list.Resize(size);

		} 

	} 

}







#endif __List_Array_h__


