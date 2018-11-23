//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  :
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
// None.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Replaced semi_dynamic_array with std::vector and removed unused stuff.
//
//----------------------------------------------------------------------------

#ifndef __List_Array_h__
#define __List_Array_h__

#include <vector>       // std::vector

template <typename LIST_DATA_TYPE>
class list_array
{
public:
	list_array
	(
		int i_size,
		int i_chunk_size = -1,
		int i_max_chunks = -1
	);

    ~list_array();

    void Clear()
    {
        data_list.clear();
    };

    size_t size() const
    {
        return data_list.size();
    };

    LIST_DATA_TYPE Return_Data_By_Number
	(
		int n
	) const
	{
		return data_list[n];
	};

	list_array & operator =
    (
		list_array<LIST_DATA_TYPE> const & the_list
	);

	int Append_Data
	(
		LIST_DATA_TYPE the_data
	);

	void Set_Data
	(
		LIST_DATA_TYPE the_data,
		int n
	);

protected:
	int chunk_size;
	int initial_size;
	int max_chunks;
	int chunks;

    std::vector<LIST_DATA_TYPE> data_list;
};

template <typename LIST_DATA_TYPE>
list_array<LIST_DATA_TYPE>::list_array
(
	int i_size,
	int i_chunk_size,
	int i_max_chunks

)
:   chunk_size      ((i_chunk_size == -1) ? i_size : i_chunk_size),
    initial_size    (i_size),
    max_chunks      (i_max_chunks),
    chunks          (0),
    data_list       ()
{ ; }

template <typename LIST_DATA_TYPE>
list_array<LIST_DATA_TYPE>::~list_array()
{
    std::vector<LIST_DATA_TYPE>().swap(data_list);
}

template <typename LIST_DATA_TYPE>
list_array<LIST_DATA_TYPE> & list_array<LIST_DATA_TYPE>::operator =
(
	list_array<LIST_DATA_TYPE> const & the_list
)
{
    if (this != &the_list)
    {
        chunk_size  = the_list.chunk_size;
        max_chunks  = the_list.max_chunks;
        chunks      = the_list.chunks;
        data_list   = the_list.data_list;
    }
    return *this;
}

template <typename LIST_DATA_TYPE>
int list_array<LIST_DATA_TYPE>::Append_Data
(
	LIST_DATA_TYPE the_data
)
{
    size_t const    old_size   = data_list.size();

    if (old_size < data_list.capacity())
    {
        // No action: space available
    }
    else
    {
        _ASSERTE(0 < chunk_size);

		size_t const    needed_chunks =
            (old_size - initial_size + chunk_size) / chunk_size;

		if ((static_cast<int>(needed_chunks) >= max_chunks) && (max_chunks >= 0))
		{
			_ASSERTE(needed_chunks < static_cast<size_t>(max_chunks));
		}
		else
		{
    	    size_t const    extra_chunks = needed_chunks - chunks;

			if (extra_chunks >= 1)
            {
    			chunks = needed_chunks;
                data_list.reserve(data_list.capacity() + (chunk_size * extra_chunks));
            }
		}
	}

	data_list.push_back(the_data);

    return data_list.size() - 1;
}

template <typename LIST_DATA_TYPE>
void list_array<LIST_DATA_TYPE>::Set_Data
(
	LIST_DATA_TYPE the_data,
	int n
)
{
	_ASSERTE((n >= 0) && (static_cast<size_t>(n) < data_list.size()));
	data_list[n] = the_data;
}

#endif // __List_Array_h__
