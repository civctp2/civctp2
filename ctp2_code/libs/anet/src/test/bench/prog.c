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
 * When programs are loaded, %1 %2 %3 etc are translated into arguments
 * Note that if parameters contain the % characters, BAD stuff happens
 * 
 * There a several build in commands to aid with flow control
 *
 * Command	Usage					Descrition
 * LABEL	LABEL label 			Labels are the destination of GOTO commands
 * GOTO		GOTO label 				When executed the program changes the current line to this label
 * ONERROR	ONERROR label 			When an error occurs the program changes the current line to this label
 * SET		SET variable value		Sets the variable to the value specified
 * ADD		ADD variable1 variable2	Adds the value to the current contents of the variable
 * DIV		DIV variable1 variable2	Divides var1 by var2.
 * IF		IF variable1 !=<> variable2 label	Tests the two variables and if the equation is true then 
 *												the program	changes the current line to the label specified
 * EXIT		EXIT code				Exit the program using the return code specified
 */


/*
 * Includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "prog.h"

#define ASSERT

#define TRUE 1
#define FALSE 0

/*
 * Prototypes
 */
static prog_res_t progAddLine(prog_t *prog, prog_cmd_t *cmd, const char *params, unsigned long flags);

/*
 * Define the built in commands.  Note that the cmd_t's are function pointers.
 */
typedef prog_res_t (prog_internal_cmd_t)(prog_t *prog, char *params);

static prog_internal_cmd_t	progInternalFunctionLabel;
static prog_internal_cmd_t	progInternalFunctionGoto;
static prog_internal_cmd_t	progInternalFunctionOnError;
static prog_internal_cmd_t	progInternalFunctionSet;
static prog_internal_cmd_t	progInternalFunctionAdd;
static prog_internal_cmd_t	progInternalFunctionDiv;
static prog_internal_cmd_t	progInternalFunctionIf;
static prog_internal_cmd_t	progInternalFunctionExit;

static prog_cmd_t			progFunctionGoto;
static prog_cmd_t			progFunctionOnError;
static prog_cmd_t			progFunctionSet;
static prog_cmd_t			progFunctionSetString;
static prog_cmd_t			progFunctionAdd;
static prog_cmd_t			progFunctionDiv;
static prog_cmd_t			progFunctionIf;
static prog_cmd_t			progFunctionExit;

typedef struct prog_internal_fn_s
{
	char						name[PROG_MAX_FUNCTION];	/* text name of the function */
	prog_internal_cmd_t			*fn;						/* function to be executed */
	struct prog_internal_fn_s	*next;						/* pointer to next funtion */
} prog_internal_fn_t;

static prog_internal_fn_t	progInternalLabel	= { "LABEL", progInternalFunctionLabel, NULL };
static prog_internal_fn_t	progInternalGoto	= { "GOTO", progInternalFunctionGoto, &progInternalLabel };
static prog_internal_fn_t	progInternalOnError = { "ONERROR", progInternalFunctionOnError, &progInternalGoto };
static prog_internal_fn_t	progInternalSet     = { "SET", progInternalFunctionSet, &progInternalOnError };
static prog_internal_fn_t	progInternalAdd		= { "ADD", progInternalFunctionAdd, &progInternalSet };
static prog_internal_fn_t	progInternalDiv		= { "DIV", progInternalFunctionDiv, &progInternalAdd };
static prog_internal_fn_t	progInternalIf		= { "IF", progInternalFunctionIf, &progInternalDiv };
static prog_internal_fn_t	progInternalExit	= { "EXIT", progInternalFunctionExit, &progInternalIf };
static prog_internal_fn_t	*progInternalFunctions = &progInternalExit;

typedef struct prog_integer_set_s
{
	int	var;
	int	value;
} prog_integer_set_t;

typedef struct prog_string_set_s
{
	int	var;
	char value[PROG_MAX_STRINGLEN];
} prog_string_set_t;

typedef struct prog_integer_add_s
{
	int	var1;
	int	var2;
} prog_integer_add_t;

typedef enum 
{
	prog_integer_if_type_eq,
	prog_integer_if_type_ne,
	prog_integer_if_type_gt,
	prog_integer_if_type_lt
} prog_integer_if_type;

typedef struct prog_integer_if_s
{
	int var1;
	int	var2;
	prog_integer_if_type	type;
	prog_line_t	*line;
	char labeltext[PROG_MAX_STRINGLEN];
} prog_integer_if_t;


/*
 * prog_syntax_Create
 *
 * Creates a program syntax
 *
 * in  -> syntax: syntax to be created
 * out <- prog_RES_OK: created successfully
 *        prog_RES_ERROR: error creating syntax
 */
prog_res_t prog_syntax_Create(prog_syntax_t *syntax)
{
	ASSERT(syntax);

	syntax->functions = NULL;

	return (prog_RES_OK);
}


/*
 * prog_syntax_Destroy
 *
 * Destroys a program syntax
 *
 * in  -> syntax: syntax to be destroyed
 * out <- prog_RES_OK: syntax destroyed
 *        prog_RES_ERROR: syntax could not be destroyed
 */
prog_res_t prog_syntax_Destroy(prog_syntax_t *syntax)
{
	prog_fn_t	*ptr;

	ASSERT(syntax);

	while (syntax->functions)
	{
		ptr = syntax->functions;
		syntax->functions = syntax->functions->next;

		free(ptr);
	}

	return (prog_RES_OK);
}


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
prog_res_t prog_syntax_AddCmd(prog_syntax_t *syntax, const char *name, prog_cmd_t *cmd)
{
	prog_fn_t	*function;

	ASSERT(syntax);
	ASSERT(name);
	ASSERT(cmd);

	/* Allocate new function */
	function = (prog_fn_t *) malloc(sizeof (prog_fn_t));

	if (!function)
	{
		return (prog_RES_MEM);
	}

	/* Initalize the function */
	function->cmd = cmd;
	strcpy(function->name, name);
	function->next = syntax->functions;

	/* Add to list of functions in syntax */
	syntax->functions = function;

	return (prog_RES_OK);
}


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
prog_res_t prog_syntax_PrintCmds(prog_syntax_t *syntax, int verbose)
{
	prog_fn_t	*fn;

	fn = syntax->functions;

	while (fn)
	{
		switch (verbose)
		{
			case 0:
				printf("%s\n", fn->name);
				break;

			case 1:
				fn->cmd(NULL, PROG_CMD_PARAM, NULL);
				break;

			case 2:
				fn->cmd(NULL, PROG_CMD_PARAM, NULL);
				fn->cmd(NULL, PROG_CMD_INFO, NULL);
				break;
		}
		fn = fn->next;
	}
	return (prog_RES_OK);
}


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
prog_res_t prog_Load(prog_t *prog, prog_syntax_t *syntax, const char *file, int argc, char **argv)
{
	FILE		*fp;
	int			line = 0;
	prog_line_t	*lptr;

	ASSERT(prog);
	ASSERT(syntax);
	ASSERT(file);

	/* Initialize the program structure */
	prog->lines = NULL;

	/* Open the file */
	fp = fopen(file, "rt");

	if (!fp)
	{
		return (prog_RES_FILE);
	}

	/* Parse each line of the program into the program */
	while (!feof(fp))
	{
		prog_res_t			res;
		char				buffer[PROG_MAX_LINE_LENGTH];
		char				*params;
		prog_internal_fn_t	*internalfn;
		prog_fn_t			*fn;
		int					found = FALSE;

		if (!fgets(buffer, PROG_MAX_LINE_LENGTH, fp) && !feof(fp))
		{
			printf("File Error : Line %d\n", line);
			return (prog_RES_FILE);
		}

		line++;

		/* Remove and LF or CR characters from the paramaters */
		params = strchr(buffer, '\n');
		if (params)
		{
			*params = '\0';
		}
		params = strchr(buffer, '\r');
		if (params)
		{
			*params = '\0';
		}

		/* If the first character is the comment character, ignore the line */
		if (buffer[0] == PROG_COMMENT)
		{
			continue;
		}

		/* If the line is blank, ignore the line */
		if (buffer[0] == '\0')
		{
			continue;
		}
		
		/* Translate any %x into the supplied argument */
		params = strchr(buffer, PROG_PARAM);
		
		while (params)
		{
			char buffer2[PROG_MAX_LINE_LENGTH];
			int	 arg;
			
			/* Terminate string at parameter */
			*params = '\0';
			params++;
			
			/* Copy the string before the parameter */
			strcpy(buffer2, buffer);
			
			/* Find the parameter and make sure its valid */
			arg = atoi(params);
			if (arg >= argc)
			{
				printf("Unknown parameter on Line %d\n", line);
				return (prog_RES_ERROR);
			}

			/* Replace with the supplied argument */
			strcat(buffer2, argv[arg]);
			
			/* Check to see if there is any text following the parameter */
			params = strchr(params, ' ');
			if (params)
			{
				strcat(buffer2, params);
			}
					
			/* Copy the new buffer into the original */
			memcpy(buffer, buffer2, PROG_MAX_LINE_LENGTH);			
			
			/* Perform the test again */
			params = strchr(buffer, PROG_PARAM);
		}
		

		/* The first word of each line is the command the rest is paramaters */
		params = strchr(buffer, PROG_SEPARATOR);

		if (params)
		{
			*params = '\0';
			params++;
		}

		/* First check to see if this is one of the built in commands */
		internalfn = progInternalFunctions;

		while (internalfn)
		{
			if (!strcmp(internalfn->name, buffer))
			{
				/* Found a match, internal command get executed immediately */
				res = internalfn->fn(prog, params);

				if (res != prog_RES_OK)
				{
					printf("Line Number %d\n", line);
					return (res);
				}

				found = TRUE;
				break;
			}
			internalfn = internalfn->next;
		}

		if (!found)
		{
			/* Attempt to find a match in the syntax */
			fn = syntax->functions;

			while (fn)
			{
				if (!strcmp(fn->name, buffer))
				{
					/* Found a match, "compile" this line */
					res = progAddLine(prog, fn->cmd, params, 0);

					if (res != prog_RES_OK)
					{
						return (res);
					}

					found = TRUE;
					break;
				}
				fn = fn->next;
			}
		}

		if (!found)
		{
			printf("Syntax Error : Line %d : %s\n", line, buffer);
			return (prog_RES_SYNTAX);
		}
	}

	/* close the file */
	fclose(fp);

	/* resolv all the lines which require resolution */
	lptr = prog->lines;

	while (lptr)
	{
		if (lptr->flags & PROG_LINE_FLAGS_RESOLV)
		{
			prog_line_t *line;

			line = prog_Resolv(lptr->params, prog);

			if (!line)
			{
				printf("Unable to resolve reference : %s\n", lptr->params);
				return (prog_RES_ERROR);
			}

			memcpy(lptr->params, &line, sizeof (&line));
		}
		else if (lptr->flags & PROG_LINE_FLAGS_IFRESOLV)
		{
			prog_integer_if_t	*i = (prog_integer_if_t *) lptr->params;
			prog_line_t *line;

			line = prog_Resolv(i->labeltext, prog);

			if (!line)
			{
				printf("Unable to resolve reference : %s\n", i->labeltext);
				return (prog_RES_ERROR);
			}

			i->line = line;

		}
		lptr = lptr->next;
	}

	return (prog_RES_OK);
}


/*
 * prog_UnLoad
 *
 * Unloads a script based text file from memory
 *
 * in  -> prog: program to be unloaded
 * out <- prog_RES_OK: program was unloaded successfully
 *        prog_RES_ERROR: program could not be unloaded
 */
prog_res_t prog_UnLoad(prog_t *prog)
{
	prog_line_t *line;

	while (prog->lines)
	{
		line = prog->lines;
		prog->lines = prog->lines->next;
		free(line);
	}

	return (prog_RES_OK);
}


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
prog_res_t prog_process_Init(prog_process_t *process, prog_t *prog, prog_line_t *line)
{
	ASSERT(process);
	ASSERT(prog);

	process->program = prog;
	process->exitcode = -1;
	process->line = line ? line : prog->lines;
	process->errhandler = NULL;
	process->next = NULL;
	return (prog_RES_OK);
}


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
 *
 */
prog_res_t prog_process_Step(prog_process_t *process, void *context)
{
	ASSERT(process);

	/* If the processes current line is null, return END */
	if (!process->line)
	{
		return (prog_RES_END);
	}

	/* Step through any and all NULL lines until we reach something interesting */
	while (!process->line->cmd)
	{
		process->line = process->line->next;

		if (!process->line)
		{
			return (prog_RES_END);
		}
	}

	/* Execute the line and test the result */
	switch (process->line->cmd(process, process->line->params, context))
	{
		case prog_cmd_RES_CONTINUE:
			/* Advance to the next line */
			process->line = process->line->next;
			break;

		case prog_cmd_RES_WAIT:
			break;
		
		case prog_cmd_RES_EXIT:
			return (prog_RES_EXIT);
			break;

		case prog_cmd_RES_ERROR:
			if (process->errhandler)
			{
				process->line = process->errhandler;
				return (prog_RES_OK);
			}

		default:
			return (prog_RES_ERROR);
			break;
	}

	return (prog_RES_OK);
}


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
int prog_process_GetInteger(prog_process_t *process, int index)
{
	ASSERT(process);
	ASSERT(index < PROG_MAX_INT && index >= 0);

	return (process->integers[index]);
}


/*
 * prog_process_SetInteger
 *
 * Retreive the value of a processes integer
 *
 * in  -> process: process to get value from
 *        index: index of integer value
 *        value: value to set
 */
void prog_process_SetInteger(prog_process_t *process, int index, int value)
{
	ASSERT(process);
	ASSERT(index < PROG_MAX_INT && index >= 0);

	process->integers[index] = value;
}


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
char *prog_process_GetString(prog_process_t *process, int index)
{
	ASSERT(process);
	ASSERT(index < PROG_MAX_STRING && index >= 0);

	return (process->strings[index]);
}


/*
 * prog_process_SetString
 *
 * Retreive the value of a processes string
 *
 * in  -> process: process to get value from
 *        index: index of string value
 *        value: value to set
 */
void prog_process_SetString(prog_process_t *process, int index, const char *value)
{
	ASSERT(process);
	ASSERT(index < PROG_MAX_STRING && index >= 0);

	strncpy(process->strings[index], value, PROG_MAX_STRINGLEN);
	process->strings[index][PROG_MAX_STRINGLEN-1] = 0;
}


/*
 * prog_process_GetExitCode
 * 
 * Retrieve the exit code of a process
 * 
 * in  -> process: process to get value from
 * out <- exit code of process or -1 if process has not exited
 * 
 */
int prog_process_GetExitCode(prog_process_t *process)
{
	ASSERT(process);
	
	return (process->exitcode);
}


/*
 * progAddLine
 *
 * Adds a line to a program
 *
 */
prog_res_t progAddLine(prog_t *prog, prog_cmd_t *cmd, const char *params, unsigned long flags)
{
	prog_line_t	*line;

	ASSERT(prog);


	line = (prog_line_t *) malloc(sizeof (prog_line_t));

	if (!line)
	{
		return (prog_RES_MEM);
	}

	line->flags = flags;
	line->cmd = cmd;
	if (params)
	{
		memcpy(line->params, params, PROG_MAX_PARAM_LENGTH);
	}
	else
	{
		line->params[0] = '\0';
	}
	line->next = NULL;

	/* Add the line to the program */
	if (prog->lines)
	{
		prog_line_t *program;

		program = prog->lines;
		while (program->next)
		{
			program = program->next;
		}

		program->next = line;
	}
	else
	{
		prog->lines = line;
	}

	return (prog_RES_OK);
}


/*
 * progInternalFunctionLabel
 *
 * A label adds a new line which is null and contains the "label" in its paramaters
 *
 */
prog_res_t progInternalFunctionLabel(prog_t *prog, char *params)
{
	return (progAddLine(prog, NULL, params, PROG_LINE_FLAGS_LABEL));
}


/*
 * progInternalFunctionGoto
 *
 * Search through the program looking for the label specified, if the label is
 * found then add a command to the program which has the next line set as the label
 *
 */
prog_res_t progInternalFunctionGoto(prog_t *prog, char *params)
{
	return (progAddLine(prog, progFunctionGoto, params, PROG_LINE_FLAGS_RESOLV));
}


/*
 * progInternalFunctionOnError
 *
 */
prog_res_t progInternalFunctionOnError(prog_t *prog, char *params)
{
	return (progAddLine(prog, progFunctionOnError, params, PROG_LINE_FLAGS_RESOLV));
}


/*
 * progInternalFunctionSet
 *
 * Covert the variable reference into a pointer to the variable
 * Integer variables have the format Ix where x is the index, example I0, I1, I2
 * String variables have the format Sx where x is the index, example S0, S1, S2
 *
 * The expected paramaters are Ix number or Sx string
 *
 */
prog_res_t progInternalFunctionSet(prog_t *prog, char *params)
{
	int		argc;
	char	*argv[PROG_MAX_PARAM];

	prog_Char2Args(params, &argc, argv);

	if (*argv[0] == 'I') {
		prog_integer_set_t	set;

		if (argc != 2) {
			printf("Expected 2 paramaters, found %d\n", argc);
			return (prog_RES_ERROR);
		}
		set.var = atoi(argv[0] + 1);
		set.value = atoi(argv[1]);
		return progAddLine(prog, progFunctionSet, (char *) &set, 0);
	} else if (*argv[0] == 'S') {
		prog_string_set_t	setstring;
		
		setstring.var = atoi(argv[0] + 1);
		if (argc < 1) {
			printf("Expected variable after Set\n");
			return (prog_RES_ERROR);
		} else if (argc == 1) {
			/* clear the string if there is no value */
			setstring.value[0] = 0;
		} else {
			char *ptr;

			/* need to hack remainder of params back into a string. */
			/* note that this relies on the internals of prog_Char2Args. */
			ptr = argv[1]; 
			while (ptr - argv[1] < PROG_MAX_STRINGLEN - 1) {
				/* convert \0's before the end of the last arg back to ' 's. */
				if (*ptr == 0) {
					if (ptr > argv[argc - 1])
						break;
					else
						*ptr = ' ';
				}
				ptr++;
			}
			*ptr = 0;
			/* argv[1] now points to the start of one long whitespace
			 * containing second parameter.
			 */
		}
		strncpy(setstring.value, argv[1], PROG_MAX_STRINGLEN);
		return progAddLine(prog, progFunctionSetString, (char *)&setstring, 0);
	} else {
		printf("Expected 'I' or 'S' found %c\n", *argv[0]);
		return prog_RES_ERROR;
	}
}


/*
 * progInternalFunctionAdd
 *
 */
prog_res_t progInternalFunctionAdd(prog_t *prog, char *params)
{
	prog_integer_add_t	add;

	int		argc;
	char	*argv[PROG_MAX_PARAM];

	prog_Char2Args(params, &argc, argv);

	if (argc != 2)
	{
		printf("Expected 2 paramaters, found %d\n", argc);
		return (prog_RES_ERROR);
	}

	if (*argv[0] != 'I')
	{
		printf("Expected 'I' found %c\n", *argv[0]);
		return (prog_RES_ERROR);
	}

	if (*argv[1] != 'I')
	{
		printf("Expected 'I' found %c\n", *argv[1]);
		return (prog_RES_ERROR);
	}

	add.var1 = atoi(argv[0] + 1);
	add.var2 = atoi(argv[1] + 1);

	return (progAddLine(prog, progFunctionAdd, (char *) &add, 0));
}

/*
 * progInternalFunctionDiv
 *
 */
prog_res_t progInternalFunctionDiv(prog_t *prog, char *params)
{
	prog_integer_add_t	add;

	int		argc;
	char	*argv[PROG_MAX_PARAM];

	prog_Char2Args(params, &argc, argv);

	if (argc != 2)
	{
		printf("Expected 2 paramaters, found %d\n", argc);
		return (prog_RES_ERROR);
	}

	if (*argv[0] != 'I')
	{
		printf("Expected 'I' found %c\n", *argv[0]);
		return (prog_RES_ERROR);
	}

	if (*argv[1] != 'I')
	{
		printf("Expected 'I' found %c\n", *argv[1]);
		return (prog_RES_ERROR);
	}

	add.var1 = atoi(argv[0] + 1);
	add.var2 = atoi(argv[1] + 1);

	return (progAddLine(prog, progFunctionDiv, (char *) &add, 0));
}


/*
 * progInternalFunctionIf
 *
 */
prog_res_t progInternalFunctionIf(prog_t *prog, char *params)
{
	prog_integer_if_t	i;
	int		argc;
	char	*argv[PROG_MAX_PARAM];
	int flags;

	prog_Char2Args(params, &argc, argv);

	if (argc != 4)
	{
		printf("Expected 4 paramaters, found %d\n", argc);
		return (prog_RES_ERROR);
	}

	if (*argv[0] != 'I')
	{
		printf("Expected 'I' found %c\n", *argv[0]);
		return (prog_RES_ERROR);
	}

	if (*argv[2] != 'I')
	{
		printf("Expected 'I' found %c\n", *argv[2]);
		return (prog_RES_ERROR);
	}

	i.var1 = atoi(argv[0] + 1);
	i.var2 = atoi(argv[2] + 1);

	switch (*argv[1])
	{
		case '=':
			i.type = prog_integer_if_type_eq;
			break;

		case '!':
			i.type = prog_integer_if_type_ne;
			break;

		case '<':
			i.type = prog_integer_if_type_lt;
			break;

		case '>':
			i.type = prog_integer_if_type_gt;
			break;

		default:
			printf("Exected !=<> found %c\n",  *argv[1]);
			break;
	}

	i.line = prog_Resolv(argv[3], prog);

	/* If it can't be resolved now, resolve it later. */
	flags = 0;
	if (!i.line)
	{
		strcpy(i.labeltext, argv[3]);
		flags = PROG_LINE_FLAGS_IFRESOLV;
	}

	return (progAddLine(prog, progFunctionIf, (char *) &i, flags));
}


/*
 * progInternalFunctionExit
 *
 */
prog_res_t progInternalFunctionExit(prog_t *prog, char *params)
{
	return (progAddLine(prog, progFunctionExit, params, 0));
}


/*
 * progFunctionGoto
 *
 */
prog_cmd_res_t progFunctionGoto(prog_process_t *process, const char *params, void *context)
{
	memcpy(&process->line, params, sizeof (&process->line));
	return (prog_cmd_RES_WAIT);
}


/*
 * progFunctionOnError
 *
 */
prog_cmd_res_t progFunctionOnError(prog_process_t *process, const char *params, void *context)
{
	memcpy(&process->errhandler, params, sizeof (&process->line));
	return (prog_cmd_RES_CONTINUE);
}


/*
 * progFunctionSet
 *
 */
prog_cmd_res_t progFunctionSet(prog_process_t *process, const char *params, void *context)
{
	prog_integer_set_t	*set = (prog_integer_set_t *) params;

//	printf("I%d = %d\n", set->var, set->value); 

	process->integers[set->var] = set->value;
	return (prog_cmd_RES_CONTINUE);
}


/*
 * progFunctionSetString
 *
 */
prog_cmd_res_t progFunctionSetString(prog_process_t *process, const char *params, void *context)
{
	prog_string_set_t *setstring = (prog_string_set_t *) params;

//	printf("S%d = %d\n", setstring->var, setstring->value); 

	strncpy(process->strings[setstring->var], setstring->value, PROG_MAX_STRINGLEN);
	return (prog_cmd_RES_CONTINUE);
}


/*
 * progFunctionAdd
 *
 */
prog_cmd_res_t progFunctionAdd(prog_process_t *process, const char *params, void *context)
{
	prog_integer_add_t	*add = (prog_integer_add_t *) params;

/*	printf("I%d [%d] += I%d [%d] => %d\n", 
		add->var1, process->integers[add->var1], 
		add->var2, process->integers[add->var2],
		process->integers[add->var1] + process->integers[add->var2]);
*/
	process->integers[add->var1] += process->integers[add->var2];
	return (prog_cmd_RES_CONTINUE);
}

/*
 * progFunctionDiv
 *
 */
prog_cmd_res_t progFunctionDiv(prog_process_t *process, const char *params, void *context)
{
	prog_integer_add_t	*add = (prog_integer_add_t *) params;

/*	printf("I%d [%d] /= I%d [%d] => %d\n", 
		add->var1, process->integers[add->var1], 
		add->var2, process->integers[add->var2],
		process->integers[add->var1] + process->integers[add->var2]);
*/
	process->integers[add->var1] /= process->integers[add->var2];
	return (prog_cmd_RES_CONTINUE);
}


/*
 * progFunctionIf
 *
 */
prog_cmd_res_t progFunctionIf(prog_process_t *process, const char *params, void *context)
{
	prog_integer_if_t	*i = (prog_integer_if_t *) params;

	switch (i->type)
	{
		case prog_integer_if_type_eq:
/*
			printf("IF I%d [%d] = I%d [%d] => %s\n", 
				i->var1, process->integers[i->var1],
				i->var2, process->integers[i->var2],
				(process->integers[i->var1] == process->integers[i->var2]) ? "TRUE" : "FALSE");
*/
			if (process->integers[i->var1] == process->integers[i->var2])
			{
				process->line = i->line;
				return (prog_cmd_RES_WAIT);
			}
			break;

		case prog_integer_if_type_ne:
/*
			printf("IF I%d [%d] ! I%d [%d] => %s\n", 
				i->var1, process->integers[i->var1],
				i->var2, process->integers[i->var2],
				(process->integers[i->var1] != process->integers[i->var2]) ? "TRUE" : "FALSE");
*/
			if (process->integers[i->var1] != process->integers[i->var2])
			{
				process->line = i->line;
				return (prog_cmd_RES_WAIT);
			}
			break;

		case prog_integer_if_type_gt:
/*
			printf("IF I%d [%d] > I%d [%d] => %s\n", 
				i->var1, process->integers[i->var1],
				i->var2, process->integers[i->var2],
				(process->integers[i->var1] > process->integers[i->var2]) ? "TRUE" : "FALSE");
*/
			if (process->integers[i->var1] > process->integers[i->var2])
			{
				process->line = i->line;
				return (prog_cmd_RES_WAIT);
			}
			break;

		case prog_integer_if_type_lt:
/*
			printf("IF I%d [%d] < I%d [%d] => %s\n", 
				i->var1, process->integers[i->var1],
				i->var2, process->integers[i->var2],
				(process->integers[i->var1] < process->integers[i->var2]) ? "TRUE" : "FALSE");
*/
			if (process->integers[i->var1] < process->integers[i->var2])
			{
				process->line = i->line;
				return (prog_cmd_RES_WAIT);
			}
			break;

		default:
			return (prog_cmd_RES_ERROR);

	};
	return (prog_cmd_RES_CONTINUE);
}


/*
 * progFunctionIf
 *
 */
prog_cmd_res_t progFunctionExit(prog_process_t *process, const char *params, void *context)
{
	process->exitcode = atoi(params);
	return (prog_cmd_RES_EXIT);
}


/*
 * prog_Resolv
 *
 * Given a line of code which has a label as its paramaters,
 * search through the program looking for the labe.  If found
 * replace the paramaters with the pointer to the label...
 *
 */
prog_line_t *prog_Resolv(const char *label, prog_t *prog)
{
	prog_line_t *line;

	line = prog->lines;

	while (line)
	{
		if ((line->flags & PROG_LINE_FLAGS_LABEL) && !strcmp(line->params, label))
		{
			return (line);
		}
		line = line->next;
	}

	/* If we made it to here then we didn't find the label */
	return (NULL);
}



/*
 * prog_Char2Args
 *
 * Coverts a character string into an argc, argv list
 *
 * in  -> string: string to be converted
 *        argc  : pointer to variable which will hold the number of arguments
 *        argv  : pointer to pointer array
 */
void prog_Char2Args(char *string, int *argc, char **argv)
{
	char *ptr;

	if (!string || *string == '\0')
	{
		*argc = 0;
		return;
	}
	
	ptr = string;
	argv[0] = ptr;
	*argc = 1;

	while (ptr = strchr(ptr, ' '))
	{
		*ptr = '\0';
		ptr++;
		argv[*argc] = ptr;
		(*argc)++;
	}
}


#if 0

prog_cmd_res_t func1(const char *params, void *context)
{
	if (PROG_CMD_TEST_PARAM)
	{
		printf("func1 [string]\n");
		return (prog_cmd_RES_OK);
	}

	if (PROG_CMD_TEST_INFO)
	{
		printf("Test function which displays the paramaters by printing them\n");
		return (prog_cmd_RES_OK);
	}

	printf("Function One: %s\n", params);
	return (prog_cmd_RES_CONTINUE);
}

prog_cmd_res_t func2(const char *params, void *context)
{
	if (PROG_CMD_TEST_PARAM)
	{
		printf("func2 [string]\n");
		return (prog_cmd_RES_OK);
	}

	if (PROG_CMD_TEST_INFO)
	{
		printf("Test function which displays the paramaters by printing them\n");
		return (prog_cmd_RES_OK);
	}

	printf("Function Two: %s\n", params);
	return (prog_cmd_RES_CONTINUE);
}

prog_cmd_res_t func3(const char *params, void *context)
{
	if (PROG_CMD_TEST_PARAM)
	{
		printf("func3 [string]\n");
		return (prog_cmd_RES_OK);
	}

	if (PROG_CMD_TEST_INFO)
	{
		printf("Test function which displays the paramaters by printing them\n");
		return (prog_cmd_RES_OK);
	}

	printf("Function Three: %s\n", params);
	return (prog_cmd_RES_CONTINUE);
}

prog_cmd_res_t func4(const char *params, void *context)
{
	if (PROG_CMD_TEST_PARAM)
	{
		printf("func4 [string]\n");
		return (prog_cmd_RES_OK);
	}

	if (PROG_CMD_TEST_INFO)
	{
		printf("Test function which displays the paramaters by printing them\n");
		return (prog_cmd_RES_OK);
	}

	printf("Function Four: %s\n", params);
	return (prog_cmd_RES_CONTINUE);
}

int main(void)
{
	prog_syntax_t	syntax;
	prog_t			program;
	prog_process_t	process;

	prog_syntax_Create(&syntax);

	prog_syntax_AddCmd(&syntax, "FUNCTION1", func1);
	prog_syntax_AddCmd(&syntax, "FUNCTION2", func2);
	prog_syntax_AddCmd(&syntax, "FUNCTION3", func3);
	prog_syntax_AddCmd(&syntax, "FUNCTION4", func4);

	prog_Load(&program, &syntax, "TEST.PR");

	prog_process_Init(&process, &program);

	while (prog_process_Step(&process, "WOOP") == prog_RES_OK)
		;

	prog_UnLoad(&program);
	prog_syntax_Destroy(&syntax);

	return (0);
}

#endif


