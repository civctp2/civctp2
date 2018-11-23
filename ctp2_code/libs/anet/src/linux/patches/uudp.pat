diff -ru anet.dan/src/linux/uudp/uudps.c anet/src/linux/uudp/uudps.c
--- anet.dan/src/linux/uudp/uudps.c	Mon Mar 15 16:52:21 1999
+++ anet/src/linux/uudp/uudps.c	Thu Mar 25 11:15:29 1999
@@ -710,7 +710,6 @@
 	commTxPktResp_t	respDummy;
 	sockaddr_in sockAddr;
 	addr_t* pAddr;
-	uudps_peer2_t *pPeer2;
 
 	/* Protect against invalid arguments */
 	assert(validateComm(comm));
@@ -760,44 +759,49 @@
 
 	logPkt(comm->logpkt_fp, req->buffer, req->length, req->dest, "tx");
 
-	/* get ptr to peer2 address from peer handle */
-	pPeer2 = (uudps_peer2_t *) dcstFindValue(comm->uudp_secondary, req->dest);
-
-	/* if there is a second address and it is marked as live then
-		send the packet to the second address */
-	if (pPeer2 && (pPeer2->status == UUDP_HDL2_LIVE)) {
-		/* only SYN && ACK && PING packets should be sent to multiple addresses 
-		   note that User packets are allowed through since they are sent 
-			unreliably */
-		/* Kludge: in the case where we establish a dpio connection
-		 * to someone using only one address, and then much later openHdl2 
-		 * is called with a 2nd address, we will notice it here.
-		 * The proper action is to turn off the 2nd address, I think.
-		 *
-		 * If this kludge doesn't work,
-		 * this could be done instead at a higher level, in dpio_openHdl2,
-		 * by testing whether a connection is established before 
-		 * creating a new handle, but that might require a flag to commSayHi
-		 * to tell it whether to insert or not.
-		 */
-		if ((*(char *)req->buffer) == (dp_PACKET_INITIALBYTE)
-		&&  (*(dp_packetType_t *)req->buffer) != (dpio_SYN_PACKET_ID)
-		&&  (*(dp_packetType_t *)req->buffer) != (dpio_ACK_PACKET_ID)
-		&&  (*(dp_packetType_t *)req->buffer) != (dp_PING_PACKET_ID)) {
-			DPRINT(("commTxPkt: kludge: sent non-SYN; marking 2nd adr dead\n"));
-			pPeer2->status = UUDP_HDL2_NONE;
-		} else {
-			int cBytes;
-			/* fire off the packet */
-			sockAddr.sin_family = AF_INET;
-			sockAddr.sin_port = pPeer2->peer.port;
-			sockAddr.sin_addr.s_addr = pPeer2->peer.addr;
-			cBytes = sendto(comm->uudp_sock, (char *) req->buffer, (int) req->length, 0,	(struct sockaddr *) &sockAddr, sizeof(sockAddr));
-			if ((cBytes == 0) || (cBytes == SOCKET_ERROR)) {
-				DPRINT(("commTxPkt: sendto second adr fails, errno %d\n", errno));
-				return(comm_STATUS_FULL);
+	/* Don't send a second packet when doing a network broadcast */
+	if ( req->dest != PLAYER_BROADCAST ) {
+		uudps_peer2_t *pPeer2;
+
+		/* get ptr to peer2 address from peer handle */
+		pPeer2 = (uudps_peer2_t *) dcstFindValue(comm->uudp_secondary, req->dest);
+
+		/* if there is a second address and it is marked as live then
+			send the packet to the second address */
+		if (pPeer2 && (pPeer2->status == UUDP_HDL2_LIVE)) {
+			/* only SYN && ACK && PING packets should be sent to multiple addresses 
+			   note that User packets are allowed through since they are sent 
+				unreliably */
+			/* Kludge: in the case where we establish a dpio connection
+			 * to someone using only one address, and then much later openHdl2 
+			 * is called with a 2nd address, we will notice it here.
+			 * The proper action is to turn off the 2nd address, I think.
+			 *
+			 * If this kludge doesn't work,
+			 * this could be done instead at a higher level, in dpio_openHdl2,
+			 * by testing whether a connection is established before 
+			 * creating a new handle, but that might require a flag to commSayHi
+			 * to tell it whether to insert or not.
+			 */
+			if ((*(char *)req->buffer) == (dp_PACKET_INITIALBYTE)
+			&&  (*(dp_packetType_t *)req->buffer) != (dpio_SYN_PACKET_ID)
+			&&  (*(dp_packetType_t *)req->buffer) != (dpio_ACK_PACKET_ID)
+			&&  (*(dp_packetType_t *)req->buffer) != (dp_PING_PACKET_ID)) {
+				DPRINT(("commTxPkt: kludge: sent non-SYN; marking 2nd adr dead\n"));
+				pPeer2->status = UUDP_HDL2_NONE;
+			} else {
+				int cBytes;
+				/* fire off the packet */
+				sockAddr.sin_family = AF_INET;
+				sockAddr.sin_port = pPeer2->peer.port;
+				sockAddr.sin_addr.s_addr = pPeer2->peer.addr;
+				cBytes = sendto(comm->uudp_sock, (char *) req->buffer, (int) req->length, 0,	(struct sockaddr *) &sockAddr, sizeof(sockAddr));
+				if ((cBytes == 0) || (cBytes == SOCKET_ERROR)) {
+					DPRINT(("commTxPkt: sendto second adr fails, errno %d\n", errno));
+					return(comm_STATUS_FULL);
+				}
+				DPRINT(("commTxPkt: Sent 2nd copy to %s:%d\n", (char *) inet_ntoa(sockAddr.sin_addr), ntohs(sockAddr.sin_port)));
 			}
-			DPRINT(("commTxPkt: Sent 2nd copy to %s:%d\n", (char *) inet_ntoa(sockAddr.sin_addr), ntohs(sockAddr.sin_port)));
 		}
 	}
 	/* Return status */