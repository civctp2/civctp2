//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Stub for missing Miles sound system header
//
//----------------------------------------------------------------------------

#ifndef StubsMssH
#define StubsMssH

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

#include <windows.h>    // HANDLE

//----------------------------------------------------------------------------
// Exported names
//----------------------------------------------------------------------------

typedef HANDLE	        HAUDIO;
typedef HANDLE          HDIGDRIVER;
typedef HANDLE          HDLSDEVICE;
typedef HANDLE          HMDIDRIVER;
typedef HANDLE	        HREDBOOK;
typedef signed int      S32;
typedef unsigned int    U32;

enum    AudioStatus
{
    QSTAT_ERROR,
    QSTAT_DONE
}; // S32

enum    DigitalStatus
{
    DSTAT_ERROR,
    DSTAT_OK
}; // unknown, probably S32

enum    DigitalUsage
{
    AIL_QUICK_USE_WAVEOUT,	             // in Debug
    DONT_KNOW_WHAT_TO_CALL_THIS          // 1, in Release and Final
}; // S32

enum	RedbookStatus
{
    REDBOOK_ERROR,
    REDBOOK_PLAYING,
    REDBOOK_PAUSED,
    REDBOOK_STOPPED
}; // U32


//----------------------------------------------------------------------------
// Stubbed functions
//----------------------------------------------------------------------------

extern "C" {

__declspec(dllimport) S32         WINAPI AIL_digital_handle_reacquire
(
    HDIGDRIVER const    a_Handle
);

__declspec(dllimport) S32         WINAPI AIL_digital_handle_release
(
    HDIGDRIVER const    a_Handle
);

__declspec(dllimport) S32         WINAPI AIL_digital_master_volume
(
    HDIGDRIVER const    a_Handle
);

__declspec(dllimport) void        WINAPI AIL_set_digital_master_volume
(
    HDIGDRIVER const    a_Handle,
    S32 const           a_Volume
);

__declspec(dllimport) void        WINAPI AIL_quick_handles
(
    HDIGDRIVER * const	a_pHandleDig,
    HMDIDRIVER * const  a_pHandleMdi,
    HDLSDEVICE * const  a_pHandleDls
);
	
__declspec(dllimport) HAUDIO      WINAPI AIL_quick_load_mem
(
    void * const        a_Dataptr, 
    S32 const        a_Datasize
);

__declspec(dllimport) void        WINAPI AIL_quick_halt
(
    HAUDIO const        a_Handle
);

__declspec(dllimport) void        WINAPI AIL_quick_play
(
    HAUDIO const        a_Handle,
    S32 const           a_Unknown            // 1
);

__declspec(dllimport) void        WINAPI AIL_quick_shutdown();

__declspec(dllimport) void        WINAPI AIL_quick_set_volume
(
    HAUDIO const        a_Handle,
    S32 const           a_Volume,
    S32 const           a_PanValue
);

__declspec(dllimport) S32         WINAPI AIL_quick_startup
(
     S32 const          a_UseDigital, 
     S32 const          a_UseMidi, 
     S32 const          a_OutputRate, 
     S32 const          a_OutputBits,
     S32 const          a_OutputChannels
);

__declspec(dllimport) S32         WINAPI AIL_quick_status
(
     HAUDIO const       a_Handle
);

__declspec(dllimport) void        WINAPI AIL_quick_unload
(
    HAUDIO const        a_Handle
);

__declspec(dllimport) void        WINAPI AIL_redbook_close
(
    HREDBOOK const      a_Handle
);

__declspec(dllimport) HREDBOOK    WINAPI AIL_redbook_open_drive
(
    MBCHAR const        a_Drive
);

__declspec(dllimport) void        WINAPI AIL_redbook_play
(
    HREDBOOK const      a_Handle,
    U32 const           a_StartTrack,
    U32 const           a_EndTrack
);

__declspec(dllimport) void        WINAPI AIL_redbook_set_volume
(
    HREDBOOK const      a_Handle,
    S32 const           a_Volume
);

__declspec(dllimport) U32         WINAPI AIL_redbook_status
(
    HREDBOOK const      a_Handle
);

__declspec(dllimport) void        WINAPI AIL_redbook_stop
(
    HREDBOOK const      a_Handle
);

__declspec(dllimport) bool        WINAPI AIL_redbook_track
(
    HREDBOOK const      a_Handle
);

__declspec(dllimport) void        WINAPI AIL_redbook_track_info
(
    HREDBOOK const      a_Handle,
    S32 const           a_TrackNum, 
    U32 * const         a_pStartTrack, 
    U32 * const         a_pEndTrack
);

__declspec(dllimport) S32         WINAPI AIL_redbook_tracks
(
    HREDBOOK const      a_Handle
);

}

#endif // Multiple include guard