/*
Copyright (C) 1995-2001 Activision, Inc.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

/*
 * PROG
 *
 * Simple Program State Machine and parsing utility for script based test programs.
 *
 * Matthew Versluys
 *
 * Lines begining with # are comments
 *
 * There a several build in commands to aid with flow control
 *
 * Command	Usage					Descrition
 * LABEL	LABEL label 			Labels are the destination of GOTO commands
 * GOTO		GOTO label 				When executed the program changes the current line to this label
 * ONERROR	ONERROR label 			When an error occurs the program changes the current line to this label
 * SET		SET variable value		Sets the variable to the value specified
 * ADD		ADD variable1 variable2	Adds the value to the current contents of the variable
 * IF		IF variable1 !=<> variable2 label	Tests the two variables and if the equation is true then
 *												the program	changes the current line to the label specified
 * EXIT		EXIT code				Exit the program using the return code specified
 *
 * There are some predefined paramaters which all registered functions must accept
 *
 * !PARAM - print out the argument specification
 * !INFO  - print out information about what this command does
 *
 * When printing out paramater information the following format should be used
 *
 * functionname required required [optional] [optional]
 *
 */




/*
 * Definitions
 */

#define PROG_MAX_FUNCTION			64
#define PROG_MAX_PARAM_LENGTH		256
#define PROG_MAX_PARAM				32
#define PROG_MAX_LINE_LENGTH		1024
#define PROG_MAX_INT				10
#define PROG_MAX_STRING				4
#define PROG_MAX_STRINGLEN			80
#define PROG_SEPARATOR				' '
#define PROG_COMMENT				'#'
#define PROG_PARAM					'%'

#define PROG_LINE_FLAGS_LABEL		0x01	/* Indicates line is a label only */
#define PROG_LINE_FLAGS_RESOLV		0x02	/* Indicates this line requires resolution */
#define PROG_LINE_FLAGS_IFRESOLV	0x04	/* Indicates this line is a prog_integer_if_t which requires resolution */

#define PROG_CMD_PARAM				"!PARAM"
#define PROG_CMD_TEST_PARAM(x)		(!strcmp(x, PROG_CMD_PARAM))

#define PROG_CMD_INFO				"!INFO"
#define PROG_CMD_TEST_INFO(x)		(!strcmp(x, PROG_CMD_INFO))

typedef enum
{
	prog_RES_OK		= 0,
	prog_RES_ERROR	= 1,
	prog_RES_MEM	= 2,
	prog_RES_SYNTAX = 3,
	prog_RES_FILE	= 4,
	prog_RES_END    = 5,
	prog_RES_EXIT	= 6
} prog_res_t;

typedef enum
{
	prog_cmd_RES_OK,
	prog_cmd_RES_CONTINUE,
	prog_cmd_RES_WAIT,
	prog_cmd_RES_ERROR,
	prog_cmd_RES_EXIT
} prog_cmd_res_t;

struct prog_process_s;

typedef prog_cmd_res_t (prog_cmd_t)(struct prog_process_s *process, const char *params, void *context);

/*
 * Structures
 */

/*
 * prog_fn_t
 *
 * A single function which is part of a syntax
 *
 */
typedef struct prog_fn_s
{
	prog_cmd_t			*cmd;						/* function */
	char				name[PROG_MAX_FUNCTION];	/* text name of the function */
	struct prog_fn_s	*next;						/* pointer to next funtion */
} prog_fn_t;

/*
 * prog_syntax_t
 *
 * A syntax is defined as a list of functions
 *
 */
typedef struct prog_syntax_s
{
	prog_fn_t			*functions;				/* list of functions for this syntax */
} prog_syntax_t;

/*
 * prog_line_t
 *
 * A line of a program has a function and a list of paramaters
 *
 */
typedef struct prog_line_s
{
	unsigned long		flags;					/* flags for this line */
	prog_cmd_t			*cmd;					/* function to be called when this is executed */
	char				params[PROG_MAX_PARAM_LENGTH];	/* paramaters sent to the function */
	struct prog_line_s	*next;					/* pointer to next line in the program */
} prog_line_t;

/*
 * prog_t
 *
 * A program consits of a list of program lines
 *
 */
typedef struct prog_s
{
	prog_line_t		*lines;				/* lines of the program, pointer is to first line */
} prog_t;

/*
 * prog_process_t
 *
 * A process consists of a program and the current location within that program
 *
 */
typedef struct prog_process_s
{
	int						integers[PROG_MAX_INT];		/* Integer Variables */
	char					strings[PROG_MAX_STRING][PROG_MAX_STRINGLEN];
														/* String Variables */
	int						exitcode;					/* Exit Code */
	prog_t					*program;					/* Program being executed */
	prog_line_t				*line;						/* Current line of the program */
	prog_line_t				*errhandler;				/* Current error handler */
	struct prog_process_s	*next;						/* Pointer to next process */
} prog_process_t;


/*
 * Prototypes
 */

/*
 * prog_syntax_Create
 *
 * Creates a program syntax
 *
 * in  -> syntax: syntax to be created
 * out <- prog_RES_OK: created successfully
 *        prog_RES_ERROR: error creating syntax
 */
prog_res_t prog_syntax_Create(prog_syntax_t *syntax);

/*
 * prog_syntax_Destroy
 *
 * Destroys a program syntax
 *
 * in  -> syntax: syntax to be destroyed
 * out <- prog_RES_OK: syntax destroyed
 *        prog_RES_ERROR: syntax could not be destroyed
 */
prog_res_t prog_syntax_Destroy(prog_syntax_t *syntax);

/*
 * prog_syntax_AddCmd
 *
 * Add a command to the program syntax
 *
 * in  -> syntax: syntax to which the function is being added
 *        cmd: text which will be recognized
 *        fn: function which will be called when this command is executed
 * out <- prog_RES_OK: command was successfully added
 *        prog_RES_ERROR: command could not be added
 *        prog_RES_MEM: ran out of memory whilst adding the command
 */
prog_res_t prog_syntax_AddCmd(prog_syntax_t *syntax, const char *name, prog_cmd_t *cmd);

/*
 * prog_syntax_PrintCmds
 *
 * Displays the commands in the current syntax
 *
 * in  -> syntax: syntax to pring commands out for
 *        verbose: verbosity level
 *         0 - command names only
 *         1 - command names and paramaters
 *         2 - command names, paramaters and information
 * out <- prog_RES_OK
 *
 */
prog_res_t prog_syntax_PrintCmds(prog_syntax_t *syntax, int verbose);

/*
 * prog_Load
 *
 * Loads a script based text file into memory ready for execution
 *
 * in  -> prog: program will be loaded into this structure
 *        syntax: syntax to load this program with
 *        file: filename of program to be loaded
 *        argc: number of arguments
 *        argv: argument list
 * out <- prog_RES_OK: program was loaded successfully
 *        prog_RES_ERROR: program could not be loaded
 *        prog_RES_MEM: ran out of memory whilst loading the program
 *        prog_RES_SYNTAX: there was a syntactic error in the program
 *        prog_RES_FILE: file error
 */
prog_res_t prog_Load(prog_t *prog, prog_syntax_t *syntax, const char *file, int argc, char **argv);

/*
 * prog_UnLoad
 *
 * Unloads a script based text file from memory
 *
 * in  -> prog: program to be unloaded
 * out <- prog_RES_OK: program was unloaded successfully
 *        prog_RES_ERROR: program could not be unloaded
 */
prog_res_t prog_UnLoad(prog_t *proc);

/*
 * prog_process_Init
 *
 * Initializes a progress to be executed on a program
 *
 * in  -> process: process structure
 *        prog: program to be used
 *        line: line to start execution from (if NULL then use program start)
 * out <- prog_RES_OK: process initialized
 *        prog_RES_ERROR: could not initialize process
 *
 */
prog_res_t prog_process_Init(prog_process_t *process, prog_t *prog, prog_line_t *line);

/*
 * prog_process_Step
 *
 * Takes a single step through a program on a particular process
 *
 * in  -> process: process to step through
 *        context: context to be used during execution
 * out <- prog_RES_OK: program executed successfully
 *        prog_RES_ERROR: error occured during execution
 *        prog_RES_END: program has ended
 */
prog_res_t prog_process_Step(prog_process_t *process, void *context);

/*
 * prog_process_GetInteger
 *
 * Retreive the value of a processes integer
 *
 * in  -> process: process to get value from
 *        index: index of integer value
 * out <- value
 *
 */
int prog_process_GetInteger(prog_process_t *process, int index);

/*
 * prog_process_SetInteger
 *
 * Retreive the value of a processes integer
 *
 * in  -> process: process to get value from
 *        index: index of integer value
 *        value: value to set
 */
void prog_process_SetInteger(prog_process_t *process, int index, int value);

/*
 * prog_process_GetString
 *
 * Retreive the value of a processes string
 *
 * in  -> process: process to get value from
 *        index: index of string value
 * out <- value
 *
 */
char *prog_process_GetString(prog_process_t *process, int index);

/*
 * prog_process_SetString
 *
 * Retreive the value of a processes string
 *
 * in  -> process: process to get value from
 *        index: index of string value
 *        value: value to set
 */
void prog_process_SetString(prog_process_t *process, int index, const char *value);

/*
 * prog_process_GetExitCode
 *
 * Returns the exit code of a process
 *
 * in  -> process: process to get value from
 * out <- exit code of process or -1 if process has not exited
 *
 */
int prog_process_GetExitCode(prog_process_t *process);

/*
 * prog_Resolv
 *
 * Resolves a string label into a line pointer
 *
 * in  -> label: name of label to resolv
 *        prog: program label is within
 * out <- pointer to resolved line or NULL if not found
 *
 */
prog_line_t *prog_Resolv(const char *label, prog_t *prog);

/*
 * prog_Char2Args
 *
 * Converts a character string into arguments
 *
 * in  -> string: string to be converted
 *        argc: pointer to variable which will hold the number of arguments
 *        argv: pointer to char * array to hold the argument pointers
 */
void prog_Char2Args(char *string, int *argc, char **argv);
