




#ifndef aneterr_h
#define aneterr_h


#define dp_SEND_UNRELIABLE 0
#define dp_SEND_RELIABLE 1











#define dp_SEND_RESERVE_NONE          0
#define dp_SEND_RESERVE_QUARTER       2
#define dp_SEND_RESERVE_HALF          4
#define dp_SEND_RESERVE_THREEQUARTERS 6


typedef unsigned short dp_packetType_t;

#define sizeof_dpio_reliable_header 5


#define dpio_MAXLEN_UNRELIABLE 255

#define dpio_MAXLEN_RELIABLE (dpio_MAXLEN_UNRELIABLE - sizeof_dpio_reliable_header)


#define dp_MAXDPIDS		64000   
#define dp_FIRST_DPID      40	


#define dp_ID_NONE		(dp_MAXDPIDS+1)
typedef unsigned short dpid_t;	




typedef unsigned long dp_uid_t;			
#define dp_UID_NONE 0

#define dp_KARMA_NONE	0
typedef short dp_karma_t;	


typedef short dp_netchar_t;


typedef int dp_result_t;


typedef unsigned short dp_species_t;

#define DP_ILLEGAL_SPECIES	0xFFFF





#define dp_RES_OK					0
#define dp_RES_NOMEM				1	
#define dp_RES_EMPTY				2	
#define dp_RES_FULL					3	
#define dp_RES_BADSIZE				4	
#define dp_RES_BAD					5	
#define dp_RES_BUG					8	
#define dp_RES_NETWORK_NOT_RESPONDING	15
#define dp_RES_HOST_NOT_RESPONDING		16

#define dp_RES_BUSY					6	
#define dp_RES_ALREADY				7	
#define dp_RES_NOTYET				32	
#define dp_RES_AGAIN				28	

#define dp_RES_USER_CANCELLED		29	
#define dp_RES_CANT_WRITE			30	
#define dp_RES_ACCESS				31	
#define dp_RES_HELP					33	

#define dp_RES_PEER_CLOSED			24	

#define dp_RES_UNIMPLEMENTED		101


#define dp_RES_MODEM_BUSY			9
#define dp_RES_MODEM_NOANSWER		10
#define dp_RES_MODEM_NODIALTONE		11
#define dp_RES_MODEM_NORESPONSE		12
#define dp_RES_MODEM_BAD_INITSTRING	22
#define dp_RES_NETWORK_NOT_PRESENT	13
#define dp_RES_COMMDLL_BAD_VERSION	14


#define dp_RES_OPENED				17
#define dp_RES_CLOSED				18

#define dp_RES_STARTED				19
#define dp_RES_FINISHED				20
#define dp_RES_DELETED				21
#define dp_RES_CREATED				26	
#define dp_RES_CHANGED				27	


#endif
