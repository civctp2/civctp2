


















































#include "c3.h"
#include "c3types.h"
#include "c3errors.h"


#include <string.h>

#include "common.h"

#include "linked_list.h"








list_cell::list_cell()
{
	cell_data = NULL;
	next = NULL;
	prev = NULL;
}












list_cell::~list_cell()
{
    cell_data = NULL;
	next = NULL;
	prev = NULL;
}












void linked_list::Init()
{

	
	

	
	count = 0;    
	head = NULL;
	tail = NULL;
	last_cell_accessed = NULL;  

	
	
	last_number_accessed = -999;

}












linked_list::linked_list()
{

	
	


	
	Init();

}




















void linked_list::Empty()
{
	
	int i;
	list_cell_ptr current_cell;
	

	for (i = 0; i < count; i++ ) 
		if (head != NULL)
		{

			current_cell = head;

			head = head->next;

			delete current_cell;
            current_cell = NULL; 
		}

	count = 0;

	head = NULL;
	tail = NULL;


	
	
	last_cell_accessed = NULL;  
	last_number_accessed = -999;
}












linked_list::~linked_list()
{

	
	Empty();

}












list_cell_ptr linked_list::Add_Data
(
	data_ptr new_data
)
{
    Assert(new_data); 

	
	list_cell_ptr current_cell;
	

	
	current_cell = new list_cell();

	
	current_cell->cell_data = new_data;

	
	current_cell->prev = NULL;
	current_cell->next = head;
	head = current_cell;

	
	if (tail is NULL)
	{
		tail = current_cell;
	}

	
	
	last_number_accessed = -999;

	
	if (head->next != NULL)
		head->next->prev = head;

	count++;

	
	
	return(current_cell);

}












list_cell_ptr linked_list::Append_Data
(
	data_ptr new_data
)
{
    Assert(new_data); 

	
	list_cell_ptr current_cell;
	

	
	current_cell = new list_cell;

	
	current_cell->cell_data = new_data;

	
	current_cell->prev = tail;
	current_cell->next = NULL;
	tail = current_cell;

	
	if (head is NULL)
	{
		head = current_cell;
	}

	
	
	last_number_accessed = -999;

	
	if (current_cell->prev != NULL)
		current_cell->prev->next = tail;

	count++;

	
	
	return(current_cell);
}











void linked_list::Remove_Cell
(
	list_cell_ptr dead_cell
)
{
    Assert(dead_cell); 

	
	

	

	
	
	last_number_accessed = -999;
	last_cell_accessed = NULL;

	
	if (dead_cell == head) 
	{

		
		if (tail == dead_cell)
		{
			tail = NULL;
		}

		head = dead_cell->next;

		if (head != NULL)
			head->prev = NULL;
	}
	
	else if (dead_cell == tail) 
	{
		tail = dead_cell->prev;

		if (tail != NULL)
			tail->next = NULL;
	}
	else 
	{
        Assert(dead_cell->prev);
		dead_cell->prev->next = dead_cell->next;

		
		if (dead_cell->next != NULL)
			dead_cell->next->prev = dead_cell->prev;

	}

	delete dead_cell;
    dead_cell = NULL; 

	count--;
}















int linked_list::Remove_Data
(
	data_ptr dead_data
)
{
    Assert(dead_data);

	
	list_cell_ptr current_cell;
	int found = 0;
	

	
	current_cell = head;

	
	
	last_number_accessed = -999;
	last_cell_accessed = NULL;  

#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

	
	while ((current_cell != NULL) && (!found)) 
	{
Assert(++finite_loop < 100000); 
		
		if (current_cell->cell_data == dead_data) 
		{
			found = 1;
		}
		else 
		{
			
			current_cell = current_cell->next;
		}

	} 

	
	if (found)
		Remove_Cell(current_cell);

	
	return ( found );
}












int linked_list::Remove_Data_Multiple
(
	data_ptr dead_data
)
{
    Assert(dead_data); 

	
	list_cell_ptr current_cell;
	list_cell_ptr next_cell;
	int count;
	

	
	count = 0;
	current_cell = head;

	
	
	last_number_accessed = -999;
	last_cell_accessed = NULL;  

#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

	
	while (current_cell != NULL) 
	{
Assert(++finite_loop < 100000); 
		next_cell = current_cell->next;

		
		if (current_cell->cell_data == dead_data) 
		{
			Remove_Cell( current_cell );
			count++;
		}

		
		current_cell = next_cell;
	}

	return (count);
}


















































































int linked_list::Already_In_List
(
	data_ptr check_data
)
{
    Assert(check_data); 

  
  list_cell_ptr current_cell;
  int i;
  

	if (count is 0) return(FALSE);

	current_cell = head;

	for (i=0; i < count; i++) 
	{

		if (current_cell->cell_data is check_data)
			return(TRUE);

		current_cell = current_cell->next;

	}

	return(FALSE);

}












list_cell_ptr linked_list::Return_Cell_By_Number
(
	int cell_num
)
{
	
	list_cell_ptr current_cell;
	int i;
	

	if ((cell_num <0) || (cell_num >= count))
	return(NULL);

	
	if (cell_num is (last_number_accessed - 1)) 
	{
		current_cell = last_cell_accessed->prev;
	}
	
	else if (cell_num is (last_number_accessed + 1)) 
	{
		current_cell = last_cell_accessed->next;
	}
	
	else if (cell_num is last_number_accessed) 
	{
		current_cell = last_cell_accessed;
	}
	
	else 
	{
		current_cell = head;

		for (i=0; i < cell_num; i++) 
		{

			current_cell = current_cell->next;

		}

	}

	
	
	
	last_number_accessed = cell_num;
	last_cell_accessed = current_cell;

	return(current_cell);

}













int linked_list::Return_Index_Of_Data 
(
	data_ptr search_data
)
{
    Assert(search_data); 

	
	int index;
	list_cell_ptr current_cell;
	

	
	index = 0;
	current_cell = head;

	
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif
	while (current_cell) 
	{

Assert(finite_loop++ < 100000); 
		
		if (current_cell->cell_data == search_data)
			return (index);

		
		index ++;
		current_cell = current_cell->next;
	}

	
	return (-1);
}

















void linked_list::Append_Whole_List
(
	linked_list * other_list
)
{
    Assert(other_list); 

	
	int i;
	data_ptr the_data;
	

	
	for (i=0; i < other_list->count; i++)
	{
		
		the_data = other_list->Return_Data_By_Number(i);

		
		Append_Data(the_data);

	} 


}







#ifdef _DEBUG

void linked_list::Validate()
{ 
    if (head) { 
        Assert(NULL == head->prev); 
    } 

    if (tail) { 
        Assert(NULL == tail->next); 
    }

#ifdef _DEBUG
    sint32 finite_loop = 0 ; 
#endif

    if (head) { 
        list_cell *ptr, *old; 
        sint32 cell_count=0; 

        old = NULL; 
        ptr = head; 

        while(ptr) { 
            Assert(finite_loop++ < 100000); 
            Assert(old == ptr->prev); 
            old = ptr; 
            ptr = ptr->next; 
            cell_count++; 
        } 

        Assert(old == tail); 

        Assert(cell_count == count); 
    } else { 
        Assert(0 == count); 
    }
} 
#endif _DEBUG       







unique_ID_cell_ptr unique_ID_list::Add_Data(data_ptr new_data)
{
    Assert(new_data); 
	
	unique_ID_cell_ptr current_cell;
	

	
	current_cell = new unique_ID_list_cell;

	
	current_cell->cell_data = new_data;

	
	current_cell->ID = next_ID;

	
	current_cell->prev = NULL;
	current_cell->next = head;
	head = current_cell;

	
	


	
	if (head->next != NULL)
		head->next->prev = head;

	
	next_ID++;

	
	
	count++;

	
	
	return(current_cell);
}












data_ptr unique_ID_list::Return_Data_By_ID(int the_ID)
{
	
	unique_ID_cell_ptr current_cell;
	int i;
	

	if ((the_ID <0) || (the_ID >= count))
		return(NULL);

	current_cell = (unique_ID_cell_ptr) head;

	
	for (i=0; i < count; i++) 
	{

		if (current_cell->ID is the_ID) 
		{
			break;
		}
		else {
			current_cell = (unique_ID_cell_ptr) current_cell->next;
		}

	}

	
	if (current_cell isnt NULL)
		return(current_cell->cell_data);
	else return(NULL);

}











data_ptr queue::Pop_First_In_Data()
{
	
	list_cell_ptr oldest_cell;
	data_ptr the_data;
	

	if (count is 0)
	{
		return(NULL);
	}

	oldest_cell = Return_Cell_By_Number(count-1);

	
	if (oldest_cell->prev is NULL)
	{
		head = NULL;
	}
	else 
	{
		oldest_cell->prev->next = NULL;
	}

	
	the_data = oldest_cell->cell_data;

	
	
	
	last_number_accessed = count - 2;
	last_cell_accessed = oldest_cell->prev;

	
	delete oldest_cell;
    oldest_cell = NULL; 

	count--;

	return(the_data);

}









