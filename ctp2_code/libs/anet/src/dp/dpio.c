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

/* #define VERYVERBOSE */
/* #define VERBOSE */

/*----------------------------------------------------------------------
 Multiplayer game networking - reliable and unreliable transport layer.
 Copyright (C) 1997, Activision.

 --- Portability Corner ---

 Today's topic is: Byte Order.  Fear not, the swapbytes2() macro in anet.h
 makes it easy, once you decide what needs swapping.
 In this module, all 16-bit quantities that are sent over the network
 are in Intel byte order, so you need to use a macro when accessing them if
 you want to run on a non-Intel machine.

 The affected variables are mostly packet numbers and packet types.
 Packet types are swapped at compile time in anet.h.
 Packet numbers are swapped at runtime.
 The affected functions include anything that prepares a buffer for
 transmission or unpacks a buffer upon reception, including:
    dpio_q_packet, dpio_update, dpio_get, and dpio_get_window.
 Packet numbers are found in the following types:
   dpio_wrapped_data_packet_t.body.pktnum
   dpio_window_t.outstanding[].body.pktnum
   and dpio_get's ackbuf.body.pktnum.

 --- This has been another episode of the Portability Corner.  Thank you. ---

 $Log: dpio.c $
 Revision 1.114  1997/10/07 02:11:34  anitalee
 stub now passes sessionId to dpio in environment.
 if dpio finds sessionId in environment, it ignores sessionId
 in commInitReq.
 This lets NetShell keep the same address every time it opens
 the modem or null modem drivers.
 Revision 1.113  1997/09/27 05:36:21  lbennett
 Added validity-checking assertions.
 Revision 1.112  1997/09/27 02:01:01  lbennett
 Better error logging.
 Revision 1.111  1997/09/25 03:49:06  lbennett
 Minor changes.
 Revision 1.110  1997/09/23 17:18:18  lbennett
 Fixed callback removal to work in version 1.
 Revision 1.109  1997/09/23 02:48:50  lbennett
 Removed per-session callback.
 Revision 1.108  1997/09/20 05:25:27  lbennett
 Added freeze/thaw functions for player handles.
 Revision 1.107  1997/09/17 04:04:57  lbennett
 Attempt to clear transfers being sent to closing or closed handles.
 Revision 1.106  1997/09/15 23:48:53  lbennett
 Suppress artificially-induced error code.
 Revision 1.105  1997/09/15 21:19:38  lbennett
 Added debugging statements.
 Revision 1.104  1997/09/11 17:22:01  lbennett
 Allow normal FIN processing even if it's before we get the SYN.
 This is risky.  This needs to be examined more carefully!
 Repeated rapid opening and closing, as with src/test/load/load.c,
 is sure to expose some very interesting bugs.
 Revision 1.103  1997/09/10 01:34:45  lbennett
 Added better DPRINT messages.
 Revision 1.102  1997/09/05 21:36:02  lbennett
 Removed FIN-from-deleted-player bug.
 Removed L2-4 warnings.
 Revision 1.101  1997/09/04 06:43:54  dkegel
 Export dpio_scanAdr... this is a duplicate of dpResolveAddress, which
 should probably vanish.
 Revision 1.100  1997/09/03 17:09:38  dkegel
 Added upper limit on retries, to catch SYN flood attacks.
 Revision 1.99  1997/09/02 18:22:01  dkegel
 Removed brain-damaged double-free in dpio_destroy().
 Revision 1.98  1997/09/01 19:09:39  dkegel
 dpio_findTimedOutHost no longer calls dpio_closeHdlImmed; it
 always just returns the suspect handle to the caller.
 Revision 1.97  1997/08/31 04:24:21  dkegel
 1. Start out with roundtrip time estimate of 2 seconds
 to avoid initial meltdown.
 2. If packets were queued before the connection was established,
 arrange for them to be transmitted immediately, not after two seconds.
 3. Surrounded the most verbose DPRINT's with #ifdef VERYVERBOSE.
 4. Added disabled DPRINT's to dump all network traffic to log file.
 Revision 1.96  1997/08/25 01:25:00  dkegel
 Moved to a TCP-like dpio_setHandleOptions() call for controlling
 keepalives.  Much easier to use.
 Old stuff still enabled if dp_ANET2 not defined.
 Revision 1.95  1997/08/23 20:46:00  dkegel
 Split part of dpio_findTimedOutHost() out into dpio_findResetHost()
 so even if we don't want to enforce keepalives, we can at least
 handle active close properly!
 Revision 1.94  1997/08/22 21:15:46  dkegel
 Don't time out the game server.
 Revision 1.93  1997/08/22 16:47:40  dkegel
 Added way to send keepalives to gameserver in v2.
 Revision 1.92  1997/08/19 21:53:12  dkegel
 All handles printed as hex - somehow, a decimal crept in.
 Revision 1.91  1997/08/15 23:53:53  dkegel
 1. Acknowledge FIN's even if they are outside the window.
 2. FIN ACK's when there is no peer record are now silently ignored.
 3. FIN ACK's when the FIN is not yet at the head of the window
 are now processed correctly.
 See src/test/dpiot/dpiot for the test harness.
 Revision 1.90  1997/08/15 02:06:15  dkegel
 Fixed bug in how we detect out-of-window FIN packets!
 Revision 1.89  1997/08/12 23:11:14  dkegel
 Use curly braces around DPRINT.
 Revision 1.88  1997/08/11 01:39:18  dkegel
 1. Save driverinfo in dpio_t for use by dp later.
 2. Got rid of two bogus error dprint's.
 Revision 1.87  1997/08/06 21:30:57  dkegel
 dpio now adapts much quicker to actual round trip time at startup,
 so Dark Reign's ping time display doesn't take forever to ramp from
 500 milliseconds to the true value.
 Revision 1.86  1997/07/29 20:54:37  dkegel
 Fewer dpio_update dprint's.
 Revision 1.85  1997/07/28 02:39:56  dkegel
 Use dp_transport_t in dpio_create().
 Revision 1.84  1997/07/27 22:42:40  dkegel
 Use dp_transport_t properly.
 Revision 1.83  1997/07/27 03:26:22  dkegel
 1. Ignore FIN packets that are outside of the window.
 2. SYN packets cause a full connection shutdown even if no data packets
 have been received yet if their packet number is wrong.
 3. Slightly better dprints.
 Revision 1.82  1997/07/08 21:42:06  dkegel
 Added dprint to dpio_create so you can tell your own address.
 Revision 1.81  1997/06/24 18:54:51  dkegel
 Trust that driver return codes are really dp_result_t's.
 Revision 1.80  1997/06/20 20:57:32  dkegel
 1. Added new call dpio_getHdlTxPktsFree() for helping throttle
 dptab transmissions.
 2. Print out 'nq %d' in q_packet to indicate how full the tx window is.
 Revision 1.79  1997/06/14 02:58:26  geychaner
 Added handling of condition where closeHdl is called on a handle with a full
 tx queue; findTimedOutHost then attempts to close the handle every time it
 is called, because closeHdl sets STATE_CLOSE_QFULL when the queue full is
 detected.  (Whew!)
 Revision 1.78  1997/06/13 23:16:38  geychaner
 ACTIVE CLOSE now implemented:
 - dpio_closeHdl now sends a FIN packet rather than marking the handle for
   findTimedOutHost
 - dpio_get handles FIN packets similarly to SYN packets; send an ack, and
   a FIN if one has not been sent.
 - dpio_openHdl checks for a closing handle, and resets it by sending a SYN
   if a closing handle is reopened.
 - gotAck handles FIN acks, and better handles SYN acks as well.
 - Better checks on handle states throughout.
 Known bug:
 Attempting to close a handle whose queue is full will fail miserably.
 Revision 1.77  1997/05/31 22:49:25  dkegel
 Moved pragma pack into dp*pack*.h for portability.
 Revision 1.76  1997/05/29 19:38:56  dkegel
 Fixed minor type mismatches caught by Codewarrior.
 Revision 1.75  1997/05/23 22:14:56  dkegel
 1. Support delay modelling even when not compiled for debugging.
 2. Free the dpio structure when destroying it.
 3. Fixed broken dprint in dpreceive.
 Revision 1.74  1997/05/23 01:35:17  dkegel
 Fixed bad initial modem bandwidth parameter, bad drpint.
 Revision 1.73  1997/05/22 20:30:06  dkegel
 First crack at supporting modem delay simulation.
 Revision 1.72  1997/05/15 22:06:02  anitalee
 Made portable to big-endian machines - calls SwapBytes2 now when reading
 or writing 16-bit integers to or from the net.
 Revision 1.71  1997/05/15 20:58:36  dkegel
 Don't double initial estimate of connection latency.
 Revision 1.70  1997/05/12 18:37:36  anitalee
 *** empty log message ***
 Revision 1.69  1997/04/29 21:41:13  dkegel
 Old names for dpini_readParameter, dpini_findSection, and dp_enableDebugPrint
 no longer supported, so switch to new names.
 Revision 1.68  1997/04/29 19:53:06  dkegel
 Added dpio_get_player_latency.
 Revision 1.67  1997/04/21 01:44:42  dkegel
 Implemented dpHigherLevelBlanketAck.
 Triples performance of dpturn-based game at 2 players with recv pktloss
 of 20%!
 Revision 1.66  1997/04/02 06:16:30  geychaner
 1. Fixed bug in put_reliable with the size of the cdests array; there
    is now a #defined limit on handles (300)
 2. Made getBroadcastHandles not include full handles so that game servers
    are happier with session updates
 Revision 1.65  1997/03/27 03:11:13  geychaner
 Minor bugfix to the get_latency function (was checking caps where it
 should have been checking needs).
 Revision 1.64  1997/03/26 19:09:46  geychaner
 Set clocksPerSec in set_clocks, and fixed the latency calculation.
 Revision 1.63  1997/03/26 04:37:38  geychaner
 Finally got rid of that lame commented-out code in set_clocks (formerly
 default_tx_interval) for guessing minTxInterval.  Added a guess at the
 latency to set the default RTT instead, so all the 'tweakables' are now
 in dpio_set_clocks.
 Revision 1.62  1997/03/26 03:49:19  geychaner
 1. Added maxTxInterval to set max RTO, instead of calculating on the fly.
 2. Used return value from setNextTx to ceiling ReTxFactor for a hopefully
    quicker recovery from network problems.
 3. Replaced default_tx_interval with dpio_set_clocks; all *Interval timers
    (minTx, maxTx, maxRx) are set here for convenience.
 Revision 1.61  1997/03/26 03:09:46  geychaner
 Some minor stuff:
 1) changed lastPktTime to the more accurate PktTime.
 2) changed nextInterval in setNextTxTime to the more accurate nextRTO.
 3) changed the initialization for ReTxFactor to 0 from 1.
 4) setNextTxTime now returns the calculated RTO for later use.
 Revision 1.60  1997/03/25 21:12:57  dkegel
 1. Cap RTO at about 4 seconds rather than 10.
 2. Fix horrible bug that caused crash after thawing; callback thawed wrong
 3. dpio_default_tx_interval is really dpio_setClocksPerSec, so
 move commSetParam() from dp_setClocksPerSec to dpio_default_tx_interval;
 have dpio use this to set minTxInterval et al instead of doing it directly
 4. Set minTxInterval to 25ms instead of 50ms to handle LANs 'better'
 5. Big fix: ReTxFactor no longer grows without bound during string of
 single-retry packets.
 6. Fixed bug in one DPRINT, improved others.
 Revision 1.59  1997/03/25 01:52:53  dkegel
 Log what percentage of received packets are dropped by lossy network
 emulation feature.
 Revision 1.58  1997/03/24 17:20:41  dkegel
 First retry now just past RTT.  Print RTO (Retry Timeout).
 Revision 1.57  1997/03/20 04:14:55  geychaner
 Added dynamic adaptive retransmission to dpio_update.  This required the
 following changes and additions:
 1. dpio->minTxInterval now ONLY controls how often dpio_update scans for
    packets to be retransmitted, NOT how often the retransmissions occur.
 2. The next retransmission for a packet is now in setNextTxTime.
 3. dpio_default_tx_interval is no longer used; it should calculate the
    initial value of tx->RTTime for each connection.
 4. tx->lastPktTime for each connection now stores the NEXT retransmission
    time, to avoid calculating it too often.
 5. dpio_get now calculates the tx->RTTime (round trip time) when it
    receives an ACK packet.
 6. dpio_q_packet stores the queue time of each packet in tx->pktQTime.
 It has the following problems:
 1. *MAJOR* *dpio->now is not updated often enough.  dpio now needs to be
    updating this, not dp.  This is a major change.
 2. There are 3 things that need to be tuned: the seeting on minTxInterval
    in dpio_create, the floor of RTTime in dpio_get, and the ceiling on
    lastpktTime in setNextTxTime.
 3. RTTime needs to have a better initial setting in openHdl, as noted
    above.
 Revision 1.56  1997/03/15 03:30:52  geychaner
 Created a callback when an incoming connection is created; this resulted
 in a slight change to all openHdl callbacks as well.
 Revision 1.55  1997/03/14 23:50:36  geychaner
 1. Added setMaxPlayerHandles to limit the number of open connections
 2. Added closeHdlRaw to close a handle leak when openHdl rejects a connection
 Revision 1.54  1997/03/14 06:37:08  geychaner
 Added 1st generation simple throttleback to dpio_update.
 dpio_update now exponentially throttles back on multiple retransmits.
 Revision 1.53  1997/03/14 03:57:50  geychaner
 DPrint in q_packet is now even better than ever.  Oh, yeah.  :)
 Revision 1.52  1997/03/14 02:26:27  geychaner
 1. Can no longer queue packets to a closed handle
 2. Improved OpenHdl callback for safety (clear before calling)
 3. dpio_update no longer send data packets to half-open handles
 4. Better dprints in q_packet and dpio_get.
 Revision 1.51  1997/03/14 01:02:52  geychaner
 New version of dpio_update with my original looping mechanism, which
 works, as opposed to the kludge Dan enforced, which doesn't.
 Revision 1.50  1997/03/13 20:58:51  geychaner
 dpio_update now loops over connections looking for packets to
 retransmit; this helps the game host in a game with a large number
 of players, only a few of which have packet loss.
 Revision 1.49  1997/03/13 04:00:22  dkegel
 1. hMaster now passed as value to dpio_setMaster instead of pointer to
 dpio_create; this lets us avoid timing out the new master after an
 election in the common case that the new master hadn't been chatting
 with us.
 2. Lots o' dprint changes.  No longer print 'dropping packet' for
 packets from PLAYER_NONE, even though we should if it's a SYN packet.
 Revision 1.48  1997/03/13 00:55:53  geychaner
 Uniform DPRINTing of time stamps (and more times printed)!
 Revision 1.47  1997/03/12 23:58:30  dkegel
 Simplified timeout processing.  Note arrival of all packets now.
 Also minor cleanup of dpio_get.
 Revision 1.46  1997/03/12 21:56:01  geychaner
 Created q_packet function to handle reliable packets, so that openHdl
 and put_reliable don't have separate packet queueing mechanisms.
 Revision 1.45  1997/03/12 04:07:15  dkegel
 Don't crash if callback null.  Tsk, tsk.
 Revision 1.44  1997/03/12 01:27:49  dkegel
 Print out pktnum in common case in dpio_get.
 Revision 1.43  1997/03/12 01:17:59  geychaner
 added getHdlState, to test the connection state of handles.  Used to
 test whether a connecti9on opened successfully.
 Revision 1.42  1997/03/11 23:08:32  dkegel
 Added 'fault' option in dp.ini.
 Revision 1.41  1997/03/11 20:52:22  geychaner
 OpenHdl_callback is now called in FindTimedOutHost when a timed out host
 is found, if the callback exists.  This is to handle the case of finding
 a timed-out host that is not yet ESTABLISHED, but that the upper layer
 doesn't call CloseHdlImmed on.
 Revision 1.40  1997/03/11 06:02:17  geychaner
 1. Changed CloseHdlImmed so that it checks the passed handle before deletion
 2. Added a callback to OpenHdl.  Callback is triggered:
    a. in OpenHdl, if the handle is a predefined handle
    b. in OpenHdl, if the connection is aready at state ESTABLISHED
    c. in dpio_get, if receiving a SYN or ACK causes the connection to
       transition to state ESTABLISHED
    d. in CloseHdlImmed, just before the connection is deleted.  This
       calls the callabck with an error (HOST_NOT_RESPONDING), because
       the connection never reached ESTABLISHED.
    The callback is cleared after being called.  The callback is stored in
    teh connection structure, allowing a different callback for each handle
    opened with OpenHdl.
 Revision 1.39  1997/03/11 03:29:04  geychaner
 changed findTimedOutHost so it takes a handle to check.  It checks all
 handles if passed PLAYER_NONE; it never automatically check the hMaster.
 Revision 1.38  1997/03/08 04:39:34  dkegel
 Fixed two bugs: one naughty, one nice.
 Revision 1.37  1997/03/08 03:04:33  dkegel
 Handles now printed uniformly as h:%x for benefit of Unix driver,
 where handles are identical to IP addresses (whee!)
 Revision 1.36  1997/03/07 19:38:57  dkegel
 use dp_MALLOC/FREE/REALLOC instead of malloc/free/realloc
 so the Windows DLL can use GlobalAlloc or whatever to avoid
 losing its mind when HeapDestroy is called.
 Revision 1.35  1997/03/07 00:45:17  dkegel
 1. Rewrote to avoid use of intermediate variable wasNew when
 testing directly for SYN_RCVD is simpler.
 2. Don't reset connection when new SYN recieved if no data packets
 had been recieved.  This may fix the heinous "echoing RST" bug.
 Revision 1.34  1997/03/06 02:28:57  dkegel
 When returning reliable packet to user code, dprint its pktnum!
 Revision 1.33  1997/03/05 02:39:20  dkegel
 ini.h and dprint.h now integrated into anet.h;
 to compile or use DP as a DLL, define DP_DLL;
 win\build.bat now creates debugging and nondebugging versions
 of both the static dp.lib and the dynamic anetdll.dll.
 Revision 1.32  1997/03/03 04:48:46  dkegel
 1. Fixed bug where some packets leak to caller after freezing.
 2. No longer allow sending reliable packets after freezing.
 3. No longer accept new connections after freezing.
 Revision 1.31  1997/03/03 04:38:48  dkegel
 Fixed bug related to packet number wraparound (can't just count up
 from windowbase to next_pktnum!)
 Revision 1.30  1997/03/03 03:46:32  dkegel
 1. dpio_STATE_CLOSED now gets its own bit, so other bits don't interfere.
 2. Remember pktnum of syn, and reset connection if a syn with a different
 pktnum comes in.  Risky, but the only way to allow reconnects after
 crashes without port numbers, I think.
 3. state now dprinted uniformly as h:%x state:%x
 4. better error checking after dpio_window_get, better dprints
 Revision 1.29  1997/03/01 20:04:29  dkegel
 1. Only dpio_update modifies or checks nextTxTime.
 2. Better DPRINTs.
 Revision 1.28  1997/03/01 08:58:29  dkegel
 1. Always print handles as h:%x!
 2. dpio_create tells driver to RESUME if thawfp not null.  Duh.
 3. Don't print messages when no pktloss in .ini file, or when
 getting packets from unknown hosts.
 Revision 1.27  1997/03/01 03:44:50  gceych
 Added closeHdlImmed (called by findTimedOutHost) for immediate handle
 deletion (no waiting for tx, rx, or other foolin' about).
 Revision 1.26  1997/03/01 02:49:29  gceych
 1. dpio_closeHdl sets connection state to CLOSED instead of deleting handle
 2. findTimedOutHost closes handles whose state is CLOSED in tx buffer
    is empty and all rx in buffer have been acked.
 (This is a cheesy version of active closing.)
 Revision 1.25  1997/02/28 01:30:49  gceych
 Fixed bug where clients would time out game server
 Revision 1.24  1997/02/27 20:33:00  dkegel
 1. Slow down retransmissions to once every 0.4 sec instead of 0.33 sec.
 Revision 1.23  1997/02/27 01:53:21  dkegel
 THE BIG REARRANGEMENT: includes now done carefully to avoid
 conflicts between anet.h and dp.h
 Revision 1.22  1997/02/26 18:15:54  dkegel
 Fixed DPRINTs that show connection established msg.
 Revision 1.21  1997/02/26 03:05:20  gceych
 1. Added getBroadcastHdls for benefit of dpPoll
 2. Session packets keep connections alive, too, so game servers don't time
    out hosts advertising sessions
 Revision 1.20  1997/02/25 23:28:51  dkegel
 dpio_create lets caller override frozen commDLLname so cross-platform
 thaws are possible.
 Revision 1.19  1997/02/25 05:01:04  gceych
 Fixed error handling in dpio_put_unreliable
 Revision 1.18  1997/02/24 01:23:45  dkegel
 1. Sped up retransmit rate slightly so it would send
 even if a big pv_update-fest was going on.
 2. Fewer DPRINT's.
 Revision 1.17  1997/02/23 05:34:30  dkegel
 Reduce retransmissions to 2/sec for LAN, 1/sec for modem and Internet.
 This is an interim measure until we implement them right a la tcp.
 Revision 1.16  1997/02/22 21:50:26  dkegel
 Print out sessionId for benefit of serial driver debugging.
 Revision 1.15  1997/02/19 20:38:33  dkegel
 1. Always print out msg when connection established.
 2. Fixed? bug in obscure case when DAT comes in before SYN.  Hard to test.
 3. Fixed bug in error checking when sending ack.
 4. Added in PPP overhead to statistics.
 Revision 1.14  1997/02/18 07:12:16  dkegel
 1. Don't let PLAYER_ME packets reach commTxPkt (oops)
 2. Don't count PLAYER_ME packets in bandwidth report
 3. Added more precision to packets/second figures (integer just not
 enough)
 4. Added UDP overhead as rough guide
 Revision 1.13  1997/02/18 05:11:08  dkegel
 Added Terry's statistics module.
 Revision 1.12  1997/02/18 03:16:54  dkegel
 1. Fixed null pointer bug.
 2. Rather verbose prints during freeze and thaw.
 Revision 1.11  1997/02/18 00:18:20  dkegel
 Get packet loss from .ini file!
 Revision 1.10  1997/02/17 21:04:07  dkegel
 Adapted to new size limit defines.
 Revision 1.9  1997/02/17 06:08:48  dkegel
 dpio_put_unreliable had wrong upper bound on packet length.

 Revision 1.8  1997/02/16 03:56:30  dkegel
 1. dpio_freeze / thaw more robust; checks for start and end tags.
 2. dpio_thaw now part of dpio_create so it can get transport DLL
 and comm parameters from freeze file.
 3. Fixed bug - cannot assume that commSayBye deletes a comm handle
 from the driver.
 Revision 1.7  1997/02/15 23:56:45  dkegel
 Fixed timeout on openHdl.
 Revision 1.6  1997/02/15 02:15:31  dkegel
 1. Fixed dp_thaw; now maps comm handles on thaw.
 2. Fixed protocol; now refrains from sending user data until other guy's
 SYN acked and our SYN sent.
 3. Fixed protocol; now doesn't go whacko if later packet acked before
 windowBase.
 4. Changed timeout scheme; caller no longer needs to call
 dpio_notePacketArrival to start timeout on a client.
 5. Major rewrite; now a single structure, dpio_conn_t, holds all
 info about a connection, rather than the info being spread out across
 several tables.
 Revision 1.5  1997/02/12 23:38:52  dkegel
 Allow commInitReq to be null in dpio_create(); ok for IPX games, anyway.
 Revision 1.4  1997/02/12 04:14:05  dkegel
 Fixed tiny typo.
 Revision 1.3  1997/02/12 03:03:25  dkegel
 Added SYN packet!  No longer compatible with older versions.
 Always opens reliable connection as soon as you call dpio_openHdl.
 Protocol still full of holes, I bet.  Can be fooled.
 Revision 1.2  1997/02/10 07:03:14  dkegel
 Works.
 Revision 1.1  1997/02/08 01:04:05  dkegel
 Initial revision
----------------------------------------------------------------------*/

/* Disable MSVC warnings as follows; the include files generate these when
MSVC's warning level is set to 4.
4201: nonstandard extension used : nameless struct/union
4214: nonstandard extension used : bit field types other than int
4115: named type definition in parentheses */
#if defined(_WIN32)
#pragma warning( disable : 4201 4214 4115 )
#endif

#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

#include "dp.h"
#include "dpmem.h"
#include "contract.h"
#include "eclock.h"
#ifdef _WIN32
#include "anetstub.h"
#endif
#include "ptimer.h"
/* PTIMER_* must be less than NPMAX */
#define PTIMER_DPIO_NETWORK_GET 6

static dp_result_t dpio_flushPacketsForHandle(dpio_t *dpio, dpio_conn_t *pc, playerHdl_t h);
static dp_result_t dpio_put_unreliable_buffered(dpio_t *dpio, dpio_conn_t *pc, playerHdl_t dest, void *buffer, size_t size);
static dp_result_t dpio_put_unreliable_unbuffered(dpio_t *dpio, playerHdl_t dest, void *buffer, size_t size);

/* Re-enable MSVC warnings (#@$&%#^& MS) */
/* Disable MSVC warning "unreferenced inline function has been removed" */
#if defined(_WIN32)
#pragma warning( default : 4201 4214 4115 )
#pragma warning( disable : 4514 )
#endif

/* Upper bound on retries.
 * Want 40 second timeout.
 * If the RTT is 50msec, and max retrans interval is 4 seconds,
 * retransmits will be at 50, 100, 200, 400, 800, 1600, 3200, 8000, 12000...
 * msec.  First 8 seconds take 8 retries, last 32 seconds take next 8.
 */
#define dpio_MAX_RETRIES 17
/* If the RTT is still a guess (2 seconds), retransmits are at
 * 2, 4, 8, 16, etc. seconds, so it only takes 10 retries to reach 40 sec.
 */
#define dpio_MAX_RETRIES_GUESS 10

#define dpio_keepaliveInterval(dpio) ((dpio)->maxRxInterval/8)

/* Bogus upper limit on player handles due to dpio_put_reliable's
 * temporary array
 * See also bogus hard limit in Linux internet driver uudps.c
 * and in call to setMaxHandles in linux/server.c
 */
#define dpio_MAX_HANDLES 2000

#if defined(dpio_LOG) && (defined(DEBUG) || defined(_DEBUG))
static void dpio_log_pkt(const unsigned char *buf, size_t len, int repeatct);
static void dpio_log_pkt_err(dp_result_t err);
static void dpio_log_term();
static void dpio_log_init();
static int dpio_log_rx;
static int dpio_log_dropped;
#else
#define dpio_log_pkt(buf, len, repeatct)
#define dpio_log_pkt_err(err)
#define dpio_log_term()
#define dpio_log_init()
#endif

#define DP_MODEL_DELAY

/* Function to dump a block of memory in hex */
#ifdef VERYVERBOSE
static void dumpBuf(const char *msg, const char *buf, int len)
{
	int i;
	DPRINT(("%s", msg));
	for (i=0; i<len; i++) {
		DPRINT(("%02x", 0xff & buf[i]));
	}
	DPRINT(("\n"));
}
#else
#define dumpBuf(a,b,c)
#endif

/*----------------------------------------------------------------------
 Debugging routine.  Broken- should print out right # of bytes.
----------------------------------------------------------------------*/
/*
* WARNING: This compiler-control condition does not match that of
* DPRINT, although this function seems to be a DPRINT-only way to
* print an address.
*/
#if 0
#define dpio_dprintAdr(adr, len)
#else
void dpio_dprintAdr(
	char adr[dp_MAX_ADR_LEN],
	int adrLen)
{
	char buf[256];
	int i;

    if(adr == NULL) {
		DPRINT(("NULL"));
    } else {
		buf[0] = 0;
		for (i=0; i<adrLen-1; i++)
			sprintf(buf + 3*i, "%02x:", ((unsigned char *)adr)[i]);
		sprintf(buf + 3*i, "%02x", ((unsigned char *)adr)[i]);
		DPRINT(("%s", buf));
	}
}
#endif

#if 1
/*------------------------------------------------------------------------
 Print out all the values of a dpio_window_t.
------------------------------------------------------------------------*/
static void dpio_pw(dpio_window_t *pw)
{
	int i;
	DPRINT(("dpio_pw: windowBase %d, next_pktnum %d, bAcked: ",
		pw->windowBase, pw->next_pktnum));
	for (i=0;i<dpio_WINDOWSIZE;i++) {
		DPRINT(("%d ", pw->bAcked[i]));
	}
	DPRINT(("\n"));
	(void) pw;
}
#endif

/*------------------------------------------------------------------------
 Print out all the values of a dpio_conn_t.
------------------------------------------------------------------------*/
static void dpio_pc(dpio_t *dpio, dpio_conn_t *pc)
{
	DPRINT(("dpio_pc: state:%x", pc->state));
	DPRINT(("dpio_pc: rx "));
	dpio_pw(&pc->rx);
	DPRINT(("dpio_pc: tx "));
	dpio_pw(&pc->tx);
}

/*------------------------------------------------------------------------
 Print out all the values of a dpio_t.
------------------------------------------------------------------------*/
static void dpio_print(dpio_t *dpio)
{
	int i;
	DPRINT(("dpio_print: conns->n_used %d; enabl %d, nexttx %lx, mintx %lx, now %lx\n",
		dpio->conns->n_used,
		dpio->reliable_enabled,
		dpio->nextTxTime,
		dpio->minTxInterval,
		*(dpio->now)));
	for (i=dpio->conns->n_used - 1; i>=0; i--) {
		dpio_conn_t *pc;
		assoctab_item_t *pe;
		pe = assoctab_getkey(dpio->conns, i);
		if (!pe) {
			 DPRINT(("dpio_print: no entry for index %d\n", i));
			 continue;
		}
		pc = (dpio_conn_t *)&pe->value;
		DPRINT(("dpio_print: conn %d of %d; h:%x; ", i, dpio->conns->n_used, pe->key));
		dpio_pc(dpio, pc);
	}
}

/* Do an internal consistancy check on the reliable transport */
void dpio_assertReliableValid(dpio_t *dpio)
{
	int txGatherNeeded = FALSE;
	int i;

	/* Make sure global txGatherNeeded accurately reflects whether any
	 * buffered packets are ready to send.
	 * Only false negatives are a problem.  We expect some false positives.
	 */
	if (dpio->txGatherNeeded == FALSE) {
		for (i=dpio->conns->n_used - 1; i>=0; i--) {
			dpio_conn_t *pc;
			assoctab_item_t *pe;
			pe = assoctab_getkey(dpio->conns, i);
			if (!pe) {
				 assert(pe);
				 continue;
			}
			pc = (dpio_conn_t *)&pe->value;
			if (pc->txGatherBufLen > 2) {
				txGatherNeeded = TRUE;
				break;
			}
		}
		if (txGatherNeeded) {
			DPRINT(("dpio_assertReliableValid: bug: txGatherNeeded should be set, but isn't\n"));
			assert(dpio->txGatherNeeded == txGatherNeeded);
		}
	}
}

/*-----------------------------------------------------------------------
 Calculates the latency of the current driver; actually just a guess
 for now.  (May eventually calculate from the RTT of the open handles.)
 Results in milliseconds.
 Returns 0 upon error.
-----------------------------------------------------------------------*/
int dpio_get_latency (dpio_t *dpio, int flags)
{
	int latency;

	long caps = dpio->driverinfo->capabilities;
	long needs = dpio->driverinfo->needs;
	DPRINT(("dpio_get_latency: comm_driverInfo_t: name %s, caps %x, needs %x\n",
		dpio->driverinfo->name, caps, needs));

	dpio_assertValid(dpio);

	/* Guess latency */
	if (needs & comm_INIT_NEEDS_MODEMINISTR) {
		/* Modem... hmm, probably not ISDN.  Guess 250 ms. */
		latency = 250;
	} else if (needs & comm_INIT_NEEDS_BAUD) {
		/* Needs a baud rate... probably direct serial.  Guess 125 ms. */
		latency = 125;
	} else if (caps & comm_DRIVER_NO_BROADCAST) {
		/* It's an internet driver...  Guess 500 ms. */
		latency = 500;
	} else {
		/* Well, maybe it's IPX.  Wild guess of 25 milliseconds. */
		latency = 25;
	}
	/* Double latency if user asks for reliable info.  Is this wise? */
	if (flags & dp_SEND_RELIABLE) latency *= 2;
	return latency;
}

/*-----------------------------------------------------------------------
 Calculates the latency of the current player according to the reliable
 transport system and pings if requested by dpio_setHandleOptions.
 Results in milliseconds.
 If ploss is not NULL, sets *ploss to round trip packet loss in percent.
 Returns 0 upon error.
-----------------------------------------------------------------------*/
int dpio_get_player_latency (dpio_t *dpio, playerHdl_t h, int flags, int *ploss)
{
	dpio_conn_t *pc;
	int		i;

	(void) flags;

	dpio_assertValid(dpio);

	pc = (dpio_conn_t *) assoctab_subscript (dpio->conns, h);
	if (!pc) {
		DPRINT(("dpio_get_player_latency: bogus h:%x!\n", h));
		dpio_assertValid(dpio);
		return 0;
	}

	if (ploss) {
		unsigned int	temp;
		unsigned int	sum;

		temp = pc->ping_bits;
		for (i = 16, sum = 0; --i >= 0; temp >>= 1) {
			if (temp & 1) sum += 100;
		}
		*ploss = (int) (sum/16U);
#ifdef VERBOSE
		DPRINT(("dpio_get_player_latency: h:%x bits:%x sum %d loss %d\n", h, pc->ping_bits, sum, *ploss));
#endif
	}

	dpio_assertValid(dpio);
	return (1000 * pc->RTTime) / dpio->clocksPerSec;
}

/*-----------------------------------------------------------------------
 Calculates the next retransmit time for a packet
-----------------------------------------------------------------------*/
static clock_t dpio_setNextTxTime(dpio_t *dpio, dpio_conn_t *pc)
{
	clock_t nextRTO;
	dpio_window_t *pw = &pc->tx;

	dpio_assertValid(dpio);
	/* RTO = RTT * backoff * beta, backoff = 2^ReTxFactor
	 * beta = 5/4ths so 1st retry just past RTT */
	nextRTO = ((pc->RTTime * (1 << pw->ReTxFactor)) * 5) >> 2;

	/* Don't let it get too large; this may need tuning */
	if (nextRTO > dpio->maxTxInterval)
		nextRTO = dpio->maxTxInterval;
	pw->PktTime = *dpio->now + nextRTO;

	DPRINT(("dpio_setNextTxTime: t:%d, RTT %d, RTO %d, ReTx %d, next %d\n",
		*dpio->now, pc->RTTime, nextRTO, pw->ReTxFactor, pw->PktTime));
	dpio_assertValid(dpio);
	return nextRTO;
}

/*-----------------------------------------------------------------------
 Set the callback for incoming connection requests
-----------------------------------------------------------------------*/
dp_result_t
dpio_setIncomingCallback(
	dpio_t *dpio, dpioOpenHdlCallback_t cb, void *context)
{
	dpio_assertValid(dpio);
	DPRINT(("dpio_setIncomingCallback: setting incoming callback\n"));
	if (dpio) {
		dpio->callback = cb;
		dpio->context = context;
		dpio_assertValid(dpio);
		return dp_RES_OK;
	}
	DPRINT(("dpio_setIncomingCallback: no dpio!\n"));
	dpio_assertValid(dpio);
	return dp_RES_BUG;
}

/*-----------------------------------------------------------------------
 Set the maximum number of player handles we will allow to be created
-----------------------------------------------------------------------*/
void dpio_setMaxPlayerHdls(dpio_t *dpio, int maxHdls) {
	dpio_assertValid(dpio);
	if (maxHdls > dpio_MAX_HANDLES) {
		DPRINT(("dpio_setMaxPlayerHdls: maxHdls %d exceeds defined limit %d\n", maxHdls, dpio_MAX_HANDLES));
		maxHdls = dpio_MAX_HANDLES;
	}
	DPRINT(("dpio_setMaxPlayerHdls: setting max connections to %d\n", maxHdls));
	if (dpio)
		dpio->max_playerHdls = maxHdls;
	dpio_assertValid(dpio);
}

/*-----------------------------------------------------------------------
 Given an ASCII network address and a buffer length, fill a buffer with the
 binary address.
 Returns length of address in bytes, or 0 upon error.
 Output buffer must be big enough, or buffer won't be valid.
-----------------------------------------------------------------------*/
DP_API int dpio_scanAdr(dpio_t *dpio, char *hostname, char *adrbuf, size_t buflen)
{
	commScanAddrReq_t req;
	commScanAddrResp_t resp;

	dpio_assertValid(dpio);
	req.printable = hostname;
	req.size = buflen;
	req.address = adrbuf;
	memset(adrbuf, 0, buflen);
	DPRINT(("dpio_scanAdr(%s,%p,%d):\n", hostname, adrbuf, buflen));
	if (!commScanAddr(&req, &resp, dpio->commPtr)) {
		DPRINT(("dpio_scanAdr: Unable to commScanAddr %s, status:%d\n", hostname, resp.status));
		dpio_assertValid(dpio);
		return 0;
	}
	(void) dpio;
	DPRINT(("dpio_scanAdr: got address.\n"));
	dpio_assertValid(dpio);
	return resp.length;
}

/*-----------------------------------------------------------------------
 Convert a binary network address (e.g. one from dpio_scanAdr),
 convert it to ASCII (in a form suitable for reading by dpio_scanAdr).

 On entry,
   adrbuf holds a binary network address,
   adrlen is the length of the address,
   buf    points to a buffer to hold the string,
   buflen is the size of buf.
 On success,
   buf contains a null-terminated ASCII string.

 Returns dp_RES_OK on success,
 Returns dp_RES_BAD if buflen is less than the size required to hold the string
 Returns dp_RES_BAD on any other failure
-----------------------------------------------------------------------*/
DP_API dp_result_t dpio_printAdr(dpio_t *dpio, const char *adrbuf, size_t adrlen, char *buf, size_t buflen)
{
	commPrintAddrReq_t req;
	commPrintAddrResp_t resp;

	dpio_assertValid(dpio);
	memset(&req, 0, sizeof(commPrintAddrReq_t));
	memset(&resp, 0, sizeof(commPrintAddrResp_t));
	req.address = adrbuf;
	req.length = adrlen;
	req.printable = buf;
	req.size = buflen;

	DPRINT(("dpio_printAdr(%p,%d,):\n", adrbuf, adrlen));
	if (!commPrintAddr(&req, &resp, dpio->commPtr)) {
		DPRINT(("dpio_printAdr: Unable to commPrintAddr, status:%d\n", resp.status));
		dpio_assertValid(dpio);
		return dp_RES_BAD;
	}
	(void) dpio;
	DPRINT(("dpio_printAdr: got address.\n"));
	dpio_assertValid(dpio);
	return dp_RES_OK;
}

/*-----------------------------------------------------------------------
 This function has been superceeded by dpio_openHdlRaw2 (below)
-----------------------------------------------------------------------*/
playerHdl_t dpio_openHdlRaw(dpio_t *dpio, void *adr)
{
  return (dpio_openHdlRaw2(dpio, adr, NULL, FALSE));
}

/*-----------------------------------------------------------------------
 Given a network address, return the handle associated with it.
 If no handle is currently associated with that address, create a handle.
 Use with caution!  Does not create a reliable connection!

 Similar to dpio_openHdlRaw except that TWO addresses are supplied.
 Refer to dpio_openHdl2 for reasons why you would want to do this.

 The flags can be a combination of:
	dpio_OPENHDL_RAW_2NDLIVE: the second address is to be treated as LIVE
-----------------------------------------------------------------------*/
playerHdl_t dpio_openHdlRaw2(dpio_t *dpio, void *adr, void *adr2, long flags)
{
	commSayHiReq_t		cshReq;
	commSayHiResp_t		cshResp;
	playerHdl_t       h;

	dpio_assertValid(dpio);

	cshReq.address = adr;
	cshReq.address2 = adr2;
	cshReq.length = dpio->myAdrLen;
	cshReq.flags = 0;
	cshReq.flags |= (flags & dpio_OPENHDL_RAW_2NDLIVE) ? comm_SAYHI_2NDLIVE : 0;

	if (!commSayHi( &cshReq, &cshResp, dpio->commPtr ))
	{
		DPRINT(("dpio_openHdlRaw: adr "));
		dpio_dprintAdr((char *) adr, dpio->myAdrLen);
		if (adr2) {
			DPRINT((", "));
			dpio_dprintAdr((char *) adr2, dpio->myAdrLen);
		}
		DPRINT((" error: commSayHi returns status %d\n", cshResp.status));
		dpio_assertValid(dpio);
		return PLAYER_NONE;
	}
	h = cshResp.player;
	DPRINT(("dpio_openHdlRaw: adr "));
	dpio_dprintAdr((char *) adr, dpio->myAdrLen);
	if (adr2) {
		DPRINT((", "));
		dpio_dprintAdr((char *) adr2, dpio->myAdrLen);
	}
	DPRINT((" status:%3u h:%x\n", cshResp.status, h));
	dpio_assertValid(dpio);
	return h;
}

/*-----------------------------------------------------------------------
 This deletes a raw handle without deleting the connection queue or any
 other info.  Internal use only.
-----------------------------------------------------------------------*/
dp_result_t dpio_closeHdlRaw(dpio_t *dpio, playerHdl_t h)
{
	commSayByeReq_t		csbReq;
	commSayByeResp_t	csbResp;

	csbReq.player = h;
	if (!commSayBye( &csbReq, &csbResp, dpio->commPtr )) {
		DPRINT(("dpio_closeHdlRaw: %d failed, status:%3u\n", h, csbResp.status));
		return dp_RES_BAD;
	}
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
Freeze a player handle.

Makes use of the fact that the address is invariant, although the player
handle itself may change value while frozen.
-------------------------------------------------------------------------*/
dp_result_t
dpio_freezeHdl(
	dpio_t* dpio,
	playerHdl_t hdl,
	FILE* file)
{
	char bTy = 0;
	commPlayerInfoReq_t		req;
	commPlayerInfoResp_t	resp;

	precondition(dpio != NULL);
	precondition(file != NULL);
	dpio_assertValid(dpio);

	/* Kluge: assume that special handles are all different in the
	   low-order byte, and that the low-order byte of all of them
	   is not zero. */
	/* Write type of handle; 0=normal */
	switch(hdl) {
		case PLAYER_NONE:
		case PLAYER_UNKNOWN:
		case PLAYER_ME:
		case PLAYER_BROADCAST:
			bTy = (char)(hdl & 0xFF);
			dpio_assertValid(dpio);
			if(fwrite(&bTy, sizeof(char), 1, file) != 1)
				return dp_RES_FULL;
			return dp_RES_OK;
		default:
			bTy = 0;
			dpio_assertValid(dpio);
			if(fwrite(&bTy, sizeof(char), 1, file) != 1)
				return dp_RES_FULL;
			break;
	}

	/* write addresses */
	req.player = hdl;
	memset(&resp, 0x00, sizeof (resp));
	if (!commPlayerInfo(&req, &resp, dpio->commPtr)) {
		DPRINT(("dpio_freezehdl: commPlayerInfo failed. status %d\n", resp.status));
		return dp_RES_BAD;
	}

	if (fwrite(resp.address, dpio->myAdrLen, 1, file) != 1)
		return dp_RES_FULL;

	if (resp.status == comm_STATUS_DUAL_ADDRESS) {
		if (fwrite(resp.address2, dpio->myAdrLen, 1, file) != 1)
			return dp_RES_FULL;
	} else {
		if (fwrite(resp.address, dpio->myAdrLen, 1, file) != 1)
			return dp_RES_FULL;
	}
	if (fwrite(&resp.flags, sizeof (long), 1, file) != 1)
		return dp_RES_FULL;

	DPRINT(("dpio_freezeHdl: handle %d, addr=", hdl));
	dpio_dprintAdr(resp.address, dpio->myAdrLen);
	if (resp.status == comm_STATUS_DUAL_ADDRESS) {
		DPRINT((", addr2="));
		dpio_dprintAdr(resp.address2, dpio->myAdrLen);
	}
	DPRINT((", flags=%d\n", resp.flags));
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
Thaw a player handle.

This function must be called *after* the dpio is thawed (dpio_create()).

Makes use of the fact that the address is invariant, although the player
handle itself may change value while frozen.
-------------------------------------------------------------------------*/
dp_result_t
dpio_thawHdl(
	dpio_t* dpio,
	playerHdl_t* hdl,
	FILE* file)
{
	char adr[dp_MAX_ADR_LEN];
	char adr2[dp_MAX_ADR_LEN];
	long flags;
	char bTy;

	precondition(dpio != NULL);
	precondition(hdl != NULL);
	precondition(file != NULL);

	/* Read type */
	if(fread(&bTy, sizeof(char), 1, file) != 1)
		return dp_RES_EMPTY;
	switch(bTy) {
		case (char)(PLAYER_NONE & 0xFF):
			*hdl = PLAYER_NONE;
			return dp_RES_OK;
		case (char)(PLAYER_UNKNOWN & 0xFF):
			*hdl = PLAYER_UNKNOWN;
			return dp_RES_OK;
		case (char)(PLAYER_ME & 0xFF):
			*hdl = PLAYER_ME;
			return dp_RES_OK;
		case (char)(PLAYER_BROADCAST & 0xFF):
			*hdl = PLAYER_BROADCAST;
			return dp_RES_OK;
		default:
			break;
	}

	/* read address */
	if(fread(&adr, dpio->myAdrLen, 1, file) != 1) {
		DPRINT(("dpio_thawHdl: could not read first address.\n"));
		return dp_RES_EMPTY;	/* incorrect file format? */
	}

	/* read 2nd address */
	if(fread(&adr2, dpio->myAdrLen, 1, file) != 1) {
		DPRINT(("dpio_thawHdl: could not read second address.\n"));
		return dp_RES_EMPTY;	/* incorrect file format? */
	}

	/* read flags */
	if (fread(&flags, sizeof(long), 1, file) != 1) {
		DPRINT(("dpio_thawHdll: could not read flags\n"));
		return dp_RES_EMPTY;
	}

	/* get handle for address */
	if (memcmp(adr, adr2, dpio->myAdrLen)) {
		/* addresses are different, open to both addresses */
		*hdl = dpio_openHdlRaw2(dpio, adr, adr2, (flags & comm_PLAYERINFO_2NDLIVE) ? dpio_OPENHDL_RAW_2NDLIVE : 0);
	} else {
		/* addresses are identical, open to only one address */
		*hdl = dpio_openHdlRaw2(dpio, adr, NULL, 0);
	}

	if (*hdl == PLAYER_NONE) {
		DPRINT(("dpio_thawHdl: bug: can't restore a handle?\n"));
		return dp_RES_BUG;	/* Incorrect order for restore? */
	}

	DPRINT(("dpio_thawHdl: handle %d, addr=", *hdl));
	dpio_dprintAdr(adr, dpio->myAdrLen);
	DPRINT((", adr2="));
	dpio_dprintAdr(adr2, dpio->myAdrLen);
	DPRINT((", flags=%d\n", flags));

	return dp_RES_OK;
}

#define dpio_SIG_START "dpio\1\3"
#define dpio_SIG_END   "\7\4dpio"

/*-----------------------------------------------------------------------
 Save the state of a dpio_t to disk, disabling acking.
-----------------------------------------------------------------------*/
dp_result_t dpio_freeze(dpio_t *dpio, FILE *fp)
{
	int i;

	precondition(dpio != NULL);
	precondition(fp != NULL);
	dpio_assertValid(dpio);

	DPRINT(("dpio_freeze: t:%d\n", *dpio->now));
	dpio_print(dpio);

	fwrite(dpio_SIG_START, strlen(dpio_SIG_START)+1, 1, fp);

	/* Save stuff needed to create new dpio */
	fwrite(&dpio->transportDLLname, sizeof(dp_transport_t), 1, fp);
	fwrite(&dpio->commInitReq, sizeof(commInitReq_t), 1, fp);
#ifdef dp_STATS
	fwrite(&dpio->stats, sizeof(dp_stat_t), dpio_NUM_STATS, fp);
#endif
	fwrite(&dpio->publicAddressKnown, sizeof(dpio->publicAddressKnown), 1, fp);

	/* Save stuff needed to restore connection state */
	DPRINT(("dpio_freeze: Saving conns\n"));

	DPRINT(("dpio_freeze: %d Connections\n", dpio->conns->n_used));
	fwrite(&dpio->conns->n_used, sizeof (dpio->conns), 1, fp);

	/*
	 * Save the handles which are in the connection list
	 * Write them in reverse order to swizzle them :)
	 */
	for (i = dpio->conns->n_used - 1; i >= 0; i--) {
		assoctab_item_t *pe;
		dpio_conn_t *pc;

		pe = assoctab_getkey(dpio->conns, i);
		if (!pe) {
			DPRINT(("dpio_freeze: no entry for index %d\n", i));
			return dp_RES_BUG;
		}

		DPRINT(("dpio_freeze: conn %d of %d: %d\n", i + 1, dpio->conns->n_used, pe->key));
		pc = (dpio_conn_t *) &pe->value;
		dpio_pc(dpio, pc);

		/*
		 * Write handle associated with this entry
		 */
		dpio_freezeHdl(dpio, pe->key, fp);

		/*
		 * Write connection entry
		 */
		fwrite(pc, sizeof(dpio_conn_t), 1, fp);
	}

	/* Save local queue messages */
	DPRINT(("dpio_freeze: Saving local queue\n"));
	q_freeze(dpio->localMsgQ, fp);

	fwrite(dpio_SIG_END, strlen(dpio_SIG_END)+1, 1, fp);

	dpio->reliable_enabled = FALSE;
	return dp_RES_OK;
}

/*-----------------------------------------------------------------------
 Restore the state of a dpio_t from disk.
 All reliable connections will pick up where they left off -
 but NOTE: handles may be swizzled around!  To establish the new
 mapping of handles to peers, call dpio_openHdl again with the
 address of each partner; this will return the new handle values.

 Broken into thaw1 "before dploaddll" and thaw2 "after comminit".

 Warning: this implementation assumes that assoctab_t does a linear
 search, and that it's okay to reach in and override the key value.  Bug!
-----------------------------------------------------------------------*/
static dp_result_t dpio_thaw1(dpio_t *dpio, FILE *fp)
{
	char buf[sizeof(dpio_SIG_END)];

	precondition(dpio != NULL);
	precondition(fp != NULL);

	if ((fread(buf, strlen(dpio_SIG_START)+1, 1, fp) != 1)
	||  memcmp(buf, dpio_SIG_START, strlen(dpio_SIG_START))) {
		DPRINT(("dpio_thaw: no start signature\n"));
		return dp_RES_BAD;
	}

	if ((fread(&dpio->transportDLLname, sizeof(dp_transport_t), 1, fp) != 1)
	||  (fread(&dpio->commInitReq, sizeof(commInitReq_t), 1, fp) != 1)
#ifdef dp_STATS
	||  (fread(&dpio->stats, sizeof(dp_stat_t), dpio_NUM_STATS, fp) != dpio_NUM_STATS)
#endif
	||  (fread(&dpio->publicAddressKnown, sizeof(dpio->publicAddressKnown), 1, fp) != 1)
	) {
		DPRINT(("dpio_thaw: short\n"));
		return dp_RES_BAD;
	}
	return dp_RES_OK;
}

/*-----------------------------------------------------------------------
-----------------------------------------------------------------------*/
static dp_result_t dpio_thaw2(dpio_t *dpio, FILE *fp)
{
	char buf[sizeof(dpio_SIG_END)];
	int i;
	int	num;

	precondition(dpio != NULL);
	precondition(fp != NULL);

	/*
	 * Restore tables according to old handle values.
	 */
	if ((fread(&num, sizeof (num), 1, fp) != 1)) {
		DPRINT(("dpio_thaw2: number of connections missing\n"));
		return dp_RES_BAD;
	}

	/*
	 * Thaw out the handles that are in the connection table
	 * For each connection, open a comm handle and write over
	 * the retrieved comm handle
	 */
	for (i = 0; i < num; i++) {
		playerHdl_t h;
		dpio_conn_t *pc;

		DPRINT(("dpio_thaw2: conn %d of %d\n", i + 1, num));

		/*
		 * Thaw out the handle
		 */
		if (dpio_thawHdl(dpio, &h, fp) != dp_RES_OK) {
			DPRINT(("dpio_thaw2: could not thaw handle\n"));
			continue;
		}

		/*
		 * Add the thawed handle to the connection table
		 */
		pc = (dpio_conn_t *) assoctab_subscript_grow(dpio->conns, h);

		/*
		 * Read connection from freeze file
		 */
		if (fread(pc, sizeof (dpio_conn_t), 1, fp) != 1) {
			DPRINT(("dpio_thaw2: could not read connection\n"));
			return dp_RES_BAD;
		}

		/*
		 * Display donnection
		 */
		dpio_pc(dpio, pc);

		/*
		 * Reset the timeout counter- else he'll time out on first check.
		 */
		pc->rx.PktTime = *dpio->now;
	}

	/*
	 * Restore local queue.
	 */
	if (q_thaw(dpio->localMsgQ, fp)) {
		DPRINT(("dpio_thaw2: unable to restore local queue.\n"));
		return dp_RES_BAD;
	}

	if ((fread(buf, strlen(dpio_SIG_END)+1, 1, fp) != 1) ||
			memcmp(buf, dpio_SIG_END, strlen(dpio_SIG_END))) {
		DPRINT(("dpio_thaw2: no end signature\n"));
		return dp_RES_BAD;
	}

	DPRINT(("dpio_thaw2: "));
	dpio_print(dpio);

	dpio_assertValid(dpio);
	return dp_RES_OK;
}

/*------------------------------------------------------------------------
 For debugging purposes only.
 Force dpio to drop a given percentage of all packets (randomly) on
 reception.
-----------------------------------------------------------------------*/
void dpio_forceRxDropPercent(dpio_t *dpio, int rxDropPercent)
{
	dpio_assertValid(dpio);
	DPRINT(("dpio_forceRxDropPercent: setting packet loss to %d%%\n", rxDropPercent));
	dpio->rxDropPercent = rxDropPercent;
}

/*------------------------------------------------------------------------
 Set the intervals used by the latency measurement system.
 If both intervals are 0, only ACKs of non-retransmitted reliable packets
	will generate new measurements.  No extra bandwidth is used, but
	measurements may be infrequent if there are few reliable packets or
	there is heavy packet loss.
 If piggbackPingInterval is set, additional small ping packets and
	responses will be piggybacked onto gathered packets every
	piggybackPingInterval ms when possible.  No extra physical packets
	will be generated, and any network traffic at all will generate
	latency and packet loss measurements.
 If forcedPingInterval is set and no piggybacked pings have been sent in
	the past forcedPingInterval ms, an additional ping packet will be
	sent to get a latency measurement.

 Pings will never be sent more than once per round trip time, regardless
 of the intervals requested, and are only sent to handles with the
 appropriate mode bit set (dpio_OPTION_PIGGYBACK_PING or
 dpio_OPTION_FORCED_PING) using dpio_setHandleOptions().

 Regardless of round trip time or the parameters to this function, pings
 will not be sent more often than is reasonable.

 Returns dp_RES_OK on success, or dp_RES_BAD on bad parameters.
------------------------------------------------------------------------*/
dp_result_t dpio_setPingIntervals(
	dpio_t *dpio,
	int piggybackPingIntervalMS,
	int forcedPingIntervalMS)
{
	if (!dpio || (piggybackPingIntervalMS < 0) || (forcedPingIntervalMS < 0))
		return dp_RES_BAD;
	if (piggybackPingIntervalMS <= 200)
		piggybackPingIntervalMS = 200;
	if (forcedPingIntervalMS <= 1000)
		forcedPingIntervalMS = 1000;
	dpio->ping_piggyback_interval = (piggybackPingIntervalMS * dpio->clocksPerSec)/1000;
	dpio->ping_forced_interval = (forcedPingIntervalMS * dpio->clocksPerSec)/1000;
	if (dpio->ping_forced_interval > dpio_keepaliveInterval(dpio))
		dpio->ping_forced_interval = dpio_keepaliveInterval(dpio);
	if (dpio->ping_piggyback_interval > dpio->ping_forced_interval)
		dpio->ping_piggyback_interval = dpio->ping_forced_interval;

	DPRINT(("dpio_setPingIntervals: set intervals: piggyback:%d forced:%d keepalive:%d\n", dpio->ping_piggyback_interval, dpio->ping_forced_interval, dpio_keepaliveInterval(dpio)));
	dpio_assertValid(dpio);
	return dp_RES_OK;
}

/*------------------------------------------------------------------------
 Set the realtime clock speed, and all the retransmission and timeout
 intervals.
 Called from dpio_create; dpio is in an invalid state during this time.
-----------------------------------------------------------------------*/
void dpio_set_clocks(dpio_t *dpio, int clocksPerSec)
{
#if 0
	commSetParamReq_t req;
	commSetParamResp_t resp;

	/* wattcp needs to know how fast the 18.2Hz system clock really is. */
	/* This is for DOS only, but shouldn't hurt others */
	req.reqLen = sizeof(commSetParamReq_t);
	req.param_num = comm_PARAM_TICKSPERSEC;
	req.param_value = (182L * clocksPerSec) / (10 * ECLOCKS_PER_SEC);
	if (!commSetParam(&req, &resp, dpio->commPtr))
		DPRINT(("dpio_set_clocks: driver couldn't set tickspersec, status %d\n", resp.status));
#endif

	dpio->clocksPerSec = clocksPerSec;

	/* minTxInterval sets how often we check for retransmission
	 * AND the minimum RTO value as well.  This may be wrong; we may want
	 * to check for retransmission faster than the minimum RTO.
	 *
	 * minTxInterval must be quite small, so that fast games work well.
	 * the RTT calculation will take care of slower networks and games.
	 */
	dpio->minTxInterval = clocksPerSec / 50;
	if (dpio->minTxInterval < 1) dpio->minTxInterval = 1;

	/* latency is our 'best guess' at a starting RTT for each connection
	 * This will be totally overridden by the first real RTT measurement.
	 * Choose 2 seconds to prevent syn retx meltdown on startup, as observed
	 * in src/tests/psp.c with n=8 and the loopback driver wloop.dll.
	 */
	dpio->latency = clocksPerSec * 2;

	/* maxRxInterval controls the handle timeout. */
	dpio->maxRxInterval = clocksPerSec * dp_PLAYER_TIMEOUT_SECS;

	{
		const char *s;
		dpini_findSection("Debug");
		s = dpini_readParameter("timeout", FALSE);
		if (s && *s) {
			int timeout_value = atoi(s);

			if (timeout_value >= 10) {
				dpio->maxRxInterval = clocksPerSec * timeout_value;
			} else {
				DPRINT(("dpio_create: [DEBUG]/timeout must be >= 10, was %s\n", s));
			}
			DPRINT(("dpio_create: Handle timeout now %d seconds\n", dpio->maxRxInterval / clocksPerSec));
		}
	}
	/* maxTxInterval controls the maximum RTO, and thus must be
	 * less than the handle timeout, but more than minTxInterval.
	 */
	dpio->maxTxInterval = dpio->maxRxInterval / 10;

	if (dpio->maxTxInterval < dpio->minTxInterval)
		dpio->maxTxInterval = dpio->minTxInterval * 4;

	if (dpio->maxTxInterval > dpio->maxRxInterval / 2)
		DPRINT (("dpio_set_clocks: warning: maxTxInterval too large\n"));
	DPRINT(("dpio_set_clocks: cps:%d minTx:%d maxTx:%d maxRx:%d lat:%d\n",
		clocksPerSec, dpio->minTxInterval, dpio->maxTxInterval, dpio->maxRxInterval, dpio->latency));
}

/*--------------------------------------------------------------------------
 This function has been superceeded by dpio_hdl2adr2 (below)
--------------------------------------------------------------------------*/
dp_result_t dpio_hdl2adr(dpio_t *dpio, playerHdl_t h, void *adr, int *len)
{
  return (dpio_hdl2adr2(dpio, h, adr, NULL, len));
}

/*--------------------------------------------------------------------------
 Find out the network address for a given handle opened with dpio_openHdl2.
 Called from dpio_create; dpio is in an invalid state during this time.
--------------------------------------------------------------------------*/
dp_result_t dpio_hdl2adr2(dpio_t *dpio, playerHdl_t h, void *adr, void *adr2, int *len)
{
	commPlayerInfoReq_t  req;
	commPlayerInfoResp_t resp;

	req.player = h;
	memset(&resp, 0x00, sizeof (resp));
	if (!commPlayerInfo(&req, &resp, dpio->commPtr))
	{
		DPRINT(("dpio_hdl2adr: commPlayerInfo failed. status %d\n", resp.status));
		return dp_RES_BAD;
	}
	memset(adr, 0, *len);
	*len = resp.addrLen;
	memcpy(adr, resp.address, resp.addrLen);

	/* If there a second address was asked for try to copy it over also */
	if (adr2)
	{
		if (resp.status == comm_STATUS_DUAL_ADDRESS)
		{
			memcpy(adr2, resp.address2, resp.addrLen);
		}
		else
		{
			memcpy(adr2, resp.address, resp.addrLen);
		}
	}

	DPRINT(("dpio_hdl2adr: h:%x len:%d Adr: ", h, *len));
	dpio_dprintAdr((char *)adr, resp.addrLen);
	DPRINT(("\n"));
	(void) dpio;
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Find out the player name for a given handle opened with dpio_openHdl.
 "name" is a pointer to the buffer to copy the name to,
 "name_size" is the number of bytes available in that buffer.
 If name_size <= strlen(player name), returns dp_RES_FULL.
 Only supported by transports which have the comm_DRIVER_KNOWS_PLAYERLST
 bit set in comm_driverInfo_t.capabilities,
--------------------------------------------------------------------------*/
dp_result_t dpio_hdl2name(dpio_t *dpio, playerHdl_t h, char *name, size_t name_size)
{
	commPlayerInfoReq_t  req;
	commPlayerInfoResp_t resp;

	req.player = h;
	memset(&resp, 0x00, sizeof (resp));
	if (!commPlayerInfo(&req, &resp, dpio->commPtr)) {
		DPRINT(("dpio_hdl2name: commPlayerInfo failed. status %d\n",resp.status));
		return dp_RES_BAD;
	}
	if (name_size <= strlen(resp.name)) {
		DPRINT(("dpio_hdl2name: name_size %d too small to hold %s\n",name_size, resp.name));
		return dp_RES_FULL;
	}
	memset(name, 0, name_size);
	strcpy(name, resp.name);
	DPRINT(("dpio_hdl2name: h:%x name: %s\n", h, name));
	(void) dpio;
	return dp_RES_OK;
}

/*-----------------------------------------------------------------------
 Deallocate all resources allocated by dpio_create.
 Shut down the network API.
 If flags is nonzero, and the network is a modem, don't hang up.
-----------------------------------------------------------------------*/
void dpio_destroy(dpio_t *dpio, long flags)
{
	commTermReq_t req;

	dpio_assertValid(dpio);

	/* Shut down transport */
	req.flags = flags;
	DPRINT(("dpio_destroy: commTerm\n"));
	commTerm(&req, NULL, dpio->commPtr);	/* This line must come before you unload the DLL - KA 1-29-95 */

	/* Unload transport */
	DPRINT(("dpio_destroy: t:%d unload DLL\n", *dpio->now));
	dpUnloadDLL(dpio);

	/* Free buffers */
	DPRINT(("dpio_destroy: deallocating buffers\n"));
	if (dpio->localMsgQ) q_destroy(dpio->localMsgQ);
	if (dpio->conns) assoctab_destroy(dpio->conns);

	dpio_log_term();

#ifdef DP_MODEL_DELAY
	if (dpio->delay_q) {
		delay_destroy(dpio->delay_q);
		dpio->delay_q = NULL;
	}
#endif

	memset(dpio, 0, sizeof(dpio_t));
	dp_FREE(dpio);

	DPRINT(("dpio_destroy: done.\n"));
}

/*------------------------------------------------------------------------
 Create a dpio_t.  Initialize the network API.

 On entry:
 pdpio is the address of an uninitialized dpio_t *.
 transportDLLname is the path to the desired transport DLL (e.g. serial.dll).
 commInitReq is filled with the comm parameters (e.g. .baud = 9600).
 now is a pointer to a variable that contains the current value of eclock().

 If thawfp is not NULL,
 commInitReq should be NULL (comm params will be read from thawfp),
 commDLLName may be NULL or an empty string (it defaults to the value
 read from thawfp),
 and the dpio's connection state is restored from the file.

 On successful exit:
 Initializes *pdpio, and returns dp_RES_OK.

 On failure:
 Returns a dp_RES_* code indicating the reason for failure.
-----------------------------------------------------------------------*/
dp_result_t dpio_create(dpio_t **pdpio, const dp_transport_t *transportDLLname,
	commInitReq_t *commInitReq, const clock_t *now, FILE *thawfp)
{
	commInitResp_t	commInitResp;
	commDriverInfoResp_t commInfoResp;
	dp_result_t err;
	dpio_t *dpio;

	/* Check parameters */
	if ((!transportDLLname || !transportDLLname->fname[0]) && !thawfp) {
		DPRINT(("dpio_create: must specify a transport or a thaw fileptr\n"));
		return dp_RES_BAD;
	}

	/* Initialize structure */
	dpio = (dpio_t *)dp_MALLOC(sizeof(dpio_t));
	if (!dpio) return dp_RES_NOMEM;

	memset(dpio, 0, sizeof(dpio_t));

	if (thawfp) {
		/* If thawing, get transport name and parameters. */
		err = dpio_thaw1(dpio, thawfp);
		if (err != dp_RES_OK)
			return err;
		/* But allow caller to override transport name. */
		if (transportDLLname && transportDLLname->fname[0])
			memcpy(&dpio->transportDLLname, transportDLLname, sizeof(dp_transport_t));
	} else {
		const char *s;

		/* Save transport name and parameters in dpio_t. */
		memcpy(&dpio->transportDLLname, transportDLLname, sizeof(dp_transport_t));
		if (commInitReq)
			dpio->commInitReq = *commInitReq;
		else
			memset((char *)&dpio->commInitReq, 0, sizeof(dpio->commInitReq));

		/* ini file can override UDP port number selected by program */
		dpini_findSection("Debug");
		s = dpini_readParameter("portnum", FALSE);
		if (s && *s) {
			unsigned short portnum = atoi(s);

			if (portnum < 1024) {
				DPRINT(("dpio_create: [DEBUG]/portnum must be >= 1024, was %s\n", s));
			} else {
				comm_driverInfo_t info;
				err = dpGetTransportInfo(&dpio->transportDLLname, &info);
				if ((err == dp_RES_OK)
				&&  (info.capabilities & comm_DRIVER_NO_BROADCAST)) {
					dpio->commInitReq.portnum = portnum;
					DPRINT(("dpio_create: portnum is now %d\n", portnum));
				}
			}
		}
	}

	/* Allocate loopback message queue. */
	dpio->localMsgQ  = q_create();
	/* Allocate buffers indexed by comm handle. */
	dpio->conns = assoctab_create(sizeof(dpio_conn_t));
	if (!dpio->localMsgQ || !dpio->conns)
		return dp_RES_NOMEM;

	dpio->nextTxTime = *now;
	dpio->nextFlush = *now;
	dpio->now = now;
	dpio->callback = NULL;
	dpio->context = NULL;

	/* Load transport */
	DPRINT(("dpio_create: t:%d %s, Calling dpLoadDLL.\n", *now, dpio->transportDLLname.fname));
	err = dpLoadDLL(dpio, &dpio->transportDLLname);
	if (err != dp_RES_OK) {
		DPRINT(("dpCreate: dpLoadDLL returns %d\n", err));
		dp_FREE(dpio);
		return err;
	}

#ifdef UNIX
	/* Allocate Transport data (need to to test to see if this function exists in driver ! */
	{
		commAllocReq_t	req;
		commAllocResp_t	resp;

		req.ptr = &dpio->commPtr;

		if (!commAlloc(&req, &resp) || resp.status != comm_STATUS_OK)
		{
			DPRINT(("dpio_create: Driver failed to allocate instance data!\n"));
			return dp_RES_BAD;
		}
	   	DPRINT(("dpio_create: comm magic : %x\n", (unsigned long *) dpio->commPtr));
	}
#endif

	/* Initialize transport */
	if (thawfp)
		dpio->commInitReq.flags |= comm_INIT_FLAGS_RESUME;
	else
		dpio->commInitReq.flags &= ~comm_INIT_FLAGS_RESUME;

#ifdef _WIN32
	{	char strSessionId[256];

		if (!GetEnvironmentVariable(DPSTUBID, strSessionId, 256)) {
			DPRINT(("dpio_create: could not find stub\n"));
		} else {
			sscanf(strSessionId, "%x", &(dpio->commInitReq.sessionId));
			DPRINT(("dpio_create: set session id to %s = %x\n", strSessionId, dpio->commInitReq.sessionId));
		}
	}
#endif
	DPRINT(("dpio_create: calling commInit. baud = %ld, portnum = %ld, sessionid = %x\n",
		dpio->commInitReq.baud, dpio->commInitReq.portnum,dpio->commInitReq.sessionId));
	/* tell driver where our dp_dprintf is.  It's a little weird to call
	 * commSetParam before calling commInit, but it's the only way
	 * to get DPRINTs in commInit to show up.
	 */
	{
		commSetParamReq_t paramReq;
		int ok;

		paramReq.param_num = comm_PARAM_DPRINTF;
		paramReq.param_value = (long)dp_dprintf;
		paramReq.reqLen = sizeof(paramReq);
		ok = commSetParam(&paramReq, NULL, dpio->commPtr);
		DPRINT(("dpio_create: before commInit: commSetParam (comm_PARAM_DPRINTF,) returns ok=%d\n", ok));
	}
	if (!commInit(&dpio->commInitReq, &commInitResp, dpio->commPtr)) {
		DPRINT(("Unable to init comm system, error: %d\n", commInitResp.status));
		dpUnloadDLL(dpio);
		dp_FREE(dpio);
		if (commInitResp.status == comm_STATUS_BUSY)
			return dp_RES_MODEM_BUSY;
		if (commInitResp.status == comm_STATUS_NO_ANSWER)
			return dp_RES_MODEM_NOANSWER;
		if (commInitResp.status == comm_STATUS_NO_DIALTONE)
			return dp_RES_MODEM_NODIALTONE;
		if (commInitResp.status == comm_STATUS_NO_RESPONSE)
			return dp_RES_MODEM_NORESPONSE;
		if (commInitResp.status == comm_STATUS_NETWORK_NOT_PRESENT)
			return dp_RES_NETWORK_NOT_PRESENT;
		if (commInitResp.status == comm_STATUS_BAD_VERSION)
			return dp_RES_COMMDLL_BAD_VERSION;
		if (commInitResp.status == comm_STATUS_FULL)
			return dp_RES_NETWORK_NOT_RESPONDING;
		if (commInitResp.status == comm_STATUS_BAD_INITSTRING)
			return dp_RES_MODEM_BAD_INITSTRING;
		return dp_RES_BUSY;
	}
	/* tell driver where our dp_dprintf is */
	{
		commSetParamReq_t paramReq;
		int ok;

		paramReq.param_num = comm_PARAM_DPRINTF;
		paramReq.param_value = (long)dp_dprintf;
		paramReq.reqLen = sizeof(paramReq);
		ok = commSetParam(&paramReq, NULL, dpio->commPtr);
		DPRINT(("dpio_create: commSetParam (comm_PARAM_DPRINTF,) returns ok=%d\n", ok));
	}

	/* One for each player in the game, and 5 for lurkers */
	dpio->max_playerHdls = dp_MAXREALPLAYERS + 5;

	/* Save capability bits for later.  Hmm.  Should just make these part of
	 * commInit...
	 */
	if (!commDriverInfo(NULL, &commInfoResp, dpio->commPtr)) {
		DPRINT(("dpio_create: Unable to get comm info, status:%d\n", commInfoResp.status));
		return dp_RES_BUG;
	}
	dpio->driverinfo = commInfoResp.info;

	/* Set the throttling and timeout parameters for dpio_update */
	/* This assumes that dpio->now points to a variable holding the
	 * current value of eclock() at all times.
	 */
	dpio_set_clocks(dpio, ECLOCKS_PER_SEC);

	/* Set default ping intervals in case anyone turns on pings but doesn't
	 * set the intervals themselves.
	 */
	dpio_setPingIntervals(dpio, 4000, 8000);

	/*
	 * Save the detected address(es)
	 */
	dpio->myAdrLen = sizeof(dpio->myAdr);
	if (dpio_hdl2adr2(dpio, PLAYER_ME, dpio->myAdr, dpio->myAdr2, &dpio->myAdrLen) != dp_RES_OK)
	{
		DPRINT(("dpio_create: Can't get me own address!\n"));
		dpio_destroy(dpio, 0);
		return dp_RES_BUG;
	}

	DPRINT(("dpio_create: myAdr = "));
	dpio_dprintAdr(dpio->myAdr, dpio->myAdrLen);
	if (memcmp(dpio->myAdr2, dpio->myAdr, dpio->myAdrLen))
	{
		DPRINT((", "));
		dpio_dprintAdr(dpio->myAdr2, dpio->myAdrLen);
	}
	DPRINT(("\n"));

#if 0
	DPRINT(("dpio_create: zeroing myAdr to test syn myadr hack\n"));
	memset(dpio->myAdr, 0, dpio->myAdrLen);
#endif

	if (thawfp) {
		/* If thawing, restore old connections. */
		err = dpio_thaw2(dpio, thawfp);
		if (err != dp_RES_OK) {
			dpio_destroy(dpio, 0);
			return err;
		}
	}

	/* Look at .ini file for packet loss settings! */
	{
		const char *s;
		dpini_findSection("Debug");
		s = dpini_readParameter("pktloss", FALSE);
		if (s && *s) {
			int percent;

			if ((1==sscanf(s, "%d", &percent))
			&& (percent >= 0)
			&& (percent <= 100))
				dpio->rxDropPercent = percent;
			else
				DPRINT(("dpio_create: [DEBUG]/pktloss must be between 0 and 100, was '%s' = %d\n", s, percent));
		}
		s = dpini_readParameter("fault", FALSE);
		if (s && *s) {
			if (atoi(s)) {
				long *x;
				DPRINT(("dpio_create: generating null pointer fault\n"));
				x = (long *)NULL;
				*x = 0x12345678;
			}
		}
	}

	DPRINT(("dpio_create: t:%d %s, dpio %p, success\n", *dpio->now, transportDLLname->fname, dpio));
	dpio->reliable_enabled = TRUE;

	dpio_log_init();

#ifdef DP_MODEL_DELAY
	/* Get delay parameters from .ini file */
	{
		const char *s;
		int bps;
		int backbone_ms;

		dpini_findSection("Debug");
		/* Load-dependant delay - a modem delays data by an amount
		 * that depends on how many bits/second it can handle.
		 * 20000 is a good default guess at how many bits/second the average
		 * modem can handle.
		 */
		s = dpini_readParameter("bitsPerSec", FALSE);
		if (!s
		|| !*s
		|| (1!=sscanf(s, "%d", &bps))
		|| (bps < 0)
		|| (bps > 100000000)) {
			if (s && *s) DPRINT(("dpio_create: [DEBUG]/bitsPerSec must be between 0 and 100 million, was '%s' = %d\n", s, bps));
			/*bps = 20000;*/
			bps = 0;
		}
		/* Load-independant delay - the backbone delays packets a
		 * certain amount regardless of how many bits/second your modem
		 * can handle.
		 * 70 ms is a good default guess for this.
		 */
		s = dpini_readParameter("backbone_ms", FALSE);
		if (!s
		|| !*s
		|| (1!=sscanf(s, "%d", &backbone_ms))
		|| (backbone_ms < 0)
		|| (backbone_ms > 5000)) {
			if (s && *s) {
				DPRINT(("dpio_create: [DEBUG]/backbone_ms must be between 0 and 5000, was '%s' = %d\n", s, backbone_ms));
			}
			/*backbone_ms = 70;*/
			backbone_ms = 0;
		}
		if (backbone_ms || bps) {
			dpio->delay_q = delay_create();
			if (!dpio->delay_q)
				return dp_RES_NOMEM;
			delay_setBytesPerSec(dpio->delay_q, bps/8);
			delay_setDelayMillisec(dpio->delay_q, backbone_ms);
			DPRINT(("dpio_create: modem delay simulation: bitsPerSec=%d, backbone_ms=%d\n", bps, backbone_ms));
		} else {
			dpio->delay_q = NULL;
			DPRINT(("dpio_create: modem delay simulation disabled.\n"));
		}
	}
#endif

	/*
	 * The following is somewhat of a kludge!
	 * Yes, dpio is usig data intended for the comm layer
	 */
	if (commInitReq) {
		/*
		 * Are we being request to open connections to addresses ?
		 */
		if (commInitReq->flags & comm_INIT_FLAGS_CONN_ADDR)	{
			char *ptr;

			ptr = (char *) commInitReq->dialing_method;

			assert(ptr);

			while (*ptr) {
				char				adr[dp_MAX_ADR_LEN];
				commScanAddrReq_t	scanReq;
				commScanAddrResp_t	scanResp;

				DPRINT(("dpio_create: opening handle to \"%s\"\n", ptr));

				/*
				 * Convert the textural address into a binary one
				 */
				scanReq.address = adr;
				scanReq.size = dp_MAX_ADR_LEN;
				scanReq.printable = ptr;

				if (!commScanAddr(&scanReq, &scanResp, dpio->commPtr) || scanResp.status != comm_STATUS_OK) {
					DPRINT(("dpio_create: could not translate address \"%s\"\n", ptr));
				} else {
					/*
					 * Open handle to address
					 */
					if (dpio_openHdl(dpio, adr, NULL, NULL) == PLAYER_NONE)	{
						DPRINT(("dpio_create: could not open handle to \"%s\"\n", ptr));
					}
				}
				ptr += strlen(ptr) + 1;
			}
		}
	}

	*pdpio = dpio;
	dpio_assertValid(dpio);

	if (commInitResp.status == comm_STATUS_OK_AM_MASTER) {
		DPRINT(("dpio_create: commInit says I am the master\n"));
		return comm_STATUS_OK_AM_MASTER;
	}
	return dp_RES_OK;
}

/*-----------------------------------------------------------------------
 Place a packet in the tx window of a connection for acking and
 retransmission, and send it once.
 Returns dp_RES_OK if it was able to queue the packet.
 Note: all 16-bit quantities in wrapped_data_packets are in Intel byte
 order!  pktnum is swapped here at runtime, and tag is swapped at
 compile time in anet.h or dppkt.h.
-----------------------------------------------------------------------*/
dp_result_t dpio_q_packet(
	dpio_t *dpio,
	playerHdl_t h,
	dpio_conn_t *pc,
	dp_packetType_t tag,
	void *buffer,
	size_t size)
{
	int winpos;
	dpio_window_t *pw;
	dpio_wrapped_data_packet_t *p;
	dp_result_t err;
	unsigned short nq;
	unsigned short pktnum;

	dpio_assertValid(dpio);
	if (!pc)
		return dp_RES_BAD;	/* Invalid handle */

	if ((pc->state & dpio_STATE_CLOSED) == dpio_STATE_CLOSED) {
		DPRINT(("dpio_q_packet: tried to queue to closed h:%x, state:%x\n",
			h, pc->state));
		dpio_assertValid(dpio);
		return dp_RES_PEER_CLOSED;
	}
	if ((pc->state & dpio_STATE_CLOSED)
	&&  (tag != dpio_FIN_PACKET_ID) && (tag != dpio_SYN_PACKET_ID)) {
		DPRINT(("dpio_q_packet: tried to queue tag %2.2s to closing h:%x, state:%x\n",
			(char *) &tag, h, pc->state));
		dpio_assertValid(dpio);
		return dp_RES_PEER_CLOSED;
	}

	/* Locate the window. */
	pw = &pc->tx;

	nq = (pw->next_pktnum - pw->windowBase);	/* be sure to wrap around right */
	if (nq == dpio_WINDOWSIZE) {
		DPRINT(("dpio_q_packet: dest h:%x full\n", h));
		dpio_assertValid(dpio);
		return dp_RES_FULL;
	}

	/* Place this packet in the window, in case it gets dropped and
	 * needs to be retransmitted.
	 */
	winpos = pw->next_pktnum % dpio_WINDOWSIZE;
	pw->bAcked[winpos] = FALSE;
	pw->pktQTime[winpos] = *dpio->now;
	pw->nRetries[winpos] = 0;
	/* Set the next retransmit time if we're adding to an empty window */
	if (pw->windowBase == pw->next_pktnum) dpio_setNextTxTime(dpio, pc);

	p = &pw->outstanding[winpos];
	p->tag = tag;
	assert(size <= UCHAR_MAX);
	p->body.len = (unsigned char) size;
	pktnum = pw->next_pktnum++;
	nq++;
	p->body.pktnum = SwapBytes2(pktnum);
	if (pw->next_pktnum == 0)
		DPRINT(("dpio_q_packet: tx wrapped to pktnum 0 (normal)\n"));
	memcpy(p->body.data, buffer, size);

	/*DPRINT(("dpio_q_packet: cleared ack %d: ", j)); dpio_pw(pw); */
	/* If the connection isn't established yet, just queue up the data
	 * in the window (unless this is a SYN packet).  Hope caller doesn't
	 * overflow window during startup; caller would then be told dp_RES_FULL.
	 */
	if ((tag != dpio_SYN_PACKET_ID)
	&&  ((pc->state & dpio_STATE_ESTABLISHED) != dpio_STATE_ESTABLISHED)) {
		DPRINT(("dpio_q_packet: queueing; tag %2.2s ln %d pktnum %hu h:%x state:%x nq %hu\n",
			(char *) &tag, size, pktnum, h, pc->state, nq));
		dpio_assertValid(dpio);
		return dp_RES_OK;
	}

	pw->nRetries[winpos] = 1;
	/*dumpBuf((char *)p, pktlen);*/
	/* Send the packet once now to the current destination. */
	err = dpio_put_unreliable_buffered(dpio, pc, h, p,
		sizeof(p->tag) + sizeof(p->body.pktnum) + sizeof(p->body.len) + size);

	DPRINT(("dpio_q_packet: t:%d sent tag %2.2s ln %d pktnum %hu h:%x status %d nq %hu\n",
		*dpio->now, (tag != dpio_SYN_PACKET_ID) ? buffer : (char *) &tag, size, pktnum, h, err, nq));
	dpio_assertValid(dpio);
	return dp_RES_OK;
}

/*-----------------------------------------------------------------------
 Get the remaining number of packets free in the TX queue for a handle.
 Returns -1 on error.
-----------------------------------------------------------------------*/
int dpio_getHdlTxPktsFree(dpio_t *dpio, playerHdl_t h)
{
	dpio_conn_t *pc;
	dpio_window_t *pw;
	unsigned short nq;

	dpio_assertValid(dpio);

	/* If it's a predefined handle, return an appropriate value */
	if ((h == PLAYER_ME) || (h == PLAYER_BROADCAST))
		return 1;
	if ((h == PLAYER_NONE) || (h == PLAYER_UNKNOWN))
		return -1;

	pc = (dpio_conn_t *) assoctab_subscript (dpio->conns, h);
	if (!pc) {
		DPRINT(("dpio_getHdlTxFree: bogus h:%x!\n", h));
		dpio_assertValid(dpio);
		return -1;
	}
	pw = &pc->tx;
	assert((pw->next_pktnum - pw->windowBase) <= SHRT_MAX);	/* protect conversion */
	nq = (short)(pw->next_pktnum - pw->windowBase);
	dpio_assertValid(dpio);
	return dpio_WINDOWSIZE - nq;
}

/*-----------------------------------------------------------------------
 Get the current state of a connection by player handle
-----------------------------------------------------------------------*/
short dpio_getHdlState(dpio_t *dpio, playerHdl_t h)
{
	dpio_conn_t *pc;

	dpio_assertValid(dpio);

	/* If it's a predefined handle, return an appropriate value */
	if ((h == PLAYER_ME) || (h == PLAYER_BROADCAST))
		return dpio_STATE_ESTABLISHED;
	if ((h == PLAYER_NONE) || (h == PLAYER_UNKNOWN))
		return dpio_STATE_CLOSED;

	pc = (dpio_conn_t *) assoctab_subscript (dpio->conns, h);
	if (!pc) {
		DPRINT(("dpio_getHdlState: bogus h:%x!\n", h));
		dpio_assertValid(dpio);
		return dpio_STATE_CLOSED;
	}
	dpio_assertValid(dpio);
	return pc->state;
}

/*-----------------------------------------------------------------------
 Get the current age in eclock units of a connection by player handle
 This number goes to zero when we receive a reliable packet or keepalive,
 and increases thereafter.
 Returns 0 for PLAYER_ME;
 returns -1 for other predefined handles;
 returns -2 for bad handle.
-----------------------------------------------------------------------*/
clock_t dpio_getHdlAge(dpio_t *dpio, playerHdl_t h)
{
	dpio_conn_t *pc;

	dpio_assertValid(dpio);

	/* If it's a predefined handle, return an appropriate value */
	if (h == PLAYER_ME)
		return 0;
	if ((h == PLAYER_NONE) || (h == PLAYER_UNKNOWN)|| (h == PLAYER_BROADCAST))
		return (clock_t)-1;

	pc = (dpio_conn_t *) assoctab_subscript (dpio->conns, h);
	if (!pc) {
		DPRINT(("dpio_getHdlAge: bogus h:%x!\n", h));
		dpio_assertValid(dpio);
		return (clock_t)-2;
	}
	dpio_assertValid(dpio);
	return *(dpio->now) - pc->rx.PktTime;
}

/*-----------------------------------------------------------------------
 Get the remote capabilies of a connection by player handle
 This is used by dptab to see if the other side is recent enough to
 handle the new style of dptab_delete.
 The return value is a bitfield full of dpio_REMCAP_*.
-----------------------------------------------------------------------*/
short dpio_getHdlRemoteCapabilities(dpio_t *dpio, playerHdl_t h)
{
	dpio_conn_t *pc;
	short remcap = 0;

	dpio_assertValid(dpio);

	pc = (dpio_conn_t *) assoctab_subscript (dpio->conns, h);
	if (!pc) {
		DPRINT(("dpio_getHdlRemoteCapabilities: bogus h:%x!\n", h));
	} else
		remcap = pc->remote_capabilities;
	DPRINT(("dpio_getHdlRemoteCapabilities: h:%x has remcap %x\n", h, remcap));
	dpio_assertValid(dpio);
	return remcap;
}


/*-----------------------------------------------------------------------
 This function has be superceeded by dpio_openHdl2 (below)
-----------------------------------------------------------------------*/
playerHdl_t dpio_openHdl(dpio_t *dpio, void *adr, dpioOpenHdlCallback_t cb, void *context)
{
  /* dummy usage of params */
  (void) cb;
	(void) context;

  /* use enhanced function to do the work */
  return (dpio_openHdl2(dpio, adr, NULL));
}

/*-----------------------------------------------------------------------
 Given a network address, open a handle to it.

 If a handle is already associated with that address,
 returns the handle associated with it.

 If no handle is currently associated with that address,
 establishes a connection, initializes the TX handshake
 buffer, sends an initial handshake packet, and returns the new handle.
 Once the other side acknowledges the handshake, the caller is
 free to send data to the new handle.  (Until then, it will be queued.)

 If the second address is supplied then both addresses will be passed
 down to the COMM layer.  Dual addresses are useful when connecting to
 machines which are known to have multiple addresses.  Refer to header
 for more infomation.
-----------------------------------------------------------------------*/
playerHdl_t dpio_openHdl2(dpio_t *dpio, void *adr, void *adr2)
{
	playerHdl_t h;
	dpio_conn_t *pc;
	dpio_window_t *pw;
	unsigned char data[dpio_MAXLEN_RELIABLE];
	dp_result_t err;
	int pktlen;
	char mycap;

	/* precondition(cb == NULL); */
	/* precondition(context == NULL); */
	dpio_assertValid(dpio);

	h = dpio_openHdlRaw2(dpio, adr, adr2, TRUE);

	/* If the handle is predefined, or already existed, just return it. */
	if ((h == PLAYER_ME) || (h == PLAYER_NONE) || (h == PLAYER_UNKNOWN) || (h == PLAYER_BROADCAST)) {
		/* Don't forget to call the callback. */
		/*if (cb)
			cb(h, dpio->conns->n_used, dp_RES_OK, context);*/
		if (dpio->callback)
			dpio->callback(h, dpio->conns->n_used, dp_RES_OK, dpio->context);
		dpio_assertValid(dpio);
		return h;
	}
	pc = (dpio_conn_t *)assoctab_subscript(dpio->conns, h);
	if (pc) {
		if (!(pc->state & dpio_STATE_CLOSED)) {
			/* If the handle is closing, we'll proceed and reopen it,
			 * sending a SYN and (hopefully) causing a dpio_STATE_RST_RCVD
			 * at the other end.
			 * Otherwise, if the handle is ESTABLISHED, call the callback
			 * immediately.  If not, set it (but don't call it).
			 */
			if (pc->state == dpio_STATE_ESTABLISHED) {
				/*if (cb) cb(h, dpio->conns->n_used, dp_RES_OK, context);*/
				if (dpio->callback)
					dpio->callback(h, dpio->conns->n_used, dp_RES_OK, dpio->context);
			/* OBSOLETE */
			/*} else {
				pc->openHdl_callback = cb;
				pc->openHdl_context = context;*/
			}
			/* end OBSOLETE */
			dpio_assertValid(dpio);
			return h;
		}
	} else {
		/* Check to make sure we haven't exceeded our handle limit */
		if (dpio->conns->n_used >= dpio->max_playerHdls) {
			DPRINT(("dpio_openHdl: used handles %d equals max handles %d\n",
				dpio->conns->n_used, dpio->max_playerHdls));
			dpio_closeHdlRaw(dpio, h);
			dpio_assertValid(dpio);
			return PLAYER_NONE;
		}

		pc = (dpio_conn_t *)assoctab_subscript_grow(dpio->conns, h);
		if (!pc) {
			DPRINT(("dpio_openHdl: error: out of memory\n"));
			dpio_closeHdlRaw(dpio, h);
			dpio_assertValid(dpio);
			return PLAYER_NONE;
		}

		*(dp_packetType_t *)(pc->txGatherBuf) = dpio_GATHER_PACKET_ID;
		pc->txGatherBufLen = sizeof(dp_packetType_t);
	}
#ifdef dp_STATS
	/* Entering connecting state */
	dpio->stats[dp_STAT_DPIO_CONNECTING].in++;
	dpio->stats[dp_STAT_DPIO_CONNECTING].waiting++;
#endif

	/* Reset the timeout counter- else he'll time out on first check. */
	pc->rx.PktTime = *dpio->now;

	/* OBSOLETE */
	/* Set the callback for this handle */
	/*pc->openHdl_callback = cb;
	pc->openHdl_context = context;*/
	/* end OBSOLETE */

	/* Send a SYN packet */
	pc->state = dpio_STATE_SYN_SENT;
	pw = &pc->tx;

	/* Choose an initial packet number at random.  Hope this *is* random. */
#ifdef UNIX
	pw->next_pktnum = (unsigned short)((eclock()^getpid()^(long)pw)
#else
	/* WARNING:  This relies on srand() called by the application */
 	pw->next_pktnum = (unsigned short)((rand() + (short) (*dpio->now) + (short) time(NULL))
#endif

#if 0
	/* Force an early wraparound to make sure we correctly handle case where
	 * next_pktnum < windowBase due to wraparound of 16-bit pktnum!
	 * To wrap, run a test where 512 reliable packets are sent.
	 */
	& 0xff) + 0xff00;
	DPRINT(("dpio_openHdl: WRAPAROUND TEST: setting initial pktnum to %x\n", pw->next_pktnum));
#else
	);
#endif
	pw->windowBase = pw->next_pktnum;
	/* Set the default parameters for the adaptive retransmission */
	pc->RTTime = dpio->latency;
	pc->RTT_nsamp = 0;
	pw->ReTxFactor = 0;
	/* dpio_setNextTxTime(dpio, pc); */
	memset(pw->bAcked, 0, sizeof(pw->bAcked));
	memset(pw->pktQTime, 0, sizeof(pw->pktQTime));
	memset(pw->nRetries, 0, sizeof(pw->nRetries));

	/* Packet consists of
	 *   data[0] = version = 5
	 *   data[1] = adrlen
	 *   data[2...adrlen+1] = sender's adr
	 *   data[adrlen+2...2*adrlen+1] = destination's adr  (added 10/23/97)
	 *   data[2*adrlen+2] = sender's capabilities (added 11/12/97)
	 *   data[2*adrlen+3...3*adrlen+4] = senders's 2nd adr (added 5/18/98)
	 */
	data[0] = 5;	/* version */
	assert(UCHAR_MAX >= dpio->myAdrLen);
	data[1] = (unsigned char) dpio->myAdrLen;
	memcpy(&data[2], dpio->myAdr, dpio->myAdrLen);
	memcpy(&data[2+dpio->myAdrLen], adr, dpio->myAdrLen);
	mycap = dpio_REMCAP_NEWTAB | dpio_REMCAP_GATHER;
	if (dpio->driverinfo->capabilities & comm_DRIVER_JUMBO_PACKETS)
		mycap |= dpio_REMCAP_JUMBO_PACKETS;
	data[2*dpio->myAdrLen+2] = mycap;
	memcpy(&data[3+dpio->myAdrLen*2], dpio->myAdr2, dpio->myAdrLen);
	pktlen = 3+3*dpio->myAdrLen;

	/* Queue up this packet in the window, and send it once */
	DPRINT(("dpio_openHdl: t:%d h:%x state:%x; sending syn pkt, pktnum %d, len %d ",
		*dpio->now, h, pc->state, pw->next_pktnum, pktlen));
	dpio_dprintAdr(dpio->myAdr, dpio->myAdrLen);
	DPRINT(("\n"));
	err = dpio_q_packet(dpio, h, pc, dpio_SYN_PACKET_ID, data, pktlen);

	if (err != dp_RES_OK)
		DPRINT(("dpio_openHdl: error: sending to h:%x got err %d; will retransmit\n",
			h, err));

#ifdef dp_STATS
	dpio->stats[dp_STAT_DPIO_TX_REL_PKTS].out++;
	dpio->stats[dp_STAT_DPIO_TX_REL_BYTES].out += pktlen;
	dpio->stats[dp_STAT_DPIO_TX_REL_PKTS].waiting++;
	dpio->stats[dp_STAT_DPIO_TX_REL_BYTES].waiting += pktlen;
	/*DPRINT(("dpio_openHdl: h:%x, queued %d bytes, pktnum %d, waiting %d %d\n", h, pktlen, (pc->tx.next_pktnum - 1)&0xffff,
			dpio->stats[dp_STAT_DPIO_TX_REL_PKTS].waiting,
			dpio->stats[dp_STAT_DPIO_TX_REL_BYTES].waiting));*/
#endif

	/* Even if the send failed, return the handle; the send will be retried. */
	dpio_assertValid(dpio);
	return h;
}


#ifdef dp_STATS
/*-----------------------------------------------------------------------
 A handle is closing.  Update the statistics.
-----------------------------------------------------------------------*/
static void dpio_countHdlClosing(dpio_t *dpio, playerHdl_t h, dpio_conn_t *pc)
{
	/* If this is the first time we've been called on this handle, */
	if (!(pc->state & (dpio_STATE_FIN_SENT|dpio_STATE_CLOSE_QFULL|dpio_STATE_RST_RCVD))) {
		/* Update stats. */
		if ((pc->state & dpio_STATE_ESTABLISHED)==dpio_STATE_ESTABLISHED) {
			/* Leaving connected state */
			dpio->stats[dp_STAT_DPIO_CONNECTED].out++;
			dpio->stats[dp_STAT_DPIO_CONNECTED].waiting--;
			DPRINT(("dpio_countHdlClosing: h:%x : leaving connected; state %x; connected.waiting %d, total.n_used %d\n",
				h, pc->state, dpio->stats[dp_STAT_DPIO_CONNECTED].waiting,
				dpio->conns->n_used));
			assert(dpio->stats[dp_STAT_DPIO_CONNECTED].waiting >= 0);
		} else {
			/* Leaving connecting state (because you disconnected before
			 * finishing connecting)
			 */
			dpio->stats[dp_STAT_DPIO_CONNECTING].out++;
			dpio->stats[dp_STAT_DPIO_CONNECTING].waiting--;
			DPRINT(("dpio_countHdlClosing: h:%x : leaving connecting; state %x; connecting.waiting %d, total.n_used %d\n",
				h, pc->state, dpio->stats[dp_STAT_DPIO_CONNECTING].waiting,
				dpio->conns->n_used));
			assert(dpio->stats[dp_STAT_DPIO_CONNECTING].waiting >= 0);
		}
	}
}
#endif

/*-----------------------------------------------------------------------
 Close a handle.
 When you're done talking to all the players at a particular machine,
 call this function to free up the low level comm handle to that machine.
 Sends a FIN packet, and begins closing the handle.  If it can't send
 the FIN due to queue full, marks the handle for future closing by
 findTimedOutHost.
-----------------------------------------------------------------------*/
dp_result_t dpio_closeHdl(dpio_t *dpio, playerHdl_t h)
{
	dpio_conn_t *pc;
	dp_result_t err;
   	unsigned char data[4] = "FIN";
	int oldstate;

	DPRINT(("dpio_closeHdl: h:%x\n", h));
	precondition(dpio != NULL);
	dpio_assertValid(dpio);

	pc = (dpio_conn_t *) assoctab_subscript(dpio->conns, h);

	if (!pc) {
		DPRINT(("dpio_closeHdl: bogus h:%x!\n", h));
		dpio_assertValid(dpio);
		return dp_RES_BUG;
	}

#ifdef dp_STATS
	dpio_countHdlClosing(dpio, h, pc);
#endif

	oldstate = pc->state;

	/* Send the closing packet to this handle */
	if (pc->state & dpio_STATE_FIN_SENT) {
		DPRINT(("dpio_closeHdl: t:%d already closing; h:%x state:%x\n", *dpio->now, h, pc->state));
	} else {
		int pktlen = 3;
		err = dpio_q_packet(dpio, h, pc, dpio_FIN_PACKET_ID, data, pktlen);

		if (err == dp_RES_FULL) {
			/* Queue full; mark handle for retry later */
			pc->state |= dpio_STATE_CLOSE_QFULL;
			DPRINT(("dpio_closeHdl: t:%d queue full; h:%x state:%x\n", *dpio->now, h, pc->state));
		} else {
			/* Mark this handle for closing.  Unmark queue full. */
			pc->state &= ~dpio_STATE_CLOSE_QFULL;
			pc->state |= dpio_STATE_FIN_SENT;
			DPRINT(("dpio_closeHdl: t:%d closing; h:%x state:%x\n", *dpio->now, h, pc->state));
#ifdef dp_STATS
			dpio->stats[dp_STAT_DPIO_TX_REL_PKTS].out++;
			dpio->stats[dp_STAT_DPIO_TX_REL_BYTES].out += pktlen;
			dpio->stats[dp_STAT_DPIO_TX_REL_PKTS].waiting++;
			dpio->stats[dp_STAT_DPIO_TX_REL_BYTES].waiting += pktlen;
		/*	DPRINT(("dpio_closeHdl: h:%x, queued %d bytes, pktnum %d, waiting %d %d\n", h, pktlen, (pc->tx.next_pktnum - 1)&0xffff,
					dpio->stats[dp_STAT_DPIO_TX_REL_PKTS].waiting,
					dpio->stats[dp_STAT_DPIO_TX_REL_BYTES].waiting));*/
#endif
		}
	}

	/* Call back with PEER_CLOSED to warn that handle is going down.
	 * Later, will call back with CLOSED when handle finishes going down.
	 * Do callback *after* work has been done, to make sure
	 * any recursive dpio_closeHdl's it triggers do nothing.
	 */
	if (!(oldstate & (dpio_STATE_FIN_SENT|dpio_STATE_CLOSE_QFULL))) {
		DPRINT(("dpio_closeHdl: h:%x was state %x.  Calling back with PEER_CLOSED.\n", h, oldstate));
		if (dpio->callback)
			dpio->callback(h, dpio->conns->n_used, dp_RES_PEER_CLOSED, dpio->context);
	}

	dpio_assertValid(dpio);
	return dp_RES_OK;
}

/*-----------------------------------------------------------------------
 Delete a peer.
 This deletes a handle without waiting for tx/rx to finish or doing a
 controlled close.
 If a callback has been registered on that connection,
 it is called with err = dp_RES_CLOSED.
-----------------------------------------------------------------------*/
dp_result_t dpio_closeHdlImmed(dpio_t *dpio, playerHdl_t h)
{
	dpio_conn_t *pc;

	dpio_assertValid(dpio);

	pc = (dpio_conn_t *) assoctab_subscript (dpio->conns, h);
	if (!pc) {
		DPRINT(("dpio_closeHdlImmed: bogus h:%x!\n", h));
		dpio_assertValid(dpio);
		return dp_RES_BUG;
	}
	/* Call the openHdl callback function with an error if it's set */
	/* Make sure we clear the callback before calling it */
	if (dpio->callback) {
		dpio->callback(h, dpio->conns->n_used - 1, dp_RES_CLOSED, dpio->context);
	}
	/* OBSOLETE */
	/*if (pc->openHdl_callback) {
		dpioOpenHdlCallback_t cb = pc->openHdl_callback;
		pc->openHdl_callback = NULL;
		cb(h, dpio->conns->n_used - 1, dp_RES_CLOSED, pc->openHdl_context);
		pc->openHdl_context = NULL;
	}*/
	/* end OBSOLETE */

#ifdef dp_STATS
	dpio_countHdlClosing(dpio, h, pc);
	/* If we're closing the handle, its outstanding packet are being
	 * discarded.
	 * Update count of packets and bytes waiting for transmission.
	 */
	{
		dpio_window_t *txw = &pc->tx;
		short windowUsed = (short) (txw->next_pktnum - txw->windowBase);	/* careful to wrap around right */
		int i;

		for (i=0; i<windowUsed; i++) {
			unsigned short winpos = (txw->windowBase + i) % dpio_WINDOWSIZE;
			if (!txw->bAcked[winpos]) {
				dpio->stats[dp_STAT_DPIO_TX_REL_PKTS].waiting--;
				dpio->stats[dp_STAT_DPIO_TX_REL_BYTES].waiting -= txw->outstanding[winpos].body.len;
				DPRINT(("dpio_closeHdlImmed: h:%x, discarding pktnum %d, at windo,Base %d; waiting %d %d\n", h, (txw->windowBase + winpos)&0xffff, txw->windowBase,
						dpio->stats[dp_STAT_DPIO_TX_REL_PKTS].waiting,
						dpio->stats[dp_STAT_DPIO_TX_REL_BYTES].waiting));
				assert(dpio->stats[dp_STAT_DPIO_TX_REL_PKTS].waiting >= 0);
				assert(dpio->stats[dp_STAT_DPIO_TX_REL_BYTES].waiting >= 0);
			}
		}
	}
#endif

	DPRINT(("dpio_closeHdlImmed: connection closed: h:%x\n", h));
	assoctab_subscript_delete(dpio->conns, h);

	dpio_assertValid(dpio);
	return dpio_closeHdlRaw(dpio, h);
}

/*----------------------------------------------------------------------
 Returns dp_RES_BUSY if dpio has reliable transmissions pending,
 and sets *pHdl to the handle of the machine causing us to wait.
----------------------------------------------------------------------*/
dp_result_t dpio_ReadyToFreeze(
	dpio_t   *dpio,
	playerHdl_t *pHdl)
{
	dpio_conn_t *pc;
	dpio_window_t *pw;
	assoctab_item_t *pe;
	int i;

#ifdef VERYVERBOSE
	DPRINT(("dpio_ReadyToFreeze: conns->n_used:%d\n", dpio->conns->n_used));
#endif
	for (i=0; i<dpio->conns->n_used; i++) {
		pe = assoctab_getkey(dpio->conns, i);
		if (!pe) {
			DPRINT(("dpReadyToFreeze: no entry for index %d\n", i));
			dpio_assertValid(dpio);
			return dp_RES_BUG;
		}
		pc = (dpio_conn_t *) &pe->value;
		pw = &pc->tx;
#ifdef VERYVERBOSE
		DPRINT(("dpio_ReadyToFreeze: checking %d: id %x, windowBase %d, next_Pktnum %d, h:%x state:%x\n", i, pe->key, pw->windowBase, pw->next_pktnum, pe->key, pc->state));
#endif
		if (pw->windowBase != pw->next_pktnum) {	/* no packets waiting */
			playerHdl_t h = (playerHdl_t) pe->key;

			if ((h != PLAYER_ME) && (h != PLAYER_NONE) && (h != PLAYER_UNKNOWN)) {
#ifdef VERYVERBOSE
				DPRINT(("dpio_ReadyToFreeze: busy: id %d, windowBase %d, next_Pktnum %d, h:%x\n",
						h, pw->windowBase, pw->next_pktnum, h));
#endif
				if (pHdl)
					*pHdl = h;
				dpio_assertValid(dpio);
				return dp_RES_BUSY;
			}
		}
	}
	dpio_assertValid(dpio);
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
 Check the given window to see if a packet is ready to hand to the user.
 If it is, deliver it to the user, and return dp_RES_OK.
 Else, return dp_RES_EMPTY.

 h is passed in just for dp_dprintf's.
-------------------------------------------------------------------------*/
static  dp_result_t  dpio_window_get(
	dpio_window_t *pw,	/* a source's window buffer */
	void *buffer,
	size_t *size,
	playerHdl_t h)
{
	if (pw->windowBase == pw->next_pktnum)
		return dp_RES_EMPTY;

	/*DPRINT(("dpio_window_get: h:%x, windowBase %d, next_pktnum %d\n",
		h, pw->windowBase, pw->next_pktnum));*/
	/*dpio_pw(pw); */

	/* Is there a packet at windowBase? */
	if (pw->bAcked[pw->windowBase % dpio_WINDOWSIZE]) {
		/* Deliver it to the user and advance windowBase. */
		dpio_wrapped_data_packet_t *p = &pw->outstanding[pw->windowBase % dpio_WINDOWSIZE];
		/* Are we sane? */
		if (SwapBytes2(p->body.pktnum) != pw->windowBase) {
			DPRINT(("dpio_window_get: bug: got pktnum %d at windowBase %d\n",
				SwapBytes2(p->body.pktnum), pw->windowBase));
			return dp_RES_BUG;
		}
		/* Can the user handle it? */
		if (*size < p->body.len) {
			DPRINT(("dpio_window_get: buffer too small %d needed %d\n",
				*size, p->body.len));
			return dp_RES_BADSIZE;
		}
		*size = p->body.len;
		memcpy(buffer, p->body.data, p->body.len);
		/* Clear the bAcked flag for the window slot being vacated. */
		pw->bAcked[pw->windowBase % dpio_WINDOWSIZE] = FALSE;
		pw->windowBase++;
		/* Finally! */
		/*DPRINT(("dpio_window_get: got h:%x pktnum %d\n", h, SwapBytes2(p->body.pktnum)));*/
		return dp_RES_OK;
	}
	(void) h;
	return dp_RES_EMPTY;
}

static void dpio_update_RTT(dpio_t *dpio, dpio_conn_t *pc, playerHdl_t h, clock_t thisTrip)
{
	/* From TCP/IP Illus. p299; note no variance measure! */
	pc->RTT_nsamp++;
	if (pc->RTT_nsamp > 10) pc->RTT_nsamp = 10;
	if (pc->RTT_nsamp < 1) pc->RTT_nsamp = 1;
	pc->RTTime = ((pc->RTT_nsamp - 1) *  pc->RTTime + thisTrip) / pc->RTT_nsamp;
	/* Don't let it get too small; this may need tuning */
	if (pc->RTTime < dpio->minTxInterval)
		pc->RTTime = dpio->minTxInterval;
#ifdef VERYVERBOSE
	DPRINT(("dpio_update_RTT: t:%d h:%x thisRT:%d newRTT:%d\n",
		*dpio->now, h, thisTrip, pc->RTTime));
#endif
}

#define my_max(a, b) (((a) > (b)) ? (a) : (b))

/* Returns the time interval between pings for the given handle */
static clock_t dpio_current_pingInterval(const dpio_t *dpio, const dpio_conn_t *pc)
{
	clock_t pingInterval = 0;

	assert(dpio->ping_piggyback_interval <= dpio->ping_forced_interval);
	assert(dpio->ping_forced_interval <= dpio_keepaliveInterval(dpio));
	if (pc->options & dpio_OPTION_PIGGYBACK_PING) {
		pingInterval = dpio->ping_piggyback_interval;
	} else if (pc->options & dpio_OPTION_FORCED_PING) {
		pingInterval = dpio->ping_forced_interval;
	} else if (pc->options & dpio_OPTION_KEEPALIVE) {
		pingInterval = dpio_keepaliveInterval(dpio);
	}
	if (pingInterval < pc->RTTime)
		pingInterval = pc->RTTime;
#ifdef VERYVERBOSE
	DPRINT(("dpio_current_pingInterval: RTT:%d current_interval:%d\n",
		pc->RTTime, pingInterval));
#endif
	return pingInterval;
}

/*-------------------------------------------------------------------------
 A PING_RESP has been received, so update the round trip time estimate.
 (h is passed in just for dp_dprintf's.)

 Returns dp_RES_OK if the latency of the player was calculated,
         dp_RES_EMPTY if not.
-------------------------------------------------------------------------*/
static dp_result_t dpio_handle_ping_response(dpio_t *dpio, dpio_conn_t *pc, playerHdl_t h, unsigned char *buffer, int len)
{
	unsigned char pktnum = buffer[2];
	unsigned char age_in_pktnums = (pc->ping_current_pktnum - pktnum - 1);
	clock_t thisTrip;
	clock_t timeSent;

	if (len < 3)
		return dp_RES_EMPTY;
	if (age_in_pktnums > 64)
		return dp_RES_EMPTY;
	if (age_in_pktnums < 16) {
		pc->ping_bits &= ~(1 << age_in_pktnums);
	}
	if (age_in_pktnums < dpio_PING_TIME_SENT_N) {
		timeSent = pc->ping_time_sent[pktnum & (dpio_PING_TIME_SENT_N-1)];
		if (timeSent == 0)
			return dp_RES_EMPTY;	/* we don't remember sending this pktnum */
		thisTrip = *dpio->now - timeSent;
		DPRINT(("dpio_handle_ping_response: t:%d, h:%x, pktnum:%d, timeSent:%d, age_in_pktnums:%d, thisTrip %d\n",
				*dpio->now, h, pktnum, timeSent, age_in_pktnums, thisTrip));
	} else {
		/* Don't know round-trip time; estimate conservatively */
		/* Why didn't we put a time_sent field in the packet? */
		timeSent = pc->ping_time_sent[pc->ping_current_pktnum & (dpio_PING_TIME_SENT_N-1)];
		if (timeSent == 0)
			return dp_RES_EMPTY;	/* we haven't sent many pkts */

		thisTrip = *dpio->now - timeSent;
		if (thisTrip < pc->RTTime)
			thisTrip = pc->RTTime;
		DPRINT(("dpio_handle_ping_response: t:%d, h:%x, stale pktnum:%d, age_in_pktnums:%d, thisTrip %d\n",
				*dpio->now, h, pktnum, age_in_pktnums, thisTrip));
	}
	dpio_update_RTT(dpio, pc, h, thisTrip);
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
 An ACK has been received for the given packet number on the given
 connection, so update the window.
 (h is passed in just for dp_dprintf's.)

 Returns dp_RES_OK if the latency of the player was calculated,
         dp_RES_EMPTY if not,
         dp_RES_BUG on internal error.
-------------------------------------------------------------------------*/
static dp_result_t dpio_gotAck(
	dpio_t *dpio,
	dpio_conn_t *pc,
	int pktnum,
	playerHdl_t h
#ifndef OLD_ACK
	, int synthesized 	/* true if ack was dropped and has been reconstructed */
#endif
	)
{
	dp_result_t res = dp_RES_EMPTY;
	dpio_window_t *pw = &pc->tx;

	dpio_assertValid(dpio);

	if ((short)(pw->windowBase+dpio_WINDOWSIZE - pktnum) <= 0) {
		/* pktnum was beyond end of window! */
		/* somehow the sender sent a ack he wasn't supposed to yet. */
		DPRINT(("dpio_gotAck: got ack %d, at windowBase %d, beyond end of window\n",
			pktnum, pw->windowBase));
		dpio_assertValid(dpio);
		return dp_RES_BUG;
	}
	/* If the packet is within the window, note that it has been acked. */
	/* If the packet is the earliest packet in the window, advance the window. */
	if ((short)(pw->windowBase - pktnum) <= 0) {
		int winpos = pktnum % dpio_WINDOWSIZE;
		int thisTrip = *dpio->now - pw->pktQTime[winpos];
		if (!pw->bAcked[winpos]) {
#ifdef dp_STATS
			dpio->stats[dp_STAT_DPIO_TX_REL_PKTS].waiting--;
			dpio->stats[dp_STAT_DPIO_TX_REL_BYTES].waiting -= pw->outstanding[winpos].body.len;
	/*		DPRINT(("dpio_gotAck: h:%x, got ack for pktnum %d, at windowBase %d; waiting %d %d\n", h, pktnum, pw->windowBase,
					dpio->stats[dp_STAT_DPIO_TX_REL_PKTS].waiting,
					dpio->stats[dp_STAT_DPIO_TX_REL_BYTES].waiting));*/
			assert(dpio->stats[dp_STAT_DPIO_TX_REL_PKTS].waiting >= 0);
			assert(dpio->stats[dp_STAT_DPIO_TX_REL_BYTES].waiting >= 0);
#endif

			pw->bAcked[winpos] = TRUE;

			/*DPRINT(("dpio_gotAck: got ack for pktnum %d, at windowBase %d\n", pktnum, pw->windowBase));*/
			/* If this packet was not retried, adjust the round-trip time */
			if ((pw->nRetries[winpos] == 1)
#ifndef OLD_ACK
			&& !synthesized
#endif
			) {
				dpio_update_RTT(dpio, pc, h, thisTrip);
				res = dp_RES_OK;  /* new latency available */
			} else {
				DPRINT(("dpio_gotAck: can't use thisRT:%d, 'cause retries > 1\n", thisTrip));
			}
		}
		if (pw->outstanding[pw->windowBase&(dpio_WINDOWSIZE-1)].tag == dpio_FIN_PACKET_ID) {
			/* FIN ACK received; set the appropriate bit */
			DPRINT(("dpio_gotAck: h:%x received ACK for FIN %d, state:%x\n", h, pw->windowBase, pc->state));
			pc->state |= dpio_STATE_FIN_RCVDACK;
		}
		if (pw->outstanding[pw->windowBase&(dpio_WINDOWSIZE-1)].tag == dpio_SYN_PACKET_ID) {
			/* SYN ACK received; check for connection establishment and call callback */
			if (pc->state == (dpio_STATE_ESTABLISHED & ~dpio_STATE_SYN_RCVDACK)) {
				/* Connection established and not closing */
#ifdef dp_STATS
				/* Leaving connecting state (because you connected) */
				dpio->stats[dp_STAT_DPIO_CONNECTING].out++;
				dpio->stats[dp_STAT_DPIO_CONNECTING].waiting--;
				/* Entering connected state */
				dpio->stats[dp_STAT_DPIO_CONNECTED].in++;
				dpio->stats[dp_STAT_DPIO_CONNECTED].waiting++;
#endif
				DPRINT(("dpio_gotAck: Connection established with h:%x state:%x t:%d.\n", h, dpio_STATE_ESTABLISHED, *(dpio->now)));
				/* OBSOLETE */
				/*if (pc->openHdl_callback)
					pc->openHdl_callback(h, dpio->conns->n_used, dp_RES_OPENED, pc->openHdl_context);*/
				/* end OBSOLETE */
				if (dpio->callback)
					dpio->callback(h, dpio->conns->n_used,
										dp_RES_OPENED, dpio->context);
			}
			pc->state |= dpio_STATE_SYN_RCVDACK;
		}
		if (pw->windowBase == pktnum) {
			/* Advance the windowbase and do the housekeeping */
			if (pw->nRetries[winpos] == 1) {
				/* Reset the exponential backoff for the next packet
				 * NOTE: This may be wrong, as it only resets when the
				 * first packet in the window gets through without
				 * retry, as opposed to ANY packet.
				 */
#ifdef VERYVERBOSE
				DPRINT(("dpio_gotAck: reset ReTxFactor\n"));
#endif
				pw->ReTxFactor = 0;
			} else if (pw->ReTxFactor) {
				/* Keep retx from skyrocketing if we have to
				 * retry once each for a long series of packets.
				 */
				pw->ReTxFactor--;
				DPRINT(("dpio_gotAck: decremented ReTxFactor to %d\n", pw->ReTxFactor));
			}
			/* Advance windowbase to first packet not yet ack'd */
			pw->windowBase++;
			while (pw->bAcked[pw->windowBase&(dpio_WINDOWSIZE-1)] && (pw->windowBase != pw->next_pktnum)) {
#if 1
				/* CHEEZY BUGFIX:  REPEAT 5 LINES FROM UP ABOVE
				 * IN CASE A FIN ACK PACKET COMES IN BEFORE
				 * A PREVIOUS DATA PACKET ACK.
				 * THE BUG SCENARIO WAS:
				 *    sent data
				 *    sent fin
				 *    got fin ack
				 *    got data ack
				 * pc->state would never be updated properly because
				 * this loop would zoom windowbase past the FIN packet!
				 */
				if (pw->outstanding[pw->windowBase&(dpio_WINDOWSIZE-1)].tag == dpio_FIN_PACKET_ID) {
					/* FIN ACK received; set the appropriate bit */
					DPRINT(("dpio_gotAck: in zoom: h:%x received ACK for FIN %d, state %d\n", h, pw->windowBase, pc->state));
					pc->state |= dpio_STATE_FIN_RCVDACK;
				}
#endif
				/* Reset the exponential backoff for the next packet
				 * This packet cannot have been retried */
				pw->ReTxFactor = 0;
				pw->windowBase++;
				DPRINT(("dpio_gotAck: reset ReTxFactor; extra advance to windowbase %d\n", pw->windowBase));
			}
			if ((pw->windowBase != pw->next_pktnum)
			&&  (pw->nRetries[pw->windowBase&(dpio_WINDOWSIZE-1)] == 0)) {
				/* If the next packet has not been sent yet, send it now.
				 * This happens now when packets are queued before the
				 * connection is established.  This is a poor-man's
				 * slow-start algorithm... FIXME
				 */
				DPRINT(("dpio_gotAck: got ACK, next pktnum %d unsent, will send soon\n", pw->windowBase));
				pw->PktTime = *dpio->now;
			}
		}
	}
	dpio_assertValid(dpio);
	return res;
}

/*-------------------------------------------------------------------------
 Pretend an ACK has been received for all outstanding packets with the
 given tag in the given window.

 This is useful only for higher-level protocols which somehow know
 that packets have gotten through.  For instance, in a turn-based game,
 when the token makes it all the way back around, you know that all the
 packets you sent out have been received, so there's no sense in waiting
 for explicit ACKs anymore.  This can improve performance under conditions
 of heavy packet loss (when the ACKs are likely to be lost).

 Returns dp_RES_OK if one or more packets were affected;
         dp_RES_EMPTY if no packets were affected;
		 dp_RES_BAD if passed invalid arguments;
		 dp_RES_BUG if passed null dpio, or internal error.
-------------------------------------------------------------------------*/
dp_result_t dpio_higherLevelBlanketAck(
	dpio_t  *dpio,
	const playerHdl_t *dests,/* Vector of destinations */
	int nDests,             /* Size of destination vector */
	char *tag,
	size_t taglen)
{
	dpio_window_t *pw;
	int iDest;
	unsigned short pktnum;	/* type very important, must match pw->pktnum for proper rollover */
	int nAcked = 0;

	/*DPRINT(("dpio_higherLevelBlanketAck(%p,%p,%d,%x)\n", dpio, dests, nDests, tag));*/
	precondition(dpio != NULL);
	precondition(dests != NULL);
	precondition(nDests >= 0);
	precondition(nDests <= dpio_MAX_HANDLES);
	dpio_assertValid(dpio);

	for (iDest=0; iDest<nDests; iDest++) {
		dpio_conn_t *pc;
		pc = (dpio_conn_t *)assoctab_subscript(dpio->conns, dests[iDest]);
		if (!pc) {
			dpio_assertValid(dpio);
			return dp_RES_BAD;
		}
		pw = &pc->tx;

		/* Loop over packet numbers modulo 65536 */
		for (pktnum=pw->windowBase; pktnum!=pw->next_pktnum; pktnum++) {
			int winpos = pktnum % dpio_WINDOWSIZE;
			dpio_wrapped_data_packet_t *p;

			/* Skip already ack'd packets */
			if (pw->bAcked[winpos])
				continue;

			/* Skip packets with wrong tag */
			p = &pw->outstanding[winpos];
			if (memcmp(p->body.data, tag, taglen))
				continue;

			/* It's an unack'd packet of the right type.  Fake an ack. */
			dpio_gotAck(dpio, pc, pktnum, dests[iDest]
#ifndef OLD_ACK
					,TRUE
#endif
			);
			nAcked++;
			/*DPRINT(("dpio_higherLevelBlanketAck: acking pktnum %d, h:%x\n", pktnum, dests[iDest]));*/
		}
	}
	/*DPRINT(("dpio_higherLevelBlanketAck: acked %d packets.\n", nAcked));*/
	dpio_assertValid(dpio);
	return nAcked ? dp_RES_OK : dp_RES_EMPTY;
}

/*--------------------------------------------------------------------------
 Send an ACK to the given peer indicating
 1. oldest packet we haven't gotten yet, so peer knows earlier ones got thru.
    If window is empty, use windowbase.
 2. the most recent packet we have gotten, so peer can judge round trip time.
--------------------------------------------------------------------------*/
static dp_result_t dpio_sendAck(dpio_t *dpio, dpio_conn_t *conn, playerHdl_t h, unsigned short last_rx_pktnum)
{
	dp_result_t err;
	int i;
	short offset;
	unsigned short oldest_nonrx_pktnum;
	unsigned short windowUsed;
#include "dppack1.h"
	struct {
		dp_packetType_t tag;
		dpio_ack_packet_t body;
	} PACK ackbuf;
#include "dpunpack.h"
	dpio_window_t *rxw = &conn->rx;

	/* Find oldest packet not yet received */
	oldest_nonrx_pktnum = rxw->windowBase;
	windowUsed = (unsigned short) (rxw->next_pktnum - rxw->windowBase);
	for (i=0; i<windowUsed; i++, oldest_nonrx_pktnum++) {
		if (!rxw->bAcked[(oldest_nonrx_pktnum) % dpio_WINDOWSIZE])
			break;
	}

	/* if the offset can not be represented in 8 bits,
	 * send 128.  The other end will know to ignore the offset then.
	 */
	offset = (oldest_nonrx_pktnum - last_rx_pktnum);
	if (offset < -126 || offset > 126)
		offset = 128;
	DPRINT(("dpio_sendAck: h:%x, last_rx_pktnum %d, oldest_nonrx_pktnum %d, ack correction %d\n", h, last_rx_pktnum, oldest_nonrx_pktnum, offset));

	ackbuf.tag = dpio_ACK_PACKET_ID;
	ackbuf.body.gotAllUpTo_offset = (char) offset;
	ackbuf.body.pktnum = SwapBytes2(last_rx_pktnum);

	err = dpio_put_unreliable_buffered(dpio, conn, h, &ackbuf, sizeof(ackbuf.tag)+sizeof_dpio_ack_packet_t);
	if ((err != dp_RES_OK) && (err != dp_RES_FULL)) {
		DPRINT(("dpio_sendAck: err:%d sending ack\n", err));
		dpio_assertValid(dpio);
		return err;
	}
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Mark a handle as needing an ack sent later.
----------------------------------------------------------------------*/
static void dpio_sendAckLater(dpio_t *dpio, dpio_conn_t *conn)
{
	dpio->ackNeeded = TRUE;
	conn->ackNeeded = TRUE;
	DPRINT(("dpio_sendAckLater\n"));
}

/* simpler ping packet used to implement silent piggyback pings */
#define sizeof_dpio_ping_packet_t (sizeof(unsigned char))
#define sizeof_dpio_ping (sizeof(dp_packetType_t) + sizeof_dpio_ping_packet_t)

/*----------------------------------------------------------------------
 Send any outstanding packets or acks queued recently.
----------------------------------------------------------------------*/
dp_result_t dpio_flush(dpio_t *dpio)
{
	int i;
	playerHdl_t		h;
	dpio_conn_t     *pc;

	DPRINT(("dpio_flush: ackNeeded:%d txGatherNeeded:%d\n", dpio->ackNeeded, dpio->txGatherNeeded));
	if (!dpio->ackNeeded && !dpio->txGatherNeeded)
		return dp_RES_OK;

	/* Check all connections for ack flag and/or stored packets */
	for (i=dpio->conns->n_used-1; i >= 0; i--) {
		assoctab_item_t	*ip;

		ip = assoctab_getkey(dpio->conns, i);
		if (!ip) continue;
		h = (playerHdl_t) ip->key;
		pc = (dpio_conn_t *) &ip->value;

		if (pc->ackNeeded) {
			dpio_sendAck(dpio, pc, h, (unsigned short)(pc->rx.next_pktnum - 1));
			pc->ackNeeded = FALSE;
		}
		if (pc->txGatherBufLen > sizeof(dp_packetType_t)) {
			dpio_flushPacketsForHandle(dpio, pc, h);
		}
	}

	dpio->nextFlush = *dpio->now + (dpio->clocksPerSec + 99)/100;
	dpio->txGatherNeeded = FALSE;
	dpio->ackNeeded = FALSE;

	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Read an reliable packet from any reliable packet window buffer.

 On successful exit:
 The packet is copied to callerBuf;
 the packet's length is stored in *pcallerBufLen;
 *pcallerSrc holds the handle of the packet's source;
 if packet was from an unknown handle, its source address is placed
 in pktadr[].

 Note: dpio_get bypasses this routine when it has a good idea which
 handle has a packet ready.  This causes some code duplication.  FIXME
----------------------------------------------------------------------*/
static dp_result_t dpio_getReliable(
	dpio_t   *dpio,
	playerHdl_t *pcallerSrc,
	void *callerBuf,
	size_t *pcallerBufLen)
{
	int i = dpio->conns->n_used;
	int flush_now = ((dpio->ackNeeded || dpio->txGatherNeeded)
					 && ((long)(*dpio->now - dpio->nextFlush) > 0)
					 && (dpio->rxGatherBufUsed >= dpio->rxGatherBufLen));

#if 0
	/* If there are zillions of connections, only check a
	 * portion each call.  Supposedly caps CPU usage...
	 * FIXME
	 */
	if (i > 32)
		i = 32 + (i-32) / 8;
#endif

	while (i-- > 0) {
		dpio_conn_t *pc;
		assoctab_item_t *pe;
		dp_result_t err;

		if (dpio->nextconncheck >= dpio->conns->n_used)
			dpio->nextconncheck = 0;
		pe = assoctab_getkey(dpio->conns, dpio->nextconncheck);
		dpio->nextconncheck++;	/* can't do this in macro above */
		if (!pe) {
			return dp_RES_BUG;
		}
		pc = (dpio_conn_t *) &pe->value;
		/*DPRINT(("dpio_get: h:%x state:%x\n", pe->key, pc->state));*/

		err = dpio_window_get(&pc->rx, callerBuf, pcallerBufLen, pe->key);
		if (err == dp_RES_OK) {
			dpio->nextconncheck--;	/* give this one another go next time */
			*pcallerSrc = pe->key;
			if (*(dp_packetType_t *)callerBuf == dp_KEEPALIVE_PACKET_ID) {
				/* Just ignore keepalive packets at this level */
#ifdef VERBOSE
				DPRINT(("dpio_getReliable: t:%d keepalive tag %2.2s from h:%x, length %d, pktnum %d\n",
					*dpio->now, callerBuf, *pcallerSrc, *pcallerBufLen, pc->rx.windowBase-1));
#endif
				;
			} else {
#ifdef VERBOSE
				DPRINT(("dpio_getReliable: t:%d tag %2.2s h:%x ln %d pktnum %d\n",
					*dpio->now, callerBuf, *pcallerSrc, *pcallerBufLen, pc->rx.windowBase-1));
#endif
#ifdef dp_STATS
				dpio->stats[dp_STAT_DPIO_RX_REL_PKTS].in++;
				dpio->stats[dp_STAT_DPIO_RX_REL_BYTES].in += *pcallerBufLen;
#endif
				return dp_RES_OK;
			}
		} else if (err != dp_RES_EMPTY) {
			DPRINT(("dpio_getReliable: h:%x window_get returns error %d\n", pe->key, err));
		}
		/* Every 10ms, do the equivalent of a dpio_flush() on handles
		 * that aren't receiving data at the moment.
		 *
		 * This code is duplicated to save the overhead of iterating
		 * through handles a second time with a full call to dpio_flush().
		 *
		 * Note: This only flushes handles up to the first that has data.
		 */
		if (flush_now) {
			if (pc->ackNeeded) {
				dpio_sendAck(dpio, pc, pe->key, (unsigned short)(pc->rx.next_pktnum - 1));
				pc->ackNeeded = FALSE;
			}
			if (pc->txGatherBufLen > sizeof(dp_packetType_t)) {
				dpio_flushPacketsForHandle(dpio, pc, pe->key);
			}
		}
	}

	/* Finished a complete pass through the handles.
	 * We can count this as a full dpio_flush, and set the appropriate flags.
	 */
	if (flush_now) {
		dpio->nextFlush = *dpio->now + (dpio->clocksPerSec + 99)/100;
		dpio->txGatherNeeded = FALSE;
		dpio->ackNeeded = FALSE;
	}
	return dp_RES_EMPTY;
}

/*----------------------------------------------------------------------
 Read an unreliable packet from the physical network.
 If delay or packet loss simulation is enabled, packet will be dropped
 or delayed here before being ungathered.

 On successful exit:
 The packet is copied to callerBuf;
 the packet's length is stored in *pcallerBufLen;
 *pcallerSrc holds the handle of the packet's source;
 if packet was from an unknown handle, its source address is placed
 in pktadr[].
----------------------------------------------------------------------*/
static dp_result_t dpio_getRaw(
	dpio_t   *dpio,
	playerHdl_t *pcallerSrc,
	char *pktadr,
	void *callerBuf,
	size_t *pcallerBufLen)
{
	commRxPktResp_t	rxPktResp;
	commRxPktReq_t	rxPktReq;

	/* Check gather buf. */
	if (dpio->rxGatherBufUsed < dpio->rxGatherBufLen) {
		rxPktResp.length = ((unsigned char *)dpio->rxGatherBuf)[dpio->rxGatherBufUsed++];
		rxPktReq.buffer = dpio->rxGatherBuf + dpio->rxGatherBufUsed;
		dpio->rxGatherBufUsed += rxPktResp.length;
		DPRINT(("dpio_getRaw: gathered; tag %2.2s, ln %d\n", rxPktReq.buffer, rxPktResp.length));
		if (dpio->rxGatherBufUsed > dpio->rxGatherBufLen) {
			DPRINT(("dpio_get: bug: subpacket longer than packet\n"));
			dpio->rxGatherBufUsed = 0;
			dpio->rxGatherBufLen = 0;
			return dp_RES_AGAIN;
		}
		memcpy(rxPktResp.adr, dpio->rxGatherAdr, dpio->myAdrLen);
		rxPktResp.src = dpio->rxGatherSrc;
	} else {

		/* Check real network. */
		rxPktReq.buffer = dpio->rxGatherBuf;
		rxPktReq.size = dpio_MAXLEN_GATHER;
		ptimer_Enter(PTIMER_DPIO_NETWORK_GET, "commRxPkt timer");
		commRxPkt(&rxPktReq, &rxPktResp, dpio->commPtr);
		ptimer_Exit(PTIMER_DPIO_NETWORK_GET, 0);

		if (rxPktResp.status == comm_STATUS_OK) {
			dumpBuf("rxraw0: ", rxPktReq.buffer, rxPktResp.length);
		}

		/* Drop packets if desired for debugging. */
		if ((rxPktResp.status == comm_STATUS_OK)
		&&  dpio->rxDropPercent
		&&  ((dpio->rxDropPercent*(int)(RAND_MAX / 100)) > rand())) {
			DPRINT(("dpio_getRaw: t:%d Dropping tag %2.2s from h:%x, ln %d\n",
			*dpio->now, dpio->rxGatherBuf, rxPktResp.src, rxPktResp.length));
			rxPktResp.status = dp_RES_EMPTY;
		}

		/* Delay the packet or error value. */
		if (dpio->delay_q) {
			size_t msgLen;
			char dbuf[dpio_MAXLEN_GATHER + sizeof(rxPktResp)];
			int derr;

			/* Store into delay queue if not empty */
			if (rxPktResp.status != comm_STATUS_EMPTY) {
				memcpy(dbuf, &rxPktResp, sizeof(rxPktResp));
				memcpy(dbuf+sizeof(rxPktResp), dpio->rxGatherBuf, rxPktResp.length);
				derr = delay_put(dpio->delay_q, dbuf, rxPktResp.length+sizeof(rxPktResp), *dpio->now);
				if (derr) {
					DPRINT(("dpio_getRaw: bug: delay_put returns err:%d\n", derr));
				} else {
					DPRINT(("dpio_getRaw: t:%d Delay tag %2.2s from h:%x, ln %d\n",
						*dpio->now, dpio->rxGatherBuf, rxPktResp.src, rxPktResp.length));
				}
			}

			/* Retrieve from delay queue */
			rxPktResp.status = comm_STATUS_EMPTY;
			msgLen = delay_get(dpio->delay_q, dbuf, sizeof(dbuf), *dpio->now);
			if (msgLen > 0) {
				memcpy(&rxPktResp, dbuf, sizeof(rxPktResp));
				if (rxPktResp.status == comm_STATUS_OK) {
					rxPktResp.length = msgLen - sizeof(rxPktResp);
					memcpy(dpio->rxGatherBuf, dbuf+sizeof(rxPktResp), rxPktResp.length);
				}
			}
		}

		/* Process empty or bad network packet */
		if (rxPktResp.status == comm_STATUS_EMPTY)
			return dp_RES_EMPTY;
		if (rxPktResp.status != comm_STATUS_OK) {
			DPRINT(("dpio_getRaw: error: rxPktResp.status %d\n", rxPktResp.status));
			/* Note: comm status values MUST all be equal to dp_result_t's.
			 * I've gone thru and set up commerr.h to define them equivalently,
			 * but we need to check to make sure it agrees with anet.h from
			 * time to time.
			 */
			return (dp_result_t) rxPktResp.status;
		}
		dumpBuf("rxraw1: ", rxPktReq.buffer, rxPktResp.length);

#ifdef dp_STATS
		dpio->stats[dp_STAT_DPIO_RX_UNREL_PKTS].in++;
		dpio->stats[dp_STAT_DPIO_RX_UNREL_BYTES].in += rxPktResp.length;
#endif

		/* If gathered packet, explode it on next call */
		if ((*(dp_packetType_t *) dpio->rxGatherBuf) == dpio_GATHER_PACKET_ID) {
			dpio->rxGatherBufUsed = 2;
			dpio->rxGatherBufLen = rxPktResp.length;
			dpio->rxGatherSrc = rxPktResp.src;
			memcpy(dpio->rxGatherAdr, rxPktResp.adr, dpio->myAdrLen);
			return dp_RES_AGAIN;
		}
	}

	/* Else pass packet to caller */
	if (rxPktResp.length > *pcallerBufLen) {
		DPRINT(("dpio_getRaw: t:%d buf too small, ln %d > %d\n", *dpio->now, rxPktResp.length, *pcallerBufLen));
		return dp_RES_FULL;
	}

	memcpy(callerBuf, rxPktReq.buffer, rxPktResp.length);
	*pcallerBufLen = rxPktResp.length;
	*pcallerSrc = rxPktResp.src;
	if (rxPktResp.src == PLAYER_NONE)
		memcpy(pktadr, rxPktResp.adr, dpio->myAdrLen);

#ifdef VERBOSE
	if (rxPktResp.src != PLAYER_NONE) {
		DPRINT(("dpio_getRaw: t:%d pkt %2.2s from h:%x, ln %d\n", *dpio->now, callerBuf, rxPktResp.src, rxPktResp.length));
	} else {
		DPRINT(("dpio_getRaw: t:%d pkt %2.2s from h:PLAYER_NONE, ln %d", *dpio->now, callerBuf, rxPktResp.length));
#ifdef VERYVERBOSE
		DPRINT((" adr "));
		dpio_dprintAdr((char*)rxPktResp.adr, dpio->myAdrLen);
#endif
		DPRINT(("\n"));
	}
#endif

	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Receive a packet.  Packet might come from the network, the local
 queue, or the reliable transport windows; the caller won't be able
 to distinguish between the sources.

 All parameters have already been validated; *dpio->now is up to date.
 This routine notes the last packet arrival time according to source,
 if the packet looks interesting (it knows about dp's packet types).
 It also immediately handles reliable data or ack packets.
 This routine knows nothing about envelopes.

 On entry:
 *size is the size of the receive buffer.

 On exit:
 Sets *src to player handle of sender.
 Sets *size to length of packet.
 If flags is a valid pointer (not NULL), sets *flags using the
 dpio_GET_RELIABLE and other dpio_GET_* bit flags.
----------------------------------------------------------------------*/
dp_result_t dpio_get(
	dpio_t   *dpio,
	playerHdl_t *psrc,
	void *buffer,
	size_t *psize,
	int *flags)
{
	unsigned short pktnum;	/* type very important, must match pw->pktnum for proper rollover */
	commRxPktResp_t	rxPktResp;
	void			*localMsg;
	size_t			localMsgLen;
	char *srcAdr;
	char *srcAdr2;
	dp_result_t err;
	dp_result_t res;
	dpio_data_packet_t  *pDat;
	dpio_ack_packet_t   *pAck;
#include "dppack1.h"
	struct {
		dp_packetType_t tag;
		dpio_ack_packet_t body;
	} PACK ackbuf;
#include "dpunpack.h"
	dpio_window_t *pw;
	dpio_conn_t *pc;
	size_t orig_size = *psize;
	playerHdl_t h;
	int handleRegenerated;

	dpio_assertValid(dpio);
	if (flags)
		*flags = 0;

	/* Check local message queue first. */
	localMsgLen = 0;
	localMsg = q_get(dpio->localMsgQ, &localMsgLen);
	if (localMsg) {
		DPRINT(("dpio_get: t:%d local packet %2.2s from h:%x, length %d\n", *dpio->now, localMsg, PLAYER_ME, localMsgLen));
		if (localMsgLen > *psize) {
			DPRINT(("dpio_get: Ignoring local message, too big.\n"));
			dpio_assertValid(dpio);
			return dp_RES_BADSIZE;
		} else {
			memcpy(buffer, localMsg, localMsgLen);
			dp_FREE(localMsg);
			*psize = localMsgLen;
			*psrc = PLAYER_ME;
			if (flags)
				*flags |= dpio_GET_RELIABLE; /* kludge: can't tell if local
												message reliable/unreliable */
			dpio_assertValid(dpio);
			return dp_RES_OK;
		}
	}

	/* Check reliable transport window buffers.  */
	if (dpio->reliable_enabled) {
		err = dpio_getReliable(dpio, psrc, buffer, psize);
		if (err == dp_RES_OK) {
			if (flags)
				*flags |= dpio_GET_RELIABLE;
			dpio_assertValid(dpio);
			return dp_RES_OK;
		}
		if (err != dp_RES_EMPTY) {
			dpio_assertValid(dpio);
			return err;
		}
	}

	/* Check real network (or gather buffer). */
	do {
		rxPktResp.length = *psize;
		err = dpio_getRaw(dpio, &h, rxPktResp.adr, buffer, &rxPktResp.length);
	} while (err == dp_RES_AGAIN);
	if (err != dp_RES_OK) {
		dpio_assertValid(dpio);
		return err;
	}

	dumpBuf("rx: ", buffer, rxPktResp.length);
	/* We have a network packet.
	 * Look up the sender's connection block and note arrival time.
	 * Note that unreliable packets keep the connection alive !?
	 */
	pc = NULL;
	if ((h != PLAYER_UNKNOWN) && (h != PLAYER_NONE)) {
		pc = (dpio_conn_t *) assoctab_subscript(dpio->conns, h);
		if (pc)
			pc->rx.PktTime = *dpio->now;
	}

	/* Handle keepalives and reliable protocol layer packets here, and
	 * return immediately; else fall through and return packet to caller.
	 */
	switch (((dp_packetType_t *)buffer)[0]) {

	case dpio_ACK_PACKET_ID:
		if (!pc) {
			DPRINT(("dpio_get: Unknown player sent ACK, ignoring\n"));
			dpio_assertValid(dpio);
			return dp_RES_AGAIN;
		}
		pAck = (dpio_ack_packet_t *)(((char *)buffer) + sizeof(dp_packetType_t));
		/* If the network packet is an ACK of a previous reliable transmission,
		 * advance our tx window appriately.
		 */
		if (!dpio->reliable_enabled) {
			dpio_assertValid(dpio);
			return dp_RES_AGAIN;
		}
		pktnum = SwapBytes2(pAck->pktnum);
		pw = &pc->tx;
		if (pc->state != dpio_STATE_ESTABLISHED)
			DPRINT(("dpio_get: not established! windowbase %d, h:%x state:%x\n", pw->windowBase, h, pc->state));

		/* Update the transmit window to reflect the acknowledgement. */
		res = dpio_gotAck(dpio, pc, pktnum, h
#ifndef OLD_ACK
					,FALSE
#endif
			);
		if (res == dp_RES_OK) {
			/* dpio_gotAck() recalculated the latency of h; inform caller. */
			if (flags) {
				*flags |= dpio_GET_LATENCY_MEASUREMENT;
				*psrc = h;
			}
		}

		if (res != dp_RES_BUG) {
#ifndef OLD_ACK
#ifdef VERYVERBOSE
			DPRINT(("dpio_get: t:%d got ack: src h:%x, pktnum %d, len %d, ack correction %d\n", *dpio->now, h, pktnum, rxPktResp.length, pAck->gotAllUpTo_offset));
#endif
			/* synthesize any acks that were dropped */
			if ((rxPktResp.length >= (sizeof_dpio_ack_packet_t + sizeof(dp_packetType_t)))
			&& ((pAck->gotAllUpTo_offset&0xff) != 128)) {
				int i;
				unsigned short synpktnum = pw->windowBase;
				unsigned short oldest_nonrx_pktnum = pktnum + pAck->gotAllUpTo_offset;
				short pkts2check = (oldest_nonrx_pktnum - pw->windowBase);	/* careful to wrap around right */

				DPRINT(("dpio_get: pktnum %d, correction %d, oldest %d, wb %d, checking %d through %d\n", pktnum, pAck->gotAllUpTo_offset, oldest_nonrx_pktnum, pw->windowBase, synpktnum, synpktnum + pkts2check));
				if ((pkts2check > -dpio_WINDOWSIZE) && (pkts2check <= dpio_WINDOWSIZE)) {
					for (i=0; i<pkts2check; i++, synpktnum++) {
						assert(synpktnum != pw->next_pktnum);
						if (!pw->bAcked[(synpktnum) % dpio_WINDOWSIZE]) {
							DPRINT(("dpio_get: synthesizing ack for pktnum %d\n", synpktnum));
							dpio_gotAck(dpio, pc, synpktnum, h, TRUE);
						}
					}
				} else {
					DPRINT(("dpio_get: got out-of-range ack: h:%x, pktnum %d, pkts2check %d; oldest_nonrx_pktnum %d, windowBase %d\n", h, pktnum, pkts2check, oldest_nonrx_pktnum, pw->windowBase));
				}
			}
#else
#ifdef VERYVERBOSE
			DPRINT(("dpio_get: t:%d got ack: src h:%x, pktnum %d\n", *dpio->now, h, pktnum));
#endif
#endif
		}
		if ((pc->state & dpio_STATE_CLOSED) == dpio_STATE_CLOSED)
			if (flags)
				*flags |= dpio_GET_CHECK_TIMEOUT;
		dpio_assertValid(dpio);
		return dp_RES_AGAIN;
		break;

	case dpio_SYN_PACKET_ID:
	{
		pDat = (dpio_data_packet_t *)(((char *)buffer) + sizeof(dp_packetType_t));
		DPRINT(("dpio_get: t:%d got syn packet: pktnum %d from src h:%x\n", *dpio->now, SwapBytes2(pDat->pktnum), h));
		if ((pDat->len < 6) || (pDat->data[0] != 5)) {
			DPRINT(("dpio_get: got syn packet, but ln %d < 6 or version %d != 5\n",
				pDat->len, pDat->data[0]));
			/*dumpBuf(buffer, rxPktResp.length);*/
			dpio_assertValid(dpio);
			return dp_RES_AGAIN;
		}
		if (!dpio->reliable_enabled) {
			dpio_assertValid(dpio);
			return dp_RES_AGAIN;
		}
		/* Oddly enough, many comm layers don't really implement commSayBye,
		 * so the sender might be known to the comm layer but
		 * unknown to dpio.  Therefore do openHdl if either needs opening.
		 */
		if (!pc) {
			/* So help me god, open a comm handle to this new player,
			 * so we can ack his syn.
			 * Upper layer will then magically get packets from this
			 * new player without having to do a dpio_openHdl or commSayHi.
			 * Gee, guess this means SYN packets have network addresses
			 * embedded in them for now, since the comm layer doesn't pass
			 * addresses up... yuk.  What lunacy.
			 *   data[0] = version = 5
			 *   data[1] = adrlen
			 *   data[2...] = src adr (might be different from comm layer's!)
			 *   data[adrlen+2...2*adrlen+1] = dest adr  (added 10/23/97; opt)
			 *   data[2*adrlen+2] = sender's capabilities (added 11/12/97)
			 *   data[2*adrlen+3...3*adrlen+4] = senders's 2nd adr (added 5/18/98)
			 */
			if (pDat->data[1] != dpio->myAdrLen) {
				DPRINT(("dpio_get: address ln %d not same as mine %d?\n", pDat->data[1], dpio->myAdrLen));
			}

			/* If we're on a non-broadcast protocol, deal with firewalls
			 * and the like.
			 */
			srcAdr = pDat->data + 2;

			if (pDat->len > (3 + dpio->myAdrLen * 2))
			{
				DPRINT(("dpio_get: 2nd Address found\n"));
				srcAdr2 = pDat->data + 3 + dpio->myAdrLen * 2;
			}
			else
			{
				srcAdr2 = NULL;
			}

			if (dpio->driverinfo->capabilities & comm_DRIVER_NO_BROADCAST) {
				/* Driver only reports source address if no handle available.
				 * Compare the source address embedded in the packet
				 * with the one the comm layer reports for that packet.
				 * If they're not equal, somebody's behind a firewall.
				 */
				if (PLAYER_NONE == h) {
					if (srcAdr2 && memcmp(srcAdr, srcAdr2, dpio->myAdrLen)) {
						if (!memcmp(srcAdr, rxPktResp.adr, dpio->myAdrLen)) {
							DPRINT(("dpio_get: 2 Addresses in correct order !\n"));
						} else if (!memcmp(srcAdr2, rxPktResp.adr, dpio->myAdrLen)) {
							DPRINT(("dpio_get: 2 Addresses in wrong order, we must be behind same NAT...\n"));
							/* DO NOT change the order.  Must pass
							 * the SYN's srcAdr as 1st param to openHdl.
							 */
							/*srcAdr2 = srcAdr;
							srcAdr = rxPktResp.adr;*/
						} else {
							DPRINT(("dpio_get: 3 Addresses Detected !\n"));
							srcAdr = rxPktResp.adr;
						}
					} else {
						/* This player thinks they have only one address, lets see if they are correct */
						if (memcmp(srcAdr, rxPktResp.adr, dpio->myAdrLen)) {
							DPRINT(("dpio_get: BEHIND FIREWALL\n"));
							srcAdr2 = srcAdr;
							srcAdr = rxPktResp.adr;
						}
					}

					DPRINT(("dpio_get: src address in packet     : "));
					dpio_dprintAdr((char*)(pDat->data+2), dpio->myAdrLen);
					DPRINT(("\n"));
					DPRINT(("dpio_get: src 2nd address in packet : "));
					dpio_dprintAdr((char*)(pDat->data + 3 + dpio->myAdrLen * 2), dpio->myAdrLen);
					DPRINT(("\n"));
					DPRINT(("dpio_get: src address from comm     : "));
					dpio_dprintAdr((char*)rxPktResp.adr, dpio->myAdrLen);
					DPRINT(("\n"));
				}
			}

			/* Must ignore bogus SYNs before opening handle and replying! */
			if (rxPktResp.length >= sizeof(dp_packetType_t) +
				sizeof(dpio_data_packet_t) - dpio_MAXLEN_RELIABLE
				+ 2 + 2*dpio->myAdrLen) {
				char *destAdr =(char*)(pDat->data+2+dpio->myAdrLen);

				if (memcmp(dpio->myAdr, destAdr, dpio->myAdrLen)) {
					DPRINT(("dpio_get: SYN for wrong host received, ignoring\n"));
					return dp_RES_AGAIN;
				}
			}

			/* Causes SYN to be sent, puts us in dpio_STATE_SYN_SENT.
			 * If user did the openHdl, we were already in that state,
			 * and may have already gotten an ACK for our SYN.
			 * Passes the incoming connection callback to openHdl.
			 */
			/*h = dpio_openHdl(dpio, pDat->data+2, dpio->incomingOpen_callback, dpio->incomingOpen_context);*/
			h = dpio_openHdl2(dpio, srcAdr, srcAdr2);
			if ((h == PLAYER_UNKNOWN) || (h == PLAYER_NONE)) {
				DPRINT(("dpio_get: could not open handle to ack SYN, ignoring\n"));
				dpio_assertValid(dpio);
				return dp_RES_AGAIN;
			}
			DPRINT(("dpio_get: syn packet from unknown node; allocated h:%x\n", h));
			/* Need to get pc now that dpio_openHdl created entry */
			pc = (dpio_conn_t *)assoctab_subscript(dpio->conns, h);
		}
		if (!pc) {
			DPRINT(("dpio_get: syn ack: no peer?\n"));
			dpio_assertValid(dpio);
			return dp_RES_BUG;
		}
		if (dpio->driverinfo->capabilities & comm_DRIVER_NO_BROADCAST) {

			/*
			 * Now compare the destination address embedded in the
			 * SYN packet (if present) with our own address.
			 *
			 * The destination address embeded will be refered to as
			 * the external address from here on.
			 *
			 * These should only be different in the following cases:
			 *
			 * 1. We're behind a firewall which has modified our address
			 * 2. We have more than on address for the given interface
			 * 3. We failed to detect our address
			 * 4. Our address has changed
			 *      (can occur when a connection goes down and comes back
			 *       up with a different address)
			 *
			 * In the event that the addresses are different then we
			 * take the following courses of action:
			 *
			 * KLUDGE!
			 *
			 * The first SYN packet received (indicated by the number of
			 * connection), is examined to determine if the detected
			 * local address is correct.
			 *
			 * If the interface detected a single local address it will
			 * have placed that address in myadr and myadr2.  If the
			 * embeded destination address is different then myadr is
			 * replaced and myadr2 is untouched.  As a result myadr is
			 * the externally detected address and myadr2 is the locally
			 * detected address.
			 *
			 * If the interface detected two distinct addresses, they
			 * will be stored in myadr and myadr2.  If the external address
			 * matches myadr then nothing is done.  If the external address
			 * matches myadr2 then myadr and myadr2 are exchanged.  The
			 * result will be that myadr is the externally detected address
			 * and myadr2 is the locally detected address.
			 *
			 * The final and yukyiest case is when the interface detected
			 * two distinct addresses which are stored in myadr and myadr2,
			 * and the external address does not match either address.
			 *
			 * In this case myadr will be overwritten with the external
			 * address and will be LOST.  This could prove troublesome if
			 * there are other machines on the network who recognise this
			 * machine by the LOST address.  It is further noted that this
			 * is a very unlikely scenario.
			 *
			 * Why all the fuss ?
			 *
			 * A very common case is that there are multiple IP
			 * interfaces, e.g. modem and ethernet, and the one
			 * the driver got the address for initially is the wrong one.
			 *
			 * Having myadr disagree is bad because the master
			 * of a session copies myadr into his own host record,
			 * and players who join his session will then not be
			 * able to open a handle to the master's host record address.
			 * (They already have a handle open to him, but they
			 * don't know this, since the address is different.)
			 *
			 * For this to work, you have to connect to the game
			 * server *before* hosting a session.  This is standard
			 * practice.
			 *
			 */
			DPRINT(("dpio_get: checking SYN for dest adr.  len %d, want %d\n",
			rxPktResp.length, sizeof(dp_packetType_t) +
				sizeof(dpio_data_packet_t) - dpio_MAXLEN_RELIABLE
				+ 2 + 2*dpio->myAdrLen));
			if (rxPktResp.length >= sizeof(dp_packetType_t) +
				sizeof(dpio_data_packet_t) - dpio_MAXLEN_RELIABLE
				+ 2 + 2*dpio->myAdrLen) {
				char *destAdr =(char*)(pDat->data+2+dpio->myAdrLen);
				DPRINT(("dpio_get: SYN has dest adr "));
				dpio_dprintAdr(destAdr, dpio->myAdrLen);
				DPRINT((" myAdr is "));
				dpio_dprintAdr(dpio->myAdr, dpio->myAdrLen);

				if (memcmp(dpio->myAdr, dpio->myAdr2, dpio->myAdrLen)) {
					DPRINT((", "));
					dpio_dprintAdr(dpio->myAdr2, dpio->myAdrLen);
				}

				DPRINT(("\n"));

				if (memcmp(dpio->myAdr, destAdr, dpio->myAdrLen)) {
					DPRINT(("dpio_get: SYN dest adr != myAdr!\n"));
					if (dpio->publicAddressKnown) {
						/* Got SYN from someone in session intended for
						 * someone else with the same local address.
						 */
						DPRINT(("dpio_get: SYN for wrong host h:%x received, ignoring\n", h));
						return dp_RES_AGAIN;
					}
					/* assert(h == dp->hGameServer); */

					/*
					 * If we only have one address then its simple
					 */
					if (!memcmp(dpio->myAdr, dpio->myAdr2, dpio->myAdrLen)) {
						DPRINT(("dpio_get: Overwriting myAdr!\n"));
						memcpy(dpio->myAdr, destAdr, dpio->myAdrLen);
					} else {
						/*
						 * If the destination matches our second address
						 * then exchange addresses
						 */
						if (!memcmp(dpio->myAdr2, destAdr, dpio->myAdrLen))	{
							DPRINT(("dpio_get: Exchanging addresses\n"));
							DPRINT(("dpio_get: myAdr was "));
							dpio_dprintAdr(dpio->myAdr, dpio->myAdrLen);
							DPRINT((", "));
							dpio_dprintAdr(dpio->myAdr2, dpio->myAdrLen);
							DPRINT(("\n"));

							memcpy(dpio->myAdr2, dpio->myAdr, dpio->myAdrLen);
							memcpy(dpio->myAdr, destAdr, dpio->myAdrLen);

							DPRINT(("dpio_get: myAdr is "));
							dpio_dprintAdr(dpio->myAdr, dpio->myAdrLen);
							DPRINT((", "));
							dpio_dprintAdr(dpio->myAdr2, dpio->myAdrLen);
							DPRINT(("\n"));
						} else {
							/*
							 * Trouble... three addresses detected !
							 *
							 * We are forced to replace the first address since it is
							 * the one which is externally detected... We may not be
							 * able to play against people who are also internal since
							 * it is somewhat of a lottery as to which address got
							 * overwritten !
							 *
							 */
							DPRINT(("dpio_get: Got a third address ... trouble!\n"));
							DPRINT(("dpio_get: Overwriting myAdr!\n"));
							memcpy(dpio->myAdr, destAdr, dpio->myAdrLen);
						}
					}
				}
			}
		}

		/* Above code is what initially notices our public
		 * address in the firewall or NAT cases.
		 */
		dpio->publicAddressKnown = TRUE;

		/*
		 * If the packet contains the sender's capabilities, store them
		 */
		if (rxPktResp.length >= sizeof(dp_packetType_t) +
			sizeof(dpio_data_packet_t) - dpio_MAXLEN_RELIABLE
			+ 3 + 2 * dpio->myAdrLen) {
			char *remcap =(char*)(pDat->data+2+2*dpio->myAdrLen);
			DPRINT(("dpio_get: SYN has remcap %x\n", *remcap));
			pc->remote_capabilities = *remcap;
		}

		pw = &pc->rx;
		/* Accept the initial packet number from the SYN packet */
		/* If he sends a SYN followed by data, and the SYN
		 * has to be retransmitted, don't want to wipe out
		 * the data.  Avoid this by not sending DATa until syn acked.
		 * Also, if the SYN has the same packet number as last time,
		 * just ack it, don't reset everything.
		 * Three cases:
		 * 1. new handle
		 *       Ack syn and initialize conn
		 * 2. old handle, but new syn packet number
		 *       Mark conn as RST, let timeout detect it, ignore SYN for now
		 * 3. old handle, but same old syn packet number
		 *       Ack syn, don't init conn
		 */

		if (!(pc->state & dpio_STATE_SYN_RCVD)
		||  (!(pc->state & dpio_STATE_CLOSED)
		&&   (((pw->windowBase-1)&0xffff) == pw->origWindowBase)
		&&   (pw->origWindowBase == SwapBytes2(pDat->pktnum)))) {
			/* No SYN received yet
			 * or same SYN packet number but no data received yet,
			 * so just quietly accept new windowbase
			 */
			pw->origWindowBase = SwapBytes2(pDat->pktnum);
			/* Let the window base wrap around if it wants to */
			pw->next_pktnum = (unsigned short) (pw->origWindowBase + 1);
			pw->windowBase = (unsigned short) (pw->origWindowBase + 1);
			memset(pw->bAcked, 0, sizeof(pw->bAcked));
			DPRINT(("dpio_get: New handle or repeat SYN.  Initialize but don't resend SYN.  OrigWindowBase %d\n", pw->origWindowBase));
		} else if (pw->origWindowBase != SwapBytes2(pDat->pktnum)) {
			/* New SYN, data already received, raise an alarm that the
			 * old connection is no more; let the next retransmission of the
			 * new SYN open a new connection after the upper layer handles
			 * the alarm by closing the handle.
			 */
#ifdef dp_STATS
			dpio_countHdlClosing(dpio, h, pc);
#endif
			pc->state |= dpio_STATE_RST_RCVD;
			DPRINT(("dpio_get: old, but new syn pktnum %d != orig %d.  Resetting.  h:%x state:%x\n",
				SwapBytes2(pDat->pktnum), pw->origWindowBase, h, pc->state));
			/* We know the handle may have timed out at this point, but this
			 * information was being lost and the handle was not being closed
			 * until dpPoll() checked 2 beacons later.
			 * Returning this flag lets the dp level know that it should
			 * check for the timed out handle as soon as possible.
			 */
			if (flags)
				*flags |= dpio_GET_CHECK_TIMEOUT;
			dpio_assertValid(dpio);
			return dp_RES_AGAIN;
		} else {
			DPRINT(("dpio_get: old handle, old syn; just ack it.\n"));
		}

		/* Acknowledge the SYN */
		ackbuf.tag = dpio_ACK_PACKET_ID;
		ackbuf.body.pktnum = pDat->pktnum;
#ifndef OLD_ACK
		ackbuf.body.gotAllUpTo_offset = (char) 128;
#endif
		err = dpio_put_unreliable_buffered(dpio, pc, h, &ackbuf, sizeof(ackbuf.tag)+sizeof_dpio_ack_packet_t);
		if (err != dp_RES_OK) {
			DPRINT(("dpio_get: can't ack syn\n"));
			dpio_assertValid(dpio);
			return dp_RES_BUG;
		}
		DPRINT(("dpio_get: sent ack for syn: dest h:%x state:%x, pktnum %d, t:%d\n", h, pc->state, SwapBytes2(ackbuf.body.pktnum), *(dpio->now)));
		if (pc->state == (dpio_STATE_ESTABLISHED & ~dpio_STATE_SYN_RCVD)) {
				/* Connection established and not closing */
#ifdef dp_STATS
				/* Leaving connecting state (because you connected) */
				dpio->stats[dp_STAT_DPIO_CONNECTING].out++;
				dpio->stats[dp_STAT_DPIO_CONNECTING].waiting--;
				/* Entering connected state */
				dpio->stats[dp_STAT_DPIO_CONNECTED].in++;
				dpio->stats[dp_STAT_DPIO_CONNECTED].waiting++;
#endif
			DPRINT(("dpio_get: Connection established with h:%x, can send now.\n", h));
			/* OBSOLETE */
			/*if (pc->openHdl_callback)
				pc->openHdl_callback(h, dpio->conns->n_used, dp_RES_OPENED, pc->openHdl_context);*/
			/* end OBSOLETE */
			if (dpio->callback)
				dpio->callback(h, dpio->conns->n_used, dp_RES_OPENED, dpio->context);

			/* Any packets queued before the connection was established
			 * were not sent- let the first one be sent now.
			 */
			pc->tx.PktTime = *dpio->now;
		}
		pc->state |= dpio_STATE_SYN_RCVD;
		dpio_assertValid(dpio);
		return dp_RES_AGAIN;

		break;
	}

	case dpio_FIN_PACKET_ID:
		pDat = (dpio_data_packet_t *)(((char *)buffer) + sizeof(dp_packetType_t));
		/* We received a FIN; ack it, and send back a FIN if we haven't already.
		 * This handle is closing.
		 */
		if (!dpio->reliable_enabled) {
			dpio_assertValid(dpio);
			return dp_RES_AGAIN;
		}

#define HANDLE_REGENERATE_CODE_ENABLED 1
		/* Regenerate the handle if it's been closed */
		#if defined(HANDLE_REGENERATE_CODE_ENABLED)
		{
			/* KLUGE: we use commDriverInfo's idea of whether broadcast
			   is supported to detect that the commRxPkt call contains
			   the address of the sender.  This works because, in the
			   current driver set, only the internet drivers implement
			   the address copying, and only the internet drivers have
			   the NO_BROADCAST bit turned on. */
			handleRegenerated = FALSE;
			assert(NULL != dpio->driverinfo);
			DPRINT(("dpio_get: got fin from h:%x.\n", h));
			if((PLAYER_NONE == h) && (dpio->driverinfo->capabilities & comm_DRIVER_NO_BROADCAST)) {
				h = dpio_openHdlRaw(dpio, rxPktResp.adr);
				handleRegenerated = (h != PLAYER_NONE);
				DPRINT(("dpio_get: regenerating handle h:%x to ack fin.\n", h));
			}
		}
		#endif

		/* Acknowledge the FIN if there's someone to hear it. */
		if(PLAYER_NONE != h) {
			ackbuf.tag = dpio_ACK_PACKET_ID;
			ackbuf.body.pktnum = pDat->pktnum;
#ifndef OLD_ACK
			ackbuf.body.gotAllUpTo_offset = (char) 128;
#endif
			err = dpio_put_unreliable_buffered(dpio, pc, h, &ackbuf, sizeof(ackbuf.tag)+sizeof_dpio_ack_packet_t);
			/* Suppress an error introduced by the artificial delay code, if present */
			#ifdef DP_MODEL_DELAY
				if ((dpio->delay_q) && (err == dp_RES_BAD))
					err = dp_RES_OK;
			#endif
			/* Handle real errors */
			if (err != dp_RES_OK) {
				DPRINT(("dpio_get: can't ack fin, h:%x, err:%d\n",h,err));
				dpio_assertValid(dpio);
				return dp_RES_BUG;
			}
		}

		#if defined(HANDLE_REGENERATE_CODE_ENABLED)
			/* Remove the handle if it was regenerated above */
			if(handleRegenerated) {
				DPRINT(("dpio_get: removing regenerated handle h:%x.\n", h));
				dpio_closeHdlRaw(dpio, h);
			}
		#endif

		if (!pc) {
			DPRINT(("dpio_get: fin ack: no peer?\n"));
			dpio_assertValid(dpio);
			return dp_RES_AGAIN;	/* Fishy, but not really an error? */
		}
		pw = &pc->rx;
		DPRINT(("dpio_get: t:%d got fin packet: pktnum %d, windowbase %d, h:%x\n", *dpio->now, SwapBytes2(pDat->pktnum), pw->windowBase, h));

		/* Make sure the FIN's sequence number is in the window, if we
		 * know it yet.  Blarg - what if this is a FIN from a previous
		 * connection, being delivered out of order?  Guess this is
		 * a good reason for the 2MSL delay.  FIXME?
		 */
		if (!(pc->state & dpio_STATE_SYN_RCVD)) {
			DPRINT(("dpio_get: got FIN before SYN on h:%x.  Hmm.\n", h));
			/* FIN's from previous sessions are acked and ignored -
			 * they do not affect the client's current connection state.
			 * This fixes a bug encountered by tservt's speed login test.
			 */
			return dp_RES_AGAIN;
		} else if ((unsigned short)(SwapBytes2(pDat->pktnum) - pw->windowBase) >= dpio_WINDOWSIZE) {
			/* pktnum was beyond end of window! */
			/* somehow the sender sent a packet we wasn't supposed to yet. */
			DPRINT(("dpio_get: got FIN pktnum %d, at windowBase %d, outside window?\n",
				SwapBytes2(pDat->pktnum), pw->windowBase));
			dpio_assertValid(dpio);
			return dp_RES_BUG;
		}

		pc->state |= dpio_STATE_FIN_RCVD;
		DPRINT(("dpio_get: sent ack for fin: dest h:%x state:%x, pktnum %d\n", h, pc->state, SwapBytes2(ackbuf.body.pktnum)));
		if ((pc->state & dpio_STATE_CLOSED) == dpio_STATE_CLOSED) {
			/* This handle is closed, and will be deleted */
			DPRINT(("dpio_get: Connection closed with h:%x.\n", h));
			if (flags)
				*flags |= dpio_GET_CHECK_TIMEOUT;
		} else if (!(pc->state & dpio_STATE_FIN_SENT)) {
			/* We've never sent a FIN; they're closing the handle, so
			 * we should close too
			 */
			DPRINT(("dpio_get: Closing connection with h:%x.\n", h));
			dpio_closeHdl(dpio, h);
		}
		dpio_assertValid(dpio);
		return dp_RES_AGAIN;
		break;

	case dpio_DATA_PACKET_ID:
		if (!pc) {
			DPRINT(("dpio_get: No peer or Unknown player sent DATA, ignoring\n"));
			dpio_assertValid(dpio);
			return dp_RES_AGAIN;
		}
		/* If the network packet is a reliable transmission,
		 * place it into rx window according to sender's playerHdl.
		 */
		pDat = (dpio_data_packet_t *)(((char *)buffer) + sizeof(dp_packetType_t));
		/* Check to make sure a SYN has been received */
		if (!(pc->state & dpio_STATE_SYN_RCVD)) {
			DPRINT(("dpio_get: got DAT pktnum %d before SYN from h:%x\n", SwapBytes2(pDat->pktnum), h));
			dpio_assertValid(dpio);
			return dp_RES_AGAIN;
		}
		pw = &pc->rx;
		pktnum = SwapBytes2(pDat->pktnum);
		if (!dpio->reliable_enabled) {
			DPRINT(("dpio_get: t:%d got data packet while disabled: pktnum %d from src h:%x tag %2.2s\n",
				*dpio->now, pktnum, h, pDat->data));
			/* Ignore new packets. */
			if ((short)(pw->windowBase - pktnum) <= 0) {
				dpio_assertValid(dpio);
				return dp_RES_EMPTY;
			}
			/* Packet is *old*.  Ack it so other end knows we already */
			/* processed it.  Otherwise, he'll keep sending forever. */
			/* This is the only thing rdp does when disabled. */
			/* rdp is only disabled after dpFreeze. */
			ackbuf.tag = dpio_ACK_PACKET_ID;
			ackbuf.body.pktnum = pDat->pktnum;
#ifndef OLD_ACK
			ackbuf.body.gotAllUpTo_offset = (char) 128;
#endif
			err = dpio_put_unreliable_buffered(dpio, pc, h, &ackbuf, sizeof(ackbuf.tag)+sizeof_dpio_ack_packet_t);
			/*dpio->nextTxTime = dpio->now + dpio->minTxInterval; */
			if (err != dp_RES_OK) {
				DPRINT(("dpio_get: ack old pkt failed while disabled\n"));
				dpio_assertValid(dpio);
				return dp_RES_BUG;
			}
			DPRINT(("dpio_get: sent ack while disabled: dest h:%x, pktnum %d\n",
				h, pktnum));
			dpio_assertValid(dpio);
			return dp_RES_EMPTY;
		}
#ifdef VERBOSE
		DPRINT(("dpio_get: t:%d got data packet: pktnum %d from src h:%x tag %2.2s; acking\n",
			*dpio->now, pktnum, h, pDat->data));
		/*DPRINT(("dpio_get: ")); dpio_pw(pw); */
#endif
		if ((short)(pw->windowBase+dpio_WINDOWSIZE - pktnum) <= 0) {
			/* pktnum was beyond end of window! */
			/* somehow the sender sent a packet we wasn't supposed to yet. */
			DPRINT(("dpio_get: got pktnum %d, at windowBase %d, beyond end of window\n",
				pktnum, pw->windowBase));
			dpio_assertValid(dpio);
			return dp_RES_BUG;
		}
		if ((short)(pw->windowBase - pktnum) <= 0) {
			/* packet is within the window */
			/* Put it in the window buffer for eventual delivery to the user. */
			dpio_wrapped_data_packet_t *p = &pw->outstanding[(pktnum) % dpio_WINDOWSIZE];
			/*DPRINT(("dpio_get: copying pktnum %d into outstanding[%d]\n",
				pktnum, (pktnum) % dpio_WINDOWSIZE));*/
			memcpy(p, buffer, sizeof_dpio_wdp(pDat->len));
			pw->bAcked[(pktnum) % dpio_WINDOWSIZE] = TRUE;
			/* Indicate one plus the highest packet outstanding in the window... */
			if ((short)(pw->next_pktnum - pktnum) <= 0) {
				/* let the packet number wrap around if necessary */
				pw->next_pktnum = (unsigned short) (pktnum+1);
			}
		} else {
			DPRINT(("dpio_get: ignoring pktnum %d because it's outside window; windowbase %d\n",
				pktnum, pw->windowBase));
		}
		if (pw->next_pktnum < pw->windowBase)
			DPRINT(("dpio_get: rx wrapped around pktnum 0 (normal)\n"));
/* define TICKLEFINBUG */
#ifdef TICKLEFINBUG
		/* To demonstrate the ACK for FIN bug, never acknowledge
		 * a DAT packet at the windowbase.  This forces us to
		 * ACK the FIN packet before the DAT packet, which should
		 * tickle the bug.  The DAT packet will get acked if it is
		 * received again after the next packet comes in.
		 * The test program src/test/dpiot/dpiot.c has an #ifdef
		 * that will set up the conditions needed to tickle this.
		 */
		if (!(pc->state & dpio_STATE_FIN_RCVD)) {
			DPRINT(("dpio_get: don't ack data packet pktnum %d until FIN - trying to tickle FIN bug\n", pktnum));
		} else {
#endif
			if ((pc->remote_capabilities & dpio_REMCAP_GATHER)
			&&  dpio->appWillFlush) {
				/* Mark the handle as needing an ack. */
				dpio_sendAckLater(dpio, pc);
			} else {
				/* Send ack immediately */
				dpio_sendAck(dpio, pc, h, pktnum);
			}
#ifdef TICKLEFINBUG
		}
#endif
		*psize = orig_size;
		*psrc = h;
		err = dpio_window_get(pw, buffer, psize, h);
		if (err == dp_RES_OK) {
			if (*(dp_packetType_t *)buffer == dp_KEEPALIVE_PACKET_ID) {
				/* Just ignore keepalive packets at this level */
				DPRINT(("dpio_get: reliable keepalive packet %2.2s from h:%x, length %d, pktnum %d\n", buffer, *psrc, *psize, pw->windowBase-1));
				dpio_assertValid(dpio);
				return dp_RES_AGAIN;
			} else {
				if (flags)
					*flags |= dpio_GET_RELIABLE;
#ifdef VERYVERBOSE
				DPRINT(("dpio_get: reliable packet %2.2s from h:%x, length %d, pktnum %d\n", buffer, *psrc, *psize, pw->windowBase-1));
#endif
				;
#ifdef dp_STATS
				dpio->stats[dp_STAT_DPIO_RX_REL_PKTS].in++;
				dpio->stats[dp_STAT_DPIO_RX_REL_BYTES].in += *psize;
#endif
			}
		} else if (err != dp_RES_EMPTY) {
			DPRINT(("dpio_get: h:%x window_get returns error %d\n", h, err));
		} else {
			/* Don't return empty here; that would cause him to
			 * stop calling us, and that would be bad.
			 * He should only stop calling us once we stop getting
			 * low-level network traffic.
			 */
			err = dp_RES_AGAIN;
		}
		dpio_assertValid(dpio);
		return err;
		break;

	case dp_PING_PACKET_ID:
		if (rxPktResp.length != sizeof_dpio_ping) /* Old pings handled by dp */
			break;
		if (!pc)
			return dp_RES_AGAIN;
		*((dp_packetType_t *)buffer) = dp_PING_RESP_PACKET_ID;
		dpio_put_unreliable_buffered(dpio, pc, h, buffer, rxPktResp.length);
		return dp_RES_AGAIN;
		break;

	case dp_PING_RESP_PACKET_ID:
		if (rxPktResp.length != sizeof_dpio_ping) /* Old pings handled by dp */
			break;
		if (!pc)
			return dp_RES_AGAIN;
		err = dpio_handle_ping_response(dpio, pc, h, buffer, rxPktResp.length);
		if ((err == dp_RES_OK) && flags) {
			*flags |= dpio_GET_LATENCY_MEASUREMENT;
			*psrc = h;
		}
		return dp_RES_AGAIN;
		break;

	default:
		break;
	}

	/* If the network packet is not a reliably-wrapped packet,
	 * just return it to the user.
	 */
	*psrc = h;
	*psize = rxPktResp.length;
	dpio_assertValid(dpio);
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Send a packet immediately.
----------------------------------------------------------------------*/
static dp_result_t dpio_put_unreliable_unbuffered(dpio_t *dpio, playerHdl_t dest, void *buffer, size_t size)
{
	commTxPktReq_t	txPktReq;
	commTxPktResp_t	txPktResp;

	txPktReq.flags = 0;
	txPktReq.dest = dest;
	txPktReq.buffer = buffer;
	txPktReq.length = size;
	commTxPkt(&txPktReq,&txPktResp, dpio->commPtr);

	DPRINT(("dpio_put_unreliable_unbuffered: t:%d tag %2.2s ln %d sent to h:%x, status %d\n", *dpio->now, buffer, size, dest, txPktResp.status));

	if (txPktResp.status != comm_STATUS_OK) {
		DPRINT(("dpio_put_unreliable: error: commTxPkt status %d\n", txPktResp.status));
#ifdef dp_STATS
		dpio->stats[dp_STAT_DPIO_TX_UNREL_PKTS].dropped++;
		dpio->stats[dp_STAT_DPIO_TX_UNREL_BYTES].dropped += size;
#endif
		return txPktResp.status;
	}

#ifdef dp_STATS
	dpio->stats[dp_STAT_DPIO_TX_UNREL_PKTS].out++;
	dpio->stats[dp_STAT_DPIO_TX_UNREL_BYTES].out += size;
#endif
	return dp_RES_OK;
}

/* Call this before adding to the gather buffer to decide whether you need
 * to flush first.
 */
#define get_mtu(pc) \
	 (((pc)->remote_capabilities & dpio_REMCAP_JUMBO_PACKETS) ? \
		dpio_MAXLEN_GATHER : dpio_MAXLEN_UNRELIABLE)
#define put_would_flush(pc, size) \
	(((pc)->txGatherBufLen + (size_t)(size) + 1) > (size_t)get_mtu(pc))

/*------------------------------------------------------------------------
 Finish sending any packets sent with dpio_put_unreliable that are
 sitting in the given peer's tx gather buffer.
 Called by dpio_flushPackets() and by dpio_put_unreliable() when needed.
--------------------------------------------------------------------------*/
static dp_result_t dpio_flushPacketsForHandle(dpio_t *dpio, dpio_conn_t *pc, playerHdl_t h)
{
	dp_result_t err;

	/* If nothing more than a header, just return */
	if (pc->txGatherBufLen == 2)
		return dp_RES_OK;
	if ((pc->txGatherBufLen < 2) || (pc->txGatherBufLen == 3))
		return dp_RES_BUG;

#ifdef VERYVERBOSE
	DPRINT(("dpio_flushPacketsForHandle: t:%d h:%x opt:%x t_next:%d, pwf:%d\n",
			*dpio->now, h, pc->options, pc->ping_next_piggyback,
			put_would_flush(pc, sizeof_dpio_ping)));
#endif
	/* If caller has asked for latency updates, sneak a ping packet in
	 * once per ping interval when there's room.
	 * Note that we also try to send forced and keepalive pings nicely here,
	 * before resorting to sending them separately.
	 */
	if ((pc->options & (dpio_OPTION_PIGGYBACK_PING | dpio_OPTION_FORCED_PING
	                    | dpio_OPTION_KEEPALIVE))
	&&  ((long)(*dpio->now - pc->ping_next_piggyback) > 0)
	&&  !put_would_flush(pc, sizeof_dpio_ping)) {
		unsigned char pkt[sizeof(char) + sizeof_dpio_ping];

		/* Simulate dpio_put_unreliable_buffered(dpio, h, new ping pkt); */
		pkt[0] = sizeof_dpio_ping;
		*((dp_packetType_t *)(pkt + sizeof(char))) = dp_PING_PACKET_ID;
		pkt[3] = pc->ping_current_pktnum;
		memcpy(pc->txGatherBuf + pc->txGatherBufLen, pkt, sizeof(char) + sizeof_dpio_ping);
		pc->txGatherBufLen += sizeof(char) + sizeof_dpio_ping;

		pc->ping_next_piggyback = *dpio->now + dpio_current_pingInterval(dpio, pc);
		pc->ping_next_forced = *dpio->now + my_max(dpio->ping_forced_interval, pc->RTTime);
		pc->ping_next_keepalive = *dpio->now + dpio_keepaliveInterval(dpio);
		pc->ping_time_sent[pc->ping_current_pktnum & (dpio_PING_TIME_SENT_N-1)] = *dpio->now;
		pc->ping_bits <<= 1;
		pc->ping_bits |= 1;
#ifdef VERBOSE
		DPRINT(("dpio_flushPacketsForHandle: t:%d, pb ping %d to h:%x, t_pb:%d t_forced:%d t_ka:%d bits:%x\n",
			*dpio->now, pc->ping_current_pktnum, h, pc->ping_next_piggyback,
			pc->ping_next_forced, pc->ping_next_keepalive, pc->ping_bits));
#endif
		pc->ping_current_pktnum++;
	}

	/* strip off gather header if only one packet */
	if ((pc->txGatherBufLen - 3) == (unsigned char)(pc->txGatherBuf[2])) {
		err = dpio_put_unreliable_unbuffered(dpio, h, pc->txGatherBuf + 3, pc->txGatherBufLen - 3);
	} else {
		err = dpio_put_unreliable_unbuffered(dpio, h, pc->txGatherBuf, pc->txGatherBufLen);
	}

	/* Initial state */
	*(dp_packetType_t *)(pc->txGatherBuf) = dpio_GATHER_PACKET_ID;
	pc->txGatherBufLen = sizeof(dp_packetType_t);

	return err;
}

/*----------------------------------------------------------------------
 Place a packet into the outgoing gather buffer.  If needed, send old
 contents.
----------------------------------------------------------------------*/
static dp_result_t dpio_put_unreliable_buffered(dpio_t *dpio, dpio_conn_t *pc, playerHdl_t dest, void *buffer, size_t size)
{
	dp_result_t err = dp_RES_OK;

	/* If no connection, or peer is old, can't buffer */
	if (!pc
	||  !(pc->remote_capabilities & dpio_REMCAP_GATHER)
	||  !dpio->appWillFlush) {
		return dpio_put_unreliable_unbuffered(dpio, dest, buffer, size);
	}

	/* If no room for packet, flush */
	DPRINT(("dpio_put_unreliable_buffered: h:%x gbl %d size %d sum %d\n",
			dest, pc->txGatherBufLen, size,
			pc->txGatherBufLen + size + 1));
	if (put_would_flush(pc, size)) {
		err = dpio_flushPacketsForHandle(dpio, pc, dest);
		if (err != dp_RES_OK) {
			DPRINT(("dpio_put_unreliable_buffered: flush to h:%x failed! err:%d\n",
					dest, err));
		}

	}
	/* Copy packet into gather buffer */
	pc->txGatherBuf[pc->txGatherBufLen++] = (unsigned char) size;
	memcpy(pc->txGatherBuf + pc->txGatherBufLen, buffer, size);
	pc->txGatherBufLen += size;

	/* nothing follows FINs so flush immediately */
	if (*(dp_packetType_t *)buffer == dpio_FIN_PACKET_ID) {
		DPRINT(("dpio_put_unreliable_buffered: sending FIN, flushing h:%x\n", dest));
		err = dpio_flushPacketsForHandle(dpio, pc, dest);
	} else {
		dpio->txGatherNeeded = TRUE;
	}

	return err;
}

/*----------------------------------------------------------------------
 Send a packet unreliably.
 Supports two ways to send to multiple destinations:
 1. List of destinations.  Set nDests to number of destinations, dests
    to address of array of playerHdl_t's (such as an array maintained
	for a player list).
 2. Hardware broadcast.  Set nDests to 1, and nDests to the address
    of a playerHdl_t holding PLAYER_BROADCAST.

 If the network driver supports vector sends, this routine will pass
 the vector send through to the driver to reduce network overhead.
----------------------------------------------------------------------*/
dp_result_t dpio_put_unreliable(
	dpio_t  *dpio,
	const playerHdl_t *dests,/* Vector of destinations */
	int nDests,             /* Size of destination vector */
	void *buffer,
	size_t size,
	playerHdl_t *errDest)   /* If error occurs, dest with error indicated here */
{
	int i;
	dp_result_t err=dp_RES_OK;

	dpio_assertValid(dpio);

	if (size > dpio_MAXLEN_UNRELIABLE) {
		DPRINT(("dpio_put_unreliable: size %d too big, can't send!\n", size));
		dpio_assertValid(dpio);
		return dp_RES_BADSIZE;
	}

	/* No drivers support vector sends yet, so loop here. */
	for (i=0; i<nDests; i++) {
		dpio_conn_t *pc;

		/* Local messages go onto a special queue. */
		if (dests[i] == PLAYER_ME) {
			DPRINT(("dpio_put_unreliable: t:%d tag %2.2s ln %d sent to local queue\n", *dpio->now, buffer, size));
			if (q_put(dpio->localMsgQ, buffer, size) == NULL) {
				DPRINT(("dpio_put_unreliable: localMsgQ is full!\n"));
				if (errDest) *errDest = dests[i];
				err=dp_RES_FULL;
				break;
			}
			continue;
		}

		pc = (dpio_conn_t *) assoctab_subscript(dpio->conns, dests[i]);
		if (!pc) {
			/* Old code (such as enumsrv2) needs to send to raw handles,
			 * so allow this through.  dpio_put_unreliable_buffered reverts
			 * to old behavior if pc is NULL.
			 */
			/* err = dp_RES_BAD;
			 * if (errDest) *errDest = dests[i];
			 * break;
			 */
			DPRINT(("dpio_put_unreliable: sending to raw h:%x\n", dests[i]));
		}
		dpio_put_unreliable_buffered(dpio, pc, dests[i], buffer, size);
	}

	dpio_assertValid(dpio);
	return (i == nDests) ? dp_RES_OK : err;
}

/*----------------------------------------------------------------------
 Send a packet reliably, possibly to multiple destinations.
 On entry,
 nDests is the destination count
 dests is the address of an array of playerHdl_t's of destinations.

 The packet will be placed in the outgoing window buffers for the
 given machines in case it needs retransmission later.  When it
 is acknowledged by each destination, it is removed from that machine's
 window buffer.

 Flags should be dp_SEND_RESERVE_NONE, dp_SEND_RESERVE_QUARTER,
 dp_SEND_RESERVE_HALF, or dp_SEND_RESERVE_THREEQUARTERS.
 This controls how much of the reliable send queue to reserve for later
 sends.
----------------------------------------------------------------------*/
dp_result_t dpio_put_reliable2(
	dpio_t  *dpio,
	const playerHdl_t *dests,/* Vector of destinations */
	int nDests,             /* Size of destination vector */
	void *buffer,
	size_t size,
	playerHdl_t *errDest,  /* If error occurs, dest with error indicated here */
	int flags)
{
	dpio_conn_t *cdests[dpio_MAX_HANDLES];
	dpio_conn_t *pc;
	int i;
	int reserve_entries;

	dpio_assertValid(dpio);

	if (errDest) *errDest = PLAYER_NONE;

	if (dpio == NULL) {
		DPRINT(("dpio_put_reliable: bug: dpio is NULL!\n"));
		dpio_assertValid(dpio);
		return dp_RES_BUG;
	}

	if (size > dpio_MAXLEN_RELIABLE) {
		DPRINT(("dpio_put_reliable: size %d too big, can't send.\n", size));
		dpio_assertValid(dpio);
		return dp_RES_BADSIZE;
	}

	if (!dpio->reliable_enabled) {
		DPRINT(("dpio_put_reliable: bug: disabled!\n"));
		dpio_assertValid(dpio);
		return dp_RES_BUG;
	}

	/* Make sure there's room in window of each destination.
	 * If any of the destinations is overfull, the whole transmission
	 * is aborted, and the caller is notified which destination was
	 * at fault.
	 */
	reserve_entries = (((flags & 0x6) >> 1) * dpio_WINDOWSIZE) / 4;
	for (i=0; i<nDests; i++) {
		unsigned short nq;

		cdests[i] = NULL;
		if (dests[i] == PLAYER_ME) continue;
		if (dests[i] == PLAYER_BROADCAST) {
			DPRINT(("dpio_put_reliable: can't put to PLAYER_BROADCAST\n"));
			if (errDest) *errDest = dests[i];
			dpio_assertValid(dpio);
			return dp_RES_BUG;
		}

		pc = (dpio_conn_t *) assoctab_subscript(dpio->conns, dests[i]);
		cdests[i] = pc;
		if (!pc) {
			DPRINT(("dpio_put_reliable: no connection for peer h:%x\n", dests[i]));
			continue;
		}
		nq = (unsigned short) (pc->tx.next_pktnum - pc->tx.windowBase);
		if (nq >= (dpio_WINDOWSIZE-reserve_entries)) {
			/* Tell caller he should delete this destination, and retry,
			 * or throttle back his transmissions; his choice.
			 */
			if (errDest) *errDest = dests[i];
			DPRINT(("dpio_put_reliable: peer h:%x full; nq >= %d\n", dests[i], nq));
			dpio_assertValid(dpio);
#ifdef dp_STATS
			dpio->stats[dp_STAT_DPIO_TX_REL_PKTS].dropped++;
			dpio->stats[dp_STAT_DPIO_TX_REL_BYTES].dropped += size;
#endif
			return dp_RES_FULL;
		}
	}

	/* Place the packet in the window of each destination. */
	for (i=0; i<nDests; i++) {
		dp_result_t err;

		/* Local messages go onto a special queue. */
		if (dests[i] == PLAYER_ME) {
			DPRINT(("dpio_put_reliable: tag %2.2s ln %d sent to local queue\n", buffer, size));
			if (q_put(dpio->localMsgQ, buffer, size) == NULL) {
				DPRINT(("dpio_put_reliable: bug: localMsgQ is full!\n"));
				if (errDest) *errDest = dests[i];
				dpio_assertValid(dpio);
				return dp_RES_FULL;
			}
			continue;
		}
		if (!cdests[i]) continue;

		err = dpio_q_packet(dpio, dests[i], cdests[i], dpio_DATA_PACKET_ID,
			buffer, size);
#ifdef dp_STATS
		if (err == dp_RES_OK) {
			dpio->stats[dp_STAT_DPIO_TX_REL_PKTS].out++;
			dpio->stats[dp_STAT_DPIO_TX_REL_BYTES].out += size;
			dpio->stats[dp_STAT_DPIO_TX_REL_PKTS].waiting++;
			dpio->stats[dp_STAT_DPIO_TX_REL_BYTES].waiting += size;
/*			DPRINT(("dpio_put_reliable: h:%x, queued %d bytes, pktnum %d, waiting %d %d\n", dests[i], size, (cdests[i]->tx.next_pktnum - 1)&0xffff,
					dpio->stats[dp_STAT_DPIO_TX_REL_PKTS].waiting,
					dpio->stats[dp_STAT_DPIO_TX_REL_BYTES].waiting));*/
		}
#endif
		/* Can't be queue full at this point */
		if (err != dp_RES_OK)
			DPRINT(("dpio_put_reliable: warning: couldn't send, err:%d!\n", err));
		/* but it can be "closed handle" */
		if (err == dp_RES_PEER_CLOSED) {
			if (errDest) *errDest = dests[i];
			dpio_assertValid(dpio);
			return dp_RES_PEER_CLOSED;
		}
	}
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Returns the number of packets in the local message queue.
--------------------------------------------------------------------------*/
int dpio_localQ_npkts(dpio_t *dpio)
{
	return (q_MAXELS - 1 - q_nfree(dpio->localMsgQ));
}

/*--------------------------------------------------------------------------
 Get a list of currently active connection handles
 Returns number of handles.  Will not overflow buffer; if buffer size is
 returned, call again with a larger buffer.
--------------------------------------------------------------------------*/
int dpio_getBroadcastHdls(dpio_t *dpio, playerHdl_t hdls[], int max_nhdls)
{
	int i, nhdls;

	dpio_assertValid(dpio);

	/* Construct destination list on the fly for now. */
	for (i=0, nhdls=0; nhdls < max_nhdls && i < dpio->conns->n_used; i++) {
		assoctab_item_t	*ip;
		dpio_conn_t     *pc;
		unsigned short nq;

		ip = assoctab_getkey(dpio->conns, i);
		if (!ip) continue;

		pc = (dpio_conn_t *) &ip->value;
		nq = (pc->tx.next_pktnum - pc->tx.windowBase);
		if (nq == dpio_WINDOWSIZE) {
			/* If there is no room in the window, exclude this handle */
			DPRINT(("dpio_getBroadcastHdls: h:%x excluded; window full\n", ip->key));
			continue;
		}
		hdls[nhdls++] = (playerHdl_t) ip->key;
	}
	DPRINT(("dpio_getBroadcastHdls: %d handles\n", nhdls));
	dpio_assertValid(dpio);
	return nhdls;
}

/*-----------------------------------------------------------------------
 Call occasionally (once/second).
 If it returns a handle, call dpio_closeHdlImmed() on the handle.

 Scans through all open handles, and checks each handle h as follows:

 1) If h has dpio_OPTION_KEEPALIVE set but no packet has been received
 on h in a long time, it calls
	dpio->callback(h, ..., dp_RES_HOST_NOT_RESPONDING),
 and returns h.

 2) If h has dpio_OPTION_KEEPALIVE set but no packet has been sent
 to h in a long time, it sends them a keepalive packet.

 3) Detects hosts which have actively reset the connection by
 sending us a new SYN with a new packetnumber.
 Also watches handles which dpio_closeHdl() has been called on;
 when their tx/rx buffers are empty, it declares them ready for
 immediate closing by returning them to the caller.

 4) Detects hosts which have a packet waiting to be sent that
 has been retried too many times.

 On exit, returns handle of a player that needs immediate closing,
 or PLAYER_NONE.
-----------------------------------------------------------------------*/
playerHdl_t dpio_findTimedOutHost(dpio_t *dpio)
{
	dp_result_t err;
	int i;
	playerHdl_t		h;
	dpio_conn_t     *pc;

	dpio_assertValid(dpio);

	/* Check all connections for closed or reset connections */
	for (i=dpio->conns->n_used-1; i >= 0; i--) {
		assoctab_item_t	*ip;
		dpio_window_t *txw, *rxw;

		ip = assoctab_getkey(dpio->conns, i);
		if (!ip) continue;
		h = (playerHdl_t) ip->key;
		pc = (dpio_conn_t *) &ip->value;
		txw = &pc->tx;
		rxw = &pc->rx;

		/* Mandatory Checks */
		if (pc->state & dpio_STATE_RST_RCVD) {
			DPRINT(("dpio_findTimedOutHost: found reset h:%x state:%x\n", h, pc->state));
			dpio_assertValid(dpio);
			return h;
		}
		if (pc->state & dpio_STATE_CLOSE_QFULL) {
			dpio_closeHdl(dpio, h);
			DPRINT(("dpio_findTimedOutHost: retried close of h:%x state:%x\n", h, pc->state));
		}
		if ((pc->state & dpio_STATE_CLOSED) == dpio_STATE_CLOSED)
			DPRINT(("dpio_findTimedOutHost: attempting to close h:%x state:%x\n", h, pc->state));
		if (((pc->state & dpio_STATE_CLOSED) == dpio_STATE_CLOSED)
		&&  (txw->windowBase == txw->next_pktnum)) { /* No tx waiting */
			int i;
			int no_unacked_pkts = TRUE;
			unsigned short windowUsed = (unsigned short) (rxw->next_pktnum - rxw->windowBase);	/* careful to wrap around right */

			DPRINT(("dpio_findTimedOutHost: h:%x, no tx waiting\n", h));
			for (i=0; i<windowUsed; i++) {
				if (!rxw->bAcked[(rxw->windowBase + i) % dpio_WINDOWSIZE]) {
					DPRINT(("dpio_findTimedOutHost: h:%x, unacked packet %d\n", h, i));
					no_unacked_pkts = FALSE;
					break;
				}
			}
			if (no_unacked_pkts) {	/* no rx packets waiting */
				dpio_assertValid(dpio);
				return h;
			}
		}

		/* Optional Checks */
		if (pc->options & dpio_OPTION_KEEPALIVE) {
			/* Have they timed out? */
			long rxidle = (long)(*dpio->now - rxw->PktTime);
			if (rxidle >= dpio->maxRxInterval) {
				DPRINT(("dpio_findTimedOutHost: t:%d host timed out: pLastRxTime:%d h:%x maxRxInterval:%d cb:%p\n",
					*dpio->now, rxw->PktTime, h, dpio->maxRxInterval, dpio->callback));
				/* Call the openHdl callback function with an error if it's set */
				/* OBSOLETE */
				/*if (pc->openHdl_callback)
					pc->openHdl_callback(h, dpio->conns->n_used, dp_RES_HOST_NOT_RESPONDING, pc->openHdl_context);*/
				/* end OBSOLETE */
				if (dpio->callback)
					dpio->callback(h, dpio->conns->n_used, dp_RES_HOST_NOT_RESPONDING, dpio->context);
				dpio_assertValid(dpio);
				return h;	/* Caller must call dpio_closeHdl on this handle */
			}
			if (txw->windowBase != txw->next_pktnum) {   /* tx waiting */
				int winpos = txw->windowBase % dpio_WINDOWSIZE;
				int maxretries = dpio_MAX_RETRIES;
				if (pc->RTT_nsamp == 0) maxretries = dpio_MAX_RETRIES_GUESS;
				if (txw->nRetries[winpos] > maxretries) {
					DPRINT(("dpio_findTimedOutHost: t:%d too many retries h:%x\n",
						*dpio->now, h));
					dpio_assertValid(dpio);
					return h;
				}
			}
		}
#ifdef VERYVERBOSE
		DPRINT(("dpio_findTimedOutHost: t:%d h:%x opt:%x t_next:%d t_last_rx:%d txwaiting:%d\n", *dpio->now, h, pc->options, pc->ping_next_forced, pc->rx.PktTime, (pc->tx.windowBase != pc->tx.next_pktnum)));
#endif
		/* If the user has asked for pings on a forced interval and we
		 * haven't managed to piggyback on another packet within a
		 * ping interval, then send it out now.
		 */
		if (((pc->options & dpio_OPTION_FORCED_PING)
			 && ((long)(*dpio->now - pc->ping_next_forced) > 0))
		||  ((pc->options & dpio_OPTION_KEEPALIVE)
			 && (pc->tx.windowBase == pc->tx.next_pktnum) /* No tx waiting*/
			 && ((long)(*dpio->now - pc->ping_next_keepalive) > 0)
			 && ((long)(*dpio->now - pc->rx.PktTime) > dpio_keepaliveInterval(dpio)))) {
			unsigned char pkt[sizeof_dpio_ping];

			*((dp_packetType_t *)pkt) = dp_PING_PACKET_ID;
			pkt[2] = pc->ping_current_pktnum;

			err = dpio_put_unreliable_unbuffered(dpio, h, pkt, sizeof_dpio_ping);
			pc->ping_next_piggyback = *dpio->now + dpio_current_pingInterval(dpio, pc);
			pc->ping_next_forced = *dpio->now + my_max(dpio->ping_forced_interval, pc->RTTime);
#if 0
			pc->ping_next_keepalive = *dpio->now + dpio_keepaliveInterval(dpio);
#else
			/* Because this function is only called every 2 seconds,
			 * can't make next keepalive relative to now if we want
			 * any resolution.  Have to make it relative to the last keepalive.
			 */
			pc->ping_next_keepalive += dpio_keepaliveInterval(dpio);
			/* If that would result in a keepalive storm, back off */
			if ((long)(*dpio->now - pc->ping_next_keepalive) > 0)
				pc->ping_next_keepalive = *dpio->now + dpio_keepaliveInterval(dpio);
#endif
			pc->ping_time_sent[pc->ping_current_pktnum & (dpio_PING_TIME_SENT_N-1)] = *dpio->now;
			pc->ping_bits <<= 1;
			pc->ping_bits |= 1;
			if (err != dp_RES_OK) {
				DPRINT(("dpio_findTimedOutHost: t:%d, forced ping %d to h:%x failed with err:%d, t_pb:%d t_forced:%d t_ka:%d bits:%x\n",
					*dpio->now, pc->ping_current_pktnum, h, err,
					pc->ping_next_piggyback, pc->ping_next_forced, pc->ping_next_keepalive, pc->ping_bits));
			} else {
#ifdef VERBOSE
				DPRINT(("dpio_findTimedOutHost: t:%d, forced ping %d to h:%x, t_pb:%d t_forced:%d t_ka:%d\n",
					*dpio->now, pc->ping_current_pktnum, h,
					pc->ping_next_piggyback, pc->ping_next_forced, pc->ping_next_keepalive));
#endif
			}
			pc->ping_current_pktnum++;
		}
	}
	dpio_assertValid(dpio);
	return PLAYER_NONE;
}

/*-----------------------------------------------------------------------
 Set the options on the given handle.
 Supported options are one or more flag bits or'd together.
 Right now, there is only one flag bit:

 dpio_OPTION_KEEPALIVE
  If we have not received a packet on this handle in the last
  dp_PLAYER_TIMEOUT seconds, declare the handle timed out
  (dpio_findTimedOutHost will find it on the next call after this happens).

  If we have not received a packet on this handle in the last
  dp_PLAYER_TIMEOUT/2 seconds, send a garbage packet to the host reliably,
  which should force it to ack the packet and reset the keepalive timer
  above.

 Returns
   dp_RES_BAD if handle invalid,
   dp_RES_BUG if option invalid,
   dp_RES_OK on success.
-----------------------------------------------------------------------*/
dp_result_t dpio_setHandleOptions(dpio_t *dpio, playerHdl_t h, int options)
{
	dpio_conn_t     *pc;

	dpio_assertValid(dpio);

	DPRINT(("dpio_setHandleOptions(h:%x, options: %x)\n", h, options));
	if (options & ~(dpio_OPTION_KEEPALIVE | dpio_OPTION_PIGGYBACK_PING |
			        dpio_OPTION_FORCED_PING)) {
		DPRINT(("dpio_setHandleOptions: invalid options:%x\n", options));
		dpio_assertValid(dpio);
		return dp_RES_BUG;
	}

	pc = (dpio_conn_t *) assoctab_subscript(dpio->conns, h);
	if (!pc) {
		DPRINT(("dpio_setHandleOptions: can't find connection \n"));
		dpio_assertValid(dpio);
		return dp_RES_BAD;
	}

	/* Don't want old values to cause these timers to get stuck due
	 * to wrap-around.
	 * These three lines are copied from dpio_flush and
	 * dpio_flushPacketsForHandle.  FIXME.
	 */
	pc->ping_next_piggyback = *dpio->now + dpio_current_pingInterval(dpio, pc);
	pc->ping_next_forced = *dpio->now + my_max(dpio->ping_forced_interval, pc->RTTime);
	pc->ping_next_keepalive = *dpio->now + dpio_keepaliveInterval(dpio);

	/* If turning on rx keepalive option on a handle,
	 * be permissive, and assume that the keepalive option
	 * on the other side is being turned on at the same time,
	 * so it's not fair to check timeout yet.
	 */
	if ((options & dpio_OPTION_KEEPALIVE)
	&& (!(pc->options & dpio_OPTION_KEEPALIVE))) {
		/* KLUDGE: Send keepalive immediately ; must match
		 * constant in dpio_findTimedOutHost
		 */
		pc->rx.PktTime = *dpio->now - dpio_keepaliveInterval(dpio);
		pc->ping_next_keepalive = *dpio->now;
	}

	pc->options = (short) options;
	dpio_assertValid(dpio);
	return dp_RES_OK;
}

/*-----------------------------------------------------------------------
 Call frequently to handle retransmissions of packets that have not been
 acknowledged, and to send keepalive packets if needed.
-----------------------------------------------------------------------*/
dp_result_t dpio_update(dpio_t *dpio)
{
	dpio_conn_t *pc;
	dpio_window_t *pw;
	assoctab_item_t *pe;
	playerHdl_t dest;
	dp_result_t err;
	int curTx;

	dpio_assertValid(dpio);

	/* Obey throttling. */
	if ((long)(*dpio->now - dpio->nextTxTime) < 0)
		return dp_RES_OK;

#ifdef VERYVERBOSE
	DPRINT(("dpio_update: t:%d, dpio->nextTxTime %d\n", *dpio->now , dpio->nextTxTime));
#endif
	dpio->nextTxTime += dpio->minTxInterval;
	/* Keep the throttling beacon from falling too far behind */
	if ((long)(dpio->nextTxTime - (*dpio->now + dpio->minTxInterval / 2)) < 0)
		dpio->nextTxTime = *dpio->now + dpio->minTxInterval / 2;

#ifdef dp_STATS
	/* Update running average for transmit window statistics */
	dpio->stats[dp_STAT_DPIO_TX_REL_BYTES].num_waiting++;
	dpio->stats[dp_STAT_DPIO_TX_REL_BYTES].sum_waiting+= dpio->stats[dp_STAT_DPIO_TX_REL_BYTES].waiting;
	dpio->stats[dp_STAT_DPIO_TX_REL_PKTS].num_waiting++;
	dpio->stats[dp_STAT_DPIO_TX_REL_PKTS].sum_waiting+= dpio->stats[dp_STAT_DPIO_TX_REL_PKTS].waiting;
	/* Update running average for connection statistics */
	dpio->stats[dp_STAT_DPIO_CONNECTING].num_waiting++;
	dpio->stats[dp_STAT_DPIO_CONNECTING].sum_waiting+= dpio->stats[dp_STAT_DPIO_CONNECTING].waiting;
	dpio->stats[dp_STAT_DPIO_CONNECTED].num_waiting++;
	dpio->stats[dp_STAT_DPIO_CONNECTED].sum_waiting+= dpio->stats[dp_STAT_DPIO_CONNECTED].waiting;
#endif

	/* Loop over all transfers */
	for (curTx = dpio->conns->n_used - 1; curTx >= 0; curTx--) {
		/* We only want to create one new physical packet, but if there is
		 * already data queued up in the gather buffer, we might as well
		 * fill the existing packet and send one more.
		 */
		int flush_would_send;

		pe = assoctab_getkey(dpio->conns, curTx);
		if (!pe) {
			DPRINT(("dpio_update: no entry for index %d\n", curTx));
			dpio_assertValid(dpio);
			return dp_RES_BUG;
		}
		pc = (dpio_conn_t *) &pe->value;
		dest = (playerHdl_t) pe->key;
		pw = &pc->tx;

		flush_would_send = FALSE;

		/* Transmit if there are packets to retransmit
		 * --AND--
		 * We haven't sent a SYN, gotten an ACK, and not gotten a SYN.
		 * This keeps us from spamming connections we're trying to open.
		 * --AND--
		 * We are past the next retransmission interval
		 */
		if ((pw->windowBase != pw->next_pktnum)
		&&  ((pc->state & dpio_STATE_ESTABLISHED) != (dpio_STATE_SYN_SENT | dpio_STATE_SYN_RCVDACK))
		&&  ((long)(*dpio->now - pw->PktTime) > 0)) {
			dpio_wrapped_data_packet_t *p;
			unsigned short pktnum;

			for (pktnum = pw->windowBase; pktnum != pw->next_pktnum; pktnum++) {
				int winpos = pktnum % dpio_WINDOWSIZE;
				size_t size;

#ifdef VERYVERBOSE
				DPRINT(("dpio_update: h:%x, pktnum %d, nRet %d, bAcked %d\n",
					dest, pktnum, pw->nRetries[winpos], pw->bAcked[winpos]));
#endif
				if (pw->bAcked[winpos])
					continue;
				p = &pw->outstanding[winpos];

				size = sizeof(p->tag) + sizeof(p->body.pktnum) +
				       sizeof(p->body.len) + p->body.len;

				/* Only send one real packet per loop. */
				if (put_would_flush(pc, size)) {
					if (flush_would_send)
						break;
					/* we're about to start a new physical packet */
					flush_would_send = TRUE;
				}
				err = dpio_put_unreliable_buffered(dpio, pc, dest, p, size);
				if (err != dp_RES_OK) {
					DPRINT(("dpio_update: dpio_put_unreliable err:%d\n", err));
				}
				DPRINT(("dpio_update: t:%d sent pktnum %d tag %2.2s to h:%x fws:%d\n",
					*dpio->now, SwapBytes2(p->body.pktnum), p->body.data, dest, flush_would_send));

				/* If this is the first time this packet has been sent, start
				 * the round trip timer.
				 */
				if (pw->nRetries[winpos] == 0)
					pw->pktQTime[winpos] = *dpio->now;
				pw->nRetries[winpos]++;

				/* Don't send data if we haven't finished connecting */
				if ((pc->state & dpio_STATE_ESTABLISHED) != dpio_STATE_ESTABLISHED)
					break;
			}
			/* Set the next retransmission time.  Increment ReTxFactor if
			 * we haven't hit the ceiling.
			 */
			if (dpio_setNextTxTime(dpio, pc) < dpio->maxTxInterval)
				pw->ReTxFactor++;
		}
	}

	dpio_assertValid(dpio);
	return dp_RES_EMPTY;	/* No packets to transmit */
}

#if defined(dpio_LOG) && (defined(DEBUG) || defined(_DEBUG))
#include <time.h>
/* Packet histogram stuff.
 * Storage should be part of dpio_t, but is static for now.
 */
static long dpio_pkts=0;
static long dpio_pkts_tx_full=0;
static long **dpio_hist_pkts=NULL;
static long **dpio_hist_bytes=NULL;
static long dpio_bytes=0;
static long start_time;
static char bGotDatapacket=0;

static void free_2dArray(long **toFree, int outer) {
	int i;

	for (i=0; i<outer; i++)
		if (toFree[i]) dp_FREE(toFree[i]);

	dp_FREE(toFree);
}

static long **alloc_2dArray(int outer, int inner) {
	int 	i;
	long 	**newArray;

	if (!(newArray = (long **)(dp_MALLOC(outer * sizeof(long *)))))
		return NULL;

	for (i=0; i<outer; i++)
		newArray[i]=0;

	for (i=0; i<outer; i++) {
		newArray[i] = (long *)(dp_MALLOC(inner * sizeof(long)));
		if (!newArray[i]) {
			free_2dArray(newArray, outer);
			return NULL;
		}
	}

	return (newArray);
}

static void zero_2dArray(long **toZero, int outer, int inner) {
	int i, j;

	for (i=0; i<outer; i++)
		for (j=0; j<inner; j++)
			toZero[i][j] = 0;
}

static void dpio_log_init()
{
	int idx, idy;

	/* Allocate histogram arrays */
	if (!(dpio_hist_pkts = alloc_2dArray(256, 256)))
		return;
	if (!(dpio_hist_bytes = alloc_2dArray(256, 256))) {
		dpio_hist_pkts = NULL;	/* flag to abort */
		return;
	}

	/* Initialize histogram slots */
	for (idx=0; idx<=255; idx++) {
		for (idy=0; idy<=255; idy++) {
			dpio_hist_pkts[idx][idy] = 0;
			dpio_hist_bytes[idx][idy] = 0;
		}
	}
	dpio_log_dropped = 0;
	dpio_log_rx = 0;

	start_time = time(0);
}

static void dpio_log_term()
{
	int idx, idy;
	long elapsed;
	long bps;
	long bpp;
	float pps;

	if (!dpio_hist_pkts) return;

	elapsed = time(0) - start_time;
	bps = ( (elapsed==0) ? 0 : ((8 * dpio_bytes) / elapsed) );
	pps = ( (elapsed==0) ? 0 : (dpio_pkts / (float)elapsed) );
	bpp = (dpio_pkts ? (dpio_bytes/dpio_pkts) : 0);

	DPRINT((				"\
dpio_STATS:  Type   Hex      Pkts Pkts/Sec Bits/Sec AvgPktSize(Bytes)\n"));
	for (idx=0; idx<=255; idx++) {
		for (idy=0; idy<=255; idy++) {
			if (dpio_hist_pkts[idx][idy] && elapsed)	{
				DPRINT((	"\
dpio_STATS:  %c%c     %02x%02x %8d   %3.1f    %6d    %3d\n",
					isprint(idx)?idx:'?', isprint(idy)?idy:'?',
					idx, idy,
					dpio_hist_pkts[idx][idy], dpio_hist_pkts[idx][idy]/(float)elapsed,
					(dpio_hist_bytes[idx][idy]*8)/elapsed, dpio_hist_bytes[idx][idy]/dpio_hist_pkts[idx][idy]));
			}
		}
	}
	DPRINT(("\
dpio_STATS: Averages: pkts/sec=%4.1f  bits/sec=%ld Bytes/pkt=%ld\n\
dpio_STATS:                  UDP+PPP bits/sec=%ld Bytes/pkt=%ld\n\
dpio_STATS:	Totals:   pkts=%ld      bytes=%ld    secs=%ld\n\
dpio_STATS:	Errors:   tx full=%ld\n\
dpio_STATS:	RX total, dropped:  %ld, %ld = %d%%\n\
",
		pps, bps, bpp,
		(int)(bps + 8 * 33 * pps), bpp + 33, /* 33 is sizeof UDP+IP+PPP headers */
		dpio_pkts, dpio_bytes, elapsed, dpio_pkts_tx_full,
		dpio_log_rx,
		dpio_log_dropped, dpio_log_rx ? (int)(100.0*dpio_log_dropped/dpio_log_rx) : 0));

	/* Free Arrays */
	free_2dArray(dpio_hist_pkts, 256);
	free_2dArray(dpio_hist_bytes, 256);
}

static void dpio_log_pkt_err(dp_result_t err)
{
	if (err == dp_RES_FULL)
		dpio_pkts_tx_full++;
}

static void dpio_log_pkt(const unsigned char *pkt, size_t len, int repeatct)
{
	unsigned int idx, idy;

	if (!dpio_hist_pkts) return;
	len *= repeatct;
	dpio_pkts += repeatct;
	dpio_bytes += len;
	idx = pkt[0];
	idy = pkt[1];
	dpio_hist_pkts[idx][idy] += repeatct;
	dpio_hist_bytes[idx][idy] += len;

	if ((idx == 'D') && (idy == 'Y') && !bGotDatapacket) {		// First Datapacket - reset profile stats
		bGotDatapacket = TRUE;

		DPRINT(("dpio_log_pkt:	(commTerm) Got a SYN. Resetting stats after %ld secs...\n",
			time(0)-start_time));

		dpio_pkts = 0;
		dpio_bytes = 0;
		dpio_pkts_tx_full=0;

		zero_2dArray(dpio_hist_pkts, 256, 256);
		zero_2dArray(dpio_hist_pkts, 256, 256);

		start_time = time(0);
	}
}
#endif
