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

/*--------------------------------------------------------------------------
 C code to invoke a score handling class written in Java.  Invokes game
 specific java servlet code via the ScoreServletHandler methods.

 Score handling servlets must implement the ScoreServlet interface as
 score_<sessionType>.class, which will involve implementing the ScoreReport
 interface for the return values.
 All score handling classes for a session type should be placed in a
 subdirectory named servjar, in a .jar file named score_<sessionType>.jar.
 
 For example, for session type 30, the class score_30.class in the jar file
 score_30.jar would be loaded, and when a score report for that session type
 comes in, the score_30.getReport() method would be called to get an
 instance of java ScoreReport implementing class to fill with data using
 the addPlayer() method.  Periodically, score_30.poll() will then be called
 to allow processing of reports as data accumulates.  score_30.jar might
 include any number of other classes involved in handling the request and
 data processing.
--------------------------------------------------------------------------*/

#include <jni.h>
#include <time.h>
#ifdef WIN32
#include "../../demo/utils/unistd.h"
#else
#include <unistd.h>
#endif
#include "assoctab.h"
#include "Activision_score_0005finit.h"	/* for c_main */
#include "sq.h"
#include "wmq.h"	/* for wmq_RECORDTAG_* defines */

#define POLL_INTERVAL	10	/* seconds between polls */
#define GET_INTERVAL	30	/* seconds to wait after sq_get returns empty
							 * before trying again.
							 */

/* The virtual machine's data */
JNIEnv *env;

/* the object and methods needed in the ScoreServletHandler */
jobject jServletHandler;
jmethodID jGetReport;
jmethodID jCloseSession;
jmethodID jPoll;

/* A struct containing classes and methods cached for each servlet */
typedef struct {
	jmethodID jAddPlayer;
} jServlet_t;

/* A cache of jServlet_t's */
assoctab_t *servlets;

/* Java objects used to pass in data in parameters to java methods. */
/* Reusing these saves the garbage collector a lot of work. */
jobject jSessId;
char *bytesSessId;
jobject jBlob;
char *bytesBlob;

/*--------------------------------------------------------------------------
 Load all the java classes, methods, and objects we will be needing, and
 initialize the servlet method cache.

 Returns 0 on success.
--------------------------------------------------------------------------*/
int java_init(const char *dbAddr)
{
	const char classname[] = "Activision/ScoreServletHandler";
	jstring jdbAddr;
	jclass jClass;
	jmethodID jInit;

	if (dbAddr) {
		if ((jdbAddr = (*env)->NewStringUTF(env, dbAddr)) == NULL) {
			printf("Can't get java string to pass dbAddr\n");
			return -1;
		} else if ((*env)->ExceptionOccurred(env) != NULL) {
/*			(*env)->ExceptionDescribe(env); */
			(*env)->ExceptionClear(env);
		}
	} else
		jdbAddr = NULL;

	/* Get the ScoreServletHandler class. */
	if ((jClass = (*env)->FindClass(env, classname)) == NULL) {
		printf("Can't get class %s\n", classname);
		return -1;
	} else if ((*env)->ExceptionOccurred(env) != NULL) {
/*		(*env)->ExceptionDescribe(env); */
		(*env)->ExceptionClear(env);
	}

	/* Get the ScoreServletHandler class's constructor. */
	if ((jInit = (*env)->GetMethodID(env, jClass, "<init>", "(Ljava/lang/String;)V")) == NULL) {
		printf("Can't get <init> method of class %s\n", classname);
		return -1;
	} else if ((*env)->ExceptionOccurred(env) != NULL) {
/*		(*env)->ExceptionDescribe(env); */
		(*env)->ExceptionClear(env);
	}

	/* Construct an instance of that class */
	if ((jServletHandler = (*env)->NewObject(env, jClass, jInit,
			jdbAddr)) == NULL) {
		printf("Can't instantiate object of class %s\n", classname);
		return -1;
	} else if ((*env)->ExceptionOccurred(env) != NULL) {
		(*env)->ExceptionDescribe(env); 
		(*env)->ExceptionClear(env);
	}
 
	/* Get the methods we're interested in */
	if ((jGetReport = (*env)->GetMethodID(env, jClass,
			"getReport", "(S[BSSISII)LActivision/ScoreReport;")) == NULL) {
		printf("Can't get getReport method of class %s\n", classname);
		return -1;
	} else if ((*env)->ExceptionOccurred(env) != NULL) {
/*		(*env)->ExceptionDescribe(env); */
		(*env)->ExceptionClear(env);
	}

	if ((jCloseSession = (*env)->GetMethodID(env, jClass, "closeSession",
			"(S[B)V")) == NULL) {
		printf("Can't get closeSession method of class %s\n", classname);
		return -1;
	} else if ((*env)->ExceptionOccurred(env) != NULL) {
/*		(*env)->ExceptionDescribe(env); */
		(*env)->ExceptionClear(env);
	}

	if ((jPoll = (*env)->GetMethodID(env, jClass, "poll", "()V")) == NULL) {
		printf("Can't get poll method of class %s\n", classname);
		return -1;
	} else if ((*env)->ExceptionOccurred(env) != NULL) {
/*		(*env)->ExceptionDescribe(env); */
		(*env)->ExceptionClear(env);
	}

	/* Create the cache of servlet methods */
	servlets = assoctab_create(sizeof(jServlet_t));

	/* Create the byte arrays we use to pass in SessionId's and Blob's */
	jSessId = (*env)->NewByteArray(env, 8 /* inet sessidlen */);
	jBlob = (*env)->NewByteArray(env, scorerep_MAX_BLOBLEN);
	if ((jSessId == NULL) || (jBlob == NULL)) {
		printf("Can't allocate jSessId or jBlob byte array\n");
		return -1;
	}
	bytesSessId = (*env)->GetByteArrayElements(env, jSessId, NULL);
	bytesBlob = (*env)->GetByteArrayElements(env, jBlob, NULL);
	if ((bytesSessId == NULL) || (bytesBlob == NULL)) {
		printf("Can't allocate bytesSessId or bytesBlob byte array elements\n");
		return -1;
	}

	return 0;
}

/*--------------------------------------------------------------------------
 Load any servlet methods we are interested in from the class associated
 with the given ScoreReport object and place them into jservlet.

 Return 0 on success.
--------------------------------------------------------------------------*/
int loadServlet(jobject jScoreReport, jServlet_t *jservlet)
{
	jclass jClass;

	if ((jScoreReport == NULL) || (jservlet == NULL)) {
		printf("loadServlet: NULL parameter\n");
		return -1;
	}
	if ((jClass = (*env)->GetObjectClass(env, jScoreReport)) == NULL) {
		printf("loadServlet: can't get class of ScoreReport object\n");
		return -1;
	}

	/* Find its addPlayer method */
	if ((jservlet->jAddPlayer = (*env)->GetMethodID(env, jClass,
			"addPlayer", "(SI[BS)V")) == NULL) {
		printf("Can't find addPlayer method id of ScoreReport object class\n");
		return -1;
	} else if ((*env)->ExceptionOccurred(env) != NULL) {
/*		(*env)->ExceptionDescribe(env); */
		(*env)->ExceptionClear(env);
	}
	
	return 0;
}

/*--------------------------------------------------------------------------
 Given a score report for a particular session type,
 call the Java class that handles it.

 Returns 0 on success or -1 on error.
 FIXME: needs error differentiation.  Some might want to be non-fatal. 
--------------------------------------------------------------------------*/
int updateScore(int sessType, char *sessid, int sessidlen, scorerep_t *rep)
{
	int err;
	jServlet_t *jservlet;
	jobject jsrep;
	int i;

	/* Stick sessid into our java byte array */
	if (sessidlen != 8) {
		printf("updateScore: sessidlen %d doesn't match inet (8)\n", sessidlen);
		return -1;
		assert(sessidlen == 8 /* inet sessidlen */);
	}
	memcpy(bytesSessId, sessid, sessidlen);

	jsrep = (*env)->CallObjectMethod(env, jServletHandler, jGetReport,
			rep->sessType, jSessId, rep->flags, rep->id, rep->uid,
			rep->leaverid, rep->leaveruid, rep->players->n_used);
	if (jsrep == NULL) {
		printf("updateScore: getReport failed\n");
		return -1;
	} else if ((*env)->ExceptionOccurred(env) != NULL) {
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}

	/* Check the cache for the methods we need.  If we haven't seen this
	 * sessType before, get and cache what we need using the object.
	 */
	jservlet = (jServlet_t *)assoctab_subscript(servlets, sessType);
	if (jservlet == NULL) {
		jservlet = (jServlet_t *)assoctab_subscript_grow(servlets, sessType);
		if (jservlet == NULL) {
			printf("updateScore: Out of memory for cache!\n");
			return -1;
		}
		err = loadServlet(jsrep, jservlet);
		if (err != 0) {
			jservlet->jAddPlayer = NULL;
			printf("updateScore: load failed, ignoring SessionType %d\n",
				sessType);
			return -1;
		}
		printf("updateScore: loaded methods for SessionType %d\n", sessType);
	} else if (jservlet->jAddPlayer == NULL)
		return -1;

	for (i = 0; i < rep->players->n_used; i++) {
		assoctab_item_t *pi = assoctab_getkey(rep->players, i);
		short id = (short)pi->key;
		scorerep_player_t *player = (scorerep_player_t *)(pi->value);
		
		assert(player->bloblen <= scorerep_MAX_BLOBLEN);
		memcpy(bytesBlob, player->blob, player->bloblen);
		
		(*env)->CallVoidMethod(env, jsrep, jservlet->jAddPlayer,
			id, player->uid, jBlob, player->bloblen);
		if ((*env)->ExceptionOccurred(env) != NULL) {
			(*env)->ExceptionDescribe(env);
			(*env)->ExceptionClear(env);
			return -1;
		}
	}

	return 0;
}

/*--------------------------------------------------------------------------
 Given a session close message, call the java method to handle it.

 Returns 0 on success or -1 on error.
 FIXME: needs error differentiation.  Some might want to be non-fatal. 
--------------------------------------------------------------------------*/
int closeSession(int sessType, char *sessid, int sessidlen)
{
	/* Stick sessid into our java byte array */
	if (sessidlen != 8) {
		printf("closeSession: sessidlen %d doesn't match inet (8)\n",
			sessidlen);
		return -1;
		assert(sessidlen == 8 /* inet sessidlen */);
	}
	memcpy(bytesSessId, sessid, sessidlen);

	(*env)->CallVoidMethod(env, jServletHandler, jCloseSession,
		sessType, jSessId);
	if ((*env)->ExceptionOccurred(env) != NULL) {
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
		return -1;
	}
	return 0;
}

static int jargv2argv(JNIEnv *env, jobjectArray jargv, char **argv, int maxargc)
{
	int i;
	jsize jargc;
	
	/* pull the first argument out of the jargv array */
	jargc = (*env)->GetArrayLength(env, jargv);
	for (i = 0; i < jargc && i < maxargc; i++) {
		int len;
		jstring jstr;
		const char *utf8temp;
	
		jstr = (*env)->GetObjectArrayElement(env, jargv, i);
		len = (*env)->GetStringUTFLength(env, jstr);
		argv[i] = (char *)malloc(len + 1);
		utf8temp = (*env)->GetStringUTFChars(env, jstr, NULL);

		memcpy(argv[i], utf8temp, len);
		argv[i][len] = '\0';
		(*env)->ReleaseStringUTFChars(env, jstr, utf8temp);
	}

	return i;
}

#define MAX_ARGC 8
	
/*--------------------------------------------------------------------------
 If we avoid the invocation api, we use a Java main function which simply
 calls this function.
--------------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_Activision_score_1init_c_1main(JNIEnv *_env, jclass score_init_class, jobjectArray jargv)
{
	dp_result_t err;
	time_t now;
	time_t next_poll;
	time_t next_get;
	int argc;
	char *argv[MAX_ARGC];
	char *wmqDir;
	char *dbAddr = NULL;

	env = _env;

#ifdef UNIX
	setlinebuf(stdout);  /* line buffer if we are redirecting output */
#endif

	argc = jargv2argv(env, jargv, argv, MAX_ARGC);
	if (argc < 1) {
		printf("\
Usage: java Activision.score_init wmqDir [dbAddr]\n\
	jscore2 will listen to the score message queue in wmqDir and send\n\
	any mysql updates to the mysql server at dbAddr (which should be in the\n\
	format my.server.name/mydbname?user=myusername&password=mypassword)\n");
		exit(1);
	}
	if (argc > 1)
		dbAddr = argv[1];
	wmqDir = argv[0];

	printf("initializing java\n");
	if (java_init(dbAddr) != 0)
		exit(1);

	printf("initializing wmq with dir:%s\n", wmqDir);
	sq_init(wmqDir);

	now = time(NULL);
	next_get = now;
	next_poll = now + POLL_INTERVAL;
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
					printf("t:%d - got a scorerep sessType:%d from uid:%d\n",
						now, sessType, msg.u.scorerep.uid);
					if (updateScore(sessType, sessid, sessidlen,
							&(msg.u.scorerep)) != 0) {
						printf("updateScore returns error\n");
						break;
					}
					next_get = now;
				} else if (!memcmp(msg.recordTag,
								   wmq_RECORDTAG_SESSIONCLOSED, 2)) {
					printf("t:%d - got SessionClosed sessType:%d\n", now,
						sessType);
					if (closeSession(sessType, sessid, sessidlen) != 0) {
						printf("closeSession returns error\n");
						break;
					}
					next_get = now;
				} else {
					printf("t:%d - got unknown record type:%02x %02x\n", now,
						msg.recordTag[0], msg.recordTag[1]);
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
		if ((now - next_poll) >= 0) {
			printf("t:%d - polling servlets...\n", now);
			(*env)->CallVoidMethod(env, jServletHandler, jPoll);
			if ((*env)->ExceptionOccurred(env) != NULL) {
				(*env)->ExceptionDescribe(env);
				(*env)->ExceptionClear(env);
				printf("Activision/ScoreServletHandler.poll() exception\n");
				break;
			}
			next_poll = now + POLL_INTERVAL;
		}
		if (((now - next_poll) < 0) && ((now - next_get) < 0)) {
			/* Everything is waiting for time to pass, so sleep */
			int time_to_next_event = ((next_poll < next_get) ?
				(next_poll - now) : (next_get - now));
			sleep(time_to_next_event);
		}
	}

	sq_destroy();

	(*env)->ReleaseByteArrayElements(env, jSessId, bytesSessId, 0);
	(*env)->ReleaseByteArrayElements(env, jBlob, bytesBlob, 0);
	
	env = NULL;
}
