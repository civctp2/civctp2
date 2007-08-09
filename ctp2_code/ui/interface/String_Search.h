




#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef STRING_SEARCH_H__
#define STRING_SEARCH_H__

#define NUM_CHAR_VALUES 256

class String_Search
{
public:
	static void Set_Search_Key
	(
		char * new_key
	);

	static bool Search
	(
		char * my_string,
		char * new_key = NULL
	);

	static void Set_Case_Sensitive
	(
		bool case_sensitive
	);

protected:

	
	static char * m_key;

	
	static int m_key_length;

	
	static bool m_case_sensitive;

	
	static int m_skip_table[NUM_CHAR_VALUES];
};

#endif 

