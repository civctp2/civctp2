#ifndef __Sorted_Array_h__
#define __Sorted_Array_h__













#define DATA_NOT_FOUND (-1)
#define DATA_FOUND 0







template <class DATA_TYPE, class KEY_TYPE>
class sorted_array
{
	public:

		int count;






		sorted_array
		(
			int i_size,
			int i_chunk_size = -1,
			int i_max_chunks = -1

		);






		~sorted_array();






		void Clear();






		void Empty();






		DATA_TYPE Return_Data_By_Number
		(
			int n
		);











		int Return_Data_By_Unique_Key
		(
			DATA_TYPE &result,
			KEY_TYPE the_key
		);











		int Remove_Data_By_Unique_Key
		(
			KEY_TYPE the_key
		);






		KEY_TYPE Return_Key_By_Number
		(
			int n
		);






		void Delete_Element
		(
			int n
		);







		int Add_Data
		(
			DATA_TYPE the_data,
			KEY_TYPE the_key
		);








		void Make_Room_For_N_Elements
		(
			int n
		);

	protected:

		int chunk_size;
		int size;
		int initial_size;
		int max_chunks;
		int chunks;

		semi_dynamic_array<DATA_TYPE> data_list;

		semi_dynamic_array<KEY_TYPE> key_list;










};












template <class DATA_TYPE, class KEY_TYPE>
sorted_array<DATA_TYPE, KEY_TYPE>::sorted_array
(
	int i_size,
	int i_chunk_size,
	int i_max_chunks

) : data_list(i_size), key_list(i_size)
{

	if (i_chunk_size is -1) i_chunk_size = i_size;

	chunk_size = i_chunk_size;
	max_chunks = i_max_chunks;

	initial_size = i_size;

	size = i_size;

	chunks = 0;

	count = 0;

}













template <class DATA_TYPE, class KEY_TYPE>
sorted_array<DATA_TYPE, KEY_TYPE>::~sorted_array()
{
}












template <class DATA_TYPE, class KEY_TYPE>
void sorted_array<DATA_TYPE, KEY_TYPE>::Clear()
{

	count = 0;
}












template <class DATA_TYPE, class KEY_TYPE>
void sorted_array<DATA_TYPE, KEY_TYPE>::Empty()
{

	Clear();
}












template <class DATA_TYPE, class KEY_TYPE>
DATA_TYPE sorted_array<DATA_TYPE, KEY_TYPE>::Return_Data_By_Number
(
	int n
)
{

	return(data_list[n]);
}

















template <class DATA_TYPE, class KEY_TYPE>
int sorted_array<DATA_TYPE, KEY_TYPE>::Return_Data_By_Unique_Key
(
	DATA_TYPE &result,
	KEY_TYPE the_key
)
{

	int pos, low, high;


	if (count < 1)
	{
		return DATA_NOT_FOUND;

	}

	low = 0; high = count-1;

#ifdef _DEBUG
    sint32 finite_loop=0;
#endif

	while (high - low >= 0)
	{
Assert(++finite_loop < 100000);

		pos = (high + low)/2;

		if (the_key < key_list[pos])
		{

			high = pos-1;
		}
		else
		{

			low = pos+1;
		}
	}

	pos = (high + low)/2;

	if (the_key == key_list[pos])
	{

		result = data_list[pos];

		return DATA_FOUND;

	}

	return DATA_NOT_FOUND;
}

















template <class DATA_TYPE, class KEY_TYPE>
int sorted_array<DATA_TYPE, KEY_TYPE>::Remove_Data_By_Unique_Key
(
	KEY_TYPE the_key
)
{

	int pos, low, high;


	if (count < 1)
	{
		return DATA_NOT_FOUND;

	}

	low = 0; high = count-1;

#ifdef _DEBUG
    sint32 finite_loop=0;
#endif

	while (high - low >= 0)
	{
Assert(++finite_loop < 100000);

		pos = (high + low)/2;

		if (the_key < key_list[pos])
		{

			high = pos-1;
		}
		else
		{

			low = pos+1;
		}
	}

	pos = (high + low)/2;

	if (the_key == key_list[pos])
	{

		Delete_Element(pos);

		return DATA_FOUND;

	}

	Assert(the_key == key_list[pos]);

	return DATA_NOT_FOUND;
}












template <class DATA_TYPE, class KEY_TYPE>
KEY_TYPE sorted_array<DATA_TYPE, KEY_TYPE>::Return_Key_By_Number
(
	int n
)
{

	return(key_list[n]);
}












template <class DATA_TYPE, class KEY_TYPE>
void sorted_array<DATA_TYPE, KEY_TYPE>::Delete_Element
(
	int n
)
{

	unsigned int element_count = (count - 1) - n;


	if (element_count > 0)
	{
		unsigned int byte_count;
		unsigned int data_size = sizeof(DATA_TYPE);
		unsigned int key_size = sizeof(KEY_TYPE);

		byte_count = element_count * data_size;

		memmove(&(data_list[n]), &(data_list[n+1]), byte_count);

		byte_count = element_count * key_size;

		memmove(&(key_list[n]), &(key_list[n+1]), byte_count);

	}












	count--;





	if ((count < (initial_size + ((chunks - 1) * chunk_size)))
		and (chunks >= 2))
	{

		chunks--;

		size -= chunk_size;

		data_list.Resize(size);

		key_list.Resize(size);

	}
}













template <class DATA_TYPE, class KEY_TYPE>
int sorted_array<DATA_TYPE, KEY_TYPE>::Add_Data
(
	DATA_TYPE the_data,
	KEY_TYPE the_key
)
{

	int pos, low, high;




	if (count >= size)
	{

		if ((chunks >= max_chunks) and (max_chunks isnt -1))
		{

			_ASSERTE(chunks + 1 <= max_chunks);

			return(-1);
		}
		else
		{

			chunks++;

			size += chunk_size;

			data_list.Resize(size);

			key_list.Resize(size);

		}

	}

	low = 0; high = count-1;

#ifdef _DEBUG
    sint32 finite_loop=0;
#endif

	while (high - low >= 0)
	{
Assert(++finite_loop < 100000);

		pos = (high + low)/2;

		if (the_key < key_list[pos])
		{

			high = pos-1;
		}
		else
		{

			low = pos+1;
		}
	}














	unsigned int element_count = count - low;

	if (element_count > 0)
	{
		unsigned int byte_count;
		unsigned int data_size = sizeof(DATA_TYPE);
		unsigned int key_size = sizeof(KEY_TYPE);

		byte_count = element_count * data_size;

		memmove(&(data_list[low+1]), &(data_list[low]), byte_count);

		byte_count = element_count * key_size;

		memmove(&(key_list[low+1]), &(key_list[low]), byte_count);

	}

	key_list[low] = the_key;
	data_list[low] = the_data;
	count++;

	return(low);
}













template <class DATA_TYPE, class KEY_TYPE>
void sorted_array<DATA_TYPE, KEY_TYPE>::Make_Room_For_N_Elements
(
	int n
)
{

	int new_chunks_needed;
	int slots_needed;


	if (n >= size)
	{

		slots_needed = n - size;

		new_chunks_needed = (int) ceil((double)slots_needed/ (double) chunk_size);

		if ((chunks+new_chunks_needed > max_chunks) and (max_chunks isnt -1))
		{

			_ASSERTE(chunks+new_chunks_needed <= max_chunks);

			return;
		}

		else
		{

			chunks += new_chunks_needed;

			size += (chunk_size*new_chunks_needed);

			data_list.Resize(size);

			key_list.Resize(size);

		}

	}

}







#endif __Sorted_Array_h__
