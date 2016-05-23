/*******************************************************************************

	$Workfile:: ldl_file.hpp                                                   $
	  $Author$
	    $Date$
	$Rev$
	 $Archive:: /LDL/test/ldl_file.hpp                                         $

*******************************************************************************/

#ifndef LDL_FILE_HPP
#define LDL_FILE_HPP

#define MAX_HASH 10000
#define MAX_STRING_LENGTH	4096

#define LOWER(c)   (((c)>='A'  && (c) <= 'Z') ? ((c)+('a'-'A')) : (c))
#define UPPER(c)   (((c)>='a'  && (c) <= 'z') ? ((c)+('A'-'a')) : (c) )

class ldl;
class ldl_datablock;

class ldl {
public:
	ldl(const char *szName, const char *szOutputDir = NULL, BOOL bUseInternalMemmap = TRUE );
	~ldl();

	// read an ldl file and new the data associated with it
	BOOL ReadData(const char *fname = NULL);

	ldl_datablock *FindDataBlock(const char *szName, ldl_datablock *dbParent = NULL );

	BOOL WriteData();
};

#endif
