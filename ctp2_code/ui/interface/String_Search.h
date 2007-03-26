







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
		char const *    my_string,
		char *          new_key = NULL
	);

	static void Set_Case_Sensitive
	(
		bool case_sensitive
	);

private:
	static char *   s_key;
	static int      s_key_length;
	static bool     s_case_sensitive;
	static int      s_skip_table[NUM_CHAR_VALUES];
};

#endif 

