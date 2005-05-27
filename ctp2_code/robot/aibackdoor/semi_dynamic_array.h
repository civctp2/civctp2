












#ifndef __semi_dynamic_array_h__
#define __semi_dynamic_array_h__


#ifndef MALLOC
	#ifdef _DEBUG
		#include "debugmemory.h"
	#else
		#ifndef REALLOC
			#define REALLOC(buf,size) realloc (buf,size)
		#endif

		#ifndef MALLOC
			#define MALLOC(size) malloc(size)
		#endif

		#ifndef CALLOC
			#define CALLOC(count,size) calloc(count,size)
		#endif

		#ifndef STRDUP
			#define STRDUP(str) strdup(str)
		#endif

		#ifndef FREE
			#define FREE(ptr) free(ptr)
		#endif
	#endif
#endif



















template <class T>
class semi_dynamic_array
{
	public:

		
		
		
		
		
		semi_dynamic_array
		(
			int i_size					
		);

		
		
		
		
		
		~semi_dynamic_array();

		
		
		
		
		
		semi_dynamic_array
		(
			semi_dynamic_array<T>& sda
		);

		
		
		
		
		
		semi_dynamic_array
		(
             int r_size,
             char *r_data
		);

		
		
		
		
        
		void Get_Raw
        (
            int *r_size,                
            char **r_data               
        );

		
		
		
		
		
		semi_dynamic_array& operator=
		(
			semi_dynamic_array<T>& sda
		);

		
		
		
		
		
		T& operator[](const int nIndex);
		T operator[](const int nIndex) const;

		
		
		
		
		
		void Resize
		(
			int new_size
		);

		
		
		
		
		
		int Get_Size();

	protected:

		T* data;						
		int size;						

		
		
		
		
		
		void Allocate
		(
			int size
		);


		
		
		
		
		
		void Assign
		(
			semi_dynamic_array<T>& sda
		);

		
		
		
		
		
		void Clean_Up();



};












template <class T>
void semi_dynamic_array<T>::Allocate
(
	int new_size
)
{
	
	if (new_size <= 0) return;

	
	size = new_size;

	
	data = (T*) MALLOC(sizeof(T)*new_size);


}












template <class T>
void semi_dynamic_array<T>::Assign
(
	semi_dynamic_array<T>& sda
)
{
	
	int i;
	

	
	Clean_Up();

	
	Allocate(sda.size);

	
	for (i = 0; i < size; i++)
	{
		
		data[i] = sda.data[i];
	}

}












template <class T>
void semi_dynamic_array<T>::Resize
(
	int new_size
)
{
	
	if (new_size <= 0) return;

	
	size = new_size;

	
	if (data == NULL)
		data = (T*)MALLOC(sizeof(T)*new_size);
	else
		data = (T*) REALLOC(data, sizeof(T)*new_size);

}












template <class T>
int semi_dynamic_array<T>::Get_Size()
{
	return size;
}












template <class T>
void semi_dynamic_array<T>::Clean_Up()
{
	
	if (data isnt NULL)
		FREE(data);

	
	data = NULL;

}












template <class T>
void semi_dynamic_array<T>::Get_Raw
(
    int *r_size,
    char **r_data
) 
{
	*r_size = size * sizeof(T);
	*r_data = (char *) data;

    return;
}












template <class T>
semi_dynamic_array<T>::semi_dynamic_array
(
	int i_size					
)
{

	
	data = NULL;
    size = 0; 

	
	Allocate(i_size);

}












template <class T>
semi_dynamic_array<T>::~semi_dynamic_array()
{
	
	Clean_Up();
}












template <class T>
semi_dynamic_array<T>::semi_dynamic_array
(
	semi_dynamic_array<T>& sda
)
{
	
	data = NULL;

	
	this->chunk_size = sda.chunk_size;
	this->max_chunks = sda.max_chunks;
	this->chunks = sda.chunks;

	
	Assign(sda);
}












template <class T>
semi_dynamic_array<T>::semi_dynamic_array
(
    int r_size,
    char *r_data
)
{

	
	data = NULL;

	
	Allocate((r_size / sizeof(T)) + 1);

    Assert(r_data); 
    memcpy((char *)data, r_data, r_size);
}












template <class T>
semi_dynamic_array<T>& semi_dynamic_array<T>::operator=
(
	semi_dynamic_array<T>& sda
)
{

	
	Assign(sda);

	return this;
}












template <class T>
T& semi_dynamic_array<T>::operator[](const int nIndex)
{
#ifdef WIN32	
	_ASSERTE(nIndex >= 0 && nIndex < size);
#endif

	
	return data[nIndex];
}











template <class T>
T semi_dynamic_array<T>::operator[](const int nIndex) const
{
	
	semi_dynamic_array<T>& curr = *(semi_dynamic_array<T>*)this;

	return curr[nIndex];
}






#endif 
