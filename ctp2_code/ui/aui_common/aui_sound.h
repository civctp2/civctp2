#ifndef __AUI_SOUND_H__
#define __AUI_SOUND_H__

#include "aui_base.h"
#include "aui_memmap.h"


class aui_SoundFormat : public aui_FileFormat
{
public:
	aui_SoundFormat() {}
	virtual ~aui_SoundFormat() {};

	virtual AUI_ERRCODE LoadSoundData(
		const MBCHAR *filename,
		uint8 **wavdata,
		size_t *size ) { return AUI_ERRCODE_OK; }
	virtual void ReleaseSoundData();
protected:
	uint8 *m_data;
};


class aui_WavSoundFormat : public aui_SoundFormat
{
public:
	aui_WavSoundFormat() {};
	virtual ~aui_WavSoundFormat() {};

	virtual AUI_ERRCODE LoadSoundData(
		const MBCHAR *filename,
		uint8 **wavdata,
		size_t *size) ;

protected:
	void TrimWavHeader( uint8 **wavdata, size_t *size);
};






class aui_Sound : public aui_Base
{
public:
	aui_Sound(
		AUI_ERRCODE *retval,
		const MBCHAR *filename = NULL);
	virtual ~aui_Sound();

	virtual AUI_ERRCODE Load(
		char *fileName,
		float x,
		float y,
		float z ) { return AUI_ERRCODE_OK; }
	virtual AUI_ERRCODE Load( void );
	virtual AUI_ERRCODE Unload ( void );
	virtual void Play( void ) {}
	virtual void Stop( void ) {}

	const MBCHAR *GetFilename(void) const { return m_filename; }
	virtual AUI_ERRCODE SetFilename(const MBCHAR  *filename);

protected:
	aui_Sound() : aui_Base() {}
	AUI_ERRCODE InitCommon (const MBCHAR *filename);

protected:
	MBCHAR m_filename[ MAX_PATH + 1 ];
	aui_SoundFormat *m_format;
	uint8 *m_data;
	size_t m_size;
};






class aui_Sound2D : public aui_Sound
{
public:
	aui_Sound2D(
		AUI_ERRCODE *retval,
		const MBCHAR *filename = NULL )
		:
	aui_Sound( retval, filename ) {}
	virtual ~aui_Sound2D() {}

	virtual void Update( void ) {}

protected:
	void SetSoundProperties( void ) {}
};

#endif
