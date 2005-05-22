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
 JNI wrapper for ActiveNet's main methods.

 To reduce the overhead of JNI's heavyweight but portable interface technique,
 enumeration methods will generally be implemented as functions that return 
 the entire set of results in a single call.

 $Log: anetj.c $
 Revision 1.30  1997/10/08 21:34:12  lbennett
 Conform to new dpEnumApps.
 Revision 1.29  1997/10/07 02:15:05  anitalee
 fixed commInitReq_JavatoC and set commInitReq.sessionId;
 latter should probably be hidden from java.
 Revision 1.28  1997/10/02 03:24:38  lbennett
 Added dprint.
 Revision 1.27  1997/09/30 23:16:21  anitalee
 added commInitReq
 Revision 1.26  1997/09/29 02:42:24  dkegel
 Added Player.getLatency()
 Revision 1.25  1997/09/28 03:52:34  dkegel
 Support new whacky enumsrv2.c scheme of enumerating servers
 Revision 1.24  1997/09/27 21:21:08  dkegel
 Pass back dpReceive error return values as empty packets with new err field nonzero.
 Revision 1.23  1997/09/14 02:33:58  dkegel
 Support dpReadyToFreeze
 Revision 1.22  1997/09/13 23:01:37  dkegel
 Added getFlags() in session.
 Revision 1.21  1997/09/09 22:22:07  anitalee
 Now compiles with dp2 library.
 Revision 1.20  1997/08/13 20:40:26  dkegel
 Added a few DPRINT's.
 Revision 1.19  1997/08/12 21:45:28  dkegel
 Got rid of two printf's!
 Revision 1.18  1997/08/12 19:52:31  dkegel
 DLL's now named with dp_transport_t's.
 Server callback hostname now const.
 Revision 1.17  1997/07/25 18:18:13  anitalee
 Now launches using dpLaunchApp(dp_appParam_t)
 Revision 1.16  1997/07/15 21:45:54  dkegel
 Added support for shellOpts on each installed app.
 Revision 1.15  1997/07/10 22:31:41  dkegel
 Added ANet.InstalledApp.execWithConsole().
 Revision 1.14  1997/07/10 19:06:50  dkegel
 Better log messages.
 Revision 1.13  1997/06/27 22:05:15  dkegel
 First cut at sending & receiving packets.
 Revision 1.12  1997/06/27 18:15:59  dkegel
 Support writeObject aka dpFreeze.
 Revision 1.11  1997/06/27 03:17:48  dkegel
 Disabled printf's.
 Revision 1.10  1997/06/27 03:16:29  dkegel
 Supports deleting players.  Not yet tested.
 Revision 1.9  1997/06/27 02:13:29  dkegel
 Supports player creation.
 Revision 1.8  1997/06/25 23:05:33  dkegel
 1. Supports enumPlayers().
 2. Fixed session open/create.
 Revision 1.7  1997/06/25 19:31:37  dkegel
 Don't forget to propagate the dwUser1 field, too.
 Mask all short fields to avoid getting garbage high words.
 Revision 1.6  1997/06/25 18:37:36  dkegel
 First cut at supporting dpOpen and dpClose.
 Revision 1.5  1997/06/23 23:12:56  dkegel
 Fixed enumSessions (it had uninit'd field ids and no return val!)
 Revision 1.4  1997/06/23 22:38:09  dkegel
 Added setGameServer(), initial cut at enumSessions() (not yet tested).
 Revision 1.3  1997/06/22 23:42:51  dkegel
 It's ANet, not Anet.  Makes a difference when running from .jar archive.
 Revision 1.2  1997/06/22 22:55:31  dkegel
 1. Added ANet.InstalledApp class and method to create vector of them
 by calling dpEnumApp().
 2. Rearranged to group stuff related to each inner class together.
 Revision 1.1  1997/06/20 22:57:12  dkegel
 Initial revision
--------------------------------------------------------------------------*/
#include "Activision_ANet.h"	/* produced by javah from anet.java */
#include "Activision_ANet_00024Session.h"	/* produced by javah from anet.java */
#include "Activision_ANet_00024InstalledApp.h"

/* To enable dprinting, define DEBUG or DPRNT in the C compiler options */
//define DPRNT
#include "dp.h"	/* ships with activenet for C developers */
#include "eclock.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

//#include <stdio.h>
//#define PRINTF(s) { printf s; fflush(stdout); }
#define PRINTF(s) 

static dp_t *myDP = NULL;
static jobject gj_mySession = NULL;	/* global handle to the currently open Session */

/*-------------------------------------------------------------------------
 Save the env here before calling a dp function that might issue a
 callback.  Clear it after the function returns.
--------------------------------------------------------------------------*/
/*JNIEnv *callback_env = NULL;*/

/* 
* Get a conventional C string from a Java string.  Use UTF-8 encoding
* to convert from Java's internal Unicode format to a byte array for
* the C representation.  If the string is too long to fit into the buffer,
* truncate it at a true character boundary.
*/
static char *getCstring(JNIEnv *env, jstring jstr, char *buf, size_t buflen)
{
    int len;
	const char *utf8path;

	/* Turn it into a null-terminated UTF string */
	len = (*env)->GetStringUTFLength(env, jstr); 
	if (len > 0) {
		/* Get the converted bytes */
		utf8path = (*env)->GetStringUTFChars(env, jstr, NULL); 
		if (!utf8path) {
			DPRINT(("getCstring: utfchars returns null\n"));
			(*env)->ReleaseStringUTFChars(env, jstr, utf8path); 
			return NULL;
		}

		/* Return NULL if the buffer is too small */
		/*if (len >= buflen) {
			DPRINT(("getCstring: len %d > buflen %d; utf8path %s\n", len, buflen, utf8path));
			return NULL;
		}*/

		/*
		* Truncate to a character boundary.  UTF8 contains either 
		* single-byte characters, which have the high bit of the byte
		* set to 0, or multi-byte characters, which have the high bit
		* of the byte set to 1, and the next highest bit set to 0,
		* except for the first byte of the multi-byte character, which
		* has the first two or more bits set to 1 (followed by a 0 bit).
		*/
		while(len >= (buflen-1)) {
			if((utf8path[len-1] & 0x80) == 0)
				len--;
			else while((utf8path[len-1] & 0xC0) != 0xC0)
				len--;
		}

		/* Copy it to the given buffer and release the original */
		memcpy(buf, utf8path, len);
		(*env)->ReleaseStringUTFChars(env, jstr, utf8path); 
	}

	/* Null-terminate it. */
	buf[len] = 0;

	return buf;
}

/* Utility function to check for exceptions.  Not sure if useful. */
static jthrowable checkExc(JNIEnv *env)
{
	jthrowable exc = (*env)->ExceptionOccurred(env);
	if (exc) {
		/* We don't do much with the exception, except that we print a
		*        debug message using ExceptionDescribe, clear it, and throw
		*               a new exception. */
		jclass newExcCls;

		(*env)->ExceptionDescribe(env);
		(*env)->ExceptionClear(env);

		newExcCls = (*env)->FindClass(env, "java/lang/IllegalArgumentException");
		if (newExcCls == 0) /* Unable to find the new exception class, give up. */
			return exc;
		(*env)->ThrowNew(env, newExcCls, "thrown from C code");
	}
	return exc;
}

/*------------------ Activision.ANet.commInitReq --------------------------*/

/* Temporary - at the moment, must reinitialize this for each call.
 * One of these days, we'll figure out how to cache them.
 */
static struct {
	jmethodID hmcommInitReqConstructor;

	jfieldID sessionId;
	jfieldID portnum;
	jfieldID baud;
	jfieldID phonenum;
	jfieldID modeministr;
	jfieldID dialingMethod;
} cfid;

static long stop; //set to 1 to stop dial/answer; reset to 0 before dpCreate

/* Initialize the field id's in cfid for use by commInitReq_CtoJava() and
 * commInitReq_JavatoC().  Must call often for now.
 * Returns hccommInitReq on success, NULL on failure.
 */
static jclass commInitReq_init(JNIEnv *env)
{
	jclass hccommInitReq;

	DPRINT(("commInitReq_init:\n"));
	/* Get handle to result class */
	hccommInitReq = (*env)->FindClass(env, "Activision/ANet$commInitReq");
	if (!hccommInitReq) return NULL;

	/* Get constructor */
	cfid.hmcommInitReqConstructor = (*env)->GetMethodID(env, hccommInitReq, "<init>", "()V");
	if (!cfid.hmcommInitReqConstructor)
		return NULL;

	/* Get field id's.  Later, perhaps we can cache these. */
	cfid.phonenum   = (*env)->GetFieldID(env, hccommInitReq, "phonenum", "Ljava/lang/String;"); 
	if (!cfid.phonenum) return NULL;
	cfid.modeministr   = (*env)->GetFieldID(env, hccommInitReq, "modeministr", "Ljava/lang/String;"); 
	if (!cfid.modeministr) return NULL;

	cfid.sessionId  = (*env)->GetFieldID(env, hccommInitReq, "sessionId", "I"); 
	if (!cfid.sessionId) return NULL;
	cfid.portnum    = (*env)->GetFieldID(env, hccommInitReq, "portnum", "I"); 
	if (!cfid.portnum) return NULL;
	cfid.baud    = (*env)->GetFieldID(env, hccommInitReq, "baud", "I"); 
	if (!cfid.baud) return NULL;
	cfid.dialingMethod = (*env)->GetFieldID(env, hccommInitReq, "dialingMethod", "I"); 
	if (!cfid.dialingMethod) return NULL;

	return hccommInitReq;
}

/* Convert a C commInitReq_t to a Java ANet.commInitReq. 
 * Returns jobject on success, NULL on failure.
 */
static jobject commInitReq_CtoJava(JNIEnv *env, jclass hccommInitReq, commInitReq_t *c)
{
	jobject cur;

	/* Create the current object */
	cur = (*env)->NewObject(env, hccommInitReq, cfid.hmcommInitReqConstructor);
	if (!cur)
		return NULL;

	/* Save the four int fields. */
	(*env)->SetIntField(env, cur, cfid.sessionId,  0xffff & c->sessionId);
	(*env)->SetIntField(env, cur, cfid.portnum, 0xffff & c->portnum);
	(*env)->SetIntField(env, cur, cfid.baud, 0xffff & c->baud);
	(*env)->SetIntField(env, cur, cfid.dialingMethod, 0xffff & c->dialing_method);
	/* Save the two string fields.  
	 */
	(*env)->SetObjectField(env, cur, cfid.phonenum, (*env)->NewStringUTF(env, c->phonenum)); 
	if ((*env)->ExceptionOccurred(env)) return NULL;

	(*env)->SetObjectField(env, cur, cfid.modeministr, (*env)->NewStringUTF(env, c->modeministr)); 
	if ((*env)->ExceptionOccurred(env)) return NULL;

	return cur;
}

#define mytemp_STR_LEN 256

/* Convert a Java ANet.commInitReq to a C commInitReq_t. 
 * Returns pointer to buf on success, NULL on failure.
 */
static commInitReq_t *commInitReq_JavatoC(JNIEnv *env, jobject cur, commInitReq_t *c)
{
	jstring js;

	c->reqLen = sizeof(commInitReq_t);
PRINTF(("jtoc:\n"));
	/* Get the int fields. */
	c->sessionId    = (*env)->GetIntField(env, cur, cfid.sessionId);
	c->portnum    = (*env)->GetIntField(env, cur, cfid.portnum);
	c->baud    = (*env)->GetIntField(env, cur, cfid.baud);
	c->dialing_method    = (*env)->GetIntField(env, cur, cfid.dialingMethod);

PRINTF(("gof:\n"));
	/* Get the string fields. */
	js = (jstring) (*env)->GetObjectField(env, cur, cfid.phonenum);
	if (!js)
		return NULL;
PRINTF(("getc:\n"));
	if (!getCstring(env, js, c->phonenum, mytemp_STR_LEN))
		return NULL;

PRINTF(("gof 2:\n"));
	js = (jstring) (*env)->GetObjectField(env, cur, cfid.modeministr);
	if (!js) return NULL;
PRINTF(("getc 2:\n"));
	if (!getCstring(env, js, c->modeministr, mytemp_STR_LEN)) {
PRINTF(("can't get password\n"));
		return NULL;
	}

PRINTF(("done:\n"));
	return c;
}

/*
 * Class:     Activision_ANet
 * Method:    create
 * Signature: (LActivision/ANet$ANetTransport;LActivision/ANet$commInitReq;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_create
  (JNIEnv *env, jobject jANet, jobject transport, jobject params, jstring jthawfname)
{
	jclass hANetTransport; 
	jfieldID fidPath;
	jsize len;
	jstring jpath;
	dp_transport_t cpath;
	char cthawfname[256];
	char *pthawfname;
	commInitReq_t cparams;
	commInitReq_t *pparams;
	char phonenum[mytemp_STR_LEN];
	char modeministr[mytemp_STR_LEN];
	dp_result_t err;

	DPRINT(("ANet_create:\n"));
	/* Get access to path field of transport object */
	hANetTransport = (*env)->GetObjectClass(env, transport); 
	if (!hANetTransport) return dp_RES_BAD;
	fidPath = (*env)->GetFieldID(env, hANetTransport, "path", "Ljava/lang/String;"); 
	if (!fidPath) return dp_RES_BAD;
	jpath = (*env)->GetObjectField(env, transport, fidPath); 
	if (!jpath) return dp_RES_BAD;

	/* Turn it into a null-terminated UTF string */
	memset(&cpath, 0, sizeof(cpath));
	if (!getCstring(env, jpath, cpath.fname, sizeof(cpath.fname)))
		return dp_RES_BAD;

	pthawfname = NULL;
	if (jthawfname && getCstring(env, jthawfname, cthawfname, sizeof(cthawfname)))
		pthawfname = cthawfname;

	pparams = NULL;
	if (params) {
		/* Initialize the field id structure for commInitReq */
		jclass hc = commInitReq_init(env);
		if (!hc)
			return dp_RES_BUG;

		memset(&cparams, 0, sizeof(cparams));
		phonenum[0] = '\0';
		modeministr[0] = '\0';
		cparams.phonenum = phonenum;
		cparams.modeministr = modeministr;
		if (!commInitReq_JavatoC(env, params, &cparams)) {
			DPRINT(("create: can't convert commInitReq\n"));
			return dp_RES_BUG;
		}
		/* kludge -- set the sessionId here */
#ifdef WIN32
		cparams.sessionId = GetTickCount();
#else
		cparams.sessionId = rand() | (rand() << 16) | time(0) | eclock();
#endif
		cparams.hwirq = 12345;   //enable stopping dial/answer
		cparams.swint = (long)&stop;
		stop = 0;
		pparams = &cparams;
	}

	//PRINTF(("path: %s; thawfname: %s\n, params: %s", cpath, pthawfname ? pthawfname : "(null)", pparams ? pparams : "(null)"));
	err = dpCreate(&myDP, &cpath, pparams, pthawfname);
	return err;
}

/*
 * Class:     Activision_ANet
 * Method:    destroy
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_destroy
  (JNIEnv *env, jobject jANet, jint flags)
{
	int err = dpDestroy(myDP, flags);
	myDP = NULL;
	return dp_RES_BAD;
}

/*
 * Class:     Activision_ANet
 * Method:    stopDialAnswer
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_Activision_ANet_stopDialAnswer
  (JNIEnv *env, jobject jANet)
{
	DPRINT(("ANet_stopDialAnswer:\n"));
	stop = 1; //causes modem comm driver to stop dial/answer
}

/*
 * Class:     Activision_ANet
 * Method:    setDpPtr
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_Activision_ANet_setDpPtr
  (JNIEnv *env, jobject jANet, jlong dpPtr)
{
	DPRINT(("ANet_setDpPtr:\n"));
	myDP = (dp_t *) dpPtr;
}

/*------------------ Activision.ANet.commPortName --------------------------*/

/*
 * Class:     Activision_ANet
 * Method:    enumPorts
 * Signature: (Ljava/lang/String;)Ljava/util/Vector;
 */
JNIEXPORT jobject JNICALL Java_Activision_ANet_enumPorts
  (JNIEnv *env, jclass hANet, jstring transname)
{
	dp_transport_t transport;
	commPortName_t ports[5];
	int nports, n, err;

	jmethodID jmPort;		/* Method to construct commPortName() */
	jclass jclPort;			/* Class of commPortName */
	jfieldID name;			/* Name field of commPortName */
	jfieldID portnum;		/* Portnum field of commPortName */
	jclass jclVec;
	jmethodID jmVec;		/* method to construct vector */
	jobject vec;			/* Vector to be returned by enumPorts */
	jmethodID jmVecAdd;		/* Method: Vector.addElement */
	jobject Port;

	DPRINT(("enumPorts:\n"));

	if (!transname ||
		!getCstring(env, transname, transport.fname, sizeof(transport.fname)))
		return NULL;
	memset(ports, 0, 5 * sizeof(commPortName_t));
	err = dpEnumPorts(&transport, ports, 5, &nports);
	DPRINT(("dpEnumPorts returns %d, nports %d\n", err, nports));
	if (err != dp_RES_OK)
		return NULL;

	if (!(jclVec = (*env)->FindClass(env, "java/util/Vector")) ||
		!(jmVec = (*env)->GetMethodID(env, jclVec, "<init>", "()V")) ||
		!(jmVecAdd = (*env)->GetMethodID(env, jclVec, "addElement", "(Ljava/lang/Object;)V")) ||
		!(vec = (*env)->NewObject(env, jclVec, jmVec)) ||
		!(jclPort = (*env)->FindClass(env,"Activision/ANet$commPortName")) ||
		!(jmPort = (*env)->GetMethodID(env,jclPort,"<init>","()V")) ||
		!(name = (*env)->GetFieldID(env,jclPort,"name","Ljava/lang/String;")) ||
		!(portnum = (*env)->GetFieldID(env,jclPort,"portnum","I")))
		return NULL;

	for (n = 0; n < nports; n++) {
		Port = (*env)->NewObject(env, jclPort, jmPort);
		(*env)->SetObjectField(env, Port, name, (*env)->NewStringUTF(env, ports[n].name));
    	(*env)->SetIntField(env, Port, portnum, ports[n].portnum);
		(*env)->CallVoidMethod(env, vec, jmVecAdd, Port);
	}

	return vec;
}


/*------------------ Activision.ANet.ANetTable --------------------------*/
#ifdef dp_ANET2

/* Temporary - at the moment, must reinitialize this for each call.
 * One of these days, we'll figure out how to cache them.
 */
static struct {
	jfieldID keyId;
} tfid;

/* Initializes the key field id.  Returns key field id on success, null on
 * failure.
 */
static jclass table_init(JNIEnv *env)
{
	jclass hctable;

	/* Get handle to result class */
	hctable = (*env)->FindClass(env, "Activision/ANet$ANetTable");
	if (!hctable) return NULL;

	tfid.keyId = (*env)->GetFieldID(env, hctable, "key", "[B");
	if (!tfid.keyId) return NULL;
	return hctable;
}

static playerHdl_t dpid2commHdl(dpid_t id)
{
	playerHdl_t *pH = NULL;
	dpid_t firstId;
	char *p;
	int i;
	assoctab_t *tab;
	assoctab_item_t *pi;

	if (myDP == NULL) return PLAYER_NONE;
	if (!myDP->dpid2commHdl) {
		DPRINT(("java dpid2commHdl: no table!\n"));
		return PLAYER_NONE;
	}
	firstId = (dpid_t) (id & ~(dp_PLAYERS_PER_HOST-1));	/*Skip back to firstId*/
	tab = myDP->dpid2commHdl;
	for (i = 0, p=(char *)tab->buf; i<tab->n_used; i++, p+=tab->unit) {
		pi = (assoctab_item_t *)p;
		if (pi->key == firstId) {
			pH = (playerHdl_t *) pi->value;
			break;
		}
	}
	if (!pH) return PLAYER_NONE;
	return *pH;
}

/* Get dptab_table_t associated with java table.
 * Returns table on success, NULL on failure.
 */
static dptab_table_t *getTable(JNIEnv *env, jobject cur)
{
	char *ckey;
	jbyteArray key;
	jint keylen;
	dptab_table_t *tab;
	if (myDP == NULL) return NULL;
	if (!(table_init(env))) return NULL;
	key = (jbyteArray)((*env)->GetObjectField(env, cur, tfid.keyId));
	keylen = (*env)->GetArrayLength(env, key);
	if (!keylen) return NULL;
	if ((ckey = (*env)->GetByteArrayElements(env, key, NULL)) == NULL)
		return NULL;
	tab = dptab_getTable(myDP->dt, ckey, (int)keylen);
	(*env)->ReleaseByteArrayElements(env, key, ckey, 0);
	return tab;
}

/** Context for a join_sess callback */
typedef struct s_tableContext {
    JNIEnv *env;
    jobject gtab;
} t_tableContext;

t_tableContext tcontext;

/*-------------------------------------------------------------------------
 Callback triggered when table variable is set, deleted or sent.
-------------------------------------------------------------------------*/
static int dp_PASCAL table_cb(
	dptab_t *dptab, dptab_table_t *table, playerHdl_t src, playerHdl_t dest,
	char *subkey, int subkeylen, void *buf, size_t sent, size_t total,
	int seconds_left, void *context, dp_result_t status)
{
	jmethodID cb_methodID;
	jobject gtab = ((t_tableContext *)context)->gtab;
    JNIEnv *callback_env = ((t_tableContext *)context)->env;
	jclass hcTableListener;
	jclass hcTable;
	jfieldID jfid_listener;
	jobject jlistener;
	jbyteArray jsubkey;

DPRINT(("JTcb0\n"));
	if (!callback_env)
		return dp_RES_BUG;

DPRINT(("JTcb1\n"));
	/* Get the class of the thing containing the callback object... */
	hcTable = table_init(callback_env);
	if (!hcTable)
		return dp_RES_BUG;

DPRINT(("JTcb2\n"));
	/* Get the id of the field within the class... */
	jfid_listener = (*callback_env)->GetFieldID(callback_env, hcTable, "listener", "LActivision/ANetTableListener;"); 
	if (!jfid_listener) return dp_RES_BUG;

DPRINT(("JTcb3\n"));
	/* Get the value of the field of the table object... */
	jlistener = (*callback_env)->GetObjectField(callback_env, gtab, jfid_listener);
	if (!jlistener) return dp_RES_BUG;

DPRINT(("JTcb4\n"));
	/* Get the class of the callback object itself */
	hcTableListener = (*callback_env)->GetObjectClass(callback_env, jlistener); 
	if (!hcTableListener ) return dp_RES_BAD;

DPRINT(("JTcb5\n"));
	if (status == dp_RES_OK) {
		cb_methodID = (*callback_env)->GetMethodID(callback_env, hcTableListener, "tableVarSet", "(LActivision/ANet$ANetTable;[B)V");
		if (!cb_methodID)
			return FALSE;
	} else if (status == dp_RES_DELETED) {
		cb_methodID = (*callback_env)->GetMethodID(callback_env, hcTableListener, "tableVarDeleted", "(LActivision/ANet$ANetTable;[B)V");
		if (!cb_methodID)
			return FALSE;
	} else if (status == dp_RES_FINISHED) {
		cb_methodID = (*callback_env)->GetMethodID(callback_env, hcTableListener, "tableVarSent", "(LActivision/ANet$ANetTable;[B)V");
		if (!cb_methodID)
			return FALSE;
	} else {
		return FALSE;
	}

DPRINT(("JTcb6\n"));
	jsubkey = (*callback_env)->NewByteArray(callback_env, (jint)subkeylen);
	if (!jsubkey)
		return FALSE;
DPRINT(("JTcb7\n"));
	(*callback_env)->SetByteArrayRegion(callback_env, jsubkey, 0, subkeylen, subkey);
DPRINT(("JTcb8\n"));
	(*callback_env)->CallVoidMethod(callback_env, jlistener, cb_methodID, gtab, jsubkey);

DPRINT(("JTcb9\n"));
	return FALSE;
}

JNIEXPORT jint JNICALL Java_Activision_ANet_00024ANetTable_getTable
  (JNIEnv *env, jobject jobj, jboolean fCreate)
{
	if (!myDP) 
		return dp_RES_CLOSED;
	if (!fCreate)  {
		if (getTable(env, jobj) == NULL)
			return dp_RES_BAD;
	} else {
		dp_result_t err;
		char *ckey;
		jbyteArray key;
		jint keylen;
		dptab_table_t *tab;
		if (!(table_init(env))) return dp_RES_BAD;
		key = (jbyteArray)((*env)->GetObjectField(env, jobj, tfid.keyId));
		keylen = (*env)->GetArrayLength(env, key);
		if (!keylen) return dp_RES_BAD;
		if ((ckey = (*env)->GetByteArrayElements(env, key, NULL)) == NULL)
			return dp_RES_BAD;
    	tcontext.env = env;
		tcontext.gtab = (*env)->NewGlobalRef(env, jobj);
		if (!tcontext.gtab) {
			tcontext.env = NULL;
			(*env)->ReleaseByteArrayElements(env, key, ckey, 0);
			DPRINT(("getTable: can't create global ref to callback\n"));
			return dp_RES_BUG;
		}
		err = dptab_createTable(myDP->dt, &tab, ckey, (int)keylen, 0, NULL, NULL, table_cb, &tcontext);
		(*env)->ReleaseByteArrayElements(env, key, ckey, 0);
		if (err != dp_RES_OK && err != dp_RES_ALREADY) {
			(*env)->DeleteGlobalRef(env, tcontext.gtab);
			tcontext.env = NULL;
			tcontext.gtab = NULL;
			return err;
		}
		return err;
	}
	return dp_RES_OK;
}

/*
 * Class:     Activision_ANet_00024ANetTable
 * Method:    destroyTable
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_00024ANetTable_destroyTable
  (JNIEnv *env, jobject jobj)
{
	dp_result_t err;
	char *ckey;
	jbyteArray key;
	jint keylen;
	if (!(table_init(env))) return dp_RES_BAD;
	key = (jbyteArray)((*env)->GetObjectField(env, jobj, tfid.keyId));
	keylen = (*env)->GetArrayLength(env, key);
	if (!keylen) return dp_RES_BAD;
	if ((ckey = (*env)->GetByteArrayElements(env, key, NULL)) == NULL)
		return dp_RES_BAD;
	err = dptab_deleteTable(myDP->dt, ckey, (int)keylen);
	(*env)->ReleaseByteArrayElements(env, key, ckey, 0);
	return err;
}

/*
 * Class:     Activision_ANet_00024ANetTable
 * Method:    addSubscriber
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_00024ANetTable_addSubscriber
  (JNIEnv *env, jobject jobj, jint pId)
{
	dp_result_t err;
	dptab_table_t *tab;
	playerHdl_t h;
	if (!myDP) return 0;
	if ((h = dpid2commHdl((dpid_t)pId)) == PLAYER_NONE) return dp_RES_BAD;
	if ((tab = getTable(env, jobj)) == NULL) return dp_RES_BAD;
	err = dptab_addSubscriber(myDP->dt, tab, h);
	return err;
}

/*
 * Class:     Activision_ANet_00024ANetTable
 * Method:    addPublisher
 * Signature: ([BI)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_00024ANetTable_addPublisher
  (JNIEnv *env, jobject jobj, jbyteArray pubKey, jint pId)
{
	dp_result_t err;
	dptab_table_t *tab;
	playerHdl_t h;
	char *ckey;
	jint keylen;
	if (!myDP) return 0;
	if ((h = dpid2commHdl((dpid_t)pId)) == PLAYER_NONE) return dp_RES_BAD;
	if ((tab = getTable(env, jobj)) == NULL) return dp_RES_BAD;
	ckey = (*env)->GetByteArrayElements(env, pubKey, NULL);
	keylen = (*env)->GetArrayLength(env, pubKey);
	err = dptab_addPublisher(myDP->dt, tab, ckey, keylen, h);
	(*env)->ReleaseByteArrayElements(env, pubKey, ckey, 0);
	return err;
}

/*
 * Class:     Activision_ANet_00024ANetTable
 * Method:    requestSubscription
 * Signature: ([BI)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_00024ANetTable_requestSubscription
  (JNIEnv *env, jobject jobj, jbyteArray pubKey, jint pId)
{
	dp_result_t err;
	playerHdl_t h;
	char *ckey;
	jint keylen;
	if ((h = dpid2commHdl((dpid_t)pId)) == PLAYER_NONE) return dp_RES_BAD;
	ckey = (*env)->GetByteArrayElements(env, pubKey, NULL);
	keylen = (*env)->GetArrayLength(env, pubKey);
	err = dptab_requestSubscription(myDP->dt, ckey, (int)keylen, h, NULL, NULL);
	(*env)->ReleaseByteArrayElements(env, pubKey, ckey, 0);
	return err;
}

/*
 * Class:     Activision_ANet_00024ANetTable
 * Method:    deletePublisher
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_00024ANetTable_deletePublisher
  (JNIEnv *env, jobject jobj, jbyteArray pubKey, jint pId)
{
	dp_result_t err;
	playerHdl_t h;
	char *ckey;
	jint keylen;
	if (!myDP) return 0;
	if ((h = dpid2commHdl((dpid_t)pId)) == PLAYER_NONE) return dp_RES_BAD;
	ckey = (*env)->GetByteArrayElements(env, pubKey, NULL);
	keylen = (*env)->GetArrayLength(env, pubKey);
	err = dptab_deletePublisher(myDP->dt, ckey, (int)keylen, h);
	(*env)->ReleaseByteArrayElements(env, pubKey, ckey, 0);
	return err;
}

/*
 * Class:     Activision_ANet_00024ANetTable
 * Method:    deleteSubscriber
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_00024ANetTable_deleteSubscriber
  (JNIEnv *env, jobject jobj, jint pId)
{
	dp_result_t err;
	dptab_table_t *tab;
	playerHdl_t h;
	if (!myDP) return 0;
	if ((h = dpid2commHdl((dpid_t)pId)) == PLAYER_NONE) return dp_RES_BAD;
	if ((tab = getTable(env, jobj)) == NULL) return dp_RES_BAD;
	err = dptab_deleteSubscriber(myDP->dt, tab, h);
	return err;
}

/*
 * Class:     Activision_ANet_00024ANetTable
 * Method:    getTableSize
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_00024ANetTable_getTableSize
  (JNIEnv *env, jobject jobj)
{
	dptab_table_t *tab;
	if (!myDP) 
		return 0;
	if ((tab = getTable(env, jobj)) == NULL) return 0;
	return dptab_tableSize(tab);
}

/*
 * Class:     Activision_ANet_00024ANetTable
 * Method:    setVal
 * Signature: ([BLjava/lang/Object;I)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_00024ANetTable_setVal
  (JNIEnv *env, jobject jobj, jbyteArray subkey, jobject buf, jint hops)
{
	dp_result_t err;
	int subkeylen, buflen;
	char *csubkey, *cbuf;
	dptab_table_t *tab;
	jclass jbcl;
	if (!myDP) 
		return dp_RES_CLOSED;
	jbcl = (*env)->FindClass(env, "[B");
	if (!(*env)->IsInstanceOf(env, buf, jbcl))
		return dp_RES_BAD;
	if ((tab = getTable(env, jobj)) == NULL) return dp_RES_BAD;
	subkeylen = (*env)->GetArrayLength(env, subkey);
	buflen = (*env)->GetArrayLength(env, (jbyteArray)buf);
	csubkey = (*env)->GetByteArrayElements(env, subkey, NULL);
	cbuf = (*env)->GetByteArrayElements(env, (jbyteArray)buf, NULL);
	err = dptab_set(myDP->dt, tab, csubkey, subkeylen, cbuf, buflen, (int) hops, PLAYER_ME);
	(*env)->ReleaseByteArrayElements(env, subkey, csubkey, 0);
	(*env)->ReleaseByteArrayElements(env, (jbyteArray)buf, cbuf, 0);
	return (jint)err;
}

/*
 * Class:     Activision_ANet_00024ANetTable
 * Method:    getValByKey
 * Signature: ([B)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_Activision_ANet_00024ANetTable_getValByKey
  (JNIEnv *env, jobject jobj, jbyteArray subkey)
{
	dp_result_t err;
	int subkeylen, buflen;
	char *csubkey, *cbuf;
	jbyteArray buf;
	dptab_table_t *tab;
	if (!myDP) 
		return NULL;
	if ((tab = getTable(env, jobj)) == NULL) return NULL;
	subkeylen = (*env)->GetArrayLength(env, subkey);
	csubkey = (*env)->GetByteArrayElements(env, subkey, NULL);
	err = dptab_get_bykey(tab, csubkey, subkeylen, &cbuf, &buflen);
	(*env)->ReleaseByteArrayElements(env, subkey, csubkey, 0);
	if (err != dp_RES_OK) return NULL;
	buf = (*env)->NewByteArray(env, buflen);
	(*env)->SetByteArrayRegion(env, buf, 0, buflen, cbuf);
	return buf;
}

/*
 * Class:     Activision_ANet_00024ANetTable
 * Method:    getValByIndex
 * Signature: (I[B)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_Activision_ANet_00024ANetTable_getValByIndex
  (JNIEnv *env, jobject jobj, jint index, jbyteArray subkey)
{
	dp_result_t err;
	int subkeylen, buflen;
	char *csubkey, *cbuf;
	jbyteArray buf;
	dptab_table_t *tab;
	if (!myDP) 
		return NULL;
	if ((tab = getTable(env, jobj)) == NULL) return NULL;
	csubkey = (*env)->GetByteArrayElements(env, subkey, NULL);
	err = dptab_get_byindex(tab, (int)index, &cbuf, &buflen, csubkey, &subkeylen);
	(*env)->ReleaseByteArrayElements(env, subkey, csubkey, 0);
	if (err != dp_RES_OK) return NULL;
	buf = (*env)->NewByteArray(env, buflen);
	(*env)->SetByteArrayRegion(env, buf, 0, buflen, cbuf);
	return buf;
}

/*
 * Class:     Activision_ANet_00024ANetTable
 * Method:    deleteVal
 * Signature: ([B)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_00024ANetTable_deleteVal
  (JNIEnv *env, jobject jobj, jbyteArray subkey)
{
	dp_result_t err;
	int subkeylen;
	char *csubkey;
	dptab_table_t *tab;
	if (!myDP) 
		return dp_RES_BAD;
	if ((tab = getTable(env, jobj)) == NULL) return dp_RES_EMPTY;
	subkeylen = (*env)->GetArrayLength(env, subkey);
	csubkey = (*env)->GetByteArrayElements(env, subkey, NULL);
	err = dptab_delete(myDP->dt, tab, csubkey, subkeylen);
	return err;
}
#else
/*
 * Class:     Activision_ANet_00024ANetTable
 * Method:    getTable
 * Signature: (Z)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_00024ANetTable_getTable
  (JNIEnv *env, jobject jobj, jboolean fCreate)
{
	return dp_RES_BAD;
}

/*
 * Class:     Activision_ANet_00024ANetTable
 * Method:    destroyTable
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_00024ANetTable_destroyTable
  (JNIEnv *env, jobject jobj)
{
	return dp_RES_BAD;
}

/*
 * Class:     Activision_ANet_00024ANetTable
 * Method:    addSubscriber
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_00024ANetTable_addSubscriber
  (JNIEnv *env, jobject jobj, jint pId)
{
	return dp_RES_BAD;
}

/*
 * Class:     Activision_ANet_00024ANetTable
 * Method:    addPublisher
 * Signature: ([BI)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_00024ANetTable_addPublisher
  (JNIEnv *env, jobject jobj, jbyteArray pubKey, jint pId)
{
	return dp_RES_BAD;
}

/*
 * Class:     Activision_ANet_00024ANetTable
 * Method:    requestSubscription
 * Signature: ([BI)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_00024ANetTable_requestSubscription
  (JNIEnv *env, jobject jobj, jbyteArray pubKey, jint pId)
{
	return dp_RES_BAD;
}

/*
 * Class:     Activision_ANet_00024ANetTable
 * Method:    deletePublisher
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_00024ANetTable_deletePublisher
  (JNIEnv *env, jobject jobj, jbyteArray pubKey, jint pId)
{
	return dp_RES_BAD;
}

/*
 * Class:     Activision_ANet_00024ANetTable
 * Method:    deleteSubscriber
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_00024ANetTable_deleteSubscriber
  (JNIEnv *env, jobject jobj, jint pId)
{
	return dp_RES_BAD;
}

/*
 * Class:     Activision_ANet_00024ANetTable
 * Method:    getTableSize
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_00024ANetTable_getTableSize
  (JNIEnv *env, jobject jobj)
{
	return dp_RES_BAD;
}

/*
 * Class:     Activision_ANet_00024ANetTable
 * Method:    setVal
 * Signature: ([BLjava/lang/Object;I)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_00024ANetTable_setVal
  (JNIEnv *env, jobject jobj, jbyteArray subkey, jobject buf, jint hops)
{
	return dp_RES_BAD;
}

/*
 * Class:     Activision_ANet_00024ANetTable
 * Method:    getValByKey
 * Signature: ([B)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_Activision_ANet_00024ANetTable_getValByKey
  (JNIEnv *env, jobject jobj, jbyteArray subkey)
{
	return NULL;
}

/*
 * Class:     Activision_ANet_00024ANetTable
 * Method:    getValByIndex
 * Signature: (I[B)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_Activision_ANet_00024ANetTable_getValByIndex
  (JNIEnv *env, jobject jobj, jint index, jbyteArray subkey)
{
	return NULL;
}

/*
 * Class:     Activision_ANet_00024ANetTable
 * Method:    deleteVal
 * Signature: ([B)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_00024ANetTable_deleteVal
  (JNIEnv *env, jobject jobj, jbyteArray subkey)
{
	return dp_RES_BAD;
}
#endif

/*------------------- Activision.ANet.Server --------------------------------*/

/*-------------------------------------------------------------------------
 Callback triggered by listing game servers.
-------------------------------------------------------------------------*/
int dp_PASCAL listServers_cb(const char *hostname, long roundtrip_ms,dp_serverInfo_t *server,long *pTimeout,long flags,void *context)
{
	(void) hostname;
	(void) roundtrip_ms;
	(void) *server;
	(void) *pTimeout;
	(void) flags;
	(void) context;
	return TRUE;
}

/*-------------------------------------------------------------------------
 Here's a tricky part.  
 When the Java program calls ANet.pingServers(), it calls dpEnumServers()
 without any useful callback.
 On each call to dpReceive(), dp will register ping replies into its
 internal server table.
 When the Java program calls ANet.enumServersEx(), it reaches into dp's
 internal server table and returns it in Java array form.
-------------------------------------------------------------------------*/
/*
 * Class:     Activision_ANet
 * Method:    pingServers
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_pingServers
  (JNIEnv *env, jobject jANet, jint type)
{
	return dpEnumServersEx(myDP, 3000L, type, listServers_cb, NULL);
}

/* Build a Java array from the 'good' elements of dp's internal
 * server table.
 */
/*
 * Class:     Activision_ANet
 * Method:    enumServers
 * Signature: ()[LActivision/ANet$ANetServer;
 */
JNIEXPORT jobjectArray JNICALL Java_Activision_ANet_enumServers
  (JNIEnv *env, jobject jANet)
{
	jclass jcANetServer;
	jmethodID constructor_methodID;
	jobjectArray out;
	jfieldID fid_hostname, fid_rtt_ms_avg, fid_loss_percent, fid_sesstype_players;
	int i;
	int n;

	if (!myDP) 
		return NULL;

	/* Get server class */
	jcANetServer = (*env)->FindClass(env, "Activision/ANet$ANetServer"); 
	if (!jcANetServer) {
		checkExc(env);
		return NULL;
	}

	/* Get server constructor */
	constructor_methodID = (*env)->GetMethodID(env, jcANetServer, "<init>", "()V");
	if (!constructor_methodID) {
		checkExc(env);
		return NULL;
	}

	/* Get the field id's. */
	fid_hostname = (*env)->GetFieldID(env, jcANetServer, "hostname", "Ljava/lang/String;"); 
	if (!fid_hostname) return NULL;

	fid_rtt_ms_avg = (*env)->GetFieldID(env, jcANetServer, "rtt_ms_avg", "I"); 
	if (!fid_rtt_ms_avg) return NULL;

	fid_loss_percent = (*env)->GetFieldID(env, jcANetServer, "loss_percent", "I"); 
	if (!fid_loss_percent) return NULL;

	fid_sesstype_players = (*env)->GetFieldID(env, jcANetServer, "sesstype_players", "I"); 
	if (!fid_sesstype_players) return NULL;

#ifdef dp_ANET2
{
	dp_serverInfo_t *server;

	/* Get number of servers */
	n = dptab_tableSize(myDP->serverpings);
	if (n <= 0)
		return NULL;

	/* Create array of null servers */
	out = (jobjectArray) (*env)->NewObjectArray(env, n, jcANetServer, NULL); 
	if (!out)
		return NULL;

	for (i=0; i<n; i++) {
		jobject cur;

		dp_serverInfo_t *server;
		size_t serverlen;
		char subkey[dptab_KEY_MAXLEN];
		int subkeylen;
		dp_result_t err = dptab_get_byindex(myDP->serverpings, i, (void **)&server, &serverlen, subkey, &subkeylen);
		if (err != dp_RES_OK)
			continue;

		/* Ignore ugly servers */
		if (server->loss_percent == 100) {
			continue;
		}

		/* Create the current array element */
		cur = (*env)->NewObject(env, jcANetServer, constructor_methodID);
		if (!cur)
			break;

		/* Save the two int fields. */
		(*env)->SetIntField(env, cur, fid_rtt_ms_avg, server->rtt_ms_avg);
		(*env)->SetIntField(env, cur, fid_loss_percent, server->loss_percent);
		(*env)->SetIntField(env, cur, fid_sesstype_players, server->cur_sessTypeUsers);

		/* Save the one string field. */
		(*env)->SetObjectField(env, cur, fid_hostname, (*env)->NewStringUTF(env, server->hostname)); 
		if ((*env)->ExceptionOccurred(env)) break;

		/* Then save it as the current element of the array. */
		(*env)->SetObjectArrayElement(env, out, i, cur);
	}
}
#else
{
	dp_enumserver_t *server;

	/* Get number of servers */
	n = myDP->n_servers;
	if (n <= 0)
		return NULL;


	/* Create array of null servers */
	out = (jobjectArray) (*env)->NewObjectArray(env, n, jcANetServer, NULL); 
	if (!out)
		return NULL;

	server = myDP->servers;
	for (i=0; i<n; i++) {
		jobject cur;

		/* Ignore ugly servers */
		if (server->loss_percent == 100) {
			server++;
			continue;
		}

		/* Create the current array element */
		cur = (*env)->NewObject(env, jcANetServer, constructor_methodID);
		if (!cur)
			break;

		/* Save the two int fields. */
		(*env)->SetIntField(env, cur, fid_rtt_ms_avg, server->rtt_ms_avg);
		(*env)->SetIntField(env, cur, fid_loss_percent, server->loss_percent);

		/* Save the one string field. */
		(*env)->SetObjectField(env, cur, fid_hostname, (*env)->NewStringUTF(env, server->hostname)); 
		if ((*env)->ExceptionOccurred(env)) break;

		/* Then save it as the current element of the array. */
		(*env)->SetObjectArrayElement(env, out, i, cur);
		server++;
	}
}
#endif

	return out;
}

/*
 * Class:     Activision_ANet
 * Method:    setGameServer
 * Signature: (LActivision/ANet$ANetServer;I)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_setGameServer
  (JNIEnv *env, jobject jANet, jobject jServer, jint type)
{
	jfieldID fid_hostname;
	jstring jhostname;
	jclass jcANetServer;
	char hostname[256];

	if (!myDP)
		return dp_RES_BAD;

	jcANetServer = (*env)->GetObjectClass(env, jServer);
	if (!jcANetServer) return dp_RES_BAD;
	fid_hostname = (*env)->GetFieldID(env, jcANetServer, "hostname", "Ljava/lang/String;"); 
	if (!fid_hostname) return dp_RES_BAD;

	jhostname = (jstring)(*env)->GetObjectField(env, jServer, fid_hostname); 
	if (!jhostname) return dp_RES_BAD;

	if (!getCstring(env, jhostname, hostname, sizeof(hostname)))
		return dp_RES_BAD;

#ifdef dp_MULTISESSTABLE
	return dpSetGameServerEx(myDP, hostname, type);
#else
	return dpSetGameServer(myDP, hostname);
#endif
}

/*------------------- Activision.ANet.Transport -----------------------------*/

/* Can store these things only because dpEnumTransports completes immediately */
static struct {
	JNIEnv *env;
	jclass hANetTransport;
	jmethodID constructor_methodID;
	jobjectArray out;
	jfieldID fidName, fidPath, fidNeeds, fidCapabilities;
	int n;
} et;

/*--------------------------------------------------------------------------
 Callback triggered by getting transport info.
--------------------------------------------------------------------------*/
void dp_PASCAL enumTransports_cb(const dp_transport_t *path, const comm_driverInfo_t *info, void *context)
{
	jobject cur;

	if (!path)
		return;

	if (et.env) {
		/* Create the current array element */
		cur = (*et.env)->NewObject(et.env, et.hANetTransport, et.constructor_methodID);
		if (!cur)
			return;

		/* Save the two int fields. */
		(*et.env)->SetIntField(et.env, cur, et.fidNeeds, 0xffff & info->needs);
		(*et.env)->SetIntField(et.env, cur, et.fidCapabilities, 0xffff & info->capabilities);

		/* Save the two string fields. */
		(*et.env)->SetObjectField(et.env, cur, et.fidName, (*et.env)->NewStringUTF(et.env, info->name)); 
		if ((*et.env)->ExceptionOccurred(et.env)) return;

		(*et.env)->SetObjectField(et.env, cur, et.fidPath, (*et.env)->NewStringUTF(et.env, path->fname)); 
		if ((*et.env)->ExceptionOccurred(et.env)) return;

		/* Then save it as the current element of the array. */
		(*et.env)->SetObjectArrayElement(et.env, et.out, et.n, cur);
	}
	et.n++;
}

/* Class:     Activision_ANet
 * Method:    enumTransports
 * Signature: ()[LActivision/ANet$ANetTransport;
 */
JNIEXPORT jobjectArray JNICALL Java_Activision_ANet_enumTransports
  (JNIEnv *env, jclass hANet)
{
	dp_result_t err;
	dp_transport_t dll;

	/* Get handle to result class */
	jclass hANetTransport = (*env)->FindClass(env, "Activision/ANet$ANetTransport");
	/* Count transports. */
	et.env = NULL;
	et.n = 0;
	memset(&dll, 0, sizeof(dll));
	strcpy(dll.fname, "anet");
	err = dpEnumTransports(&dll, enumTransports_cb, NULL);

	/* Get transport constructor */
	et.constructor_methodID = (*env)->GetMethodID(env, hANetTransport, "<init>", "()V");
	if (!et.constructor_methodID)
		return NULL;

	et.out = (jobjectArray) (*env)->NewObjectArray(env, et.n, hANetTransport, NULL); 
	et.env = env;
	et.hANetTransport = hANetTransport;
	et.n = 0;

	if (!et.out)
		return NULL;

	/* First, get the field id's. */
	et.fidName = (*env)->GetFieldID(env, hANetTransport, "name", "Ljava/lang/String;"); 
	if (!et.fidName) return NULL;

	et.fidPath = (*env)->GetFieldID(env, hANetTransport, "path", "Ljava/lang/String;"); 
	if (!et.fidPath) return NULL;

	et.fidNeeds = (*env)->GetFieldID(env, hANetTransport, "needs", "I"); 
	if (!et.fidNeeds) return NULL;

	et.fidCapabilities = (*env)->GetFieldID(env, hANetTransport, "capabilities", "I"); 
	if (!et.fidCapabilities) return NULL;


	/* Then have dp call the callback function once for each transport. 
	 * The callback will fill the output array.
	 */
	err = dpEnumTransports(&dll, enumTransports_cb, NULL);

	return et.out;
}


/*------------------ Activision.ANet.InstalledApp --------------------------*/

/* Temporary - at the moment, must reinitialize this for each call.
 * One of these days, we'll figure out how to cache them.
 */
static struct {
	/* Initialized by InstalledApp_init() */
	jmethodID jmApp;		/* Method to construct AppParam() */
	jclass jclApp;			/* Class of Vec element AppParam */
	jfieldID name;			/* Name field of AppParam */
	jfieldID path;			/* Path field of AppParam */
	jfieldID args;			/* Args field of AppParam */
	jfieldID cwd;			/* Cwd field of AppParam */
	jfieldID shellOpts;		/* shellOpts field of AppParam */
	jfieldID sessionType;	/* Cwd field of AppParam */
	jfieldID platform;		/* Platform field of AppParam*/
	jfieldID language;		/* Language field of AppParam*/
	jfieldID majorVersion;	/* Major version field of AppParam */
	jfieldID minorVersion;	/* Minor version field of AppParam */
	jfieldID majorLatest;	/* Major latest field of AppParam */
	jfieldID minorLatest;	/* Minor latest field of AppParam */

	/* Used only by enumInstalledApps */
	JNIEnv *env;
	jobject vec;			/* Vector to be returned by enumInstalledApps */
	jmethodID jmVecAdd;		/* Method: Vector.addElement */
} afid;

/* Initialize the field id's in afid.
 * Must call often for now.
 * Returns TRUE on success, FALSE on failure.
 */
static int InstalledApp_init(JNIEnv *env)
{
	afid.jclApp = (*env)->FindClass(env,"Activision/ANet$InstalledApp");
	if (afid.jclApp == NULL) return FALSE;

	afid.jmApp = (*env)->GetMethodID(env,afid.jclApp,"<init>","()V");
	if (afid.jmApp == NULL) return FALSE;

	afid.name = (*env)->GetFieldID(env,afid.jclApp,"name","Ljava/lang/String;");
	if (afid.name == NULL) return FALSE;

	afid.path = (*env)->GetFieldID(env,afid.jclApp,"path","Ljava/lang/String;");
	if (afid.path == NULL) return FALSE;

	afid.args = (*env)->GetFieldID(env,afid.jclApp,"args","Ljava/lang/String;");
	if (afid.args == NULL) return FALSE;

	afid.cwd = (*env)->GetFieldID(env,afid.jclApp,"cwd","Ljava/lang/String;");
	if (afid.cwd == NULL) return FALSE;

	afid.shellOpts = (*env)->GetFieldID(env,afid.jclApp,"shellOpts","Ljava/lang/String;");
	if (afid.shellOpts == NULL) return FALSE;

	afid.sessionType = (*env)->GetFieldID(env,afid.jclApp,"sessionType","I");
	if (afid.sessionType == NULL) return FALSE;

	afid.platform = (*env)->GetFieldID(env,afid.jclApp,"platform","S");
	if (afid.platform == NULL) return FALSE;

	afid.language = (*env)->GetFieldID(env,afid.jclApp,"language","S");
	if (afid.language == NULL) return FALSE;

	afid.majorVersion = (*env)->GetFieldID(env,afid.jclApp,"majorVersion","S");
	if (afid.majorVersion == NULL) return FALSE;

	afid.minorVersion = (*env)->GetFieldID(env,afid.jclApp,"minorVersion","S");
	if (afid.minorVersion == NULL) return FALSE;

	afid.majorLatest = (*env)->GetFieldID(env,afid.jclApp,"majorLatest","S");
	if (afid.majorLatest == NULL) return FALSE;

	afid.minorLatest = (*env)->GetFieldID(env,afid.jclApp,"minorLatest","S");
	if (afid.minorLatest == NULL) return FALSE;

	return TRUE;
}


/* Convert a Java ANet.InstalledApp to a C dp_appParam_t. 
 * appParam character buffers must have size MAX_PATH.
 * Returns pointer to appParam on success, NULL on failure.
 */
static dp_appParam_t *InstalledApp_JavatoC(JNIEnv *env, jobject cur, dp_appParam_t *appParam)
{
	jstring js;

	if (!(js = (jstring) (*env)->GetObjectField(env, cur, afid.name)) ||
		!getCstring(env, js, appParam->name, MAX_PATH))
		return NULL;

	if (!(js = (jstring) (*env)->GetObjectField(env, cur, afid.path)) ||
		!getCstring(env, js, appParam->path, MAX_PATH))
		return NULL;

	if (!(js = (jstring) (*env)->GetObjectField(env, cur, afid.args)) ||
		!getCstring(env, js, appParam->args, MAX_PATH))
		return NULL;

	if (!(js = (jstring) (*env)->GetObjectField(env, cur, afid.cwd)) ||
		!getCstring(env, js, appParam->cwd, MAX_PATH))
		return NULL;

	if (!(js = (jstring) (*env)->GetObjectField(env, cur, afid.shellOpts)) ||
		!getCstring(env, js, appParam->shellOpts, MAX_PATH))
		return NULL;

	appParam->sessionType = (*env)->GetIntField(env, cur, afid.sessionType);
	appParam->platform = (*env)->GetIntField(env, cur, afid.platform);
	appParam->language = (*env)->GetIntField(env, cur, afid.language);
	appParam->current.major = (*env)->GetIntField(env, cur, afid.majorVersion);
	appParam->current.minor = (*env)->GetIntField(env, cur, afid.minorVersion);
	appParam->latest.major = (*env)->GetIntField(env, cur, afid.majorLatest);
	appParam->latest.minor = (*env)->GetIntField(env, cur, afid.minorLatest);

	return appParam;
}

/* Convert a C dp_appParam_t to a Java ANet.InstalledApp. 
 * Returns jobject on success, NULL on failure.
 */
static jobject InstalledApp_CtoJava(JNIEnv *env, jclass jclApp, dp_appParam_t *param)
{
	jobject App;

	App = (*env)->NewObject(env, jclApp, afid.jmApp);
	if (!App)
		return NULL;
    (*env)->SetObjectField(env, App, afid.name, (*env)->NewStringUTF(env, param->name));
    (*env)->SetObjectField(env, App, afid.path, (*env)->NewStringUTF(env, param->path));
    (*env)->SetObjectField(env, App, afid.args, (*env)->NewStringUTF(env, param->args));
    (*env)->SetObjectField(env, App, afid.cwd, (*env)->NewStringUTF(env, param->cwd));
    (*env)->SetObjectField(env, App, afid.shellOpts, (*env)->NewStringUTF(env, param->shellOpts));
    (*env)->SetIntField(env, App, afid.sessionType, 0xffff & param->sessionType);
    (*env)->SetIntField(env, App, afid.language, 0xffff & param->language);
    (*env)->SetIntField(env, App, afid.platform, 0xffff & param->platform);
    (*env)->SetIntField(env, App, afid.majorVersion, 0xffff & param->current.major);
    (*env)->SetIntField(env, App, afid.minorVersion, 0xffff & param->current.minor);
    (*env)->SetIntField(env, App, afid.majorLatest, 0xffff & param->latest.major);
    (*env)->SetIntField(env, App, afid.minorLatest, 0xffff & param->latest.minor);
	return App;
}

static void dp_PASCAL outputGame_cb(dp_appParam_t *param,void *context)
{
	jobject App;

	App = InstalledApp_CtoJava(afid.env, afid.jclApp, param);
	if (App)
		(*afid.env)->CallVoidMethod(afid.env, afid.vec, afid.jmVecAdd, App);
}

/*
 * Class:     Activision_ANet
 * Method:    enumInstalledApps
 * Signature: ()Ljava/util/Vector;
 */
JNIEXPORT jobject JNICALL Java_Activision_ANet_enumInstalledApps
  (JNIEnv *env, jobject jANet)
{
	int err;
	jclass jclVec;
	jmethodID jmVec;

//	if (env == NULL || jANet == NULL || myDP == NULL) return NULL;
	if (env == NULL || jANet == NULL) return NULL;

	DPRINT(("enumInstalledApps:\n"));
	if (!InstalledApp_init(env)) return NULL;

	jclVec = (*env)->FindClass(env, "java/util/Vector");
	if (jclVec == NULL) return NULL;

	jmVec = (*env)->GetMethodID(env, jclVec, "<init>", "(II)V");
	if (jmVec == NULL) return NULL;
	afid.jmVecAdd = (*env)->GetMethodID(env, jclVec, "addElement", "(Ljava/lang/Object;)V");
	if (afid.jmVecAdd == NULL) return NULL;

	afid.vec = (*env)->NewObject(env, jclVec, jmVec, 10, 5);
	if (afid.vec == NULL) return NULL;

	afid.env = env;

	err = dpEnumApp(myDP, outputGame_cb, NULL);

	if (err == dp_RES_OK)
		return afid.vec;

	return NULL;
}

/*
 * Class:     Activision_ANet_00024InstalledApp
 * Method:    execWithConsole
 * Signature: ()I
 *
 * Returns negative integer on failure.  Does not return on success.
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_00024InstalledApp_execWithConsole
  (JNIEnv *env, jobject jInstalledApp)
{
	int status;
	char name[MAX_PATH];
	char path[MAX_PATH];
	char args[MAX_PATH];
	char cwd[MAX_PATH];
	char shellOpts[MAX_PATH];
	dp_appParam_t appParam;

	DPRINT(("execWithConsole:\n"));
	if (!InstalledApp_init(env))
		return -1;
	appParam.name = &name[0];
	appParam.path = &path[0];
	appParam.args = &args[0];
	appParam.cwd =  &cwd[0];
	appParam.shellOpts = &shellOpts[0];

	if (InstalledApp_JavatoC(env, jInstalledApp, &appParam) == NULL)
		return -2;

	DPRINT(("execWithConsole: path is %s\n", appParam.path));
	/* Create the process. */
	status = dpLaunchApp(&appParam);

	if (dp_RES_OK == status || dp_RES_EMPTY == status) {
		DPRINT(("execWithConsole: process creation succeeded.\n"));
	} else {
		DPRINT(("execWithConsole: process creation failed.\n"));
		return -3;
	}

	/* Destroy the Java VM.  (Let the user do this with
	 * java.lang.System.exit(). )
	 */

	return 0;
}

/*
 * Class:     Activision_ANet_00024InstalledApp
 * Method:    checkAppVersion
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_Activision_ANet_00024InstalledApp_checkAppVersion
  (JNIEnv *env, jobject jInstalledApp)
{
	dp_result_t err;
	char name[MAX_PATH];
	char path[MAX_PATH];
	char args[MAX_PATH];
	char cwd[MAX_PATH];
	char shellOpts[MAX_PATH];
	dp_appParam_t app;

	if (!myDP || !InstalledApp_init(env)) return FALSE;
	app.name = &name[0];
	app.path = &path[0];
	app.args = &args[0];
	app.cwd =  &cwd[0];
	app.shellOpts = &shellOpts[0];

	if (InstalledApp_JavatoC(env, jInstalledApp, &app) == NULL) {
		DPRINT(("checkAppVersion: err: InstalledApp_JavatoC returns NULL\n"));
		return FALSE;
	}
	err = dpCheckAppVersion(myDP, &app);
    (*env)->SetIntField(env, jInstalledApp, afid.majorLatest, 0xffff & app.latest.major);
    (*env)->SetIntField(env, jInstalledApp, afid.minorLatest, 0xffff & app.latest.minor);

	if (err == dp_RES_OK)
		return TRUE;
	return FALSE;
}

/*
 * Class:     Activision_ANet_00024InstalledApp
 * Method:    downloadPatch
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_Activision_ANet_00024InstalledApp_downloadPatch
  (JNIEnv *env, jobject jInstalledApp)
{
	dp_result_t err;
	char name[MAX_PATH];
	char path[MAX_PATH];
	char args[MAX_PATH];
	char cwd[MAX_PATH];
	char shellOpts[MAX_PATH];
	dp_appParam_t app;

	if (!myDP || !InstalledApp_init(env)) return FALSE;
	app.name = &name[0];
	app.path = &path[0];
	app.args = &args[0];
	app.cwd =  &cwd[0];
	app.shellOpts = &shellOpts[0];

	if (InstalledApp_JavatoC(env, jInstalledApp, &app) == NULL) {
		DPRINT(("downloadPatch: err: InstalledApp_JavatoC returns NULL\n"));
		return FALSE;
	}

	if ((err = dpDownloadUpdate(myDP, &app)) == dp_RES_OK) {
		return TRUE;
	} else {
		DPRINT(("downloadPatch: err: dpDownloadUpdate returns %d\n", err));
	}
	
	return FALSE;
}

/*------------------ Activision.ANet.Session --------------------------*/

/* Temporary - at the moment, must reinitialize this for each call.
 * One of these days, we'll figure out how to cache them.
 */
static struct {
	jmethodID hmSessionConstructor;

	jfieldID name;
	jfieldID password;
	jfieldID type;
	jfieldID maxPlayers;
	jfieldID curPlayers;

	jfieldID karma;
	jfieldID adrMaster;
#ifdef dp_ANET2
	jfieldID reserved2;
#endif
	jfieldID userField;
	jfieldID dwUser1;
	jfieldID flags;
} sfid;

/* Can store these things only because dpEnumSession (timeout=0) completes immediately */
static struct {
	int n;
	int nSessions;
	jobjectArray out;
	jclass hcSession;
} es;

/* Initialize the field id's in sfid for use by Session_CtoJava() and
 * Session_JavatoC().  Must call often for now.
 * Returns hcSession on success, NULL on failure.
 */
static jclass Session_init(JNIEnv *env)
{
	jclass hcSession;

	DPRINT(("Session_init:\n"));
	/* Get handle to result class */
	hcSession = (*env)->FindClass(env, "Activision/ANet$Session");
	if (!hcSession) return NULL;

	/* Get session constructor */
	sfid.hmSessionConstructor = (*env)->GetMethodID(env, hcSession, "<init>", "()V");
	if (!sfid.hmSessionConstructor)
		return NULL;

	/* Get field id's.  Later, perhaps we can cache these. */
	sfid.name       = (*env)->GetFieldID(env, hcSession, "name", "Ljava/lang/String;"); 
	if (!sfid.name) return NULL;
	sfid.password   = (*env)->GetFieldID(env, hcSession, "password", "Ljava/lang/String;"); 
	if (!sfid.password) return NULL;

	sfid.type       = (*env)->GetFieldID(env, hcSession, "type", "I"); 
	if (!sfid.type) return NULL;
	sfid.maxPlayers = (*env)->GetFieldID(env, hcSession, "maxPlayers", "I"); 
	if (!sfid.maxPlayers) return NULL;
	sfid.curPlayers = (*env)->GetFieldID(env, hcSession, "curPlayers", "I"); 
	if (!sfid.curPlayers) return NULL;
	sfid.karma      = (*env)->GetFieldID(env, hcSession, "karma", "I"); 
	if (!sfid.karma) return NULL;
	sfid.userField  = (*env)->GetFieldID(env, hcSession, "userField", "[B"); 
	if (!sfid.userField) return NULL;
	sfid.dwUser1    = (*env)->GetFieldID(env, hcSession, "dwUser1", "I"); 
	if (!sfid.dwUser1) return NULL;
	sfid.flags      = (*env)->GetFieldID(env, hcSession, "flags", "I"); 
	if (!sfid.flags) return NULL;
	sfid.adrMaster  = (*env)->GetFieldID(env, hcSession, "adrMaster", "[B"); 
	if (!sfid.adrMaster) return NULL;
#ifdef dp_ANET2
	sfid.reserved2  = (*env)->GetFieldID(env, hcSession, "reserved2", "[B"); 
	if (!sfid.reserved2) return NULL;
#endif

	return hcSession;
}

/* Convert a C dp_session_t to a Java ANet.Session. 
 * Returns jobject on success, NULL on failure.
 */
static jobject Session_CtoJava(JNIEnv *env, jclass hcSession, dp_session_t *s)
{
	jbyteArray arr;
	jobject cur;
	char *szPassword;

	/* Create the current object */
	cur = (*env)->NewObject(env, hcSession, sfid.hmSessionConstructor);
	if (!cur)
		return NULL;

	/* Save the int fields. */
	(*env)->SetIntField(env, cur, sfid.type,       0xffff & s->sessionType);
	(*env)->SetIntField(env, cur, sfid.maxPlayers, 0xffff & s->maxPlayers);
	(*env)->SetIntField(env, cur, sfid.curPlayers, 0xffff & s->currentPlayers);
	(*env)->SetIntField(env, cur, sfid.karma,      0xffff & s->karma);
	(*env)->SetIntField(env, cur, sfid.dwUser1,    0xffff & s->dwUser1);
	(*env)->SetIntField(env, cur, sfid.flags,      0xffff & s->flags);

	/* Save the string fields.  Forcably null-terminate them; hope that's
	 * right.  It seems to be so for passwords, anyway.
	 */
	s->sessionName[sizeof(s->sessionName)-1] = 0;
	(*env)->SetObjectField(env, cur, sfid.name, (*env)->NewStringUTF(env, s->sessionName)); 
	if ((*env)->ExceptionOccurred(env)) return NULL;

	s->szPassword[sizeof(s->szPassword)-1] = 0;
	szPassword = s->szPassword;
	(*env)->SetObjectField(env, cur, sfid.password, (*env)->NewStringUTF(env, szPassword)); 
	if ((*env)->ExceptionOccurred(env)) return NULL;

	/* Save the byte array fields. */
	arr = (*env)->NewByteArray(env, sizeof(s->szUserField));
	if (!arr) return NULL;
	(*env)->SetByteArrayRegion(env, arr, 0, sizeof(s->szUserField), s->szUserField);
	if (checkExc(env))
		return NULL;
	(*env)->SetObjectField(env, cur, sfid.userField, arr); 
	if ((*env)->ExceptionOccurred(env)) return NULL;

	arr = (*env)->NewByteArray(env, sizeof(s->adrMaster));
	if (!arr) return NULL;
	(*env)->SetByteArrayRegion(env, arr, 0, sizeof(s->adrMaster), s->adrMaster);
	if (checkExc(env))
		return NULL;
	(*env)->SetObjectField(env, cur, sfid.adrMaster, arr); 
	if ((*env)->ExceptionOccurred(env)) return NULL;

#ifdef dp_ANET2
	arr = (*env)->NewByteArray(env, sizeof(s->reserved2));
	if (!arr) return NULL;
	(*env)->SetByteArrayRegion(env, arr, 0, sizeof(s->reserved2), s->reserved2);
	if (checkExc(env))
		return NULL;
	(*env)->SetObjectField(env, cur, sfid.reserved2, arr); 
	if ((*env)->ExceptionOccurred(env)) return NULL;
#endif

	return cur;
}

/* Convert a Java ANet.Session to a C dp_session_t. 
 * Returns pointer to buf on success, NULL on failure.
 */
static dp_session_t *Session_JavatoC(JNIEnv *env, jobject cur, dp_session_t *s)
{
	jbyteArray arr;
	jstring js;

	memset(s, 0, sizeof(*s));
PRINTF(("jtoc:\n"));
	/* Get the int fields. */
	s->sessionType    = (*env)->GetIntField(env, cur, sfid.type);
	s->maxPlayers     = (*env)->GetIntField(env, cur, sfid.maxPlayers);
	s->currentPlayers = (*env)->GetIntField(env, cur, sfid.curPlayers);
	s->karma          = (*env)->GetIntField(env, cur, sfid.karma);
	s->dwUser1        = (*env)->GetIntField(env, cur, sfid.dwUser1);
	s->flags          = (*env)->GetIntField(env, cur, sfid.flags);

PRINTF(("gof:\n"));
	/* Get the string fields. */
	js = (jstring) (*env)->GetObjectField(env, cur, sfid.name);
	if (!js)
		return NULL;
PRINTF(("getc:\n"));
	if (!getCstring(env, js, s->sessionName, sizeof(s->sessionName)))
		return NULL;

PRINTF(("gof 2:\n"));
	js = (jstring) (*env)->GetObjectField(env, cur, sfid.password);
	if (!js) return NULL;
PRINTF(("getc 2:\n"));
	if (!getCstring(env, js, s->szPassword, sizeof(s->szPassword))) {
PRINTF(("can't get password\n"));
		return NULL;
	}

PRINTF(("gof 3:\n"));
	/* Get the byte array fields. */
	arr = (*env)->GetObjectField(env, cur, sfid.userField);
	if (!arr) return NULL;
PRINTF(("getbar:\n"));
	(*env)->GetByteArrayRegion(env, arr, 0, sizeof(s->szUserField), s->szUserField);
	if (checkExc(env))
		return NULL;

PRINTF(("gof 4:\n"));
	arr = (*env)->GetObjectField(env, cur, sfid.adrMaster);
	if (!arr) return NULL;
PRINTF(("getbar2:\n"));
	(*env)->GetByteArrayRegion(env, arr, 0, sizeof(s->adrMaster), s->adrMaster);
	if (checkExc(env))
		return NULL;

#ifdef dp_ANET2
PRINTF(("gof 5:\n"));
	arr = (*env)->GetObjectField(env, cur, sfid.reserved2);
	if (!arr) return NULL;
PRINTF(("getbar5:\n"));
	(*env)->GetByteArrayRegion(env, arr, 0, sizeof(s->reserved2), s->reserved2);
	if (checkExc(env))
		return NULL;
#endif

PRINTF(("done:\n"));
	return s;
}

/*--------------------------------------------------------------------------
 Callback triggered by getting player info.
--------------------------------------------------------------------------*/
int dp_PASCAL enumSessions_cb(dp_session_t *sess, long *pTimeout, long flags, void *context)
{
    JNIEnv *callback_env = (JNIEnv *)context;
	if (!myDP || !sess) return 0;

	if ((es.n < es.nSessions) && callback_env) { 
		jobject cur;

		/* Create the current array element */
		cur = Session_CtoJava(callback_env, es.hcSession, sess);
		if (cur) {
			/* Then save it as the current element of the array. */
			(*callback_env)->SetObjectArrayElement(callback_env, es.out, es.n, cur);
		}
	}
	es.n++;
	return 0;
}

int dp_PASCAL countSessions_cb(dp_session_t *sess, long *pTimeout, long flags, void *context)
{
	if (sess)
		es.nSessions++;
	return 0;
}

/*
 * Class:     Activision_ANet
 * Method:    enumSessions
 * Signature: (I)[LActivision/ANet$Session;
 */
JNIEXPORT jobjectArray JNICALL Java_Activision_ANet_enumSessions
  (JNIEnv *env, jobject jANet, jint sessType)
{
	int i;
	dp_session_t sess;
	dp_result_t err;

	DPRINT(("enumSessions:\n"));
	if (!myDP) return NULL;

	sess.sessionType = sessType;

	/* Count sessions. */
	/*callback_env = env;*/
	es.nSessions = 0;
	err = dpEnumSessions(myDP, &sess, NULL, 0L, countSessions_cb, env);
	if (err != dp_RES_OK)
		return NULL;

	/* Initialize the field id structure for Session */
	es.hcSession = Session_init(env);
	if (!es.hcSession)
		return NULL;

	/* Create the result array */
	es.out = (jobjectArray) (*env)->NewObjectArray(env, es.nSessions, es.hcSession, NULL);
	if (!es.out)
		return NULL;

	es.n = 0;
	err = dpEnumSessions(myDP, &sess, NULL, 0L, enumSessions_cb, env);
	/*callback_env = NULL;*/
	if (err != dp_RES_OK)
		return NULL;

	return (es.n == es.nSessions ? es.out : NULL);
}

/*
 * Class:     Activision_ANet
 * Method:    getSessionDesc
 * Signature: ()LActivision/ANet$Session;
 */
JNIEXPORT jobject JNICALL Java_Activision_ANet_getSessionDesc
  (JNIEnv *env, jobject jANet)
{
	dp_session_t sess;
	dp_result_t err;
    size_t sessSize = sizeof(dp_session_t);

    /* Initialize the field id structure for Session */
	es.hcSession = Session_init(env);
	if (!es.hcSession)
		return NULL;

    /* Get the current session description */
    err = dpGetSessionDesc(myDP, &sess, &sessSize);
	if (err != dp_RES_OK)
		return NULL;

    /* Convert and return the session description */
    return Session_CtoJava(env, es.hcSession, &sess);
}

/*
 * Class:     Activision_ANet
 * Method:    setSessionDesc
 * Signature: (LActivision/ANet$Session;)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_setSessionDesc
  (JNIEnv *env, jobject jANet, jobject jSession)
{
	dp_session_t sess;

    /* Convert the session description */
    if (NULL == Session_JavatoC(env, jSession, &sess))
        return dp_RES_BUG;

    /* Set the session description (3rd parameter, "flags", is unused) */
    return dpSetSessionDesc(myDP, &sess, 0L);
}


/*
 * Class:     Activision_ANet
 * Method:    enableNewPlayers
 * Signature: (Z)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_enableNewPlayers
  (JNIEnv *env, jobject jANet, jboolean enable)
{
    return dpEnableNewPlayers(myDP, enable);
}


/** Context for a join_sess callback */
typedef struct s_openContext {
    JNIEnv *env;
    jobject gjcb;
} t_openContext;

t_openContext context;


/*-------------------------------------------------------------------------
 Callback triggered by dpOpen when joining a session.
-------------------------------------------------------------------------*/
static int dp_PASCAL join_sess_cb(dp_session_t *ps, long *pTimeout, long flags, void *context) 
{
	jmethodID cb_methodID ;
	jobject gjcb = ((t_openContext *)context)->gjcb;
    JNIEnv *callback_env = ((t_openContext *)context)->env;
	jclass hcSessionListener;
	jclass hcSession;

PRINTF(("AA:\n"));
	if (!callback_env)
		return FALSE;

PRINTF(("Ab:\n"));
	/* Initialize the field id structure for Session */
	hcSession = Session_init(callback_env);
	if (!hcSession)
		return FALSE;

PRINTF(("Ac:\n"));
	hcSessionListener = (*callback_env)->GetObjectClass(callback_env, gjcb); 
	if (!hcSessionListener ) return dp_RES_BAD;

PRINTF(("Ad:\n"));
	cb_methodID = (*callback_env)->GetMethodID(callback_env, hcSessionListener, "sessionOpened", "(LActivision/ANet$Session;)V");
	if (!cb_methodID)
		return FALSE;

PRINTF(("Ae:\n"));
	if (ps) {
		/* Create the current array element */
		jobject js = Session_CtoJava(callback_env, hcSession, ps);
		if (!js)
			return FALSE;
PRINTF(("Af:\n"));
		(*callback_env)->CallVoidMethod(callback_env, gjcb, cb_methodID, js);
	} else {
PRINTF(("Ag:\n"));
		(*callback_env)->CallVoidMethod(callback_env, gjcb, cb_methodID, NULL);
	}
PRINTF(("Ah:\n"));

	(*callback_env)->DeleteGlobalRef(callback_env, gjcb);
    ((t_openContext *)context)->gjcb = NULL;
    ((t_openContext *)context)->env = NULL;
PRINTF(("Ai:\n"));

	return FALSE;
}

/*
 * Class:     Activision_ANet
 * Method:    openSession
 * Signature: (LActivision/ANet$Session;ZLActivision/ANetSessionListener;)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_openSession
  (JNIEnv *env, jobject jANet, jobject jSession, jboolean create, jobject jcb)
{
	dp_session_t s;
	dp_result_t err;

PRINTF(("openSession 1:\n"));
    context.env = env;
	context.gjcb = (*env)->NewGlobalRef(env, jcb);
	if (! context.gjcb) {
		DPRINT(("openSession: can't create global ref to callback\n"));
		return dp_RES_BUG;
	}

PRINTF(("openSession 2:\n"));
	/* Initialize the field id structure for Session */
	if (!Session_init(env)) {
		DPRINT(("openSession: can't get field ids\n"));
		return dp_RES_BUG;
	}

PRINTF(("openSession 3:\n"));
	if (!Session_JavatoC(env, jSession, &s)) {
		DPRINT(("openSession: can't convert session\n"));
		return dp_RES_BUG;
	}

    /* Set creation flag based on boolean parameter */
	if (create)
		s.flags |= dp_SESSION_FLAGS_CREATESESSION;
	else
		s.flags &= ~dp_SESSION_FLAGS_CREATESESSION;

PRINTF(("openSession 4:\n"));
	/* Callback must free the global reference */
	/*callback_env = env;*/
	err = dpOpen(myDP, &s, join_sess_cb, &context);
	/*callback_env = NULL;*/
PRINTF(("openSession 5:\n"));

	/* Save a global handle to the session */
	if (err == dp_RES_OK) {
		gj_mySession = (*env)->NewGlobalRef(env, jSession);
		if (!gj_mySession) {
			DPRINT(("openSession: can't create global ref to session\n"));
			return dp_RES_BUG;
		}
	}
PRINTF(("openSession 6:\n"));

	return err;
}

/*
 * Class:     Activision_ANet
 * Method:    closeSession
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_closeSession
  (JNIEnv *env, jobject jANet)
{
	dp_result_t err;
	
	/*callback_env = env;*/
	err = dpClose(myDP);
	/*callback_env = NULL;*/
	if (gj_mySession) {
		(*env)->DeleteGlobalRef(env, gj_mySession);
		gj_mySession = NULL;
	}
	return err;
}

/*------------------ Activision.ANet.Player --------------------------*/

/* Temporary - at the moment, must reinitialize this for each call.
 * One of these days, we'll figure out how to cache them.
 */
static struct {
	jmethodID hmPlayerConstructor;

	jfieldID name;
	jfieldID id;
	jfieldID local;
} pfid;

/* Can store these things only because dpEnumPlayers (local) completes immediately */
static struct {
	int n;
	int nPlayers;
	jclass hcPlayer;
	jobjectArray out;
} ep;

/* Initialize the field id's in pfid for use by Player_CtoJava() and
 * Player_JavatoC().  Must call often for now.
 * Returns hcPlayer on success, NULL on failure.
 */
static jclass Player_init(JNIEnv *env)
{
	jclass hcPlayer;

	/* Get handle to result class */
	hcPlayer = (*env)->FindClass(env, "Activision/ANet$Player");
	if (!hcPlayer) return NULL;

	/* Get player constructor */
	pfid.hmPlayerConstructor = (*env)->GetMethodID(env, hcPlayer, "<init>", "()V");
	if (!pfid.hmPlayerConstructor)
		return NULL;

	/* Get field id's.  Later, perhaps we can cache these. */
	pfid.name       = (*env)->GetFieldID(env, hcPlayer, "name", "Ljava/lang/String;"); 
	if (!pfid.name) return NULL;

	pfid.id         = (*env)->GetFieldID(env, hcPlayer, "id", "I"); 
	if (!pfid.id) return NULL;
	pfid.local      = (*env)->GetFieldID(env, hcPlayer, "local", "Z"); 
	if (!pfid.local) return NULL;

	return hcPlayer;
}

/* Convert a C dp_playerId_t to a Java ANet.Player. 
 * Returns jobject on success, NULL on failure.
 */
static jobject Player_CtoJava(JNIEnv *env, jclass hcPlayer, dp_playerId_t *s)
{
	jbyteArray arr;
	jobject cur;

	/* Create the current object */
	cur = (*env)->NewObject(env, hcPlayer, pfid.hmPlayerConstructor);
	if (!cur)
		return NULL;

	/* Save the int fields. */
	(*env)->SetIntField(env, cur, pfid.id, 0xffff & s->id);
	(*env)->SetIntField(env, cur, pfid.local, memcmp(s->adr, myDP->dpio->myAdr, myDP->dpio->myAdrLen) ? FALSE : TRUE);

	/* Save the string field. */
	(*env)->SetObjectField(env, cur, pfid.name, (*env)->NewStringUTF(env, s->name)); 
	if ((*env)->ExceptionOccurred(env)) return NULL;

	return cur;
}

/* Convert a Java ANet.Player to a C dp_playerId_t. 
 * Returns pointer to buf on success, NULL on failure.
 */
static dp_playerId_t *Player_JavatoC(JNIEnv *env, jobject cur, dp_playerId_t *s)
{
	jbyteArray arr;
	jstring js;

	/* Get the int field. */
	s->id    = (*env)->GetIntField(env, cur, pfid.id);

	/* Get the string field. */
	js = (jstring) (*env)->GetObjectField(env, cur, pfid.name);
	if (!js)
		return NULL;
	if (!getCstring(env, js, s->name, sizeof(s->name)))
		return NULL;

	return s;
}

/*--------------------------------------------------------------------------
 Callback triggered by getting player info.
--------------------------------------------------------------------------*/
void dp_PASCAL enumPlayers_cb(dpid_t id, dp_char_t *name, long flags, void *context)
{
	jbyteArray arr;
	jobject cur;
    JNIEnv* callback_env = (JNIEnv *) context;

	if (!myDP) return;
	if (id == dp_ID_NONE) return;
	if (ep.n >= ep.nPlayers) return;

	if (callback_env) {
		jobject cur;

		/* Create the current array element */
		cur = (*callback_env)->NewObject(callback_env, ep.hcPlayer, pfid.hmPlayerConstructor);
		if (!cur) 
			return;

		/* Save the int fields. */
		(*callback_env)->SetIntField(callback_env, cur, pfid.id, 0xffff & id);
		(*callback_env)->SetBooleanField(callback_env, cur, pfid.local,
                    (jboolean) ((flags & dp_EPC_FLAGS_LOCAL) == dp_EPC_FLAGS_LOCAL));

		/* Save the string field. */
		(*callback_env)->SetObjectField(callback_env, cur, pfid.name, (*callback_env)->NewStringUTF(callback_env, name)); 
		if ((*callback_env)->ExceptionOccurred(callback_env))
            return;
	
		/* Then save it as the current element of the array. */
		(*callback_env)->SetObjectArrayElement(callback_env, ep.out, ep.n, cur);
		ep.n++;
	}
}

/*
 * Class:     Activision_ANet
 * Method:    enumPlayers
 * Signature: ()[LActivision/ANet$Player;
 */
JNIEXPORT jobjectArray JNICALL Java_Activision_ANet_enumPlayers
  (JNIEnv *env, jobject jANet)
{
	int i;
	dp_result_t err;

	if (!myDP) return NULL;

	/* Initialize the field id structure for Player */
	ep.hcPlayer = Player_init(env);
	if (!ep.hcPlayer)
		return NULL;

	/* Count players. */
	ep.nPlayers = dpNumPlayers(myDP);

	/* Create the result array */
	ep.out = (jobjectArray) (*env)->NewObjectArray(env, ep.nPlayers, ep.hcPlayer, NULL); 
	if (!ep.out)
		return NULL;

	/*callback_env = env;*/
	ep.n = 0;
	err = dpEnumPlayers(myDP, NULL, enumPlayers_cb, env, 0L);
	if (err != dp_RES_OK)
		return NULL;
	/*callback_env = NULL;*/

	return (ep.n == ep.nPlayers ? ep.out : NULL);
}

/*-------------------------------------------------------------------------
 Callback triggered by creating a player.
-------------------------------------------------------------------------*/
int dp_PASCAL create_player_cb(dpid_t id, char_t *name, long flags, void *context)
{
	jmethodID cb_methodID;
	jclass hcPlayerListener;
	jclass hcSession;
	jfieldID jfid_listener;
	jobject jlistener;
	jobject jplayer;
    JNIEnv* callback_env = (JNIEnv *) context;

PRINTF(("Ba:\n"));
	if (!callback_env)
		return FALSE;
PRINTF(("Bb:\n"));
	if (!gj_mySession)
		return FALSE;

	/* Get the callback object, jlistener.  Should cache this! */

	/* Get the class of the thing containing the callback object... */
	hcSession = (*callback_env)->FindClass(callback_env, "Activision/ANet$Session");
	if (!hcSession) return dp_RES_BUG;

PRINTF(("Bc:\n"));
	/* Get the id of the field within the class... */
	jfid_listener = (*callback_env)->GetFieldID(callback_env, hcSession, "listener", "LActivision/ANetPlayerListener;"); 
	if (!jfid_listener) return dp_RES_BUG;
PRINTF(("Bd:\n"));

	/* Get the value of the field within the class... */
	jlistener = (*callback_env)->GetObjectField(callback_env, gj_mySession, jfid_listener);
	if (!jlistener)
		return dp_RES_BUG;
PRINTF(("Be:\n"));

	/* Get the class of the callback object itself */
	hcPlayerListener = (*callback_env)->GetObjectClass(callback_env, jlistener); 
	if (!hcPlayerListener ) return dp_RES_BAD;
PRINTF(("Bf:\n"));

	/* Get the id of the callback method within the callback class */
	cb_methodID = (*callback_env)->GetMethodID(callback_env, hcPlayerListener, "playerEnters", "(LActivision/ANet$Session;LActivision/ANet$Player;)V");
	if (!cb_methodID)
		return FALSE;
PRINTF(("Bg:\n"));

	/* Call the callback method within the callback class */
	jplayer = NULL;
	if (id != dp_ID_NONE) {
		dp_playerId_t pid;
		jclass hcPlayer;

		/* Build a player structure */
		memset(&pid, 0, sizeof(pid));
		strncpy(pid.name, name, sizeof(pid.name));
		pid.name[sizeof(pid.name)-1] = 0;
		pid.id = id;
		if (flags & dp_EPC_FLAGS_LOCAL)
			memcpy(pid.adr, myDP->dpio->myAdr, myDP->dpio->myAdrLen);

		/* Initialize the field id structure for Player */
		hcPlayer = Player_init(callback_env);
		if (!hcPlayer)
			return FALSE;
PRINTF(("Bh:\n"));

		/* Convert it to Java */
		jplayer = Player_CtoJava(callback_env, hcPlayer, &pid);
		if (!jplayer)
			return FALSE;
PRINTF(("Bi:\n"));
	}

	/* At last: call it.  Couldn't we keep the results of all the above
	 * cached?  Sure would be nice, eh?
	 */

	(*callback_env)->CallVoidMethod(callback_env, jlistener, cb_methodID, gj_mySession, jplayer);

PRINTF(("Bj:\n"));
	return TRUE;
}

/*
 * Class:     Activision_ANet_00024Session
 * Method:    createPlayer
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_00024Session_createPlayer
  (JNIEnv *env, jobject jSession, jstring jPlayerName)
{
	char name[dp_PNAMELEN];
	dp_result_t err;

PRINTF(("createPlayer: in\n"));
	if (!myDP) return dp_RES_BUG;

	if (!getCstring(env, jPlayerName, name, dp_PNAMELEN))
		return dp_RES_BAD;

	/* For now, just create the player in the current session.
	 * The only use for the Session will be for its SessionListener.
	 */
	/*callback_env = env;*/
PRINTF(("createPlayer: name is [%s]\n",name));
	err = dpCreatePlayer(myDP, create_player_cb, env, name);
	/*callback_env = NULL;*/
DPRINT(("createPlayer: returning err:%d\n", err));
	return err;
}

/*
 * Class:     Activision_ANet_00024Player
 * Method:    destroyPlayer
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_00024Player_destroyPlayer
  (JNIEnv *env, jobject jPlayer)
{
	dp_result_t err;
	dpid_t id;
	jclass hcPlayer;

PRINTF(("DP 1\n"));
	if (!myDP) return dp_RES_BUG;

PRINTF(("DP 2\n"));
	/* Initialize the field id structure for Player */
	hcPlayer = Player_init(env);

	if (!hcPlayer)
		return dp_RES_BUG;
PRINTF(("DP 3\n"));

	/* Get the id field */
	id = (*env)->GetIntField(env, jPlayer, pfid.id);
	if (!id) return dp_RES_BUG;
PRINTF(("DP 4\n"));

	/*callback_env = env;*/
	err = dpDestroyPlayer(myDP, id);
	/*callback_env = NULL;*/
PRINTF(("DP 5\n"));

	return err;
}

/*
 * Class:     Activision_ANet_00024Player
 * Method:    getLatency
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_00024Player_getLatency
  (JNIEnv *env, jobject jPlayer)
{
	dp_result_t err;
	dpid_t id;
	jclass hcPlayer;
	dp_caps_t info;

PRINTF(("DP 1\n"));
	if (!myDP) return dp_RES_BUG;

PRINTF(("DP 2\n"));
	/* Initialize the field id structure for Player */
	hcPlayer = Player_init(env);

	if (!hcPlayer)
		return dp_RES_BUG;
PRINTF(("DP 3\n"));

	/* Get the id field */
	id = (*env)->GetIntField(env, jPlayer, pfid.id);
	if (!id) return dp_RES_BUG;
PRINTF(("DP 4\n"));

	/* Retrieve the latency */
	info.dwSize = sizeof(info);
	err = dpGetPlayerCaps(myDP, id, &info, 0);
	if (err != dp_RES_OK)
		return -1;

	return info.dwLatency;
}


/*
 * Class:     Activision_ANet
 * Method:    writeObject
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_writeObject
  (JNIEnv *env, jobject jANet, jstring jThawFilename)
{
	dp_result_t err;
	char cThawFilename[256];

	if (!myDP) return dp_RES_BUG;
	if (!getCstring(env, jThawFilename, cThawFilename, sizeof(cThawFilename)))
		return dp_RES_BAD;

	err = dpFreeze(myDP, cThawFilename);

	return err;
}

/*
 * Class:     Activision_ANet
 * Method:    readyToFreeze
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_readyToFreeze
  (JNIEnv *env, jobject jANet)
{
	if (!myDP) return dp_RES_BAD;
	return dpReadyToFreeze(myDP);
}

/*------------------ Activision.ANet.Packet --------------------------*/

/* Temporary - at the moment, must reinitialize this for each call.
 * One of these days, we'll figure out how to cache them.
 */
static struct {
	jmethodID hmPacketConstructor;
	jfieldID body;
	jfieldID src;
	jfieldID dest;
	jfieldID err;
} qfid;

/* C version of Packet.  Only used here, at the moment. */
typedef struct {
	char body[dpio_MAXLEN_UNRELIABLE];
	dpid_t src;
	dpid_t dest;
	dp_result_t err;
	size_t len;
} dp_packet_t;

/* Initialize the field id's in sfid for use by Packet_CtoJava() and
 * Packet_JavatoC().  Must call often for now.
 * Returns hcPacket on success, NULL on failure.
 */
static jclass Packet_init(JNIEnv *env)
{
	jclass hcPacket;

	/* Get handle to result class */
	hcPacket = (*env)->FindClass(env, "Activision/ANet$Packet");
	if (!hcPacket) return NULL;

	/* Get packet constructor */
	qfid.hmPacketConstructor = (*env)->GetMethodID(env, hcPacket, "<init>", "()V");
	if (!qfid.hmPacketConstructor)
		return NULL;

	/* Get field id's.  Later, perhaps we can cache these. */
	qfid.body       = (*env)->GetFieldID(env, hcPacket, "body", "[B"); 
	if (!qfid.body) return NULL;
	qfid.src       = (*env)->GetFieldID(env, hcPacket, "src", "I"); 
	if (!qfid.src) return NULL;
	qfid.dest       = (*env)->GetFieldID(env, hcPacket, "dest", "I"); 
	if (!qfid.dest) return NULL;
	qfid.err       = (*env)->GetFieldID(env, hcPacket, "err", "I"); 
	if (!qfid.err) return NULL;

	return hcPacket;
}

/* Convert a C dp_packet_t to a Java ANet.Packet. 
 * Returns jobject on success, NULL on failure.
 */
static jobject Packet_CtoJava(JNIEnv *env, jclass hcPacket, dp_packet_t *pkt)
{
	jbyteArray arr;
	jobject cur;

	/* Create the current object */
	cur = (*env)->NewObject(env, hcPacket, qfid.hmPacketConstructor);
	if (!cur)
		return NULL;

	/* Save the three int fields. */
	(*env)->SetIntField(env, cur, qfid.src,  0xffff & pkt->src);
	(*env)->SetIntField(env, cur, qfid.dest, 0xffff & pkt->dest);
	(*env)->SetIntField(env, cur, qfid.err, 0xffff & pkt->err);

	/* Save the byte array field. */
	arr = (*env)->NewByteArray(env, pkt->len);
	if (!arr) return NULL;
	(*env)->SetByteArrayRegion(env, arr, 0, pkt->len, pkt->body);
	if (checkExc(env))
		return NULL;
	(*env)->SetObjectField(env, cur, qfid.body, arr); 
	if ((*env)->ExceptionOccurred(env)) return NULL;

	return cur;
}

/* Convert a Java ANet.Packet to a C packet. 
 * Returns dp_RES_OK on success.
 */
static int Packet_JavatoC(JNIEnv *env, jobject cur, dp_packet_t *pkt)
{
	jbyteArray arr;

	/* Get the three int fields. */
	pkt->src    = (*env)->GetIntField(env, cur, qfid.src);
	pkt->dest   = (*env)->GetIntField(env, cur, qfid.dest);
	pkt->err   = (*env)->GetIntField(env, cur, qfid.err);

	/* Get the byte array field. */
	arr = (*env)->GetObjectField(env, cur, qfid.body);
	if (!arr) return dp_RES_BUG;
	pkt->len = (*env)->GetArrayLength(env, arr);
	(*env)->GetByteArrayRegion(env, arr, 0, pkt->len, pkt->body);
	if (checkExc(env))
		return dp_RES_BUG;

	return dp_RES_OK;
}

/* The receive routine returns an array of packets for
 * efficiency and to shield the caller from having to know
 * about the "call until it returns empty three times" kludge.
 * A 10x speedup should be sufficient, so only buffer 10 packets.
 */
#define MAX_PKTS 10

/*
 * Class:     Activision_ANet
 * Method:    receivePackets
 * Signature: ()[LActivision/ANet$Packet;
 */
JNIEXPORT jobjectArray JNICALL Java_Activision_ANet_receivePackets
  (JNIEnv *env, jobject jANet)
{
	int i;
	int n;
	jobjectArray out;
	jclass hcPacket;
	dp_packet_t pkts[MAX_PKTS];
	dp_result_t err;

PRINTF(("receivePackets: in\n"));
	if (!myDP) return NULL;

	/* Initialize the field id structure for Packet */
	hcPacket = Packet_init(env);
	if (!hcPacket)
		return NULL;

	/*callback_env = env;*/

	/* Fetch packets.  Grab them into a local C array first. */
	for (i=0; i<MAX_PKTS; i++) {
		int j;

		/* Check for a packet.  Don't believe it right away if it
		 * tells you none are ready!
		 */
		for (j=0; j<3; j++) {
			pkts[i].len = sizeof(pkts[i].body);
			err = dpReceive(myDP, &pkts[i].src, &pkts[i].dest, 0, pkts[i].body, &pkts[i].len);
			pkts[i].err = err;
			if (err != dp_RES_EMPTY)
				break;
		}
		if (err == dp_RES_EMPTY)
			break;

		/* Record errors as if they were zero-length packets to nobody */
		if (err != dp_RES_OK) {
			/* pkts[i].err = err;  already there */
			pkts[i].len = 0;
			pkts[i].src = dp_ID_NONE;
			pkts[i].dest = dp_ID_NONE;
		}
	}
	n = i;
	/*callback_env = NULL;*/

	if (n == 0)
		return NULL;

	/* Create the result array */
	out = (jobjectArray) (*env)->NewObjectArray(env, n, hcPacket, NULL); 
	if (!out)
		return NULL;

	for (i=0; i<n; i++) {
		dp_packet_t *pkt = &pkts[i];
		jbyteArray arr;
		jobject cur;

		/* Create the current array element */
		cur = Packet_CtoJava(env, hcPacket, pkt);
		if (!cur)
			return NULL;

		/* Save it as the current element of the array. */
		(*env)->SetObjectArrayElement(env, out, i, cur);
	}
PRINTF(("receivePackets: success\n"));
	return out;
}

#if 0
void dumpBuf(char *buf, int len)
{
	int i;
	for (i=0; i<len; i++) {
		printf("%02x ", buf[i]);
	}
	DPRINT(("\n"));
}
#endif

/*
 * Class:     Activision_ANet
 * Method:    sendPacket
 * Signature: (III[BI)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_sendPacket
  (JNIEnv *env, jobject jANet, jint idSrc, jint idDest, jint flags, jbyteArray jbody, jint offset)
{
	dp_result_t err;
	size_t len;
	char buf[dpio_MAXLEN_UNRELIABLE];

	if (!myDP) return dp_RES_BUG;

	/* Get the byte array field. */
	len = (*env)->GetArrayLength(env, jbody);
	if (len > dpio_MAXLEN_UNRELIABLE)
		return dp_RES_BAD;

	(*env)->GetByteArrayRegion(env, jbody, offset, len-offset, buf);
	if (checkExc(env))
		return dp_RES_BUG;
//printf("sendPacket: len %d, buf ", len);
//dumpBuf(buf, len);

	/*callback_env = env;*/
	err = dpSend(myDP, (dpid_t) idSrc, (dpid_t) idDest, flags, buf, len-offset);
	/*callback_env = NULL;*/
	return err;
}

/*
 * Class:     Activision_ANet
 * Method:    dprint
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_Activision_ANet_dprint
  (JNIEnv *env, jclass hANet, jstring jMessage)
{
	#if defined(DPRNT) || defined(DEBUG)
		if(NULL != myDP) {
			char cMessage[2048];
			if (!getCstring(env, jMessage, cMessage, sizeof(cMessage)))
				return dp_RES_BAD;
			DPRINT((cMessage));
		}
	#endif

	return dp_RES_OK;
}

/*
 * Class:     Activision_ANet
 * Method:    getWindowsDir
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_Activision_ANet_getWindowsDir
  (JNIEnv *env, jclass hANet)
{
#if defined(_WIN32)
#include <windows.h>
#include <winbase.h>
	char buf[512];
	jstring jstr;
	if (!GetWindowsDirectory(buf, 512)) {
		DPRINT(("getWindowsDir: failed\n"));
		return NULL;
	}
	jstr = (*env)->NewStringUTF(env, buf);
	return jstr;
#else
	/* not implemented */
	return NULL;
#endif
}
