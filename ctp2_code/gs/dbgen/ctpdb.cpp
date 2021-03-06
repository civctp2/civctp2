//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Database generator
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// _DEBUG
// - Generate debug version when set.
//
// HAVE_UNISTD_H
//
// WIN32
//
// _MAX_PATH
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Variable 'or' renamed, because this a reserved symbol (same as ||) now.
// - Modifed db_add_bit_pair function to allow bit pairs to have default
//   values so that when two records are merged, only the bit is merged
//   in that is set. - Sep. 28th 2004 Martin G�hmann
// - Portability improved (ctplinuxfan).
// - Added method to determine the number of entries that should be parsed
//   in order to support the old pollution database. (July 15th 2006 Martin G�hmann)
// - Added map.txt support. (27-Mar-2007 Martin G�hmann)
// - Added Const.txt support. (29-Jul-2007 Martin G�hmann)
// - Reported failure to create a (writable) destination file in copy_file
//
//----------------------------------------------------------------------------

#include "ctp2_config.h"
#include "ctp2_inttypes.h"
#include "ctpdb.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif
#if defined(WIN32)
#include <windows.h>
#include <direct.h>
#else
#include "windows.h"
#endif
#include <time.h>
#include <errno.h>
#include <inttypes.h>

#include "RecordDescription.h"

RecordDescription * g_record = NULL;

#if !defined(PATH_MAX)
#if defined(_MAX_PATH)
#define PATH_MAX _MAX_PATH
#else
#define PATH_MAX 1024
#endif  // _MAX_PATH
#endif  // PATH_MAX

#ifndef FREAD_BUF_SIZE
#define FREAD_BUF_SIZE 16384
#endif

// located in ctpdb.y and ctpdb.l
extern "C" int yyparse();
extern "C" void yyrestart(FILE * input);
extern "C" int s_done;
extern "C" int g_line_number;
extern "C" int g_generateRequirementWarnings;

/*
	Usage: ctpdb [-r] [outputdir | -i inputfile]
*/
int main(int argc, const char **argv)
{
	int	errorFound	= 1;	/* Not started yet */
	int arg;
	const char *outputDir = NULL;
	bool output_into_working_dir = false;
	g_generateRequirementWarnings = 1;

	for(arg = 1; arg < argc; arg++) {
		if(argv[arg][0] == '-') {
			if(strcmp(argv[arg], "-r") == 0) {
				g_generateRequirementWarnings = 0;
			}
			if(strcmp(argv[arg], "-i") == 0) {
				output_into_working_dir = true;
			}
		} else {
			outputDir = argv[arg];
		}
	}

	if(!outputDir) {
		fprintf(stderr, "Usage: %s [-r] [outputdir | -i inputfile]\n", argv[0]);
		exit(errorFound);
	}

	if(output_into_working_dir) {
		FILE * input = fopen(outputDir, "r");
		if (input == NULL)
		{
			printf("Coundn't open file: %s", outputDir);
			exit(errorFound);
		}
    yyrestart(input);
		outputDir = ".";
	}
	db_set_output_dir(outputDir);

	g_line_number = 1;
	for (s_done = 0; !s_done; /* s_done updated in yyparse */ )
	{
		errorFound = yyparse();
	}
	exit(errorFound);
}

namespace
{
char s_output_dir[1 + PATH_MAX];

/// Report a problem with file handling
/// \param a_FileName The name of the file causing the problem
/// \param a_Reason   Description of the problem
/// \remarks Assumption: Both \a a_FileName and \a a_Reason are non-NULL
void ReportFileError(char const * a_FileName, char const * a_Reason)
{
	fprintf(stderr, "\ndbgen: file '%s' %s. ", a_FileName, a_Reason);
}

/// Copy the contents of one file to another
/// \param srcFName The name of the file to copy from
/// \param dstFName The name of the file to copy to
/// \return Indication whether copying succeeded
bool copy_file(char const * srcFName, char const * dstFName)
{
	Assert(srcFName);
	Assert(dstFName);

	FILE * inFile = fopen(srcFName, "r");
	Assert(inFile);
	if (!inFile)
	{
		ReportFileError(srcFName, "not found");
		return false;
	}

	FILE *outFile = fopen(dstFName, "w");
	Assert(outFile);
	if (!outFile) {
		fclose(inFile);
		ReportFileError(dstFName, "could not be created");
		return false;
	}

	char buf[FREAD_BUF_SIZE];
	size_t read;
	size_t wrote = 0;

	while ((read = fread((void *)&buf, sizeof(char),
	                     sizeof(buf) / sizeof(char), inFile
	                    )
	       ) > 0) {
		wrote = fwrite((void *)&buf, sizeof(char),
		               read, outFile);

	}

	fclose(outFile);
	fclose(inFile);

	return (static_cast<size_t>(-1) != read)
	    && (static_cast<size_t>(-1) != wrote);
}

} // namespace

void db_set_output_dir(const char *dir)
{
    memset(s_output_dir, '\0', sizeof(s_output_dir));
	strncpy(s_output_dir, dir, PATH_MAX);
}

static const char *db_get_code_directory()
{
	return s_output_dir;
}

void db_start_record(char *name)
{
	delete g_record;
	g_record = new RecordDescription(name);
}

void db_start_record_allows_single(char *name)
{
	delete g_record;
	g_record = new RecordDescription(name, true);
}

FILE *db_open_file(const char *filename)
{
	FILE *outfile = fopen(filename, "w");

	if (!outfile)
    {
		ReportFileError(filename, "could not be created");
		return NULL;
    }

	fprintf(outfile, "\n/*\n");
	fprintf(outfile, " * DO NOT EDIT THIS FILE!\n");
	fprintf(outfile, " * It is generated automatically by ctpdb\n");
	fprintf(outfile, " */\n");

	return outfile;
}

bool db_files_differ(char const * newFilePath, char const * oldFilePath)
{
	FILE * n = fopen(newFilePath, "r");
	if (!n) return true;

	FILE * o = fopen(oldFilePath, "r");
	if (!o) {
		fclose(n);
		return true;
	}

#define DIFF_SIZE FREAD_BUF_SIZE

	while(!feof(n) && !feof(o)) {
		char nb[DIFF_SIZE], ob[DIFF_SIZE];
		size_t const    nr      = fread(nb, 1, DIFF_SIZE, n);
		size_t const    oldr    = fread(ob, 1, DIFF_SIZE, o);

		if (nr != oldr) {
			fclose(n);
			fclose(o);
			return true;
		}

		for (size_t i = 0; i < nr; i++)
		{
			if(nb[i] != ob[i]) {
				fclose(n);
				fclose(o);
				return true;
			}
		}
	}
	fclose(n);
	fclose(o);
	return false;
}

void db_maybe_copy(char * newFilePath)
{
	char oldpath[PATH_MAX];
	getcwd(oldpath, PATH_MAX);

	chdir(db_get_code_directory());

	char oldFilePath[PATH_MAX];
	strcpy(oldFilePath, newFilePath);

	char *dot = strrchr(oldFilePath, '.');
	if(!dot) {
		chdir(oldpath);
		return;
	}
	*dot = 0;

	FILE *  oldFile     = fopen(oldFilePath, "r");
    bool    hasOldFile  = oldFile != NULL;
    bool    hasChanges  = !hasOldFile;

    if (hasOldFile)
    {
        fclose(oldFile);
        hasChanges = db_files_differ(oldFilePath, newFilePath);

	    if (hasChanges)
	    {
			char backupPath[PATH_MAX] = { 0 };
			sprintf(backupPath, "%s.old", oldFilePath);

#ifdef _DEBUG
			printf("%s -> %s\n", oldFilePath, backupPath);
#endif
			if (!copy_file(oldFilePath, backupPath))
			{
			    fprintf(stderr, "%s\n", strerror(errno));
			}
		}
	}
    // else No action: first time generation

    if (hasChanges)
    {
#ifdef _DEBUG
		printf("%s -> %s\n", newFilePath, oldFilePath);
#endif
		if (!copy_file(newFilePath, oldFilePath))
		{
		    fprintf(stderr, "%s\n", strerror(errno));
		}
    }

	unlink(newFilePath);
	chdir(oldpath);
}

void db_end_record(char *name)
{
	char filename[PATH_MAX];
	sprintf(filename, "%s" FILE_SEP "%sRecord.h.new",
	        db_get_code_directory(), name);

	FILE * outfile = db_open_file(filename);
	if(!outfile)
		return;

	g_record->ExportHeader(outfile);
	fclose(outfile);

	db_maybe_copy(strrchr(filename, FILE_SEPC) + 1);

	sprintf(filename, "%s" FILE_SEP "%sRecord.cpp.new",
	        db_get_code_directory(), name);

	outfile = db_open_file(filename);
	Assert(outfile);
	if(!outfile)
		return;

	g_record->ExportCode(outfile);
	fclose(outfile);

	db_maybe_copy(strrchr(filename, FILE_SEPC) + 1);

	sprintf(filename, "%s" FILE_SEP "%sRecord.stamp",
	        db_get_code_directory(), name);
	FILE * stamp = fopen(filename, "w");
	Assert(stamp);
	if(stamp) {
		fprintf(stamp, "//%" PRId64 "\n", time(0));
		fclose(stamp);
	}

	delete g_record;
	g_record = NULL;
}

void db_make_int_db(char *name)
{
	db_start_record(name);
	g_record->SetBaseType(DATUM_INT);
	db_end_record(name);
}

void db_make_float_db(char *name)
{
	db_start_record(name);
	g_record->SetBaseType(DATUM_FLOAT);
	db_end_record(name);
}

void db_make_string_db(char *name)
{
	db_start_record(name);
	g_record->SetBaseType(DATUM_STRING);
	db_end_record(name);
}

void db_make_string_id_db(char *name)
{
	db_start_record(name);
	g_record->SetBaseType(DATUM_STRINGID);
	db_end_record(name);
}

void db_start_member_class(char *name)
{
	Assert(g_record);
	g_record->StartMemberClass(name);
}

void db_end_member_class(char *name)
{
	Assert(g_record);
	g_record->EndMemberClass(name);
}

void db_add_simple
(
	DATUM_TYPE 			a_Type,
	struct namelist * 	a_List,
	struct fieldsize *	a_Size
)
{
	Assert(g_record);
	for
	(
		struct namelist *	it	= a_List;
		it;
		/* it updated in loop */
	)
	{
		struct namelist * toAdd	= it;
		it = it->next;

		g_record->AddDatum(a_Type, toAdd, a_Size->minSize, a_Size->maxSize);
		free(toAdd);
	}
}

void db_add_bits(struct namelist *list, struct fieldsize *size)
{
	db_add_simple(DATUM_BIT, list, size);
}

void db_add_ints(struct namelist *list, struct fieldsize *size)
{
	db_add_simple(DATUM_INT, list, size);
}

void db_add_ints_prebody(struct namelist *list, struct fieldsize *size)
{
	Assert(g_record);
	while(list) {
		g_record->AddDatum(DATUM_INT, list, size->minSize, size->maxSize, NULL, true);
		struct namelist *next = list->next;
		free(list);
		list = next;
	}
}

void db_add_floats(struct namelist *list, struct fieldsize *size)
{
	db_add_simple(DATUM_FLOAT, list, size);
}

void db_add_records(char *recType, struct namelist *list, fieldsize *size)
{
	Assert(g_record);
	while(list) {
		g_record->AddDatum(DATUM_RECORD, list, size->minSize, size->maxSize,
						   recType);
		struct namelist *next = list->next;
		free(list);
		list = next;
	}
}

void db_add_structs(char *structType, struct namelist *list, fieldsize *size)
{
	Assert(g_record);
	while(list) {
		g_record->AddDatum(DATUM_STRUCT, list, size->minSize, size->maxSize,
						   structType);
		struct namelist *next = list->next;
		free(list);
		list = next;
	}
}

void db_add_filenames(struct namelist *list, fieldsize *size)
{
	db_add_simple(DATUM_FILE, list, size);
}

void db_add_strings(struct namelist *list, struct fieldsize *size)
{
	db_add_simple(DATUM_STRING, list, size);
}

void db_add_string_ids(struct namelist *list, struct fieldsize *size)
{
	db_add_simple(DATUM_STRINGID, list, size);
}

void db_add_bit_pair(struct namelist *list, struct fieldsize *size, struct bitpairtype *pairtype)
{
	Assert(g_record);
	while(list) {
		g_record->AddBitPair(list, size->minSize, size->maxSize, pairtype);
		struct namelist *next = list->next;
		free(list);
		list = next;
	}
}

void db_add_grouped_bits(char *groupName, struct namelist *list)
{
	Assert(g_record);
	g_record->AddGroupedBits(groupName, list);
}

void db_add_parse_num(sint32 parseNum)
{
	Assert(g_record);
	g_record->SetParseNum(parseNum);
}
