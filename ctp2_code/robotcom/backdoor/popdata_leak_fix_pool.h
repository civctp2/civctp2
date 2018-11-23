#ifndef __PopData_Leak_Fix_Pool_h__
#define __PopData_Leak_Fix_Pool_h__

#include "common.h"
#include "list_array.h"

template <class DATA_TYPE> class list_array;


#define INITIAL_CHUNK_LIST_SIZE (30)








template <class DATA_TYPE>
class PopData_Leak_Fix_Pool
{
	public:






		PopData_Leak_Fix_Pool
		(
			int i_chunk_size,
			int i_max_chunks = -1

		);







		~PopData_Leak_Fix_Pool();








		DATA_TYPE * Get_Next_Pointer(int & which_element_is_it);







		void Release_Pointer
		(
			int ptr_to_free
		);






		void Clear();


	protected:







		DATA_TYPE * Get_Nth_Pointer
		(
			int n
		);









		bool Prepare_New_Chunk();

		int chunk_size;
		int max_chunks;
		int count;
		int next_element;


		list_array<DATA_TYPE *> * chunks;

		list_array<int> *next_free_element_list;

};















template <class DATA_TYPE>
bool PopData_Leak_Fix_Pool<DATA_TYPE>::Prepare_New_Chunk()
{

	DATA_TYPE *new_chunk;
	int bytes_to_allocate;
	int size_of_data_type;
	int i;
	int first_new_element;


	if (chunks->count is max_chunks) return false;

	size_of_data_type = sizeof(DATA_TYPE);
	bytes_to_allocate = size_of_data_type * chunk_size;

	new_chunk = (DATA_TYPE *) malloc(bytes_to_allocate);

	chunks->Append_Data(new_chunk);

	first_new_element = (chunks->count - 1) * chunk_size;

	for (i = first_new_element; i < first_new_element + chunk_size; i++)
	{

		next_free_element_list->Append_Data(i+1);

	}

	return true;
}












template <class DATA_TYPE>
PopData_Leak_Fix_Pool<DATA_TYPE>::PopData_Leak_Fix_Pool
(
	int i_chunk_size,
	int i_max_chunks

)
{
	_ASSERTE(i_chunk_size > 0);

	chunk_size = i_chunk_size;
	max_chunks = i_max_chunks;

	count = 0;

	chunks = new list_array<DATA_TYPE *> (INITIAL_CHUNK_LIST_SIZE);

	next_free_element_list =
		new list_array<int>(chunk_size, chunk_size, max_chunks);

	next_element = 0;

	Prepare_New_Chunk();

}












template <class DATA_TYPE>
PopData_Leak_Fix_Pool<DATA_TYPE>::~PopData_Leak_Fix_Pool()
{

	DATA_TYPE *bad_chunk;
	int i;


	for (i = 0; i < chunks->count; i++)
	{

		bad_chunk = chunks->Return_Data_By_Number(i);

		free( bad_chunk );
	}

	delete chunks;

	delete next_free_element_list;

}













template <class DATA_TYPE>
DATA_TYPE * PopData_Leak_Fix_Pool<DATA_TYPE>::Get_Next_Pointer(int & which_element_is_it)
{

	DATA_TYPE *the_ptr;
	DATA_TYPE *the_chunk;
	int which_chunk;
	int which_element_in_chunk;


	count++;

	if (count > (chunks->count * chunk_size))
	{

		if (not Prepare_New_Chunk())
		{
			which_element_is_it = -1;
			return NULL;
		}
	}

	which_element_is_it = next_element;

	which_chunk = which_element_is_it/chunk_size;
	which_element_in_chunk = which_element_is_it - (which_chunk * chunk_size);

	the_chunk = chunks->Return_Data_By_Number(which_chunk);

	the_ptr = &(the_chunk[which_element_in_chunk]);

	next_element = next_free_element_list->Return_Data_By_Number(next_element);


	next_free_element_list->Set_Data(-1, which_element_is_it);

#ifdef _DEBUG

#endif

	return the_ptr;
}












template <class DATA_TYPE>
void PopData_Leak_Fix_Pool<DATA_TYPE>::Release_Pointer
(
	int ptr_to_free
)
{

#ifdef _DEBUG

#endif


	_ASSERTE(next_free_element_list->Return_Data_By_Number(ptr_to_free) is -1);

#ifdef _DEBUG





#endif

	next_free_element_list->Set_Data(next_element, ptr_to_free);

	next_element = ptr_to_free;

	count--;

}












template <class DATA_TYPE>
void PopData_Leak_Fix_Pool<DATA_TYPE>::Clear()
{

	int i;
	int first_new_element;


	for (i = 0; i < (chunks->count * chunk_size); i++)
	{

		next_free_element_list->Set_Data(i+1, i);
	}

	count = 0;

	next_element = 0;
}












template <class DATA_TYPE>
DATA_TYPE * PopData_Leak_Fix_Pool<DATA_TYPE>::Get_Nth_Pointer
(
	int n
)
{

	DATA_TYPE *the_ptr;
	DATA_TYPE *the_chunk;
	int which_chunk;
	int which_element_in_chunk;


	which_chunk = n/chunk_size;
	which_element_in_chunk = n - (which_chunk * chunk_size);

	the_chunk = chunks->Return_Data_By_Number(which_chunk);

	the_ptr = &(the_chunk[which_element_in_chunk]);

	return the_ptr;
}







#endif __PopData_Leak_Fix_Pool_h__
