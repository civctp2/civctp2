












#ifndef __Pointer_Pool_h__
#define __Pointer_Pool_h__

#include "common.h"
#include "list_array.h"

template <class DATA_TYPE> class list_array;



#define INITIAL_CHUNK_LIST_SIZE (30)








template <class DATA_TYPE>
class Pointer_Pool
{
	public:

		
		
		
		
		
		Pointer_Pool
		(
			int i_chunk_size,			
			int i_max_chunks = -1		
										
		);


		
		
		
		
		
		~Pointer_Pool();


		
		
		
		
		
		
		DATA_TYPE * Get_Next_Pointer();

		
		
		
		
		
		
		void Release_Pointer
		(
			DATA_TYPE * ptr_to_free
		);


	protected:

		
		
		
		
		
		
		
		
		bool Prepare_New_Chunk();

		int chunk_size;					
		int max_chunks;					
		int count;						

		DATA_TYPE * next_element;		
										

		
		list_array<DATA_TYPE *> * chunks;

};















template <class DATA_TYPE>
bool Pointer_Pool<DATA_TYPE>::Prepare_New_Chunk()
{
	
	DATA_TYPE *new_chunk;				
	int i;								
	DATA_TYPE *next_ptr_in_list;		
	

	
	if (chunks->count is max_chunks) return false;

	
	new_chunk = new DATA_TYPE[chunk_size];

	
	chunks->Append_Data(new_chunk);

	
	next_ptr_in_list = next_element;

	
	for (i = 0; i < chunk_size; i++)
	{
		
		next_element = &(new_chunk[i]);

		
		*((DATA_TYPE **) next_element) = next_ptr_in_list;

		
		next_ptr_in_list = next_element;

	} 

	
	return true;
}












template <class DATA_TYPE>
Pointer_Pool<DATA_TYPE>::Pointer_Pool
(
	int i_chunk_size,					
	int i_max_chunks					
										
)
{
	
	

	
	_ASSERTE(i_chunk_size > 0);

	
	_ASSERTE(sizeof(DATA_TYPE) >= 4);

	
	chunk_size = i_chunk_size;
	max_chunks = i_max_chunks;

	
	count = 0;

	
	chunks = new list_array<DATA_TYPE *> (INITIAL_CHUNK_LIST_SIZE);
	
	
	next_element = NULL;

	
	Prepare_New_Chunk();

}












template <class DATA_TYPE>
Pointer_Pool<DATA_TYPE>::~Pointer_Pool() 
{
	
	DATA_TYPE *bad_chunk;				
	int i;								
	

	
	for (i = 0; i < chunks->count; i++)
	{
		
		bad_chunk = chunks->Return_Data_By_Number(i);

		
		delete[] bad_chunk;
        bad_chunk = NULL;
	}

	
	delete chunks;
    chunks = NULL; 

}













template <class DATA_TYPE>
DATA_TYPE * Pointer_Pool<DATA_TYPE>::Get_Next_Pointer() 
{
	
	DATA_TYPE *the_new_pointer;			
	

	
	count++;

	
	if (count > (chunks->count * chunk_size))
	{
		
		if (not Prepare_New_Chunk())
		{
			return NULL;
		}
	}

	
	the_new_pointer = next_element;

	
	
	next_element = *((DATA_TYPE **) the_new_pointer);

#ifdef _DEBUG
	

#endif

	
	return the_new_pointer;
}












template <class DATA_TYPE>
void Pointer_Pool<DATA_TYPE>::Release_Pointer
(
	DATA_TYPE * ptr_to_free
) 
{
	
#ifdef _DEBUG

#endif
	

	
	_ASSERTE(ptr_to_free != NULL);

#ifdef _DEBUG
	


	

#endif

	
	*((DATA_TYPE **) ptr_to_free) = next_element;

	
	next_element = ptr_to_free;

	
	count--;

}







#endif __Pointer_Pool_h__


