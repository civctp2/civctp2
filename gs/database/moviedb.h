
#pragma once

#ifndef __MOVIE_DB_H__
#define __MOVIE_DB_H__ 1

class CivArchive ;
class Token;

struct MovieNameNode { 
                        
	MBCHAR	m_name[_MAX_PATH]; 
	MBCHAR	m_movieFilename[_MAX_PATH];
};

class MovieDB { 
    
	
	
    sint32 m_size;
	
	

	
	
	

	MovieNameNode *m_map; 
	
	

public:


	MovieDB();
	MovieDB(CivArchive &archive);
	~MovieDB();
	
	void	SetSize(sint32 size);

	sint32	FindTypeIndex(char *str) const; 
	MBCHAR	*GetMovieFilename(sint32 index) const; 
    void	SetName(sint32 count, char str[_MAX_PATH]); 
    void	SetMovieFilename(sint32 count, MBCHAR *s);

	void	Serialize(CivArchive &archive) ;

	sint32	ParseAMovie(Token *iconToken, sint32 count);
	sint32	Parse(char *filename);
};

#endif