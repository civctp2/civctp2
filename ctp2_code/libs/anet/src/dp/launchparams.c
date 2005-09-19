/* 
Copyright (C) 1995-2001 Activision, Inc.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

/*----------------------------------------------------------------------------
 Functions to do I/O on dp_launchParams_t.
----------------------------------------------------------------------------*/

#include "launchparams.h"

/*----------------------------------------------------------------------------
 Read one field of a dp_launchParams_t from a string.
----------------------------------------------------------------------------*/
dp_result_t dp_launchParams_parse(dp_launchParams_t *params, char *line)
{
	int c1 = line[0];
	int c2 = line[1];
	char *arg;

	DPRINT(("dp_launchParams_parse: %s\n", line));

	/* Parse argument, if present */
	/* Allow either = or : for argument separator */
	arg = NULL;
	if ((c2 == '=') || (c2 == ':'))
		arg = line+2;

	/* Verify argument for those options that need it */
	if (!arg && (strchr("bcfidqJnspamguzo", c1))) {
		DPRINT(("Bad argument '%s'; -%c requires parameter\n", line, c1));
		return dp_RES_BAD;
	}

	switch (c1) {
	case 'b': params->commInitReq.baud=atoi(arg); break;
	case 'c': params->commInitReq.portnum=atoi(arg); break;
	case 'f': strcpy(params->Phonenum, arg); break;
	case 'i': strcpy(params->Modeministr, arg); break;
	case 'd': params->commInitReq.dialing_method=atoi(arg);break;
	case 'h': params->Host=1; break;
	case 'j': if (params->Join != 0) {
				DPRINT(("Must specify only one of -j or -z.\n"));
				return dp_RES_BAD;
			  }
			  params->Join=1; break;
	case 'q':
	case 'J': if (params->Join != 0) {
				DPRINT(("Must specify only one of -j or -z.\n"));
				return dp_RES_BAD;
			  }
			  params->JoinAnyFlags=atoi(arg);
			  params->Join=dp_LAUNCHPARAMS_JOIN_ANY | 0x01; break;
	case 'w': params->Wait=1; break;
	case 'v': params->EnablePlayervars=1; break;
	case 'n': strcpy(params->Driver,arg); break;
	case 'l': {
				  unsigned int	i = 0;
				  char	*tok;
				  char	argcopy[65];
				  
				  strncpy(argcopy, arg, sizeof(argcopy));
				  argcopy[sizeof(argcopy)-1] = 0;
				  for (tok=strtok(argcopy, "."); tok; tok=strtok(NULL, ".")) {
					  params->sessId[i++] = (char) strtol(tok, NULL, 16);
					  if (i >= sizeof(params->sessId))
						  break;
				  }
			  }
			  break;
	case 's': strcpy(params->Sessname, arg); break;
	case 'p': strcpy(params->Playname, arg); break;
	case 'a': strcpy(params->Adr,arg); break;
	case 'm': params->Maxplayers = atoi(arg); break;
	case 'g': strcpy(params->GameServer,arg); break;
	case 'u': strncpy(params->Username, arg, 16); break;
	case 'z': strncpy(params->Password, arg, 16); break;
	case 'o':
	{
		/*
		 * Addresses are separated by ';'
		 * We convert this to the following
		 *
		 * "text address1"\0
		 * "text address2"\0
		 * "text address3"\0
		 * \0
		 *
		 */
		char	*ptr;

		params->commInitReq.flags |= comm_INIT_FLAGS_CONN_ADDR;
		strcpy(params->OpenAddresses, arg);

		/*
		 * Add a second \0 to end
		 */
		ptr = params->OpenAddresses + strlen(params->OpenAddresses) + 1;
		*ptr = '\0';

		/*
		 * Convert ';' to \0
		 */
		ptr = params->OpenAddresses;
		while (ptr = strchr(ptr, ';'))
		{
			*ptr = '\0';
			ptr++;
		}
		break;
	}

	default:
		DPRINT(("bad argument '%s'\n", line));
		return dp_RES_BAD;
	}
	return dp_RES_OK;
}

#if 0
/* Main for testing dp_launchParams_parse() */
void main(int argc, char *argv[])
{
	int		i;
	dp_launchParams_t	params;
	dp_result_t	err;
	char	*tok;

	memset(&params, 0, sizeof(params));

	for (i = 1; i < argc; i++) {
		err = dp_launchParams_parse(&params, argv[i]);
		if (dp_RES_OK != err) {
			printf("Error type %x on argument : %s\n", err, argv[i]);
			exit(1);
		}
	}

	printf("Host : %d\n", params.Host);
	printf("Join : %d\n", params.Join);
	printf("Wait : %d\n", params.Wait);
	printf("Maxplayers : %d\n", params.Maxplayers);
	printf("EnablePlayervars : %d\n", params.EnablePlayervars);
	printf("JoinAnyFlags : %d\n", params.JoinAnyFlags);
	printf("Driver : %s\n", params.Driver);
	printf("Sessname : %s\n", params.Sessname);
	printf("Playname : %s\n", params.Playname);
	printf("Adr : %s\n", params.Adr);
	printf("GameServer : %s\n", params.GameServer);
	printf("Phonenum : %s\n", params.Phonenum);
	printf("Modeministr : %s\n", params.Modeministr);
	printf("OpenAddresses : %s\n", params.OpenAddresses);
	printf("Username : %s\n", params.Username);
	printf("Password : %s\n", params.Password);
	printf("commInitReq.portnum : %ld\n", params.commInitReq.portnum);
	printf("commInitReq.baud : %ld\n", params.commInitReq.baud);
	printf("commInitReq.dialing_method : %ld\n", params.commInitReq.dialing_method);
	printf("commInitReq.flags : %x\n", params.commInitReq.flags);
	
	printf("SessId : ");
	for (i = 0; i < sizeof(params.sessId); i++) {
		printf("%x.", params.sessId[i] & 0xFF);
	}
	printf("\n");
}
#endif
