#ifndef __matrix_h__
#define __matrix_h__

#ifndef MALLOC
#define MALLOC malloc
#endif
#ifndef REALLOC
#define REALLOC realloc
#endif
#ifndef FREE
#define FREE free
#endif







template <class DATA_TYPE>
class matrix
{
	public:






		matrix
		(
			int i_rows,
			int i_columns
		);






		~matrix();






		matrix
		(
			matrix& the_matrix
		);






		matrix& operator=
		(
			matrix& the_matrix
		);






		DATA_TYPE& operator()(const int nRow, const int nColumn);
		DATA_TYPE operator()(const int nRow, const int nColumn) const;







		int Get_Rows()
		{
			return rows;
		}






		int Get_Columns()
		{
			return columns;
		}






		inline void Fast_Copy
		(
			matrix& the_matrix
		)
		{
			_ASSERTE(size == the_matrix.size);
			int bytes_to_copy = size * sizeof(DATA_TYPE);

			memcpy(data, the_matrix.data, bytes_to_copy);
		}

	private:

		DATA_TYPE* data;
		int rows;
		int columns;
		int size;







		void Allocate
		(
			int i_rows,
			int i_columns
		);







		void Assign
		(
			matrix& the_matrix
		);






		void Clean_Up();


};












template <class DATA_TYPE>
void matrix<DATA_TYPE>::Allocate
(
	int i_rows,
	int i_columns
)
{

	size = i_rows * i_columns;


	_ASSERTE(size > 0);

	rows = i_rows;
	columns = i_columns;

	data = (DATA_TYPE*) MALLOC(sizeof(DATA_TYPE) * size);

}












template <class DATA_TYPE>
void matrix<DATA_TYPE>::Assign
(
	matrix& the_matrix
)
{

	int i;


	Clean_Up();

	Allocate(the_matrix.rows, the_matrix.columns);

	for (i = 0; i < size; i++)
	{

		data[i] = the_matrix.data[i];
	}

}












template <class DATA_TYPE>
void matrix<DATA_TYPE>::Clean_Up()
{

	if (data isnt NULL)
		FREE(data);

	data = NULL;

	size = 0;

}












template <class DATA_TYPE>
matrix<DATA_TYPE>::matrix
(
	int i_rows,
	int i_columns

)
{

	data = NULL;

	Allocate(i_rows, i_columns);

}












template <class DATA_TYPE>
matrix<DATA_TYPE>::~matrix()
{

	Clean_Up();
}












template <class DATA_TYPE>
matrix<DATA_TYPE>::matrix
(
	matrix& the_matrix
)
{

	data = NULL;

	Assign(the_matrix);
}












template <class DATA_TYPE>
matrix<DATA_TYPE>& matrix<DATA_TYPE>::operator=
(
	matrix& the_matrix
)
{

	Assign(the_matrix);

	return this;
}












template <class DATA_TYPE>
DATA_TYPE& matrix<DATA_TYPE>::operator()
(
	const int nRow,
	const int nColumn
)

{

	int data_location;


	data_location = nRow*columns + nColumn;

	_ASSERTE(data_location >= 0 && data_location < size);

	return data[data_location];
}











template <class DATA_TYPE>
DATA_TYPE matrix<DATA_TYPE>::operator()
(
	const int nRow,
	const int nColumn
) const

{

	int data_location;


	data_location = nRow*columns + nColumn;

	_ASSERTE(data_location >= 0 && data_location < size);

	matrix<DATA_TYPE>& curr = *(matrix<DATA_TYPE>*)this;

	return curr[data_location];
}







#endif
