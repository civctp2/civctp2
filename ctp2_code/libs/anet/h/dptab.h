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

/*------- Generic Table Transfer --------------------------------------------
 This module provides a generic facility to transmit changes in a set of
 tables across a dpio connection.

 ----------------------------------------------------------------------
 A hypothetical example of how this module might be used:

 Define the constants MYSESSIONS = 1 and SESSIONS = 2.

 Note: In the following discussion, table names are given as NUMBER.NUMBER...
 where each number is a byte, and the dots just separate the numbers.
 For instance, SESSIONS would be key = {2}, keylen = 1.
 Sometimes, tables will be referred to by their key, even though many
 function calls require table pointers rather than table keys.

 Both server and client periodically call dptab_update(), and call
 dptab_handlePacket() whenever a packet arrives that looks like it's for dptab.

 A game server creates two tables with dptab_createTable():
 1) MYSESSIONS, a table of sessions created by clients connected to the server,
 2) SESSIONS, a table of sessions created by clients connected to any server.
 It then waits for clients to log in.
 (Each table contains the same kind of data - but SESSIONS contains
 the data from all the other servers.  We'll talk about how the MYSESSIONS
 tables from the other servers end up merged in the SESSIONS table of
 each server later.)

 A game client wishing to connect creates two tables with dptab_createTable():
 1) MYSESSIONS, a table of sessions created by the client, and
 2) SESSIONS, a table of all sessions created by anybody, then
 connects to the server, 
 calls dptab_addPublisher on SESSIONS to accept session data from the server,
 and sends the server a login packet (presumably using dpio_put_reliable();
 this is beyond the scope of this module).

 The server, in response to the login packet,
 1) calls dptab_addSubscriber on SESSIONS to send it to the client, and 
 2) calls dptab_addPublisher on MYSESSIONS to accept the client's MYSESSIONS
 data.

 The client then 
 calls dptab_addSubscriber on MYSESSIONS to send it to the now-listening server.

 This set of calls leaves us with a happy situation in which the client
 get constantly updated SESSIONS table from the server,
 and the server gets a constantly updated MYSESSIONS table containing
 all the session records published by all the clients attached to it.

 The server, meanwhile, could register a callback on its MYSESSIONS table
 with dptab_setTableCallback(); the callback could calls dptab_set() to copy 
 all the variables that arrive in MYSESSIONS right back to the SESSIONS table.  

 Or the server might itself connect to a master game server which
 behaves just like a normal server but tells dptab_addPublisher to mount 
 the slave servers' MYSESSIONS tables on SESSIONS instead of MYSESSIONS.
 The SESSIONS table on the master server would then contain all the records 
 published in the MYSESSIONS tables of the clients connected to any server.

 Each table entry is tagged with the time it was created on this machine
 and the network handle of the source (as given by dptab_set()).
 To clean up a peer's entries when he disconnects, call
 dptab_delete_bySrc() with the handle of the source that is going away.

 ----------------------------------------------------------------------

 There are two levels of tables.
 Associative arrays (hkeytab_t's) are used at both levels. 
 Keys and subkeys are strings of arbitrary bytes; embedded zeroes are ok.

 The top level is the master table.  Each entry in the master table
 has a name and a pointer to an hkeytab_t.
 The master table is itself an hkeytab_t private to each machine.
 This means that each machine can have a different set of tables.
 The functions dptab_createTable(), dptab_deleteTable(), 
 dptab_getTable(), dptab_addPublisher(), and dptab_deletePublisher()
 take a key/keylen pair which is used as an index into the master table.

 The bottom level is the set of hkeytab_t's pointed to by the master table.
 The functions dptab_set(), dptab_delete(), and dptab_get_bykey()
 take a table pointer and a subkey/subkeylen pair which is used
 as an index into the given table.

 In this document, I try to consistently use the word "key" to refer
 to an index into the master table, and "subkey" to refer to an index
 into a regular table.

 Locally, one may create and delete tables, and set, get, or delete
 entries within the tables.

 Tables can be "published" by calling dptab_addSubscription() on them
 once for each subscriber.  The subscriber must at the same time
 call dptab_addPublisher() for the data from the publisher to be accepted
 and routed to a table.  This mounts a table from the remote machine 
 onto a table on the
 local machine.  The mount can be plain or filtered.  
 In the case of dpsess, filtered mounts are useful for listing just 
 the sessions with a particular session type, or just the sessions
 not published by a given game server.
 Remote updates for a table are ignored unless they are from a source 
 pair registered with dptab_addPublisher().
 
 Table changes are handled by generic create, delete, and change functions.
 These functions, plus a send function, take a pointer to the hkeytab
 containing the element, the element's full id (both the id of the table
 plus the id of the element within the table), the id length, and finally
 a list of destination handles to propagate the change to.

 Each (change, destination handle) pair is called a transfer.
 Each transfer is tracked independently; current status about each
 transfer, and a chance to abort, are passed to the caller at two-second
 intervals.  (Transfers shorter than two seconds cannot be aborted.)
 A table pointer is provided in the callback so the user code can
 use the same callback for several tables, if desired.
 Note: aborting transfers is not currently supported.

 Note that no buffering is done on nodes during transmission, so
 if a node is changed while it is in transit, the transmission is cancelled
 and restarted automatically.

 Note: the user code bears some responsibility for maintaining table
 consistency across machines using this layer.  For instance, do not
 let the user abort transfers of data that must remain consistant.
 The only transfers that should be aborted are those that the user initiates
 themself.

 Pack/unpack routines may be provided by the user at table
 creation time to optimally compress (and byte-swap) items before/after 
 transmission; this is not yet supported.  It will probably be limited
 to small records, and is intended to allow user code to use
 inefficint structures in memory, but have them packed optimally
 when sent.

 $Log: dptab.h $
 Revision 1.26  1997/09/29 20:35:52  lbennett
 Changed assertion condition for element size.
 Revision 1.25  1997/09/28 03:55:44  dkegel
 Allow other modules access to dptab_table_{freeze, thaw}
 Revision 1.24  1997/09/27 05:34:29  lbennett
 Enabled full validity checking.
 Revision 1.23  1997/09/27 01:58:14  lbennett
 Modified debugging utilities.
 Revision 1.22  1997/09/26 18:42:48  lbennett
 Improved assertValid code; disabled.
 Revision 1.21  1997/09/24 20:48:34  lbennett
 Made dptab_tableRef_freeze/thaw public.
 Revision 1.20  1997/09/24 02:44:10  lbennett
 Added dptab_getkey() and dptab_size() macros.
 Revision 1.19  1997/09/16 18:09:37  lbennett
 Added dptab_getTableContext()
 Revision 1.18  1997/09/14 01:58:04  lbennett
 Modified tables to accept multiple callbacks.
 Revision 1.17  1997/09/04 06:45:10  dkegel
 Added way to request a subscription from a publisher.
 Revision 1.16  1997/08/31 22:17:59  dkegel
 Now compiles under gcc (needed forward reference for struct).
 Revision 1.15  1997/08/31 04:45:19  dkegel
 Better comment for dptab_update().
 Revision 1.14  1997/08/27 18:26:09  dkegel
 Added dptab_SMALL_PACKET.
 Obfusicate dptab exports.
 Revision 1.13  1997/08/23 20:43:22  dkegel
 Store tables indirectly rather than directly in master table
 so the return value of dptab_getTable() remains valid when
 other tables are deleted.
 Revision 1.12  1997/08/22 21:13:50  dkegel
 Addressed many issues raised in the code review:
 1. explained what types are not supposed to be touched by user code
 2. added dptab_tableSize() accessor
 3. Better explanation of hops, and of context
 4. Better explanation of key vs. subkey and master vs. bottom table
 Revision 1.11  1997/08/18 17:16:01  dkegel
 Added dptab_delete_byAge() to support broadcast tables.
 Revision 1.10  1997/08/16 19:09:13  dkegel
 Export user functions.
 Revision 1.9  1997/08/15 01:10:18  dkegel
 Added dptab_delete_bySrc() so we can wipe out session records
 from a particular source in a mixed table.
 Revision 1.8  1997/08/11 23:25:12  dkegel
 Implemented dptab_delete() and dptab_send_delete().
 Revision 1.7  1997/08/11 01:28:20  dkegel
 1. Callback now includes buffer pointer.
 2. setTableCallback now lets you set context, too (duh)
 Revision 1.6  1997/08/10 01:09:11  dkegel
 Added missing prototype.
 Revision 1.5  1997/08/05 22:48:49  dkegel
 Publish/subscribe works.  Callbacks per table.
 Revision 1.4  1997/07/30 23:06:29  dkegel
 Loopback test now works.
 Revision 1.3  1997/07/30 19:04:46  dkegel
 First step in converting to long subkeys.  Passes local selftests.
 Now on to network...
 Revision 1.2  1997/07/13 00:13:28  dkegel
 Added way to iterate thru tables efficiently.
 Revision 1.1  1997/06/24 16:54:17  dkegel
 Initial revision
--------------------------------------------------------------------------*/

#ifndef dptab_h
#define dptab_h

#include <time.h>
#include <stdio.h>

#ifndef DP_API
#if defined(_WIN32) && defined(DP_DLL)
#define DP_API __declspec( dllexport )
#else
#define DP_API
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "hkeytab.h"
#include "dppkt.h"

/*********** Types ***********/

#include "dppackn.h"		/* natural structure packing */

#define dptab_KEY_MAXLEN hkeytab_MAXLEN

/* Maximum number of callbacks that can be registered on a table */
#define dptab_MAX_CALLBACKS	2

/* Function to notify the user code of a transfer in progress. 
 * Each table can have its own callback function; still, a pointer to
 * the table is passed to the callback.
 * User code returns FALSE to cancel, TRUE to continue transfer.
 * if 'src' is PLAYER_ME, it's an outbound transfer.
 * if 'dest' is PLAYER_ME, it's an inbound transfer.
 *
 * if 'err' is dp_RES_OK, the variable is being set.
 * if 'err' is dp_RES_BUSY, the variable is in transit, being sent or received.
 * if 'err' is dp_RES_CLOSED, the variable is being deleted!
 */
struct dptab_s;
struct dptab_table_s;
typedef int (dp_FAR dp_PASCAL *dptab_status_cb)(struct dptab_s *dptab, struct dptab_table_s *table, playerHdl_t src, playerHdl_t dest, char *subkey, int subkeylen, void *buf, size_t sent, size_t total, int seconds_left, void *context, dp_result_t err);

/* Function to pack a structure for transmission - or unpack it. 
 * Returns number of bytes in result, or 0 for failure.
 */
typedef size_t (*dptab_pack_fn)(const void *ibuf, size_t ibuflen, void *obuf, size_t obuflen);

/*---------------------------------------------------------------*
 * Note: all types after this point are private to this module,  *
 * and are not to be referenced by user code                     *
 *---------------------------------------------------------------*/
 
/* Information about each variable.  Appended to end locally - never
 * transmitted.
 */
typedef struct {
	playerHdl_t src;	/* where data element came from; arg to dptab_set() */
	int hops;			/* copy of hops argument of dptab_set() */
	clock_t arrived;	/* when it arrived here.  Used by dptab_delete_byAge */
} dptab_varinfo_t;

/* A type used to hold variable-sized variables.  
 * Fixed-size variables (i.e. if table->elsize != 0) are stored directly,
 * without this wrapper.
 */
typedef struct {
	size_t len;		/* length of user data */
	void *buf;			/* pointer to malloc'd user data */
} dptab_var_t;

/* A peer and all outstanding transfers to or from it, indexed by
 * full id.
 */
typedef struct {
	assoctab_t *tx;		/* [xferid] outstanding outgoing dptab_xfer_t's */
	assoctab_t *rx;		/* [xferid] outstanding incoming dptab_xfer_t's */
	hkeytab_t *pubs;	/* [key] dptab_table_t *'s mounted from this src */
	int cur_tx;			/* round robin offset into tx for fairness */
	int next_xferid;	/* incremented each time we start a multipack xfer */
} dptab_peer_t;

/* A master table entry.  Points to a table of variables.
 * If elsize is nonzero, 'vars' holds the variables directly, and they are
 * all of uniform size.
 * If elsize is zero, 'vars' holds a {pointer, length} pair for each variable.
 */
typedef struct dptab_table_s {
	hkeytab_t *vars;		/* variables under this table id */
	size_t elsize;			/* size of each item, or zero for nonfixed size */
	dptab_pack_fn pack;		/* how to compress for transmission, or NULL */
	dptab_pack_fn unpack;	/* how to decompress on reception, or NULL */
	assoctab_t *subscribers;/* [playerHdl_t]; who to mirror the table to */
	char key[dptab_KEY_MAXLEN];/* key for dptab_getTable() or createTable */
	int keylen;
	dptab_status_cb aCb[dptab_MAX_CALLBACKS];	/* callbacks for status updates */
	void *aContext[dptab_MAX_CALLBACKS];		/* contexts passed to callbacks */
} dptab_table_t;

/* A single variable in transit to or from a particular peer */
typedef struct {

 	size_t cur_offset;		/* how far along in current xfer, or dptab_OFFSET_DELETE */
	dptab_var_t incoming;	/* incoming variable held here until complete */
	size_t allocated;		/* bytes allocated for incoming.buf */
	long crc;				/* checksum */

	dptab_table_t *table;	/* result of dptab_getTable() or createTable */

	char subkey[dptab_KEY_MAXLEN];/* key for dptab_set() */
	int subkeylen;

	int hops;				/* hops left before variable dies */

	int xferid;				/* from peer->next_xferid */

	int is_delete;			/* TRUE, and offset == dptab_OFFSET_DELETE,
							 * if it's a delete command rather than set.
							 */
} dptab_xfer_t;

/* The mother object that holds all the tables and outstanding transfers,
 * and knows how to propagate them.
 */
typedef struct dptab_s {
	hkeytab_t	 *tables;	/* hkeytab_t* of dptab_table_t *   */
	assoctab_t	 *peers;	/* [playerHdl_t] of dptab_peer_t   */

	struct dpio_s *dpio;

    /* cur_dest indicates which destination we checked most recently
	 * in the round robin.  We start the scan with a different destination
	 * each time to try to give each destination a fair share of the outgoing
	 * bandwidth.
	 */
	playerHdl_t cur_dest;

	/* Timer to throttle how often we go thru the pain of dptab_udpate(). */
	clock_t next_send;

#ifdef dp_STATS
	/* Statistics */
	dp_stat_t stats[4];		/* dptab_STAT_RX_BYTES...dptab_STAT_TX_RECORDS*/ 
#endif
} dptab_t;

#ifdef dp_STATS
#define dptab_STAT_RX_BYTES     (dp_STAT_DPTAB_RX_BYTES     - dp_STAT_DPTAB_RX_BYTES)
#define dptab_STAT_RX_RECORDS   (dp_STAT_DPTAB_RX_RECORDS   - dp_STAT_DPTAB_RX_BYTES)
#define dptab_STAT_TX_BYTES     (dp_STAT_DPTAB_TX_BYTES     - dp_STAT_DPTAB_RX_BYTES)
#define dptab_STAT_TX_RECORDS   (dp_STAT_DPTAB_TX_RECORDS   - dp_STAT_DPTAB_RX_BYTES)
#endif

/* Obscure names to slow down the crackers a moment */
#define dptab_addPeer				_daa
#define dptab_addPublisher			_dab
#define dptab_addSubscriber			_dac
#define dptab_create				_dad
#define dptab_createTable			_dae
#define dptab_delete				_daf
#define dptab_deletePeer			_dag
#define dptab_deletePublisher		_dah
#define dptab_deleteSubscriber		_dai
#define dptab_deleteTable			_daj
#define dptab_delete_byAge			_dak
#define dptab_delete_bySrc			_dal
#define dptab_destroy				_dam
#define dptab_freeze				_dan
#define dptab_getTable				_dao
#define dptab_get_byindex			_dap
#define dptab_get_bykey				_daq
#define dptab_handlePacket			_dar
#define dptab_requestSubscription	_darr
#define dptab_send					_das
#define dptab_send_delete			_dat
#define dptab_set					_dau
#define dptab_setTableCallback		_dav
#define dptab_clearTableCallback	_daw
#define dptab_tableSize				_dax
#define dptab_thaw					_day
#define dptab_update				_daz
#define dptab_table_freeze			_darz
#define dptab_table_thaw			_dars

/*--------------------------------------------------------------------------
 Create a dptab_t.
 Returns NULL if it can't allocate enough memory.
--------------------------------------------------------------------------*/
DP_API dptab_t *dptab_create(struct dpio_s *dpio);

/*--------------------------------------------------------------------------
 Destroy a dptab_t.
--------------------------------------------------------------------------*/
DP_API void dptab_destroy(dptab_t *dptab);

/*-----------------------------------------------------------------------
 Save the state of a dptab_t to disk.  Use only if you want to stop your
 program and resume it in a later run with dptab_thaw().
-----------------------------------------------------------------------*/
DP_API dp_result_t dptab_freeze(dptab_t *dptab, FILE *fp);

/*-----------------------------------------------------------------------
 Restore the state of a dptab_t from disk.
 Call after creating a virgin dptab_t.
 Use only if you want to resume your program after calling dptab_freeze()
 and exiting in an earlier run.
-----------------------------------------------------------------------*/
DP_API dp_result_t dptab_thaw(dptab_t *dptab, FILE *fp);

/*-----------------------------------------------------------------------
 Save a reference to a dptab_table_t to disk.
-----------------------------------------------------------------------*/
DP_API dp_result_t dptab_tableRef_freeze(dptab_table_t *table, FILE *fp);

/*-----------------------------------------------------------------------
 Restore a reference to a dptab_table_t from disk.
 Requires that the table's state has been restored (dptab_table_thaw()
 has been called for this table).  Assumes that the table is a member
 of dptab's tables list.
-----------------------------------------------------------------------*/
DP_API dp_result_t dptab_tableRef_thaw(dptab_table_t **table, dptab_t *dptab, FILE *fp);

/*-----------------------------------------------------------------------
 Save the state of a dptab_table_t to disk.
-----------------------------------------------------------------------*/
dp_result_t			/* status */
dptab_table_freeze(
	dptab_table_t *table,	/* (input) table to save */
	dpio_t* dpio,			/* (input) context for player handles */
	FILE *fp);				/* (modified) stream to save to */

/*-----------------------------------------------------------------------
 Restore the state of a dptab_table_t from disk and add it to the dptab_t.
-----------------------------------------------------------------------*/
dp_result_t		/* status */
dptab_table_thaw(
	dptab_t *dptab,		/* (modified) table collection to add table to */
	dpio_t* dpio,		/* (input) context for player handles */
	FILE *fp);			/* (modified) stream to thaw from */

/*--------------------------------------------------------------------------
 Call dptab_update() periodically to propagate changes.
 Twice per second is sufficient.

 When dptab_send() is called, a transfer request for the given item
 is pushed onto the given destination's outgoing transfer list.
 dptab_update() scans through the destinations, checking this list,
 sending packets, updating the transfer requests, and removing those
 that have finished.
 An attempt is made to ensure fairness - large transfers share the road
 with small transfers, and do not block them.
 Note that this means that transfer requests are not fulfilled in the order
 submitted!

 Note that if dptab_send() is called to send a particular item to a particular
 destination, and there is already an outstanding (possibly in progress, even)
 transfer request with the same key and destination, the outstanding transfer
 request is silently replaced by the new one!

 If an xfer's destination is PLAYER_BROADCAST, only one packet
 at a time is sent, and the packets are sent unreliably.
 This is a kludge that allows us to misuse this system to
 broadcast small things periodically, e.g. session advertisements on a LAN.

 Note that if you want to shut down, but still want all the variables you 
 have set so far to propagate, you should loop,
 calling dptab_update(), dpio_update(), dpio_get(), and dpio_readyToFreeze() 
 until the latter returns all clear.

 Returns dp_RES_EMPTY if no transfers waiting; dp_RES_OK if transfers
 waiting but everything OK.
--------------------------------------------------------------------------*/

DP_API dp_result_t dptab_update(dptab_t *dptab);


/*********** Table Management ***********/

/*--------------------------------------------------------------------------
 Create a table.
 Specify the size of the elements of the table.  If zero is given,
 elements are variable-sized.
 Specify functions used to pack and unpack table elements for
 transmission over a network.  (You can specify NULL, but then you
 can't put any pointers in your elements, and your network bandwidth
 usage might be higher than optimal.)

 If cb is not NULL, it will be called to announce the arrival and departure
 (and eventually, intermediate status during transfer) of variables
 in this table.  In particular, when a variable is set, this callback
 will eventually be called at various points during the transfer
 to allow the user to terminate the transfer if desired.

 See dptab_setTableCallback() for discussion of cb and context parameters.

 On success, places pointer to table in *ptable, and returns dp_RES_OK.
 If table already existed, places pointer to table in *ptable, and returns
 dp_RES_ALREADY.
 If table could not be created and didn't already exist, returns dp_RES_NOMEM.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_createTable(dptab_t *dptab, dptab_table_t **ptable, char *key, int keylen, size_t elsize, dptab_pack_fn pack, dptab_pack_fn unpack, dptab_status_cb cb, void *context);

/*--------------------------------------------------------------------------
 Set (register) a table's callback function and callback context.  
 
 Context is used later as the context parameter of the callback function.
 It may be set to NULL if the callback doesn't need it.

 The callback functions will be called for these table events, with err as follows:
	 dp_RES_CREATED: table entry created
	 dp_RES_CHANGED: table entry changed value
	 dp_RES_FINISHED: outbound transfer finished
	 dp_RES_DELETED: table entry deleted
 More than one callback may be registered for any table.

 For instance, if two hosts are subscribed to a table, and dp_set is
 called, all registered callbacks will be called three times:
 once with dp_RES_CHANGED (or dp_RES_CREATED), and twice with dp_RES_FINISHED.

 If a remote system sends us a variable to a table that is not republished, 
 all registered callbacks will be called once with dp_RES_CHANGED 
 (or dp_RES_CREATED).
 If the table is republished to three other hosts, they will then also be
 called three times with dp_RES_FINISHED.

 WARNING: the callback function must not delete the table!

 If a callback is registered twice, the callback will be called
 twice for each event.

 Returns:
   dp_RES_OK	Callback set as requested.
   dp_RES_FULL	Table has too many callbacks; callback not set.
   dp_RES_BAD	Invalid parameter.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_setTableCallback(dptab_table_t *tab, dptab_status_cb cb, void *context);

/*--------------------------------------------------------------------------
 Clear (remove) a table's callback.

 Removes a callback from a table.  The callback will no longer be called
 for any table events; no other callbacks that have been registered on that
 table are affected.  Any context associated with that callback is abandoned.

 If the callback has been registered more than once, only one instance
 of it will be removed.

 Returns:
   dp_RES_OK	Callback cleared as requested.
   dp_RES_EMPTY	Callback not found in table registry.
   dp_RES_BAD	Invalid parameter.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_clearTableCallback(dptab_table_t *tab, dptab_status_cb cb);

/*--------------------------------------------------------------------------
 Return the context for a callback.

 Returns the context associated with cb, or NULL if the callback is not
 registered on tab.  If the callback has been registered more than once,
 returns the context of the first callback encountered.
--------------------------------------------------------------------------*/
DP_API void* dptab_getTableContext(dptab_table_t *tab, dptab_status_cb cb);

/*--------------------------------------------------------------------------
 Delete a table.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_deleteTable(dptab_t *dptab, char *key, int keylen);

/*--------------------------------------------------------------------------
 Get a pointer to the table with the given key.
--------------------------------------------------------------------------*/
DP_API dptab_table_t *dptab_getTable(dptab_t *dptab, char *key, int keylen);

/*--------------------------------------------------------------------------
 Get a pointer to the nth table in the dptab; returns a dptab_table_t*,
 or NULL if there are fewer than n tables in the dptab.
--------------------------------------------------------------------------*/
#define dptab_getkey(dptab,n)  ((dptab_table_t *)hkeytab_getkey(dptab->tables, n))

/*--------------------------------------------------------------------------
 Get the number of tables in the dptab; returns an int.
--------------------------------------------------------------------------*/
#define dptab_size(dptab)  (dptab->tables->n_used)


/*********** Subscription management ***********/

/*--------------------------------------------------------------------------
 Add a subscriber for the given table.
 Initially, and upon any changes, items with hops>0 in the given 
 table are automatically sent to the destination, which places them
 in the corresponding table.
 Note: the subscriber must call dptab_addPublisher(), or he will ignore
 the packets this sends.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_addSubscriber(dptab_t *dptab, dptab_table_t *tab, playerHdl_t dest);

/*--------------------------------------------------------------------------
 Delete a subscriber.  No remote action occurs.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_deleteSubscriber(dptab_t *dptab, dptab_table_t *tab, playerHdl_t dest);

/*--------------------------------------------------------------------------
 Request that the remote system call dptab_addSubscriber on the given table
 to send it to us.
 Eventually, we should have a callback here indicating whether request
 was rejected.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_requestSubscription(dptab_t *dptab, char *key, int keylen, playerHdl_t src, void *cb, void *context);

/*--------------------------------------------------------------------------
 Request that the remote system call dptab_deleteSubscriber on the given
 table to stop sending it to us.  Also delete them as a publisher, so
 future packets from them will be ignored.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_requestUnsubscription(dptab_t *dptab, char *key, int keylen, playerHdl_t h);

/*--------------------------------------------------------------------------
 Find the first subscription to a table matching the given key from the
 given publisher, tell him not to publish it to us any more, and erase all
 traces of the subscription.
 For use e.g. when shutting off remote player or session deltas.
 If you expect multiple subscriptions, call this until it returns
 dp_RES_EMPTY.
 Returns a pointer to the table the subscription dumped into locally.   
--------------------------------------------------------------------------*/
dp_result_t dptab_shutdownMatchingSubscription(dptab_t *dptab, char key, playerHdl_t h, dptab_table_t **ptable);

/*--------------------------------------------------------------------------
 Add a publisher for the given table.
 Other hosts that try to update this table are ignored.
 The key given here is the key of the table on the publisher, which can
 be different from the local key.
 Note: the publisher must call dptab_addSubscriber(), or he will never
 send any packets for this table.

 Also note: this overrides any old table mount from this key of this source.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_addPublisher(dptab_t *dptab, dptab_table_t *table, char *key, int keylen, playerHdl_t src);

/*--------------------------------------------------------------------------
 Delete a publisher.  
 The given table at the given source will no longer be allowed to update
 any table here.
 (Note: the key here is the key on the publisher's machine.)
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_deletePublisher(dptab_t *dptab, char *key, int keylen, playerHdl_t src);

/*--------------------------------------------------------------------------
 Replace one publisher with another in all tables.
 This is a really cheesy method for implementing host migration.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_changePublisher(dptab_t *dptab, playerHdl_t oldSrc, playerHdl_t newSrc);

/*--------------------------------------------------------------------------
 Notify dptab that a new node has entered the session.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_addPeer(dptab_t *dptab, playerHdl_t dest);

/*--------------------------------------------------------------------------
 When closing a dpio comm handle, call this to clean up any transfers that
 had been in progress, and to remove that destination from any subscription
 or publisher lists.  No remote action occurs.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_deletePeer(dptab_t *dptab, playerHdl_t dest);


/*********** Item management ***********/

/*--------------------------------------------------------------------------
 Set the value for item 'key'.
 Value is copied onto the heap, and must be freed later with
 dptab_set (overwriting frees), dptab_delete, dptab_deleteTable, 
 or dptab_destroy.
 If the item does not yet exist in the table, it is created.

 "Hops" is the number of publish/subscribe pairs across which this
 variable should be propagated.  
 If hops=0, the variable is not sent automatically to any subscribers.
 If hops>0, the variable is sent automatically to any current or future
 subscribers; when it arrives there, it is stored with hops=hops-1.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_set(dptab_t *dptab, dptab_table_t *table, char *subkey, int subkeylen, void *buf, size_t len, int hops, playerHdl_t src);

/*--------------------------------------------------------------------------
 Delete a particular item from a table.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_delete(dptab_t *dptab, dptab_table_t *table, char *subkey, int subkeylen);

/*--------------------------------------------------------------------------
 Delete items with a particular source tag from a table.
 Useful for data that should go away when the host that sent the
 data disconnects.
 Returns dp_RES_OK if any were deleted; dp_RES_EMPTY if no elements matched.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_delete_bySrc(dptab_t *dptab, dptab_table_t *table, playerHdl_t src);

/*--------------------------------------------------------------------------
 Delete items that are older than the given number of seconds.
 Useful for broadcast data that is broadcast unreliably 
 (which can only be done by calling dptab_send with dest=PLAYER_BROADCAST;
  dptab_send_delete should also be used then, but can't be relied upon).

 Returns dp_RES_OK if any were deleted; dp_RES_EMPTY if no elements matched.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_delete_byAge(dptab_t *dptab, dptab_table_t *table, int seconds);

/*--------------------------------------------------------------------------
 Given a table pointer and a subkey, get the value of item 'subkey'.
 Caller must have obtained 'table' via a call to dptab_createTable()
 or dptab_getTable().

 If the player or variable does not exist, dp_RES_EMPTY is returned.

 On success,
 *pbuf is filled with the item's buffer address, and
 *plen is filled with the item's length.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_get_bykey(dptab_table_t *table, const char *subkey, int subkeylen, void **pbuf, size_t *plen);

/*--------------------------------------------------------------------------
 Given a table pointer and an index, 
 retrieve the buffer and subkey of the nth item in the given table.
 Caller must have obtained 'table' via a call to dptab_get_table().
 
 On entry, psubkey should point to a single short,
 pbuf should point to a void *, and
 plen should point to a size_t.

 On exit, *psubkey is filled with the item's id,
 *pbuf is filled with the item's buffer address, and
 *plen is filled with the item's length.

 If n >= dptab_tableSize(table), returns dp_RES_EMPTY.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_get_byindex(dptab_table_t *table, int n, void **pbuf, size_t *plen, char *psubkey, int *psubkeylen);

/*--------------------------------------------------------------------------
 Get number of elements in a table.
--------------------------------------------------------------------------*/
DP_API int dptab_tableSize(dptab_table_t *table);

/*********** Explicit network calls ***********/

/*--------------------------------------------------------------------------
 Send item 'key' to a particular destination.
 Useful for items originally set with hops=0; the hops count specified here 
 temporarily overrides any hop count stored by dptab_set().
 Hops is the number of hops left - if hops is 0, it will not be
 resent by the destination.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_send(dptab_t *dptab, dptab_table_t *table, char *subkey, int subkeylen, playerHdl_t dest, int hops);

/*--------------------------------------------------------------------------
 Send deletion of item 'key' to a particular destination.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_send_delete(dptab_t *dptab, dptab_table_t *table, char *subkey, int subkeylen, playerHdl_t dest, int hops);

/*********** Packet-handling ***********/

/*--------------------------------------------------------------------------
 Process a network message generated by this module on another computer.
 Caller should only call if message not recognized in its own code,
 and if the first byte is 'd'.  The packet types are chosen to not clash
 with those used privately by dp in dppkt.h.

 Note: player deletion should be noticed by some other handler, and
 should cause a call to dptab_delete().
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_handlePacket(dptab_t *dptab, playerHdl_t src, size_t len, void *buf);

/* End of function declarations */
#include "dpunpack.h"		/* pop back previous settings */

/* Begin packet declarations (should be in dppkt.h) */
#include "dppack1.h"		/* network structure packing (use PACK, too) */

/*---------------------------------------------------------------*
 * Note: all packet types are private to this module,            *
 * and are not to be referenced by user code                     *
 *---------------------------------------------------------------*/

/* Delete a variable */
#define dptab_DELETE_PACKET_ID		dppt_MAKE('d','*')
typedef struct {
	unsigned char keylen PACK;
	unsigned char subkeylen PACK;
	char key[1] PACK;			/* key, then subkey; dummy size. */
} dptab_delete_packet_t;

/* Get size of struct without key */
#define sizeof_dptab_DELETE_PACKET_HDR (sizeof(dptab_initial_packet_t) - sizeof(char))

/* First (and maybe last) packet is sent with this header */
#define dptab_INITIAL_PACKET_ID		dppt_MAKE('d','!')
typedef struct {
	size_t len PACK;			/* length of user data */
	long crc PACK;				/* checksum */
	unsigned char xferid PACK;
	unsigned char keylen PACK;
	unsigned char subkeylen PACK;
	unsigned char hops PACK;	/* hops left before variable dies */
	char key[1] PACK;			/* key, then subkey; dummy size. */
} dptab_initial_packet_t;

/* Get size of struct without key */
#define sizeof_dptab_INITIAL_PACKET_HDR (sizeof(dptab_initial_packet_t) - sizeof(char))

/* 2nd, 3rd, etc. packets are sent with this packet type and header. */
#define dptab_BODY_PACKET_ID		dppt_MAKE('d','@')
typedef struct {
	unsigned char xferid PACK;	/* Refers back to key in first packet */
} dptab_body_packet_t;
#define dptab_BODY_MAXLEN (dpio_MAXLEN_RELIABLE - sizeof(dptab_body_packet_t) - sizeof(dp_packetType_t))

/* Bundles of small transfers are sent with this header */
#define dptab_SMALL_PACKET_ID		dppt_MAKE('d','%')
#if 0
/* Can't represent this in C */
typedef struct {
	unsigned char hops PACK;		/* all vars in pkt have same hop count */
	unsigned char subkeylen PACK;	/* all vars in pkt have same subkeylen */	
	unsigned char keylen PACK;
	char key[keylen] PACK;			/* table's key in master table. */
	array of {
		unsigned char len PACK;			/* length of user data */
		char subkey[keylen] PACK;		/* item's subkey in table. */
		char data[len] PACK;			/* actual data of transfer */
	}
} dptab_small_packet_t;
#endif

/* Bytes of overhead in a dptab_small_packet.
 * This + keylen + subkeylen + varlen must be <= dpio_MAXLEN_RELIABLE
 * or you have to use a normal transfer rather than a small transfer.
 */
#define dptab_SMALL_PACKET_HDRLEN (sizeof(dp_packetType_t)+4)

#if 0
/* Recipient of one of the above can send this to report an error
 * or reject a transmission.
 */
#define dptab_REPLY_PACKET_ID		dppt_MAKE('d','#')
typedef struct {
	/* The error - either BAD (for bad CRC) or ALREADY (for stop sending,
	 * please, I'm british :-)
	 */
	dp_result_t err PACK;

	/* Enough info to identify the transmission in question. */
	unsigned char xferid PACK;
} dptab_reply_packet_t;
#endif

/* Deleting a table entry */
#define dptab_DELITEM_PACKET_ID		dppt_MAKE('d','$')
typedef struct {
	unsigned char keylen PACK;
	unsigned char subkeylen PACK;
	char key[1] PACK;			/* key, then subkey; dummy size. */
} dptab_delitem_packet_t;

/* Requesting a table subscription
 * This should eventually contain a filter spec, too.
 */
#define dptab_SUBSCRIBE_PACKET_ID		dppt_MAKE('d','^')
typedef struct {
	unsigned char keylen PACK;
	char key[1] PACK;			/* key; dummy size. */
} dptab_subscribe_packet_t;

/* Request to cancel a table subscription */
#define dptab_UNSUBSCRIBE_PACKET_ID		dppt_MAKE('d','&')
typedef dptab_subscribe_packet_t dptab_unsubscribe_packet_t;

/*-------------------------------------------------------------------------
 Someone has requested that we subscribe them to a given table.

 Request has already been byte-swapped to native (not network) byte order
 (but fortunately doesn't have any fields that need swapping, so that was
 easy), and has already had the packet type ripped off it.
 Not public.
-------------------------------------------------------------------------*/
dp_result_t dptab_handleSubscriptionRequest(dptab_t *dptab, playerHdl_t h, dptab_subscribe_packet_t *req, size_t reqlen);

/*-------------------------------------------------------------------------
 Someone has requested that we unsubscribe them from a given table.

 Request has already been byte-swapped to native (not network) byte order
 (but fortunately doesn't have any fields that need swapping, so that was
 easy), and has already had the packet type ripped off it.
-------------------------------------------------------------------------*/
dp_result_t dptab_handleUnsubscriptionRequest(dptab_t *dptab, playerHdl_t h, dptab_unsubscribe_packet_t *req, size_t reqlen);

/*-------------------------------------------------------------------------
Debug-only validity checking
-------------------------------------------------------------------------*/
#if !defined(NDEBUG)

void dptab_assertXfers(assoctab_t *xfers);
void dptab_assertAllXfers(dptab_t* dt);
void dptab_assertPeerXfers(dptab_t* dt);

#define dptab_assertValidPeer(p) {\
	assert((p) != NULL);\
	dptab_assertXfers((p)->tx);\
	dptab_assertXfers((p)->rx);\
}

#define dptab_assertValidTable(t) {\
	assert((t) != NULL);\
	assert((t)->vars != NULL);\
	assert((t)->subscribers != NULL);\
	assert((t)->elsize <= 20000);\
	assert((t)->keylen <= dptab_KEY_MAXLEN);\
	assert((t)->vars->n_used < 1000);\
}

#define dptab_assertValid(d) {\
	assert((d) != NULL);\
	dptab_assertPeerXfers((d));\
}

#else
#define dptab_assertValidPeer(p)
#define dptab_assertValidTable(t)
#define dptab_assertValid(d)
#endif

#include "dpunpack.h"		/* pragmas to revert structure packing */

#ifdef __cplusplus
}
#endif

#endif


