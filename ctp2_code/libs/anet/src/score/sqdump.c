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

#include <time.h>
#ifdef WIN32
#include "../../demo/utils/unistd.h"
#else
#include <unistd.h>
#endif
#include "sq.h"
#include "wmq.h"	/* for wmq_RECORDTAG_* defines */

#define GET_INTERVAL	30	/* seconds to wait after sq_get returns empty
							 * before trying again.
							 */

/* Convert a binary buffer to hex notation. Don't use twice in one statement! */
static const char *hexstring(const unsigned char *binstr, int len)
{
	static char buf[768];
	int i;
	if (len < 1) return "";
	for (i = 0; i < len && i < 256; i++)
		sprintf(buf + 3*i, "%02x ", binstr[i]);
	buf[3*i-1] = '\0';
	return buf;
}

void main(int argc, char *argv[])
{
	dp_result_t err;
	time_t now;
	time_t next_get;
	char *wmqDir;

#ifdef UNIX
	setlinebuf(stdout);  /* line buffer if we are redirecting output */
#endif

	if (argc < 2) {
		printf("\
Usage: sqdump wmqDir\n\
	sqdump will listen to the score message queue in wmqDir and dump\n\
	any records to the screen\n");
		exit(1);
	}
	wmqDir = argv[1];

	printf("initializing score queue with dir:%s\n", wmqDir);
	sq_init(wmqDir);

	now = time(NULL);
	next_get = now;
	while (1) {
		char sessid[dp_KEY_MAXLEN];
		int sessidlen;
		dp_species_t sessType;
		sq_message_t msg;

		now = time(NULL);
		if ((now - next_get) >= 0) {
			err = sq_get(sessid, &sessidlen, &sessType, &msg);
			if (err == dp_RES_OK) {
				if (!memcmp(msg.recordTag, wmq_RECORDTAG_SCORE, 2)) {
					int i;

					printf("t:%d - Score Report for sessType:%d sessid:%s\n",
						now, sessType, hexstring(sessid, sessidlen));
					printf("* flags:%x id:%d uid:%d leaverid:%d leaveruid:%d\n",
						msg.u.scorerep.flags, msg.u.scorerep.id,
						msg.u.scorerep.uid, msg.u.scorerep.leaverid,
						msg.u.scorerep.leaveruid);
					printf("* nplayers:%d -\n", msg.u.scorerep.players->n_used);
					for (i = 0; i < msg.u.scorerep.players->n_used; i++) {
						assoctab_item_t *pi = assoctab_getkey(msg.u.scorerep.players, i);
						scorerep_player_t *psp = (scorerep_player_t *)pi->value;
						printf("* %d -> id:%d uid:%d blob:%s\n", i, pi->key,
							psp->uid, hexstring(psp->blob, psp->bloblen));
					}
					next_get = now;
				} else if (!memcmp(msg.recordTag,
								   wmq_RECORDTAG_SESSIONCLOSED, 2)) {
					printf("t:%d - Session Close for sessType:%d sessid:%s\n",
						now, sessType, hexstring(sessid, sessidlen));
					next_get = now;
				} else {
					printf("t:%d - Unknown record type:%s\n", now,
						hexstring(msg.recordTag, 2));
					break;
				}
			} else if (err == dp_RES_EMPTY) {
				next_get = now + GET_INTERVAL;
			} else if (err == dp_RES_AGAIN) {
				/* Non-score entry */
				next_get = now;
			} else {
				printf("Error: sq_get returns err:%d\n", err);
				break;
			}
		}
		if ((now - next_get) < 0) {
			/* Everything is waiting for time to pass, so sleep */
			int time_to_next_event = next_get - now;
			sleep(time_to_next_event);
		}
	}

	sq_destroy();
}
