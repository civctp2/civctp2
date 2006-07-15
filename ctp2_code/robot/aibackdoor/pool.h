//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : General pool handling
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
// HAVE_PRAGMA_ONCE
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __Pool_h__
#define __Pool_h__

//----------------------------------------------------------------------------
// Library imports
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Exported names
//----------------------------------------------------------------------------

template <class DATA_TYPE> class Pool;

//----------------------------------------------------------------------------
// Project imports
//----------------------------------------------------------------------------

#include "list_array.h"

//----------------------------------------------------------------------------
// General declarations
//----------------------------------------------------------------------------

int const   ELEMENT_OCCUPIED            = -1;
int const   INITIAL_CHUNK_LIST_SIZE     = 30;

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

template <class DATA_TYPE>
class Pool
{
public:
	Pool
	(
		int i_chunk_size,			
		int i_max_chunks = -1		
	);

	~Pool();

	DATA_TYPE * Get_Next_Pointer(int & which_element_is_it);

	void Release_Pointer
	(
		int ptr_to_free
	);

protected:

	bool Prepare_New_Chunk();

	int chunk_size;					
	int max_chunks;					
	int count;						
	int next_element;				
									
	list_array<DATA_TYPE *> chunks;
	list_array<int>         next_free_element_list;
};



template <class DATA_TYPE>
bool Pool<DATA_TYPE>::Prepare_New_Chunk()
{
	if (static_cast<int>(chunks.size()) == max_chunks) 
    {
        return false;
    }
    else
    {
        DATA_TYPE *     test                = new DATA_TYPE[chunk_size];

	    chunks.Append_Data(test);
	    
	    size_t const    first_new_element   = (chunks.size() - 1) * chunk_size;

        for (size_t i = first_new_element + 1; i <= first_new_element + chunk_size; ++i)
	    {
		    next_free_element_list.Append_Data(i);
	    } 

	    return true;
    }
}


template <class DATA_TYPE>
Pool<DATA_TYPE>::Pool
(
	int i_chunk_size,					
	int i_max_chunks					
)
:   count                   (0),
    chunks                  (INITIAL_CHUNK_LIST_SIZE),
    chunk_size              (i_chunk_size),
    max_chunks              (i_max_chunks),
    next_element            (0),
    next_free_element_list  (i_chunk_size, i_chunk_size, i_max_chunks)
{
	_ASSERTE(chunk_size > 0);
	
	Prepare_New_Chunk();
}


template <class DATA_TYPE>
Pool<DATA_TYPE>::~Pool() 
{
	for (size_t i = 0; i < chunks.size(); ++i)
	{
        DATA_TYPE * bad_chunk = chunks.Return_Data_By_Number(i);
        delete [] bad_chunk;
	}
}


template <class DATA_TYPE>
DATA_TYPE * Pool<DATA_TYPE>::Get_Next_Pointer(int & which_element_is_it) 
{
	if (count >= (static_cast<int>(chunks.size()) * chunk_size))
	{
		if (!Prepare_New_Chunk())
		{
			which_element_is_it = -1;
			return NULL;
		}
	}

	++count;
	which_element_is_it = next_element;

	int const   which_chunk         = which_element_is_it / chunk_size;
	int const   element_in_chunk    = which_element_is_it - (which_chunk * chunk_size);
	DATA_TYPE * the_chunk           = chunks.Return_Data_By_Number(which_chunk);
	
	next_element = next_free_element_list.Return_Data_By_Number(next_element);
	next_free_element_list.Set_Data(ELEMENT_OCCUPIED, which_element_is_it);

	return &(the_chunk[element_in_chunk]);
}


template <class DATA_TYPE>
void Pool<DATA_TYPE>::Release_Pointer
(
	int ptr_to_free
) 
{
	if (ELEMENT_OCCUPIED == next_free_element_list.Return_Data_By_Number
                                (ptr_to_free)
       )
    {
	    next_free_element_list.Set_Data(next_element, ptr_to_free);
	    next_element = ptr_to_free;

        Assert(count > 0);
        --count;
    }
    // else No action: equivalent of delete NULL;
}

#endif // __Pool_h__


