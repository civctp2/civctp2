
#pragma once

#ifndef __FILENAME_DB_H__
#define __FILENAME_DB_H__ 1

class CivArchive ;
class Token;

struct FilenameNode { 
                        
	MBCHAR	m_name[_MAX_PATH]; 
	MBCHAR	m_filename[_MAX_PATH];
};

class FilenameDB { 
    
	
	
	
    sint32 m_size;
	
	

	
	
	
	

	FilenameNode *m_map; 
	
	

public:
	FilenameDB();
	FilenameDB(CivArchive &archive);
	~FilenameDB();
	
	void	SetSize(sint32 size);

	sint32	FindTypeIndex(const char *str) const; 
	MBCHAR	*GetFilename(sint32 index) const; 
    void	SetName(sint32 count, char str[_MAX_PATH]); 
    void	SetFilename(sint32 count, MBCHAR *s);

	void	Serialize(CivArchive &archive) ;

	sint32	ParseAFilename(Token *token, sint32 count);
	sint32	Parse(char *filename);
};

#endif
