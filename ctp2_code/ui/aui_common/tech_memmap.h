










#ifndef __TECH_MEMMAP_H__
#define __TECH_MEMMAP_H__



class tech_MemMap
{
public:
	tech_MemMap() {}
	virtual ~tech_MemMap() {}

	virtual unsigned char *GetFileBits(
		char *filename,
		size_t *filesize = NULL );
	virtual void ReleaseFileBits( unsigned char *&bits );

	
	static int GetFileExtension( char *filename, char *extension );
};


#endif 
