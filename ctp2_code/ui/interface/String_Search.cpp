








#include "c3.h"

#ifdef TEST_STRING_SEARCH

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#endif 


#include "String_Search.h"



char * String_Search::m_key = NULL;


int String_Search::m_key_length = 0;


bool String_Search::m_case_sensitive = false;


int String_Search::m_skip_table[NUM_CHAR_VALUES];





void String_Search::Set_Search_Key
(
	
	char * new_key
)
{
	
	if (!new_key)
		return;

	
	m_key = new_key;

	
	m_key_length = strlen(m_key);

	
	for (int i = 0; i < NUM_CHAR_VALUES; i++)
	{
		
		m_skip_table[i] = m_key_length;

	} 

	
	char key_char;

	
	for (int j = 0; j < m_key_length; j++)
	{
		
		key_char = m_key[j];

		
		if (!m_case_sensitive)
			key_char = tolower(key_char);

		
		
		m_skip_table[key_char] = m_key_length - j - 1;

	} 
}










bool String_Search::Search
(
	
	char * my_string,

	
	char * new_key 

)
{
	
	if (new_key)
		Set_Search_Key(new_key);

	
	if (!m_key)
		return false;

	
	int i, j, t;

	
	char key_char, string_char;

	
	int my_string_length = strlen(my_string);

	
	for (i = m_key_length-1, j = m_key_length - 1; j >= 0; i--, j--)
	{
		
		key_char = m_key[j];
		string_char = my_string[i];

		
		if (!m_case_sensitive)
		{
			
			key_char = tolower(key_char);
			string_char = tolower(string_char);

		} 

		
		while (string_char != key_char)
		{
			

			t = m_skip_table[string_char];

			
			i += (m_key_length-j > t) ? m_key_length-j : t;

			
			if (i >= my_string_length)
				return false;

			
			j = m_key_length - 1;

			
			key_char = m_key[j];
			string_char = my_string[i];

			
			if (!m_case_sensitive)
			{
				
				key_char = tolower(key_char);
				string_char = tolower(string_char);

			} 

		} 

	} 

	
	return true;
}










void String_Search::Set_Case_Sensitive
(
	bool case_sensitive
)
{
	
	m_case_sensitive = case_sensitive;
}






#ifdef TEST_STRING_SEARCH





void main()
{
	
	char my_string[1024];
	char key[256];

	key[0] = 0;

	
	printf("Enter main string\n");
	gets(my_string);

	
	while (strcmp(key, "STOP"))
	{
		
		printf("Enter the search key. 'STOP' to end.\n");
		gets(key);

		
		String_Search::Set_Search_Key(key);

		
		if (String_Search::Search(my_string))
			printf("YES\n");
		else
			printf("NO\n");


	} 
}





#endif 
