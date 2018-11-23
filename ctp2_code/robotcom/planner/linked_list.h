#ifndef __linked_list_H__
#define __linked_list_H__

#ifndef ABS
#define ABS(a) (((a) < 0) ? -(a) : (a))
#endif

typedef void * data_ptr;

typedef class list_cell * list_cell_ptr;
typedef class unique_ID_list_cell *unique_ID_cell_ptr;






class list_cell
{
public:
  data_ptr cell_data;

  list_cell_ptr next;
  list_cell_ptr prev;

  list_cell();
  ~list_cell();

};











class unique_ID_list_cell : public list_cell {
private :
public:

	int ID;

};






typedef class linked_list * linked_list_ptr;





class DLLHEADER linked_list
{

protected:
	int last_number_accessed;
	list_cell_ptr last_cell_accessed;

public:

	list_cell_ptr head;
	list_cell_ptr tail;

	int count;

	void Init();

	linked_list();

	void Empty();

	~linked_list();

	list_cell_ptr Add_Data
	(
		data_ptr new_data
	);

	list_cell_ptr Append_Data
	(
		data_ptr new_data
	);

	void Remove_Cell
	(
		list_cell_ptr dead_cell
	);

	int Remove_Data
	(
		data_ptr dead_data
	);


	int Remove_Data_Multiple
	(
		data_ptr dead_data
	);











    inline list_cell_ptr Random_Access_Lookup
    (
        int data_num
    )
    {
        int i;
        int df_head;
        int df_point;
        int df_tail;
        int tA, tB, tC;

		list_cell_ptr p;

        df_head = data_num;
        df_point = data_num - last_number_accessed;
        df_tail = count - data_num - 1;

        tA = df_head < ABS(df_point);
        tB = df_head < df_tail;
        tC = ABS(df_point) < df_tail;

        if (tA && tB) {

            p = head;
            for(i=0; i<data_num; i++, p = p->next);

        } else if (!tB && !tC) {

            p = tail;
            for(i=df_tail; i; i--, p = p->prev);

        } else {

            p = last_cell_accessed;
            if (df_point < 0) {

                for(i= -df_point; i; i--, p = p->prev);
            } else {

                for(i=0; i < df_point; i++, p = p->next);
            }
        }

        return(p);
    }






	inline data_ptr Return_Data_By_Number
	(
		int data_num
	)
	{

		list_cell_ptr current_cell;

		if ((data_num <0) || (data_num >= count))
			return(NULL);




		if (data_num is 0)
		{
			current_cell = head;
		}

		if (data_num is (last_number_accessed - 1))
		{
			current_cell = last_cell_accessed->prev;
		}

		else if (data_num is (last_number_accessed + 1))
		{
			current_cell = last_cell_accessed->next;
		}

		else if (data_num is (count-1))
		{
			current_cell = tail;
		}

		else
		{
            current_cell = Random_Access_Lookup(data_num);
		}




		last_number_accessed = data_num;
		last_cell_accessed = current_cell;

		Assert(current_cell);
		return(current_cell->cell_data);
	};







	int Already_In_List
	(
		data_ptr check_data
	);

	list_cell_ptr Return_Cell_By_Number
	(
		int cell_num
	);

	int Return_Index_Of_Data
	(
		data_ptr search_data
	);

	void Append_Whole_List(linked_list * other_list);

    #ifdef _DEBUG
        void Validate();
    #endif _DEBUG

};











class DLLHEADER unique_ID_list : public linked_list
{
private :

	int next_ID;

public :

	unique_ID_cell_ptr Add_Data(data_ptr new_data);
	data_ptr Return_Data_By_ID(int num);

};











class DLLHEADER queue : public linked_list
{
private :

public :

	data_ptr Pop_First_In_Data();

};






#endif
