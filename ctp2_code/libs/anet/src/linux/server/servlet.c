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

/* When a table session record is set, the function handleSess for the java
 * servlet corresponding to that session is called to handle the record.
 * The java servlet should be named as s<sessionType>, eg. for session type
 * 30, the servlet would be called s30.class.  It should implement two
 * functions: handleSess to handle a session record and outputResults to
 * print out results.  All servlets should be placed in a subdirectory
 * named servjar. */

#include <jni.h>
#include "dp.h"

typedef struct {
	jclass jSessClass;
	jobject jSessObj;
	jmethodID jHandleSessId;
	jmethodID jOutputSessId;
} jStruct;

assoctab_t *servlets;

/* Java stuff */
JNIEnv *env;
static struct {
	jobject obj;
	jmethodID loadClsId;
} jload;

static struct {
	jobject obj;
	jfieldID name;
	jfieldID password;
	jfieldID type;
	jfieldID maxPlayers;
	jfieldID curPlayers;
	jfieldID karma;
	jfieldID adrMaster;
	jfieldID reserved2;
	jfieldID userField;
	jfieldID dwUser1;
	jfieldID flags;
} jasess;

JavaVM *init_java(char *classpath)
{
	JavaVM *jvm;
	JDK1_1InitArgs vm_args;
	jclass jcls;
	jmethodID jmid;

	vm_args.version =0x10001;
	JNI_GetDefaultJavaVMInitArgs(&vm_args);
	vm_args.classpath = classpath;
	if (JNI_CreateJavaVM(&jvm, &env, &vm_args) < 0) {
		printf("Can't start Java VM\n");
		exit(1);
	} else if ((*env)->ExceptionOccurred != NULL) {
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	if ((jcls = (*env)->FindClass(env, "myLoader")) == NULL) {
		printf("Can't find class myLoader\n");
		exit(1);
	} else if ((*env)->ExceptionOccurred != NULL) {
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	if ((jmid = (*env)->GetMethodID(env, jcls, "<init>", "()V")) == NULL) {
		printf("Can't get init method id of class myLoader\n");
		exit(1);
	} else if ((*env)->ExceptionOccurred != NULL) {
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	if ((jload.obj = (*env)->NewObject(env, jcls, jmid)) == NULL) {
		printf("Can't get object of class myLoader\n");
		exit(1);
	} else if ((*env)->ExceptionOccurred != NULL) {
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	if ((jload.loadClsId = (*env)->GetMethodID(env, jcls, "loadClass", "(Ljava/lang/String;Z)Ljava/lang/Class;")) == NULL) {
		printf("Can't get loadClass method id of class myLoader\n");
		exit(1);
	} else if ((*env)->ExceptionOccurred != NULL) {
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	if ((jcls = (*env)->FindClass(env, "Activision/ANet$Session")) == NULL) {
		printf("Can't find class Activision/ANet$Session");
		exit(1);
	} else if ((*env)->ExceptionOccurred != NULL) {
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	if ((jmid = (*env)->GetMethodID(env, jcls, "<init>", "()V")) == NULL) {
		printf("Can't get init method id of class Activision/ANet$Session\n");
		exit(1);
	} else if ((*env)->ExceptionOccurred != NULL) {
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	if ((jasess.obj = (*env)->NewObject(env, jcls, jmid)) == NULL) {
		printf("Can't get object of class Activision/ANet$Session\n");
		exit(1);
	} else if ((*env)->ExceptionOccurred != NULL) {
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	if ((jasess.name = (*env)->GetFieldID(env, jcls, "name", "Ljava/lang/String;")) == NULL) {
		printf("Can't get name field id of class Activision/ANet$Session\n");
		exit(1);
	} else if ((*env)->ExceptionOccurred != NULL) {
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	if ((jasess.password = (*env)->GetFieldID(env, jcls, "password", "Ljava/lang/String;")) == NULL) {
		printf("Can't get passwd field id of class Activision/ANet$Session\n");
		exit(1);
	} else if ((*env)->ExceptionOccurred != NULL) {
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	if ((jasess.type = (*env)->GetFieldID(env, jcls, "type", "I")) == NULL) {
		printf("Can't get type field id of class Activision/ANet$Session\n");
		exit(1);
	} else if ((*env)->ExceptionOccurred != NULL) {
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	if ((jasess.maxPlayers = (*env)->GetFieldID(env, jcls, "maxPlayers", "I")) == NULL) {
		printf("Can't get maxPlayers field id of class Activision/ANet$Session\n");
		exit(1);
	} else if ((*env)->ExceptionOccurred != NULL) {
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	if ((jasess.curPlayers = (*env)->GetFieldID(env, jcls, "curPlayers", "I")) == NULL) {
		printf("Can't get curPlayers field id of class Activision/ANet$Session\n");
		exit(1);
	} else if ((*env)->ExceptionOccurred != NULL) {
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	if ((jasess.karma = (*env)->GetFieldID(env, jcls, "karma", "I")) == NULL) {
		printf("Can't get karma field id of class Activision/ANet$Session\n");
		exit(1);
	} else if ((*env)->ExceptionOccurred != NULL) {
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	if ((jasess.userField = (*env)->GetFieldID(env, jcls, "userField", "[B")) == NULL) {
		printf("Can't get userField field id of class Activision/ANet$Session\n");
		exit(1);
	} else if ((*env)->ExceptionOccurred != NULL) {
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	if ((jasess.dwUser1= (*env)->GetFieldID(env, jcls, "dwUser1", "I")) == NULL) {
		printf("Can't get init method id of class Activision/ANet$Session\n");
		exit(1);
	} else if ((*env)->ExceptionOccurred != NULL) {
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	if ((jasess.flags = (*env)->GetFieldID(env, jcls, "flags", "I")) == NULL) {
		printf("Can't get init method id of class Activision/ANet$Session\n");
		exit(1);
	} else if ((*env)->ExceptionOccurred != NULL) {
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	if ((jasess.adrMaster = (*env)->GetFieldID(env, jcls, "adrMaster", "[B")) == NULL) {
		printf("Can't get adrMaster field id of class Activision/ANet$Session\n");
		exit(1);
	} else if ((*env)->ExceptionOccurred != NULL) {
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	if ((jasess.reserved2 = (*env)->GetFieldID(env, jcls, "reserved2", "[B")) == NULL) {
		printf("Can't get reserved2 field id of class Activision/ANet$Session\n");
		exit(1);
	} else if ((*env)->ExceptionOccurred != NULL) {
		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);
	}
	return jvm;
}

/* copied from anetj.c */
static dp_result_t Session_CtoJava(jobject jSess, dp_session_t *s)
{
	jbyteArray arr;
	char *szPassword;

	/* Create the current object */
	if (!jSess)
		return dp_RES_BAD;

	/* Save the int fields. */
	(*env)->SetIntField(env, jSess, jasess.type,       0xffff & s->sessionType);
	(*env)->SetIntField(env, jSess, jasess.maxPlayers, 0xffff & s->maxPlayers);
	(*env)->SetIntField(env, jSess, jasess.curPlayers, 0xffff & s->currentPlayers);
	(*env)->SetIntField(env, jSess, jasess.karma,      0xffff & s->karma);
	(*env)->SetIntField(env, jSess, jasess.dwUser1,    0xffff & s->dwUser1);
	(*env)->SetIntField(env, jSess, jasess.flags,      0xffff & s->flags);

	/* Save the string fields.  Forcably null-terminate them; hope that's
	 * right.  It seems to be so for passwords, anyway.
	 */
	s->sessionName[sizeof(s->sessionName)-1] = 0;
	(*env)->SetObjectField(env, jSess, jasess.name, (*env)->NewStringUTF(env, s->sessionName)); 
	if ((*env)->ExceptionOccurred(env)) return dp_RES_BAD;
	s->szPassword[sizeof(s->szPassword)-1] = 0;
	szPassword = s->szPassword;
	(*env)->SetObjectField(env, jSess, jasess.password, (*env)->NewStringUTF(env, szPassword)); 
	if ((*env)->ExceptionOccurred(env)) return dp_RES_BAD;

	/* Save the byte array fields. */
	arr = (*env)->NewByteArray(env, sizeof(s->szUserField));
	if (!arr) return dp_RES_BAD;
	(*env)->SetByteArrayRegion(env, arr, 0, sizeof(s->szUserField), s->szUserField);
	if ((*env)->ExceptionOccurred(env)) return dp_RES_BAD;
	(*env)->SetObjectField(env, jSess, jasess.userField, arr); 
	if ((*env)->ExceptionOccurred(env)) return dp_RES_BAD;
	arr = (*env)->NewByteArray(env, sizeof(s->adrMaster));
	if (!arr) return dp_RES_BAD;
	(*env)->SetByteArrayRegion(env, arr, 0, sizeof(s->adrMaster), s->adrMaster);
	if ((*env)->ExceptionOccurred(env)) return dp_RES_BAD;
	(*env)->SetObjectField(env, jSess, jasess.adrMaster, arr); 
	if ((*env)->ExceptionOccurred(env)) return dp_RES_BAD;
	arr = (*env)->NewByteArray(env, sizeof(s->reserved2));
	if (!arr) return dp_RES_BAD;
	(*env)->SetByteArrayRegion(env, arr, 0, sizeof(s->reserved2), s->reserved2);
	if ((*env)->ExceptionOccurred(env)) return dp_RES_BAD;
	(*env)->SetObjectField(env, jSess, jasess.reserved2, arr); 
	if ((*env)->ExceptionOccurred(env)) return dp_RES_BAD;
	return dp_RES_OK;
}

int dp_PASCAL sess_cb(dptab_t *dptab, dptab_table_t *table, playerHdl_t src, 
	playerHdl_t dest, char *subkey, int subkeylen, void *buf, size_t sent,
	size_t total, int seconds_left, void *context, dp_result_t status)
{
	dp_result_t err;
	int keylen;
	char classname[256];
	jStruct *jstruct;
	jobject jSession;
	dp_t *dp = (dp_t *)context;
	dp_session_t s;
	DPRINT(("in sess_cb\n"));
	if (!dp || !buf) {
		printf("sess_cb: null dp or buf\n");
		return dp_RES_BAD;
	}
	if (status != dp_RES_OK && status != dp_RES_DELETED)
		return dp_RES_OK;
	dp_unpack_session(dp, subkey, subkeylen, buf, total, &s);
	jstruct = (jStruct *)assoctab_subscript(servlets, s.sessionType);
	if (jstruct == NULL) {
		jstruct = (jStruct *)assoctab_subscript_grow(servlets, s.sessionType);
		sprintf(classname, "s%d", s.sessionType);
		err = loadclass(classname, jstruct, dp);
		if (err != dp_RES_OK) {
			jstruct->jSessClass = NULL;
			jstruct->jSessObj = NULL;
			jstruct->jHandleSessId = NULL;
			jstruct->jOutputSessId = NULL;
			printf("sess_cb: ignoring session type %d\n", s.sessionType);
			return err;
		}
	} else if (jstruct->jSessObj == NULL || jstruct->jHandleSessId == NULL)
		return dp_RES_EMPTY;
 	err = Session_CtoJava(jasess.obj, &s);
	if (err != dp_RES_OK)
		return err;
	err = (*env)->CallIntMethod(env, jstruct->jSessObj, jstruct->jHandleSessId, jasess.obj, (jint)status);
	if ((*env)->ExceptionOccurred != NULL) {
/*		(*env)->ExceptionDescribe(env); */
		(*env)->ExceptionClear(env);
	}
	return err;
}

dp_result_t loadclass(char *classname, jStruct *jstruct, dp_t *dp)
{
	jmethodID jmid;
	jstring jclassname;
	jclassname = (*env)->NewStringUTF(env, classname);
	if ((jstruct->jSessClass = (*env)->CallObjectMethod(env, jload.obj, jload.loadClsId, jclassname)) == NULL) {
		printf("Can't find class %s\n", classname);
		return dp_RES_BAD;
	} else if ((*env)->ExceptionOccurred != NULL) {
/*		(*env)->ExceptionDescribe(env); */
		(*env)->ExceptionClear(env);
	}
	if ((jmid = (*env)->GetMethodID(env, jstruct->jSessClass, "<init>", "(J)V")) == NULL) {
		printf("Can't get init method id of class %s\n", classname);
		return dp_RES_BAD;
	} else if ((*env)->ExceptionOccurred != NULL) {
/*		(*env)->ExceptionDescribe(env); */
		(*env)->ExceptionClear(env);
	}
	if ((jstruct->jSessObj = (*env)->NewObject(env, jstruct->jSessClass, jmid, (jlong)dp)) == NULL) {
		printf("Can't get object of class %s\n", classname);
		return dp_RES_BAD;
	} else if ((*env)->ExceptionOccurred != NULL) {
/*		(*env)->ExceptionDescribe(env); */
		(*env)->ExceptionClear(env);
	}
	if ((jstruct->jHandleSessId = (*env)->GetMethodID(env, jstruct->jSessClass, "handleSess", "(LActivision/ANet$Session;I)I")) == NULL) {
		printf("Can't get handleSess method id of class %s\n", classname);
		return dp_RES_BAD;
	} else if ((*env)->ExceptionOccurred != NULL) {
/*		(*env)->ExceptionDescribe(env); */
		(*env)->ExceptionClear(env);
	}
	if ((jstruct->jOutputSessId = (*env)->GetMethodID(env, jstruct->jSessClass, "outputResults", "(Z)V")) == NULL) {
		printf("Can't get handle outputResults method id of class %s\n", classname);
		return dp_RES_BAD;
	} else if ((*env)->ExceptionOccurred != NULL) {
/*		(*env)->ExceptionDescribe(env); */
		(*env)->ExceptionClear(env);
	}
	return dp_RES_OK;
}

void printResults(int bDetailed)
{
	int i;
	char *p;
	assoctab_item_t *pi;
	jStruct *jstruct;
	for (i = 0, p = (char *)servlets->buf; i < servlets->n_used; i++, p += servlets->unit) {
		if ((pi = (assoctab_item_t *)p) == NULL) {
			printf("error getting assoctab record\n");
			exit(1);
		}
		jstruct = (jStruct *)(&(pi->value));
		if (jstruct->jSessObj == NULL || jstruct->jOutputSessId == NULL)
			return;
		(*env)->CallIntMethod(env, jstruct->jSessObj, jstruct->jOutputSessId, bDetailed);
		if ((*env)->ExceptionOccurred != NULL) {
/*			(*env)->ExceptionDescribe(env); */
			(*env)->ExceptionClear(env);
		}
	}
}

#ifdef TEST_MAIN

#include <time.h>
#include "eclock.h"
#define WAIT_TIME 120 * ECLOCKS_PER_SEC
boolean mySessOpen;

int dp_PASCAL open_cb(dp_session_t *s, long *timeout, long flags, void *context)
{
	dp_result_t err;
	int keylen;
	if (s == NULL) {
		printf("Couldn't create session\n");
		exit(1);
	}
	printf("open_cb: session created\n");
	mySessOpen = TRUE;
	return dp_RES_OK;
}

void main(int argc, char **argv)
{
	int i, bDestroy, bClose;
	clock_t tstart;

	/* dp stuff */
	dp_t *dp;
	dp_session_t mysess;
	dp_result_t err;
	dpid_t src, dest;
	char buf[512];
	size_t size;
	 
	JavaVM *jvm;

	if ((argc != 3) || strcmp(argv[1], "-classpath")) {
		printf("Usage: servlet -classpath <classpath>\n");
		exit(1);
	}

	/* dp stuff */
	{	dp_transport_t trans;
		commInitReq_t comm;
		memset(&trans, 0, sizeof(dp_transport_t));
		memset(&comm, 0, sizeof(commInitReq_t));
		memset(&mysess, 0, sizeof(dp_session_t));
		strcpy(trans.fname, "anet\\winets2.dll");
		comm.sessionId = rand() + eclock();
		comm.reqLen = sizeof(commInitReq_t);
		comm.baud = 19200;
		err = dpCreate(&dp, &trans, &comm, NULL);
		if (err != dp_RES_OK) {
			printf("Can't create dp\n");
			exit(1);
		}
		dpSetGameServer(dp, "california3.activision.com");
		mysess.maxPlayers = 20;
		mysess.sessionType = 30;
		mysess.flags = dp_SESSION_FLAGS_CREATESESSION|dp_SESSION_FLAGS_ISLOBBY;
		strcpy(mysess.sessionName, "myGame");
		err = dpOpen(dp, &mysess, open_cb, (void *)dp);
		if (err != dp_RES_OK) {
			printf("Can't open session\n");
			exit(1);
		}
		tstart = eclock();
		while (!mySessOpen) {
			size = 512;
			dpReceive(dp, &src, &dest, 0, buf, &size);
			if ((long)(eclock() - tstart) > 10 * ECLOCKS_PER_SEC) {
				printf("Timed out waiting for session\n");
				exit(1);
			}
		}
		dptab_setTableCallback(dp->sessions, sess_cb, (void *)dp);
		dptab_setTableCallback(dp->mysessions, sess_cb, (void *)dp);
		servlets = assoctab_create(sizeof(jStruct));
		for (i = 0; i < 4; i++) {
			char name[10];
			sprintf(name, "%s%d", "Player", i);
			dpCreatePlayer(dp, NULL, NULL, name);
		}
	}

	jvm = init_java(argv[2]);

	/* main loop */
	tstart = eclock();
	bDestroy = 1;
	bClose = 1;
	while (eclock() < tstart + WAIT_TIME) {
		if (bDestroy && eclock() > tstart + WAIT_TIME/3) {
			printResults(1);
			for (i = 0; i < 3; i++) {
				dpid_t id = dp->firstId + i;
				dpDestroyPlayer(dp, id);
			}
			bDestroy = 0;
		}
		if (bClose && eclock() > tstart + 2*WAIT_TIME/3) {
			printResults(1);
			dpClose(dp);
			bClose = 0;
		}
		size = 512;
		err = dpReceive(dp, &src, &dest, 0, buf, &size);
		if (err == dp_RES_EMPTY) {
			continue;
		} else if (err != dp_RES_OK) {
			printf("dpReceive err %d", err);
			exit(1);
		} else {
			switch (*((dp_packetType_t *)buf)) {
				default:
					break;
			}
		}
	}
	printResults(1);
	(*jvm)->DestroyJavaVM(jvm);
}

#endif
