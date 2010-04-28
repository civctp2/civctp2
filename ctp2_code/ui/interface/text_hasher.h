#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __Text_Hasher_h__
#define __Text_Hasher_h__

#include <algorithm>

template <class DATA_TYPE>  class Text_Hasher;
template <class DATA_TYPE>  class Translation;

size_t const                MAX_KEY_CHARS   = 40;


template <class DATA_TYPE> 
class Translation
{
public:
	TCHAR *         m_key;						
	DATA_TYPE       m_data;					
	Translation *   m_next;				

	
	Translation
	(
		TCHAR *     the_key,					
		DATA_TYPE   the_data			
	)
    :
        m_key       (the_key),
        m_data      (the_data),
        m_next      (NULL)
	{};

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
		size_t      size,						
		DATA_TYPE   untranslated_value	
	);

	~Text_Hasher();

	DATA_TYPE Look_Up_Data
	(
		const _TCHAR *  key
	);

	void Add_To_Hash_Table
	(
		_TCHAR *    key,
		DATA_TYPE   the_data
	);

protected:

	Translation<DATA_TYPE> **   m_hash_table;
	size_t                      m_data_count;
	size_t                      m_hash_table_size;
	DATA_TYPE                   m_untranslated;

	size_t Hash_The_String
	(
		const _TCHAR *  key
	);

};



















template <class DATA_TYPE>
Text_Hasher<DATA_TYPE>::Text_Hasher
(
	size_t      size,							
	DATA_TYPE   untranslated_value		
)
:
    m_hash_table            (NULL),
	m_data_count            (0),
	m_hash_table_size       (size + size/2),
	m_untranslated          (untranslated_value)
{
	m_hash_table = new Translation<DATA_TYPE> *[m_hash_table_size];
    std::fill(m_hash_table, m_hash_table + m_hash_table_size, 
              (Translation<DATA_TYPE> *) NULL
             );
}










template <class DATA_TYPE>
Text_Hasher<DATA_TYPE>::~Text_Hasher()
{
	for (size_t i = 0; i < m_hash_table_size; ++i)
	{
		for 
        (
            Translation<DATA_TYPE> * translation = m_hash_table[i];
            translation;
            // translation updated in loop
        )
		{
			Translation<DATA_TYPE> * trash_me = translation;
			translation = translation->m_next;
			delete trash_me;
		} 
	} 
	
	delete [] m_hash_table;
}












template <class DATA_TYPE>
DATA_TYPE Text_Hasher<DATA_TYPE>::Look_Up_Data
(
	const _TCHAR * key
)
{
#ifdef WIN32
    size_t      keyLength       = _tcslen(key);
#else
    size_t      keyLength       = strlen(key);
#endif
	_TCHAR *    lowerCaseKey    = new _TCHAR[keyLength + 1];
    std::transform(key, key + keyLength, lowerCaseKey, tolower);
    lowerCaseKey[keyLength]     = _TCHAR(0);

	for
    (
        Translation<DATA_TYPE> * translation = m_hash_table[Hash_The_String(key)];
        translation;
        translation = translation->m_next
    )
	{
#ifdef WIN32
		if (!_tcsncmp(key, translation->m_key, MAX_KEY_CHARS))
#else
		  if (!strncmp(key, translation->m_key, MAX_KEY_CHARS))
#endif // WIN32
		{
            delete [] lowerCaseKey;
			return translation->m_data;
		} 
	} 

    delete [] lowerCaseKey;
	return m_untranslated;
}




template <class DATA_TYPE>
void Text_Hasher<DATA_TYPE>::Add_To_Hash_Table
(
	_TCHAR *    key,
	DATA_TYPE   the_data
)
{
	size_t                      hash_location   = Hash_The_String(key);
	Translation<DATA_TYPE> *    translation     = 
        new Translation<DATA_TYPE>(key, the_data);

	translation->m_next         = m_hash_table[hash_location];
	m_hash_table[hash_location] = translation;
}



template <class DATA_TYPE>
size_t Text_Hasher<DATA_TYPE>::Hash_The_String
(
	const _TCHAR * key
)
{
	size_t          a               = 31415;						
	size_t const    b               = 27183;				
	size_t          hash_location   = 0;				
	for 
    (
        size_t chars_used = 0;
        (chars_used < MAX_KEY_CHARS) && (*key);
        ++chars_used
    )
    {
        hash_location   = (hash_location*a + abs(*key)) % m_hash_table_size;
        a               = a * b % (m_hash_table_size - 1);
        ++key;
	} 

	Assert(hash_location < m_hash_table_size);
    return (hash_location < m_hash_table_size) ? hash_location : 0;
}

#endif 

