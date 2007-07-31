

















































































































































































































#include "c3.h"
#include "c3types.h"
#include "c3errors.h"

#include "IC3GameState.h"
#include "IC3ErrorReport.h"

#include <ctype.h>

#include "math.h" 
#include "common.h"
#include "Linked_List.h"

#include "Globals.h"
#include "AIMain.h"

#include "AILog.h"

#include "dr_debug.h"
#include "IParser.h"
#include "ExpressionP.h"













void iparser::Report_Error
(
  char *raw_message,                      
  int report_type                     
)

{
    char message[1024]; 
    char buf[1024];
	va_list list;

    sprintf (message, "%s", raw_message); 

	va_start(list, message);
	vsprintf(buf, message, list);
	va_end(list);
	ai->m_gs->GetErrorReport()->ReportError(buf);

	switch(report_type) {
		case REPORT_WARNING:
			
			AILOG((wstr, "IParse WARNING: %s\n", message));
			break;

		case REPORT_ERROR:
			
			AILOG((wstr, "IParse ERROR: %s\n", message));
			break;

		case REPORT_FATAL:
			
			AILOG((wstr, "IParse FATAL ERROR: %s\n", message));
           

			Assert(false);
			break;

		default:
			AILOG((wstr, "IParse MESSAGE: %s\n", message));
			break;
	}
}














int IParser_Find_Char_Type
(

  char ch                      
)
{
	if (ch == EOF)
		return END_OF_FILE;

	if (isalnum(ch) || (ch == '.') || (ch == '_'))
		return ALNUM;
    
	if (isspace(ch) || !isgraph(ch) || (ch == 13))
		return WHITE_SPACE;

	return SYMBOL;
}















int IParser_Is_OK_Char_Not_Float(char ch)
{
  if (isalnum(ch))
    return(ALNUM);
  if (ch is ';') return(SYMBOL);        
  if (ch is '"') return(SYMBOL);
  if (ch is '{') return(SYMBOL);
  if (ch is '}') return(SYMBOL);
  if (ch is '\'') return(SYMBOL);
  if (ch is '=') return(SYMBOL);
  if (ch is '!') return(SYMBOL);
  if (ch is '<') return(SYMBOL);
  if (ch is '>') return(SYMBOL);
  if (ch is '.') return(SYMBOL);
  if (ch is ',') return(SYMBOL);
  if (ch is '(') return(SYMBOL);
  if (ch is ')') return(SYMBOL);
  if (ch is '[') return(SYMBOL);
  if (ch is ']') return(SYMBOL);
  if (ch is '*') return(SYMBOL);
  if (ch is '^') return(SYMBOL);
  if (ch is '+') return(SYMBOL);
  if (ch is '/') return(SYMBOL);
  if (ch is '-') return(SYMBOL);
  if (ch is '#') return(SYMBOL);
  if (ch is '_') return(ALNUM);
  return(BOGUS);
}
//
// IParser_Is_OK_Char_Not_Float
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Get_New_Struct
//
// Allocate a new IParse struct.
//
// Returns: Ptr to the struct
struct_list_element *iparser::Get_New_Struct
(
  char *the_struct_name                 // What shall we call it?
)

{
  // Local Variables //////////////////////////////////////////////////////////
  struct_list_element *new_struct;      // Ptr for mem allocation            
  char *new_string;                     // Temp ptr for mem allocation       
  /////////////////////////////////////////////////////////////////////////////

  new_struct = (struct_list_element *) MALLOC (sizeof(struct_list_element));

  new_string = (char *) MALLOC (strlen(the_struct_name)+1);

  strcpy(new_string, the_struct_name);  // Copy over the name of struct      

  new_struct->struct_name = new_string; // Assign it in the right place      
  new_struct->struct_length = 0;        // Blank it                          
  new_struct->the_definition = NULL;    // Blank it                          
  new_struct->next_struct = NULL;       // Blank it                          

  return(new_struct);                   // Send it back to user              
}
//
// iparser::Get_New_Struct
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Get_New_Variable
//
// Allocate a new variable of the specified type
//
// Returns: Pointer to the variable.
variable_struct *iparser::Get_New_Variable
(
  int the_variable_type,                   // What sort are we getting
  char *the_variable_name                  // What shall we call it
)

{
  // Local Variables //////////////////////////////////////////////////////////
  variable_struct *new_variable;           // Ptr for mem allocation            
  char *new_string;                        // Temp ptr for mem allocation       
  /////////////////////////////////////////////////////////////////////////////

  new_variable = (variable_struct *)MALLOC(sizeof(variable_struct));



  new_string = (char *)MALLOC(strlen(the_variable_name)+1);

  strcpy(new_string, the_variable_name);   // Copy over the name of variable       

  new_variable->variable_name = new_string;// Assign it in the right place      

  new_variable->variable_type = the_variable_type;

  new_variable->int_value = 0;
  new_variable->double_value = 0.0;

  new_variable->the_struct_el = NULL;      // Blank it                          
  new_variable->string_value = NULL;       // Blank it                          
  new_variable->next_array_variable = NULL;// Blank it                          
  new_variable->next_variable = NULL;      // Blank it   
  new_variable->struct_element_count = 0;  // Blank it

  return(new_variable);                    // Send it back to user              
}
//
// iparser::Get_New_Variable
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Get_New_Field
//
// Allocate a new field structure for a "struct"
//
// Returns: Ptr to the new field
field_def *iparser::Get_New_Field
(
  int the_field_type,                   // What sort are we getting
  char *the_field_name                  // What shall we call it
)

{
  // Local Variables //////////////////////////////////////////////////////////
  field_def *new_field;                 // Ptr for mem allocation            
  char *new_string;                     // Temp ptr for mem allocation       
  /////////////////////////////////////////////////////////////////////////////

  new_field = (field_def *)MALLOC(sizeof(field_def));

  new_string = (char *)MALLOC(strlen(the_field_name)+1);

  strcpy(new_string, the_field_name);   // Copy over the name of field       

  new_field->field_name = new_string;   // Assign it in the right place      

  new_field->field_type = the_field_type;

  new_field->next_field = NULL;         // Blank it                          

  return(new_field);                    // Send it back to user              
}
//
// iparser::Get_New_Field
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Insert_Variable
//
// Insert a new variable into the front of the parse domain's variable list
//
void iparser::Insert_Variable
(
  variable_struct *the_variable         // Ptr to variable to insert
)

{
  // Local Variables //////////////////////////////////////////////////////////
  variable_struct *temp;
  /////////////////////////////////////////////////////////////////////////////

  temp = variables;         // Insert variable to head of list      
  the_variable->next_variable = temp;
  variables = the_variable;

  number_of_variables += 1; // Increment variable count             
}
//
// iparser::Insert_Variable
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Insert_Struct
//
// Takes a pointer to an iparse struct, and puts it into the domain.
//
void iparser::Insert_Struct
(
  struct_list_element *the_struct
)

{
  // Local Variables //////////////////////////////////////////////////////////
  struct_list_element *temp;
  /////////////////////////////////////////////////////////////////////////////

  temp = the_structs;
  the_struct->next_struct = temp;
  the_structs = the_struct;

  number_of_structures += 1;// Increment struct count            
}
//
// iparser::Insert_Struct
//
///////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
//
// iparser::Read_Next_String
//
// Read the next declaration string in the file, check if we reached the end
//
// Returns: EOF, EXCELLENT
//
int iparser::Read_Next_String
(
  char *parse_buffer,                   // Memory buffer of unparsed params
  int *parse_pos,                       // Where are we in the buffer?
  int buffer_length,                    // Where's the end of the buffer
  char *declaration_string              // The new declaration
)

{
  // Local Variables //////////////////////////////////////////////////////////
  char ch = 20;                         // Character read in from file
  /////////////////////////////////////////////////////////////////////////////

  // FIRST, get rid of junk before the string.
#ifdef _DEBUG
    sint32 finite_loop=0; //use to catch inifinite loops
#endif

  while (IParser_Find_Char_Type(ch) is BOGUS) 
  {
Assert(++finite_loop < 100000); // make sure we catch infinite loogs

    ch = parse_buffer[*parse_pos];      // Read a single character from file 
    (*parse_pos)++;

    if ((*parse_pos) > buffer_length)
      return(EOF);                      // Hit end of file prematurely      
  } // while (IParser_Find_Char_Type(ch) is BOGUS...

  // We special case "Compiler Directives"
  // C.D.'s are an alpha-numeric string, but they start with the #
  if (ch is '#') 
  {
    (*declaration_string) = ch;         // Allow #defines
    declaration_string++;
    ch = parse_buffer[*parse_pos];      // Read a single character from file
    (*parse_pos)++;

    if ((*parse_pos) > buffer_length)
      return(EOF);                      // Hit end of file prematurely 
  }

  // We also special case curly-bracketed code
  // C.B.'s are a one-character long declaration string!
  if (ch is '{') {
    (*declaration_string) = ch;         // Allow Bracketed code segments
    declaration_string++;
    ch = parse_buffer[*parse_pos];      // Read a single character from file 
    (*parse_pos)++;

    if ((*parse_pos) > buffer_length)
      return(EOF);                      // Hit end of file prematurely       
  }

  else
    // SECOND, read until we hit a junk character                          
    while (IParser_Find_Char_Type(ch) is ALNUM) 
    {
      (*declaration_string) = ch;
      declaration_string++;
      ch = parse_buffer[*parse_pos];    // Read asingle character from file
      (*parse_pos)++;
  
      if ((*parse_pos) > buffer_length)
        return(EOF);                    // Hit end of file prematurely     
    }

  // Set back file ptr by one to let "bad" character be part of decl       
  (*parse_pos)--;

  (*declaration_string) = 0;            // End string nicely                 

  return(EXCELLENT);                    // Say if we reached end of string   
}
//
// iparser::Read_Next_String
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Which_Struct
//
// Given a name for a struct, return the pointer
//
// Returns: Ptr to a requested struct, or NULL
//
struct_list_element *iparser::Which_Struct
(
  char *declaration_string              // A string specifying a struct
)

{
  // Local Variables //////////////////////////////////////////////////////////
  struct_list_element *current_structure; // Ptr to current struct in list  
  int done = BOGUS;                     // Loop exit flag                   
  /////////////////////////////////////////////////////////////////////////////

  current_structure = the_structs; // Start at head of list  
#ifdef _DEBUG
    sint32 finite_loop=0; //use to catch inifinite loops
#endif

  while (!done) {
Assert(++finite_loop < 100000); // make sure we catch infinite loogs

    if (current_structure is NULL)      // We've reached end of list         
      return(NULL);                     // Didn't find a matching struct    

    else
      done = (!strcmp(declaration_string,
                      current_structure->struct_name)
             );                         // Do we match current struct name  

    if (!done)
      current_structure = current_structure->next_struct;
  }

  return(current_structure);            // We matched to a structure name    
}
//
// iparser::Which_Struct
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Which_Variable
//
// Given a string naming a variable, return a pointer to it
//
// Returns: Ptr to the specified variable, or NULL
//
variable_struct *iparser::Which_Variable
(
  char *declaration_string              // Variable name string
)

{
  // Local Variables //////////////////////////////////////////////////////////
  variable_struct *current_variable;    // Ptr to current struct in list     
  int done = BOGUS;                     // Loop exit flag                    
  /////////////////////////////////////////////////////////////////////////////

  current_variable = variables; // Start at head of list             

  // Go til we match or until we've reached end of list                      
  while ((!done)&&(current_variable isnt NULL)) {

    done = (!strcmp(declaration_string,
                    current_variable->variable_name)
           );                           // Do we match current variable name     

    if (!done)
      current_variable = current_variable->next_variable;
  }

  return(current_variable);                // We matched to a variable name        
}
//
// iparser::Which_Variable
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Variable_Match
//
// We want to match the variable to all known declaration type
//
// Returns: Variable Type
//
int iparser::Variable_Match
(
  char *declaration_string              // First string in declaration
)

{
  if (!strcmp(declaration_string,"int"))
    return(INT_VARIABLE);               // Declaring an integer or int array 

  if (!strcmp(declaration_string,"float"))
    return(FLOAT_VARIABLE);             // Declaring a float or float array  

  if (!strcmp(declaration_string,"double"))
    return(DOUBLE_VARIABLE);            // Declaring a double or double array

  if (!strcmp(declaration_string,"char"))
    return(CHAR_VARIABLE);              // Declaring a char or char array    

  if (!strcmp(declaration_string,"struct"))
    return(STRUCT_DEFINITION);          // Declaring a structure             

  if (!strcmp(declaration_string,"#define"))
    return(DEFINE_DECLARATION);         // Declaration by define             

  if (!strcmp(declaration_string,"#include"))
    return(INCLUDE_DIRECTIVE);          // Declaration by define             

  if (!strcmp(declaration_string,"if"))
    return(CONDITIONAL_CODE);           // Conditional code follows          

  if (!strcmp(declaration_string,"else"))
    return(ELSE_CODE);                  // Conditional code follows          

  if (!strcmp(declaration_string,"for"))
    return(PARSE_FOR_LOOP);             // For-loop follows                  

  if (!strcmp(declaration_string,"{"))
    return(CODE_BRACKET);               // code_bracket follows              

  if (Which_Struct( declaration_string))
    return(STRUCT_ARRAY_VARIABLE);      // Array of previously defined struct

  if (Which_Variable( declaration_string) isnt NULL)
    return(VARIABLE_ASSIGN);            // Assignment to defined variable       

  return(BAD_DECLARATION);              // Declaration of unknown type       
}
//
// iparser::Variable_Match
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Search_For_Next_Declaration
//
// Get the next declaration string in the parse buffer 
//
// Returns: EARLY_END_OF_FILE, or next Toekn Type
//
int iparser::Search_For_Next_Declaration
(
  char *parse_buffer,                   // Current Parsing buffer
  int *parse_pos,                       // Position in parse buffer
  int buffer_length,                    // Length of the buffer
  char *declaration_string              // The next declaration string
)

{
  // Local Variables //////////////////////////////////////////////////////////
  int declaration_type;                 // What sort of declaration is next  
  /////////////////////////////////////////////////////////////////////////////

  if (Read_Next_String(parse_buffer, parse_pos, buffer_length, 
                              declaration_string) is EOF)
    declaration_type = EARLY_END_OF_FILE;
  else
    declaration_type = Variable_Match( declaration_string);

  return(declaration_type);
}
//
// iparser::Search_For_Next_Declaration
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Read_Until_String
//
// Read all the characters from the global parse buffer into a declaration
// buffer set up just for the current declaration
//
// Returns: EARLY_END_OF_FILE, or BOGUS
// Modifies: Can grow (REALLOC) the declaration buffer
//
int iparser::Read_Until_String
(
  char *parse_buffer,                   // Where to read chars from
  int *parse_pos,                       // Position in the source buffer
  int buffer_length,                    // Source buffer length
  char *end_string,                     // String marks end of declaration
  char **declaration_buffer,            // Storage for declaration
  int & current_declaration_length		// Length of declaration
)

{
  // Local Variables //////////////////////////////////////////////////////////
  int end_flag = BOGUS;                 // Are we comparing to end string?   
  int done = BOGUS;                     // Run out of file or matched end    
  signed char ch;                       // Character read in from file       
  int length = 1;                       // Length of declaration in chars    
  int end_pos = 1;                      // Position in end string to compare 
  /////////////////////////////////////////////////////////////////////////////

  char *current_char;                   // Ptr to current place in buffer    
  current_char = (*declaration_buffer); // set to start of buffer            


#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

  while(!done) 
  {
Assert(++finite_loop < 100000); // make sure we catch infinite loogs
    ch = parse_buffer[*parse_pos];      // Read a single character from file 
    (*parse_pos)++;

    // See if we've overrun the buffer
    if ((*parse_pos) > buffer_length) 
    {
      ch = EOF;
      done = EXCELLENT;
    } // if ((*parse_pos) > buffer_length...

    // If we're still in a valid part of the buffer
    else 
    {
      (*declaration_buffer)[length-1] = ch;  // Add character to buffer   

      length++;                         // Increment count of buf size       

//CUTME We don't do this silly realloc any more...too slow.
//CUTME      (*declaration_buffer) = (char *) REALLOC ((*declaration_buffer), length);

      // See if we may be reading in the end string.
      if (end_flag is EXCELLENT)
      {
        if (end_string[end_pos] isnt ch)
          end_flag = BOGUS;             // Doesn't really match end string   

        else 
        {
          end_pos++;

          if (end_string[end_pos] is 0) 
          {
            done = EXCELLENT;           // We've hit the end of declaration  
            length -= strlen(end_string)+1;
          } // if (end_string[end_pos] is 0...

        } // else...

      } // if (end_flag is EXCELLENT...

      // See if we've found a potential start to the end string
      if (ch is end_string[0]) 
      {
        end_flag = EXCELLENT;

        if (end_string[1] is 0) 
        {
          done = EXCELLENT;
          length -=2;
        } // if (end_string[1] is 0...

      } // if (ch is end_string[0]...

    } // else... we're still in a valid part of the buffer

  } // while(!done...

  if (ch is EOF) return(EARLY_END_OF_FILE);

  (*declaration_buffer)[length] = 0;    // Mark the end of declaration in buf

  // Remember how long the active declaration is!
  current_declaration_length = length+1;

  return(BOGUS);
}
//
// iparser::Read_Until_String
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Append_Good_Char
//
// Stick a pre-approved character onto the end of a string.  This version of 
// the function removes excess white space, too.
//
// Modifies: Can grow (REALLOC) the string.
//
void iparser::Append_Good_Char
(
	char *to_string,                     // String to which to add a character
	int *pos,                             // Current length of the string
	char the_char                         // New character
)

{                          
  // Local Variables //////////////////////////////////////////////////////////
//CUTME  char *temp;
  /////////////////////////////////////////////////////////////////////////////

	if (the_char is ' ') 
	{
		if ((*pos) is 0) return;

		if (to_string[(*pos)-1] is ' ')

		return;
	}

	to_string[(*pos)] = the_char;
	(*pos)++;                     
  
//CUTME  temp = (*to_string) = (char *) REALLOC ((*to_string), (*pos)+1);
//CUTME  if (temp is NULL) Report_Error("YUCK. Bad. Puke.", REPORT_ERROR);
}
//
// iparser::Append_Good_Char
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Append_Quote_Char
//
// Stick a pre-approved character onto the end of a string.  This version does
// NOT remove excess white space.
//
// Modifies: Can grow (REALLOC) the string.
//
void iparser::Append_Quote_Char
(
	char *to_string,                     // String to which to add a character
	int *pos,                             // Current length of the string
	char the_char                         // New character
)

{
	// Local Variables //////////////////////////////////////////////////////////
//CUTME	char *temp;
	/////////////////////////////////////////////////////////////////////////////

	to_string[(*pos)] = the_char;
	(*pos)++;
//CUTME	temp =   (*to_string) = (char *) REALLOC ((*to_string), (*pos)+1);
//CUTME	if (temp is NULL) Report_Error("YUCK. Bad. Puke.", REPORT_ERROR);
}
//
// iparser::Append_Quote_Char
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Save_Only_Alpha_Numeric
//
// Go through a buffer of characters, and save only the alpha-numeric chars.
//
// Can replace original buffer
//
char * iparser::Save_Only_Alpha_Numeric
(
	char *a_string,						// The string to modify
	int (*check_char_fcn)(char),		// The "Save Char" function
	int source_length					// How big is the source buffer
)

{
	// Local Variables //////////////////////////////////////////////////////////
	int i=0;							// Index to position in line         
	int good=0;							// Index to last good character      
	int char_type;						// What sort of character is this?   
	char *new_string;					// String with only good stuff in it 
	/////////////////////////////////////////////////////////////////////////////

	// Make sure there's no zero length
	_ASSERTE(source_length > 0);

	// Allocate room for string and end mark (and one for good luck!)
	// Note that the biggest the good stuff can grow to is 3 times the
	// original (ALL SYMBOLS).
	new_string = (char *)MALLOC((3*source_length)+1);      
	new_string[0] = 0;                    // Mark end of new string            


	#ifdef _DEBUG
	sint32 finite_loop=0; 
	#endif

	while (a_string[i] isnt 0)         // Check the whole string for good 
	{
		Assert(++finite_loop < 100000); // make sure we catch infinite loogs
		// Check the current character with the provided function
		if ((char_type = (*check_char_fcn)(a_string[i])) is ALNUM) 
		{
			// Save the confirmed good characters 
			Append_Good_Char(new_string, &good, a_string[i]);

		} // if ((char_type = (*check_char_fcn)(a_string[i])) is ALNUM...

		// Handle non-confirmed good characters
		else if (char_type is SYMBOL) 
		{
			// Save things in quotes                    
			if (a_string[i] is '"') 
			{
				
				Append_Quote_Char(new_string, &good, a_string[i]);

				
				i++;

				
				
				while ((a_string[i] isnt 0) && (a_string[i] isnt '"')) 
				{
					Assert(++finite_loop < 100000); // make sure we catch infinite loogs
					Append_Quote_Char(new_string, &good, a_string[i]);
					i++;                          // Increment source counter

				} // while ((a_string[i] isnt 0) && (a_string[i] isnt '"')...

				
				if (a_string[i] is '"')  
					Append_Quote_Char(new_string, &good, a_string[i]);

			}  // This was a quote symbol 
			else 
			// This was some other Symbol character!
			{

				// Insert the symbol character with a space before and after.        
				// Add space before is needed                                        
				Append_Good_Char(new_string, &good, ' ');

				// Add symbol                                                        
				Append_Good_Char(new_string, &good, a_string[i]);

				// Add space after                                                   
				Append_Good_Char(new_string, &good, ' ');

			}  // This is a non-quote symbol   

		} // This was a symbol of some sort 

		else // Treat any unknown character as a blank space 
			Append_Good_Char(new_string, &good, ' ');

		// Increment the source counter
		i++;

	} // While not at end of the string 

	// Resize the string to be big enough for an end-string marker
//CUTME	new_string = (char *) REALLOC (new_string, good+1);
	new_string[good] = 0;                 // End line properly.               

//CUTME	temp = (*a_string);
//CUTME	(*a_string) = new_string;             // Save new string.                 
//CUTME	FREE(temp);                           // Release extra memory.             

	// send back the good stuff
	return new_string;
}
//
// iparser::Save_Only_Alpha_Numeric
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Parse_Int
//
// We have an int declaration.  Set up a variable, and parse its value.
//
// Returns: Error code (IPARSE_NO_ERR, HEINOUS, etc.)
//
int iparser::Parse_Int
(
  int num_symbols,                      // Number of symbols in string
  int *int_symbols,                     // Integer values of all symbols
  double *double_symbols,               // Double value of all symbols
  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH], // String for symbols
  int *symbol_types                     // Pre-determined types of symbols
)
           
{
  // Local Variables //////////////////////////////////////////////////////////
  variable_struct *new_int_variable;    // Ptr to mem for variable              
  int error = IPARSE_NO_ERR;                   // Was there a bad declaration?      
  int is_integer;                       // Is the expression an int?       
  /////////////////////////////////////////////////////////////////////////////

  // FIRST, see if this is a simple assignment of an expression to an int  
  if (!strcmp(string_symbols[1], "=")) 
  {
    // Allocate a new variable
    new_int_variable = Get_New_Variable(INT_VARIABLE, string_symbols[0]);

    // Compute its value with the expression evaluator
    new_int_variable->double_value = 
      expression(this,2,num_symbols-1,int_symbols,
                 double_symbols,string_symbols,
                 symbol_types, &is_integer, &(new_int_variable->int_value), 
                 &error);
  }

  // SECOND, check to see if this is a decalaration w/o assignment          
  else if (num_symbols is 1) 
  {
    // Just allocate a new int variable.  Don't give an special
    new_int_variable = Get_New_Variable(INT_VARIABLE, string_symbols[0]);
  }

  else return(HEINOUS);                 // Not a valid integer variable declr   

  // Put the variable in the list 
  Insert_Variable(new_int_variable); 

  return(error);                        // Say it's OK                       
}
//
// iparser::Parse_Int
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Parse_Char
//
// We have an char declaration.  Set up a variable, and parse its value.
//
// Returns: Error code (IPARSE_NO_ERR, HEINOUS, etc.)
//
int iparser::Parse_Char
(
  int num_symbols,                      // Number of symbols in string
  int *int_symbols,                     // Integer values of all symbols
  double *double_symbols,               // Double value of all symbols
  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH], // String for symbols
  int *symbol_types                     // Pre-determined types of symbols
)

{
  // Local Variables //////////////////////////////////////////////////////////
  variable_struct *new_char_variable;   // Ptr to mem for variable             
  int error = IPARSE_NO_ERR;                 // Was there a bad declaration?      
  /////////////////////////////////////////////////////////////////////////////

  // FIRST, see if this is a simple assignment of a single character         
  if (!strcmp(string_symbols[1], "=")) 
  {

    new_char_variable = Get_New_Variable(CHAR_VARIABLE, string_symbols[0]);

    // Assign first character of 4th symbol to int value. No error checking  
    new_char_variable->int_value = (int) string_symbols[3][0];

  }

  // SECOND, check to see if this is a decalaration w/o assignment           
  else if (num_symbols is 1) {
    new_char_variable = Get_New_Variable(CHAR_VARIABLE, string_symbols[0]);
  }

  // THIRD, check to see if a string is being declared, w/o assignment       
  else if ((!strcmp(string_symbols[1], "["))&&(num_symbols is 4)) 
  {
    new_char_variable = Get_New_Variable(STRING_VARIABLE, string_symbols[0]);
          
    // Allocate room for string          
    new_char_variable->string_value  = (char *)MALLOC(int_symbols[2]);
    new_char_variable->int_value  = int_symbols[2];
  }

  // FOURTH, check to see if a string is being declared, with assignment     
  else if ((!strcmp(string_symbols[1], "["))&&(num_symbols is 6)) {
    new_char_variable = Get_New_Variable(STRING_VARIABLE, string_symbols[0]);
          
    // Allocate room for string          
    new_char_variable->string_value  = (char *)MALLOC(int_symbols[2]);
    new_char_variable->int_value  = int_symbols[2];

    strncpy(new_char_variable->string_value, &(string_symbols[5][1]), 
            strlen(string_symbols[5])-2);

    // Put a string-end marker on the end
    new_char_variable->string_value[strlen(string_symbols[5])-2] = 0;
  }

  else return(HEINOUS);                 // Not a valid integer variable declr   

  // Put the variable in the list 
  Insert_Variable(new_char_variable);    

  return(error);                        // Say it's OK  
  
}
//
// iparser::Parse_Char
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Parse_Double
//
// We have a double declaration.  Set up a variable, and parse its value.
//
// Returns: Error code (IPARSE_NO_ERR, HEINOUS, etc.)
// Modifies: Fills in the values for a double variable
//
int iparser::Parse_Double
(
  int num_symbols,                      // Number of symbols in string
  int *int_symbols,                     // Integer values of all symbols
  double *double_symbols,               // Double value of all symbols
  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH], // String for symbols
  int *symbol_types,                    // Pre-determined types of symbols
  int the_type                          // Particular type of double
)

{
  // Local Variables //////////////////////////////////////////////////////////
  variable_struct *new_double_variable; // Ptr to mem for variable              
  int error = IPARSE_NO_ERR;                 // Was there a bad declaration?      
  int is_integer;                       // Is the expression an int?       
  /////////////////////////////////////////////////////////////////////////////

  // FIRST, see if this is a simple assignment of an expression to an double 
  if (!strcmp(string_symbols[1], "=")) 
  {

    new_double_variable = Get_New_Variable(the_type, string_symbols[0]);

    new_double_variable->double_value = 
      expression(this,2,num_symbols-1,int_symbols,
                 double_symbols,string_symbols,
                 symbol_types, &is_integer, &(new_double_variable->int_value), 
                 &error);

   }

  // SECOND, check to see if this is a decalaration w/o assignment           
  else if (num_symbols is 1) {
    new_double_variable = Get_New_Variable(the_type, string_symbols[0]);
    }

  else return(HEINOUS);                 // Not a valid integer variable declr   

  // Put the variable in the list 
  Insert_Variable(new_double_variable);  

  return(error);                        // Say it's OK                       
}
//
// iparser::Parse_Double
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Assign_To_Variable
//
// There is an assign statement in the parse buffer/file.  Put the value
// of the expression in the chosen variable.
//
// Returns: Error code (IPARSE_NO_ERR, BAD_VARIABLE, HEINOUS, etc.)
// Modifies: Value of the specified variable
//
int iparser::Assign_To_Variable
(
  char *declaration_string,             // The whole original declaration
  int num_symbols,                      // Number of symbols in string
  int *int_symbols,                     // Integer values of all symbols
  double *double_symbols,               // Double value of all symbols
  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH], // String for symbols
  int *symbol_types                     // Pre-determined types of symbols
)

{
  // Local Variables //////////////////////////////////////////////////////////
  variable_struct *assign_variable;     // Ptr to mem for variable              
  int error = IPARSE_NO_ERR;                 // Was there a bad declaration?      
  int is_integer;                       // Is expr really an int?      
  /////////////////////////////////////////////////////////////////////////////

  // What variable are we talking about?
  assign_variable = Which_Variable(declaration_string);

  if (assign_variable is NULL) return(BAD_VARIABLE);

  switch (assign_variable->variable_type)
  {
  case INT_VARIABLE :
    assign_variable->double_value = 
      expression(this,
        1,num_symbols-1,int_symbols,
        double_symbols,string_symbols,
        symbol_types, &is_integer, &(assign_variable->int_value), 
        &error);
    break;
  case FLOAT_VARIABLE :
    assign_variable->double_value = 
      expression(this,
        1,num_symbols-1,int_symbols,
        double_symbols,string_symbols,
        symbol_types, &is_integer, &(assign_variable->int_value), 
        &error);
    break;
  case DOUBLE_VARIABLE :
    assign_variable->double_value = 
      expression(this,
        1,num_symbols-1,int_symbols,
        double_symbols,string_symbols,
        symbol_types, &is_integer, &(assign_variable->int_value), 
        &error);
    break;
  case CHAR_VARIABLE :
    assign_variable->int_value = (int) string_symbols[2][0];
    break;
  case STRING_VARIABLE :
    strncpy(assign_variable->string_value, &(string_symbols[1][1]), 
            strlen(string_symbols[1])-2);
    assign_variable->string_value[strlen(string_symbols[1])-2] = 0;
    break;
  default :
    error = HEINOUS;
    break;
  };

  return(error);                        // Say it's OK                       
}
//
// iparser::Assign_To_Variable
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Parse_Pound_Define
//
// Handle a #define "Compiler Directive".  This can be an int, a double, or a
// string
//
// Returns: Error code (IPARSE_NO_ERR, HEINOUS, etc.)
// Modifies: Creates and fills in a value for an int, double, or string
//
int iparser::Parse_Pound_Define
(
  int num_symbols,                      // Number of symbols in string
  int *int_symbols,                     // Integer values of all symbols
  double *double_symbols,               // Double value of all symbols
  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH], // String for symbols
  int *symbol_types                     // Pre-determined types of symbols
)

{
  // Local Variables //////////////////////////////////////////////////////////
  variable_struct *new_variable;        // Ptr to mem for variable              
  int error = IPARSE_NO_ERR;                 // Was there a bad declaration?      
  int is_integer;                       // Is it really an int?        
  /////////////////////////////////////////////////////////////////////////////

  // FIRST, see if this is a string define             
  if (string_symbols[1][0] is '"') 
  {

    new_variable = Get_New_Variable(STRING_VARIABLE, string_symbols[0]);
          
    
    new_variable->string_value 
      = (char *)MALLOC(strlen(string_symbols[1])-1);
    new_variable->int_value  = strlen(string_symbols[1])-1;
    strncpy(new_variable->string_value, &(string_symbols[1][1]), 
            strlen(string_symbols[1])-2);

    new_variable->string_value[strlen(string_symbols[1])-2] = 0;
  }

  
  else 
  {
    new_variable = Get_New_Variable(DOUBLE_VARIABLE, string_symbols[0]);
    new_variable->double_value = expression(this, 1,num_symbols-1,
                                         int_symbols,
                                         double_symbols,string_symbols,
                                         symbol_types, &is_integer,
                                         &(new_variable->int_value), &error);

    
    if (is_integer)
      new_variable->variable_type = INT_VARIABLE;
    }

  
  Insert_Variable(new_variable);        
  
  return(error);                        
}

















int iparser::Parse_Include
(
  int num_symbols,                      
  int *int_symbols,                     
  double *double_symbols,               
  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH], 
  int *symbol_types                     
)

{
  
  int error = IPARSE_NO_ERR;                 
  int file_name_length;               
  char file_name[IPARSE_MAX_STRING_LENGTH];  

  

  if (num_symbols isnt 1)
    return(HEINOUS);

  
  file_name_length = strlen(string_symbols[0]);

  
  if ((string_symbols[0][0] isnt '"') or (string_symbols[0][file_name_length-1] isnt '"'))
  {
    return(HEINOUS);
  }

  
  strncpy(file_name, &(string_symbols[0][1]), file_name_length-2);

  
  file_name[file_name_length-2] = 0;

  


    

    
 
    error = Parse_File(file_name);


    
 


  return(error);                        
}















void iparser::Append_Field_To_Structure
(
  struct_list_element *new_struct,      
  int *current_symbol,                  
  int num_symbols, 
  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH], 
  int field_type,                       
  int the_field_length                  
)

{
  
  field_def *new_field;                 
  field_def *prev_field;                
  

  
  new_field = Get_New_Field(field_type, string_symbols[(*current_symbol)+1]);

  
  new_struct->struct_length += the_field_length; 
  new_field->field_length = the_field_length;

  
  if (new_struct->the_definition is NULL)
    new_struct->the_definition = new_field; 
  
  else 
  
  {
    prev_field = new_struct->the_definition;


#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    while (prev_field->next_field isnt NULL) 
    {
Assert(++finite_loop < 100000); 
      prev_field = prev_field->next_field;    
    }
    prev_field->next_field = new_field; 

  }


#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif
  
  while (strcmp(string_symbols[(*current_symbol)++], ";")) 
  {
Assert(++finite_loop < 100000); 
  }                                     
}

















int iparser::Add_Next_Field
(
  struct_list_element *new_struct,      
  int *current_symbol,                  
  int num_symbols, 
  int *int_symbols,                     
  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH]  
)

{
  if (!strcmp(string_symbols[(*current_symbol)], "char")) 
  {
    if (!strcmp(string_symbols[(*current_symbol)+2], "["))
      Append_Field_To_Structure(new_struct, current_symbol,num_symbols, 
                                       string_symbols, STRING_VARIABLE, 
                                       int_symbols[(*current_symbol)+3]);
    else 
      Append_Field_To_Structure(new_struct, current_symbol, num_symbols,
                                       string_symbols, CHAR_VARIABLE, 1);
  }
  else if (!strcmp(string_symbols[(*current_symbol)], "int")) 
  {
    Append_Field_To_Structure(new_struct, current_symbol, num_symbols, 
                                     string_symbols, INT_VARIABLE, sizeof(int));
  }
  else if (!strcmp(string_symbols[(*current_symbol)], "float")) 
  {
    Append_Field_To_Structure(new_struct, current_symbol, num_symbols, 
                                     string_symbols, FLOAT_VARIABLE,sizeof(float));
  }
  else if (!strcmp(string_symbols[(*current_symbol)], "double")) 
  {
    Append_Field_To_Structure(new_struct, current_symbol, num_symbols,
                                     string_symbols, DOUBLE_VARIABLE,sizeof(double));
  }
  else Report_Error("Bad field name", REPORT_ERROR);

  return(*current_symbol);              
}
















int iparser::Parse_Struct
(
  int num_symbols,                      
  int *int_symbols,                     
  double *double_symbols,               
  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH], 
  int *symbol_types                     
)

{
  
  struct_list_element *new_struct;      
  int error = IPARSE_NO_ERR;                 
  int current_symbol;                   
  

  new_struct = Get_New_Struct(string_symbols[0]);
 
  current_symbol = 2;


#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif
  while (current_symbol < num_symbols-1) 
  {
Assert(++finite_loop < 100000); 
    Add_Next_Field(new_struct, &current_symbol, num_symbols, int_symbols,
                          string_symbols);
  }

  Insert_Struct(new_struct);

  return(error);                        
}

















int iparser::Fill_Individual_Field
(
  char *buf,                            
  int the_type,                         
  int *int_symbols,                     
  double *double_symbols,               
  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH], 
  int *symbol_types,                    
  int *current_symbol                   
)

{
  
  double temp_double;                   
  float temp;                           
  int temp_int;                         
  int is_integer;                       
  int error;                            
  int field_end_symbol;                 
  

  
  field_end_symbol = *current_symbol;
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

  
  while ((string_symbols[field_end_symbol][0] isnt ';') and
         (string_symbols[field_end_symbol][0] isnt ','))
  {
Assert(++finite_loop < 100000); 
    field_end_symbol++;
  }

  sint32 symbol_len; 
  switch (the_type) 
  {

    case CHAR_VARIABLE :
      (*current_symbol)++;                
      
      
      (*buf) = string_symbols[(*current_symbol)][0];

      (*current_symbol)+=2;               
      (*current_symbol)++;                
      break;

    case STRING_VARIABLE :
      
     symbol_len = strlen(string_symbols[(*current_symbol)])-2;

     if (symbol_len <= 0) { 
         Report_Error("Missing array field", REPORT_FATAL);
         return(HEINOUS); 
     } 

     Assert(0 < symbol_len); 

     strncpy(buf, &(string_symbols[(*current_symbol)][1]), 
        symbol_len);
      buf[symbol_len] = 0;
      (*current_symbol)++;                
      (*current_symbol)++;                
      break;

    case INT_VARIABLE :

      
      expression(this, *current_symbol, field_end_symbol-1,
             int_symbols,
             double_symbols,string_symbols,
             symbol_types, &is_integer,
             &(temp_int), &error);

      
      memcpy(buf, (char *) (&(temp_int)), sizeof(int));

      
      (*current_symbol) = field_end_symbol+1;
      
      break;

    case FLOAT_VARIABLE :

      
      temp = (float) expression(this, *current_symbol,field_end_symbol-1,
             int_symbols,
             double_symbols,string_symbols,
             symbol_types, &is_integer,
             &(temp_int), &error);

      
      memcpy(buf, (char *) (&temp), sizeof(float));

      
      (*current_symbol) = field_end_symbol+1;

      break;

    case DOUBLE_VARIABLE :

      
      temp_double = expression(this, *current_symbol,field_end_symbol-1,
             int_symbols,
             double_symbols,string_symbols,
             symbol_types, &is_integer,
             &(temp_int), &error);
    
      memcpy(buf, (char *) (&temp_double), sizeof(double));



      
      (*current_symbol) = field_end_symbol+1;
      break;

    default : return(HEINOUS);
  }

  return(IPARSE_NO_ERR);
}
















void iparser::Fill_All_Fields
(
  struct_list_element *the_struct,      
  char **buf,                           
  int *int_symbols,                     
  double *double_symbols,               
  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH], 
  int *symbol_types,                    
  int *current_symbol                   
)

{
  
  field_def *current_field;             
  

  current_field = the_struct->the_definition;
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

  while (current_field isnt NULL) 
  {
Assert(++finite_loop < 100000); 
    Fill_Individual_Field( (*buf), current_field->field_type, 
                             int_symbols, double_symbols, string_symbols, 
                             symbol_types, current_symbol);

    (*buf) += current_field->field_length;     
    current_field = current_field->next_field; 
  }

}
















int iparser::Parse_Struct_Array
(
  char *declaration_string,             
  int num_symbols,                      
  int *int_symbols,                     
  double *double_symbols,               
  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH], 
  int *symbol_types                     
)           
         
{
  
  struct_list_element *the_struct;      
  variable_struct *new_variable;        
  int error = IPARSE_NO_ERR;                 
  int current_symbol;                   
  int array_size;                       
  char *new_field_ptr;                  
  int end_of_index = 3;                 
  int is_integer;                       
  

  the_struct = Which_Struct( declaration_string);
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

  
  while(string_symbols[end_of_index][0] isnt ']') 
  {
Assert(++finite_loop < 100000); 
    end_of_index++;
  }

  
  expression(this, 2,end_of_index-1,int_symbols,
             double_symbols,string_symbols,
             symbol_types, &is_integer, 
             &(array_size), 
             &error);

  
  new_variable = Get_New_Variable(STRUCT_ARRAY_VARIABLE, string_symbols[0]);

  
  new_variable->the_struct_el = the_struct;

  
  new_variable->string_value               
    = (char *) MALLOC((the_struct->struct_length)*(array_size));

  
  new_field_ptr = new_variable->string_value;  

  
  current_symbol = end_of_index + 4;    

  
  while (current_symbol < num_symbols-1) 
  {
Assert(++finite_loop < 100000); 
    
    Fill_All_Fields(the_struct, &new_field_ptr, int_symbols, 
                           double_symbols, string_symbols, symbol_types, 
                           &current_symbol);

    
    new_variable->struct_element_count++;

  }

  Insert_Variable(new_variable);

  return(error);                        
}
















int iparser::Find_Matched_Bracket
(
  char *parse_buffer,                   
  int parse_pos,                        
  int buffer_length                     
) 

{
  
  int done = BOGUS;
  int left_brackets = 1;                
  

  #ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

  while (!done) 
  {
Assert(++finite_loop < 100000); 
    if (parse_pos is buffer_length) 
      done = EXCELLENT;
    else 
    {
      
      if (parse_buffer[parse_pos] is '{')
        left_brackets++;

      
      if (parse_buffer[parse_pos] is '}') 
      {
        
        left_brackets--;

        
        if (left_brackets is 0)
          done = EXCELLENT;

      } 

    } 

    if (!done) parse_pos++;

  } 

  return(parse_pos);
}













void iparser::Get_Next_Statement_Start_And_End
(
  char *parse_buffer,                   
  int *parse_pos,                       
  int buffer_length,                    
  int *the_start,                       
  int *the_end                          
)

{
  
  int statement_start, statement_end;
  char next_string[200];
  char *buf;                            
  int next_type = NO_DECLARATION;
  

  buf = (char *)MALLOC(1);              

  next_type = Search_For_Next_Declaration(parse_buffer, parse_pos, 
                                                 buffer_length,
                                                 next_string);

  FREE(buf);                            

  statement_start = *parse_pos;

  if (next_type is CODE_BRACKET)
    statement_end = Find_Matched_Bracket(parse_buffer, statement_start,
                                                buffer_length);

  *the_start = statement_start;
  *the_end = statement_end;

}















int iparser::Get_Else_Start_And_End
(
  char *parse_buffer,                   
  int *temp_pos,                        
  int buffer_length,                    
  int *the_start,                       
  int *the_end                          
)

{
  
  char next_string[200];
  char *buf;                            
  int next_type = NO_DECLARATION;
  

  buf = (char *)MALLOC(1);              

  next_type = Search_For_Next_Declaration(parse_buffer, temp_pos, 
			 buffer_length,next_string);

  FREE(buf);                            

  if (next_type is ELSE_CODE) 
  {

    Get_Next_Statement_Start_And_End(parse_buffer, temp_pos, buffer_length, 
             the_start, the_end);
    return(EXCELLENT);
  }

  else return(BOGUS);
}
















int iparser::Parse_Conditional_Code
(
  char *parse_buffer,                   
  int *parse_pos,                       
  int buffer_length,                    
  int num_symbols,                      
  int *int_symbols,                     
  double *double_symbols,               
  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH], 
  int *symbol_types                     
)

{
  
  int error = IPARSE_NO_ERR;                 
  int is_integer;                       
  int conditional;                      
  int is_else = BOGUS;                  
  int then_start, then_end;
  int else_start, else_end;
  int temp_pos;
  

  
  if (!strcmp(string_symbols[0], "(")) 
  {

    
    expression(this,
               1,num_symbols-1,int_symbols,
               double_symbols,string_symbols,
               symbol_types, &is_integer, &(conditional), 
               &error);

    
    temp_pos = *parse_pos;
    Get_Next_Statement_Start_And_End(
		parse_buffer, &temp_pos, buffer_length, 
        &then_start, &then_end);

    
    temp_pos = then_end+1;
    is_else = Get_Else_Start_And_End(parse_buffer, &temp_pos, buffer_length, 
         &else_start, &else_end);

    if (!is_else) else_end = then_end;  

    if (conditional)
      Do_Parse(parse_buffer, &then_start, then_end);
    else
      Do_Parse(parse_buffer, &else_start, else_end);

    *parse_pos = else_end+1;            

    }

  else return(HEINOUS);                 

  return(error);                        
}
















int iparser::Next_Symbol
(
  char *the_string, 
  const int the_string_len, 
  int *place_in_the_string, 
  char *new_symbol, 
  const int new_symbol_len
)

{
  
  int type = STR_TOK;
  int is_number = BOGUS;                
  char *new_symbol_end; 
  

  new_symbol_end = new_symbol + new_symbol_len; 

  Assert(*place_in_the_string < the_string_len); 

  if (the_string[*place_in_the_string] is ' ')  
     (*place_in_the_string)++;          


  Assert(*place_in_the_string < the_string_len); 
  if (isdigit(the_string[*place_in_the_string])) 
  {
    type = INT_TOK;
    is_number = EXCELLENT;
  }

  
  Assert(*place_in_the_string < the_string_len); 
  if (the_string[*place_in_the_string] is '"') 
  {

    Assert(new_symbol < new_symbol_end); 
    (*new_symbol) = the_string[*place_in_the_string];
    new_symbol++;                       // Move to next character in symbol  
    (*place_in_the_string)++;           // Move to next character in string  

    while ((the_string[*place_in_the_string] isnt 0) 
     && (the_string[*place_in_the_string] isnt '"')) 
    {

      Assert(new_symbol < new_symbol_end); 
      Assert(*place_in_the_string < the_string_len); 
      (*new_symbol) = the_string[*place_in_the_string];

      (*place_in_the_string)++;         
      new_symbol++;                     
    }

    Assert(*place_in_the_string < the_string_len); 
    if (the_string[*place_in_the_string] is '"') 
    {

       Assert(new_symbol < new_symbol_end); 
      (*new_symbol) = the_string[*place_in_the_string];

      (*place_in_the_string)++;         // Move to next character in string  
      new_symbol++;                     // Move to next character in symbol  
    }

  }

  else 
  // Keep all of the next bunch of alpha-numeric characters as a symbol     
  Assert(*place_in_the_string < the_string_len); 
  while(IParser_Find_Char_Type(the_string[*place_in_the_string]) isnt BOGUS) 
  {

    Assert(new_symbol < new_symbol_end); 
    Assert(*place_in_the_string < the_string_len); 
    if ((!isdigit(the_string[*place_in_the_string]))
         && (the_string[*place_in_the_string] isnt '.')) 
    {
      is_number = HEINOUS;
      type = STR_TOK;
    }

    if (is_number isnt HEINOUS) 
    {
      if ((type is DOUBLE_TOK) && (the_string[*place_in_the_string] is '.')) 
      {
        is_number = HEINOUS;
        type = STR_TOK;
      }
      else if (the_string[*place_in_the_string] is '.') type = DOUBLE_TOK;
    }

    // Copy the symbol's character to the symbol array                       
    (*new_symbol) = the_string[*place_in_the_string];

    (*place_in_the_string)++;           // Move to next character in string  
    new_symbol++;                       // Move to next character in symbol  
  }

   Assert(new_symbol < new_symbol_end); 
  (*new_symbol) = 0;                    // Mark the end of the symbol string 
  if (the_string[*place_in_the_string] isnt 0)  // End of initial string     
    (*place_in_the_string)++;           // Move to next symbol in the string 

  return(type);
}
//
// iparser::Next_Symbol
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Parse_New_Command
//
// Taken a command, and parse the sucker into symbols
//
// Modifies: Modifies symbol arrays
//
void iparser::Parse_New_Command
(
  char *command_str,                    // The ascii command string
  const int command_str_len,
  int *num_symbols,                     // Number of symbols  
  int *int_symbols,                     // Integer values of all symbols
  double *double_symbols,               // Double value of all symbols
  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH], // String for symbols
  int *symbol_types                     // Pre-determined types of symbols
)
        
{
  // Local Variables //////////////////////////////////////////////////////////
  int i = 0;
  int tok_type;
  int tmp_int;
  float tmp_float;
  int parsed;
  /////////////////////////////////////////////////////////////////////////////

  (*num_symbols) = 0;

  while (command_str[i] isnt 0) 
  {

      Assert(i < command_str_len); 
      Assert(*num_symbols < MAX_SYMBOLS); 

    // Find the next symbol in the string                                    
    tok_type = Next_Symbol(command_str, command_str_len, &i, 
        string_symbols[(*num_symbols)], IPARSE_MAX_STRING_LENGTH);

    // Extract numbers from the numeric symbols if needed                    
    if (tok_type is INT_TOK) 
    {
		//REM280299: added error checking, bad values can cause infinite loops
		//int_symbols[(*num_symbols)] = atoi(string_symbols[(*num_symbols)]);
		//double_symbols[(*num_symbols)] = int_symbols[(*num_symbols)];
		tmp_int = HEINOUS;
		parsed = sscanf(string_symbols[(*num_symbols)], "%d", &tmp_int);
		if (parsed==1)
			{
				int_symbols[(*num_symbols)] = tmp_int;
				double_symbols[(*num_symbols)] = tmp_int;
			}
		else
			int_symbols[(*num_symbols)] = HEINOUS;
    }
    else int_symbols[(*num_symbols)] = HEINOUS;

    if (tok_type is DOUBLE_TOK)
	{
		//REM280299: added error checking, bad values can cause infinite loops
		//double_symbols[(*num_symbols)] = atof(string_symbols[(*num_symbols)]);
		tmp_float = HEINOUS;
		parsed = sscanf(string_symbols[(*num_symbols)], "%f", &tmp_float);
		if (parsed==1)
			{
				double_symbols[(*num_symbols)] = tmp_float;
			}
		else
			double_symbols[(*num_symbols)] = HEINOUS;
	}
    else double_symbols[(*num_symbols)] = HEINOUS;

    symbol_types[(*num_symbols)] = tok_type;

    (*num_symbols)++;
  }

}
//
// iparser::Parse_New_Command
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Parse_Declaration
//
// Given a single declaration or "line of code", create or modify the variables
// we've created
//
// Returns: Error Code (IPARSE_NO_ERR, BAD_VARIABLE, etc.)
// Modifies: 
//
int iparser::Parse_Declaration
(
  char *parse_buffer,                   // Raw buffer
  int *parse_pos,                       // Position in buffer
  int buffer_length,                    // Length of buffer
  int declaration_type,                 // What sort of declaration is this?
  char *declaration_string,             // Raw buffer containing declaration    
  char **buf,                            // Cleaned-up buffer
	int current_declaration_length		// How long is the current declaration?
)
              
{
  // Local Variables //////////////////////////////////////////////////////////
  int int_symbols[MAX_SYMBOLS];         // Any symbols which are integers    
  int symbol_types[MAX_SYMBOLS];        // what kind of symbol is it         
  double double_symbols[MAX_SYMBOLS];   // Any symbols which are doubles     
  char string_symbols[MAX_SYMBOLS][IPARSE_MAX_STRING_LENGTH];  // Strings           
  int num_symbols;                      // How many symbols there are       
  char *good_string;					// Just the good stuff
  /////////////////////////////////////////////////////////////////////////////

  if (declaration_type is NO_DECLARATION) return(IPARSE_NO_ERR);

  good_string = Save_Only_Alpha_Numeric(*buf, IParser_Find_Char_Type,
	  current_declaration_length);

  int good_str_len = strlen(good_string); 
  Parse_New_Command(good_string, good_str_len, &num_symbols,
        int_symbols, 
        double_symbols, 
        string_symbols,
        symbol_types);

  // Trash the good string
  // FIXME: Let's just do a good string once per file!!!
  FREE(good_string);

  switch (declaration_type) 
  {
  case INT_VARIABLE :
    return(Parse_Int( num_symbols,
         int_symbols,double_symbols,string_symbols,symbol_types));
    break;
  case FLOAT_VARIABLE :
    return(Parse_Double( num_symbols,
      int_symbols,double_symbols,string_symbols,symbol_types,
      FLOAT_VARIABLE));
    break;
  case DOUBLE_VARIABLE :
    return(Parse_Double( num_symbols,
      int_symbols,double_symbols,string_symbols,symbol_types,
      DOUBLE_VARIABLE));
    break;
  case CHAR_VARIABLE :
    return(Parse_Char( num_symbols,
         int_symbols,double_symbols,string_symbols,symbol_types));
    break;
  case VARIABLE_ASSIGN :
    return(Assign_To_Variable(declaration_string, num_symbols,
         int_symbols,double_symbols,string_symbols,symbol_types));
    break;
  case DEFINE_DECLARATION :
    return(Parse_Pound_Define( num_symbols,
         int_symbols,double_symbols,string_symbols,symbol_types));
    break;
  case INCLUDE_DIRECTIVE :
    return(Parse_Include( num_symbols,
         int_symbols,double_symbols,string_symbols,symbol_types));
    break;
  case STRUCT_DEFINITION :
    return(Parse_Struct( num_symbols,
         int_symbols,double_symbols,string_symbols,symbol_types));
    break;
  case STRUCT_ARRAY_VARIABLE :
    return(Parse_Struct_Array(declaration_string, num_symbols,
         int_symbols,double_symbols,string_symbols,symbol_types));
    break;
  case CONDITIONAL_CODE :
    return(Parse_Conditional_Code(parse_buffer, parse_pos, buffer_length,
          num_symbols,
          int_symbols,double_symbols,string_symbols,
          symbol_types));
    break;
  default :
    return(BAD_VARIABLE);
    break;
  }

}
//
// iparser::Parse_Declaration
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Get_Termination_String
//
// Get the ending string for a particular type of definition
// INEFFICIENT, but so what?  Doesn't happen enough to matter.
//
// Returns: A string which should mark the end of this type of declaration
//
char *iparser::Get_Termination_String
(
  int declaration_type, 
  char *end_string
)

{

  switch (declaration_type) 
  {
  case DEFINE_DECLARATION :
    strcpy(end_string, "\n");
    break;
  case INCLUDE_DIRECTIVE :
    strcpy(end_string, "\n");
    break;
  case STRUCT_DEFINITION :
    strcpy(end_string, "};");
    break;
  case STRUCT_ARRAY_VARIABLE :
    strcpy(end_string, "#END_DATA");
    break;
  case CODE_BRACKET :
    strcpy(end_string, "}");
    break;
  case CONDITIONAL_CODE :
    strcpy(end_string, ")");
    break;
  case PARSE_FOR_LOOP :
    strcpy(end_string, ")");
    break;
  default :
    strcpy(end_string, ";");
    break;
  }

  return(end_string);
}
//
// iparser::Get_Termination_String
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Complete_Declaration
//
// Given what sort of declaration we have, finish reading it, and parse it
//
// Returns: Error code for reading and parsing.
//
int iparser::Complete_Declaration
(
	char *parse_buffer,                 // Ptr to source buffer
	int *parse_pos,                     // Position in source buffer
	int buffer_length,                  // Length of source buffer
	int declaration_type,               // What type of declaration?
	char *declaration_string,           // Full declaration string
	char *declaration_buffer 			// Used to copy single delcarations,
										// but cna be as long as whole source!
)
{
	// Local Variables //////////////////////////////////////////////////////////
	char *buf;                            // Buffer for reading declaration    
	char *start_buf;                      // Where was the start of the buffer 
	int error = IPARSE_NO_ERR;                 // Default to no error               
	char end_string[80];                  // String to end declaration         
	int current_declaration_length;		// How long is the current declaration?
	/////////////////////////////////////////////////////////////////////////////

	//  buf = (char *)MALLOC(1);              // Allocate room for end mark        */

	// Use the pre-set-aside declaration buffer to hold the text
	buf = declaration_buffer;

	// Read until we hit a semi-colon or other termination marker
	error = Read_Until_String(parse_buffer, parse_pos, 
        buffer_length, 
        Get_Termination_String(declaration_type,end_string), 
		&buf, current_declaration_length);     

	start_buf = buf;

	if (error is IPARSE_NO_ERR)                // Don't go on if messed up    
	{
		// Interpret the data in buffer     
		error = Parse_Declaration(parse_buffer, parse_pos, buffer_length,
						  declaration_type, 
						  declaration_string,
						  &buf,
						  current_declaration_length);    
	}

	//  FREE(buf);                            // Release the buffer's memory       

	return(error);                        // Let them know if bad              
}
//
// iparser::Complete_Declaration
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Do_Parse
//
// Given a buffer (pre-formatted as in the file reading process), parse it
// completely and supplement our parse domain with it.
//
// Returns: Error Code (BAD_VARIABLE, IPARSE_NO_ERR, etc.)
// Modifies: Parse domain variables.  Also changes parse_pos.
//
int iparser::Do_Parse
(
	char *parse_buffer,                 // PTR to properly formatted ASCII Buffer
	int *parse_pos,                     // Where to start in the buffer
	int buffer_length                   // Length of the buffer
)

{
	// Local Variables //////////////////////////////////////////////////////////
	int current_declaration_type;       // What is the current declaration   
	char declaration_string[IPARSE_MAX_STRING_LENGTH]; // First word of declaration   
	int error = BOGUS;                  // There was an error in declaration 
	char *declaration_buffer;			// Used to copy single delcarations,
										// but cna be as long as whole source!
	/////////////////////////////////////////////////////////////////////////////

	current_declaration_type = BAD_VARIABLE; // Just default to get into loop     

	// Set up a buffer into which to copy each declaration.  Make it as long
	// as the source, since we don't know a priori how big it is (and a comment
	// could be the whole length!
	declaration_buffer = (char *) MALLOC(buffer_length);

#ifdef _DEBUG
    sint32 finite_count=0; 
#endif _DEBUG

	while ((current_declaration_type isnt EARLY_END_OF_FILE) && (!error)) 	{

Assert(finite_count++ < 100000); 


        Assert(*parse_pos <= buffer_length);
        
		// Find the next known declaration   
		current_declaration_type = 
		Search_For_Next_Declaration(parse_buffer,
									 parse_pos,
									 buffer_length,
									 declaration_string);

		if (current_declaration_type is EARLY_END_OF_FILE)
		{
			// Get rid of the declaration buffer
			FREE(declaration_buffer);

			return(IPARSE_NO_ERR);
		}

		// Finish the variable declaration      
		error = Complete_Declaration(parse_buffer, parse_pos, 
            buffer_length, current_declaration_type, 
            declaration_string, declaration_buffer);
	}

	// Get rid of the declaration buffer
	FREE(declaration_buffer);

	return(error);                        // Relay an error code               
}
//
// iparser::Do_Parse
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Read_File_Into_Buffer
//
// Read the file into the buffer, but remove excess white space, skip comments,
// and separate alpha-numeric words from symbols by one white space
//
// Modifies: Fills the parse buffer with data from the parse file. Sets length
//           of the buffer.
//
void iparser::Read_File_Into_Buffer
(
  FILE *parse_file,                     // PTR to the file to parse
  char **parse_buffer,                  // Buffer to hold pre-processed ascii
  int *length                           // Length of buffer
)

{
  // Local Variables //////////////////////////////////////////////////////////
  signed char ch;
	char *next_char;

	int in_cpp_comment = FALSE;		        // C++ style comment (//...)
	int in_c_comment = FALSE;			        // C style comment (//...*/)
	int comment_flag = FALSE;			        // maybe a comment
	int comment_end_flag = FALSE;	        // maybe a comment end

	int last_type = NO_DECLARATION;
	int current_type;
  /////////////////////////////////////////////////////////////////////////////

	// seek to the end of file to determine the file length
	if (fseek(parse_file, 0, 2))
		Report_Error("Unable to seek to end of file while preprocessing script", 
                  REPORT_FATAL);

	// allocate the buffer
	if ((*parse_buffer = (char *) MALLOC (ftell(parse_file))) == NULL)
		Report_Error("Unable to allocate memory for buffer while preprocessing script", 
                  REPORT_FATAL);
	
	// starting location
	next_char = *parse_buffer;

	// seek back to the beginning
	if (fseek(parse_file, 0, 0))
		Report_Error("Unable to seek to start of file", REPORT_FATAL);

	// preprocess the file
	while(TRUE) 
  {
		ch = signed char (getc(parse_file));				      // Read a single character from file
		current_type = IParser_Find_Char_Type(ch);	// figure out what it is

		if (comment_flag) {
			comment_flag = FALSE;				      // clear comment flag
			if (ch == '/')
				in_cpp_comment = TRUE;
			else if (ch == '*')
				in_c_comment = TRUE;
			else {
				if (last_type == ALNUM)
					*next_char++ = ' ';			      // Add space character to buffer

				*next_char++ = '/';				      // Add first '/' character to buffer

  			// We are no longer in a comment
  			in_c_comment = in_cpp_comment = FALSE;
  		}
  	}

  	// Just eat up all the characters in a double slash comment
		if (in_cpp_comment) 
    {
			if ((ch == 10) || (ch == 13)) 
      {
				in_cpp_comment = FALSE;
        *next_char++ = ch;  // add character to buffer
			}
		}
		// Just eat up all the characters in a slash star comment
		else if (in_c_comment) 
    {

			if (comment_end_flag) 
      {
				if (ch == '/') in_c_comment = FALSE;
				else comment_end_flag = FALSE;
			}

			if (ch == '*')
				comment_end_flag = TRUE;
		}

		
		else switch (current_type) 
    {
		case END_OF_FILE:
			// compute length of output and return
			*length = next_char - *parse_buffer;
			return;

		case SYMBOL :
			if (ch == '/') {
				comment_flag = TRUE;
				break;
			}
			*next_char++ = ch;			// Add character to buffer
			break;

		default:
			*next_char++ = ch;			// Add character to buffer
			break;
		}

  }

}
//
// iparser::Read_File_Into_Buffer
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Trash_Variables
//
// Get rid of a variable, and all variables after it in its list
//
void iparser::Trash_Variables
(
  variable_struct *the_variable         // First variable in a list
)

{
  if (the_variable is NULL) return;

  Trash_Variables(the_variable->next_variable);

  if (the_variable->variable_name isnt NULL)
    FREE(the_variable->variable_name);  // Free mem used by name             

  if (the_variable->string_value isnt NULL)
    FREE(the_variable->string_value);   // Free mem used by data             

  FREE(the_variable);                   // Free mem used by variable            

}
//
// iparser::Trash_Variables
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Trash_Field_Definitions
//
// Get rid of a field definitions and all defs after it in its list
//
// Modifies: Nukes fields
//
void iparser::Trash_Field_Definitions
(
  field_def *the_field
)

{
  if (the_field is NULL) return;

  Trash_Field_Definitions(the_field->next_field);

  if (the_field->field_name isnt NULL)
    FREE(the_field->field_name);        // Free mem used by name             

  FREE(the_field);                      // Free mem used by field            
}
//
// iparser::Trash_Field_Definitions
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Trash_Structs
//
// Free up a struct and all structs after it in its list
//
// Modifies: Frees the memory
//
void iparser::Trash_Structs
(
  struct_list_element *the_struct
)

{
  if (the_struct is NULL) return;

  Trash_Structs(the_struct->next_struct);

  if (the_struct->struct_name isnt NULL)
    FREE(the_struct->struct_name);      // Free mem used by name             

  // Free mem used by field defs       
  if (the_struct->the_definition isnt NULL)
    Trash_Field_Definitions(the_struct->the_definition);        

  FREE(the_struct);                     // Free mem used by struct           

}
//
// iparser::Trash_Structs
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Get_Runtime_Struct_Field
//
// Return ptr to memory location of field of a structure variable  
// 
// Returns: Ptr to struct field
//
char *iparser::Get_Runtime_Struct_Field
(
	char *source_name                   // Whole name of the field
) 
           
{
	// Local Variables //////////////////////////////////////////////////////////
	variable_struct *the_variable_ptr;  // ptr to the int variable          
	int int_symbols[MAX_FIELD_SYMBOLS]; // Any symbols which are integers    
	int symbol_types[MAX_FIELD_SYMBOLS];// what kind of symbol is it         
	double double_symbols[MAX_FIELD_SYMBOLS];// Any symbols which are double   
	char string_symbols[MAX_FIELD_SYMBOLS][IPARSE_MAX_STRING_LENGTH];// Strings       
	int num_symbols;                    // How many symbols there are        
	int field_start_offset;             // Where in the block is this field  
	struct_list_element *the_struct;    // Definition of the structure       
	int done = BOGUS;                   // Have we found the field           
	field_def *the_field;               // Ptr to field - is it the one?     
	char *name;							// Needed to avoid memory problem
	int source_length;					// How long is the raw string?
	char *good_name;					// just the good chars
	/////////////////////////////////////////////////////////////////////////////

	// How long is the source string?
	source_length = strlen(source_name);

	// Allocate memory for a string to be used by a lower level
	// Name is freed in another routine
	name = (char *) MALLOC(source_length + 1);

	// Copy the passed in name into one that can be made gone
	strcpy(name, source_name);

	good_name = Save_Only_Alpha_Numeric(name, IParser_Is_OK_Char_Not_Float,
		source_length);

    int good_name_len = strlen(good_name); 
	Parse_New_Command(good_name, good_name_len, &num_symbols,
						int_symbols, 
						double_symbols, 
						string_symbols,
						symbol_types);

	// Find the variable 
	the_variable_ptr = Which_Variable( string_symbols[0]);

	if (the_variable_ptr is NULL) 
	{
		FREE (name);
		return(NULL);                       // Couldn't find it                  
	}

	// Retrieve structure definition                                           
	the_struct = the_variable_ptr->the_struct_el;

	// Second symbol contains index in the array.  Skip up to start of nth item
	field_start_offset = (the_struct->struct_length)*int_symbols[2];

	the_field = the_struct->the_definition; // First field in struct            

	while (!done) 
	{
		if (the_field is NULL) 
		{
			FREE (name);
			return(NULL);
		}

		done = (!strcmp(the_field->field_name,string_symbols[5]));

		if (!done) 
		{
			field_start_offset += the_field->field_length;
			the_field = the_field->next_field;
		}

	}

	FREE(good_name);
	FREE (name);
	return((&(the_variable_ptr->string_value[field_start_offset])));
}
//
// iparser::Get_Runtime_Struct_Field
//
///////////////////////////////////////////////////////////////////////////////



/*****************************************************************************
 * PUBLIC ROUTINES
 *****************************************************************************/



///////////////////////////////////////////////////////////////////////////////
//
// iparser::iparser
//
// Allocate a new IParse domain.
//
// Returns: Ptr to a domain_s.
//
// PUBLIC
//
iparser::iparser(AiMain *init_ai)
{
  // Local Variables //////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  number_of_variables = 0;
  variables = NULL;
  number_of_structures = 0;
  the_structs = NULL;

  // Remember the main AI structure
  ai = init_ai;

  // Initialize the include path for #include
  text_path.Init();

}
//
// iparser::iparser
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Parse_File
//
// Given a file name, and a parse domain, parse the file into the parse domain
//
// Returns: Error code (NO_FILE_ERROR, etc.)
// Modifies: Fills in parse domain
// 
// PUBLIC
//
int iparser::Parse_File
(
  char *file_name                       // Name of file to parse
)

{
  // Local Variables //////////////////////////////////////////////////////////
  FILE *parse_file = NULL;              // Ptr to file to add to domain      
  int error;                            // Error code for parsing the file   
  char errmsg[1000];
  char *parse_buffer=NULL;
  int parse_pos = 0;
  int buffer_length;
  int directory_number = 0;             // Which directory are we checking?
  int found = BOGUS;                    // Have we found the file yet?
  char *directory_name;                 // A directory to search for the file
  char full_file_name[500];             // File name with path
  /////////////////////////////////////////////////////////////////////////////

  //set up for Report_Error
  strcpy(m_current_file_name, file_name); 

  // Go through the path and find what directory the file is in
  while ((not found) and 
    (directory_number < text_path.count))
  {
    // Get the next directory path
    directory_name = (char *) 
      text_path.Return_Data_By_Number(directory_number);

    // Find the full file name
    sprintf(full_file_name, "%s%s", directory_name, file_name);

    // See if we can open the file
    found = ((parse_file = fopen(full_file_name, "r")) != NULL);
    
    
    // Go to the next directory
    directory_number++;

  }


  // If it wasn't found in one of the path directories, try it without 
  // changing the path.
  if (not found)
  {
    // See if it's in the basic directory. (Don't add anything to the path)
    if ((parse_file = fopen(file_name, "r")) == NULL)
    {
      errmsg[0] = 0;
      strcat(errmsg,"Error Opening parse file: '");
      strcat(errmsg, file_name);
      strcat(errmsg,"'");
      Report_Error(errmsg, REPORT_ERROR);
      return(NO_FILE_ERROR);
    }
  }

  Read_File_Into_Buffer(parse_file, &parse_buffer, &buffer_length);

  error = Do_Parse(parse_buffer, &parse_pos, buffer_length);

  fclose(parse_file);

  FREE(parse_buffer);

  return(error);
}
//
// iparser::Parse_File
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::~iparser
//
// We're done.  Let go of all the memory
//
// Modifies: Frees the parse domain and all variables and structs. PTR no longer
//           valid.
//
// PUBLIC
//
iparser::~iparser()

{
  // Local Variables //////////////////////////////////////////////////////////
  int i;
  char *an_include_path;                // String containing and include directory
  /////////////////////////////////////////////////////////////////////////////

  // Lose all the structure definitions
  Trash_Structs(the_structs);

  // Lose all the variables
  Trash_Variables(variables);

  // Trash the list of include directories
  for (i = text_path.count-1; i >= 0; i--)
  {
    // Grab the last include directory
    an_include_path = (char *) 
      text_path.Return_Data_By_Number(i);

    // Release the memory
    FREE(an_include_path);

  }

  // Free the internal structure of the list
  text_path.Empty();

}
//
// iparser::~iparser
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Get_Runtime_Char
//
// Return the value of a runtime char variable from an existing domain
//
// Returns: The char's value
// Modifies: Error code in error.
//
// PUBLIC
//
signed char iparser::Get_Runtime_Char
(
  char *name,                           // Name of the variable
  int *error                            // Error code (HEINOUS, IPARSE_NO_ERR)
) 

{
  // Local Variables //////////////////////////////////////////////////////////
  variable_struct *char_variable_ptr;   // ptr to the char variable            
  char error_string[256];				// Error if not found
  /////////////////////////////////////////////////////////////////////////////

  char_variable_ptr = Which_Variable( name);

  if (char_variable_ptr is NULL) 
  {
    (*error) = VARIABLE_NOT_FOUND;
	sprintf(error_string, "Config or AIP char parameter '%s' not found.", name);
	Report_Error(error_string, REPORT_ERROR);
    return(HEINOUS);                    // Couldn't find it                  
  }

  (*error) = IPARSE_NO_ERR;

  return((char) char_variable_ptr->int_value); // Send back the number
}
//
// iparser::Get_Runtime_Char
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Get_Runtime_Int
//
// Return the value of a runtime int variable from an existing domain
//
// Returns: The int's value
// Modifies: Error code in error.
//
// PUBLIC
//
int iparser::Get_Runtime_Int
(
  char *name,                           // Name of the variable
  int *error                            // Error code (HEINOUS, IPARSE_NO_ERR)
)

{
  // Local Variables //////////////////////////////////////////////////////////
  variable_struct *int_variable_ptr;    // ptr to the int variable              
  char error_string[256];				// Error if not found
  int report_error;						// Should we Report_Error the error
  /////////////////////////////////////////////////////////////////////////////

  // Should we Report_Error any errors?
	report_error = ((*error) != IPARSE_DONT_REPORT_ERROR);


  int_variable_ptr = Which_Variable( name);

  if (int_variable_ptr is NULL) 
  {
    (*error) = VARIABLE_NOT_FOUND;
 	sprintf(error_string, "Config or AIP int parameter '%s' not found.", name);
	if (report_error) Report_Error(error_string, REPORT_ERROR);
   return(HEINOUS);                    // Couldn't find it                  
  }

  if (int_variable_ptr->variable_type isnt INT_VARIABLE) 
  {
    (*error) = WRONG_TYPE;
  	sprintf(error_string, "Config or AIP int parameter '%s' found but wrong type.", name);
	if (report_error) Report_Error(error_string, REPORT_WARNING);
   return(HEINOUS);                    // Couldn't find it                  
  }
  (*error) = IPARSE_NO_ERR;

  return(int_variable_ptr->int_value);  // Send back the number              
}
//
// iparser::Get_Runtime_Int
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Get_Runtime_Float
//
// Return the value of a runtime float variable from an existing domain
//
// Returns: The float's value
// Modifies: Error code in error.
//
// PUBLIC
//
double iparser::Get_Runtime_Float
(
  char *name,                           // Name of the variable
  int *error                            // Error code (HEINOUS, IPARSE_NO_ERR)
)

{
	// Local Variables //////////////////////////////////////////////////////////
	variable_struct *variable_ptr;        // ptr to the int variable           
	char error_string[256];				// Error if not found
	int report_error;
	/////////////////////////////////////////////////////////////////////////////

	variable_ptr = Which_Variable( name);

	if (variable_ptr is NULL) 
	{
		(*error) = VARIABLE_NOT_FOUND;
		sprintf(error_string, "Config or AIP float parameter '%s' not found.", name);
		Report_Error(error_string, REPORT_ERROR);
		return(HEINOUS);                    // Couldn't find it                  
	}

	// Should we Report_Error any errors?
	report_error = ((*error) != IPARSE_DONT_REPORT_ERROR);

	if ((variable_ptr->variable_type isnt DOUBLE_VARIABLE) &&
		(variable_ptr->variable_type isnt FLOAT_VARIABLE))
	{
		(*error) = WRONG_TYPE;
		sprintf(error_string, "Config or AIP float parameter '%s' found but wrong type.", name);
		if (report_error) Report_Error(error_string, REPORT_WARNING);
		return(HEINOUS);                    // Couldn't find it                  
	}
	(*error) = IPARSE_NO_ERR;

	return(((float)variable_ptr->double_value)); // Send back the number      
}
//
// iparser::Get_Runtime_Float
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Get_Runtime_Double
//
// Return the value of a runtime double variable from an existing domain
//
// Returns: The double's value
// Modifies: Error code in error.
//
// PUBLIC
//
double iparser::Get_Runtime_Double
(
	char *name,                         // Name of the variable
	int *error                          // Error code (HEINOUS, IPARSE_NO_ERR)
)

{
	// Local Variables //////////////////////////////////////////////////////////
	variable_struct *variable_ptr;      // ptr to the int variable              
	char error_string[256];				// Error if not found
	int report_error;					// Should we Report_Error the error
	/////////////////////////////////////////////////////////////////////////////

	// Should we Report_Error any errors?
	report_error = ((*error) != IPARSE_DONT_REPORT_ERROR);

	variable_ptr = Which_Variable( name);

	if (variable_ptr is NULL) 
	{
		(*error) = VARIABLE_NOT_FOUND;
		sprintf(error_string, "Config or AIP double parameter '%s' not found.", name);
		if (report_error) Report_Error(error_string, REPORT_ERROR);
		return(HEINOUS);                    // Couldn't find it                  
	}

	if (variable_ptr->variable_type isnt DOUBLE_VARIABLE) 
	{
		(*error) = WRONG_TYPE;
		sprintf(error_string, "Config or AIP string parameter '%s' found but wrong type.", name);
		if (report_error) Report_Error(error_string, REPORT_WARNING);
		return(variable_ptr->double_value);                    // Couldn't find it                  
	}

	(*error) = IPARSE_NO_ERR;

	return(variable_ptr->double_value);   // Send back the number              
}
//
// iparser::Get_Runtime_Double
//
///////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
//
// iparser::Get_Runtime_String
//
// Return the value of a runtime string variable from an existing domain.
// NOTE: The user must provide an allocated char buffer to hold the string!!!
//
// Returns: Ptr to the string
// Modifies: Error code in error.
//
// PUBLIC
//
char *iparser::Get_Runtime_String
(
  char *name,                           // Name of the variable
  int *error,                           // Error code (HEINOUS, IPARSE_NO_ERR)
  char *new_ptr                         // PTR to existing buffer for string
)

{
  // Local Variables //////////////////////////////////////////////////////////
  variable_struct *variable_ptr;        // ptr to the int variable              
  char error_string[256];				// Error if not found
  /////////////////////////////////////////////////////////////////////////////

  variable_ptr = Which_Variable( name);

  if (variable_ptr is NULL) 
  {
    (*error) = VARIABLE_NOT_FOUND;
	sprintf(error_string, "Config or AIP string parameter '%s' not found.", name);
	Report_Error(error_string, REPORT_ERROR);
    return(NULL);                       // Couldn't find it                  
  }

  (*error) = IPARSE_NO_ERR;

  strcpy(new_ptr, variable_ptr->string_value);

  return(new_ptr);                      // Send back the number              
}
//
// iparser::Get_Runtime_String
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Allocate_And_Get_Runtime_String
//
// Return the value of a runtime string variable from an existing domain.
// NOTE: This function allocates (but does not de-allocate) the new string.
//
// Returns: Ptr to the string
// Modifies: Error code in error.
//
// PUBLIC
//
char *iparser::Allocate_And_Get_Runtime_String
(
  char *name,                           // Name of the variable
  int *error                            // Error code (HEINOUS, IPARSE_NO_ERR)
)

{
  // Local Variables //////////////////////////////////////////////////////////
  variable_struct *variable_ptr;        // ptr to the int variable              
  char *new_ptr;                        // PTR to new buffer for string
  char error_string[256];				// Error if not found
  /////////////////////////////////////////////////////////////////////////////

  variable_ptr = Which_Variable( name);

  if (variable_ptr is NULL) 
  {
    (*error) = VARIABLE_NOT_FOUND;
 	sprintf(error_string, "Config or AIP string parameter '%s' not found.", name);
	Report_Error(error_string, REPORT_ERROR);
   return(NULL);                       // Couldn't find it                  
  }

  (*error) = IPARSE_NO_ERR;

  // Allocate enough new memory to fit the string value
  new_ptr = (char *) MALLOC(strlen(variable_ptr->string_value)+1);

  strcpy(new_ptr, variable_ptr->string_value);

  return(new_ptr);                      // Send back the number              
}
//
// iparser::Get_Runtime_String
//
///////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
//
// iparser::Get_Runtime_Char_Field
//
// Grab a particular character from one element of a struct array
//
// Returns: The Character field sought
// Modifies: Error code in error.
//
// PUBLIC
//
signed char iparser::Get_Runtime_Char_Field
(
  char *name,                           // Name of the variable
  int *error                            // Error code (HEINOUS, IPARSE_NO_ERR)
) 
           
{
  // Local Variables //////////////////////////////////////////////////////////
  char *the_field_ptr;                  // Ptr to the field                 
  char error_string[256];				// Error if not found
  /////////////////////////////////////////////////////////////////////////////

  the_field_ptr = Get_Runtime_Struct_Field( name);

  if (the_field_ptr is NULL) 
  {
    (*error)  = VARIABLE_NOT_FOUND;
	sprintf(error_string, "Config or AIP char parameter '%s' not found.", name);
 	Report_Error(error_string, REPORT_ERROR);
   return(HEINOUS);
  }

  (*error) = IPARSE_NO_ERR;
  return(the_field_ptr[0]);

}
//
// iparser::Get_Runtime_Char_Field
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Get_Runtime_Int_Field
//
// Grab a particular int from one element of a struct array
//
// Returns: The int field sought
// Modifies: Error code in error.
//
// PUBLIC
//
int iparser::Get_Runtime_Int_Field
(
  char *name,                           // Name of the variable
  int *error                            // Error code (HEINOUS, IPARSE_NO_ERR)
) 
           
{
  // Local Variables //////////////////////////////////////////////////////////
  char *the_field_ptr;                  // Ptr to the field                 
  int result;
  char error_string[256];				// Error if not found
  /////////////////////////////////////////////////////////////////////////////

  the_field_ptr = Get_Runtime_Struct_Field( name);

  if (the_field_ptr is NULL) 
  {
    (*error)  = VARIABLE_NOT_FOUND;
	sprintf(error_string, "Config or AIP int parameter '%s' not found.", name);
	Report_Error(error_string, REPORT_ERROR);
    return(HEINOUS);
  }

  memcpy((char *) (&result), 
   the_field_ptr,
   sizeof(int));

  (*error) = IPARSE_NO_ERR;

  return(result);                       // Send back the number              
}
//
// iparser::Get_Runtime_Int_Field
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Get_Runtime_Float_Field
//
// Grab a particular float from one element of a struct array
//
// Returns: The float field sought
// Modifies: Error code in error.
//
// PUBLIC
//
double iparser::Get_Runtime_Float_Field
(
  char *name,                           // Name of the variable
  int *error                            // Error code (HEINOUS, IPARSE_NO_ERR)
) 
           
{
  // Local Variables //////////////////////////////////////////////////////////
  char *the_field_ptr;                  // Ptr to the field                 
  float result;
  char error_string[256];				// Error if not found
  /////////////////////////////////////////////////////////////////////////////

  the_field_ptr = Get_Runtime_Struct_Field(name);

  if (the_field_ptr is NULL) 
  {
    (*error)  = VARIABLE_NOT_FOUND;
 	sprintf(error_string, "Config or AIP float parameter '%s' not found.", name);
	Report_Error(error_string, REPORT_ERROR);
   return(HEINOUS);
  }

  memcpy((char *) (&result), 
   the_field_ptr,
   sizeof(float));

  (*error) = IPARSE_NO_ERR;

  return(result);                       // Send back the number              
}
//
// iparser::Get_Runtime_Float_Field
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Get_Runtime_Double_Field
//
// Grab a particular double from one element of a struct array
//
// Returns: The double field sought
// Modifies: Error code in error.
//
// PUBLIC
//
double iparser::Get_Runtime_Double_Field
(
  char *name,                           // Name of the variable
  int *error                            // Error code (HEINOUS, IPARSE_NO_ERR)
) 
           
{
  // Local Variables //////////////////////////////////////////////////////////
  char *the_field_ptr;                  // Ptr to the field                 
  double result;
  char error_string[256];				// Error if not found
  /////////////////////////////////////////////////////////////////////////////

  the_field_ptr = Get_Runtime_Struct_Field( name);

  if (the_field_ptr is NULL) 
  {
    (*error)  = VARIABLE_NOT_FOUND;
	sprintf(error_string, "Config or AIP double parameter '%s' not found.", name);
	Report_Error(error_string, REPORT_ERROR);
    return(HEINOUS);
  }

  memcpy((char *) (&result), 
   the_field_ptr,
   sizeof(double));

  (*error) = IPARSE_NO_ERR;

  return(result);                       // Send back the number              
}
//
// iparser::Get_Runtime_Double_Field
//
///////////////////////////////////////////////////////////////////////////////

   

///////////////////////////////////////////////////////////////////////////////
//
// iparser::Get_Runtime_String_Field
//
// Grab a particular string from one element of a struct array
//
// Returns: Ptr to the string buffer (same as passed in)
// Modifies: Error code in error. Fills in pre-allocated buffer
//
// PUBLIC
//
char *iparser::Get_Runtime_String_Field
(
  char *name,                           // Name of the variable
  int *error,                           // Error code (HEINOUS, IPARSE_NO_ERR)
  char *buf                             // PTR to existing buffer for string
)
      
{
  // Local Variables //////////////////////////////////////////////////////////
  char *the_field_ptr;                  // Ptr to the field                
  char error_string[256];				// Error if not found
  /////////////////////////////////////////////////////////////////////////////

  the_field_ptr = Get_Runtime_Struct_Field( name);

  if (the_field_ptr is NULL) 
  {
    (*error)  = VARIABLE_NOT_FOUND;
	sprintf(error_string, "Config or AIP string parameter '%s' not found.", name);
	Report_Error(error_string, REPORT_ERROR);
    return(NULL);
  }

  strncpy(buf, the_field_ptr, strlen(the_field_ptr)+1);

  (*error) = IPARSE_NO_ERR;

  return(buf);                          // Send back the number              
}
//
// iparser::Get_Runtime_String_Field
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Allocate_And_Get_Runtime_String_Field
//
// Grab a particular string from one element of a struct array.  Allocate and
// fill in a buffer.
//
// Returns: Ptr to the string buffer (same as passed in)
// Modifies: Error code in error. Allocates and fills in buffer
//
// PUBLIC
//
char *iparser::Allocate_And_Get_Runtime_String_Field
(
  char *name,                           // Name of the variable
  int *error                            // Error code (HEINOUS, IPARSE_NO_ERR)
)
      
{
  // Local Variables //////////////////////////////////////////////////////////
  char *the_field_ptr;                  // Ptr to the field                
  char *new_ptr;                        // Newly allocated string
  /////////////////////////////////////////////////////////////////////////////


  the_field_ptr = Get_Runtime_Struct_Field( name);

  if (the_field_ptr is NULL) 
  {
    (*error)  = VARIABLE_NOT_FOUND;
    return(NULL);
  }

  // Allocate enough new memory to fit the string value
  new_ptr = (char *) MALLOC(strlen(the_field_ptr)+1);

  if (new_ptr is NULL) 
  {
    (*error)  = OTHER_ERROR;
    return(NULL);
  }

  (*error) = IPARSE_NO_ERR;

  strcpy(new_ptr, the_field_ptr);
//  strncpy(buf, the_field_ptr, strlen(the_field_ptr)+1);

  return(new_ptr);                      // Send back the number              

}
//
// iparser::Allocate_And_Get_Runtime_String_Field
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Get_Struct_Array_Element_Count
//
// Find out how many elements were actually in a struct array
//
// Returns: # of elements in the are
//
// PUBLIC
//
int iparser::Get_Struct_Array_Element_Count
(
  char *name,                           // Name of the variable
  int *error                            // Error code (HEINOUS, IPARSE_NO_ERR)
) 
           
{
  // Local Variables //////////////////////////////////////////////////////////
  variable_struct *the_variable_ptr;    // ptr to the int variable          
  /////////////////////////////////////////////////////////////////////////////

  // Find the variable 
  the_variable_ptr = Which_Variable( name);

  if (the_variable_ptr is NULL) 
  {
    (*error)  = VARIABLE_NOT_FOUND;
    return(HEINOUS);
  }

  (*error) = IPARSE_NO_ERR;
  return(the_variable_ptr->struct_element_count);

}
//
// iparser::Get_Struct_Array_Element_Count
//
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// iparser::Append_Path
//
// Add a new directory at the end of the list of directories to search for include
// files.
//
// NOTE: The current directory ".\" is always put at the end of the list 
// automatically, but it can be added manually earlier in the list"






void iparser::Append_Path
(
  char *new_path                        
) 
           
{
  
  char *real_path;                      
  int path_length;                      
  int add_slash;                        
  


  
  path_length = strlen(new_path);

  
  if (new_path[path_length-1] isnt DIRECTORY_CHARACTER)
  {
    
    path_length++;

    
    add_slash = TRUE;
  }

  
  real_path = (char *) MALLOC(path_length+1);

  
  strcpy(real_path, new_path);

  
  if (add_slash)
  {
    real_path[path_length-1] = DIRECTORY_CHARACTER;
    real_path[path_length] = 0;         
  }

  
  text_path.Append_Data(real_path);

}




