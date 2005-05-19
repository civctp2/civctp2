









#pragma once
#ifndef __CIVARCHIVE_H__
#define __CIVARCHIVE_H__

#include "ic3CivArchive.h"

#include "globals.h"
#include <SDL_endian.h>

#define k_ARCHIVE_MAGIC_VALUE_1	'OTAK'
#define k_ARCHIVE_MAGIC_VALUE_2	'U-98'

class GameFile ;
class DataCheck ;

class CivArchive : public IC3CivArchive
	{
	private:

        ULONG m_refCount;

		bool	m_bIsStoring ;										

		uint32	m_ulAllocated,										
				m_ulLength ;										

		uint8	*m_pbBaseMemory,									
				*m_pbInsert ;										

		void DoubleUp() ;											
		void DoubleExpand(uint32 ulAmount) ;						

	public:
		void SetSize(uint32 ulSize) ;								
		void SetStore(void) { m_bIsStoring = true ; }				
		void SetLoad(void) { m_bIsStoring = false ; }				
		void ResetForLoad(void);
		uint8 *GetStream(void) { return (m_pbBaseMemory) ; }		
		uint32 StreamLen(void) { return (m_ulLength) ; }			

		
		friend class GameFile ;
		friend class GameMapFile ; 
		friend class DataCheck ;
		friend class BuildQueue ;									
		friend class NetCRC;  
		friend class MapFile;
	public:
		CivArchive() ;												
		CivArchive(uint32 ulSize) ;									
		~CivArchive() ;												


        	
	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

		
		 STDMETHODIMP_ (void) Store(uint8 *pbData, uint32 ulLen) ;					
#ifndef HUNT_SERIALIZE
		void StoreChunk(uint8 *start, uint8 *end);
#endif

        void TypeCheck(const uint8 tc);

		CivArchive &operator<< (const double &val) {
			PutDOUBLE(val); return (*this);
		}
		CivArchive &operator<< (const sint8 &val) {
			PutSINT8(val); return (*this);
		}
		CivArchive &operator<< (const uint8 &val) {
			PutUINT8(val); return (*this);
		}
		CivArchive &operator<< (const sint16 &val) {
			PutSINT16(val); return (*this);
		}
		CivArchive &operator<< (const uint16 &val) {
			PutUINT16(val); return (*this);
		}
		CivArchive &operator<< (const sint32 &val) {
			PutSINT32(val); return (*this);
		}
		CivArchive &operator<< (const uint32 &val) {
			PutUINT32(val); return (*this);
		}
		CivArchive &operator<< (const sint64 &val) {
			PutUINT64(val); return (*this);
		}
		CivArchive &operator<< (const uint64 &val) {
			PutUINT64(val); return (*this);
		}
		void PutDOUBLE(const double &val) {
			
			double temp = val; 
			Store((uint8 *)&temp, sizeof(temp));
		}
		void PutDoubleString(const double &val);

		void PutSINT8(const sint8 &val) {
			Store((uint8 *)&val, sizeof(sint8));
		}
		void PutUINT8(const uint8 &val) {
			Store((uint8 *)&val, sizeof(uint8));
		}
		void PutSINT16(const sint16 &val) {
			sint16 temp = SDL_SwapLE16(val);
			Store((uint8 *)&temp, sizeof(temp));
		}
		void PutUINT16(const uint16 &val) {
			uint16 temp = SDL_SwapLE16(val);
			Store((uint8 *)&temp, sizeof(temp));
		}
		void PutSINT32(const sint32 &val) {
			sint32 temp = SDL_SwapLE32(val);
			Store((uint8 *)&temp, sizeof(temp));
		}
		void PutUINT32(const uint32 &val) {
			uint32 temp = SDL_SwapLE32(val);
			Store((uint8 *)&temp, sizeof(temp));
		}
		void PutSINT64(const sint64 &val) {
			sint64 temp = SDL_SwapLE64(val);
			Store((uint8 *)&temp, sizeof(temp));
		}
		void PutUINT64(const uint64 &val) {
			uint64 temp = SDL_SwapLE64(val);
			Store((uint8 *)&temp, sizeof(temp));
		}

		
		 STDMETHODIMP_ (void) Load(uint8 *pbData, uint32 ulLen) ;					
#ifndef HUNT_SERIALIZE
		  void LoadChunk(uint8 *start, uint8 *end);
#endif
		CivArchive &operator>> (double &val) {
			val = GetDOUBLE(); return (*this);
		}
		CivArchive &operator>> (sint8 &val) {
			val = GetSINT8(); return (*this);
		}
		CivArchive &operator>> (uint8 &val) {
			val = GetUINT8(); return (*this);
		}
		CivArchive &operator>> (sint16 &val) {
			val = GetSINT16(); return (*this);
		}
		CivArchive &operator>> (uint16 &val) {
			val = GetUINT16(); return (*this);
		}
		CivArchive &operator>> (sint32 &val) {
			val = GetSINT32(); return (*this);
		}
		CivArchive &operator>> (uint32 &val) {
			val = GetUINT32(); return (*this);
		}
		CivArchive &operator>> (sint64 &val) {
			val = GetUINT64(); return (*this);
		}
		CivArchive &operator>> (uint64 &val) {
			val = GetUINT64(); return (*this);
		}
		double GetDOUBLE(void) {
			double val;
			Load((uint8 *)&val, sizeof(val));
			
			return val; 
		}
		sint8 GetSINT8(void) {
			sint8 val;
			Load((uint8 *)&val, sizeof(val));
			return val;
		}
		uint8 GetUINT8(void) {
			uint8 val;
			Load((uint8 *)&val, sizeof(val));
			return val;
		}
		sint16 GetSINT16(void) {
			sint16 val;
			Load((uint8 *)&val, sizeof(val));
			return SDL_SwapLE16(val);
		}
		uint16 GetUINT16(void) {
			uint16 val;
			Load((uint8 *)&val, sizeof(val));
			return SDL_SwapLE16(val);
		}
		sint32 GetSINT32(void) {
			sint32 val;
			Load((uint8 *)&val, sizeof(val));
			return SDL_SwapLE32(val);
		}
		uint32 GetUINT32(void) {
			uint32 val;
			Load((uint8 *)&val, sizeof(val));
			return SDL_SwapLE32(val);
		}
		sint64 GetSINT64(void) {
			sint64 val;
			Load((uint8 *)&val, sizeof(val));
			return SDL_SwapLE64(val);
		}
		uint64 GetUINT64(void) {
			uint64 val;
			Load((uint8 *)&val, sizeof(val));
			return SDL_SwapLE64(val);
		}
	
		void PerformMagic(uint32 id) ;
		void TestMagic(uint32 id) ;
		
		 STDMETHODIMP_ (BOOL) IsStoring(void) { return (m_bIsStoring) ; }			

		
		void StoreArray( sint8 * dataarray, size_t size ) {
			Store((uint8 *)dataarray, size);
		}
		void StoreArray( uint8 * dataarray, size_t size ) {
			Store((uint8 *)dataarray, size);
		}
		void StoreArray( sint16 * dataarray, size_t size ) {
			for ( int i=0; i<(int)size; ++i )
				PutSINT16(dataarray[i]);
		}
		void StoreArray( uint16 * dataarray, size_t size ) {
			for ( int i=0; i<(int)size; ++i )
				PutUINT16(dataarray[i]);
		}
		void StoreArray( sint32 * dataarray, size_t size ) {
			for ( int i=0; i<(int)size; ++i )
				PutSINT32(dataarray[i]);
		}
		void StoreArray( uint32 * dataarray, size_t size ) {
			for ( int i=0; i<(int)size; ++i )
				PutUINT32(dataarray[i]);
		}
		void StoreArray( sint64 * dataarray, size_t size ) {
			for ( int i=0; i<(int)size; ++i )
				PutSINT64(dataarray[i]);
		}
		void StoreArray( uint64 * dataarray, size_t size ) {
			for ( int i=0; i<(int)size; ++i )
				PutUINT64(dataarray[i]);
		}
		void StoreArray( double * dataarray, size_t size ) {
			for ( int i=0; i<(int)size; ++i )
				PutDOUBLE(dataarray[i]);
		}
		void StoreArrayString( double * dataarray, size_t size ) {
			for ( int i=0; i<(int)size; ++i )
				PutDoubleString(dataarray[i]);
		}
		void LoadArray( sint8 * dataarray, size_t size ) {
			Load( (uint8 *)dataarray, size );
		}
		void LoadArray( uint8 * dataarray, size_t size ) {
			Load( (uint8 *)dataarray, size );
		}
		void LoadArray( sint16 * dataarray, size_t size ) {
			for ( int i=0; i<(int)size; ++i )
				dataarray[i] = GetSINT16();
		}
		void LoadArray( uint16 * dataarray, size_t size ) {
			for ( int i=0; i<(int)size; ++i )
				dataarray[i] = GetUINT16();
		}
		void LoadArray( sint32 * dataarray, size_t size ) {
			for ( int i=0; i<(int)size; ++i )
				dataarray[i] = GetSINT32();
		}
		void LoadArray( uint32 * dataarray, size_t size ) {
			for ( int i=0; i<(int)size; ++i )
				dataarray[i] = GetUINT32();
		}
		void LoadArray( sint64 * dataarray, size_t size ) {
			for ( int i=0; i<(int)size; ++i )
				dataarray[i] = GetSINT64();
		}
		void LoadArray( uint64 * dataarray, size_t size ) {
			for ( int i=0; i<(int)size; ++i )
				dataarray[i] = GetUINT64();
		}
		void LoadArray( double * dataarray, size_t size ) {
			for ( int i=0; i<(int)size; ++i )
				dataarray[i] = GetDOUBLE();
		}
	} ;



#ifdef _DEBUG

#ifdef _USE_CHECKSERIALIZE
#define CHECKSERIALIZE \
{\
    char archive_filename_here[100]= {0}; \
    char archive_filename_tmp[100]= {0}; \
    sint32 archive_filename_len=0; \
	char *partialname = strrchr(__FILE__, '\\') + 1; \
	if(!partialname) \
		partialname = __FILE__; \
    sprintf(archive_filename_here, "%s%d", partialname, __LINE__);\
    archive_filename_len = strlen(archive_filename_here); \
    if (archive.IsStoring()) { \
        archive.Store((uint8*)archive_filename_here, archive_filename_len); \
    } else {\
        archive.Load((uint8*)archive_filename_tmp, archive_filename_len); \
        Assert(strncmp(archive_filename_tmp, archive_filename_here, archive_filename_len) == 0);\
    }\
}
#else
#define CHECKSERIALIZE
#endif

#else

#define CHECKSERIALIZE

#endif

#endif
