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

#define VERBOSE
/*------- Generic Table Transfer --------------------------------------------
 This module provides a generic facility to mirror hkeytab_t's across 
 a dpio connection.

 $Log: dptab.c $
 Revision 1.54  1997/09/30 06:37:43  lbennett
 Fixed attach-before-fill temporary invalid state in freeze/thaw.
 Revision 1.53  1997/09/28 03:53:42  dkegel
 1. Allow other modules access to dptab_table_{freeze, thaw}
 2. Don't crash when dprinting in dptab_table_thaw
 Revision 1.52  1997/09/27 05:35:46  lbennett
 Changed file-size DPRINTs.
 Revision 1.51  1997/09/27 02:00:22  lbennett
 Fixed a bug, added a bunch of state checking.
 Revision 1.50  1997/09/26 18:43:50  lbennett
 Improved assertValid code; disabled same.
 Revision 1.49  1997/09/25 21:18:46  lbennett
 Fixed variable-length variable freeze bug.
 Revision 1.48  1997/09/25 18:35:13  lbennett
 Fixed variables-incorrect freeze bug.
 Revision 1.47  1997/09/25 17:36:17  dkegel
 Don't call update twice... breaks precondition
 Revision 1.46  1997/09/25 03:49:26  lbennett
 Work in progress.
 Revision 1.45  1997/09/24 21:43:12  lbennett
 Modified types of dptab_tableRef_freeze/thaw.
 Revision 1.44  1997/09/24 01:32:14  anitalee
 check if entry exists before calling callback in dptab_delete
 Revision 1.43  1997/09/22 23:10:19  lbennett
 Added code to save and restore xfers.
 Revision 1.42  1997/09/22 02:34:17  lbennett
 Fixed freeze/thaw.
 Revision 1.41  1997/09/21 04:45:16  lbennett
 Replaced freeze/thaw code; untested.
 Revision 1.40  1997/09/20 04:50:01  anitalee
 handleSubscriptionRequest now adds requestee to peer table
 dptab_set doesn't callback if no new information was set
 Revision 1.39  1997/09/19 17:28:27  lbennett
 Revert to 1.32, split off inner loop of update.
 Revision 1.32  1997/09/16 18:10:08  lbennett
 Added dptab_getTableContext(), conformed to new element names.
 Revision 1.31  1997/09/16 02:06:14  lbennett
 More logging.
 Revision 1.30  1997/09/14 01:56:25  lbennett
 Modified tables to accept multiple callbacks.
 Revision 1.29  1997/09/04 06:45:28  dkegel
 Added way to request a subscription from a publisher.
 Revision 1.28  1997/09/01 20:40:04  dkegel
 Pass length in 'total' rather than 'sent' in callback when deleting
 Revision 1.27  1997/09/01 00:36:23  dkegel
 more gcc compatibility...
 Revision 1.26  1997/08/31 22:15:17  dkegel
 Compiles with gcc.  (Had to cat foo ** to void **... yech.)
 Revision 1.25  1997/08/31 04:30:40  dkegel
 1. If wrapping around in dptab_update(), need to call flush
 to avoid having xfer's moved out from under you.
 We can probably reduce number of packets by only being fair if 
 top xfer is a big one - then we won't usually wrap.
 2. Better comments.
 3. Surrounded most verbose DPRINT's with #ifdef VERBOSE.
 Revision 1.24  1997/08/28 15:20:25  dkegel
 1. Delete incoming xfer's even if dptab_set() fails.
 Haven't seen this happen, but we probably don't want to retry the set!
 2. Better-defined return value for dptab_update().
 Revision 1.23  1997/08/27 18:25:14  dkegel
 Implemented dptab_SMALL_PACKET's which bundle lots of little
 xfers into a single packet.  
 Updated loopback regression test.
 Revision 1.22  1997/08/26 00:18:03  dkegel
 1. Broke dptab_update() into pieces; it was too long.
 Getting ready to gather small updates into single packet.
 2. Redid loopback test to require no user interaction.
 Revision 1.21  1997/08/25 01:23:10  dkegel
 1. Let caller decide how often to do dptab_update(); don't throttle.
 2. If a variable gets deleted while an xfer is pending, delete the
 xfer when it comes up.
 Revision 1.20  1997/08/23 20:44:11  dkegel
 Store tables indirectly rather than directly in master table
 so the return value of dptab_getTable() remains valid when
 other tables are deleted.
 Revision 1.19  1997/08/23 00:40:42  dkegel
 Don't let user delete items he doesn't have!
 Revision 1.18  1997/08/22 21:22:13  dkegel
 1. Added a few more bug checks - in case user callbacks delete table
 2. Added dptab_tableSize() accessor so user code can keep kosher
 Revision 1.17  1997/08/19 21:55:42  dkegel
 1. Better define table callback behavior; each different use
 of the callback has its own err value, and the delete use passes in
 a pointer to the record about to be deleted.
 2. In dptab_delete_by* and dptab_update, I was iterating up through
 an array, and deleting items from it.  Iterate downward now to avoid crash.
 3. Fewer DPRINT's.  Crucial ones now print out the time, too.
 Revision 1.16  1997/08/18 18:43:10  dkegel
 1. More dprint's.
 2. Note that dptab doesn't compress on the fly yet.
 3. Added last missing piece of broadcast kludge, I hope.
 Revision 1.15  1997/08/18 17:14:09  dkegel
 1. Added dptab_delete_byAge
 2. Support PLAYER_BROADCAST in a kludgy way to support distribution
 of session records 
 3. In unit test, first argument now sets name of log file, so all
 nodes can be run in same directory.  (dp.ini should not specify the
 log file for this to work.)
 Revision 1.14  1997/08/18 01:49:41  dkegel
 Call table callback even for local dptab_set()'s.
 Revision 1.13  1997/08/16 18:46:55  dkegel
 1. Export user functions
 2. Use h:%x when dprinting handles
 Revision 1.12  1997/08/16 00:19:20  dkegel
 1. Fixed bug in table entry allocation (hadn't added extra space for
 dptab_varinfo_t).
 2. Added dumpTable().
 3. Better dprints.
 Revision 1.11  1997/08/15 02:19:06  dkegel
 Fixed subtle bug in dptab_delete_bySrc().
 Revision 1.10  1997/08/15 01:11:19  dkegel
 Added dptab_delete_bySrc so we can wipe out session records from
 a particular source when it disconnects.
 Revision 1.9  1997/08/11 23:25:25  dkegel
 Implemented dptab_delete() and dptab_send_delete().
 Revision 1.8  1997/08/11 01:30:31  dkegel
 1. Added dptab_setTableCallback().
 2. dptab_update() now propagates hops (oops)
 3. dptab_addSubscriber() now sends current contents of table
 Note: hops will need to be saved in the variable somewhere :-(
 Revision 1.7  1997/08/08 20:37:35  dkegel
 1. Fixed crash bug in deletePeer.
 2. Split UNITTEST define into UNITTEST (for functions) and dptab_UNITTEST
 (for main).
 Revision 1.6  1997/08/05 22:49:30  dkegel
 Publish/subscribe works.  Callbacks per table.
 Revision 1.5  1997/07/30 23:05:37  dkegel
 Loopback test works, at least the first time around.
 Revision 1.4  1997/07/30 20:19:31  dkegel
 Cleaned up a bit.
 Revision 1.3  1997/07/30 19:05:11  dkegel
 First step in converting to long subkeys.  Passes local selftests.
 Now on to network...
 Revision 1.2  1997/07/13 00:13:52  dkegel
 Added way to iterate thru tables efficiently.
 Switched argument order to be uniformly (buf, len).
 Revision 1.1  1997/06/24 16:54:02  dkegel
 Initial revision
--------------------------------------------------------------------------*/

/* Disable MSVC warnings as follows; the include files generate these when
MSVC's warning level is set to 4.
4201: nonstandard extension used : nameless struct/union
4214: nonstandard extension used : bit field types other than int
4115: named type definition in parentheses */
#if defined(WIN32)
#pragma warning( disable : 4201 4214 4115 )
#endif

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "dp.h"
#include "dpmem.h"
#include "dpprivy.h"
#include "../../demo/utils/dpcrc32.h"
#include "contract.h"

#include "dptab.h"

#define dptab_OFFSET_DELETE 0xffffffff		/* special value meaning delete */
#define dptab_SIZE_DELETE 0xff	/* special, means delete in send_small only */

#if 0
#ifdef UNITTEST
#include <stdio.h>
#define DPRNT
#undef DPRINT
#define DPRINT(s) printf s; fflush(stdout)
#define DPRINTBUFS
#endif
#endif

/* Re-enable MSVC warnings (#@$&%#^& MS) */
/* Disable MSVC warning "unreferenced inline function has been removed" */
#if defined(WIN32)
#pragma warning( default : 4201 4214 4115 )
#pragma warning( disable : 4514 )
#endif


#define DPRINTBUFS

#ifdef DPRINTBUFS
static void dprint_buf(char *buf, size_t len)
{
	unsigned int i;
	if (len > 50) len = 50;
	for (i=0; i<len; i++) {
		DPRINT(("%02x ", ((unsigned char *)buf)[i]));
	}
	DPRINT(("\n"));
}

#else
#define dprint_buf(buf, len)
#endif

/* Forward declarations */
static dp_result_t dptab_get_bykey2(
	dptab_table_t *table, const char *subkey, int subkeylen, void **pbuf, size_t *plen, dptab_varinfo_t *ptag);

/**
* Debug-only validity checking and logging
*/

#if !defined(NDEBUG)	/* Debug-only */

/*-------------------------------------------------------------------------
Internal-only: is a dptab_xfer_t valid?
-------------------------------------------------------------------------*/
#define dptab_assertValidXfer(xfer) {\
	assert((xfer) != NULL);\
	assert((xfer)->subkeylen <= dptab_KEY_MAXLEN);\
	dptab_assertValidTable((xfer)->table);\
}

/*-------------------------------------------------------------------------
Internal-only: is an assoctab of xfers valid?
-------------------------------------------------------------------------*/
void dptab_assertXfers(assoctab_t *at)
{
	int i;
	for (i=0; i<at->n_used; i++) {
		dptab_xfer_t *xfer;
		assoctab_item_t *pe;
		pe = assoctab_getkey(at, i);
		assert(pe != NULL);	/* assoctab of xfers corrupted */
		xfer = (dptab_xfer_t *)pe->value;
		dptab_assertValidXfer(xfer);
	}
}

/*-------------------------------------------------------------------------
Is a dptab valid?
-------------------------------------------------------------------------*/
void dptab_assertPeerXfers(dptab_t* dt)
{
	int i;
	for(i = 0; i < dt->peers->n_used; i++) {
		assoctab_item_t *pe;
		pe = assoctab_getkey(dt->peers, i);
		assert(pe != NULL);	/* assoctab of peers corrupted */
		dptab_assertXfers(((dptab_peer_t *)pe->value)->tx);
		dptab_assertXfers(((dptab_peer_t *)pe->value)->rx);
	}
}

#else
#define dptab_assertValidXfer(xfer)
#define dptab_assertXfers(assoctab)
#define dptab_assertPeerXfers(dptab)
#endif	/* Debug-only */


/*-------------------------------------------------------------------------
Special debug-only logging
-------------------------------------------------------------------------*/
#if 0 && (defined(DPRNT) || defined(DEBUG) || defined(_DEBUG)) /* Special-debug only */
void dumpTable(dptab_table_t *table)
{
	int i;
	DPRINT(("dumpTable: key %s: %d elements\n", 
			key2a(table->key, table->keylen), table->vars->n_used));
	for (i=0; i<table->vars->n_used; i++) {
		void *buf;
		size_t len;
		dptab_varinfo_t tag;
		char subkey[dptab_KEY_MAXLEN];
		int subkeylen;
		dp_result_t err;

		err = dptab_get_byindex(table, i, &buf, &len, subkey, &subkeylen);
		assert(err == dp_RES_OK);
		memcpy(&tag, ((char *)buf) + len, sizeof(tag));
		DPRINT(("dumpTable: key %s subkey %s, hops %d, src h:%x, tag@%p\n", 
				key2a(table->key, table->keylen),
				key2a2(subkey, subkeylen),
				tag.hops, tag.src,((char *)buf)+len));
	}
	dptab_assertValidTable(table);
}

void dumpXfers(assoctab_t *xfers, char *lab)
{
	int i;
	for (i=0; i<xfers->n_used; i++) {
		dptab_xfer_t *xfer;
		assoctab_item_t *pe;
		pe = assoctab_getkey(xfers, i);
		if (!pe)
			break;	/* horrible error */
		xfer = (dptab_xfer_t *)pe->value;
		DPRINT(("%s xfer %d: offset %x alloc %d sub %d id %d key %s subkey %s hops %d\n", 
			lab, i, xfer->cur_offset, xfer->allocated, pe->key, xfer->xferid, 
			xfer->table ? key2a(xfer->table->key, xfer->table->keylen) : "NULL",
			key2a2(xfer->subkey, xfer->subkeylen), xfer->hops));
		assert(xfer->table);
		assert(pe->key == xfer->xferid);
		dptab_assertValidXfer(xfer);
	}
}

void dumpAllXfers(dptab_t* dt, char* lab)
{
	int i;
	for(i = 0; i < dt->peers->n_used; i++) {
		assoctab_item_t *pe;
		pe = assoctab_getkey(dt->peers, i);
		if (!pe)
			break;	/* horrible error */
		dumpXfers(((dptab_peer_t *)pe->value)->tx, lab);
		dumpXfers(((dptab_peer_t *)pe->value)->rx, lab);
	}
}

/*-------------------------------------------------------------------------
 Given a peer, print out a list of all the tables we subscribe
 to from the peer's machine.  For each table, give both its
 name on the peer's machine and what it maps to on this machine.
 The second argument is the prefix to put on each line printed out.
-------------------------------------------------------------------------*/
static void dumpSubscriptionsFromPeer(dptab_peer_t *peer, char *lab)
{
	hkeytab_t *pubs = peer->pubs;	/* dptab_table_t * pubs[remote key];  */
	int i;
	for (i=0; i<pubs->n_used; i++) {
		dptab_table_t **sub;
		hkeytab_item_t *pe;
		pe = hkeytab_getkey(pubs, i);
		if (!pe)
			break;	/* horrible error */
		sub = (dptab_table_t **)pe->value;
		DPRINT(("%s sub %d: peer's table %s maps to our table %p = %s\n", 
			lab, i,
			key2a2(pe->key, pe->keylen),
			*sub, key2a((*sub)->key, (*sub)->keylen) ));
	}
}

/*-------------------------------------------------------------------------
 Given a table, print out a list of all the peers we send it to.
 The second argument is the prefix to put on each line printed out.
-------------------------------------------------------------------------*/
static void dumpPeersWhoSubscribeToTable(dptab_table_t *tab, char *lab)
{
	assoctab_t *subs = tab->subscribers;	/* void subs[playerHdl_t];  */
	int i;
	DPRINT(("%s: table %s is sent to ", lab, key2a(tab->key, tab->keylen)));
	for (i=0; i<subs->n_used; i++) {
		assoctab_item_t *pe;
		pe = assoctab_getkey(subs, i);
		if (!pe)
			break;	/* horrible error */
		DPRINT(("h:%x ", pe->key));
	}
	DPRINT(("\n"));
}

#else		/* (not) Special-debug only */
#define dumpTable(table)
#define dumpXfers(xfers, lab)
#define dumpAllXfers(xfers, lab)
#define dumpSubscriptionsFromPeer(peer, lab)
#define dumpPeersWhoSubscribeToTable(table, lab)
#endif		/* Special-debug only */


/*
static dptab_table_t *tab2dump = NULL;
void dumpit(dptab_table_t *x)
{
	if (x) tab2dump = x;
	if (tab2dump) dumpTable(tab2dump);
}
*/

/*--------------------------------------------------------------------------
 Create a dptab_t.
--------------------------------------------------------------------------*/
DP_API dptab_t *dptab_create(struct dpio_s *dpio)
{
	dptab_t *dptab;

	if (!dpio || !dpio->now) {
		DPRINT(("dptab_create: must specify dpio with valid now pointer\n"));
		return NULL;
	}

	dptab = (dptab_t *)dp_MALLOC(sizeof(dptab_t));
	if (!dptab)
		return NULL;
	memset(dptab, 0, sizeof(dptab_t));

	dptab->peers = assoctab_create(sizeof(dptab_peer_t));
	dptab->tables = hkeytab_create(sizeof(dptab_table_t *));

	if (!dptab->peers || !dptab->tables) {
		if (dptab->peers) assoctab_destroy(dptab->peers);
		if (dptab->tables) hkeytab_destroy(dptab->tables);
		memset(dptab, 0, sizeof(dptab_t));
		dp_FREE(dptab);
		return NULL;
	}

	dptab->cur_dest = 0;
	dptab->dpio = dpio;

	dumpAllXfers(dptab, "dptab_update entry");

	dptab_assertValid(dptab);
	return dptab;
}

/*--------------------------------------------------------------------------
 Destroy a dptab_t.
--------------------------------------------------------------------------*/
void dptab_destroy(dptab_t *dptab)
{
	int i;
	if (!dptab) return;

	dptab_assertValid(dptab);
	dumpAllXfers(dptab, "dptab_update entry");

	/* For all tables.  Step backwards to avoid having to move in delete. */
	for (i=dptab->tables->n_used-1; i>=0; i--) {
		hkeytab_item_t *pe;
		pe = hkeytab_getkey(dptab->tables, i);
		if (!pe)
			break;	/* horrible error */

		/* Delete this table */
		dptab_deleteTable(dptab, pe->key, pe->keylen);
	}

	/* For all peers. */
	for (i=0; i < dptab->peers->n_used; i++) {
		assoctab_item_t *pe;
		pe = assoctab_getkey(dptab->peers, i);
		if (!pe)
			break;	/* horrible error */
		dptab_deletePeer(dptab, pe->key);
	}

	/* Destroy the main tables */
	hkeytab_destroy(dptab->tables);
	assoctab_destroy(dptab->peers);
	memset(dptab, 0, sizeof(dptab_t));
	dp_FREE(dptab);
}

/*--------------------------------------------------------------------------
 When closing a dpio comm handle, call this to clean up any transfers that
 had been in progress, and to remove that destination from any subscription
 lists.  No remote action occurs.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_deletePeer(dptab_t *dptab, playerHdl_t dest)
{
	dptab_peer_t *peer;
	int i;

	dptab_assertValid(dptab);

	peer = (dptab_peer_t *)assoctab_subscript(dptab->peers, dest);
	if (!peer) {
		DPRINT(("dptab_deletePeer: no table for destination h:%x\n", dest));
		dptab_assertValid(dptab);
		return dp_RES_EMPTY;
	}
	dumpXfers(peer->tx, "dptab_deletePeer entry");
	DPRINT(("dptab_deletePeer(dptab, h:%x): peer %p, pubs %p\n", dest, peer, peer->pubs));
	/* Should free up memory in elements of rx and tx! */
	assoctab_destroy(peer->tx);
	//if (peer->incoming.buf) dp_FREE(peer->incoming.buf);
	assoctab_destroy(peer->rx);
	hkeytab_destroy(peer->pubs);
	assoctab_subscript_delete(dptab->peers, dest);

	/* Scan subscription tables and remove this destination from them */
	for (i=0; i < dptab->tables->n_used; i++) {
		hkeytab_item_t *pe;
		dptab_table_t **htable;
		dptab_table_t *table;
		pe = hkeytab_getkey(dptab->tables, i);
		if (!pe)
			break;	/* horrible error */
		htable = (dptab_table_t **) &pe->value;
		table = *htable;
		if (!table)
			break;	/* horrible error */
		/* Delete this destination, if present */
		assoctab_subscript_delete(table->subscribers, dest);
	}
	dptab_assertValid(dptab);
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Notify dptab that a new node has entered the session.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_addPeer(dptab_t *dptab, playerHdl_t dest)
{
	dptab_peer_t *peer, temp;

	precondition(dptab != NULL);
	dptab_assertValid(dptab);

	DPRINT(("dptab_addPeer(%p, h:%x)\n", dptab, dest));

	/* Make sure doesn't already exist */
	peer = (dptab_peer_t *)assoctab_subscript(dptab->peers, dest);
	if (peer) {
		DPRINT(("dptab_addPeer: h:%x already in peer table\n", dest));
		dptab_assertValid(dptab);
		return dp_RES_ALREADY;
	}

	/* Create record for it */
	temp.tx = hkeytab_create(sizeof(dptab_xfer_t));
	temp.rx = hkeytab_create(sizeof(dptab_xfer_t));
	if (!temp.tx || !temp.rx) return dp_RES_NOMEM;
	temp.cur_tx = 0;
	temp.next_xferid = dest*10;	/* for debugging */
	temp.pubs = hkeytab_create(sizeof(dptab_table_t *));

	/* Insert record into table of peers */
	peer = (dptab_peer_t *)assoctab_subscript_grow(dptab->peers, dest);
	if (!peer) {
		dptab_assertValid(dptab);
		return dp_RES_NOMEM;
	}
	*peer = temp;
	DPRINT(("dptab_addPeer(dptab, h:%x) succeeded.  peer=%p, pubs=%p\n", dest, peer, temp.pubs));
	dumpXfers(peer->tx, "dptab_addPeer entry");

	dptab_assertValid(dptab);
	return dp_RES_OK;
}

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
DP_API dp_result_t			/* status */
dptab_createTable(
	dptab_t *dptab,			/* this */
	dptab_table_t **ptable,	/* (output) new table */
	char *key,				/* (input) key under which table is stored */
	int keylen,				/* (input) size of key in bytes */
	size_t elsize,			/* (input) size of table's elements, or 0 for variable size elements */
	dptab_pack_fn pack,		/* (unused) must be NULL */
	dptab_pack_fn unpack,	/* (unused) must be NULL */
	dptab_status_cb cb,		/* (input) callback to register, or NULL for none */
	void *context)			/* (input) context to use with callback */
{
	dptab_table_t **htable;
	dptab_table_t *table;

	precondition(dptab != NULL);
	precondition(ptable != NULL);
	dptab_assertValid(dptab);

	condition(pack == NULL, "pack not yet supported");
	condition(unpack == NULL, "unpack not yet supported");

	/* Check to make sure table doesn't already exist. */
	htable = (dptab_table_t **)hkeytab_subscript(dptab->tables, key, keylen);
	if (htable) {
		DPRINT(("dptab_createTable: table %s already exists\n", key2a(key, keylen)));
		if(cb != NULL)
			dptab_setTableCallback(*htable, cb, context);
		dptab_assertValid(dptab);
		*ptable = *htable;
		return dp_RES_ALREADY;
	}

	/* No table yet.  Create one.  First, allocate table structure: */
	table = (dptab_table_t *)dp_MALLOC(sizeof(dptab_table_t));
	if (!table) {
		DPRINT(("dptab_createTable: can't allocate table\n"));
		dptab_assertValid(dptab);
		return dp_RES_NOMEM;
	}
	memset(table, 0, sizeof(dptab_table_t));

	/* Second, make a spot for it in the master table: */
	htable = (dptab_table_t **)hkeytab_subscript_grow(dptab->tables, key, keylen);
	if (!htable) {
		DPRINT(("dptab_createTable: can't grow tables for %s!\n", key2a(key, keylen)));
		dp_FREE(table);
		dptab_assertValid(dptab);
		return dp_RES_NOMEM;
	}
	/* Finally, save a pointer to the table in the master table. */
	*htable = table;

	table->elsize = elsize;
	table->pack = pack;
	table->unpack = unpack;

	/* Have to store this inside the table, not just as key to table,
	 * because some routines (dptab_send) only get the pointer to the table
	 * and need to know its key.
	 */
	table->keylen = keylen;
	memcpy(table->key, key, keylen);

	/* Create table proper */
	if (elsize) {
		table->vars = hkeytab_create(elsize + sizeof(dptab_varinfo_t));
	} else {
		table->vars = hkeytab_create(sizeof(dptab_var_t));
	}
	if (!table->vars) {
		dptab_assertValid(dptab);
		return dp_RES_NOMEM;
	}

	/* Create subscriber array.  All info currently in key of assoctab! */
	table->subscribers = assoctab_create(0);
	if (!table->subscribers) {
		dptab_assertValid(dptab);
		return dp_RES_NOMEM;
	}

	if(cb != NULL)
		dptab_setTableCallback(table, cb, context);

	DPRINT(("dptab_createTable: created table %s\n", key2a(key, keylen)));
	*ptable = table;
	dptab_assertValidTable(*ptable);
	dptab_assertValid(dptab);
	return dp_RES_OK;
}

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

 The same callback can not be registered twice.

 Returns:
   dp_RES_OK	Callback set as requested.
   dp_RES_ALREADY  Callback was already registered.
   dp_RES_FULL	Table has too many callbacks; callback not set.
   dp_RES_BAD	Invalid parameter.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_setTableCallback(
	dptab_table_t *tab,			/* (modified) table to set callback of */
	dptab_status_cb cb,			/* (input) new callback; must be non-NULL */
	void *context)				/* (input) new context */
{
	int i;

	precondition(tab != NULL);
	precondition(cb != NULL);
	dptab_assertValidTable(tab);

	/* Check to see that it isn't already there */
	for (i = 0; i < dptab_MAX_CALLBACKS; i++) {
		if (cb == tab->aCb[i]) {
			DPRINT(("dptab_setTableCallback: table %s already has callback[%d] == %p\n", key2a(tab->key, tab->keylen), i, cb));
			return dp_RES_ALREADY;
		}
	}

	/* Find space in the callback array */
	for(i = 0; i < dptab_MAX_CALLBACKS; i++) {
		if(NULL == tab->aCb[i]) {
			tab->aCb[i] = cb;
			tab->aContext[i] = context;
			DPRINT(("dptab_setTableCallback: setting table %s callback %d to %p\n",
					key2a(tab->key, tab->keylen), i, cb));
			dptab_assertValidTable(tab);
			return dp_RES_OK;
		}
	}

	/* No space */
	DPRINT(("dptab_setTableCallback: WARNING--table %s callback refused.\n",
			key2a(tab->key, tab->keylen)));
	dptab_assertValidTable(tab);
	return dp_RES_FULL;
}

/*--------------------------------------------------------------------------
 Clear (remove) a table's callback.

 Removes a callback from a table.  The callback will no longer be called
 for any table events; no other callbacks that may be registered on that
 table are affected.

 If the callback has been registered more than once, only one instance
 of it will be removed.

 Returns:
   dp_RES_OK	Callback cleared as requested.
   dp_RES_EMPTY	Callback not found in table registry.
   dp_RES_BAD	Invalid parameter.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_clearTableCallback(
	dptab_table_t *tab,			/* (modified) table to set callback of */
	dptab_status_cb cb)			/* (input) callback to remove; must be non-NULL */
{
	int i;

	precondition(tab != NULL);
	precondition(cb != NULL);
	dptab_assertValidTable(tab);

	/* Find it in the callback array */
	for(i = 0; i < dptab_MAX_CALLBACKS; i++) {
		if(cb == tab->aCb[i]) {
			tab->aCb[i] = NULL;
			tab->aContext[i] = NULL;
			DPRINT(("dptab_clearTableCallback: clearing table %s callback %p\n",
					key2a(tab->key, tab->keylen), cb));
			dptab_assertValidTable(tab);
			return dp_RES_OK;
		}
	}

	/* Not found */
	DPRINT(("dptab_setTableCallback: WARNING--table %s callback refused.\n",
			key2a(tab->key, tab->keylen)));
	dptab_assertValidTable(tab);
	return dp_RES_EMPTY;
}

/*--------------------------------------------------------------------------
 Return the context for a callback.

 Returns the context, or NULL if the callback is not registered on the
 table.  If the callback has been registered more than once, returns the
 context of the first callback encountered.
--------------------------------------------------------------------------*/
DP_API void* dptab_getTableContext(
	dptab_table_t *tab,			/* (input) table to examine */
	dptab_status_cb cb)			/* (input) return context of this callback */
{
	int i;

	dptab_assertValidTable(tab);

	/* Check parameters */
	precondition_r(tab, NULL);
	precondition_r(cb, NULL);

	/* Find it in the callback array */
	for(i = 0; i < dptab_MAX_CALLBACKS; i++) {
		if(cb == tab->aCb[i]) {
			dptab_assertValidTable(tab);
			return tab->aContext[i];
		}
	}

	/* Not found */
	dptab_assertValidTable(tab);
	return NULL;
}

/*--------------------------------------------------------------------------
 Delete a particular table.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_deleteTable(dptab_t *dptab, char *key, int keylen)
{
	dptab_table_t **htable;
	dptab_table_t *table;
	int i;

	precondition(dptab != NULL);
	dptab_assertValid(dptab);

	DPRINT(("dptab_deleteTable(dp, %s, %d)\n", key2a(key, keylen), keylen));

	/* Get this table. */
	htable = (dptab_table_t **)hkeytab_subscript(dptab->tables, key, keylen);
	if (!htable) {
		DPRINT(("dptab_deleteTable: no table with key %s\n", key2a(key, keylen)));
		dptab_assertValid(dptab);
		return dp_RES_EMPTY;
	}
	table = *htable;
	if (!table || !table->vars) {
		DPRINT(("dptab_deleteTable: bug: no var array in table %s?\n", key2a(key, keylen)));
		dptab_assertValid(dptab);
		return dp_RES_BUG;
	}
	if (!table->subscribers) {
		DPRINT(("dptab_deleteTable: bug: no subscribers array in table %s?\n", key2a(key, keylen)));
		dptab_assertValid(dptab);
		return dp_RES_BUG;
	}

	/* If the table elements are allocated individually, delete them. */
	if (!table->elsize) {
		int i;
		for (i=table->vars->n_used-1; i>=0; i--) {
			hkeytab_item_t *pe = hkeytab_getkey(table->vars, i);
			dptab_var_t *var;
			if (!pe) continue;
			var = (dptab_var_t *) &pe->value;
			if (var->buf) dp_FREE(var->buf);
			var->buf = NULL;
			/*Handled by destroy, below:
			hkeytab_subscript_delete(table->vars, pe->key);*/
		}
	}
	/* Delete the table's variable table. */
	hkeytab_destroy(table->vars);

	/* Undo any subscriptions. */
	assoctab_destroy(table->subscribers);

	/* Remove publishers of and any transfers that reference this table */
	for(i = 0; i < dptab->peers->n_used; i++) {
		int j;
		dptab_peer_t* peer;
		assoctab_item_t* pPI = assoctab_getkey(dptab->peers, i);
		condition(pPI != NULL, "corrupted peers table");
		peer = (dptab_peer_t*)(pPI->value);
		for (j = peer->tx->n_used-1; j >= 0; j--) {
			dptab_xfer_t *xfer;
			assoctab_item_t *pXI = assoctab_getkey(peer->tx, j);
			condition(pXI != NULL, "corrupted tx table");
			xfer = (dptab_xfer_t *)(pXI->value);
			if (xfer->table == table)
				assoctab_subscript_delete(peer->tx, xfer->xferid);
		}
		for (j = peer->rx->n_used-1; j >= 0; j--) {
			dptab_xfer_t *xfer;
			assoctab_item_t *pXI = assoctab_getkey(peer->rx, j);
			condition(pXI != NULL, "corrupted rx table");
			xfer = (dptab_xfer_t *)(pXI->value);
			if (xfer->table == table)
				assoctab_subscript_delete(peer->rx, xfer->xferid);
		}
		hkeytab_subscript_delete(peer->pubs, key, keylen);
	}

	/* remove the table from the table table, and destroy it. */
	hkeytab_subscript_delete(dptab->tables, key, keylen);
	dp_FREE(table);

	dptab_assertValid(dptab);
	return dp_RES_OK;
}


/*--------------------------------------------------------------------------
 Get a pointer to the table with the given key.
 Pointer remains valid even if other tables are created or deleted.
--------------------------------------------------------------------------*/
DP_API dptab_table_t *dptab_getTable(dptab_t *dptab, char *key, int keylen)
{
	dptab_table_t **htable;

	if (!dptab) return NULL;
	if (!dptab->tables) return NULL;	/* can only happen after destroy! */
	if (!key) return NULL;

	dptab_assertValid(dptab);

	DPRINT(("dptab_get_tab(dptab, key %s):\n", key2a(key, keylen)));

	if (keylen < 0 || keylen > hkeytab_MAXLEN) {
		dptab_assertValid(dptab);
		return NULL;
	}

	/* Get the table. */
	htable = (dptab_table_t **)hkeytab_subscript(dptab->tables, key, keylen);
	if (!htable) {
		dptab_assertValid(dptab);
		return NULL;
	}
	dptab_assertValid(dptab);
	return *htable;
}

/*********** Subscription management ***********/

/*--------------------------------------------------------------------------
 Add a subscriber for the given table.
 Initially, and upon any changes, items not marked "NOFLOOD" in the given 
 table are automatically sent to the destination, which places them
 in the corresponding table.
 Note: the subscriber must call dptab_addPublisher(), or he will ignore
 the packets this sends.
--------------------------------------------------------------------------*/
DP_API dp_result_t
dptab_addSubscriber(
	dptab_t *dptab, dptab_table_t *tab, playerHdl_t dest)
{
	int i;
	dp_result_t err;

	dptab_assertValid(dptab);
	dptab_assertValidTable(tab);

	if (NULL == assoctab_subscript_grow(tab->subscribers, dest)) {
		dptab_assertValid(dptab);
		return dp_RES_BAD; /* Shouldn't this be dp_RES_NOMEM? */
	}
	DPRINT(("dptab_addSubscriber: %d items in table %s.\n",
			tab->vars->n_used, key2a(tab->key, tab->keylen)));
	dumpPeersWhoSubscribeToTable(tab, "dptab_addSubscriber:");

	/* Now send the existing contents of the table! */
	for (i=0; i<tab->vars->n_used; i++) {
		void *buf;
		size_t len;
		char subkey[dptab_KEY_MAXLEN];
		int subkeylen;
		dptab_varinfo_t tag;

		err = dptab_get_byindex(tab, i, &buf, &len, subkey, &subkeylen);
		if (err != dp_RES_OK) {
			DPRINT(("dptab_addSubscriber: horrible err: can't get var\n"));
			return dp_RES_BAD;
		}
		/* Get hop count from our secret little suffix on each variable */
		memcpy(&tag, ((char *)buf) + len, sizeof(tag));
		/*DPRINT(("dptab_addSubscriber: tag@%p, subkey %s, hops %d\n", 
				((char *)buf)+len, key2a(subkey, subkeylen), tag.hops));*/
		if (tag.hops > 0) {
			int hops;
			hops = tag.hops;
			if (tag.src != PLAYER_ME)
				hops--;
			/* Send if there are hops left */
			err = dptab_send(dptab, tab, subkey, subkeylen, dest, hops);
			if (err != dp_RES_OK) {
				DPRINT(("dptab_addSubscriber: horrible err: can't send var\n"));
				break;	/* Horrible error */
			}
		} else
			DPRINT(("dptab_addSubscriber: ignoring var; hops = %d\n",tag.hops));
	}

	dptab_assertValid(dptab);
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Delete a subscriber.  No remote action occurs.
--------------------------------------------------------------------------*/
DP_API dp_result_t 
dptab_deleteSubscriber(
	dptab_t *dptab, dptab_table_t *tab, playerHdl_t dest)
{
	dptab_assertValid(dptab);

	if (assoctab_subscript_delete(tab->subscribers, dest)) {
		dptab_assertValid(dptab);
		return dp_RES_BAD;
	}
	(void) dptab;
	dptab_assertValid(dptab);
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
 Someone has requested that we subscribe them to a given table.

 Request has already been byte-swapped to native (not network) byte order
 (but fortunately doesn't have any fields that need swapping, so that was
 easy), and has already had the packet type ripped off it.
-------------------------------------------------------------------------*/
dp_result_t
dptab_handleSubscriptionRequest(
	dptab_t *dptab, playerHdl_t h, dptab_subscribe_packet_t *req, size_t reqlen)
{
	dptab_table_t *table;

	dptab_assertValid(dptab);

	DPRINT(("dptab_handleSubscriptionRequest: src h:%x, t:%d, table %s\n", 
			h, *(dptab->dpio->now), key2a(req->key, req->keylen)));
	if (reqlen < 2 || reqlen > 1 + dptab_KEY_MAXLEN) {
		DPRINT(("dptab_handleSubscriptionRequest: request %d bytes; bad size\n", reqlen));
		return dp_RES_BUG;
	}
	table = dptab_getTable(dptab, req->key, req->keylen);
	if (!table) {
		DPRINT(("dptab_handleSubscriptionRequest: no such table %s\n", 
				key2a(req->key, req->keylen)));
		return dp_RES_EMPTY;
	}

	/***** !!!!!!!! could be a security risk !!!!!!!! *****/
	dptab_addPeer(dptab, h);
	dptab_assertValid(dptab);
	return dptab_addSubscriber(dptab, table, h);
}

/*-------------------------------------------------------------------------
 Someone has requested that we unsubscribe them from a given table.

 Request has already been byte-swapped to native (not network) byte order
 (but fortunately doesn't have any fields that need swapping, so that was
 easy), and has already had the packet type ripped off it.
-------------------------------------------------------------------------*/
dp_result_t dptab_handleUnsubscriptionRequest(dptab_t *dptab, playerHdl_t h, dptab_unsubscribe_packet_t *req, size_t reqlen)
{
	dptab_table_t *table;
	dp_result_t err;

	dptab_assertValid(dptab);

	DPRINT(("dptab_handleUnsubscriptionRequest: src h:%x, t:%d, table %s\n", 
			h, *(dptab->dpio->now), key2a(req->key, req->keylen)));
	if (reqlen < 2 || reqlen > 1 + dptab_KEY_MAXLEN) {
		DPRINT(("dpHandleUnsubscriptionRequest: request %d bytes; bad size\n", reqlen));
		return dp_RES_BUG;
	}
	table = dptab_getTable(dptab, req->key, req->keylen);
	if (!table) {
		DPRINT(("dptab_handleUnsubscriptionRequest: no such table %s\n", 
				key2a(req->key, req->keylen)));
		return dp_RES_EMPTY;
	}

	err = dptab_deleteSubscriber(dptab, table, h);
	if (err != dp_RES_OK) {
		DPRINT(("dptab_handleUnsubscriptionRequest: can't delete h:%x, err:%d\n", h, err));
		return err;
	}
	dptab_assertValid(dptab);
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Request that the remote system call dptab_addSubscriber on the given table
 to send it to us.
 Eventually, we should have a callback here indicating whether request
 was rejected.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_requestSubscription(dptab_t *dptab, char *key, int keylen, playerHdl_t h, void *cb, void *context)
{
	char buf[dpio_MAXLEN_UNRELIABLE];
	struct {
		dp_packetType_t   tag;
		dptab_subscribe_packet_t body;
	} PACK *pkt = (void *)buf;
	dp_result_t err;

	(void) cb;
	(void) context;

	dptab_assertValid(dptab);

	if (h == PLAYER_ME) {
		dptab_assertValid(dptab);
		return dp_RES_BAD;
	}

	DPRINT(("dptab_requestSubscription(table %s, h:%x)\n", key2a(key, keylen), h));
	pkt->tag = dptab_SUBSCRIBE_PACKET_ID;
	/* No check for keylen in valid range (pkt->body.keylen is unsigned char) */
	pkt->body.keylen = (unsigned char) keylen;
	memcpy(pkt->body.key, key, keylen);

	dptab_assertValid(dptab);
	err = dpio_put_reliable(dptab->dpio, &h, 1, buf, sizeof(pkt->tag)+1+keylen, NULL);
	if (err != dp_RES_OK) {
		DPRINT(("dptab_requestSubscription: error %d sending to h:%x\n", err, h));
	}
	return err;
}

/*--------------------------------------------------------------------------
 Request that the remote system call dptab_deleteSubscriber on the given
 table to stop sending it to us.  Also delete them as a publisher, so
 future packets from them will be ignored.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_requestUnsubscription(dptab_t *dptab, char *key, int keylen, playerHdl_t h)
{
	char buf[dpio_MAXLEN_UNRELIABLE];
	struct {
		dp_packetType_t   tag;
		dptab_unsubscribe_packet_t body;
	} PACK *pkt = (void *)buf;
	dp_result_t err;

	dptab_assertValid(dptab);

	if (h == PLAYER_ME) {
		dptab_assertValid(dptab);
		return dp_RES_BAD;
	}

	DPRINT(("dptab_requestUnsubscription(table %s, h:%x)\n", key2a(key, keylen), h));
	pkt->tag = dptab_UNSUBSCRIBE_PACKET_ID;
	/* No check for keylen in valid range (pkt->body.keylen is unsigned char) */
	pkt->body.keylen = (unsigned char) keylen;
	memcpy(pkt->body.key, key, keylen);

	dptab_deletePublisher(dptab, key, keylen, h);
	
	dptab_assertValid(dptab);
	err = dpio_put_reliable(dptab->dpio, &h, 1, buf, sizeof(pkt->tag)+1+keylen, NULL);
	if (err != dp_RES_OK) {
		DPRINT(("dptab_requestUnsubscription: error %d sending to h:%x\n", err, h));
	}
	return err;
}

/*--------------------------------------------------------------------------
 Find the first subscription to a table matching the given key from the
 given publisher, tell him not to publish it to us any more, and erase all
 traces of the subscription.
 For use e.g. when shutting off remote player or session deltas.
 If you expect multiple subscriptions, call this until it returns
 dp_RES_EMPTY.
 Returns a pointer to the table the subscription dumped into locally.   
--------------------------------------------------------------------------*/
dp_result_t dptab_shutdownMatchingSubscription(dptab_t *dptab, char key, playerHdl_t h, dptab_table_t **ptable)
{
	dptab_peer_t *peer;
	int j;

	*ptable = NULL;
	if (h == PLAYER_NONE) {
		DPRINT(("dptab_shutdownMatchingSubscription: h:PLAYER_NONE?\n"));
		return dp_RES_BAD;
	}

	peer = (dptab_peer_t *)assoctab_subscript(dptab->peers, h);
	if (!peer) {
		DPRINT(("dptab_shutdownMatchingSubscription: source h:%x not in peer table\n", h));
		dptab_assertValid(dptab);
		return dp_RES_CLOSED;
	}
	if (!peer->pubs) {
		DPRINT(("dptab_shutdownMatchingSubscription: bug: bad peer record for h:%x\n", h));
		return dp_RES_BUG;
	}

	for (j = 0; j < peer->pubs->n_used; j++) {
		hkeytab_item_t *p = hkeytab_getkey(peer->pubs, j);
		if (!p) {
			DPRINT(("dptab_shutdownMatchingSubscription: bug: can't fetch publisher %d; pubs->n_used %d\n", j, peer->pubs->n_used));
			return dp_RES_BUG;
		}
		/* We only want to blow away the first matching subscription */
		if (p->key[0] == key) {
			*ptable = *((dptab_table_t **)(p->value));
			dptab_requestUnsubscription(dptab, p->key, p->keylen, h);
			break;
		}
	}
	if (j == peer->pubs->n_used)
		return dp_RES_EMPTY;
#if 0
	/* Well, we'd like to delete him if he's no longer pushing anything
	 * to us, but we might theoretically be pushing to him, and checking
	 * that would be horribly painful.
	 */
	if ((peer->pubs->n_used == 0)
	&& (dptab_countSubscriptions(dp->dt, h) == 0)) {
		dptab_deletePeer(dp->dt, h);
		dpio_closeHdl(dp->dpio, h);
	}
#endif
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Add a publisher for the given table.
 Other hosts that try to update this table are ignored.
 The key given here is the key of the table on the publisher, which can
 be different from the local key.
 Note: the publisher must call dptab_addSubscriber(), or he will never
 send any packets for this table.

 Also note: this overrides any old table mount from this key of this source.

 Also also note: you can, if you like, let several hosts publish into
 the same table on your machine.  The table then becomes the union of
 the publishers' tables.  The only bad thing about this is how to clean
 up the table if one of the publishers dies.  That's bad enough that it
 might be best to avoid it.
--------------------------------------------------------------------------*/
DP_API dp_result_t
dptab_addPublisher(
	dptab_t *dptab, dptab_table_t *table, char *key, int keylen, playerHdl_t src)
{
	dptab_table_t **pt;
	dptab_peer_t *peer;

	dptab_assertValid(dptab);

	peer = (dptab_peer_t *)assoctab_subscript(dptab->peers, src);
	if (!peer) {
		DPRINT(("dptab_addPublisher: source h:%x not in peer table\n", src));
		dptab_assertValid(dptab);
		return dp_RES_CLOSED;
	}
	DPRINT(("dptab_addPublisher: adding h:%x table %s as source of table %s.\n",
			src, key2a(key, keylen), key2a2(table->key, table->keylen)));
	dumpXfers(peer->tx, "dptab_addPublisher entry");

	pt = hkeytab_subscript_grow(peer->pubs, key, keylen);
	if (!pt) {
		dptab_assertValid(dptab);
		return dp_RES_NOMEM;
	}
	*pt = table;
	dumpSubscriptionsFromPeer(peer, "dptab_addPublisher:");
	dumpXfers(peer->tx, "dptab_addPublisher exit");

	dptab_assertValid(dptab);
	return dp_RES_OK;
}
	
/*--------------------------------------------------------------------------
 Delete a publisher.  
 The given table at the given source will no longer be allowed to update
 any table here.
 (Note: the key here is the key on the publisher's machine.)

 (Big Note: this doesn't delete any data the given publisher sent us.
 If you want to do this, the best way might be to mount only one
 publisher's data into any given table, then when you notice it's time
 to delete that publisher, delete the table[s] devoted to him.)
--------------------------------------------------------------------------*/
DP_API dp_result_t
dptab_deletePublisher(dptab_t *dptab, char *key, int keylen, playerHdl_t src)
{
	dptab_peer_t *peer;

	dptab_assertValid(dptab);

	DPRINT(("dptab_deletePublisher: key %s, src h:%x\n", key2a(key, keylen), src));
	peer = (dptab_peer_t *)assoctab_subscript(dptab->peers, src);
	if (!peer) {
		dptab_assertValid(dptab);
		return dp_RES_CLOSED;
	}
	dumpXfers(peer->tx, "dptab_deletePublisher entry");

	if (hkeytab_subscript_delete(peer->pubs, key, keylen)) {
		dptab_assertValid(dptab);
		return dp_RES_EMPTY;
	}

	dumpSubscriptionsFromPeer(peer, "dptab_deletePublisher:");
	dumpXfers(peer->tx, "dptab_deletePublisher exit");
	dptab_assertValid(dptab);
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Find items with a particular source tag from a table, and replace their 
 source tag with the new value.
 Useful during cheesy host migration.
--------------------------------------------------------------------------*/
static dp_result_t
dptab_changeSrc(
	dptab_t *dptab, dptab_table_t *table, playerHdl_t oldSrc, playerHdl_t newSrc)
{
	void *varbuf;
	size_t varlen;
	int i;

	DPRINT(("dptab_changeSrc(h:%x h:%x):\n", oldSrc, newSrc));

	precondition(dptab != NULL);
	precondition(table != NULL);
	precondition(table->vars != NULL);
	dptab_assertValid(dptab);
	dptab_assertValidTable(table);

	DPRINT(("dptab_changeSrc(table %s, h:%x, h:%x): n_used %d\n", 
			key2a(table->key, table->keylen), oldSrc, newSrc, table->vars->n_used));

	/* For each key/value pair */
	for (i=table->vars->n_used-1; i>=0; i--) {
		dptab_varinfo_t tag;
		hkeytab_item_t *pe;

		pe = hkeytab_getkey(table->vars, i);
		if (!pe) {
			DPRINT(("dptab_changeSrc: no such element\n"));
			dptab_assertValid(dptab);
			return dp_RES_EMPTY;
		}
		/* Retrieve variable's size and address */
		if (table->elsize) {
			varlen = table->elsize;
			varbuf = &pe->value;
		} else {
			dptab_var_t *var = (dptab_var_t *)&pe->value[0];
			if (!var)
				return dp_RES_BUG;
			varlen = var->len;
			varbuf = var->buf;
		}
		/* Retrieve secret tag */
		memcpy(&tag, ((char *)varbuf) + varlen, sizeof(tag));
		DPRINT(("dptab_changeSrc: item %d src h:%x hops %d\n",
				i, tag.src, tag.hops));
		if (tag.src == oldSrc) {
			DPRINT(("dptab_changeSrc: src matches; deleting\n"));
			/* Must make copy of key - else it goes away before 
			 * dptab_send_delete is called! 
			 */
			tag.src = newSrc;
			memcpy(((char *)varbuf) + varlen, &tag, sizeof(tag));
		}
	}
	dptab_assertValid(dptab);
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Replace one publisher with another in all tables.
 If the new publisher is PLAYER_ME, delete him.
 This is a really cheesy method for implementing host migration.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_changePublisher(dptab_t *dptab, playerHdl_t oldSrc, playerHdl_t newSrc)
{
	dptab_peer_t *oldPeer;
	dptab_peer_t *newPeer;
	int i;
	dp_result_t err;

	dptab_assertValid(dptab);

	DPRINT(("dptab_changePublisher(, oldSrc h:%x, newSrc h:%x)\n", oldSrc, newSrc));
	oldPeer = (dptab_peer_t *)assoctab_subscript(dptab->peers, oldSrc);
	if (!oldPeer) {
		DPRINT(("dptab_changePublisher: No oldPeer h:%x\n", oldSrc));
		dptab_assertValid(dptab);
		return dp_RES_CLOSED;
	}
	newPeer = (dptab_peer_t *)assoctab_subscript(dptab->peers, newSrc);
	if (!newPeer) {
		DPRINT(("dptab_changePublisher: No newPeer h:%x\n", newSrc));
		dptab_assertValid(dptab);
		return dp_RES_CLOSED;
	}
	DPRINT(("dptab_changePublisher: oldPeer h:%x was allowed to publish %d tables to us\n", oldSrc, oldPeer->pubs->n_used));
	for (i=oldPeer->pubs->n_used; i-- > 0 ;) {
		hkeytab_item_t *tab;
		tab = hkeytab_getkey(oldPeer->pubs, i);
		condition(tab != NULL, "");
		dptab_changeSrc(dptab, *((dptab_table_t **)tab->value), oldSrc, newSrc);
		if (newSrc != PLAYER_ME) {
			err = dptab_addPublisher(dptab, *((dptab_table_t **)tab->value),
								tab->key, tab->keylen, newSrc);
			if (err != dp_RES_OK)
				return err;
		}
		dptab_deletePublisher(dptab, tab->key, tab->keylen, oldSrc);
	}
	return dp_RES_OK;
}

/*********** Item management ***********/

/*--------------------------------------------------------------------------
 Given a table and a subkey, create a (variable-sized) variable of
 the given size.  Return a pointer to the variable, or NULL.
 Do not include the size of the seecrit tag (dptab_varinfo_t).
--------------------------------------------------------------------------*/
static dptab_var_t *
dptab_varCreate(
	dptab_t *dptab, dptab_table_t *table, char *subkey, int subkeylen, size_t len)
{
	dptab_var_t *var;

	dptab_assertValid(dptab);

	var = (dptab_var_t *)hkeytab_subscript_grow(
			table->vars, subkey, subkeylen);
	if (!var) {
		DPRINT(("dptab_varCreate: can't grow table's vars[%d]!\n", subkey));
		dptab_assertValid(dptab);
		return NULL;
	}

	/* Delete old value from heap if present and not big enough */
	if (!var->buf || var->len < len) {
		if (var->buf) dp_FREE(var->buf);
		var->buf = dp_MALLOC(len + sizeof(dptab_varinfo_t));
	}
	if (!var->buf) return NULL;
	/* Fill the key/value pair with what we can.  User must fill in
	 * buf.
	 */
	var->len = len;

	(void) dptab;
	dptab_assertValid(dptab);
	return var;
}

/*--------------------------------------------------------------------------
 Given a table and a subkey, delete the (variable-sized) variable.
 Returns zero on success.
--------------------------------------------------------------------------*/
static int
dptab_varDelete(
	dptab_t *dptab, dptab_table_t *table, char *subkey, int subkeylen)
{
	dptab_var_t *var;

	dptab_assertValid(dptab);

	var = (dptab_var_t *)hkeytab_subscript(
				table->vars, subkey, subkeylen);
	if (!var) {
		dptab_assertValid(dptab);
		return 1;
	}

	/* Delete old value from heap if present */
	if (var->buf) dp_FREE(var->buf);

	(void) dptab;
	dptab_assertValid(dptab);
	return hkeytab_subscript_delete(table->vars, subkey, subkeylen);
}


/*--------------------------------------------------------------------------
 Set the value for item 'key'.
 Value is copied onto the heap, and must be freed later with
 dptab_set (overwriting frees), dptab_delete, dptab_deleteTable, 
 or dptab_destroy.
 If the item does not yet exist in the table, it is created.
 Hops should be set to 1 for public variables, and 0 for private ones.
--------------------------------------------------------------------------*/
DP_API dp_result_t
dptab_set(
	dptab_t *dptab, dptab_table_t *table, char *subkey, int subkeylen,
	void *buf, size_t len, int hops, playerHdl_t src)
{
	void *pe;
	void *varbuf;
	dptab_varinfo_t info;
	int i;
	int bIgnore = FALSE;
	int bAlreadyExists;
	dp_result_t status;

	dptab_assertValid(dptab);
	dptab_assertValidTable(table);

	memset(&info, 0, sizeof(info));
	info.hops = hops;
	info.src = src;
	info.arrived = *(dptab->dpio->now);

	if (!dptab || !table) {
		DPRINT(("dptab_set: dptab %p or table %p NULL\n", dptab, table));
		dptab_assertValid(dptab);
		return dp_RES_BUG;
	}
	DPRINT(("dptab_set(table %s, subkey %s, len %d, src h:%x): t:%d\n", 
			key2a(table->key, table->keylen), 
			key2a2(subkey, subkeylen), 
			len, src, *(dptab->dpio->now)));
	if (subkeylen < 1 || subkeylen > hkeytab_MAXLEN) {
		DPRINT(("dptab_set: bad keylen %d\n", subkeylen));
		dptab_assertValid(dptab);
		return dp_RES_BUG;
	}
	if (len > dp_PLAYERDATA_LEN_MAX) {
		DPRINT(("dptab_set: variable too long, %d\n", len));
		dptab_assertValid(dptab);
		return dp_RES_BADSIZE;
	}

	pe = hkeytab_subscript(table->vars,subkey,subkeylen);
	bAlreadyExists = (NULL != pe);
	status = bAlreadyExists ? dp_RES_CHANGED: dp_RES_CREATED;
	DPRINT(("dptab_set: subkey %s, pe %p, bAlreadyExists %d, status %d\n",
			key2a(subkey, subkeylen), pe, bAlreadyExists, status));
	if (!len && bAlreadyExists) {
		/* BUG: can't set existing variable to zero length?? 
		 * This code should make sure old variable was zero length
		 * before ignoring...
		 */
		DPRINT(("dptab_set: identical zero length data; ignoring set\n"));
		bIgnore = TRUE;
	}
	/* Add a key/value pair to it, remember the address of the buffer. */
	if (table->elsize) {
		if (len != table->elsize) {
			DPRINT(("dptab_set: var wrong size: len %d != table->elsize %d\n", len, table->elsize));
			dptab_assertValid(dptab);
			return dp_RES_BUG;
		}
		varbuf = hkeytab_subscript_grow(table->vars, subkey, subkeylen);
	} else {
		dptab_var_t *var = dptab_varCreate(dptab, table, subkey, subkeylen, len );
		if (!var) {
			DPRINT(("dptab_set: can't grow table's vars[%s]!\n", key2a(subkey, subkeylen)));
			dptab_assertValid(dptab);
			return dp_RES_NOMEM;
		}
		varbuf = var->buf;
	}
	if (!varbuf) {
		DPRINT(("dptab_set: can't grow table's vars[%s]!\n", key2a(subkey, subkeylen)));
		dptab_assertValid(dptab);
		return dp_RES_NOMEM;
	}
	if (bAlreadyExists && len && !memcmp(varbuf, buf,len)) {
		DPRINT(("dptab_set: identical data; ignoring set\n"));
		bIgnore = TRUE;
	}
	/* need the following even if bIgnore true to prevent dptab_delete_byAge
	 * from deleting our record */
	memcpy(varbuf, buf, len);
	/* Append our secret little tag record to end of variable */
	memcpy(((char *)varbuf)+len, &info, sizeof(info));
	DPRINT(("dptab_set: tag@%p, subkey %s, hops %d\n",
			((char *)varbuf)+len, key2a(subkey, subkeylen), info.hops));
	if (bIgnore)
		return dp_RES_OK;

	/*DPRINT(("dptab_set: calling callbacks\n"));*/
	for(i = 0; i < dptab_MAX_CALLBACKS; i++) {
		DPRINT(("dptab_set: calling callback %d:%p; status %d\n", i, table->aCb[i], status));
		if (table->aCb[i])
			(table->aCb[i])(dptab, table, src, PLAYER_ME, subkey, subkeylen, 
					buf, len, len, 0, table->aContext[i], status);
	}

	/* Inform other players of the change if there are hops left. */
	if (hops > 0) {
		dp_result_t err;
		int cur_sub;

		/* Set new hop count */
		if (src != PLAYER_ME) 
			hops--;

		/* Loop over the table's subscribers */
		for (cur_sub=0; cur_sub < table->subscribers->n_used; cur_sub++) {
			playerHdl_t dest;
			assoctab_item_t *sub;

			sub = assoctab_getkey(table->subscribers, cur_sub);
			if (!sub)
				break;	/* horrible error */
			dest = (playerHdl_t) sub->key;

			err = dptab_send(dptab, table, subkey, subkeylen, dest, hops);
			if (err != dp_RES_OK) {
				DPRINT(("dptab_set: err:%d in dptab_send for dest h:%x\n", 
						err, dest));
				dptab_assertValid(dptab);
				return err;
			}
		}
	}

	dptab_assertValid(dptab);
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Delete a particular item from a table.
--------------------------------------------------------------------------*/
DP_API dp_result_t
dptab_delete(
	dptab_t *dptab, dptab_table_t *table, char *subkey, int subkeylen)
{
	dp_result_t err;
	int cur_sub;
	int failure;
	int i;
	void *pebuf;
	void *varbuf;
	size_t varlen;
	dptab_varinfo_t tag;

	precondition(dptab != NULL);
	precondition(table != NULL);
	precondition(subkeylen > 0 && subkeylen <= hkeytab_MAXLEN);
	precondition(table->vars != NULL);

	dptab_assertValid(dptab);

	DPRINT(("dptab_delete(dptab, table %s, subkey %s):\n", 
			key2a(table->key, table->keylen), 
			key2a2(subkey, subkeylen)));
	pebuf = hkeytab_subscript(table->vars, subkey, subkeylen);
	if (!pebuf) {
		DPRINT(("dptab_delete: no such entry\n"));
		dptab_assertValid(dptab);
		return dp_RES_EMPTY;
	}

	/* Retrieve variable's size and address */
	if (table->elsize) {
		varlen = table->elsize;
		varbuf = pebuf;
	} else {
		dptab_var_t *var = (dptab_var_t *)pebuf;
		varlen = var->len;
		varbuf = var->buf;
	}

	/* Retrieve secret tag */
	if (!varbuf) {
		DPRINT(("dptab_delete: bug: no varbuf!\n"));
		return dp_RES_BUG;
	}

	memcpy(&tag, ((char *)varbuf) + varlen, sizeof(tag));
	DPRINT(("dptab_delete: src h:%x hops %d\n", tag.src, tag.hops));

	/* Warning: users must be warned that their callbacks must not delete 
	 * tables!
	 */
	DPRINT(("dptab_delete: calling callbacks\n"));
	for(i = 0; i < dptab_MAX_CALLBACKS; i++) {
		if (NULL != table->aCb[i]) {
			char *buf = NULL;
			size_t buflen = 0;
			DPRINT(("dptab_delete: calling callback %d:%p\n", i, table->aCb[i]));
			err = dptab_get_bykey(table, subkey, subkeylen, (void **)&buf, &buflen);
			if (err == dp_RES_OK)
				table->aCb[i](dptab, table, PLAYER_UNKNOWN, PLAYER_ME,
						subkey, subkeylen, buf, 0, buflen, 0,
						table->aContext[i], dp_RES_DELETED);   /* FIXME */
		}
	}

	/*condition(table->vars != NULL, "no vars array after callback!");*/
	if (!table->vars) {
		DPRINT(("dptab_delete: bug: no vars array after callback!\n"));
		dptab_assertValid(dptab);
		return dp_RES_BUG;
	}
	
	if (table->elsize) {
		failure = hkeytab_subscript_delete(table->vars, subkey, subkeylen);
	} else {
		failure = dptab_varDelete(dptab, table, subkey, subkeylen);
	}
	if (failure) {
		DPRINT(("dptab_delete: bug: no such entry after calling callback\n"));
		dptab_assertValid(dptab);
		return dp_RES_BUG;
	}
	/* Inform other players of the change.  
	 *
	 * Note: we probably need to do this before we delete the
	 * item itself - unwary callers may pass a pointer into
	 * the variable to be deleted itself!  (See dptab_delete_bySrc().)
	 * Moving this code might take some thought, though, and only 
	 * dp insiders would try that anyway, we'll just hope nobody does it
	 * for now.
	 */

	if (tag.hops > 0) {
		/* Set new hop count */
		if (tag.src != PLAYER_ME) 
			tag.hops--;

		DPRINT(("dptab_delete: looping over %d destinations\n", table->subscribers->n_used));
		/* Loop over the table's subscribers */
		for (cur_sub=0; cur_sub < table->subscribers->n_used; cur_sub++) {
			playerHdl_t dest;
			assoctab_item_t *sub;

			sub = assoctab_getkey(table->subscribers, cur_sub);
			if (!sub) break;	/* horrible error */
			dest = (playerHdl_t) sub->key;
			DPRINT(("dptab_delete: dest h:%x\n", dest));

			err = dptab_send_delete(dptab, table, subkey, subkeylen, dest, tag.hops);
			if (err != dp_RES_OK) {
				DPRINT(("dptab_delete: err:%d in dptab_send_delete dest h:%x\n", 
						err, dest));
				dptab_assertValid(dptab);
				return err;
			}
		}
	}

	dptab_assertValid(dptab);
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Delete a particular item from a table if the source matches.
--------------------------------------------------------------------------*/
dp_result_t
dptab_deleteIfSrc(
	dptab_t *dptab, dptab_table_t *table, char *subkey, int subkeylen, playerHdl_t src)
{
	dptab_varinfo_t tag;
	dp_result_t err;

	err = dptab_get_bykey2(table, subkey, subkeylen, NULL, NULL, &tag);
	if (err != dp_RES_OK) {
		DPRINT(("dptab_deleteIfSrc: no such variable: table %s subkey %s\n",
			key2a2(table->key, table->keylen),
			key2a(subkey, subkeylen)));
		return err;
	}

	#ifdef VERBOSE
		DPRINT(("dptab_deleteIfSrc: table %s subkey %s oldsrc h:%x delsrc h:%x t:%d\n",
			key2a2(table->key, table->keylen),
			key2a(subkey, subkeylen),
			tag.src, src,
			*(dptab->dpio->now)));
	#endif
	if (src == tag.src)
		err = dptab_delete(dptab, table, subkey, subkeylen);

	return err;
}

/*--------------------------------------------------------------------------
 Delete items with a particular source tag from a table.
 Useful for data that should go away when the host that sent the
 data disconnects.
 Returns dp_RES_OK if any were deleted; dp_RES_EMPTY if no elements matched.
--------------------------------------------------------------------------*/
DP_API dp_result_t
dptab_delete_bySrc(
	dptab_t *dptab, dptab_table_t *table, playerHdl_t src)
{
	void *varbuf;
	size_t varlen;
	int i;
	int deleted = 0;
	char subkey[dptab_KEY_MAXLEN];

	DPRINT(("dptab_delete_bySrc(h:%x):\n", src));

	precondition(dptab != NULL);
	precondition(table != NULL);
	precondition(table->vars != NULL);
	dptab_assertValid(dptab);
	dptab_assertValidTable(table);

	DPRINT(("dptab_delete_bySrc(table %s, h:%x): n_used %d\n", 
			key2a(table->key, table->keylen), src, table->vars->n_used));

	/* For each key/value pair - step downwards since we'll be deleting! */
	for (i=table->vars->n_used-1; i>=0; i--) {
		dptab_varinfo_t tag;
		hkeytab_item_t *pe;

		pe = hkeytab_getkey(table->vars, i);
		if (!pe) {
			DPRINT(("dptab_delete_bySrc: no such element\n"));
			dptab_assertValid(dptab);
			return dp_RES_EMPTY;
		}
		/* Retrieve variable's size and address */
		if (table->elsize) {
			varlen = table->elsize;
			varbuf = &pe->value;
		} else {
			dptab_var_t *var = (dptab_var_t *)&pe->value[0];
			if (!var)
				return dp_RES_BUG;
			varlen = var->len;
			varbuf = var->buf;
		}
		/* Retrieve secret tag */
		memcpy(&tag, ((char *)varbuf) + varlen, sizeof(tag));
		/*DPRINT(("dptab_delete_bySrc: item %d src h:%x hops %d\n",
				i, tag.src, tag.hops));*/
		if (tag.src == src) {
			DPRINT(("dptab_delete_bySrc: src matches; deleting\n"));
			/* Must make copy of key - else it goes away before 
			 * dptab_send_delete is called! 
			 */
			memcpy(subkey, pe->key, pe->keylen);
			dptab_delete(dptab, table, subkey, pe->keylen);
			deleted++;
		}
	}
	if (deleted) {
		dptab_assertValid(dptab);
		return dp_RES_OK;
	}
	dptab_assertValid(dptab);
	return dp_RES_EMPTY;
}

/*--------------------------------------------------------------------------
 Delete items that are older than the given number of seconds.
 Useful for broadcast data that is broadcast unreliably 
 (which can only be done by calling dptab_send with dest=PLAYER_BROADCAST;
  dptab_send_delete should also be used then, but can't be relied upon).

 Returns dp_RES_OK if any were deleted; dp_RES_EMPTY if no elements matched.
--------------------------------------------------------------------------*/
DP_API dp_result_t
dptab_delete_byAge(dptab_t *dptab, dptab_table_t *table, int seconds)
{
	void *varbuf;
	size_t varlen;
	int i;
	int deleted = 0;
	char subkey[dptab_KEY_MAXLEN];

	/*DPRINT(("dptab_delete_byAge(sec %d):\n", seconds));*/

	precondition(dptab != NULL);
	precondition(table != NULL);

	dptab_assertValid(dptab);
	dptab_assertValidTable(table);

	/*DPRINT(("dptab_delete_byAge(sec %d): n_used %d\n", seconds, table->vars->n_used));*/

	/* For each key/value pair - step downwards since we'll be deleting! */
	for (i=table->vars->n_used-1; i>=0; i--) {
		dptab_varinfo_t tag;
		hkeytab_item_t *pe;
		int age;

		pe = hkeytab_getkey(table->vars, i);
		if (!pe) {
			DPRINT(("dptab_delete_byAge: no such element\n"));
			dptab_assertValid(dptab);
			return dp_RES_EMPTY;
		}
		/* Retrieve variable's size and address */
		if (table->elsize) {
			varlen = table->elsize;
			varbuf = &pe->value;
		} else {
			dptab_var_t *var = (dptab_var_t *)&pe->value[0];
			if (!var) {
				dptab_assertValid(dptab);
				return dp_RES_BUG;
			}
			varlen = var->len;
			varbuf = var->buf;
		}
		/* Retrieve secret tag */
		memcpy(&tag, ((char *)varbuf) + varlen, sizeof(tag));
		age = (*(dptab->dpio->now) - tag.arrived) / dptab->dpio->clocksPerSec;
		DPRINT(("dptab_delete_byAge: item %d src h:%x hops %d age %d sec\n",
				i, tag.src, tag.hops, age));
		if (age >= seconds) {
			DPRINT(("dptab_delete_byAge: old enough; deleting\n"));
			/* Must make copy of key - else it goes away before 
			 * dptab_send_delete is called! 
			 */
			memcpy(subkey, pe->key, pe->keylen);
			dptab_delete(dptab, table, subkey, pe->keylen);
			deleted++;
		}
	}
	if (deleted) {
		dptab_assertValid(dptab);
		return dp_RES_OK;
	}
	dptab_assertValid(dptab);
	return dp_RES_EMPTY;
}

/*--------------------------------------------------------------------------
 Given a table pointer and a subkey, get the value of item 'subkey'.
 Caller must have obtained 'table' via a call to dptab_getTable().

 If the player or variable does not exist, dp_RES_EMPTY is returned.

 On success,
 *pbuf is filled with the item's buffer address, and
 *plen is filled with the item's length.
--------------------------------------------------------------------------*/
DP_API dp_result_t
dptab_get_bykey(
	dptab_table_t *table, const char *subkey, int subkeylen, void **pbuf, size_t *plen)
{
	void *varbuf;
	size_t varlen;
	void *p;

	precondition(table != NULL);
	precondition(plen != NULL);
	precondition(pbuf != NULL);

	dptab_assertValidTable(table);

	/* Find the given key/value pair */
	p = hkeytab_subscript(table->vars, subkey, subkeylen);
	if (!p) {
		DPRINT(("dptab_get_bykey: no element %s in table %s\n",
				key2a(subkey, subkeylen),
				key2a2(table->key, table->keylen)));
		return dp_RES_EMPTY;
	}
	/* Retrieve variable's size and address */
	if (table->elsize) {
		varlen = table->elsize;
		varbuf = p;
	} else {
		dptab_var_t *var = (dptab_var_t *)p;
		if (!var || !var->buf)
			return dp_RES_BUG;
		varlen = var->len;
		varbuf = var->buf;
	}
	/* Return the results to the caller. */
	*plen = varlen;
	*pbuf = varbuf;
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Given a table pointer and a subkey, get the value of item 'subkey',
 along with its hop count and source handle.
 Caller must have obtained 'table' via a call to dptab_getTable().

 If the player or variable does not exist, dp_RES_EMPTY is returned.

 On success,
 if pbuf is not NULL, *pbuf is filled with the item's buffer address
 if plen is not NULL, *plen is filled with the item's length
 if ptag is not NULL, *ptag is filled with the item's secret tag
--------------------------------------------------------------------------*/
static dp_result_t
dptab_get_bykey2(
	dptab_table_t *table, const char *subkey, int subkeylen, void **pbuf, size_t *plen, dptab_varinfo_t *ptag)
{
	void *varbuf;
	size_t varlen;
	void *p;

	precondition(table != NULL);

	dptab_assertValidTable(table);

	/* Find the given key/value pair */
	p = hkeytab_subscript(table->vars, subkey, subkeylen);
	if (!p) {
		DPRINT(("dptab_get_bykey: no element %s in table %s\n",
				key2a(subkey, subkeylen),
				key2a2(table->key, table->keylen)));
		return dp_RES_EMPTY;
	}
	/* Retrieve variable's size and address */
	if (table->elsize) {
		varlen = table->elsize;
		varbuf = p;
	} else {
		dptab_var_t *var = (dptab_var_t *)p;
		if (!var || !var->buf)
			return dp_RES_BUG;
		varlen = var->len;
		varbuf = var->buf;
	}
	/* Return the results to the caller. */
	if (plen) *plen = varlen;
	if (pbuf) *pbuf = varbuf;
	if (ptag) memcpy(ptag, ((char *)varbuf) + varlen, sizeof(*ptag));
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Given a table pointer and an index, 
 retrieve the buffer and subkey of the nth item in the given table.
 Caller must have obtained 'table' via a call to dptab_getTable().
 
 On entry, psubkey should point to a single short,
 pbuf should point to a void *, and
 plen should point to a size_t *.

 On exit, *psubkey is filled with the item's id,
 *pbuf is filled with the item's buffer address, and
 *plen is filled with the item's length.

 If n >= the number of elements in the table, returns dp_RES_EMPTY.
--------------------------------------------------------------------------*/
DP_API dp_result_t
dptab_get_byindex(
	dptab_table_t *table, int n, void **pbuf, size_t *plen,
	char *psubkey, int *psubkeylen)
{
	hkeytab_item_t *pe;
	void *varbuf;
	size_t varlen;

	precondition(table != NULL);
	precondition(plen != NULL);
	precondition(pbuf != NULL);
	precondition(psubkey != NULL);

	dptab_assertValidTable(table);

	/* Find the nth key/value pair */
	pe = hkeytab_getkey(table->vars, n);
	if (!pe) {
		DPRINT(("dptab_get_byindex: no such element\n"));
		return dp_RES_EMPTY;
	}
	/* Retrieve variable's size and address */
	if (table->elsize != 0) {
		varlen = table->elsize;
		varbuf = &pe->value;
	} else {
		dptab_var_t *var = (dptab_var_t *)&pe->value[0];
		if (!var || !var->buf) {
			return dp_RES_BUG;
		}
		varlen = var->len;
		varbuf = var->buf;
	}
	/* Return the results to the caller. */
	memcpy(psubkey, pe->key, pe->keylen);
	*psubkeylen = pe->keylen;
	*plen = varlen;
	*pbuf = varbuf;
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Get number of elements in a table.
--------------------------------------------------------------------------*/
DP_API int dptab_tableSize(dptab_table_t *table)
{
	if (table) return table->vars->n_used;
	return 0;	/* just in case */
}

/*********** Explicit network calls ***********/

/*--------------------------------------------------------------------------
 Compare the names of two variables being transferred, taking into account 
 first their table key, then their subkey.
--------------------------------------------------------------------------*/
#define dp_KEY_LOWSESSIONS 256
static int compare_xfer_keys(dptab_xfer_t *xfer1, dptab_xfer_t *xfer2)
{
	int diff;
	int minlen;
	unsigned int k1, k2;

	/* Sort keys as if they were strings, i.e. sort by first byte, then
	* second, etc.  They cannot be zero length, but they may have single-
	* byte keys.
	*/
	/* Swizzle order so sessions have low priority. */
	k1 = ((unsigned char *)xfer1->table->key)[0];
	if (k1 == dp_KEY_SESSIONS) k1 = dp_KEY_LOWSESSIONS;
	k2 = ((unsigned char *)xfer2->table->key)[0];
	if (k2 == dp_KEY_SESSIONS) k2 = dp_KEY_LOWSESSIONS;
	diff = k1 - k2;
	if (!diff) {
		minlen = xfer1->table->keylen - 1;
		if (minlen > xfer2->table->keylen - 1)
		    minlen = xfer2->table->keylen - 1;
		diff = memcmp(xfer1->table->key, xfer2->table->key, minlen);
		if (!diff) {
			diff = xfer1->table->keylen - xfer2->table->keylen;
			if (!diff) {
				/* Subkeys are sorted by length, then by value, 'cause it's easier. */
				diff = xfer1->subkeylen - xfer2->subkeylen;
				if (!diff) {
					diff = memcmp(xfer1->subkey, xfer2->subkey, xfer1->subkeylen);
				}
			}
		}
	}
#ifdef VERBOSE
	DPRINT(("compare_xfer_keys(%s/%s, %s/%s) = %d\n", 
			key2a(xfer1->table->key, xfer1->table->keylen), key2a2(xfer1->subkey, xfer1->subkeylen),
			key2a3(xfer2->table->key, xfer2->table->keylen), key2a4(xfer2->subkey, xfer2->subkeylen),
			diff));
#endif
	return diff;
}

/*--------------------------------------------------------------------------
 Create the next xfer record, and place it into the peer's tx xfer list.
 If a record with the same key already exists, overwrite it and
 use its old xferid.  Otherwise update peer's next xferid.
 Returns pointer to record on success,
 NULL if out of memory.
--------------------------------------------------------------------------*/
static dptab_xfer_t *dptab_xfer_insert(dptab_peer_t *peer, dptab_table_t *table, int hops, char *subkey, int subkeylen, int offset)
{
	dptab_xfer_t *xfer, temp;
	int j;
	int diff = 1;

	/* Create a dptab_xfer_t. */
	memset(&temp, 0, sizeof(temp));
	temp.table = table;
	temp.hops = hops;
	temp.subkeylen = subkeylen;
	temp.cur_offset = offset;
	temp.is_delete = (offset == dptab_OFFSET_DELETE);
	temp.xferid = peer->next_xferid++;
	memcpy(temp.subkey, subkey, sizeof(subkey[0]) * subkeylen);
#if 0
	/* Old behavior */
	xfer = (dptab_xfer_t *)assoctab_subscript_grow(peer->tx, temp.xferid);
	if (!xfer) 
		return NULL;
	*xfer = temp;
	return xfer;
#else
	/* New behavior
	 * The list is kept in {key,subkey} order by the following code.
	 * Duplicate transmissions are detected and deleted.
	 * e.g. If you do two sets in a row on a user record,
	 * the user record should only be sent once.  
	 * The receiving end must detect aborted transfers, and handle properly;
	 * this doesn't happen yet FIXME.
	 */
	for (j = peer->tx->n_used-1; j >= 0; j--) {
		dptab_xfer_t *xfer2;
		assoctab_item_t *pXI = assoctab_getkey(peer->tx, j);
		condition(pXI != NULL, "corrupted tx table");
		xfer2 = (dptab_xfer_t *)(pXI->value);
		/* This compare order causes variables to be sent
		 * LOWEST KEY FIRST.
		 */
		diff = compare_xfer_keys(&temp, xfer2);
		if (diff == 0) {
			DPRINT(("dptab_xfer_insert: duplicate xfer, overwriting!\n"));
			temp.xferid = xfer2->xferid;
			*xfer2 = temp;
			peer->next_xferid--;

			return xfer2;

		} else if (diff < 0) {
			break;
		}
	}
	/* If we get here, the comparison will never be equal,
	 * so let's insert a new record.  
	 * To ensure the truth of the above statement, insert the
	 * new record between the current and the previous record.
	 */
	DPRINT(("dptab_xfer_insert: inserting new record with xferid %d at %d\n", temp.xferid, j+1));
	xfer = (dptab_xfer_t *)assoctab_subscript_insert(peer->tx, temp.xferid, j+1);
	if (!xfer) {
		DPRINT(("dptab_xfer_insert: bug: can't insert new record; j = %d\n", j));
		return NULL;
	}
	*xfer = temp;
	return xfer;
#endif
}

/*--------------------------------------------------------------------------
 Send item 'key' to a particular destination.
 If cb is not NULL, it will be called periodically with the status of the 
 transfer.
 Hops is the number of hops left - if hops is 0, it will not be
 resent by the destination.
--------------------------------------------------------------------------*/
DP_API dp_result_t
dptab_send(
	dptab_t *dptab, dptab_table_t *table, char *subkey, int subkeylen,
	playerHdl_t dest, int hops)
{
	dptab_xfer_t *xfer;
	dptab_peer_t *peer;
	size_t varlen;

	precondition(dptab != NULL);
	precondition(table != NULL);
	#ifdef VERBOSE
		DPRINT(("dptab_send: table %s subkey %s dest h:%x t:%d\n",
			key2a2(table->key, table->keylen), key2a(subkey, subkeylen),
			dest, *(dptab->dpio->now)));
	#endif
	precondition(subkeylen > 0 && subkeylen <= dptab_KEY_MAXLEN);
	dptab_assertValid(dptab);
	dptab_assertValidTable(table);

	/* For debugging, mostly */
	{
		dp_result_t err;
		char *varbuf;
		err = dptab_get_bykey(table, subkey, subkeylen, (void **)&varbuf, &varlen);
		if (err != dp_RES_OK) {
			DPRINT(("dptab_send: no element %s in table %s\n",
					key2a(subkey, subkeylen),
					key2a2(table->key, table->keylen)));
			return err;
		}
	}

	/* Locate the destination's record. */
	peer = (dptab_peer_t *)assoctab_subscript(dptab->peers, dest);
	if (!peer) {
		DPRINT(("dptab_send: handle %x does not have an entry in dptab->peers\n", dest));
		return dp_RES_CLOSED;
	}
	/*DPRINT(("dptab_send: on entry: \n"));
	dumpXfers(peer->tx, "dptab_send tx");*/

#ifdef dp_STATS
	dptab->stats[dptab_STAT_TX_RECORDS].in++;
	dptab->stats[dptab_STAT_TX_BYTES].in += varlen + subkeylen;
#ifdef VERBOSE
	DPRINT(("dptab_send: STAT_TX_RECORDS.in %d STAT_TX_BYTES.in %d, len %d\n", 
		dptab->stats[dptab_STAT_TX_RECORDS].in, 
		dptab->stats[dptab_STAT_TX_BYTES].in, varlen + subkeylen));
#endif
#endif

	/* Add it to the destination's outgoing transfer list. */
	xfer = dptab_xfer_insert(peer, table, hops, subkey, subkeylen, 0);
	if (!xfer) {
		DPRINT(("dptab_send: bug: can't insert new record\n"));
		return dp_RES_NOMEM;
	}

#ifdef VERBOSE
	DPRINT(("dptab_send: table %s subkey %s dest h:%x t:%d xferid %d\n",
		key2a2(table->key, table->keylen), key2a(subkey, subkeylen),
		dest, *(dptab->dpio->now), xfer->xferid));
#endif
	assert(xfer->table);

	/*DPRINT(("dptab_send: peer->tx now: \n"));
	dumpXfers(peer->tx, "dptab_send tx");*/

	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Send deletion of item 'key' to a particular destination.
--------------------------------------------------------------------------*/
DP_API dp_result_t
dptab_send_delete(
	dptab_t *dptab, dptab_table_t *table, char *subkey, int subkeylen,
	playerHdl_t dest, int hops)
{
	dptab_xfer_t *xfer;
	dptab_peer_t *peer;

	precondition(dptab != NULL);
	precondition(table != NULL);
	precondition(subkeylen > 0 && subkeylen <= dptab_KEY_MAXLEN);
	dptab_assertValid(dptab);
	dptab_assertValidTable(table);

	/* Locate the destination's record. */
	peer = (dptab_peer_t *)assoctab_subscript(dptab->peers, dest);
	if (!peer) {
		DPRINT(("dptab_send_delete: handle %x does not have an entry in dptab->peers\n", dest));
		return dp_RES_CLOSED;
	}
	/*dumpXfers(peer->tx, "dptab_send_delete entry");*/

#ifdef dp_STATS
	dptab->stats[dptab_STAT_TX_RECORDS].in++;
	dptab->stats[dptab_STAT_TX_BYTES].in += subkeylen;
#ifdef VERBOSE
	DPRINT(("dptab_send_delete: STAT_TX_RECORDS.in %d STAT_TX_BYTES.in %d, len %d\n", 
		dptab->stats[dptab_STAT_TX_RECORDS].in, 
		dptab->stats[dptab_STAT_TX_BYTES].in, subkeylen));
#endif
#endif

	/* Add it to the destination's outgoing transfer list. */
	xfer = dptab_xfer_insert(peer, table, hops, subkey, subkeylen, dptab_OFFSET_DELETE);
	if (!xfer) {
		DPRINT(("dptab_send_delete: can't insert new record\n"));
		return dp_RES_NOMEM;
	}

#ifdef VERBOSE
	DPRINT(("dptab_send_delete: table %s subkey %s dest h:%x xferid %d\n",
		key2a2(table->key, table->keylen),
		key2a(subkey, subkeylen), dest, xfer->xferid));
#endif

	/*DPRINT(("dptab_send_delete: peer->tx now: \n"));
	dumpXfers(peer->tx, "dptab_send_delete tx");*/

	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Send a single DELETE xfer.
 Called only by dptab_update().
 Deletes xfer upon success.
 Return value is whatever dpio_put_[un]reliable returns.
--------------------------------------------------------------------------*/
static dp_result_t
dptab_update_send_delete(
	dpio_t *dpio, playerHdl_t dest, assoctab_t *peer_tx, dptab_xfer_t *xfer)
{
	char buf[dpio_MAXLEN_UNRELIABLE];
	size_t hdrlen;
	dp_packetType_t *tag = (dp_packetType_t *)buf;
	dptab_delete_packet_t *body = (dptab_delete_packet_t *)(buf + sizeof(dp_packetType_t));
	void *payload = body->key + xfer->subkeylen + xfer->table->keylen;
	dp_result_t err;

	DPRINT(("dptab_update_send_delete: id %d key %s subkey %s\n",
			xfer->xferid, 
			key2a(xfer->table->key, xfer->table->keylen),
			key2a2(xfer->subkey, xfer->subkeylen)));

	*tag = dptab_DELETE_PACKET_ID;
	body->keylen = (unsigned char)xfer->table->keylen;
	memcpy(body->key, xfer->table->key, xfer->table->keylen);
	body->subkeylen = (unsigned char)xfer->subkeylen;
	memcpy(body->key+xfer->table->keylen, xfer->subkey, xfer->subkeylen);
	hdrlen = sizeof(dp_packetType_t) + sizeof_dptab_DELETE_PACKET_HDR + xfer->subkeylen + xfer->table->keylen;
	if (dest == PLAYER_BROADCAST)
		err = dpio_put_unreliable(dpio, &dest, 1, buf, hdrlen, NULL);
	else
		err = dpio_put_reliable(dpio, &dest, 1, buf, hdrlen, NULL);
	DPRINT(("dptab_update_send_delete: key %s subkey %s to h:%x ",
		key2a(xfer->table->key, xfer->table->keylen),
		key2a2(xfer->subkey, xfer->subkeylen), dest));
	if (err == dp_RES_OK) {
		DPRINT(("ok\n"));
		assoctab_subscript_delete(peer_tx, xfer->xferid);
	} else {
		DPRINT(("fails, err:%d\n", err));
	}
	return err;
}

/*--------------------------------------------------------------------------
 dptab_xferBuffer - 
 a place to accumulate small transfers until we have a packet to send.

 To initialize the buffer, call dptab_xferBuffer_init(&xbuf).

 To put stuff in the buffer, drop the bytes into buf[offset++].
 Start off with dptab_SMALL_PACKET_ID, then the fields of 
 "dptab_small_packet_t", then the len, subkey, and data of each transfer.

 To send and clear the buffer, call dptab_xferBuffer_flush().
 If the destination is PLAYER_BROADCAST, the packet is sent unreliably;
 otherwise, it is sent reliably.
 If the send succeeds, dptab_xferBuffer_flush() will also call the 
 table callbacks and clear the xfer's.

 The only thing that puts stuff in the buffer at the moment is 
 dptab_update_send_small().  Both that and dptab_update() flush it.
--------------------------------------------------------------------------*/

typedef struct {
	/* Accumulate packet bytes in here */
	char buf[dpio_MAXLEN_UNRELIABLE + 1];	/* +1 for overflow detection */
	size_t offset;

	/* Accumulate xfer's in here for deletion if xmit goes ok */
	int nxfers;
	dptab_xfer_t xfers[100];

	/* Keep these suckers around for the callback */
	void *varbuf[100];
	size_t varlen[100];

	playerHdl_t dest;
	assoctab_t *peer_tx;
} dptab_xferBuffer_t;

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
static void dptab_xferBuffer_init(dptab_xferBuffer_t *xbuf, playerHdl_t dest, assoctab_t *peer_tx)
{
	/*DPRINT(("dptab_xferBuffer_init:\n"));*/
	xbuf->offset = 0;
	xbuf->nxfers = 0;
	xbuf->buf[dpio_MAXLEN_UNRELIABLE] = 0x49;	/* sentinel */
	xbuf->dest = dest;
	xbuf->peer_tx = peer_tx;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
static void dptab_xferBuffer_flush(dptab_t *dptab, dptab_xferBuffer_t *xbuf)
{
	dp_result_t err;

	dptab_assertValid(dptab);

	if (xbuf->offset == 0)
		return;

	if (xbuf->buf[dpio_MAXLEN_UNRELIABLE] != 0x49) {
		DPRINT(("dptab_xferBuffer_flush: bug: buffer overrun!\n"));
	}
	/*DPRINT(("dptab_xferBuffer_flush: sending %d xfers\n", xbuf->nxfers));*/

	/* Flush the buffer. */
	if (xbuf->dest == PLAYER_BROADCAST)
		err = dpio_put_unreliable(dptab->dpio, &xbuf->dest, 1, xbuf->buf, xbuf->offset, 0);
	else
		err = dpio_put_reliable(dptab->dpio, &xbuf->dest, 1, xbuf->buf, xbuf->offset, 0);

	/* If we were able to send it, mark the xfers as successful. */
	if (err == dp_RES_OK) {
		int i, j;
		long totallen = 0;
		int ierr;
		for (i=0; i<xbuf->nxfers; i++) {
			dptab_xfer_t *xfer = &xbuf->xfers[i];
			/* Inform table callback that the vars have been sent */
#ifdef VERBOSE
			DPRINT(("dptab_xferBuffer_flush: sent table %s subkey %s t:%d; h:%x; xferid %d; is_delete %d\n", 
				key2a(xfer->table->key, xfer->table->keylen),
				key2a2(xfer->subkey, xfer->subkeylen),
				*(dptab->dpio->now), xbuf->dest, xfer->xferid, xfer->is_delete));
#endif
			if (!xfer->is_delete) {
				for(j = 0; j < dptab_MAX_CALLBACKS; j++) {
					if (xfer->table->aCb[j])
						xfer->table->aCb[j](dptab, xfer->table, PLAYER_ME, xbuf->dest,
								xfer->subkey, xfer->subkeylen, xbuf->varbuf[i],
								xbuf->varlen[i], xbuf->varlen[i], 0,
								xfer->table->aContext[j], dp_RES_FINISHED);   /* FIXME */
				}
			}
			totallen += xbuf->varlen[i] + xfer->subkeylen;

			/* Delete the transfer request */
			ierr = assoctab_subscript_delete(xbuf->peer_tx, xfer->xferid);
			assert(ierr == 0);
		}
#ifdef dp_STATS
		dptab->stats[dptab_STAT_TX_RECORDS].out += xbuf->nxfers;
		dptab->stats[dptab_STAT_TX_BYTES].out += totallen;
#endif
	}

	/* Regardless of whether we succeeded, clear the buffer!
	 * If the send failed, we'll rebuild and retry it later.
	 */
	dptab_xferBuffer_init(xbuf, xbuf->dest, xbuf->peer_tx);
}


/*--------------------------------------------------------------------------
 Is the transfer/variable small enough for one packet?
--------------------------------------------------------------------------*/
#define dptab_is_small_packet(xfer,vlen) (((dptab_SMALL_PACKET_HDRLEN + \
	(xfer)->table->keylen + (xfer)->subkeylen + (vlen)) < dpio_MAXLEN_RELIABLE) && (vlen) != dptab_SIZE_DELETE)

/*--------------------------------------------------------------------------
 Send a single small data xfer; xfer->cur_offset must be zero on entry,
 and varlen must be less than or equal to
 dpio_MAXLEN_RELIABLE - sizeof(dptab_initial_packet_t) - keylen - subkeylen.
 
 On entry, *pnpkts is the number of packets we are allowed to queue.
 On exit, *pnpkts is decremented by the number of packets we actually queued.

 Special kludge: if varlen is dptab_SIZE_DELETE, zero bytes are sent,
 and the receiver is supposed to delete the variable.
 Variables really of size dptab_SIZE_DELETE should be sent with 
 dptab_update_send_large.  The macro dptab_is_small_packet ensures this.

 Called only by dptab_update().
 Deletes xfer upon success.
 Return value is whatever dpio_put_[un]reliable returns.
--------------------------------------------------------------------------*/
static dp_result_t
dptab_update_send_small(
	dptab_t *dptab,
	dptab_xferBuffer_t *xbuf,
	dptab_xfer_t *xfer,
	void *varbuf,				/* (input) variable to send, or NULL to force a flush */
	size_t varlen,				/* (input) size of data in varbuf */
	int *pnpkts)
{
	unsigned char *p;
	int is_delete = (varlen == dptab_SIZE_DELETE);
	size_t orig_varlen = varlen;

	/* Special kludge! */
	if (is_delete)
		varlen = 0;

	precondition(xbuf != NULL);
	precondition(xfer != NULL);
	precondition(xfer->table != NULL);
	precondition(dptab_is_small_packet(xfer, varlen));
	dptab_assertValid(dptab);

#ifdef VERBOSE
	DPRINT(("dptab_update_send_small: offset %x alloc %d id %d hops %d key %s subkey %s\n",
			xfer->cur_offset, xfer->allocated, xfer->xferid, xfer->hops,
			key2a(xfer->table->key, xfer->table->keylen),
			key2a2(xfer->subkey, xfer->subkeylen)));
#endif

	if (xbuf->offset > 0) {
		dptab_xfer_t *oxfer = &xbuf->xfers[0];

		/* Check to see if we're being asked to flush (varbuf==NULL),
		 * if the buffer is too full to hold the new transfer,
		 * or if we're incompatible with what was already
		 * in the xfer buffer.  If so, send and reset the buffer.
		 */
		if (!varbuf
		|| (xbuf->offset + 1+xfer->subkeylen+varlen > dpio_MAXLEN_RELIABLE)
		|| (xfer->hops != oxfer->hops)
		|| (xfer->subkeylen != oxfer->subkeylen)
		|| (xfer->table->keylen != oxfer->table->keylen)
		|| (memcmp(xfer->table->key, oxfer->table->key, xfer->table->keylen))){ 

			DPRINT(("dptab_update_send_small: flushing; offset %d >? %d, hops %d != %d, subkeylen %d != %d, keylen %d != %d, key diff %d\n",
				xbuf->offset + 1+xfer->subkeylen+varlen, dpio_MAXLEN_RELIABLE,
				xfer->hops, oxfer->hops,
				xfer->subkeylen, oxfer->subkeylen,
				xfer->table->keylen, oxfer->table->keylen,
				memcmp(xfer->table->key, oxfer->table->key, xfer->table->keylen)));
			dptab_xferBuffer_flush(dptab, xbuf);
			(*pnpkts)--;
		}
	}
	/* condition(xfer->table != NULL, "xfer->table NULL"); */
	if (!xfer->table) {
		DPRINT(("dptab_update_send_small: err: xfer->table NULL\n"));
		return dp_RES_BUG;
	}
	/* If we were just being asked to flush, we're done. */
	if (!varbuf) 
		return dp_RES_OK;

	if (xbuf->offset == 0) {
		/* Fill initial part of packet */
		(*(dp_packetType_t *)xbuf->buf) = dptab_SMALL_PACKET_ID;
		p = ((unsigned char *)xbuf->buf) + sizeof(dp_packetType_t);
		*p++ = (unsigned char)xfer->hops;
		*p++ = (unsigned char)xfer->subkeylen;
		*p++ = (unsigned char)xfer->table->keylen;
		memcpy(p, xfer->table->key, xfer->table->keylen);
		p += xfer->table->keylen;
		xbuf->offset = p - ((unsigned char *)xbuf->buf);
	}

	/* Fill this transfer's part of packet */
	p = (unsigned char *) &xbuf->buf[xbuf->offset];
	*p++ = (unsigned char) orig_varlen;
	memcpy(p, xfer->subkey, xfer->subkeylen);
	p += xfer->subkeylen;
	memcpy(p, varbuf, varlen);
	p += varlen;
	xbuf->offset = p - ((unsigned char *)xbuf->buf);

	/* Save vital stats of this transfer for during flush */
	xbuf->xfers[xbuf->nxfers] = *xfer;
	xbuf->varbuf[xbuf->nxfers] = varbuf;
	xbuf->varlen[xbuf->nxfers] = varlen;
	xbuf->nxfers++;

	/* buffer will be flushed either next call here, or
	 * by dptab_update() when it goes on to the next destination.
	 */
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Send part of a single large data xfer.

 Called only by dptab_update().
 Destination must not be PLAYER_BROADCAST.

 On entry, *pnpkts is the number of packets we are allowed to queue.
 On exit, *pnpkts is decremented by the number of packets we actually queued.

 If transfer finishes successfully during this call, it deletes xfer and 
 calls the xfer's callback.

 Return value is whatever dpio_put_reliable returns.
--------------------------------------------------------------------------*/
static dp_result_t
dptab_update_send_large(
	dptab_t *dptab,
	playerHdl_t dest,
	assoctab_t *peer_tx,
	dptab_xfer_t *xfer,
	void *varbuf,			/* (input) variable to send */
	size_t varlen,			/* (input) size of data in varbuf */
	int *pnpkts)
{
	char buf[dpio_MAXLEN_UNRELIABLE];
	size_t hdrlen;
	size_t len;
	dp_result_t err;
	dp_packetType_t *tag = (dp_packetType_t *)buf;

	precondition(xfer != NULL);
	precondition(xfer->table != NULL);
	precondition(varbuf != NULL);
	precondition(varlen > 0);
	dptab_assertValid(dptab);

	DPRINT(("dptab_update_send_large: offset %x alloc %d id %d key %s subkey %s\n",
			xfer->cur_offset, xfer->allocated, xfer->xferid, 
			key2a(xfer->table->key, xfer->table->keylen),
			key2a2(xfer->subkey, xfer->subkeylen)));

	/*condition(xfer->cur_offset <= dp_PLAYERDATA_LEN_MAX, "bad offset");*/
	if (xfer->cur_offset > dp_PLAYERDATA_LEN_MAX) {
		DPRINT(("dptab_update: bug: offset %x\n", xfer->cur_offset));
		/* delete xfer?  I would if this were to actually happen. */
		return dp_RES_BUG;
	}

	while (*pnpkts > 0) {
		(*pnpkts)--;
		/* Send the current chunk of the current variable. */
		if (xfer->cur_offset == 0) {

			dptab_initial_packet_t *body = (dptab_initial_packet_t *)(buf + sizeof(dp_packetType_t));
			char *key = body->key;
			void *payload = key + xfer->subkeylen + xfer->table->keylen;
			size_t maxlen = dpio_MAXLEN_RELIABLE 
				- (((char *)payload) - (char *)buf);

			/* Send an initial chunk. */
			len = varlen;
			if (len > maxlen)
				len = maxlen;

			*tag = dptab_INITIAL_PACKET_ID;
			body->len = varlen;
			body->crc = dp_crc32(varbuf, varlen);
			body->xferid = (unsigned char)xfer->xferid;
			body->hops = (unsigned char)xfer->hops;
			DPRINT(("dptab_update: crc %x; len %d; hops %d; buf ", body->crc, varlen, body->hops));
			dprint_buf(varbuf, varlen);

			{
				int i;
				unsigned char xferid = body->xferid;

				/* wait if active xfers that conflict with xferid */
				for (i=0; i < peer_tx->n_used; i++) {
					dptab_xfer_t *x;
					assoctab_item_t *item = assoctab_getkey(peer_tx, i);
					condition(item != NULL, "tx table");
					x = (dptab_xfer_t*)item->value;
					if ((x->cur_offset > 0) && (xferid == (0xff & x->xferid))) {
						DPRINT(("dptab_update: xferid %d already busy!\n", xferid));
						return dp_RES_OK;
					}
				}
			}
			body->keylen = (unsigned char)xfer->table->keylen;
			memcpy(body->key, xfer->table->key, xfer->table->keylen);
			body->subkeylen = (unsigned char)xfer->subkeylen;
			memcpy(body->key+xfer->table->keylen, xfer->subkey, xfer->subkeylen);
			//dpSwap_dptab_initial_packet(body);
			memcpy(payload, varbuf, len);

			hdrlen = sizeof(dp_packetType_t) + sizeof_dptab_INITIAL_PACKET_HDR + xfer->subkeylen + xfer->table->keylen;
			DPRINT(("initial: len %d, hdrlen %d\n", len, hdrlen));
		} else {
			char *pastTag = buf + sizeof(dp_packetType_t);
			dptab_body_packet_t *body = (dptab_body_packet_t *)pastTag;
			void *payload = ((char *)body) + sizeof(dptab_body_packet_t);
			len = varlen - xfer->cur_offset;
			DPRINT(("dptab_update: varlen %d, offset %d, len = %d\n",
					varlen, xfer->cur_offset, len));
			if (len > dptab_BODY_MAXLEN)
				len = dptab_BODY_MAXLEN;
#ifndef __MWERKS__
			assert(len>0);
#endif
			*tag = dptab_BODY_PACKET_ID;
			body->xferid = (unsigned char)xfer->xferid;
			//dpSwap_dptab_packet(body);
			memcpy(payload, (char *)(varbuf)+xfer->cur_offset, len);
			hdrlen = sizeof(dp_packetType_t)+sizeof(dptab_body_packet_t);
		}

		err = dpio_put_reliable(dptab->dpio, &dest, 1, buf, len + hdrlen, NULL);
		/*DPRINT(("dptab_update: called dpio_put_reliable, dest h:%x, e:%d, buf: ", dest, err));
		dprint_buf(buf, len+hdrlen);*/

		/*DPRINT(("dptab_update: Sent xferid %d table %s subkey %s off %d len %d hops %d to h:%x; result %d, errHdl h:%x\n",
			xfer->xferid,
			key2a(xfer->table->key, xfer->table->keylen),
			key2a2(xfer->subkey, xfer->subkeylen),
			xfer->cur_offset, len, xfer->hops, dest, err, errHdl
			));*/
		if (err != dp_RES_OK) {
			DPRINT(("dptab_update: serious send problem: dpio_put_reliable returned %d\n", err));
			return err;
		}
		xfer->cur_offset += len;
		if (xfer->cur_offset == varlen) {
			int i;
			/* Now call callback and delete this xfer */
			DPRINT(("dptab_update: large: sent table %s subkey %s t:%d; h:%x; xferid %d\n", 
				key2a(xfer->table->key, xfer->table->keylen),
				key2a2(xfer->subkey, xfer->subkeylen),
				*(dptab->dpio->now), dest, xfer->xferid));
			for(i = 0; i < dptab_MAX_CALLBACKS; i++)
				if (xfer->table->aCb[i])
					xfer->table->aCb[i](dptab, xfer->table, PLAYER_ME, dest,
							xfer->subkey, xfer->subkeylen, varbuf, varlen,
							varlen, 0, xfer->table->aContext[i], dp_RES_FINISHED);   /* FIXME */
			assoctab_subscript_delete(peer_tx, xfer->xferid);

#ifdef dp_STATS
			dptab->stats[dptab_STAT_TX_RECORDS].out++;
			dptab->stats[dptab_STAT_TX_BYTES].out += varlen + xfer->subkeylen;
#ifdef VERBOSE
			DPRINT(("dptab_update_send_large: STAT_TX_RECORDS.out %d STAT_TX_BYTES.out %d, len %d\n", 
				dptab->stats[dptab_STAT_TX_RECORDS].out, 
				dptab->stats[dptab_STAT_TX_BYTES].out, varlen + xfer->subkeylen));
#endif
#endif

			break;		/* done - even if we have packets to spare. */
		}
	}
	return dp_RES_OK;
}


/*--------------------------------------------------------------------------
Attempt to send all transfers for this destination
--------------------------------------------------------------------------*/
static dp_result_t			/* status */
dptab_update_destination(
	dptab_t *dptab,			/* this */
	dptab_peer_t *peer,		/* stuff waiting to go to destination machine */
	const playerHdl_t dest,	/* comm handle of destination machine */
	int* pNpktsfree)		/* current estimate of free packets left */
{
	dptab_xferBuffer_t xbuf;
	dp_result_t err;
	int n_tx;	/* how many are in the tx array */
	int i_tx;	/* how many we've done; counts up from 0 each time */
	int cur_tx;	/* where we are in the tx array; wraps around end */
	short remcap; 
	
	precondition(dptab != NULL);
	precondition(peer != NULL);
	precondition(peer->tx != NULL);
	precondition(pNpktsfree != NULL);
	precondition((*pNpktsfree) > 0);
	dptab_assertValid(dptab);

	remcap = dpio_getHdlRemoteCapabilities(dptab->dpio, dest);
	dumpXfers(peer->tx, "dptab_update_destination entry");

	n_tx = peer->tx->n_used;
#if 0
	/* Loop through all xfers, starting with cur_tx. 
	 * Loop downwards because we delete them as we go.
	 */
	cur_tx = peer->cur_tx--;	/* start next xfer next time for fairness */
	if (cur_tx >= peer->tx->n_used)
		cur_tx = 0;
	if (cur_tx < 0) {
		cur_tx = peer->tx->n_used - 1;
		peer->cur_tx = cur_tx;
	}
#else
	/* Do xfers in strict key/subkey order, to avoid the extra flush
	 * we incur when we wrap around the bottom.  This is a kludge;
	 * wrapping around shouldn't cause a real flush.  It flushes
	 * now to avoid corrupt pointers into the xfer array...
	 * This runs the risk of starving the keys at the beginning
	 * of the array.
	 */
	cur_tx = n_tx - 1;
#endif

	dptab_xferBuffer_init(&xbuf, dest, peer->tx);
	dumpXfers(peer->tx, "dptab_update_destination initialized");
	for (i_tx=0; (i_tx < n_tx) && ((*pNpktsfree) > 0); i_tx++, cur_tx--) {
		assoctab_item_t *xpe;
		dptab_xfer_t *xfer;
		int npkts;

		if (cur_tx >= peer->tx->n_used)
			cur_tx = 0;
		if (cur_tx < 0) {
			/*DPRINT(("dptab_update: cur_tx wrapping to max\n"));*/
			/* Must flush if cur_tx is to decrease? */
			dptab_xferBuffer_flush(dptab, &xbuf);
			cur_tx = peer->tx->n_used - 1;
		}

		/* Send as many packets as possible, but leave
		 * a packet free for the other xfers to this dest, if possible.
		 */
		npkts = *pNpktsfree;	/* Send as many packets as possible, */
		if (n_tx > 0) {
			if (npkts > n_tx)
				npkts -= n_tx;	/* but save one for each xfer left. */
			else
				npkts = 1;		/* or at least don't be greedy. */
		}
#ifdef VERBOSE 
		DPRINT(("dptab_update: cur_tx now %d, npkts=%d, npktsfree=%d\n",
				cur_tx,npkts,*pNpktsfree));
#endif

		/* Get pointer to this xfer. */
		xpe = assoctab_getkey(peer->tx, cur_tx);
		if (!xpe) {
			DPRINT(("dptab_update: bug: can't fetch entry %d from tx; n_used %d\n", cur_tx, peer->tx->n_used));
			break;	/* horrible error */
		}
		xfer = (dptab_xfer_t *)xpe->value;
		if (!xfer->table) {
			DPRINT(("dptab_update: xfer: err: no table?\n"));
			return dp_RES_BUG;
		}
		dptab_assertValidXfer(xfer);

		/* Send as much of this transfer as will fit in npkts packets. */
		if (xfer->cur_offset == dptab_OFFSET_DELETE) {
			if (!(remcap & dpio_REMCAP_NEWTAB)) {
				dptab_update_send_delete(dptab->dpio, dest, peer->tx, xfer);
				(*pNpktsfree)--;
			} else {
				/* send deletes as 0 length sets so we can take advantage of
				 * the nice buffering that send_small does.
				 * This should radically reduce the number of packets sent.
				 */
				dptab_update_send_small(dptab, &xbuf, xfer, "", dptab_SIZE_DELETE, pNpktsfree);
			}
		} else {
			/* Get access to the current variable. */
			size_t varlen = 0;
			char  *varbuf = NULL;
			err = dptab_get_bykey(xfer->table, xfer->subkey, 
									xfer->subkeylen, (void **)&varbuf, &varlen);
			if (err != dp_RES_OK) {
				DPRINT(("dptab_update: no buf for %s; deleting xfer\n",
						key2a(xfer->subkey, xfer->subkeylen)));
				assoctab_subscript_delete(peer->tx, xfer->xferid);
				continue;
			}
			if (dptab_is_small_packet(xfer,varlen)) {
				dptab_update_send_small(dptab, &xbuf, xfer, varbuf, varlen, pNpktsfree);
			} else {
				int old_npkts = npkts;
				dptab_update_send_large(dptab, dest, peer->tx, xfer,
						varbuf, varlen, &npkts);
				(*pNpktsfree) -= (old_npkts - npkts);
			}
		}
	}
	/* Flush the send_small buffer. */
	dptab_xferBuffer_flush(dptab, &xbuf);
	dumpXfers(peer->tx, "dptab_update_destination exit");
	return dp_RES_OK;
}


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

 Implementation notes:

 To reduce the number of packets needed to transfer multiple items,
 this routine will try to package up as many items as will fit into a single
 packet, as long as they all come from the same table.
 The table keylen and key are sent first; then, for each item,
 the item's subkey, length, and data are sent.

	 dptab_SMALL_PACKET_TYPE
	 keylen key subkeylen etc.
	 subkey len data 
	 subkey len data
	 etc.
 
 Items that are too big to fit in a single packet are handled quite 
 differently.  Each time we start sending one of these, we generate a new
 8-bit xferid, and send out the following packet:
	 dptab_INITIAL_PACKET_TYPE
	 xferid datalen crc keylen key 
	 partial_data

 The remaining packets of the transfer contain only the xferid and item data:
	 dptab_BODY_PACKET_TYPE
	 xferid partial_data

 This means we can only have 256 outstanding transfers to the same destination,
 a restriction that must be enforced by dptab_send().

 Overhead:
 Each call to this currently results in a complete sweep through all
 destinations' outstanding transfers.  If no destination has outstanding
 transfers, it takes something like twenty microseconds per destination.  
 If 1000 peers are logged in, that's 20 milliseconds.  This routine should
 be called once/second, so that's 3% CPU load right there!
 Eventually, we will keep a separate list of the destinations with 
 outstanding transfers to reduce this overhead.
--------------------------------------------------------------------------*/
DP_API dp_result_t dptab_update(dptab_t *dptab)
{
	dp_result_t err;
	int cur_dest;
	int n_dest;
	int n_xfers_noticed = 0;		/* Increment if we see any that need work */

	precondition(dptab != NULL);

	dptab_assertValid(dptab);

#if 0	/* Let caller decide how often to do this stuff */
	DPRINT(("dptab_update: now %d, next_send %d, diff %d\n",
		*(dptab->dpio->now), dptab->next_send,
		*(dptab->dpio->now) - dptab->next_send));

	/* Wait 'til previous transmission has had time to get sent. */
	if ((long)(*(dptab->dpio->now) - dptab->next_send) < 0)
		return dp_RES_OK;

	/* Set next-check-time.  Don't have check terribly often,
	 * since each check can queue up a second's worth of data.
	 */
	dptab->next_send = *dptab->dpio->now + dptab->dpio->clocksPerSec;
#endif

	/* Loop through all destinations, starting with cur_dest. */

	n_dest = dptab->peers->n_used;
	cur_dest = dptab->cur_dest++;
	if (cur_dest >= n_dest) {
		cur_dest = 0;
		dptab->cur_dest = 0;
	}
	for (; n_dest>0; n_dest--, cur_dest++) {
		playerHdl_t dest;
		assoctab_item_t *dpe;
		dptab_peer_t *peer;
		int npktsfree;

		if (cur_dest >= dptab->peers->n_used)
			cur_dest = 0;

		dpe = assoctab_getkey(dptab->peers, cur_dest);
		if (!dpe) {
			DPRINT(("dptab_update: bug: can't fetch cur_dest %d; peers->n_used %d\n", cur_dest, dptab->peers->n_used));
			break;	/* horrible error */
		}
		peer = (dptab_peer_t *)dpe->value;

		dumpXfers(peer->tx, "dptab_update peer found");

		/* Don't bother if no xfers outstanding */
		if (!(peer->tx->n_used)) 
			continue;

		n_xfers_noticed++;

		/* Don't bother if output window full (or nearly so) */
		dest = dpe->key;		/* good thing keys are four bytes */
		if (dest == PLAYER_BROADCAST) {
			/* Kludge - allow sending small things to broadcast */
			npktsfree = 1;
		} else {
			npktsfree = dpio_getHdlTxPktsFree(dptab->dpio, dest);
			DPRINT(("dptab_update: npktsfree = %d-3\n", npktsfree));
			npktsfree -= 3;		/* Leave three free for other subsystems */
		}
		if (npktsfree < 1)
			continue;

		dumpXfers(peer->tx, "dptab_update tx1");

		err = dptab_update_destination(dptab, peer, dest, &npktsfree);
		if(err != dp_RES_OK) {
			DPRINT(("dptab_update: dptab_update_destination returned %d\n", err));
			/* continue */
		}

		dumpXfers(peer->tx, "dptab_update tx2");
	}
	if (n_xfers_noticed == 0)
		return dp_RES_EMPTY;

	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Look up what table a given incoming key should be mapped to.
 Dances around a bit to handle the odd case of subscribing to the
 broadcast publisher.

 Returns NULL on failure, or a pointer to the table on success.
--------------------------------------------------------------------------*/
static dptab_table_t *
dptab_mapIncomingTable(
	dptab_t *dptab, dptab_peer_t *peer, playerHdl_t src, char *key, int keylen)
{
	dptab_table_t **tablep;

	dptab_assertValid(dptab);

	/* Make sure we've agreed to accept this table from this
	 * source, and translate the table's key to where it should
	 * end up in our table space.
	 */
	dumpSubscriptionsFromPeer(peer, "dptab_mapIncomingTable:");
	tablep = hkeytab_subscript(peer->pubs, key, keylen);
	if (!tablep) {
		/* kludge: maybe it's a broadcast packet */
		src = PLAYER_BROADCAST;
		peer = assoctab_subscript(dptab->peers, src);
		if (!peer) {
			DPRINT(("dptab_mapIncomingTable: 1: can't find table for key %s on peer h:%x\n",
					key2a(key, keylen), src));
			return NULL;
		}
		tablep = hkeytab_subscript(peer->pubs, key, keylen);
		if (!tablep) {
			DPRINT(("dptab_mapIncomingTable: 2: can't find table for key %s on peer h:%x\n",
					key2a(key, keylen), src));
			return NULL;
		}
	}

#if 0
#ifdef VERBOSE
	DPRINT(("dptab_mapIncomingTable: table %s from peer h:%x maps to table %x=%s\n",
		key2a(key, keylen), src,
		*tablep, key2a2((*tablep)->key, (*tablep)->keylen)));
#endif
#endif

	return *tablep;
}

/*--------------------------------------------------------------------------
 Process a network message generated by this module on another computer.

 Many small variables can be packed into a single packet if they
 share the same table key, subkeylen, and hop count.
 The packet has tag dptab_SMALL_PACKET_ID.

 Large variables will be handled as follows:
 The buffer is broken into pieces.
 The first piece is sent with tag dptab_INITIAL_PACKET_ID
 and has a header structure describing the length, owner, and key of the
 incoming variables.
 Subsequent pieces are sent with tag dptab_BODY_PACKET_ID,
 and lack headers.

 When a first piece comes in, it goes into a holding area for that source;
 When middle or last pieces come in, they are appended to the holding area;
 When a last piece comes in, the holding area is copied to the real variable
 area, and the user code is informed.

 Handles its own packet byte swapping.

 Calls dptab_set() and dptab_delete() according to the contents of the
 packets; this can cause callbacks to the user code.
 Return value is dp_RES_OK normally, dp_RES_EMPTY if packet not recognized,
 other values returned for various internal problems.
--------------------------------------------------------------------------*/
DP_API dp_result_t
dptab_handlePacket(dptab_t *dptab, playerHdl_t src, size_t len, void *buf)
{
	dp_packetType_t *tag = (dp_packetType_t *)buf;
	dptab_var_t *var = NULL;
	dptab_peer_t *peer = NULL;
	dptab_xfer_t *xfer = NULL;
	dptab_table_t *table;
	dp_result_t err;
	void *payload;
	size_t datalen;
	char *key;
	int keylen;
	char *subkey;
	int subkeylen;

#if 0
	DPRINT(("dptab_handlePacket(dptab, src h:%x...) buf: ", src));
	dprint_buf(buf, len);
	DPRINT(("\n"));
#endif

	precondition(dptab != NULL);

	dptab_assertValid(dptab);

	peer = assoctab_subscript(dptab->peers, src);
	if (!peer) {
		/*DPRINT(("dptab_handlePacket: no peer\n"));*/
		/* Kludge: maybe it's the broadcast peer? */
		peer = assoctab_subscript(dptab->peers, PLAYER_BROADCAST);
		if (!peer) {
			DPRINT(("dptab_handlePacket: no peer h:%x, not even broadcast\n",
					src));
			return dp_RES_BAD;
		}
		src = PLAYER_BROADCAST;
	}
	dumpXfers(peer->rx, "dptab_handlePacket rx1");
	switch (*tag) {
	case dptab_INITIAL_PACKET_ID: {
		dptab_initial_packet_t *body = (dptab_initial_packet_t *)((char *)buf + sizeof(dp_packetType_t));

		keylen = body->keylen;
		subkeylen = body->subkeylen;
		key = body->key;
		subkey = body->key + keylen;
		payload = (key + keylen + subkeylen);
		datalen = len - keylen - subkeylen - sizeof(dp_packetType_t) - sizeof_dptab_INITIAL_PACKET_HDR;

		DPRINT(("dptab_handlePacket: "
				"pktlen %d, keylen %d, subkeylen %d, datalen %d, key %s, subkey %s\n", 
				len, keylen, subkeylen, datalen,
				key2a(key, keylen),
				key2a2(subkey, subkeylen)));
		if ((datalen < 1)
		|| (datalen > (dpio_MAXLEN_RELIABLE- keylen- subkeylen - sizeof(dp_packetType_t) - sizeof_dptab_INITIAL_PACKET_HDR))) {
			DPRINT(("dptab_handlePacket: bad datalen %d\n", datalen));
			return dp_RES_BUG;
		}

		table = dptab_mapIncomingTable(dptab, peer, src, key, keylen);

		if (!table) {
			DPRINT(("dptab_handlePacket: no table for key %s on peer h:%x\n",
					key2a(key, keylen), src));
			return dp_RES_EMPTY;
		}

		/* Create a transfer entry for this new transfer. */
		xfer = (dptab_xfer_t *)assoctab_subscript_grow(peer->rx, body->xferid);
		if (!xfer) {
			DPRINT(("dptab_handlePacket: can't alloc xfer\n"));
			return dp_RES_NOMEM;
		}
		xfer->table = table;

		/* Clear the holding area & copy this in. */
		var = &xfer->incoming;
		var->len = SwapBytes4(body->len);
		xfer->hops = body->hops;
		xfer->subkeylen = subkeylen;
		memcpy(xfer->subkey, subkey, subkeylen);
		DPRINT(("dptab_handlePacket: new xfer key %s subkey %s h:%x t:%d\n",
				key2a(xfer->table->key, xfer->table->keylen),
				key2a2(xfer->subkey, xfer->subkeylen),
				/*body->hops,*/
				src,
				*(dptab->dpio->now)));
		xfer->crc = SwapBytes4(body->crc);
		xfer->xferid = body->xferid;	/* note: what if already there? */
		if (xfer->allocated < var->len) {
			void *p = var->buf;
			p = p ? dp_REALLOC(p, var->len) : dp_MALLOC(var->len);
			if (!p) {
				DPRINT(("dptab_handlePacket: can't realloc buf\n"));
				return dp_RES_NOMEM;
			}
			var->buf = p;
			xfer->allocated = var->len;
		}
		memcpy(var->buf, payload, datalen);
		xfer->cur_offset = datalen;
		DPRINT(("dptab_handlePacket: after 1st packet, xfer %p, offset is %d, len is %d\n", xfer, xfer->cur_offset, var->len));
		break;
	}

	case dptab_BODY_PACKET_ID: {
		dptab_body_packet_t *body = (dptab_body_packet_t *)((char *)buf + sizeof(dp_packetType_t));
		void *payload = ((char *)body) + sizeof(dptab_body_packet_t);
		size_t datalen = len - sizeof(dp_packetType_t) - sizeof(dptab_body_packet_t);
		int xferid;

		if (datalen < 1 || datalen > dptab_BODY_MAXLEN) {
			DPRINT(("dptab_handlePacket: datalen\n"));
			return dp_RES_BUG;
		}
		xferid = body->xferid;

		/* Figure out which transfer this is from the id. */
		xfer = assoctab_subscript(peer->rx, xferid);
		if (!xfer) {
			DPRINT(("dptab_handlePacket: can't find xferid %d, aborting\n", xferid));
			return dp_RES_OK;		/* fishy - but not an error */
		}
		DPRINT(("dptab_handlePacket: found xferid %d; xfer is %p\n", xferid, xfer));
		var = &xfer->incoming;
		if (xfer->allocated < xfer->cur_offset + datalen) {
			DPRINT(("dptab_handlePacket: allocated %d need %d + %d\n", xfer->allocated, xfer->cur_offset, datalen));
			return dp_RES_BUG;
		}
		if (xfer->cur_offset > var->len) {
			DPRINT(("dptab_handlePacket: bug: var too long\n"));
			return dp_RES_BUG;
		}


		/* Append to holding area. */
		memcpy((char *)var->buf + xfer->cur_offset, payload, datalen);
		xfer->cur_offset += datalen;
		DPRINT(("dptab_handlePacket: after: xferid %d, offset %d, len %d\n", xfer->xferid, xfer->cur_offset, var->len));
		break;
	}

	case dptab_SMALL_PACKET_ID: {
		unsigned char *p = (unsigned char *)buf + sizeof(dp_packetType_t);
		char *varbuf;
		int varlen;
		int hops;

		/* Read shared packet header */
		hops = *p++ & 255;
		subkeylen = *p++ & 255;
		keylen = *p++ & 255;
		key = (char *) p;
		p += keylen;
		len -= 3 + keylen;

		DPRINT(("dptab_handlePacket: small:\n"));

		table = dptab_mapIncomingTable(dptab, peer, src, key, keylen);
		if (!table) {
			DPRINT(("dptab_handlePacket: no table for key %s on peer h:%x\n",
					key2a(key, keylen), src));
			return dp_RES_OK;	/* Fishy - but not an error */
		}

		/* Read individual variables from packet.  Ooh, this is fast.
		 * Now all we need is a version of dptab_set() that can take
		 * a vector of variables, and we'll really be cookin' with gas.
		 */
		while (len > (unsigned int)1+subkeylen) {
			int is_delete = 0;
			varlen = *p++ & 255;	len--;
			if (varlen == dptab_SIZE_DELETE) {
				is_delete = 1;
				varlen = 0;
			}
			subkey = (char *) p;
			p += subkeylen;			len -= subkeylen;
			varbuf = (char *) p;
			p += varlen;			len -= varlen;

			DPRINT(("dptab_handlePacket: small: len %d, varlen %d\n", len, varlen));

			if (len < 0)
				break;
			/* New scheme for deleting is to send record with "length"
			 * dptab_SIZE_DELETE
			 */
			if (is_delete) {
				/* Ignore delete commands from wrong source. */
				err = dptab_deleteIfSrc(dptab, table, subkey, subkeylen, src);
			} else
				err = dptab_set(dptab, table, subkey, subkeylen, varbuf, varlen, hops, src);
			if (err != dp_RES_OK) {
				DPRINT(("dptab_handlePacket: small: dptab_set returned err:%d\n", err));
			}
#ifdef dp_STATS
			if (is_delete)
				varlen = 0;
			if (err == dp_RES_OK) {
				dptab->stats[dptab_STAT_RX_RECORDS].in++;
				dptab->stats[dptab_STAT_RX_BYTES].in += varlen + subkeylen;
			} else {
				dptab->stats[dptab_STAT_RX_RECORDS].dropped++;
				dptab->stats[dptab_STAT_RX_BYTES].dropped += varlen + subkeylen;
			}
#endif
		}
		return dp_RES_OK;
	}

	case dptab_DELETE_PACKET_ID: {
		dptab_delete_packet_t *body = (dptab_delete_packet_t *)((char *)buf + sizeof(dp_packetType_t));

		keylen = body->keylen;
		subkeylen = body->subkeylen;
		key = body->key;
		subkey = body->key + keylen;

		DPRINT(("dptab_handlePacket: "
				"pktlen %d, keylen %d, subkeylen %d, key %s, subkey %s\n", 
				len, keylen, subkeylen,
				key2a(key, keylen),
				key2a2(subkey, subkeylen)));

		table = dptab_mapIncomingTable(dptab, peer, src, key, keylen);
		if (!table) {
			DPRINT(("dptab_handlePacket: no table for key %s on peer h:%x\n",
					key2a(key, keylen), src));
			return dp_RES_OK;	/* fishy - but not really an error */
		}
		/* Bug: should count stats here */
		DPRINT(("dptab_handlePacket: table %s from peer h:%x maps to table %x=%s\n",
			key2a(key, keylen), src,
			table, key2a2(table->key, table->keylen)));

		/* Delete the given variable in that table. */
		dptab_delete(dptab, table, subkey, subkeylen);
		return dp_RES_OK;
	}

	default:
		DPRINT(("dptab_handlePacket: not a dptab packet\n"));
		return dp_RES_EMPTY;	/* no error - but no dptab packet recognized */
	}

	if (xfer->cur_offset == var->len) {
		/* The variable has arrived! Obey the value change it carries.
		 * Don't echo to other machines!
		 */
		long newcrc = dp_crc32((unsigned char *)var->buf, var->len);
		dptab_table_t *table;
		if (newcrc != xfer->crc) {
			DPRINT(("dptab_handlePacket: bad crc %x, expected %x!\n",
					newcrc, xfer->crc));
			return dp_RES_BAD;
		}

		table = xfer->table;
		if (!table) {
			DPRINT(("dptab_handlePacket: no table for xfer?\n"));
			return dp_RES_BUG;
		}

		err = dptab_set(dptab, table, xfer->subkey, xfer->subkeylen, var->buf, var->len, xfer->hops, src);
		if (err != dp_RES_OK) {
			DPRINT(("dptab_handlePacket: can't set?\n"));
		}

#ifdef dp_STATS
		if (err == dp_RES_OK) {
			dptab->stats[dptab_STAT_RX_RECORDS].in++;
			dptab->stats[dptab_STAT_RX_BYTES].in += var->len + xfer->subkeylen;
		} else {
			dptab->stats[dptab_STAT_RX_RECORDS].dropped++;
			dptab->stats[dptab_STAT_RX_BYTES].dropped += var->len + xfer->subkeylen;
		}
#endif
		/* Done with this transfer. */
		assoctab_subscript_delete(peer->rx, xfer->xferid);
		return dp_RES_OK;
	}
	dumpXfers(peer->rx, "dptab_handlePacket rx2");
	return dp_RES_OK;
}


#define FREEZE_AND_THAW 1
#if FREEZE_AND_THAW

/*-----------------------------------------------------------------------
 Freeze and Thaw Support
-----------------------------------------------------------------------*/

#define dptab_SIG_START "dptab 1"
#define dptab_SIG_END   "dptab 2"
#define dptab_VAR_SIGNATURE		(0xECC0)
#define dptab_TABLE_SIGNATURE	(0xDE1)
#define dptab_PEER_SIGNATURE	(0xFE1F)
#define dptab_XFER_SIGNATURE	(0xFEED)

#define dptab_NULLVALUE		-1

/*-----------------------------------------------------------------------
 Save a reference to a dptab_table_t to disk.
-----------------------------------------------------------------------*/
DP_API dp_result_t			/* status */
dptab_tableRef_freeze(
	dptab_table_t *table,	/* (input) table to save */
	FILE *fp)				/* (modified) stream to save to */
{
	precondition(fp != NULL);

	/* For robustness, it may be useful to save a reference
	   to the table's owner (dptab), so that the thaw method
	   can check that it's restoring correctly. */

	if(table == NULL) {
		/* Save the special NULL value */
		int nulVal = dptab_NULLVALUE;
		DPRINT(("tableR ?? starts at %ld file-bytes\n", ftell(fp)));
		if (fwrite(&nulVal, sizeof(int), 1, fp) != 1)
			return dp_RES_FULL;
		DPRINT(("tableR ?? ends at %ld file-bytes\n", ftell(fp)));
	} else {
		/* Save the table's key */
		DPRINT(("tableR %s starts at %ld file-bytes\n", key2a(table->key, table->keylen), ftell(fp)));
		condition(table->keylen <= dptab_KEY_MAXLEN, "invalid key length");
		if ((fwrite(&(table->keylen), sizeof(int), 1, fp) != 1) ||
			(fwrite(table->key, sizeof(char), table->keylen, fp) !=
						(size_t)table->keylen))
			return dp_RES_FULL;
		DPRINT(("tableR %s ends at %ld file-bytes\n", key2a(table->key, table->keylen), ftell(fp)));
	}

	return dp_RES_OK;
}


/*-----------------------------------------------------------------------
 Restore a reference to a dptab_table_t from disk.
 Requires that the table's state has been restored (dptab_table_thaw()
 has been called for this table).  Assumes that the table is a member
 of dptab's tables list.
-----------------------------------------------------------------------*/
DP_API dp_result_t			/* status */
dptab_tableRef_thaw(
	dptab_table_t **table,	/* (output) table */
	dptab_t *dptab,			/* (input) table collection to find table in */
	FILE *fp)				/* (modified) stream to thaw from */
{
	char tkey[dptab_KEY_MAXLEN];/* tables' key */
	int tkeylen;				/* Length of tables' key */
	long pos;

	precondition(table != NULL);
	precondition(fp != NULL);

	pos = ftell(fp);

	/* Read key and keylengths. */
	if (fread(&tkeylen, sizeof(int), 1, fp) != 1) {
		DPRINT(("dptab_tableRef_thaw: can't read key length.\n"));
		return dp_RES_BAD;
	}

	if(tkeylen == dptab_NULLVALUE) {
		*table = NULL;
		DPRINT(("tableR ?? starts at %ld file-bytes\n", pos));
		DPRINT(("tableR ?? ends at %ld file-bytes\n", ftell(fp)));
	} else {
		condition(tkeylen <= dptab_KEY_MAXLEN, "invalid key length");
		if (fread(tkey, sizeof(char), tkeylen, fp) != (size_t)tkeylen) {
			DPRINT(("dptab_tableRef_thaw: can't read key.\n"));
			return dp_RES_BAD;
		}
		DPRINT(("tableR %s starts at %ld file-bytes\n", key2a(tkey, tkeylen), pos));

		/* Find the corresponding table */
		*table = dptab_getTable(dptab, tkey, tkeylen);
		if(*table == NULL) {
			DPRINT(("dptab_tableRef_thaw: can't find table %s.\n",key2a(tkey, tkeylen)));
			return dp_RES_BAD;
		}
		DPRINT(("tableR %s ends at %ld file-bytes\n", key2a(tkey, tkeylen), ftell(fp)));
	}

	return dp_RES_OK;
}


/*-----------------------------------------------------------------------
 Save the state of a dptab_table_t to disk.
-----------------------------------------------------------------------*/
dp_result_t			/* status */
dptab_table_freeze(
	dptab_table_t *table,	/* (input) table to save */
	dpio_t* dpio,			/* (input) context for player handles */
	FILE *fp)				/* (modified) stream to save to */
{
	int i;
	int sig = dptab_TABLE_SIGNATURE;
	dp_result_t err;

	precondition(table != NULL);
	precondition(dpio != NULL);
	precondition(fp != NULL);

	DPRINT(("dptab_table_freeze: table %s starts at %ld file-bytes\n", key2a(table->key, table->keylen), ftell(fp)));

	/* Header: signature */
	if (fwrite(&sig, sizeof(sig), 1, fp) != 1)
		return dp_RES_FULL;

	DPRINT(("dptab_table_freeze: freezing table %s\n",
			key2a(table->key, table->keylen)));

	/* Table details */
	if ((fwrite(&(table->elsize), sizeof(size_t), 1, fp) != 1) ||
		(fwrite(&(table->keylen), sizeof(int), 1, fp) != 1))
		return dp_RES_FULL;
	if ((fwrite(table->key, sizeof(char), table->keylen, fp) !=
									((size_t)table->keylen)) ||
		(fwrite(&(table->vars->n_used), sizeof(int), 1, fp) != 1) ||
		(fwrite(&(table->subscribers->n_used), sizeof(int), 1, fp) != 1))
		return dp_RES_FULL;

	/* Variables */
	/* Variables are stored in two different ways; either as fixed-size
	   blobs or as variable-sized blobs.  The fixed-size blobs are
	   in the hkeytab's buffer (an array of those blobs).  The variable-
	   sized blobs are stored by placing a length and a pointer to the
	   blob in the hkeytab's buffer (an array of these length-and-pointer
	   pairs), and having that pointer point to a separately allocated
	   block of the correct size. */
	DPRINT(("dptab_table_freeze: table %s: %d variables of len %d\n",
			key2a(table->key, table->keylen), table->vars->n_used, table->elsize));
	/* Save low-level table */
	hkeytab_freeze(table->vars, fp);
	if(table->elsize == 0) {
		/* Variable-size tables must save stuff in pointers, too */
		for(i = 0; i < table->vars->n_used; i++) {
			hkeytab_item_t* item = hkeytab_getkey(table->vars, i);
			dptab_var_t* var = (dptab_var_t*) item->value;
			size_t datalen = var->len + sizeof(dptab_varinfo_t);
			if (fwrite(var->buf, sizeof(char), datalen, fp) != datalen)
				return dp_RES_FULL;
		}
	}
	/* Freeze the handles for the source of each variable so can reset the
	 * table source to newly assigned handles during thaw*/
	for(i = 0; i < table->vars->n_used; i++) {
		dptab_varinfo_t tag;
		void *varbuf;
		size_t varlen;
		hkeytab_item_t *item = hkeytab_getkey(table->vars, i);
		if (table->elsize) {
			varlen = table->elsize;
			varbuf = item->value;
		} else {
			dptab_var_t *var = (dptab_var_t *)item->value;
			varlen = var->len;
			varbuf = var->buf;
		}
		/* Retrieve secret tag */
		memcpy(&tag, ((char *)varbuf) + varlen, sizeof(tag));
		err = dpio_freezeHdl(dpio, tag.src, fp);
		if (err != dp_RES_OK) {
			DPRINT(("dptab_table_freeze: var src h:%x: err %d\n", tag.src, err));
			return err;
		}
	}

	/* Subscribers */
	DPRINT(("dptab_table_freeze: %d subscribers\n", table->subscribers->n_used));
	for (i=0; i < table->subscribers->n_used; i++) {
		assoctab_item_t *sub;
		sub = assoctab_getkey(table->subscribers, i);
		condition(sub != NULL, "subscribers table");
		err = dpio_freezeHdl(dpio, (playerHdl_t) sub->key, fp);
		if (err != dp_RES_OK) {
			DPRINT(("dptab_table_freeze: subscriber %d: err %d\n", i, err));
			return err;
		}
	}

	/* Ignore callbacks and pack/unpack functions; they will be
	   handled by the caller. */

	DPRINT(("dptab_table_freeze: table %s ends at %ld file-bytes\n", key2a(table->key, table->keylen), ftell(fp)));
	return dp_RES_OK;
}


/*-----------------------------------------------------------------------
 Restore the state of a dptab_table_t from disk and add it to the dptab_t.
-----------------------------------------------------------------------*/
dp_result_t		/* status */
dptab_table_thaw(
	dptab_t *dptab,		/* (modified) table collection to add table to */
	dpio_t* dpio,		/* (input) context for player handles */
	FILE *fp)			/* (modified) stream to thaw from */
{
	dptab_table_t *table;
	size_t elsize;
	int keylen;
	char key[dptab_KEY_MAXLEN];
	int vars_used;
	int subs_used;
	int i;
	int sig;
	dp_result_t err;
	long start;

	precondition(dptab != NULL);
	precondition(dpio != NULL);
	precondition(fp != NULL);

	start = ftell(fp);

	/* Header: signature */
	if (fread(&sig, sizeof(sig), 1, fp) != 1) {
		DPRINT(("dptab_table_thaw: can't read header\n"));
		return dp_RES_BAD;	/* Invalid file */
	}
	if (sig != dptab_TABLE_SIGNATURE) {
		DPRINT(("dptab_table_thaw: bad sig\n"));
		return dp_RES_BAD;
	}

	/* Table details */
	if ((fread(&elsize, sizeof(size_t), 1, fp) != 1) ||
			(fread(&keylen, sizeof(int), 1, fp) != 1)) {
		DPRINT(("dptab_table_thaw: can't read details\n"));
		return dp_RES_BAD;	/* Invalid file */
	}
	if ((fread(key, sizeof(char), keylen, fp) != (size_t) keylen) ||
			(fread(&vars_used, sizeof(int), 1, fp) != 1) ||
			(fread(&subs_used, sizeof(int), 1, fp) != 1)) {
		DPRINT(("dptab_table_thaw: can't read key\n"));
		return dp_RES_BAD;	/* Invalid file */
	}
	DPRINT(("dptab_table_thaw: thawing table %s\n", key2a(key, keylen)));
	DPRINT(("table %s starts at %ld file-bytes\n", key2a(key, keylen), start));
	/* Is there a range limit on elsize, keylen, vars_used, or subs_used? */
	/* If so, put in a check here */

	/* Create new table */
	err = dptab_createTable(dptab, &table, key, keylen, elsize,
				NULL, NULL, NULL, NULL);
	if(err != dp_RES_OK) {
		DPRINT(("dptab_table_thaw: couldn't create table\n"));
		return err;
	}

	/* Variables */
	/* Variables are stored in two different ways; either as fixed-size
	   blobs or as variable-sized blobs.  The fixed-size blobs are
	   in the hkeytab's buffer (an array of those blobs).  The variable-
	   sized blobs are stored by placing a length and a pointer to the
	   blob in the hkeytab's buffer (an array of these length-and-pointer
	   pairs), and having that pointer point to a separately allocated
	   block of the correct size. */
	DPRINT(("dptab_table_thaw: table %s, %d variables of size %d\n",
			key2a(key, keylen), vars_used, elsize));
	/* Restore info in low-level table */
	if(hkeytab_thaw(table->vars, fp) == NULL) {
		DPRINT(("dptab_table_thaw: error thawing vars\n"));
		return dp_RES_BAD;
	}
	/* Tables with variable-size elements must restore stuff from pointers */
	if(elsize == 0) {
		for(i = 0; i < table->vars->n_used; i++) {
			hkeytab_item_t* item = hkeytab_getkey(table->vars, i);
			dptab_var_t* var = (dptab_var_t*) item->value;
			size_t datalen;
			/* Must match unusual allocation scheme used in dptab_set(),
			   which calls dptab_varCreate() to do what it needs. */
			var->buf = dp_MALLOC(var->len + sizeof(dptab_varinfo_t));
			if(var->buf == NULL) {
				DPRINT(("dptab_table_thaw: error allocating var %d\n", i));
				return dp_RES_EMPTY;
			}
			datalen = var->len + sizeof(dptab_varinfo_t);
			if(fread(var->buf, sizeof(char), datalen, fp) != datalen) {
				DPRINT(("dptab_table_thaw: error reading var %d\n", i));
				return dp_RES_EMPTY;
			}
		}
	}
	/* Thaw handles for the source of each variable and reset the
	 * table source to newly assigned handle */
	for(i = 0; i < table->vars->n_used; i++) {
		dptab_varinfo_t *tag;
		void *varbuf;
		size_t varlen;
		hkeytab_item_t *item;
		playerHdl_t h;
		err = dpio_thawHdl(dpio, &h, fp);
		if(err != dp_RES_OK) {
			DPRINT(("dptab_table_thaw: couldn't thaw handle %d\n",i));
			return err;
		}
		item = hkeytab_getkey(table->vars, i);
		if (table->elsize) {
			varlen = table->elsize;
			varbuf = item->value;
		} else {
			dptab_var_t *var = (dptab_var_t *)item->value;
			varlen = var->len;
			varbuf = var->buf;
		}
		/* Retrieve secret tag and replace src with new handle */
		tag = (dptab_varinfo_t *)(((char *)varbuf) + varlen);
		tag->src = h;
	}

	/* Subscribers */
	DPRINT(("dptab_table_thaw: table %s, %d subscribers\n",
				key2a(key, keylen), subs_used));
	for (i=0; i < subs_used; i++) {
		playerHdl_t hdl;
		err = dpio_thawHdl(dpio, &hdl, fp);
		if(err != dp_RES_OK) {
			DPRINT(("dptab_table_thaw: couldn't thaw handle %d\n",i));
			return err;
		}
		if(assoctab_subscript_grow(table->subscribers, hdl) == NULL) {
			DPRINT(("dptab_table_thaw: couldn't store handle %d\n",i));
			return dp_RES_NOMEM;
		}
	}

	/* Functions: clear them all; caller will restore explicitly if needed. */
	table->pack = NULL;
	table->unpack = NULL;
	for (i=0; i < dptab_MAX_CALLBACKS; i++) {
		table->aCb[i] = NULL;
		table->aContext[i] = NULL;
	}

	DPRINT(("dptab_table_thaw: table %s ends at %ld file-bytes\n", key2a(key, keylen), ftell(fp)));
	dptab_assertValidTable(table);
	return dp_RES_OK;
}


/*-----------------------------------------------------------------------
 Save the state of a dptab_xfer_t to disk.
-----------------------------------------------------------------------*/
static dp_result_t		/* status */
dptab_xfer_freeze(
	dptab_xfer_t *xfer,	/* (input) xfer to save */
	FILE *fp)			/* (modified) stream to save to */
{
	int sig = dptab_XFER_SIGNATURE;
	dp_result_t err;

	precondition(xfer != NULL);
	precondition(fp != NULL);

	DPRINT(("xfer %d starts at %ld file-bytes\n", xfer->xferid, ftell(fp)));

	/* Header: signature */
	if (fwrite(&sig, sizeof(sig), 1, fp) != 1)
		return dp_RES_FULL;

	DPRINT(("dptab_xfer_freeze: freezing xfer %d.\n", xfer->xferid));

	/* Details */
	if ((fwrite(&(xfer->xferid), sizeof(int), 1, fp) != 1) ||
		(fwrite(&(xfer->cur_offset), sizeof(size_t), 1, fp) != 1) ||
		(fwrite(&(xfer->crc), sizeof(long), 1, fp) != 1) ||
		(fwrite(&(xfer->subkeylen), sizeof(int), 1, fp) != 1) ||
		(fwrite(xfer->subkey, sizeof(char), xfer->subkeylen, fp) != 
									(size_t)xfer->subkeylen) ||
		(fwrite(&(xfer->hops), sizeof(int), 1, fp) != 1))
		return dp_RES_FULL;
	err = dptab_tableRef_freeze(xfer->table, fp);
	if(err != dp_RES_OK)
		return err;

	/* Variable */
	if (fwrite(&(xfer->incoming.len), sizeof(size_t), 1, fp) != 1)
		return dp_RES_FULL;
	if ((xfer->incoming.len != 0) &&
		(fwrite(xfer->incoming.buf, sizeof(char),
				xfer->incoming.len, fp) != xfer->incoming.len))
		return dp_RES_FULL;

	DPRINT(("xfer %d ends at %ld file-bytes\n", xfer->xferid, ftell(fp)));
	return dp_RES_OK;
}


/*-----------------------------------------------------------------------
 Restore the state of a dptab_xfer_t from disk.
 Requires that dptab->tables has already been restored.
-----------------------------------------------------------------------*/
static dp_result_t			/* status */
dptab_xfer_thaw(
	assoctab_t* tab,		/* (modified) list to thaw into */
	dptab_t *dptab,			/* (input) table collection this xfer is part of */
	FILE *fp)				/* (modified) stream to thaw from */
{
	dptab_xfer_t dummy;
	dptab_xfer_t *temp = &dummy;
	dptab_xfer_t *xfer;
	dp_result_t err;
	int sig;
	long pos;

	precondition(tab != NULL);
	precondition(dptab != NULL);
	precondition(fp != NULL);

	pos = ftell(fp);
	memset(temp, 0, sizeof(dptab_xfer_t));

	/* Header: signature */
	if (fread(&sig, sizeof(sig), 1, fp) != 1) {
		DPRINT(("dptab_xfer_thaw: can't read header\n"));
		return dp_RES_BAD;	/* Invalid file */
	}
	if (sig != dptab_XFER_SIGNATURE) {
		DPRINT(("dptab_xfer_thaw: bad sig\n"));
		return dp_RES_BAD;
	}

	/* Create new xfer */
	if (fread(&(temp->xferid), sizeof(int), 1, fp) != 1) {
		DPRINT(("dptab_xfer_thaw: error reading id\n"));
		return dp_RES_EMPTY;
	}
	DPRINT(("xfer %d starts at %ld file-bytes\n", temp->xferid, pos));

	/* Remaining Details */
	if ((fread(&(temp->cur_offset), sizeof(size_t), 1, fp) != 1) ||
		(fread(&(temp->crc), sizeof(long), 1, fp) != 1) ||
		(fread(&(temp->subkeylen), sizeof(int), 1, fp) != 1) ||
		(fread(temp->subkey, sizeof(char), temp->subkeylen, fp) !=
									(size_t)(temp->subkeylen)) ||
		(fread(&(temp->hops), sizeof(int), 1, fp) != 1)) {
		DPRINT(("dptab_xfer_thaw: error reading details\n"));
		return dp_RES_EMPTY;
	}
	DPRINT(("dptab_xfer_thaw: thawing xfer %d.\n", temp->xferid));
	err = dptab_tableRef_thaw(&(temp->table), dptab, fp);
	if(err != dp_RES_OK) {
		DPRINT(("dptab_xfer_thaw: error reading table\n"));
		return err;
	}

	/* Variable */
	if (fread(&(temp->incoming.len), sizeof(size_t), 1, fp) != 1)
		return dp_RES_EMPTY;
	DPRINT(("dptab_xfer_thaw: xfer %d  table %s  varlen %d.\n",
		temp->xferid, key2a(temp->table->key, temp->table->keylen), temp->incoming.len));
	temp->allocated = temp->incoming.len + 1;		/* kludge: avoid zero length */
	temp->incoming.buf = dp_MALLOC(temp->allocated);
	if (!temp->incoming.buf) {
		DPRINT(("dptab_xfer_thaw: can't allocate %d bytes\n", temp->allocated));
		return dp_RES_NOMEM;
	}
	if ((temp->incoming.len != 0) && (fread(temp->incoming.buf, sizeof(char),
				temp->incoming.len, fp) != temp->incoming.len))
		return dp_RES_EMPTY;

	/* Create new xfer */
	xfer = (dptab_xfer_t *)assoctab_subscript_grow(tab, temp->xferid);
	if (xfer == NULL) {
		DPRINT(("dptab_xfer_thaw: could not allocate xfer\n"));
		return dp_RES_NOMEM;
	}
	*xfer = *temp;	/* Copy contents */

	DPRINT(("xfer %d ends at %ld file-bytes\n", temp->xferid, ftell(fp)));
	return dp_RES_OK;
}


/*-----------------------------------------------------------------------
 Save the state of a dptab_peer_t to disk.
-----------------------------------------------------------------------*/
static dp_result_t		/* status */
dptab_peer_freeze(
	dptab_peer_t *peer,	/* (input) peer to save */
	playerHdl_t hdl,	/* (input) player handle we're filed under. */
	dpio_t* dpio,		/* (input) context for player handles */
	FILE *fp)			/* (modified) stream to save to */
{
	int sig = dptab_PEER_SIGNATURE;
	int i;
	dp_result_t err;

	precondition(peer != NULL);
	precondition(peer->tx != NULL);
	precondition(peer->rx != NULL);
	precondition(peer->pubs != NULL);
	precondition(dpio != NULL);
	precondition(fp != NULL);

	DPRINT(("peer h:%x starts at %ld file-bytes\n", hdl, ftell(fp)));

	/* Header: signature */
	if (fwrite(&sig, sizeof(sig), 1, fp) != 1)
		return dp_RES_FULL;
	err = dpio_freezeHdl(dpio, hdl, fp);
	if(err != dp_RES_OK)
		return err;

	DPRINT(("dptab_peer_freeze: freezing peer h:%x.\n", hdl));

	/* Details */
	if ((fwrite(&(peer->cur_tx), sizeof(int), 1, fp) != 1) ||
		(fwrite(&(peer->next_xferid), sizeof(int), 1, fp) != 1) ||
		(fwrite(&(peer->tx->n_used), sizeof(int), 1, fp) != 1) ||
		(fwrite(&(peer->rx->n_used), sizeof(int), 1, fp) != 1) ||
		(fwrite(&(peer->pubs->n_used), sizeof(int), 1, fp) != 1))
		return dp_RES_FULL;
	DPRINT(("dptab_peer_freeze: %d tx, %d rx, %d pubs.\n",
			peer->tx->n_used, peer->rx->n_used, peer->pubs->n_used));

	/* Tables being published */
	for (i=0; i < peer->pubs->n_used; i++) {
		hkeytab_item_t *tab = hkeytab_getkey(peer->pubs, i);
		condition(tab != NULL, "pubs table");
		/* Save key info from pubs table. */
		if ((fwrite(&(tab->keylen), sizeof(char), 1, fp) != 1) ||
			(fwrite(tab->key, sizeof(char), tab->keylen, fp) != (size_t)tab->keylen))
			return dp_RES_FULL;
		/* Save value as a reference to table. */
		err = dptab_tableRef_freeze(*((dptab_table_t **) tab->value), fp);
		if(err != dp_RES_OK)
			return err;
	}

	/* Outstanding transfers */
	dumpXfers(peer->tx, "dptab_peer_freeze tx");
	for (i=0; i < peer->tx->n_used; i++) {
		assoctab_item_t *item = assoctab_getkey(peer->tx, i);
		condition(item != NULL, "tx table");
		/* Key info stored inside xfer, as xfer->xferid */
		/* Save value */
		dptab_xfer_freeze((dptab_xfer_t*)item->value, fp);
	}
	dumpXfers(peer->rx, "dptab_peer_freeze rx");
	for (i=0; i < peer->rx->n_used; i++) {
		assoctab_item_t *item = assoctab_getkey(peer->rx, i);
		condition(item != NULL, "rx table");
		/* Key info stored inside xfer, as xfer->xferid */
		/* Save value */
		dptab_xfer_freeze((dptab_xfer_t*)item->value, fp);
	}

	DPRINT(("peer h:%x ends at %ld file-bytes\n", hdl, ftell(fp)));
	return dp_RES_OK;
}


/*-----------------------------------------------------------------------
 Restore the state of a dptab_peer_t from disk and add it to the dptab_t.
 Requires that dptab->tables has already been restored.
-----------------------------------------------------------------------*/
static dp_result_t		/* status */
dptab_peer_thaw(
	dptab_t *dptab,		/* (modified) table collection to add peer to */
	dpio_t* dpio,		/* (input) context for player handles */
	FILE *fp)			/* (modified) stream to thaw from */
{
	dptab_peer_t dummy;
	dptab_peer_t * temp = &dummy;
	dptab_peer_t * peer;
	playerHdl_t hdl;
	int sig;
	int i;
	int tx_used;
	int rx_used;
	int pubs_used;
	dp_result_t err;
	long pos;

	precondition(dptab != NULL);
	precondition(dpio != NULL);
	precondition(fp != NULL);

	/* Initialize the temporary structure */
	pos = ftell(fp);
	memset(temp, 0, sizeof(dptab_peer_t));
	temp->cur_tx = 0;
	temp->next_xferid = 0;
	temp->tx = hkeytab_create(sizeof(dptab_xfer_t));
	temp->rx = hkeytab_create(sizeof(dptab_xfer_t));
	temp->pubs = hkeytab_create(sizeof(dptab_table_t *));
	if ((temp->tx == NULL) || (temp->rx == NULL) || (temp->pubs == NULL)) {
		DPRINT(("dptab_peer_thaw: can't create tables.\n"));
		return dp_RES_NOMEM;
	}

	/* Header: signature and handle */
	if (fread(&sig, sizeof(sig), 1, fp) != 1) {
		DPRINT(("dptab_peer_thaw: can't read header\n"));
		return dp_RES_EMPTY;	/* Invalid file */
	}
	if (sig != dptab_PEER_SIGNATURE) {
		DPRINT(("dptab_peer_thaw: bad sig\n"));
		return dp_RES_BAD;
	}
	err = dpio_thawHdl(dpio, &hdl, fp);
	if (err != dp_RES_OK) {
		DPRINT(("dptab_peer_thaw: bad hdl\n"));
		return err;
	}
	DPRINT(("peer h:%x starts at %ld file-bytes\n", hdl, pos));
	DPRINT(("dptab_peer_thaw: thawing peer h:%x\n", hdl));

	/* Details */
	if ((fread(&(temp->cur_tx), sizeof(int), 1, fp) != 1) ||
		(fread(&(temp->next_xferid), sizeof(int), 1, fp) != 1) ||
		(fread(&tx_used, sizeof(int), 1, fp) != 1) ||
		(fread(&rx_used, sizeof(int), 1, fp) != 1) ||
		(fread(&pubs_used, sizeof(int), 1, fp) != 1)) {
		DPRINT(("dptab_peer_thaw: can't read details.\n"));
		return dp_RES_EMPTY;
	}
	DPRINT(("dptab_peer_thaw: %d tx, %d rx, %d pubs.\n",tx_used, rx_used, pubs_used));

	/* Tables being published */
	for (i=0; i < pubs_used; i++) {
		dptab_table_t **pt;			/* Table in peer's pubs */
		dptab_table_t *tt;			/* Table in dptab's tables */
		char pkey[hkeytab_MAXLEN];	/* pubs' key */
		char pkeylen;				/* Length of pubs' key */
		/* Read key and keylengths. */
		if ((fread(&pkeylen, sizeof(char), 1, fp) != 1) ||
			(fread(pkey, sizeof(char), pkeylen, fp) != (size_t)pkeylen)) {
			DPRINT(("dptab_peer_thaw: can't read pub %d.\n",i));
			return dp_RES_BAD;
		}
		/* Find the table we're referencing */
		err = dptab_tableRef_thaw(&tt, dptab, fp);
		if(err != dp_RES_OK) {
			DPRINT(("dptab_peer_thaw: can't restore table ref.\n"));
			return err;
		}
		/* Save a pointer to that table in pubs */
		pt = hkeytab_subscript_grow(temp->pubs, pkey, pkeylen);
		if (pt == NULL) {
			DPRINT(("dptab_peer_thaw: can't save table in pubs.\n"));
			return dp_RES_NOMEM;
		}
		*pt = tt;
	}

	/* Outstanding transfers */
	for (i=0; i < tx_used; i++) {
		err = dptab_xfer_thaw(temp->tx, dptab, fp);
		if(err != dp_RES_OK) {
			DPRINT(("dptab_peer_thaw: can't thaw tx xfer %d.\n",i));
			return err;
		}
	}
	for (i=0; i < rx_used; i++) {
		err = dptab_xfer_thaw(temp->rx, dptab, fp);
		if(err != dp_RES_OK) {
			DPRINT(("dptab_peer_thaw: can't thaw rx xfer %d.\n",i));
			return err;
		}
	}

	/* Create the peer */
	peer = (dptab_peer_t *)assoctab_subscript_grow(dptab->peers, hdl);
	if (peer == NULL) {
		DPRINT(("dptab_peer_thaw: couldn't add\n"));
		dptab_assertValid(dptab);
		return dp_RES_NOMEM;
	}
	*peer = *temp;

	dumpXfers(peer->rx, "dptab_peer_thaw rx");
	dumpXfers(peer->tx, "dptab_peer_thaw tx");

	DPRINT(("peer h:%x ends at %ld file-bytes\n", hdl, ftell(fp)));
	return dp_RES_OK;
}


/*-----------------------------------------------------------------------
 Save the state of a dptab_t to disk.
-----------------------------------------------------------------------*/
DP_API dp_result_t		/* status */
dptab_freeze(
	dptab_t *dptab,		/* (input) table collection to save */
	FILE *fp)			/* (modified) stream to save to */
{
	int i;
	dp_result_t err;

	precondition(dptab != NULL);
	precondition(dptab->dpio != NULL);
	precondition(dptab->tables != NULL);
	precondition(dptab->peers != NULL);
	precondition(fp != NULL);

	DPRINT(("dptab starts at %ld file-bytes\n", ftell(fp)));

	DPRINT(("dptab_freeze:\n"));

	/* Header & Details*/
	if ((fwrite(dptab_SIG_START, sizeof(dptab_SIG_START), 1, fp) != 1) ||
		(fwrite(&(dptab->tables->n_used), sizeof(int), 1, fp) != 1) ||
		(fwrite(&(dptab->peers->n_used), sizeof(int), 1, fp) != 1)) {
		DPRINT(("dptab_freeze: Header full.\n"));
		return dp_RES_FULL;
	}

	/* Tables */
	DPRINT(("dptab_freeze: %d tables\n", dptab->tables->n_used));
	for (i=0; i < dptab->tables->n_used; i++) {
		hkeytab_item_t *tab;
		tab = hkeytab_getkey(dptab->tables, i);
		condition(tab != NULL, "tables table");
		err = dptab_table_freeze(*((dptab_table_t **)tab->value),
							dptab->dpio, fp);
		if (err != dp_RES_OK)
			return err;
	}

	/* Peers */
	DPRINT(("dptab_freeze: %d peers\n", dptab->peers->n_used));
	for (i=0; i < dptab->peers->n_used; i++) {
		assoctab_item_t *pEl;
		pEl = assoctab_getkey(dptab->peers, i);
		DPRINT(("dptab_freeze: peer %d.\n", i));
		condition(pEl != NULL, "peers table");
		err = dptab_peer_freeze((dptab_peer_t *)pEl->value, pEl->key, dptab->dpio, fp);
		if (err != dp_RES_OK)
			return err;
	}

#ifdef dp_STATS
	fwrite(&dptab->stats, sizeof(dptab->stats), 1, fp);
#endif

	/* Trailer */
	fwrite(dptab_SIG_END, sizeof(dptab_SIG_END), 1, fp);

	DPRINT(("dptab ends at %ld file-bytes\n", ftell(fp)));
	return dp_RES_OK;
}


/*-----------------------------------------------------------------------
 Restore the state of a dptab_t from disk.
 Call after creating a virgin dptab_t; assumes that the dpio has already
 been restored.
-----------------------------------------------------------------------*/
DP_API dp_result_t		/* status */
dptab_thaw(
	dptab_t *dptab,		/* (output) table collection to restore */
	FILE *fp)			/* (modified) stream to restore from */
{
	char buf[sizeof(dptab_SIG_END)];
	int i;
	int tables_used;
	int peers_used;
	dp_result_t err;

	precondition(dptab != NULL);
	precondition(dptab->dpio != NULL);
	precondition(dptab->tables != NULL);
	precondition(dptab->peers != NULL);
	precondition(fp != NULL);

	DPRINT(("dptab starts at %ld file-bytes\n", ftell(fp)));

	/* Header & Details */
	if ((fread(buf, sizeof(dptab_SIG_START), 1, fp) != 1) ||
		(fread(&tables_used, sizeof(int), 1, fp) != 1) ||
		(fread(&peers_used, sizeof(int), 1, fp) != 1))
		return dp_RES_EMPTY;
	if (0 != memcmp(buf, dptab_SIG_START, sizeof(dptab_SIG_START))) {
		DPRINT(("dptab_thaw: no start signature\n"));
		return dp_RES_BAD;
	}

	/* Tables */
	DPRINT(("dptab_thaw: %d tables\n", tables_used));
	for (i=0; i < tables_used; i++) {
		err = dptab_table_thaw(dptab, dptab->dpio, fp);
		if(err != dp_RES_OK) {
			DPRINT(("dptab_thaw: couldn't thaw table %d\n",i));
			return err;
		}
	}

	/* Peers */
	DPRINT(("dptab_thaw: %d peers\n", peers_used));
	for (i=0; i < peers_used; i++) {
		err = dptab_peer_thaw(dptab, dptab->dpio, fp);
		if(err != dp_RES_OK) {
			DPRINT(("dptab_thaw: couldn't thaw peer %d\n",i));
			return err;
		}
	}

#ifdef dp_STATS
	if  (fread(&dptab->stats, sizeof(dptab->stats), 1, fp) != 1) {
		DPRINT(("dptab_thaw: no stats table\n"));
		return dp_RES_BAD;
	}
#endif

	/* Trailer */
	if ((fread(buf, sizeof(dptab_SIG_END), 1, fp) != 1)
	||  memcmp(buf, dptab_SIG_END, sizeof(dptab_SIG_END))) {
		DPRINT(("dptab_thaw: no end signature\n"));
		return dp_RES_BAD;
	}

	DPRINT(("dptab ends at %ld file-bytes\n", ftell(fp)));
	return dp_RES_OK;
}

#endif /* FREEZE_AND_THAW */


#ifdef dptab_UNITTEST

/*-----------------------------------------------------------------------
 Unit Test
-----------------------------------------------------------------------*/

#include <stdio.h>
#include <assert.h>
#include "eclock.h"

int nvars_rx;
int nvars_tx;
int nvars_deleted;

/*-----------------------------------------------------------------------
-----------------------------------------------------------------------*/
int dp_PASCAL table_cb(dptab_t *dptab, dptab_table_t *table, playerHdl_t src, playerHdl_t dest, char *subkey, int subkeylen, void *buf, size_t sent, size_t total, int seconds_left, void *context, dp_result_t err)
{
	DPRINT(("table_cb: status:%d\n", err));
	if (err == dp_RES_CREATED) {
		DPRINT(("table_cb: got: src h:%x, dest h:%x, key %s, subkey %s, sent %d, total %d, nvars_tx %d\n", 
			src, dest, 
			key2a(table->key, table->keylen),
			key2a2(subkey, subkeylen),
			sent, total, nvars_tx));
		if ((dest == PLAYER_ME) && (sent == total)) {
			nvars_rx++;
			printf("Got variable from h:%x; table %s, subkey %s; len %d\n", 
				src, 
				key2a(table->key, table->keylen),
				key2a2(subkey, subkeylen),
				total);
		}
	} else if (err == dp_RES_DELETED) {
		DPRINT(("table_cb: delete: src h:%x, dest h:%x, key %s, subkey %s\n", 
			src, dest, 
			key2a(table->key, table->keylen),
			key2a2(subkey, subkeylen)));
		if (dest == PLAYER_ME)
			nvars_deleted++;		/* deleted by someone else */
		printf("Got delete variable from h:%x; table %s, subkey %s\n", 
			src, 
			key2a(table->key, table->keylen),
			key2a2(subkey, subkeylen));
	} else if (err == dp_RES_FINISHED) {
		DPRINT(("table_cb: sent: src h:%x, dest h:%x, key %s, subkey %s, sent %d, total %d, nvars_tx %d\n", 
			src, dest, 
			key2a(table->key, table->keylen),
			key2a2(subkey, subkeylen),
			sent, total, nvars_tx));
		if ((src == PLAYER_ME) && (sent == total)) {
			nvars_tx++;
			DPRINT(("table_cb: nvars_tx incremented to %d\n", nvars_tx));
			printf("Sent variable to h:%x; table %s, subkey %s; len %d\n", 
				dest, 
				key2a(table->key, table->keylen),
				key2a2(subkey, subkeylen),
				total);
		}
	}
	return TRUE;
}

/*-----------------------------------------------------------------------
-----------------------------------------------------------------------*/
void table_test()
{
	dptab_t *tab;
	dp_result_t err;
	char key[10];
	dpio_t dummy_dpio;
	dptab_table_t *table;
	clock_t dpio_now;

	memset(&dummy_dpio, 0, sizeof(dummy_dpio));
	dummy_dpio.now = &dpio_now;

	tab = dptab_create(&dummy_dpio);
	assert(tab);
	key[0] = 1;
	key[1] = 2;
	err = dptab_createTable(tab, &table, key, 2, sizeof(int), NULL, NULL, table_cb, NULL);
	assert(err == dp_RES_OK);

#if 1
	/* Clean up. */
	err = dptab_deleteTable(tab, key, 2);
	assert(err == dp_RES_OK);
	/* Clean up. */
	err = dptab_deleteTable(tab, key, 2);
	assert(err == dp_RES_EMPTY);
#endif

	dptab_destroy(tab);

	printf("No table test failed.\n");
}

#define BIGDATALEN 800

/*-------------------------------------------------------------------------
 Build tables 1 and 2.
 Table 1 is a table of integers with two elements: 1.2 = 12, 1.8 = 18.
 Table 2 is a table of blobs with one element: 2.1 = (20000 element ramp)
 Make sure those elements are accessible, and nonexistent elements aren't.
 Tables are left built for use in later tests.
-------------------------------------------------------------------------*/
void build_tables(dptab_t *tab, dptab_table_t **ptable1, dptab_table_t **ptable2)
{
	dp_result_t err;
	char key[10];
	char badkey[10];
	char subkey[10];
	int dummy;
	int *pdummy;
	size_t len;
	size_t subkeylen;
	char bigdata[BIGDATALEN];
	char *pbigdata;
	int i;
	dptab_table_t *table1;
	dptab_table_t *table2;

	/* Create one fixed size table and one variable sized table. */
	key[0] = 1;
	err = dptab_createTable(tab, &table1, key, 1, sizeof(int), NULL, NULL, table_cb, NULL);
	assert(err == dp_RES_OK);

	key[0] = 2;
	err = dptab_createTable(tab, &table2, key, 1, 0, NULL, NULL, table_cb, NULL);
	assert(err == dp_RES_OK);

	/* Put two elements in the fixed table: 1.2 = 12, and 1.8 = 18. */
	dummy = 12;
	key[0] = 2;
	err = dptab_set(tab, table1, key, 1,  &dummy, sizeof(dummy), 0, PLAYER_ME);
	assert(err == dp_RES_OK);

	dummy = 18;
	key[0] = 8;
	err = dptab_set(tab, table1, key, 1,  &dummy, sizeof(dummy), 0, PLAYER_ME);
	assert(err == dp_RES_OK);

	/* Put an element in the variable table: 2.1 = bigdata */
	for (i=0; i<BIGDATALEN; i++)
		bigdata[i] = i & 255;
	key[0] = 1;
	err = dptab_set(tab, table2, key, 1,  bigdata, BIGDATALEN, 0, PLAYER_ME);
	assert(err == dp_RES_OK);

	/* Make sure they can be retrieved. */
	pdummy = NULL;
	key[0] = 2;
	len = sizeof(dummy);
	err = dptab_get_bykey(table1, key, 1, &pdummy, &len);
	assert(err == dp_RES_OK);
	assert(pdummy);
	assert(*pdummy == 12);
	if (len != sizeof(dummy)) {
		printf("Got len %d instead of %d!\n", len, sizeof(dummy));
		assert(len == sizeof(dummy));
	}

	pdummy = NULL;
	key[0] = 8;
	len = sizeof(dummy);
	err = dptab_get_bykey(table1, key, 1, &pdummy, &len);
	assert(err == dp_RES_OK);
	assert(pdummy);
	assert(*pdummy == 18);

	memset(bigdata, 0, BIGDATALEN);
	key[0] = 1;
	len = BIGDATALEN;
	err = dptab_get_bykey(table2, key, 1, &pbigdata, &len);
	assert(err == dp_RES_OK);
	assert(pbigdata);
	if (len != BIGDATALEN) {
		printf("Got len %d instead of %d!\n", len, BIGDATALEN);
		assert(len == BIGDATALEN);
	}
	for (i=0; i<BIGDATALEN; i++) {
		if ((pbigdata[i]&255) != (i & 255))
			printf("err: pbigdata[%d/%x] = %d/%x\n", i, i, pbigdata[i], pbigdata[i]);
		assert((pbigdata[i]&255) == (i & 255));
	}

	/* Make sure we can enumerate them. */
	pdummy = NULL; len = 0; subkeylen = 0;
	memset(subkey, 0, sizeof(subkey));

	err = dptab_get_byindex(table1, 0, &pdummy, &len, subkey, &subkeylen);
	if (err != dp_RES_OK)
		printf("dptab_get_byindex returned err:%d\n", err);
	assert(err == dp_RES_OK);
	assert(*pdummy == 12);
	assert(subkeylen == 1);
	assert(subkey[0] == 2);

	pdummy = NULL; len = 0; subkeylen = 0;
	memset(subkey, 0, sizeof(subkey));

	err = dptab_get_byindex(table1, 1, &pdummy, &len, subkey, &subkeylen);
	if (err != dp_RES_OK)
		printf("dptab_get_byindex returned err:%d\n", err);
	assert(err == dp_RES_OK);
	assert(*pdummy == 18);
	assert(subkeylen == 1);
	assert(subkey[0] == 8);

	pdummy = NULL; len = 0; subkeylen = 0;
	memset(subkey, 0, sizeof(subkey));

	err = dptab_get_byindex(table1, 2, &pdummy, &len, subkey, &subkeylen);
	if (err != dp_RES_EMPTY)
		printf("dptab_get_byindex returned err:%d\n", err);
	assert(err == dp_RES_EMPTY);

	/* Make sure other values can't be retrieved! */
	badkey[0] = 5;
	err = dptab_get_bykey(table1, badkey, 1, &pdummy, &len);
	assert(err == dp_RES_EMPTY);

	len = sizeof(dummy);
	badkey[0] = 2;
	err = dptab_get_bykey(table2, badkey, 1, &pdummy, &len);
	if (err != dp_RES_EMPTY)
		printf("bug: got err %d\n", err);
	assert(err == dp_RES_EMPTY);

	if (ptable1) *ptable1 = table1;
	if (ptable2) *ptable2 = table2;
}

/*-----------------------------------------------------------------------
-----------------------------------------------------------------------*/
void local_test()
{
	dptab_t *tab;
	dp_result_t err;
	char key[10];
	dpio_t dummy_dpio;
	dptab_table_t *table1, *table2;
	clock_t dpio_now;

	memset(&dummy_dpio, 0, sizeof(dummy_dpio));
	dummy_dpio.now = &dpio_now;

	tab = dptab_create(&dummy_dpio);
	assert(tab);

	build_tables(tab, &table1, &table2);

	/* Make sure tables are what we expect. */
	key[0] = 1;
	assert((table1->keylen == 1) && !memcmp(key, table1->key, 1));
	key[0] = 2;
	assert((table2->keylen == 1) && !memcmp(key, table2->key, 1));

	/* Clean up. */
	key[0] = 1;
	err = dptab_deleteTable(tab, key, 1);
	assert(err == dp_RES_OK);

	key[0] = 1;
	err = dptab_deleteTable(tab, key, 1);
	assert(err == dp_RES_EMPTY);

	key[0] = 2;
	/* Verify that old table pointer still valid! */
	assert((table2->keylen == 1) && !memcmp(key, table2->key, 1));
	err = dptab_deleteTable(tab, key, 1);
	assert(err == dp_RES_OK);

	dptab_destroy(tab);

	printf("No local test failed.\n");
}

#if 1

/*--------------------------------------------------------------------------
 Loopback networking test.  Run two of these things together.
 Hostnum should be either 1 or 2
--------------------------------------------------------------------------*/
void loopback_test(int hostnum)
{
	char key[10];
	clock_t dpio_now;
	clock_t started;
	commInitReq_t commInitReq;
	commScanAddrReq_t		scanReq;
	commScanAddrResp_t		scanResp;
	dpio_t *dpio;
	dptab_t *tab;
	dptab_table_t *peerTable1, *peerTable2;
	dptab_table_t *table1, *table2;
	dp_result_t dptab_status;
	dp_result_t err;
	dp_transport_t dll;
	int i;

	int peernum;
	char peeradr_printable[128];
	unsigned char peeradr[dp_MAX_ADR_LEN];
	playerHdl_t dest;

	if (hostnum == 1) peernum = 2;
	else if (hostnum == 2) peernum = 1;
	else {
		printf("usage: dptab.exe [1|2]\n");
		exit(1);
	}

	memset(&commInitReq, 0, sizeof(commInitReq));
	commInitReq.reqLen = sizeof(commInitReq_t);
	commInitReq.sessionId = hostnum;	/* my own address */

	dpio_now = eclock();
	memset(&dll, 0, sizeof(dll));
	strcpy(dll.fname, "wloop2d.dll");
	err = dpio_create(&dpio, &dll, &commInitReq, &dpio_now, NULL);
	assert(err == dp_RES_OK);

	sprintf(peeradr_printable, "%d.0.0.0", peernum);	/* partner's address */
	scanReq.printable = peeradr_printable;
	scanReq.address = peeradr;
	scanReq.size = sizeof(peeradr);
	if (!commScanAddr(&scanReq, &scanResp)) {
		printf("Unable to scan player address %s, err: %d", scanReq.printable, scanResp.status);
		assert(FALSE);
	}
	/*  Open a comm handle to the partner's address */
	dest = dpio_openHdl(dpio, peeradr, NULL, NULL);
	if (dest == PLAYER_NONE) {
		printf("Unable to connect to address %s", scanReq.printable);
		assert(FALSE);
	}
	printf("Opening address %s returns h:%x\n", scanReq.printable, dest);

	tab = dptab_create(dpio);
	assert(tab);

	build_tables(tab, &table1, &table2);

	/* Add the second machine to the party */
	err = dptab_addPeer(tab, dest);
	assert(err == dp_RES_OK);

	/* Create fixed table #9.1 and route peer's table #1 to it. */
	key[0] = 9;
	key[1] = 1;
	err = dptab_createTable(tab, &peerTable1, key, 2, sizeof(int), NULL, NULL, table_cb, NULL);
	assert(err == dp_RES_OK);
	key[0] = 1;
	err = dptab_addPublisher(tab, peerTable1, key, 1, dest);
	assert(err == dp_RES_OK);

	/* Create variable table #9.2 and route peer's table #2 to it. */
	key[0] = 9;
	key[1] = 2;
	err = dptab_createTable(tab, &peerTable2, key, 2, 0, NULL, NULL, table_cb, NULL);
	assert(err == dp_RES_OK);
	key[0] = 2;
	err = dptab_addPublisher(tab, peerTable2, key, 1, dest);
	assert(err == dp_RES_OK);

	/* Make sure we can send one. */
	key[0] = 2;
	err = dptab_send(tab, table1, key, 1, dest, 1);
	assert(err == dp_RES_OK);
	/* Make sure we can send a second one from the same table. */
	key[0] = 8;
	err = dptab_send(tab, table1, key, 1, dest, 1);
	assert(err == dp_RES_OK);
	/* Make sure we can overload that second one with no ill effects.
	 * This should NOT generate a second transfer, or at least, it should
	 * abort the previous one.
	 */
	key[0] = 8;
	err = dptab_send(tab, table1, key, 1, dest, 1);
	assert(err == dp_RES_OK);

	/* Make sure we can send a big one. */
	key[0] = 1;
	err = dptab_send(tab, table2, key, 1, dest, 1);
	assert(err == dp_RES_OK);

	started = eclock();
	nvars_rx = 0;
	nvars_tx = 0;
	nvars_deleted = 0;

	dptab_status = dp_RES_OK;

	/* Call the poll routine for a while, see if we get anything. */
	for (i=0; (nvars_rx < 3) || (nvars_tx < 3) || (nvars_deleted < 1) || (dptab_status != dp_RES_EMPTY) || (dp_RES_BUSY == dpio_ReadyToFreeze(dpio, NULL)); i++) {
		playerHdl_t src;
		char pkt[512];
		size_t size;
		dpio_now = eclock();

		/* Returns EMPTY when nothing waiting to go out */
		dptab_status = dptab_update(tab);
		assert(dptab_status == dp_RES_OK || dptab_status == dp_RES_EMPTY);

		if (nvars_tx == 3) {
			static int done = 0;
			if (!done) {
				done = 1;

				/* Make sure we can delete one remotely, too! */
				key[0] = 1;
				err = dptab_send_delete(tab, table2, key, 1, dest, 1);
				assert(err == dp_RES_OK);
			}
		}

		err = dpio_update(dpio);
		if (err != dp_RES_OK && err != dp_RES_EMPTY) {
			printf("dpio_update reports error %d\n", err);
			assert(FALSE);
		}
		src = (dpid_t) PLAYER_NONE;	/* kludge */
		size = sizeof(pkt);
		err = dpio_get(dpio, &src, &pkt, &size, NULL);
		if (err != dp_RES_OK && err != dp_RES_EMPTY && err != dp_RES_AGAIN) {
			printf("dpio_get reports error %d\n", err);
			assert(FALSE);
		}
		if (err == dp_RES_OK) {
			printf("Got packet from handle %x\n", src);
			err = dptab_handlePacket(tab, src, size, pkt);
			if (err != dp_RES_OK && err != dp_RES_EMPTY) {
				printf("dptab_handlePacket reports error %d\n", err);
				fflush(stdout);
				assert(FALSE);
			}
		}
		DPRINT(("rx %d tx %d del %d stat %d ready %d\n", nvars_rx, nvars_tx, nvars_deleted, dptab_status, dpio_ReadyToFreeze(dpio, NULL)));
		printf("rx %d tx %d del %d stat %d ready %d\n", nvars_rx, nvars_tx, nvars_deleted, dptab_status, dpio_ReadyToFreeze(dpio, NULL));
	}
	printf("Hmm, it took %d ticks for %d polls, or %d microsec each.\n", 
			eclock() - started,
			i,
			((eclock() - started) * (1000000/i) / ECLOCKS_PER_SEC));

	/* Make sure that we can delete the items created by the other host
	 * by referring to it by handle.
	 */
	assert(peerTable1->vars->n_used != 0);
	err = dptab_delete_bySrc(tab, peerTable1, dest);
	assert(err == dp_RES_OK);
	assert(peerTable1->vars->n_used == 0);

	/* Make sure delete_bySrc doesn't delete anything if there were
	 * no variables from that source.
	 */
	err = dptab_delete_bySrc(tab, table1, dest);
	assert(err == dp_RES_EMPTY);

	dpio_destroy(dpio, 0);

	/* Clean up. */
	key[0] = 1;
	err = dptab_deleteTable(tab, key, 1);
	assert(err == dp_RES_OK);

	key[0] = 1;
	err = dptab_deleteTable(tab, key, 1);
	assert(err == dp_RES_EMPTY);

	key[0] = 2;
	/* Verify that old table pointer still valid! */
	assert((table2->keylen == 1) && !memcmp(key, table2->key, 1));
	err = dptab_deleteTable(tab, key, 1);
	assert(err == dp_RES_OK);

	key[0] = 9;
	key[1] = 1;
	err = dptab_deleteTable(tab, key, 2);
	assert(err == dp_RES_OK);

	key[0] = 9;
	key[1] = 2;
	err = dptab_deleteTable(tab, key, 2);
	assert(err == dp_RES_OK);

	printf("No loopback test failed.\n");
}

/*-----------------------------------------------------------------------
 Broadcast loopback networking test.  Run two
 of these things together.
 Demonstrates how unreliable broadcast can be used with the
 dptab system.
-----------------------------------------------------------------------*/
void broadcast_test(void)
{
	char key[10];
	clock_t dpio_now;
	clock_t last_sent;
	clock_t started;
	dpio_t *dpio;
	dptab_t *tab;
	dptab_table_t *peerTable1;
	dptab_table_t *table1, *table2;
	dp_result_t dptab_status;
	dp_result_t err;
	dp_transport_t dll;
	int i;

	dpio_now = eclock();
	memset(&dll, 0, sizeof(dll));
	strcpy(dll.fname, "wloop2d.dll");
	err = dpio_create(&dpio, &dll, NULL, &dpio_now, NULL);
	assert(err == dp_RES_OK);

	tab = dptab_create(dpio);
	assert(tab);

	build_tables(tab, &table1, &table2);

	/* Add the broadcast address(!) to the party */
	err = dptab_addPeer(tab, PLAYER_BROADCAST);
	assert(err == dp_RES_OK);

	/* Create fixed table #9.1 and route PLAYER_BROADCAST's table #1 to it. */
	key[0] = 9;
	key[1] = 1;
	err = dptab_createTable(tab, &peerTable1, key, 2, sizeof(int), NULL, NULL, table_cb, NULL);
	assert(err == dp_RES_OK);
	key[0] = 1;
	err = dptab_addPublisher(tab, peerTable1, key, 1, PLAYER_BROADCAST);
	assert(err == dp_RES_OK);

	last_sent = 0;
	nvars_rx = 0;
	nvars_tx = 0;
	nvars_deleted = 0;
	dptab_status = dp_RES_OK;
	i = 0;

	/* Loop for ten seconds, broadcasting the value of our small variable. */
	dpio_now = eclock();
	for (started=dpio_now; (long)(dpio_now-started)<10*ECLOCKS_PER_SEC; ) {
		playerHdl_t src;
		char pkt[512];
		size_t size;
		dpio_now = eclock();

		if ((long)(dpio_now - last_sent) > ECLOCKS_PER_SEC) {
			last_sent = dpio_now;

			/* Broadcast the two variables (unreliably) every second.
			 * Only send one of them per second, else we'll overrun the
			 * transmit system.
			 */
			if (i++ & 1) {
				key[0] = 2;
				err = dptab_send(tab, table1, key, 1, PLAYER_BROADCAST, 1);
				assert(err == dp_RES_OK);
			} else {
				key[0] = 8;
				err = dptab_send(tab, table1, key, 1, PLAYER_BROADCAST, 1);
				assert(err == dp_RES_OK);
			}

			/* Fill output buffers once a second.
			 * Note: Brodcast only sends one variable per update.
			 */
			dptab_status = dptab_update(tab);
			assert(dptab_status == dp_RES_OK || dptab_status == dp_RES_EMPTY);
		}

		err = dpio_update(dpio);
		if (err != dp_RES_OK && err != dp_RES_EMPTY) {
			printf("dpio_update reports error %d\n", err);
			assert(FALSE);
		}
		src = (dpid_t) PLAYER_NONE;	/* kludge */
		size = sizeof(pkt);
		err = dpio_get(dpio, &src, &pkt, &size, NULL);
		if (err != dp_RES_OK && err != dp_RES_EMPTY && err != dp_RES_AGAIN) {
			printf("dpio_get reports error %d\n", err);
			assert(FALSE);
		}
		if (err == dp_RES_OK) {
			printf("Got packet from handle %x\n", src);
			err = dptab_handlePacket(tab, src, size, pkt);
			if (err != dp_RES_OK && err != dp_RES_EMPTY) {
				printf("dptab_handlePacket reports error %d\n", err);
				fflush(stdout);
				assert(FALSE);
			}
		}
	}

	printf("Calling dptab_send_delete on first variable.  Let other one time out.\n");
	key[0] = 2;
	err = dptab_send_delete(tab, table1, key, 1, PLAYER_BROADCAST, 1);
	assert(err == dp_RES_OK);

	/* Loop for ten seconds and see if the other side's delete gets here */
	for (started=dpio_now; (long)(dpio_now-started)<10*ECLOCKS_PER_SEC; ) {
		playerHdl_t src;
		char pkt[512];
		size_t size;
		dpio_now = eclock();

		if ((long)(dpio_now - last_sent) > ECLOCKS_PER_SEC) {
			last_sent = dpio_now;

			/* Fill output buffers once a second */
			dptab_status = dptab_update(tab);
			assert(dptab_status == dp_RES_OK || dptab_status == dp_RES_EMPTY);

			/* Flush variables at least 4 seconds old */
			err = dptab_delete_byAge(tab, peerTable1, 4);
		}

		err = dpio_update(dpio);
		if (err != dp_RES_OK && err != dp_RES_EMPTY) {
			printf("dpio_update reports error %d\n", err);
			assert(FALSE);
		}
		src = (dpid_t) PLAYER_NONE;	/* kludge */
		size = sizeof(pkt);
		err = dpio_get(dpio, &src, &pkt, &size, NULL);
		if (err != dp_RES_OK && err != dp_RES_EMPTY && err != dp_RES_AGAIN) {
			printf("dpio_get reports error %d\n", err);
			assert(FALSE);
		}
		if (err == dp_RES_OK) {
			printf("Got packet from handle %x\n", src);
			err = dptab_handlePacket(tab, src, size, pkt);
			if (err != dp_RES_OK && err != dp_RES_EMPTY) {
				printf("dptab_handlePacket reports error %d\n", err);
				fflush(stdout);
				assert(FALSE);
			}
		}
	}
	printf("nvars_deleted = %d; should be 2\n", nvars_deleted);

	dptab_destroy(tab);
	dpio_destroy(dpio, 0);

	printf("No broadcast test failed.\n");
}
#endif

/*-----------------------------------------------------------------------
-----------------------------------------------------------------------*/
main(int argc, char **argv)
{
	int i;
	int childNum;
	char fname[128];

	/* Can't really print out a good usage message, since
	 * usually only one of the tests isn't commented out...
	 * this test program is not really ready to be a regression test.
	 */
	if (argc < 2) {
		printf("Usage: %s childnum ...\n", argv[0]);
		printf("Creates dp<childnum>.log\n");
		exit(1);
	}
	childNum = atoi(argv[1]);
	sprintf(fname, "dp%d.log", childNum);
	dp_setLogFname(fname);

	for (i=0; i<1; i++) {
		//table_test();
		local_test();
		loopback_test(childNum);
		//broadcast_test();
	}
	printf("No test failed.\n");

	return 0;
}
#endif
