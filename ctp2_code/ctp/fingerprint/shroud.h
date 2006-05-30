













































#ifndef shroud_h
#define shroud_h


#include <stdio.h>

















typedef struct
{
	FILE	*file;
	size_t	curpos;
	int		mode;
} shroud_t;





enum
{
	shroud_READ,
	shroud_WRITE,
	shroud_APPEND,
	shroud_UPDATE
};





#define shroud_BLOCKSIZE 512







shroud_t *shroud_readopen( char *filename );







shroud_t *shroud_writeopen( char *filename );











shroud_t *shroud_appendopen( char *filename );









shroud_t *shroud_updateopen( char *filename );







size_t shroud_read( shroud_t *sfile, unsigned char *buffer, size_t length );








size_t shroud_write( shroud_t *sfile, unsigned char *buffer, size_t length );







int shroud_seek( shroud_t *sfile, long offset, int origin );






int shroud_close( shroud_t *sfile );






















void shroud_encode( unsigned char *bytes, size_t length, size_t operand );






















void shroud_decode( unsigned char *bytes, size_t length, size_t operand );


#endif
