#ifndef __Text_Hasher_h__
#define __Text_Hasher_h__



















const int MAX_KEY_CHARS = 40;








template <class DATA_TYPE> class Translation;





















template <class DATA_TYPE> 
class Translation
{
public:
	TCHAR *m_key;						
	DATA_TYPE m_data;					
	Translation *m_next;				

	
	Translation
	(
		TCHAR *the_key,					
		DATA_TYPE the_data			
	)
	{
		m_key = the_key;
		m_data = the_data;
		m_next = NULL;
	};

	~Translation()
	{
		delete m_data;
		delete m_key;
	}
};













template <class DATA_TYPE> 
class Text_Hasher
{
public:

	
	
	
	
	Text_Hasher
	(
		int size,						
		DATA_TYPE untranslated_value	
	);

	
	
	
	
	~Text_Hasher();

	
	
	
	
	
	DATA_TYPE Look_Up_Data
	(
		const _TCHAR *key
	);

	
	
	
	
	void Add_To_Hash_Table
	(
		_TCHAR *key,
		DATA_TYPE the_data
	);


protected:

	
	Translation<DATA_TYPE> ** m_hash_table;

	
	int m_data_count;

	
	int m_hash_table_size;

	
	DATA_TYPE m_untranslated;

	
	
	
	
	int Hash_The_String
	(
		const _TCHAR *key
	);

};



















template <class DATA_TYPE>
Text_Hasher<DATA_TYPE>::Text_Hasher
(
	int size,							
	DATA_TYPE untranslated_value		
)
{
	
	m_data_count = 0;

	
	m_hash_table_size = (int) (size * 1.5);

	
	m_untranslated = untranslated_value;

	
	m_hash_table = new Translation<DATA_TYPE> *[m_hash_table_size];

	
	for (int i = 0; i < m_hash_table_size; i++)
	{
		
		m_hash_table[i] = NULL;

	} 

}










template <class DATA_TYPE>
Text_Hasher<DATA_TYPE>::~Text_Hasher()
{
	
	Translation<DATA_TYPE> * translation;
	Translation<DATA_TYPE> * trash_me;	
	int deleted_count=0;				
	

	
	for (int i = 0; i < m_hash_table_size; i++)
	{
		
		translation = m_hash_table[i];

		
		while (translation != NULL)
		{
			
			trash_me = translation;

			
			translation = translation->m_next;

			
			delete trash_me;

			
			deleted_count++;

		} 

	} 

	
	delete[] m_hash_table;

}












template <class DATA_TYPE>
DATA_TYPE Text_Hasher<DATA_TYPE>::Look_Up_Data
(
	const _TCHAR *key
)
{
	
	int hash_location = 0;				
	Translation<DATA_TYPE> * translation;			
	

	
	hash_location = Hash_The_String(key);

	
	translation = m_hash_table[hash_location];

	
	
	while (translation != NULL)
	{
		
		
		if (!_tcsncmp(key, translation->m_key, MAX_KEY_CHARS))
		{
			
			return translation->m_data;

		} 

		
		translation = translation->m_next;

	} 

	
	return m_untranslated;
}


















template <class DATA_TYPE>
void Text_Hasher<DATA_TYPE>::Add_To_Hash_Table
(
	_TCHAR *key,
	DATA_TYPE the_data
)
{
	
	Translation<DATA_TYPE> * translation;			
	int hash_location;					
	

	
	hash_location = Hash_The_String(key);

	
	translation = new Translation<DATA_TYPE>(key, the_data);

	
	translation->m_next = m_hash_table[hash_location];

	
	m_hash_table[hash_location] = translation;
}


















template <class DATA_TYPE>
int Text_Hasher<DATA_TYPE>::Hash_The_String
(
	const _TCHAR *key
)
{
	
	int hash_location = 0;				
	int a = 31415;						
	const int b = 27183;				
	int chars_used = 0;					
	

	
	for (;*key != 0; key++, a = a*b%(m_hash_table_size-1))
	{
		
		if (chars_used >= MAX_KEY_CHARS)
			break;

		
		hash_location = (hash_location*a + abs(*key)) % m_hash_table_size;

		
		chars_used++;

	} 

	
	
	Assert(hash_location < m_hash_table_size);
	Assert(hash_location >= 0);

	
	if ((hash_location >= m_hash_table_size) ||
		(hash_location < 0))
	{
		
		Assert(false);

		
		hash_location = 0;

	} 


	
	return hash_location;
}







#endif 

