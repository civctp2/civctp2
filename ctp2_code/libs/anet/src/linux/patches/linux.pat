diff -ruN anet/demo/gtest/makefile.lin anet.dan/demo/gtest/makefile.lin
--- anet/demo/gtest/makefile.lin	Wed Mar 10 04:00:26 1999
+++ anet.dan/demo/gtest/makefile.lin	Mon Mar 15 12:39:38 1999
@@ -19,7 +19,7 @@
 	cp $(DEST) ../../../linux86/bin/$(DEST)
 
 $(DEST): $(OBJ_FILES)
-	gcc $(COPT) $(OBJ_FILES) -o $(DEST)
+	gcc -Wl,--export-dynamic $(COPT) $(OBJ_FILES) -o $(DEST) -ldl
 
 gtest.o: ../gtest.c ../../../h/dp.h
 	gcc -c $(COPT) ../gtest.c
diff -ruN anet/src/dp/dpio.c anet.dan/src/dp/dpio.c
--- anet/src/dp/dpio.c	Wed Mar 10 04:01:18 1999
+++ anet.dan/src/dp/dpio.c	Mon Mar 15 15:07:59 1999
@@ -1,5 +1,5 @@
-/* #define VERYVERBOSE */
-/* #define VERBOSE */
+ #define VERYVERBOSE 
+ #define VERBOSE 
 
 /*----------------------------------------------------------------------
  Multiplayer game networking - reliable and unreliable transport layer.
@@ -2222,7 +2222,12 @@
 	pw = &pc->tx;
 
 	/* Choose an initial packet number at random.  Hope this *is* random. */
+#ifdef UNIX
+	pw->next_pktnum = (unsigned short)((eclock()^getpid()^(long)pw)
+#else
+	/* WARNING:  This relies on srand() called by the application */
 	pw->next_pktnum = (unsigned short)((rand() + (short) (*dpio->now) + (short) time(NULL))
+#endif
 #if 0
 	/* Force an early wraparound to make sure we correctly handle case where
 	 * next_pktnum < windowBase due to wraparound of 16-bit pktnum!
@@ -3482,9 +3487,12 @@
 						if (!memcmp(srcAdr, rxPktResp.adr, dpio->myAdrLen)) {
 							DPRINT(("dpio_get: 2 Addresses in correct order !\n"));
 						} else if (!memcmp(srcAdr2, rxPktResp.adr, dpio->myAdrLen)) {
-							DPRINT(("dpio_get: 2 Addresses in wrong order !\n"));
-							srcAdr2 = srcAdr;
-							srcAdr = rxPktResp.adr;
+							DPRINT(("dpio_get: 2 Addresses in wrong order, we must be behind same NAT...\n"));
+							/* DO NOT change the order.  Must pass
+							 * the SYN's srcAdr as 1st param to openHdl.
+							 */
+							/*srcAdr2 = srcAdr;
+							srcAdr = rxPktResp.adr;*/
 						} else {
 							DPRINT(("dpio_get: 3 Addresses Detected !\n"));
 							srcAdr = rxPktResp.adr;
@@ -3517,7 +3525,9 @@
 				char *destAdr =(char*)(pDat->data+2+dpio->myAdrLen); 
 
 				if (memcmp(dpio->myAdr, destAdr, dpio->myAdrLen)) {
-					DPRINT(("dpio_get: SYN for wrong host received, ignoring\n"));
+					DPRINT(("dpio_get: SYN for wrong host "));
+					dpio_dprintAdr(destAdr, dpio->myAdrLen);
+					DPRINT((" received, ignoring\n"));
 					return dp_RES_AGAIN;
 				}
 			}
diff -ruN anet/src/dp/enumsrv2.c anet.dan/src/dp/enumsrv2.c
--- anet/src/dp/enumsrv2.c	Wed Mar 10 04:01:22 1999
+++ anet.dan/src/dp/enumsrv2.c	Mon Mar 15 17:23:05 1999
@@ -752,7 +752,7 @@
 
 	/* Figure out the round-trip time delay and store it. */
 	memcpy(&tStart, pkt->body.data, sizeof(clock_t));
-	RES(&serv)->rtt_ms[RES(&serv)->i_rtt] = (short) (dp->now - tStart);
+	RES(&serv)->rtt_ms[RES(&serv)->i_rtt] = (short) (((dp->now - tStart)*1000L)/dp->dpio->clocksPerSec);
 
 	ps = (short *)(pkt->body.data + timeAndAdrlen);
 	serv.sessType = SwapBytes2(*ps); ps++;
Binary files anet/src/dp/t.o and anet.dan/src/dp/t.o differ
diff -ruN anet/src/linux/dp/dpdll.c anet.dan/src/linux/dp/dpdll.c
--- anet/src/linux/dp/dpdll.c	Wed Mar 10 04:01:35 1999
+++ anet.dan/src/linux/dp/dpdll.c	Fri Mar 12 16:04:59 1999
@@ -6,6 +6,9 @@
  where only the UDP transport is available, and exports the symbol
  uudp_commDriverInfo.
 
+ Whittled back up to something for use with dynamically loaded transports
+ - Sam Lantinga
+
  $Log: dpdll.c $
  Revision 1.3  1997/07/28 19:42:17  lbennett
  Conform to anet.h/dpdll.h changes (dp_transport_t, etc.)
@@ -16,6 +19,8 @@
 ----------------------------------------------------------------------*/
 
 #include <stddef.h>
+#include <dirent.h>
+#include <dlfcn.h>
 
 #define DEBUG_MODULE TRUE   /* enable DPRINT in this file */
 #include "dprint.h"
@@ -23,22 +28,185 @@
 
 #include "dp.h"
 
+#ifndef DYNALINK
+  #define DYNALINK 1        // set to zero to test with static linking
+#endif
+
+
+/******************************************************************************
+
+                                    Globals
+
+*******************************************************************************/
+
+static void *hcommDLL = NULL;
+
+#if DYNALINK
+#ifdef COMM_INST
+#define COMM_3RDPARAMVAL	, commPtr
+#endif
+  // Hooks to DLL comm functions
+  static int (* pcommAlloc     ) (commAllocReq_t *,      commAllocResp_t *     ) = NULL;
+  static int (* pcommNoOp      ) (commNoOpReq_t *,       commNoOpResp_t *      
+	COMM_3RDPARAM) = NULL;
+  static int (* pcommInit      ) (commInitReq_t *,       commInitResp_t *      
+	COMM_3RDPARAM) = NULL;
+  static int (* pcommTerm      ) (commTermReq_t *,       commTermResp_t *      
+	COMM_3RDPARAM) = NULL;
+  static int (* pcommDriverInfo) (commDriverInfoReq_t *, commDriverInfoResp_t *
+	COMM_3RDPARAM) = NULL;
+  static int (* pcommPlayerInfo) (commPlayerInfoReq_t *, commPlayerInfoResp_t *
+	COMM_3RDPARAM) = NULL;
+  static int (* pcommTxFull    ) (commTxFullReq_t *,     commTxFullResp_t *    
+	COMM_3RDPARAM) = NULL;
+  static int (* pcommTxPkt     ) (commTxPktReq_t *,      commTxPktResp_t *     
+	COMM_3RDPARAM) = NULL;
+  static int (* pcommPeekPkt   ) (commPeekPktReq_t *,    commPeekPktResp_t *   
+	COMM_3RDPARAM) = NULL;
+  static int (* pcommRxPkt     ) (commRxPktReq_t *,      commRxPktResp_t *     
+	COMM_3RDPARAM) = NULL;
+  static int (* pcommScanAddr  ) (commScanAddrReq_t *,   commScanAddrResp_t *  
+	COMM_3RDPARAM) = NULL;
+  static int (* pcommPrintAddr ) (commPrintAddrReq_t *,  commPrintAddrResp_t * 
+	COMM_3RDPARAM) = NULL; 
+  static int (* pcommGroupAlloc) (commGroupAllocReq_t *, commGroupAllocResp_t *
+	COMM_3RDPARAM) = NULL;
+  static int (* pcommGroupFree ) (commGroupFreeReq_t *,  commGroupFreeResp_t * 
+	COMM_3RDPARAM) = NULL;
+  static int (* pcommGroupAdd  ) (commGroupAddReq_t *,   commGroupAddResp_t *  
+	COMM_3RDPARAM) = NULL;
+  static int (* pcommSetParam  ) (commSetParamReq_t *,   commSetParamResp_t *  
+	COMM_3RDPARAM) = NULL;
+  static int (* pcommSayHi     ) (commSayHiReq_t *,      commSayHiResp_t *     
+	COMM_3RDPARAM) = NULL;
+  static int (* pcommSayBye    ) (commSayByeReq_t *,     commSayByeResp_t *    
+	COMM_3RDPARAM) = NULL;
+
+  // structure to hook DLL exported function names to above comm function ptrs
+  typedef struct _COMMFUNC
+  {
+    char    name[32];
+    void ** procptr;
+  }
+  COMMFUNC;
+  
+  static COMMFUNC commfuncs[] =
+  {
+    {"commAlloc",      (void **) &pcommAlloc},
+    {"commNoOp",       (void **) &pcommNoOp},
+    {"commInit",       (void **) &pcommInit},
+    {"commTerm",       (void **) &pcommTerm},
+    {"commDriverInfo", (void **) &pcommDriverInfo},
+    {"commPlayerInfo", (void **) &pcommPlayerInfo},
+    {"commTxFull",     (void **) &pcommTxFull},
+    {"commTxPkt",      (void **) &pcommTxPkt},
+    {"commPeekPkt",    (void **) &pcommPeekPkt},
+    {"commRxPkt",      (void **) &pcommRxPkt},
+    {"commScanAddr",   (void **) &pcommScanAddr},
+    {"commPrintAddr",  (void **) &pcommPrintAddr},
+    {"commGroupAlloc", (void **) &pcommGroupAlloc},
+    {"commGroupFree",  (void **) &pcommGroupFree},
+    {"commGroupAdd",   (void **) &pcommGroupAdd},
+    {"commSetParam",   (void **) &pcommSetParam},
+    {"commSayHi",      (void **) &pcommSayHi},
+    {"commSayBye",     (void **) &pcommSayBye},
+  };
+
+  #define NO_COMMDLL_FUNCS  (sizeof(commfuncs) / sizeof(commfuncs[0]))
+
+
+  // public comm function stubs
+  int cdecl commAlloc(commAllocReq_t *req, commAllocResp_t *resp) {return (*pcommAlloc)(req, resp);}
+
+  int cdecl commNoOp(commNoOpReq_t *req, commNoOpResp_t *resp  COMM_3RDPARAM) {return (*pcommNoOp)(req, resp  COMM_3RDPARAMVAL);}
+  
+  int cdecl commInit(commInitReq_t *req, commInitResp_t *resp  COMM_3RDPARAM) {return (*pcommInit)(req, resp  COMM_3RDPARAMVAL);}
+  
+  int cdecl commTerm(commTermReq_t *req, commTermResp_t *resp  COMM_3RDPARAM) {return (*pcommTerm)(req, resp  COMM_3RDPARAMVAL);}
+  
+  int cdecl commDriverInfo(commDriverInfoReq_t *req, commDriverInfoResp_t *resp  COMM_3RDPARAM) {return (*pcommDriverInfo)(req, resp  COMM_3RDPARAMVAL);}
+  
+  int cdecl commPlayerInfo(commPlayerInfoReq_t *req, commPlayerInfoResp_t *resp  COMM_3RDPARAM) {return (*pcommPlayerInfo)(req, resp  COMM_3RDPARAMVAL);}
+  
+  int cdecl commTxFull(commTxFullReq_t *req, commTxFullResp_t *resp  COMM_3RDPARAM) {return (*pcommTxFull)(req, resp  COMM_3RDPARAMVAL);}
+  
+  int cdecl commTxPkt(commTxPktReq_t *req, commTxPktResp_t *resp  COMM_3RDPARAM) {return (*pcommTxPkt)(req, resp  COMM_3RDPARAMVAL);}
+  
+  int cdecl commPeekPkt(commPeekPktReq_t *req, commPeekPktResp_t *resp  COMM_3RDPARAM) {return (*pcommPeekPkt)(req, resp  COMM_3RDPARAMVAL);}
+  
+  int cdecl commRxPkt(commRxPktReq_t *req, commRxPktResp_t *resp  COMM_3RDPARAM) {return (*pcommRxPkt)(req, resp  COMM_3RDPARAMVAL);}
+  
+  int cdecl commScanAddr(commScanAddrReq_t *req, commScanAddrResp_t *resp  COMM_3RDPARAM) {return (*pcommScanAddr)(req, resp  COMM_3RDPARAMVAL);}
+  
+  int cdecl commPrintAddr(commPrintAddrReq_t *req, commPrintAddrResp_t *resp  COMM_3RDPARAM) {return (*pcommPrintAddr)(req, resp  COMM_3RDPARAMVAL);}
+  
+  int cdecl commGroupAlloc(commGroupAllocReq_t *req, commGroupAllocResp_t *resp  COMM_3RDPARAM) {return (*pcommGroupAlloc)(req, resp  COMM_3RDPARAMVAL);}
+  
+  int cdecl commGroupFree(commGroupFreeReq_t *req, commGroupFreeResp_t *resp  COMM_3RDPARAM) {return (*pcommGroupFree)(req, resp  COMM_3RDPARAMVAL);}
+  
+  int cdecl commGroupAdd(commGroupAddReq_t *req, commGroupAddResp_t *resp  COMM_3RDPARAM) {return (*pcommGroupAdd)(req, resp  COMM_3RDPARAMVAL);}
+  
+  int cdecl commSetParam(commSetParamReq_t *req, commSetParamResp_t *resp  COMM_3RDPARAM) {return (*pcommSetParam)(req, resp  COMM_3RDPARAMVAL);}
+  
+  int cdecl commSayHi(commSayHiReq_t *req, commSayHiResp_t *resp  COMM_3RDPARAM) {return (*pcommSayHi)(req, resp  COMM_3RDPARAMVAL);}
+  
+  int cdecl commSayBye(commSayByeReq_t *req, commSayByeResp_t *resp  COMM_3RDPARAM) {return (*pcommSayBye)(req, resp  COMM_3RDPARAMVAL);}
+  
+#else
+
 /* Horrible kludge */
 extern comm_driverInfo_t uudp_commDriverInfo;
 
+#endif
+
 /*----------------------------------------------------------------------
  Read description out of the DLL file.
 ----------------------------------------------------------------------*/
 dp_result_t dpGetTransportInfo(
-	dp_transport_t *path,
-	comm_driverInfo_t *info)
+	dp_transport_t *transport,
+	comm_driverInfo_t *infoptr)
 {
-	(void) path;
+#if DYNALINK
+	FILE *fp;
+	int match;
+	int c;
+
+	fp = fopen(transport->fname, "rb");
+	if (!fp) {
+		DPRINT(("dpGetTransportInfo: %s not found\n", transport->fname));
+		return dp_RES_BAD;
+	}
+	for (match=0; (match<comm_DRIVER_SIGLEN) && ((c=getc(fp)) != EOF); ) {
+		if (c == comm_DRIVER_SIGNATURE[match])
+			match++;
+		else
+			match = 0;
+	}
+	if (match != comm_DRIVER_SIGLEN) {
+		DPRINT(("dpGetTransportInfo: couldn't find signature in %s (%d)\n", transport->fname, match));
+		fclose(fp);
+		return dp_RES_EMPTY;
+	}
+	if (fread(&infoptr->recordLen, sizeof(comm_driverInfo_t)-comm_DRIVER_SIGLEN, 1, fp) != 1) {
+		DPRINT(("dpGetTransportInfo: error reading info\n"));
+		fclose(fp);
+		return dp_RES_EMPTY;
+	}
+	if (infoptr->recordLen != sizeof(comm_driverInfo_t)) {
+		DPRINT(("dpGetTransportInfo: bad recordLen %x, should be %x\n",
+			infoptr->recordLen, sizeof(comm_driverInfo_t)));
+		fclose(fp);
+		return dp_RES_BUG;
+	}
+	fclose(fp);
+#else
+	(void) transport;
 
-	*info = uudp_commDriverInfo;
+	*infoptr = uudp_commDriverInfo;
+#endif
 
 	DPRINT(("dpGetTransportInfo(%s): needs %x, capabilities %x\n",
-		path, info->needs, info->capabilities));
+		transport->fname, infoptr->needs, infoptr->capabilities));
 
 	return dp_RES_OK;
 }
@@ -49,10 +217,44 @@
  it is passed.
 ----------------------------------------------------------------------*/
 dp_result_t dpEnumTransports(
-	dp_transport_t *path,
+	dp_transport_t *tranDir,
 	dpEnumTransportCallback_t cb,
 	void *context)
 {
+#if DYNALINK
+	dp_transport_t soname;
+	char pathns[256];
+	comm_driverInfo_t info;
+	DIR *dp;
+	struct dirent *entry;
+
+	if (tranDir && tranDir->fname[0]) {
+		strcpy(pathns, tranDir->fname);
+	} else {
+		strcpy(pathns, ".");
+	}
+
+	DPRINT(("dpEnumTransports: %s/*.so\n", pathns));
+	dp = opendir(pathns);
+	if (dp == NULL)  return dp_RES_EMPTY;
+
+	// scan all DLLs found in path, get their info, and call callback funciton
+	while ( (entry=readdir(dp)) != NULL ) {
+		if ( (entry->d_name[0] == '.') || !strstr(entry->d_name, ".so") )
+			continue;
+
+		memset(&soname, 0, sizeof(soname));
+		if (tranDir && tranDir->fname[0]) 
+			sprintf(soname.fname, "%s/%s", pathns, entry->d_name);
+		else  
+			sprintf(soname.fname, "%s", entry->d_name);
+
+		if (dpGetTransportInfo(&soname, &info) != dp_RES_OK)  continue;
+
+		cb(&soname, &info, context);
+	}
+	closedir(dp);
+#else
 	comm_driverInfo_t info;
 	static dp_transport_t dummy;	/* static just in case they misbehave */
 
@@ -63,7 +265,7 @@
 	strcpy(dummy.fname, "udp");
 	dpGetTransportInfo(&dummy, &info);
 	cb(&dummy, &info, context);
-
+#endif
 	return dp_RES_OK;
 }
 
@@ -72,10 +274,35 @@
 ----------------------------------------------------------------------*/
 dp_result_t dpLoadDLL(
 	dpio_t *dpio,
-	const dp_transport_t *transportName)
+	const dp_transport_t *transport)
 {
+#if DYNALINK 
+	int i;
+
+	if (!transport)
+		return dp_RES_BUG;
+
+    if (hcommDLL) 
+		return dp_RES_FULL;   // Don't allow double loading.
+
+	hcommDLL = dlopen(transport->fname, RTLD_NOW);
+	if (!hcommDLL) {
+		return dp_RES_EMPTY;
+	}
+
+	for (i = 0; i < NO_COMMDLL_FUNCS; i++) {
+		commfuncs[i].procptr[0] = dlsym(hcommDLL, commfuncs[i].name);
+		if (!commfuncs[i].procptr[0])  {
+			dlclose(hcommDLL);
+			hcommDLL = NULL;
+			return dp_RES_BAD;
+		}
+	}
+#endif
+
 	(void) dpio;
-	(void) transportName;
+	(void) transport;
+
 	return dp_RES_OK;
 }
 
@@ -84,6 +311,12 @@
 ----------------------------------------------------------------------*/
 void dpUnloadDLL(dpio_t *dpio)
 {
+#if DYNALINK
+	if (hcommDLL)
+		dlclose(hcommDLL);
+    hcommDLL = NULL;
+#endif
+
 	(void) dpio;
 }
 
diff -ruN anet/src/linux/dp/makefile anet.dan/src/linux/dp/makefile
--- anet/src/linux/dp/makefile	Wed Mar 10 04:01:37 1999
+++ anet.dan/src/linux/dp/makefile	Fri Mar 12 15:48:04 1999
@@ -4,13 +4,13 @@
 PACK=__attribute__ ((packed))
 COPT = $(FLAGS) -DUNIX -Dcdecl="" "-D__NO_ANONYMOUS_UNIONS__" "-DPACK=$(PACK)" -Dstrnicmp=strncasecmp -Dstricmp=strcasecmp -Ddp_ANET2 -DCOMM_INST -I. -I../../../../h -I../../../aeh -I../../../../demo/utils
 CC = gcc $(COPT)
-DRIVER=uudps.o dictset.o logpkt.o
-OBJ_FILES = dp2.o dpmem.o dpdll.o dynatab.o assoctab.o dprint.o q.o ini.o \
+dp_DRIVER_OBJ=dictset.o logpkt.o
+OBJ_FILES = dp2.o dpdll.o dpmem.o dynatab.o assoctab.o dprint.o q.o ini.o \
 	dpuser.o dpio.o hkeytab.o dptab.o dpcrc32.o delay.o enumsrv2.o \
 	enumapp.o launchap.o dpexcept.o aeh.o aehmap.o aehlog.o dpprivy.o \
 	dpscore.o deltas.o tserv.o tserva.o tca.o tcapw.o crypttab.o \
 	scorerep.o mywcs.o md5.o d3des.o dpshutdn.o
-dp_OBJ_FILES = $(DRIVER) $(OBJ_FILES)
+dp_OBJ_FILES = $(OBJ_FILES)
 
 default: ../../../../linux86/lib/$(DEST)
 
@@ -19,13 +19,21 @@
 	if [ ! -d ../../../../linux86/lib ]; then mkdir ../../../../linux86/lib; fi
 	cp $(DEST) ../../../../linux86/lib
 
-$(DEST): $(dp_OBJ_FILES)
+$(DEST): $(dp_OBJ_FILES) udplan.so udpwan.so
 	-rm $(DEST)
 	ar -rsc $(DEST) $(dp_OBJ_FILES)
 
+udplan.so: $(dp_DRIVER_OBJ) udplan.o
+	$(CC) -shared -o $@ $^
+
+udpwan.so: $(dp_DRIVER_OBJ) udpwan.o
+	$(CC) -shared -o $@ $^
+
 # UDP Driver (uses 6-byte addresses)
-uudps.o: ../../uudp/uudps.c
-	$(CC) -c  ../../uudp/uudps.c -o uudps.o
+udplan.o: ../../uudp/uudps.c
+	$(CC) -c  $< -o $@ -DTCP_LAN
+udpwan.o: ../../uudp/uudps.c
+	$(CC) -c  $< -o $@
 dictset.o: ../../../dp/dictset.c
 	$(CC) -c  ../../../dp/dictset.c -o dictset.o
 logpkt.o: ../../../../demo/utils/logpkt.c
diff -ruN anet/src/linux/uudp/uudps.c anet.dan/src/linux/uudp/uudps.c
--- anet/src/linux/uudp/uudps.c	Wed Mar 10 04:01:45 1999
+++ anet.dan/src/linux/uudp/uudps.c	Mon Mar 15 16:52:21 1999
@@ -62,6 +62,8 @@
 * Constants
 */
 
+/*#define TCP_LAN  Makefile defines this for LAN, leaves alone for WAN */
+
 /* Magic Number for comm structure */
 #define COMM_MAGIC 		0x5136
 
@@ -108,7 +110,11 @@
 
 /* Second address of a peer (can assume it's his private, internal adr) */
 typedef struct uudps_peer2_s {
-	addr_t peer;  /* address; must be first field */
+	addr_t peer PACK;  /* address; must be first field */
+	enum {
+		UUDP_HDL2_NONE,
+		UUDP_HDL2_LIVE
+	} status PACK;
 } uudps_peer2_t;
 
 /* Handle type */
@@ -141,9 +147,6 @@
 * Macros
 */
 
-/* Get the handle corresponding to the address */
-#define getHandleFromAddress(d,a)	dcstFindKey((d),(a))
-
 /* Get the address corresponding to the handle */
 #define getAddressFromHandle(d,h)	((addr_t*)dcstFindValue((d),(h)))
 
@@ -162,7 +165,14 @@
 		((unsigned char *)&(adr->addr))[1] & 0xFF,
 		((unsigned char *)&(adr->addr))[2] & 0xFF,
 		((unsigned char *)&(adr->addr))[3] & 0xFF,
-		adr->port));
+		ntohs(adr->port)));
+	DPRINT((" (%02x.%02x.%02x.%02x:%02x.%02x)",
+		((unsigned char *)&(adr->addr))[0] & 0xFF,
+		((unsigned char *)&(adr->addr))[1] & 0xFF,
+		((unsigned char *)&(adr->addr))[2] & 0xFF,
+		((unsigned char *)&(adr->addr))[3] & 0xFF,
+		((unsigned char *)&(adr->port))[0] & 0xFF,
+		((unsigned char *)&(adr->port))[1] & 0xFF));
 	(void) adr;
 }
 
@@ -333,6 +343,7 @@
 	uudps_peer2_t addr2;
 	u_short port;	/* in host byte ordering */
 	comm_t	*comm = (comm_t *) commPtr;
+	int bind_error;
 
 	DPRINT(("commInit: uudps\n"));
 
@@ -363,6 +374,9 @@
 	}
 
 	/* Get requested port number */
+#ifdef TCP_LAN
+	port = UUDP_SOCKET_DEFAULT;
+#else
 	port = req->portnum;
 #if 0
 	if(0 == port)		/* For backwards compatibility */
@@ -371,6 +385,7 @@
 #endif
 	if(comm_PORT_ANY == port)
 		port = UUDP_SOCKET_ANY;
+#endif /* TCP_LAN */
 
 	/* create the datagram socket */
 	comm->uudp_sock = socket(AF_INET, SOCK_DGRAM, 0);
@@ -400,14 +415,33 @@
 	/* Set up a sockaddr to listen on port */
 	sockAddr.sin_family = AF_INET;
 	sockAddr.sin_addr.s_addr = INADDR_ANY;
-	sockAddr.sin_port = htons(port);
-	if (bind(comm->uudp_sock, (const sockaddr *)&sockAddr, sizeof(sockAddr))) {
-		DPRINT(("commInit: bind failure\n"));
-		dcstDestroy(comm->uudp_handles);
-		dcstDestroy(comm->uudp_secondary);
-		close(comm->uudp_sock);
-		resp->status = comm_STATUS_BUG;
-		return FALSE;
+#define UUDP_PORT_RANGE_START 21100
+#define UUDP_PORT_RANGE_LEN   200
+	bind_error = 0;
+	DPRINT(("commInit: portnum == %d\n", port));
+	if (UUDP_SOCKET_ANY == port) {
+		int maxtries = 3;
+		do {
+			/* If user specified 'any old port', try picking a random socket 
+			 * number inside our range.
+			 */
+			unsigned short randport = ((eclock()+((3-maxtries)*23)) % UUDP_PORT_RANGE_LEN) + UUDP_PORT_RANGE_START;
+			sockAddr.sin_port = htons(randport);
+			DPRINT(("commInit: Binding to %s:%d from range %d...%d\n", inet_ntoa(sockAddr.sin_addr), randport, UUDP_PORT_RANGE_START, UUDP_PORT_RANGE_START + UUDP_PORT_RANGE_LEN-1));
+
+			bind_error = bind(comm->uudp_sock, (const sockaddr *) &sockAddr, sizeof(sockAddr));
+		} while ( bind_error && (maxtries-- > 0) );
+	}
+	if ( (UUDP_SOCKET_ANY != port) || bind_error ) {
+		sockAddr.sin_port = htons(port);
+		if (bind(comm->uudp_sock, (const sockaddr *)&sockAddr, sizeof(sockAddr))) {
+			DPRINT(("commInit: failure to bind port %d\n", port));
+			dcstDestroy(comm->uudp_handles);
+			dcstDestroy(comm->uudp_secondary);
+			close(comm->uudp_sock);
+			resp->status = comm_STATUS_BUG;
+			return FALSE;
+		}
 	}
 
 	/* determine the actual port number */
@@ -499,11 +533,17 @@
 {
 	comm_DRIVER_SIGNATURE,
 	sizeof(comm_driverInfo_t),
-	"Unix Internet S",				/* name */
-	0x0100,							/* version */
+#ifdef TCP_LAN
+	"TCP/IP LAN",			/* name */
+#else
+	"Internet",				/* name */
+#endif
+	0x0100,					/* version */
 	(comm_DRIVER_IS_VISIBLE
 	| comm_DRIVER_ALLOWS_GAMELIST
+#ifndef TCP_LAN
 	| comm_DRIVER_NO_BROADCAST
+#endif
 	| comm_DRIVER_JUMBO_PACKETS),
 	0
 };
@@ -670,6 +710,7 @@
 	commTxPktResp_t	respDummy;
 	sockaddr_in sockAddr;
 	addr_t* pAddr;
+	uudps_peer2_t *pPeer2;
 
 	/* Protect against invalid arguments */
 	assert(validateComm(comm));
@@ -706,10 +747,10 @@
 	sockAddr.sin_port = pAddr->port;
 
 	/* Send the packet */
-/*	DPRINT(("commTxPkt: dest = ")); printAdr(pAddr); DPRINT(("\n"));
+	DPRINT(("commTxPkt: dest = ")); printAdr(pAddr); DPRINT(("\n"));
 	DPRINT(("commTxPkt: sendto(%d, %p, %d, 0, %x, %d)\n",
-					uudp_sock, req->buffer, req->length,
-					(const sockaddr *)&sockAddr, sizeof(sockAddr)));*/
+					comm->uudp_sock, req->buffer, req->length,
+					(const sockaddr *)&sockAddr, sizeof(sockAddr)));
 	if (sendto(comm->uudp_sock, req->buffer, req->length, 0,
 			(const sockaddr *)&sockAddr, sizeof(sockAddr)) < 0) {
 		DPRINT(("commTxPkt: sendto fails, errno %d\n", errno));
@@ -719,6 +760,46 @@
 
 	logPkt(comm->logpkt_fp, req->buffer, req->length, req->dest, "tx");
 
+	/* get ptr to peer2 address from peer handle */
+	pPeer2 = (uudps_peer2_t *) dcstFindValue(comm->uudp_secondary, req->dest);
+
+	/* if there is a second address and it is marked as live then
+		send the packet to the second address */
+	if (pPeer2 && (pPeer2->status == UUDP_HDL2_LIVE)) {
+		/* only SYN && ACK && PING packets should be sent to multiple addresses 
+		   note that User packets are allowed through since they are sent 
+			unreliably */
+		/* Kludge: in the case where we establish a dpio connection
+		 * to someone using only one address, and then much later openHdl2 
+		 * is called with a 2nd address, we will notice it here.
+		 * The proper action is to turn off the 2nd address, I think.
+		 *
+		 * If this kludge doesn't work,
+		 * this could be done instead at a higher level, in dpio_openHdl2,
+		 * by testing whether a connection is established before 
+		 * creating a new handle, but that might require a flag to commSayHi
+		 * to tell it whether to insert or not.
+		 */
+		if ((*(char *)req->buffer) == (dp_PACKET_INITIALBYTE)
+		&&  (*(dp_packetType_t *)req->buffer) != (dpio_SYN_PACKET_ID)
+		&&  (*(dp_packetType_t *)req->buffer) != (dpio_ACK_PACKET_ID)
+		&&  (*(dp_packetType_t *)req->buffer) != (dp_PING_PACKET_ID)) {
+			DPRINT(("commTxPkt: kludge: sent non-SYN; marking 2nd adr dead\n"));
+			pPeer2->status = UUDP_HDL2_NONE;
+		} else {
+			int cBytes;
+			/* fire off the packet */
+			sockAddr.sin_family = AF_INET;
+			sockAddr.sin_port = pPeer2->peer.port;
+			sockAddr.sin_addr.s_addr = pPeer2->peer.addr;
+			cBytes = sendto(comm->uudp_sock, (char *) req->buffer, (int) req->length, 0,	(struct sockaddr *) &sockAddr, sizeof(sockAddr));
+			if ((cBytes == 0) || (cBytes == SOCKET_ERROR)) {
+				DPRINT(("commTxPkt: sendto second adr fails, errno %d\n", errno));
+				return(comm_STATUS_FULL);
+			}
+			DPRINT(("commTxPkt: Sent 2nd copy to %s:%d\n", (char *) inet_ntoa(sockAddr.sin_addr), ntohs(sockAddr.sin_port)));
+		}
+	}
 	/* Return status */
 	resp->status = comm_STATUS_OK;
 	return TRUE;
@@ -853,7 +934,52 @@
 	/* Get handle of sender, copy into resp */
 	addr.addr = sockAddr.sin_addr.s_addr;
 	addr.port = sockAddr.sin_port;
-	resp->src = getHandleFromAddress(comm->uudp_handles, &addr);
+	resp->src = dcstFindKey(comm->uudp_handles, &addr);
+	DPRINT(("commRxPkt: dcstFindKey() returned h:%x ", resp->src));
+		DPRINT(("was looking for ")); printAdr(&addr); DPRINT(("\n"));
+
+	if(dcst_INVALID_KEY == resp->src) {
+	    /*
+	     * Since we did not find the address in the primary handle list
+	     * check to see if a LIVE address exists in the secondary list
+	     */
+	    uudps_peer2_t peer2;
+
+	    DPRINT(("commRxPkt: checking for live secondary adr: ")); printAdr(&addr); DPRINT(("\n"));
+	    peer2.status = UUDP_HDL2_LIVE;
+	    peer2.peer = addr;
+	    resp->src = dcstFindKey(comm->uudp_secondary, &peer2);
+
+	    /*
+	     * Did we find a handle ?
+	     */
+		if(dcst_INVALID_KEY != resp->src) {
+			/*
+			 * Since a secondary LIVE address has been found we must now
+			 * exchange the primary and secondary address and mark the
+			 * secondary address as NON-LIVE
+			 */
+			addr_t peer;
+			addr_t *peerOut;
+
+			/* Copy the secondary to the primary */
+			DPRINT(("commRxPkt: found live secondary adr, swapping\n"));
+			peer = peer2.peer;
+
+			/* Get the primary */
+			peerOut = dcstFindValue(comm->uudp_handles, resp->src);
+			assert(peerOut);
+
+			/* Copy the primary to the secondary */
+			peer2.status = UUDP_HDL2_NONE;
+			peer2.peer = *peerOut;
+
+			/* After the exchange replace them both */
+			dcstReplaceEx(comm->uudp_handles, resp->src, &peer);
+			dcstReplaceEx(comm->uudp_secondary, resp->src, &peer2);
+		}
+	}
+
 	if(dcst_INVALID_KEY == resp->src) {
 		resp->src = PLAYER_NONE;
 		assert(sizeof(addr_t) <= comm_MAX_ADR_LEN);
@@ -1098,12 +1224,26 @@
 	}
 
 	/* Find or assign a handle for this address */
-	resp->player = getHandleFromAddress(comm->uudp_handles, req->address);
-	if(resp->player == dcst_INVALID_KEY) {
+	/* Check primary address */
+	resp->player = dcstFindKey(comm->uudp_handles, req->address);
+	/* Check secondary address, if needed */
+	if((resp->player == dcst_INVALID_KEY) && req->address2) {
+		resp->player = dcstFindKey(comm->uudp_handles, req->address2);
+	}
+	/* If neither primary nor secondary address match... */
+	if (resp->player == dcst_INVALID_KEY) {
+		/* This is a new address, create a new handle */
 		resp->player = dcstAdd(comm->uudp_handles, req->address);
-		if (req->address2) {
-			dcstAddEx(comm->uudp_secondary, resp->player, req->address2);
-		}
+	} 
+	/* If we know a second address, but none has been stored yet, store it */
+	if (req->address2
+	&&  (dcst_INVALID_KEY != resp->player) 
+	&&  !dcstFindValue(comm->uudp_secondary, resp->player)) {
+		uudps_peer2_t peer2;
+		peer2.status = UUDP_HDL2_LIVE;
+		peer2.peer = *(addr_t *)req->address2;
+		DPRINT(("commSayHi: adding live secondary adr: ")); printAdr(&peer2.peer); DPRINT(("\n"));
+		dcstAddEx(comm->uudp_secondary, resp->player, &peer2);
 	}
 
 	/* Translate handles to commapi values */
