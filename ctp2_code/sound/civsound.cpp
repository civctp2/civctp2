

#include "c3.h"

#include "CivSound.h"
#include "SoundRecord.h"
#include "prjfile.h"

extern ProjectFile  *g_SoundPF;

CivSound::CivSound(uint32 associatedObject, sint32 soundID)
{
    const char *fname;
	if(soundID < 0)
		fname = NULL;
	else
		fname = g_theSoundDB->Get(soundID)->GetValue();

    
	m_associatedObject = associatedObject;
	m_soundID = soundID;
    m_isPlaying = FALSE;
    m_isLooping = FALSE;
    m_hAudio = NULL;

    
    if (fname == NULL) {
        m_soundFilename[0] = 0;
        m_dataptr = NULL;
        m_datasize = 0;
        return;
    }

    strcpy(m_soundFilename, fname);

    
    m_dataptr = g_SoundPF->getData(m_soundFilename, &m_datasize);

    
    m_hAudio = AIL_quick_load_mem(m_dataptr, m_datasize);
}

CivSound::~CivSound()
{
    if (m_hAudio)
        AIL_quick_unload(m_hAudio);
    
    if (m_dataptr)
        g_SoundPF->freeData(m_dataptr);
}

void CivSound::SetVolume(sint32 volume)
{
    if (m_hAudio == NULL)
        return;

	sint32 scaledVolume = (sint32)((double)volume * 12.7);

	AIL_quick_set_volume(m_hAudio, scaledVolume, 64);
	m_volume = volume;
}
