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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include "strlkup.h"

StrLookup StrLookup_Global_Object = NULL;

#define SPACESTR  " \n\r\t\v\f"
#define SEPARATOR "\x7f"

/* The comparison routine for doing STRLOOKUPs
 */
static int compare_table( const void *elem1, const void *elem2 )
{
	return strcmp(((Table_ptr) elem1)->index, ((Table_ptr) elem2)->index);
}

/* Convert C escape sequences in a read-in string to their character
 * equivalents
 */
static char *str_convert(char *buffer)
{
	int inpos = 0, outpos = 0;
	char num[4];
	int digits, chrcode;

	if (!buffer) return NULL;
	/* Scan the string to the nul terminator
	 * Convert in-place since the string can't get longer than it is...
	 */
	while (buffer[inpos]) {
		/* Yeah, I hate big switch statements too. */
		switch (buffer[inpos]) {
			case '\n':
			case '\r':
			case '\v':
			case '\f':
				/* Newlines in lang.txt get ignored */
				inpos++;
				break;
			case '\\':
				/* Backslash found; we have an escaped character.
				 * Look at the next character in the string
				 */
				switch (buffer[++inpos]) {
					/* Big nested switch statements are particularly bad */
					case '\0':
						/* In case of string end, break glass...er, out */
						break;
					/* Deal with the easy standard C escape codes */
					case 'a':	buffer[outpos++] = '\a'; inpos++; break;
					case 'b':	buffer[outpos++] = '\b'; inpos++; break;
					case 'f':	buffer[outpos++] = '\f'; inpos++; break;
					case 'n':	buffer[outpos++] = '\n'; inpos++; break;
					case 'r':	buffer[outpos++] = '\r'; inpos++; break;
					case 't':	buffer[outpos++] = '\t'; inpos++; break;
					case 'v':	buffer[outpos++] = '\v'; inpos++; break;
					case '\'':	buffer[outpos++] = '\''; inpos++; break;
					case '"':	buffer[outpos++] = '"';  inpos++; break;
					case '\\':	buffer[outpos++] = '\\'; inpos++; break;
					case '?':	buffer[outpos++] = '?';	 inpos++; break;
					/* Now we have the hex and octal cases */
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
						/* The next 1-3 chars after the \ (including this
						 * one) are octal digits
						 */
						digits = 0;
						while (isdigit(buffer[inpos])
						&&     buffer[inpos] != '8' && buffer[inpos] != '9'
						&&     digits < 3) {
							num[digits++] = buffer[inpos++];
						}
						num[digits] = '\0';
						/* Convert the octal and add it to the string */
						sscanf(num, "%o", &chrcode);
						buffer[outpos++] = chrcode;
						break;
					case 'x':
						/* The next 1-3 chars after the x (not including
						 * the x) are hex digits
						 */
						digits = 0;
						inpos++;
						while (isxdigit(buffer[inpos]) && digits < 3) {
							num[digits++] = buffer[inpos++];
						}
						num[digits] = '\0';
						/* Unlike the octal case, we may have no digits */
						if (digits) {
							/* Convert the hex and add it to the string */
							sscanf(num, "%x", &chrcode);
							buffer[outpos++] = chrcode;
						} else {
							/* No hex digits; pass through the backslash
							 * and x unchanged (probably incorrect)
							 */
							buffer[outpos++] = '\\';
							buffer[outpos++] = 'x';
						}
						break;
					default:
						/* Incorrect escape code; pass through the backslash
						 * and next character unchanged (probably incorrect)
						 */
						buffer[outpos++] = '\\';
						buffer[outpos++] = buffer[inpos++];
						break;
				}
				/* End of the the escape code case */
				break;
			default:
				/* This isn't a special character; copy the character
				 * from in to out */
				buffer[outpos++] = buffer[inpos++];
				break;
		}
	}
	/* Re-terminate the string and return the buffer */
	buffer[outpos] = '\0';
	return buffer;
}

/* Read lines from a file until a blank line is encountered
 */
static char *read_until_blank_line(FILE *fp, int *line_num)
{
	char buffer[256], *str_read = NULL;

	/* Grab a line from the file */
	while (fgets(buffer, 256, fp)) {
		size_t length = strlen(buffer);
		++*line_num;

		/* If the line grabbed is all whitespace, return */
		if (strspn(buffer, SPACESTR) == length) {
			/* If this line in the file is all whitespace,
			 * convert the escape sequences and return the string read */
			return str_convert(str_read);
		}
		/* Concatenate this line onto what's already been read */
		if (str_read) {
			/* Reallocate the output string */
			if (!(str_read = realloc(str_read, strlen(str_read)+length+2))) return NULL;
			/* Add a space to the string */
			strcat(str_read, " ");
		} else {
			/* Get a new string */
			if (!(str_read = malloc(length + 1))) return NULL;
			str_read[0] = '\0';
		}
		strcat(str_read, buffer);
	}
	/* Ran out of file; return the string read
	 * Note that this will return NULL if no string was read */
	return str_read;
}

/* Debugging options */
#define ELBONIAN 	1
#define STARS		2
#define LINENUM		4
#define XTDELBON	8	/* Extended Elbonian - NYI */

/* Build a translation string from an english string with debugging options
 */
static void do_options(char **english, char **xlation,
	int options, int line_num)
{
	char *xl_temp;

	if (options & ELBONIAN || options & XTDELBON) {
		/* Elbonian mode; built a bigger version of the English string
		 * with extra vowels.  Assumes character set is ISO-Latin.
		 */
		int length = strlen(*xlation), inpos = 0, outpos = 0;

		/* First, allocate a temporary string */
		if (!(xl_temp = malloc(length * 2 + 1))) return;

		/* Scan the string to the nul terminator
		 * Convert the english string to the Elbonian string
		 */
		while ((*xlation)[inpos]) {
			/* Copy the character from english to translated */
			xl_temp[outpos++] = (*xlation)[inpos];
			switch ((*xlation)[inpos]) {
				/* If it's a vowel, tack on a bonus umlaut vowel */
				case 'A': xl_temp[outpos++] = '\x0c4'; break;
				case 'a': xl_temp[outpos++] = '\x0e4'; break;
				case 'E': xl_temp[outpos++] = '\x0cb'; break;
				case 'e': xl_temp[outpos++] = '\x0eb'; break;
				case 'I': xl_temp[outpos++] = '\x0cf'; break;
				case 'i': xl_temp[outpos++] = '\x0ef'; break;
				case 'O': xl_temp[outpos++] = '\x0d6'; break;
				case 'o': xl_temp[outpos++] = '\x0f6'; break;
				case 'U': xl_temp[outpos++] = '\x0dc'; break;
				case 'u': xl_temp[outpos++] = '\x0fc'; break;
				case 'y': xl_temp[outpos++] = '\x0ff'; break;
				case '?': xl_temp[outpos++] = '\x0bf'; break;
				case '!': xl_temp[outpos++] = '\x0a1'; break;
			}
			inpos++;
		}
		/* Terminate the Elbonian and set it to the translated string */
		xl_temp[outpos++] = '&';
		xl_temp[outpos] = '\0';
		free(*xlation);
		*xlation = xl_temp;
	}
	if (options & STARS) {
		int length = strlen(*xlation), i;

		/* Change all non-space characters in the string to stars,
		 * EXCEPT the first and last, which are dollar signs */
		for (i = 0; i < length; i++) {
			if (!strchr(SPACESTR, (*xlation)[i])) (*xlation)[i] = '*';
		}
		(*xlation)[0] = '$';
		(*xlation)[length - 1] = '$';
	}
	if (options & LINENUM) {
		/* Prepend the linenumber to the translated string */
		int length = strlen(*xlation) + 10;

		/* Allocate a temporary holding string */
		if (!(xl_temp = malloc(length))) return;

		/* Prepend the linenumber to the translated string */
		sprintf(xl_temp, "%d: %s", line_num, *xlation);
		free(*xlation);
		*xlation = xl_temp;
	}
}

/* Get an English string and its translated equivalent from a file
 * Sets the xlat_options if an :OPTIONS: keyword is encountered
 */
static int get_string_pair(FILE *fp, char **english, char **xlation,
	int *xlat_options, int *line_num)
{
	char buffer[256];

	while (fgets(buffer, 256, fp)) {
		++*line_num;
		/* Check to see if we have a keyword string */
		if (!strnicmp(buffer, ":KEY:", 5)) {
			/* Grab the english string from the file */
			*english = read_until_blank_line(fp, line_num);
		} else if (!strnicmp(buffer, ":TRANSLATED:", 12)) {
			/* Grab the translated string from the buffer */
			*xlation = read_until_blank_line(fp, line_num);
			/* If we already have the english, return them both */
			if (*english) {
				if (!*xlation) {
					/* If there was no translation, copy it from English */
					if (!(*xlation = malloc(strlen(*english) + 1))) return 0;
					strcpy(*xlation, *english);
				}
				if (*xlat_options) {
					/* Build the translated string with debug options */
					do_options(english, xlation, *xlat_options, *line_num);
				}
				return 1;
			}
		} else if (!strnicmp(buffer, ":OPTIONS:", 9)) {
			/* Get the options from the file and process them */
			char *optstr = read_until_blank_line(fp, line_num);
			if (optstr) {
				int i;

				/* Convert the whole string to lower case */
				for (i = 0; i < (int)strlen(optstr); i++) {
					if (isupper(optstr[i])) optstr[i] = _tolower(optstr[i]);
				}
				if (strstr(optstr, "elbonian")) *xlat_options |= ELBONIAN;
				if (strstr(optstr, "stars")) *xlat_options |= STARS;
				if (strstr(optstr, "linenum")) *xlat_options |= LINENUM;
				if (strstr(optstr, "xtdelbon")) *xlat_options |= XTDELBON;
				free(optstr);
			}
		}
		/* No keyword; throw this line away and get another line */
	}
	/* We ran out of file!  We're done! */
	return 0;
}

/* Read in the lang.txt or other specified file for English strings
 * and their translations
 */
StrLookup StrLookupCreate(char *filename)
{
	StrLookup obj;
	char *index_string;
	char *xlated_string;
	FILE *fp;
	int allocated = 0, xlat_options = 0, linenum = 0;

	/* Open the file, lang.txt if none specified */
	if (!filename) filename = "lang.txt";
	if (fp = fopen(filename, "r")) {
		/* Allocate a strlookup object */
		if (obj = calloc(sizeof(StrLookupStruct), 1)) {
			/* Loop over getting the pairs of English and translated strings */
			while(get_string_pair(fp, &index_string, &xlated_string,
					&xlat_options, &linenum)) {
				if (allocated <= obj->stringcount) {
					Table_ptr new_lookup;

					/* We need to allocate some more space */
					allocated = allocated ? allocated * 2 : 100;
					if (!(new_lookup = (Table_ptr) realloc(obj->lookup_table,
							sizeof(struct StrLookup_table) * allocated))) {
						/* Note- this will return the table so far on error,
						   which is probably wrong */
						free(index_string);
						free(xlated_string);
						break;
					}
					obj->lookup_table = new_lookup;
				}
				/* Add these strings to the table */
				obj->lookup_table[obj->stringcount].index = index_string;
				obj->lookup_table[obj->stringcount].string = xlated_string;
				obj->stringcount++;
				index_string = xlated_string = NULL;
			}
			/* Finally, sort the whole table */
			qsort(obj->lookup_table, obj->stringcount,
					sizeof(struct StrLookup_table), compare_table);
		}
		fclose(fp);
	}
	else
		obj = 0;
	return obj;
}

void StrLookupDestroy(StrLookup obj)
{
	int i;

	if (obj && obj->lookup_table) {
		for (i = 0; i < obj->stringcount; i++) {
			free(obj->lookup_table[i].index);
			free(obj->lookup_table[i].string);
		}
		free(obj->lookup_table);
	}
	if (obj) free (obj);
	return;
}

char *StrLookupFind(StrLookup obj, char *string)
{
	struct StrLookup_table input;
	struct StrLookup_table *ret;

	if (obj) {
		input.index = string;
		if (ret = bsearch(&input, obj->lookup_table, obj->stringcount,
				sizeof(struct StrLookup_table), compare_table)) {
			string = ret->string;
		}
	}
	return string;
}

/* sprintf style string formatter with translation lookup.
 * Takes extended format specifications %num:fmt where num is the
 * argument number associated with the format spec, and fmt is the
 * standard C format spec
 */
int StrLookupFormat(char *buffer, char *fmt, ...)
{
	va_list args;
	char *fmt2,				/* An intermediate fmt string */
		**fmtspec = NULL,	/* Array storage for the format specs */
		*ptr, *ptr2;		/* Temporary pointers */
	int i,
		nfmtspec = 0,		/* Number of format specs encountered */
		totalfmtlen = 0;	/* Total length of the format specifiers */

	/* Get the lookup of the format string */
	if (!(fmt = STRLOOKUP(fmt))) return EOF;
	ptr = fmt;

	/* Allocate storage for the intermediate format string
	 * This string is the fmt string without the format specs,
	 * but WITH the format spec numbers and colons as placeholders */
	if (!(fmt2 = malloc(strlen(fmt) + 1))) return EOF;
	fmt2[0] = '\0';

	/* Start hunting for extended format specifiers %number:fmt */
	while ((ptr2 = strchr(ptr, '%')) != NULL) {
		size_t fmtlen;
		int fmtn;

		/* We have one; copy skipped string to the intermediate */
		if (ptr2 - ptr) strncat(fmt2, ptr, ptr2 - ptr);
		if (*(ptr2 + 1) == '%') {
			/* We have a %% format spec; just copy it over */
			strcat(fmt2, "%%");
			ptr = ptr2 + 2;
		} else {
			/* Get the number of this format spec */
			if (!sscanf(ptr2, "%%%d:", &fmtn)) return EOF;

			/* Skip forward to the colon (we already know is there)
			 * and copy the %number: to the intermediate */
			ptr = ptr2;
			ptr2 = strchr(ptr, ':');
			strncat(fmt2, ptr, ptr2 - ptr + 1);

			/* Now find the length of the rest of the format specifier */
			fmtlen = strcspn(ptr2, "diouxXeEfgGcCsSnp");
			totalfmtlen += fmtlen + 1;		/* We add 1 for the % here */

			if (fmtn > nfmtspec) {
				/* We need a bigger array for format specs */
				if (!(fmtspec = realloc(fmtspec, sizeof(char *)*fmtn))) return EOF;
				/* Null the new memory */
				memset(&fmtspec[nfmtspec], 0, (fmtn-nfmtspec)*sizeof(char *));
				nfmtspec = fmtn;
			}
			/* Allocate space for the format spec (and % and NUL) and copy
			 * Note that the numbers start with 1, so decrement */
			if (!(fmtspec[--fmtn] = malloc(fmtlen + 2))) return EOF;
			fmtspec[fmtn][0] = '%';
			/* Note that we skip past the colon by incrementing ptr2 */
			strncpy(&fmtspec[fmtn][1], ++ptr2, fmtlen);
			fmtspec[fmtn][fmtlen+1] = '\0';

			/* Skip past this format spec and continue parsing */
			ptr = ptr2 + fmtlen;
		}
	}
	/* No more formatting tokens found; grab the string tail */
	strcat(fmt2, ptr);

	/* Concatenate the found format specifiers with a separator */
	if (!(ptr = malloc(totalfmtlen + nfmtspec + 1))) return EOF;
	ptr[0] = '\0';
	for (i = 0; i < nfmtspec; i++) {
		if (fmtspec[i]) {
			strcat(ptr, fmtspec[i]);
			strcat(ptr, SEPARATOR);
		}
	}
	/* Now vsprintf the format strings into the output buffer;
	 * it's "guaranteed" to be large enough */
	va_start(args, fmt);
	if (vsprintf(buffer, ptr, args) == EOF) return EOF;
	va_end(args);

	/* Debugging output
	puts(fmt);
	puts(fmt2);
	puts(ptr);
	puts(buffer);
	 */

	/* Now, we have to parse the vsprintf string apart, but that's easy */
	free(ptr);		/* We're done with the string of format specs */
	ptr = buffer;
	for (i = 0; i < nfmtspec; i++) {
		if (fmtspec[i]) {
			if (!(ptr = strtok(ptr, SEPARATOR))) return EOF;
			if (!(fmtspec[i] = realloc(fmtspec[i], strlen(ptr)+1))) return EOF;
			strcpy(fmtspec[i], ptr);
			ptr = NULL;
		}
	}
	/* And finally, we assemble the result in the output buffer! */
	buffer[0] = '\0';
	ptr = fmt2;
	while ((ptr2 = strchr(ptr, '%')) != NULL) {
		int fmtn;

		/* We have a format spec number; copy skipped stuff to the output */
		if (ptr2 - ptr) strncat(buffer, ptr, ptr2 - ptr);
		if (*(ptr2 + 1) == '%') {
			/* We have a %% format spec; copy over a % */
			strcat(buffer, "%");
			ptr = ptr2 + 2;
		} else {
			/* Get the number from this format spec number */
			if (!sscanf(ptr2, "%%%d:", &fmtn)) return EOF;

			/* Now copy the vsprintf output for this spec */
			strcat(buffer, fmtspec[--fmtn]);

			/* Skip past this format spec number and continue */
			ptr = strchr(ptr2, ':') + 1;
		}
	}
	/* No more format spec numbers found; grab the string tail */
	strcat(buffer, ptr);

	/* Free the remaining intermediates and return the buffer */
	for (i = 0; i < nfmtspec; i++) if (fmtspec[i]) free(fmtspec[i]);
	free(fmtspec);
	free(fmt2);
	return strlen(buffer);
}
