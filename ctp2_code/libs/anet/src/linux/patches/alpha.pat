Only in anet.alpha: linux86
Only in anet.alpha/src/3rdparty: d3des
Only in anet.alpha/src/3rdparty: md5
diff -ru anet/src/dp/dictset.c anet.alpha/src/dp/dictset.c
--- anet/src/dp/dictset.c	Thu Feb 25 07:00:41 1999
+++ anet.alpha/src/dp/dictset.c	Tue Feb 25 17:31:06 1997
@@ -46,7 +46,7 @@
 
 #include <assert.h>
 #include <limits.h>
-#ifdef __MWERKS__
+#if defined(linux) || defined(__MWERKS__)
     #include <string.h>
 #else
     #include <malloc.h>
diff -ru anet/src/dp/dp2.c anet.alpha/src/dp/dp2.c
--- anet/src/dp/dp2.c	Thu Feb 25 07:00:42 1999
+++ anet.alpha/src/dp/dp2.c	Tue Feb 25 17:40:06 1997
@@ -1070,7 +1070,7 @@
 	if (len > 0)
 		memcpy(p->szUserField, userbuf+4, len);	
 
-	len = buflen - ((int)q - (int)buf);
+	len = buflen - ((long)q - (long)buf);
 	if (len > 0) {
 		/* session has migrated.  Receive new master address. */
 		assert(len >= dp->dpio->myAdrLen);
@@ -1083,7 +1083,7 @@
 
 	dprint_sess(dp, p, "dp_unpack_session (new)");
 	dp_assertValid(dp);
-	return ((int)q - (int)buf);
+	return ((long)q - (long)buf);
 }
 #else
 
@@ -1274,7 +1274,7 @@
 	printAdr(myAdrLen, p->iadr);
 
 	/* Depending upon the size of this host a second address could be embeded */
-	if (length > ((int)q - (int)buf)) {
+	if (length > ((long)q - (long)buf)) {
 		memcpy(p->iadr2, q, myAdrLen);
 		q += myAdrLen;
 
@@ -1287,7 +1287,7 @@
 	p->firstId = firstId;
 	DPRINT(("\n"));
 
-	return ((int)q - (int)buf);
+	return ((long)q - (long)buf);
 }
 
 /*----------------------------------------------------------------------
@@ -1317,7 +1317,7 @@
 		printAdr(myAdrLen, p->iadr2);
 	}
 	DPRINT(("\n"));
-	return ((int)q - (int)buf);
+	return ((long)q - (long)buf);
 }
 
 /*----------------------------------------------------------------------
@@ -1400,7 +1400,7 @@
 	DPRINT(("dp_unpack_playerId(id %d): name %s\n", id, p->name));
 #endif
 
-  return ((int)q - (int)buf);
+  return ((long)q - (long)buf);
 }
 
 /*--------------------------------------------------------------------------
@@ -3916,7 +3916,7 @@
 		int i;
 		dp_serverInfo_t *server;
 		char subkey[dptab_KEY_MAXLEN];
-		int serverlen;
+		size_t serverlen;
 		int subkeylen;
 		for (i = 0; i < dptab_tableSize(dp->serverpings); i++) {
 			if (dptab_get_byindex(dp->serverpings, i, (void **)&server,
@@ -5048,7 +5048,7 @@
 		char subkey[dp_KEY_MAXLEN];
 		int subkeylen = 2;
 		char *buf;
-		int buflen;
+		size_t buflen;
 
 		host.firstId = (dpid_t) (dp_PLAYERS_PER_HOST * sp->hostid++);
 		/* Wrap dpids around at dp_MAXDPIDS back to dp_FIRST_DPID 
@@ -8095,7 +8095,7 @@
 	 */
 	sp->hostid = (highest_id / dp_PLAYERS_PER_HOST) + 2;
 	DPRINT(("dp_election_become_master: new players will start at id:%d\n", sp->hostid * dp_PLAYERS_PER_HOST));
-	{	int i; int len; int subkeylen;
+	{	int i; size_t len; size_t subkeylen;
 		char subkey[dptab_KEY_MAXLEN];
 		char *buf;
 		subkeylen = 2;
@@ -9028,7 +9028,7 @@
 	char subkey[dptab_KEY_MAXLEN];
 	char *pbuf;
 	dp_result_t err;
-	int len;
+	size_t len;
 
 	subkey[0] = dpGETLONG_FIRSTBYTE(uid);
 	subkey[1] = dpGETLONG_SECONDBYTE(uid);
@@ -9511,7 +9511,7 @@
 			char subkey[dptab_KEY_MAXLEN];
 			int subkeylen = 0;
 			char *buf;
-			int buflen;
+			size_t buflen;
 			int idoffset = (dpid_t)pkt->body.pData.id - firstId;
 			subkey[subkeylen++] = (char) dpGETSHORT_FIRSTBYTE(idoffset);
 			subkey[subkeylen++] = (char) dpGETSHORT_FIRSTBYTE(pkt->body.pData.key);
Only in anet.alpha/src/dp: tags
Only in anet.alpha/src/linux/dp: debug
Only in anet.alpha/src/linux/dp: normal