diff -ruN anet.dan/src/linux/uudp/ddprint.h anet/src/linux/uudp/ddprint.h
--- anet.dan/src/linux/uudp/ddprint.h	Wed Dec 31 16:00:00 1969
+++ anet/src/linux/uudp/ddprint.h	Sat Apr 10 13:23:02 1999
@@ -0,0 +1,48 @@
+/*--------------------------------------------------------------------------
+ DDPrint.h
+ 
+ Debug printing for ActiveNet dynamic drivers.
+
+ Define the symbol
+   DEBUG_MODULE
+ to TRUE or FALSE before including dprint.h.  For instance,
+   #define DEBUG_MODULE 1
+    include "ddprint.h"
+
+ Then, to print a debugging message, use
+   DPRINT(("message %s", str));
+ and compile the driver for debugging (which turns on _DEBUG).
+
+--------------------------------------------------------------------------*/
+
+#ifndef _DDPrint_h_
+#define _DDPrint_h_
+
+/*------------------------------------------------------------------------
+ Callback used to list servers and the round trip delay to them.
+------------------------------------------------------------------------*/
+typedef int (*dp_dprintf_t) (
+	const char *    __format,   /* printf-style format (or NULL) */
+	...);                       /* printf-style arguments on stack (if any) */
+
+dp_dprintf_t dp_so_dprintf = NULL;
+
+/*-------------------------------------------------------------------------
+ Call this to specify a dp_dprintf compatible function to call instead of 
+ the default dp_dprintf.
+-------------------------------------------------------------------------*/
+#define dp_dprintf_set(fn) { dp_so_dprintf = fn; }
+
+/*--------------------------------------------------------------------------
+ Print a debug string to the debugging console, or, if dp_dprintf_set has been
+ called, printed to the function specified by dp_dprintf_set.
+ Don't use dp_dprintf directly; use the DPRINT macro instead for lower overhead.
+--------------------------------------------------------------------------*/
+#if defined(_DEBUG) && defined(DEBUG_MODULE) && DEBUG_MODULE
+    #define DPRINT(p)  \
+		do { if ( dp_so_dprintf ) (dp_so_dprintf) p; } while (0)
+#else
+	#define DPRINT(p)  ;
+#endif
+
+#endif // _DDPrint_h_
diff -ruN anet.dan/src/linux/uudp/uudps.c anet/src/linux/uudp/uudps.c
--- anet.dan/src/linux/uudp/uudps.c	Mon Mar 15 16:52:21 1999
+++ anet/src/linux/uudp/uudps.c	Sat Apr 10 13:20:30 1999
@@ -54,8 +54,9 @@
 /* enable snoopy packet logging in all but normal builds */
 #include "logpkt.h"
 
+#undef DPRINT
 #define DEBUG_MODULE	!FALSE
-/* #define DPRINT(s) printf s */
+#include "ddprint.h"
 
 
 /**
@@ -1386,6 +1391,9 @@
 	/* Get the parameter */
 	resp->status = comm_STATUS_UNIMPLEMENTED;
 	switch (req->param_num) {
+		case comm_PARAM_DPRINTF:
+			dp_dprintf_set((dp_dprintf_t) req->param_value);
+			break;
 		case comm_PARAM_SCORE:
 			/* Do stuff here */
 			break;