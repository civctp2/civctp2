


























#ifndef _WINDOWS_
	#error EAR only runs under Windows!!!
#endif


#ifndef _EAR_IAS
#define _EAR_IAS

#ifndef _MAX_PATH
#include <stdlib.h>
#endif


#pragma pack( push, before_include1 )


#pragma pack(1)

#if 1




	
	#ifdef _DEBUG
		#define EARIAS_DLL_FILENAME "dll\\sound\\earias.dll"
		#define EARPDS_DLL_FILENAME "dll\\sound\\earpds.dll"
	#else
		#define EARIAS_DLL_FILENAME "dll\\sound\\earias.dll"
		#define EARPDS_DLL_FILENAME "dll\\sound\\earpds.dll"
	#endif
#else
	
	#ifdef _DEBUG
		#define EARIAS_DLL_FILENAME "earias_d.dll"
		#define EARPDS_DLL_FILENAME "earpds_d.dll"
	#else
		#define EARIAS_DLL_FILENAME "earias.dll"
		#define EARPDS_DLL_FILENAME "earpds.dll"
	#endif
#endif

#define EAR_H_VERSION 1.5












#define EAR_VALIDATION_NUMBER 10213771



















































#define EAR_INIT_USEDEFAULTS	0x0000
#define EAR_INIT_QUERYUSER		0x0001
#define EAR_INIT_MUSTBEEAR		0x0002
#define EAR_INIT_SETUPMINIMUM 	0x0004
#define EAR_INIT_CONFIGURE		0x0008
#define EAR_INIT_DEBUGTERSE		0x0000
#define EAR_INIT_DEBUGNORMAL	0x0010
#define EAR_INIT_DEBUGVERBOSE	0x0020
#define EAR_INIT_SHOWERRORS		0x0040
#define EAR_INIT_NOTIMER		0x0080
#define EAR_INIT_CHECKVERSION	0x0100
#define EAR_INIT_RAMPOFF		0x0200
#define EAR_INIT_DEBUGHELP		0x0400






#define EAR_ALL_CHANNELS    0 

#define EAR_CHAN_STATIC			 0x0001
#define EAR_CHAN_DISCRETE		 0x0000 

#define EAR_CHAN_QUEUED			 0x0002
#define EAR_CHAN_DYNAMIC		 0x0000 

#define EAR_CHAN_SOUNDTRACK		 0x000E
#define EAR_CHAN_SOUNDTRACK_CD	 0x0006
#define EAR_CHAN_SOUNDTRACK_MIDI 0x000A

#define EAR_CHAN_LOWPRIORITY	 0x0010
#define EAR_CHAN_MEDPRIORITY	 0x0030	
#define EAR_CHAN_HIPRIORITY		 0x0070	
#define EAR_CHAN_NEVERIGNORE	 0x0000 

#define EAR_CHAN_FADEROFF		 0x0000
#define EAR_CHAN_FADERFAST		 0x0100
#define EAR_CHAN_FADERMED		 0x0300
#define EAR_CHAN_FADERSLOW		 0x0700








#define EAR_CTRL_GETCONTROLS		0x00000000
#define EAR_CTRL_MUTE				0x00000001
#define EAR_CTRL_UNMUTE				0x00000003	
#define EAR_CTRL_PAUSE				0x00000004
#define EAR_CTRL_UNPAUSE			0x0000000C	
#define EAR_CTRL_MAKELOWPRIORITY	0x00000010
#define EAR_CTRL_MAKEMEDPRIORITY	0x00000030	
#define EAR_CTRL_MAKEHIPRIORITY		0x00000070	
#define EAR_CTRL_MAKENEVERIGNORE	0x000000F0	
#define EAR_CTRL_MAKESTATIC			0x00000100
#define EAR_CTRL_MAKEDISCRETE		0x00000300	
#define EAR_CTRL_MAKEQUEUED			0x00000400
#define EAR_CTRL_MAKEDYNAMIC		0x00000C00	
#define EAR_CTRL_MAKEFADERFAST		0x00001000
#define EAR_CTRL_MAKEFADERMED		0x00003000
#define EAR_CTRL_MAKEFADERSLOW		0x00007000
#define EAR_CTRL_MAKEFADEROFF		0x0000F000

#define EAR_CTRL_CD					0x10000000
#define EAR_CTRL_CD_EJECTMEDIA		0x10000001
#define EAR_CTRL_CD_CLOSEMEDIA		0x10000002
#define EAR_CTRL_CD_VERIFYMEDIA		0x10000004
#define EAR_CTRL_CD_REFRESHMEDIA	0x10000008
#define EAR_CTRL_CD_GETNUMTRACKS	0x10000010
#define EAR_CTRL_CD_GETFIRSTAUDIO	0x10000020
#define EAR_CTRL_CD_GETLASTAUDIO	0x10000040

#define EAR_CTRL_ENG				0x20000000
#define EAR_CTRL_ENG_GETRAMP		0x20010000
#define EAR_CTRL_ENG_RAMPOFF		0x20020000
#define EAR_CTRL_ENG_RAMPON			0x20040000

#define EAR_CTRL_CHAN				0x40000000
#define EAR_CTRL_CHAN_NUMEVENTS		0x40010000
#define EAR_CTRL_CHAN_ATTENUATION	0x40020000
#define EAR_CTRL_CHAN_NUMACTIVE		0x40040000

#define EAR_CTRL_EVENT				0x80000000
#define EAR_CTRL_EVENT_GETCUE		0x80010000
#define EAR_CTRL_EVENT_GETSTATUS	0x80020000
#define EAR_CTRL_EVENT_GETLENGTH	0x80040000




enum EAR_ERROR_TABLE
 {	




	EAR_ERR_NOERROR,
	EAR_ERR_EARNOTVALIDATED,
	EAR_ERR_SOUNDCARDINUSE,
	EAR_ERR_NOSOUNDCARDDLL,
	EAR_ERR_EARDLLINUSE,
	EAR_ERR_FAULTYTABLE,
	EAR_ERR_HARDWAREPROBLEM,
	EAR_ERR_GETCONFIGFAILED,
	EAR_ERR_SETCONFIGFAILED,
	EAR_ERR_USERCANCELLED,
	EAR_ERR_USERWANTSSTEREO,
	EAR_ERR_NOFRONTSPEAKERS,
	EAR_ERR_NOREARSPEAKERS,
	EAR_ERR_FILEOPENFAILED,
	EAR_ERR_FILEOPSFAILED,
	EAR_ERR_NOTAWAVEFILE,
	EAR_ERR_OUTOFMEMORY,
	EAR_ERR_CHANNELFAILED,
	EAR_ERR_PAGELOCKFAILED,
	EAR_ERR_BUFFERPTRFAILED,
	EAR_ERR_STARTFAILED,
	EAR_ERR_NULLMATRIX,
	EAR_ERR_DUPLICATEMATRIX,
	EAR_ERR_UNSUPPORTED,
	EAR_ERR_BADTIMER,
	EAR_ERR_FATALCALLBACK,
	EAR_ERR_CALLBACKERROR,
	EAR_ERR_INVALIDCHANNEL,
	EAR_ERR_INVALIDEVENT,
	EAR_ERR_BADDATAFORMAT,
	EAR_ERR_DUPLICATEENV,
	EAR_ERR_INVALIDENV,
	EAR_ERR_ONLYTWOSPEAKERS,
	EAR_ERR_INVALIDHWND,
	EAR_ERR_DSOUNDFAILED,
	EAR_ERR_MUSTBEUNIQUE,
	EAR_ERR_OUTOFRANGE,
	EAR_ERR_PAUSEFAILED,
	EAR_ERR_CHANNELFULL,
	EAR_ERR_INVALIDMATRIX,
	EAR_ERR_DUPLICATEPRESET,
	EAR_ERR_INVALIDPRESET,
	EAR_ERR_INVALIDRESOURCE,
	EAR_ERR_NOCDDEVICE,
	EAR_ERR_NOMIDIDEVICE,
	EAR_ERR_LASTERROR
 };












#define EAR_SOS_THRU_AIR	   33	
#define EAR_SOS_THRU_VAPOR	 1545	
#define EAR_SOS_THRU_WATER	 1785	
#define EAR_SOS_THRU_SILVER	 2841	
#define EAR_SOS_THRU_GRANITE 4252	
#define EAR_SOS_THRU_IRON	 5354	
#define EAR_SOS_THRU_GLASS	 5905	










#define EAR_REV_OPENAIR			0	
#define EAR_REV_PADDEDCELL		1	
#define EAR_REV_ROOM			2	
#define EAR_REV_CUBICLE			3	
#define EAR_REV_TUNNEL			4	
#define EAR_REV_HALFPIPE		5	
#define EAR_REV_HALL			6	
#define EAR_REV_STADIUM			7	
#define EAR_REV_CAVERN			8	
#define EAR_REV_ARENA			9	

#define EAR_REVSZ_DEFAULT		0	
#define EAR_REVSZ_SMALL			1	
#define EAR_REVSZ_MEDIUM	   10   
#define EAR_REVSZ_LARGE		   20	
#define EAR_REVSZ_HUGE		   30	
#define EAR_REVSZ_UNENDING	  100	





#define	EAR_ENVD_SEALEVEL		0	
#define EAR_ENVD_ALTDISP	16000	
#define EAR_ENVD_MEANTEMP		0	
#define EAR_ENVD_SPOFSOUND	   EAR_SOS_THRU_AIR
#define	EAR_ENVD_TIMEOUT	  500	
#define EAR_ENVD_DIFFUSION   1500	
#define EAR_ENVD_FREQUENCY	22050	
#define EAR_ENVD_BITRATE	   16	
#define EAR_ENVD_MEDIUM		  air	
#define EAR_ENVD_AMBMASK	 6000	
#define EAR_ENVD_REVERB		   EAR_REV_OPENAIR
#define EAR_ENVD_ROOMSIZE	   EAR_REVSZ_DEFAULT
#define EAR_ENVD_SWEETSPOT		0

#define EAR_ENV_DEFAULT	0x00000001
#define EAR_ENV_NEW		0xFFFFFFFF

#define EAR_ALL_ENVIRONMENTS    0














typedef struct
 {	WORD	size;
	WORD	altitude_dispersion;
	WORD	speed_of_sound;
	LONG	sea_level;
	LONG	mean_temperature;
	LONG	timeout_default;
	WORD	frequency_default;
	WORD	bitrate_default;
	WORD	diffusion;
	WORD	ambient_mask;
	WORD	reverb_type;
	WORD	reverb_size;
	WORD	sweetspot;
	BYTE	reserved_for_future[28];
	DWORD	number;

#ifdef __cplusplus
	__inline VOID Default(VOID)
	 {	size				= sizeof(EAR_ENVIRONMENT);	
		sea_level			= EAR_ENVD_SEALEVEL;
		altitude_dispersion	= EAR_ENVD_ALTDISP;
		mean_temperature	= EAR_ENVD_MEANTEMP;
		speed_of_sound		= EAR_ENVD_SPOFSOUND;
		timeout_default		= EAR_ENVD_TIMEOUT;
		frequency_default	= EAR_ENVD_FREQUENCY;
		bitrate_default		= EAR_ENVD_BITRATE;
		diffusion			= EAR_ENVD_DIFFUSION;
		ambient_mask		= EAR_ENVD_AMBMASK;
		reverb_type			= EAR_ENVD_REVERB;
		reverb_size			= EAR_ENVD_ROOMSIZE;
		sweetspot			= EAR_ENVD_SWEETSPOT;

		number				= EAR_ENV_DEFAULT;

		memset(&reserved_for_future, 0, 28);

	 } 

#endif

 }	EAR_ENVIRONMENT;


#define EAR_EVENT_SHUTDOWN_IMMEDIATE	0x00000000
#define EAR_EVENT_SHUTDOWN_PLAYOUT		0x00000001
#define EAR_EVENT_SHUTDOWN_FADEFAST		0x00000002
#define EAR_EVENT_SHUTDOWN_FADEMEDIUM	0x00000004
#define EAR_EVENT_SHUTDOWN_FADESLOW		0x00000008
#define EAR_EVENT_SHUTDOWN_BYOBJECT		0x00000000
#define EAR_EVENT_SHUTDOWN_BYCUE		0x00010000

#define EAR_CHAN_SHUTDOWN_ALLOWFADE		0x00000000
#define EAR_CHAN_SHUTDOWN_FORCESHUT		0x00010000














#define EAR_MAT_KING		0x0001
#define EAR_TIER_KING		0x0002
#define EAR_MAT_GRID		0x0004
#define EAR_TIER_GRID		0x0008
#define EAR_MAT_CIRCLE		0x0010
#define EAR_MAT_FLAT3D		0x0020
#define EAR_MAT_SLOPED3D	0x0040
#define EAR_TIER_FLAT3D		0x0080
#define EAR_MAT_FULLSTEREO	0x0100
#define EAR_MAT_MIDI		0x0200

#define EAR_MAT_USER		0x0400

#define EAR_ALL_MATRICES	  0


#define EAR_MAX_MATRIX_POINTS	128
#define EAR_ALL_MATRIX_POINTS	 -1














#define EAR_KING_CENTER			0
#define EAR_KING_LEFT_REAR		1
#define EAR_KING_LEFT_CENTER	2
#define EAR_KING_LEFT_FRONT		3
#define EAR_KING_FRONT_CENTER	4
#define EAR_KING_RIGHT_FRONT	5
#define EAR_KING_RIGHT_CENTER	6
#define EAR_KING_RIGHT_REAR		7
#define EAR_KING_REAR_CENTER	8


#define EAR_KING_FRONT_FACE		9	
#define EAR_KING_REAR_FACE	   10	
#define EAR_KING_LEFT_FACE	   11	
#define EAR_KING_RIGHT_FACE	   12	
#define EAR_KING_LEFT_BISECT   13	
#define EAR_KING_RIGHT_BISECT  14	
#define EAR_KING_FOUR_CORNERS  15	
#define EAR_KING_FOUR_CENTERS  16	
#define EAR_KING_FOUR_LESS_LF  17	
#define EAR_KING_FOUR_LESS_RF  18   
#define EAR_KING_FOUR_LESS_RR  19   
#define EAR_KING_FOUR_LESS_LR  20   
















#define	EAR_CIRCLE_REAR_CENTER	 0
#define EAR_CIRCLE_LEFT_REAR	16
#define EAR_CIRCLE_LEFT_CENTER	32
#define EAR_CIRCLE_LEFT_FRONT	48
#define EAR_CIRCLE_FRONT_CENTER	64
#define EAR_CIRCLE_RIGHT_FRONT	80
#define EAR_CIRCLE_RIGHT_CENTER	96
#define EAR_CIRCLE_RIGHT_REAR  112




#define EAR_MACRO_NULL			0x00000000
#define EAR_MACRO_PROGRESSIVE	0x00000001

typedef struct
 {	WORD azimuth;
	WORD percent;
 } EAR_MACRO_PROGRESSIVE_STAGE;

typedef struct
 {	WORD	size;
	WORD	type;
	WORD	stages;
	WORD	stage_size;
	DWORD	first;
	
#ifdef __cplusplus
	__inline VOID Default(VOID)
	 {	size				= sizeof(EAR_MACRO);	
		type				= EAR_MACRO_NULL;
		stages				= 0;
		stage_size			= NULL;
		first				= NULL;

		





















	 } 

#endif

 }	EAR_MACRO;





#define EAR_ALL_PRESETS			0




#define EAR_ALL_REDBOOK_TRACKS	0







typedef VOID  (__stdcall *VPROCV)(VOID);
typedef DWORD (__stdcall *DPROCV)(VOID);
typedef VOID  (__stdcall *VPROCD)(DWORD);
typedef VOID  (__stdcall *VPROCDD)(DWORD,DWORD);
typedef DWORD (__stdcall *DPROCD)(DWORD);
typedef DWORD (__stdcall *DPROCDD)(DWORD,DWORD);
typedef DWORD (__stdcall *DPROCDDD)(DWORD,DWORD,DWORD);
typedef DWORD (__stdcall *DPROCDDDD)(DWORD,DWORD,DWORD,DWORD);
typedef DWORD (__stdcall *DPROCDDDDD)(DWORD,DWORD,DWORD,DWORD,DWORD);
typedef DWORD (__stdcall *DPROCDDDDDD)(DWORD,DWORD,DWORD,DWORD,DWORD,DWORD);


#define EAR_MONIKER "EAR Interactive Around-Sound"

#define EAR_WAVE_NAME_LENGTH _MAX_PATH







#define EAR_TYPE_WAVE			0x0000  
#define EAR_TYPE_PATCH			0x0001	
#define EAR_TYPE_RESOURCE		0x0002	
#define EAR_TYPE_PRESET			0x0003	
#define EAR_TYPE_RAW			0x0004	
#define EAR_TYPE_MOD			0x0005	
#define EAR_TYPE_MIDI			0x0006	

#define EAR_TYPE_MP1			0x000A	
#define EAR_TYPE_MP2			0x000B	
#define EAR_TYPE_MP3			0x000C	
#define EAR_TYPE_MP4AUDIO		0x000D	
#define EAR_TYPE_MP4MIDI		0x000E	

#define EAR_TYPE_DOLBYPROLOGIC	0x0014	
#define EAR_TYPE_DOLBYSURROUND	0x0015	
#define EAR_TYPE_DOLBYDIGITAL	0x0016	

#define EAR_TYPE_DTS			0x001A	

#define EAR_TYPE_REDBOOK		0x001E	
#define EAR_TYPE_ADPCM			0x001F	


#define EAR_TYPE_AVS			0x0080	


#define EAR_TYPE_IS_08BIT		0x0000 
#define EAR_TYPE_IS_16BIT		0x0100 
#define EAR_TYPE_IS_SAMP11		0x0000 
#define EAR_TYPE_IS_SAMP22		0x0200 
#define EAR_TYPE_IS_SAMP44		0x0600 
#define EAR_TYPE_IS_MONO		0x0000 
#define EAR_TYPE_IS_STEREO		0x0800 
#define EAR_TYPE_IS_QUAD		0x1000 
#define EAR_TYPE_IS_5DOT1		0x1800 
#define EAR_TYPE_IS_FILE		0x0000 
#define EAR_TYPE_IS_RESOURCE	0x2000 
#define EAR_TYPE_IS_HMMIO		0x4000 
#define EAR_TYPE_IS_HANDLE		0x6000 





#define EAR_100	10000 
#define EAR_90	 9000
#define EAR_80	 8000
#define EAR_75   7500 
#define EAR_70	 7000
#define EAR_60	 6000
#define EAR_50	 5000 
#define EAR_40	 4000
#define EAR_30	 3000
#define EAR_25	 2500 
#define EAR_20	 2000
#define EAR_10	 1000 
#define EAR_05	  500
#define EAR_01	  100 
#define EAR_00		0





#define EAR_PERF_DEFAULT		0x00000000
#define EAR_PERF_DISTANCE		0x00000001
#define EAR_PERF_VOLUME			0x00000002
#define EAR_PERF_DELAY			0x00000004
#define EAR_PERF_TIMEOUT		0x00000008
#define EAR_PERF_WAIT			0x00000080
#define EAR_PERF_UPDATE			0x00000010
#define EAR_PERF_LOOP			0x00000020
#define EAR_PERF_BUFFER			0x00000040
#define EAR_PERF_CALLBACK		0x00000100
#define EAR_PERF_DOPPLER		0x00000200
#define EAR_PERF_AMBIANCE		0x00000400
#define EAR_PERF_TRACK			0x00000800
#define EAR_PERF_SCROLL			0x00001000
#define EAR_PERF_TRIGGER		0x00002000
#define EAR_PERF_MERGE			0x00004000
#define EAR_PERF_MACRO			0x00008000
#define EAR_PERF_PRIORITIZE		0x00010000
#define EAR_PERF_PAUSE			0x00020000
#define EAR_PERF_MUTE			0x00040000
#define EAR_PERF_EXPIRE			0x00080000
#define EAR_PERF_MODULATE		0x00100000
#define EAR_PERF_TRANSPOSE		0x00200000
#define EAR_PERF_SYNCH			0x00400000
#define EAR_PERF_DUMB			0x00800000
#define EAR_PERF_ADVANCE		0x01000000
#define EAR_PERF_DIFFUSE		0x02000000






#define EAR_PERF_REPEAT		EAR_PERF_LOOP
#define EAR_PERF_SHUFFLE	EAR_PERF_BUFFER




#define EAR_PERF_DISCRETE_ONESHOT	(EAR_PERF_DISTANCE \
									| EAR_PERF_VOLUME \
									| EAR_PERF_DELAY \
									| EAR_PERF_TIMEOUT \
									| EAR_PERF_UPDATE \
									| EAR_PERF_DOPPLER)

#define EAR_PERF_DISCRETE_LOOP		(EAR_PERF_DISCRETE_ONESHOT \
									| EAR_PERF_LOOP)





#define EAR_STAT_MUTED		0x0001
#define EAR_STAT_PAUSED		0x0002
#define EAR_STAT_STARTED	0x0004
#define EAR_STAT_QUEUED		0x0008
#define EAR_STAT_VARIED		0x0010
#define EAR_STAT_STREAMED	0x0020
#define EAR_STAT_WAITING	0x0040





#define EAR_PRI_ALWAYS		   0 
#define EAR_PRI_LOW			   1
#define EAR_PRI_MED			  50
#define EAR_PRI_HI			 100




#define EAR_CALL_NORMAL	  0x0000
#define EAR_CALL_SYNCH	  0x0001
#define EAR_CALL_BACK	  0x0002





#define EAR_QUEUE_TAIL		0 
#define EAR_QUEUE_HEAD		1
#define EAR_QUEUE_FORCE		2


typedef enum 
 {	



	user_defined_1,
	user_defined_2,
	user_defined_3,
	user_defined_4,
	centimeter,
	meter,
	kilometer,
	inch,
	foot,
	yard,
	mile
 } EAR_DU;

typedef WORD EAR_RINGS;

typedef struct 
 {	LONG x;
	LONG y;
	LONG z;
 } EAR_XYZ;


typedef struct 
 {	WORD	size;
	EAR_XYZ	xyz;	
	EAR_XYZ	rpy;	
#ifdef __cplusplus
	__inline VOID Default(VOID)
	 {	size = sizeof(EAR_USER);
		memset(&xyz, 0, sizeof(EAR_XYZ) * 2);
	 } 
#endif
 }EAR_USER;


typedef struct 
 {	WORD		size;					
	WORD		type;					
	DWORD		location;				
	DWORD		cue;					
	WORD		channel;				
	WORD		matrix;					
	WORD		minimum_volume;			
	WORD		maximum_volume;			
	DWORD		outer_range;			
	DWORD		inner_range;			
	DWORD		performance_flags;		
	DWORD		macros;					
	DWORD		static_data;			
	WORD		static_azimuth;			
	WORD		static_delay;			
	LONG		timeout_after;			
	LONG		expire_after;			
	DWORD		insert;					
	DWORD		environment;			
	WORD		priority;				
	WORD		modulation;				
	WORD		transposition;			
	WORD		advancement;			
	WORD		trigger_range;			
	WORD		merge_flags;			
	DWORD		merge_with;				
	BYTE		reserved_for_future[20];
	EAR_DU		distance_unit;			
	EAR_XYZ		cartesian;				
	EAR_RINGS	rings[8];				
#ifdef __cplusplus
	__inline VOID Default(VOID)
	 {	size				= sizeof(EAR_OBJECT);
		type				= EAR_TYPE_WAVE;
		location			= 0;
		cue					= 0;
		channel				= 1;
		matrix				= EAR_MAT_CIRCLE;
		minimum_volume		= 0;
		maximum_volume		= EAR_100;	
		outer_range			= 10000;	
		inner_range			= 0;
		performance_flags	= EAR_PERF_DEFAULT;
		macros				= 0;
		static_data			= 0;
		static_azimuth		= EAR_CIRCLE_FRONT_CENTER;
		static_delay		= 0;
		timeout_after		= 0;
		expire_after		= 0;
		modulation			= EAR_100; 
		transposition		= EAR_100; 
		advancement			= 0;
		priority			= EAR_PRI_ALWAYS;
		insert				= EAR_QUEUE_TAIL;
		environment			= EAR_ENV_DEFAULT;
		distance_unit		= centimeter;

		memset(&cartesian, 0, sizeof(EAR_XYZ));

		rings[0] = EAR_00;
		rings[1] = EAR_10;
		rings[2] = EAR_25;
		rings[3] = EAR_50;
		rings[4] = EAR_75;
		rings[5] = EAR_90;
		rings[6] = EAR_100;

		memset(&reserved_for_future, 0, 20);

	 } 
#endif
 }  EAR_OBJECT;


typedef struct 
 {	WORD	size;
	DWORD	patch_id;
	DWORD	cue;
	EAR_XYZ cartesian;
#ifdef __cplusplus
	__inline VOID Default(VOID)
	 {	size		= sizeof(EAR_PATCH);
		patch_id	= 0;
		cue			= 0;

		memset(&cartesian, 0, sizeof(EAR_XYZ));

	 } 
#endif
 } EAR_PATCH;

























#ifndef EAR_SERVICE_TABLE
#define EAR_SERVICE_TABLE

extern HMODULE EAR_DLL;

extern DPROCD		EAR_AAA_Validate;
extern DPROCD		EAR_AssignHwnd;
extern DPROCDD		EAR_ChangeChannelControl;
extern DPROCDD		EAR_ClearChannel;
extern DPROCV		EAR_EarInactive;
extern DPROCDD		EAR_GetEventStatus;
extern DPROCV		EAR_GetLastError;
extern DPROCV		EAR_GetVersion;
extern DPROCD		EAR_InitializeEar;
extern DPROCD		EAR_MixEvent;
extern DPROCD		EAR_MoveEvent;
extern DPROCDD		EAR_RegisterBank;
extern DPROCDDDD	EAR_RegisterChannel;
extern DPROCD		EAR_RegisterEnvironment;
extern DPROCD		EAR_RegisterMatrix;
extern DPROCDD		EAR_RegisterPreset;
extern VPROCV		EAR_ResetEar;
extern VPROCDD		EAR_SetAttenuationLevel;
extern VPROCDD		EAR_SetUserDistanceUnit;
extern VPROCV		EAR_ShowLastError;
extern DPROCD		EAR_ShutDownBank;
extern DPROCD		EAR_ShutDownChannel;
extern VPROCV		EAR_ShutDownEar;
extern DPROCD		EAR_ShutDownEnvironment;
extern DPROCDD		EAR_ShutDownEvent;
extern DPROCD		EAR_ShutDownMatrix;
extern DPROCD		EAR_ShutDownPreset;
extern VPROCV		EAR_ShutDownTimer;
extern DPROCD		EAR_StartEvent;
extern DPROCV		EAR_StartTimer;
extern VPROCV		EAR_UpdateEar;

extern UINT WM_EAR_CALLBACK;

extern BOOL EP_RegisterEarServiceTable(BOOL bForceDirectSound);
extern VOID EP_ClearEarServiceTable(VOID);
 
#endif 


#pragma pack( pop, before_include1 )

#endif 