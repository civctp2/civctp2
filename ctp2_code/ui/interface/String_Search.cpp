








#include "c3.h"
#include "String_Search.h"

#include <algorithm>
#ifdef TEST_STRING_SEARCH
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif 


char *  String_Search::s_key            = NULL;
int     String_Search::s_key_length     = 0;
bool    String_Search::s_case_sensitive = false;
int     String_Search::s_skip_table[NUM_CHAR_VALUES];


void String_Search::Set_Search_Key
(
	char * new_key
)
{
	if (!new_key)
		return;

	s_key           = new_key;
	s_key_length    = strlen(s_key);
    std::fill(s_skip_table, s_skip_table + NUM_CHAR_VALUES, s_key_length);

	for (int j = 0; j < s_key_length; j++)
	{
        char key_char = 
            (s_case_sensitive) ? s_key[j] : static_cast<char>(tolower(s_key[j]));

		s_skip_table[key_char] = s_key_length - j - 1;
	} 
}


bool String_Search::Search
(
	char const *    my_string,
	char *          new_key 
)
{
	if (new_key)
		Set_Search_Key(new_key);

	if (!s_key)
		return false;

	int     my_string_length    = strlen(my_string);
	int     i                   = s_key_length-1;
    char    key_char;
    char    string_char;

	for (int j = s_key_length - 1; j >= 0; i--, j--)
	{
		if (s_case_sensitive)
        {
            key_char    = s_key[j];
            string_char = my_string[i];
        }
        else
		{
			key_char    = static_cast<char>(tolower(s_key[j]));
			string_char = static_cast<char>(tolower(my_string[i]));
		} 

		while (string_char != key_char)
		{
			int t = s_skip_table[string_char];

			i += (s_key_length-j > t) ? s_key_length-j : t;

			if (i >= my_string_length)
				return false;

			j   = s_key_length - 1;

			if (s_case_sensitive)
            {
			    key_char    = s_key[j];
			    string_char = my_string[i];
            }
            else
			{
				key_char    = static_cast<char>(tolower(s_key[j]));
				string_char = static_cast<char>(tolower(my_string[i]));
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
	s_case_sensitive = case_sensitive;
}


#ifdef TEST_STRING_SEARCH
int main()
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
