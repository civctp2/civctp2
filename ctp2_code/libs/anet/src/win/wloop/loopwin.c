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

/*****************************************************************************

                           Loopback Protocol Driver Functions For Win32

                              (c) 1996 By Activision

  Revision Log:
  $Log: loopwin.c $
  Revision 1.6  1997/08/14 19:28:08  dkegel
  Better DPRINT's, but disable them.
  Revision 1.5  1997/07/10 03:38:33  dkegel
  Big change: allow opening a handle to an address that doesn't
  exist at moment, and let sessionId *be* the address.
  When sessionId is given, addresses are no longer allocated
  starting at 1.0.0.0!
  Revision 1.4  1997/07/08 21:52:02  dkegel
  Use sessionId passed in commInitReq, if present, to
  regain same network address when thawing.
  Revision 1.3  1997/05/23 23:50:48  mrobin
  Added no broadcast versions of the DLLs
  Revision 1.2  1997/05/23 01:19:27  mrobin
  Added 100 msec sleep on no packets received
  Revision 1.1  1997/05/21 20:04:25  mrobin
  Initial revision
******************************************************************************/

/*****************************************************************************

                                    Literals

******************************************************************************/

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "dpmem.h"

#include "loopwin.h"

//#define DEBUG_MODULE 1
#include "ddprint.h"

//*****************************************************************************
// Constants

#define	LOOP_BUF_SIZE			256
#define	NUM_DIRENTRIES			512

#define	MEMMAP_NAME				"asdfLOOPSHAREDMEM"
#define	MUTEX_NAME				"asdfLOOPMUTEX"

// Packet buffer flags

#define	PBUF_CLOSED				0x00000001			// Process has closed its instance

//*****************************************************************************
// Typedefs

// Directory format

typedef struct {
	LOOPADDR	next_addr;
	LOOPADDR	entries[NUM_DIRENTRIES];
} LOOP_DIRECTORY;

// Packet buffer definitions

typedef struct {
	LOOPADDR	from;
	LOOPADDR	to;
	int			flags;
	int			len;
	char		data[LOOP_MAX_USERDATALEN];
} LOOP_PACKET;

typedef struct {
	DWORD		flags;
	DWORD		front;
	DWORD		back;
	LOOP_PACKET	packets[LOOP_BUF_SIZE];
} LOOP_PACKBUFFER;

//*****************************************************************************
// Static declarations

static LOOPADDR		open_loopback(LOOPPEER *mypeer, long sessionId);
static void			close_loopback(LOOPADDR myaddr);
static int			loopsend(LOOPADDR myaddr, char *buf, int len, int flags, LOOPPEER *to);
static int			loopbroadcast(LOOPADDR myaddr, char *buf, int len, int flags);
static int			looprecv(LOOPADDR myaddr, char *buf, int len, int flags, LOOPADDR *from);
static BOOL			address_to_peer(LOOPADDR addr, LOOPPEER *peer);

//*****************************************************************************
// File scope globals

static HANDLE			hDirFileMap;
static HANDLE			hDirMutex;
static LOOP_DIRECTORY	*dir;

static HANDLE			hPbufFileMap;
static HANDLE			hPbufMutex;
static LOOP_PACKBUFFER	*pbuffer;

static char		error_text[80];

/*****************************************************************************

                                 getsockerror()

******************************************************************************/

static void getlooperror(void)

{
  DPRINT(("Loopback error: %s\n", error_text));
}

/*****************************************************************************

                                  printaddr()

******************************************************************************/

void printaddr(LOOPADDR *peer, char *string)

{
  dp_dprintf("%s: %X\n", string, *peer);
}

/*****************************************************************************

                                  LOOPWIN_Create()

******************************************************************************/

// Create an loopback driver.
// Returns NULL on failure.
// Only one of these may exist at a time, since multiple ones would
// compete for the same source of rx packets.

LOOPINSTANCE * LOOPWIN_Create(int *status, long sessionId)

{
  LOOPINSTANCE * loopptr;
//  int           len;

  DPRINT(("LOOPWIN_Create() Called..\n"));

  // allocate an instance structure
  loopptr = (LOOPINSTANCE *) dp_MALLOC(sizeof(LOOPINSTANCE));
  if (!loopptr)  {*status = 100;  return NULL;}

  // init the instance structure
  memset(loopptr, 0, sizeof(LOOPINSTANCE));
  loopptr->nexthandle = loop_HDL_ME + 1;

  // open an loopback socket
  loopptr->addr = open_loopback(&loopptr->peers[loop_HDL_ME], sessionId);
  if (loopptr->addr == INVALID_LOOPADDR) {
	  dp_FREE(loopptr);  *status = 130; getlooperror(); return(NULL);
  }

  // setup the broadcast peer
  memset(&loopptr->peers[loop_HDL_BROADCAST], 0, sizeof(LOOPPEER));
  loopptr->peers[loop_HDL_BROADCAST].addr = BROADCAST_LOOPADDR;

  return(loopptr);
}

/*****************************************************************************

                                  LOOPWIN_Destroy()

******************************************************************************/

// Destroy a loopback driver.

void LOOPWIN_Destroy(LOOPINSTANCE *loopptr)

{
  DPRINT(("LOOPWIN_Destroy() Called..\n"));

  close_loopback(loopptr->addr);

  dp_FREE(loopptr);
}

/*****************************************************************************

                            LOOPWIN_Address2Handle()

******************************************************************************/

// Given an loopback peer address, return a handle that can be used to send an
// address to that address.  Handles are integers starting at 0.
// If no handle exists for that address,
//   If insert is non-zero,
//    allocate one.
//   else
//    return loop_HDL_NONE
// Multiple calls with the same arg will return the same result.
// There is no upper bound on handles.
// (There is no way provided to close handles, and no particular need,
// since nobody will be creating more than 100 or so, and they only take up
// 16 bytes each.)
// Returns loop_HDL_NONE on failure.

LOOPHANDLE LOOPWIN_Address2Handle(LOOPINSTANCE *loop, LOOPADDR addr, int insert)

{
  LOOPHANDLE	h;

  //printaddr(addr, "Address2Handle: ");

  // Search peer table and return handle if found
  for (h = 0; h < loop->nexthandle; h++)
  {
    if ( loop->peers[h].addr == addr) {
	  //DPRINT(("Address2Handle: got %d\n", h));
      return(h);
    }
  }

  // handle not found: add it to peer table, if desired
  // Open all mutexes, etc

	if ((!insert) || (loop->nexthandle >= MAX_PEERS))
		return(loop_HDL_NONE);
	if (!address_to_peer(addr, &loop->peers[h]))
		return loop_HDL_NONE;

	DPRINT(("Address2Handle: created new handle %d == %d\n", h, loop->nexthandle));
	loop->nexthandle++;

	return(h);
}

/*****************************************************************************

                            LOOPWIN_Handle2Address()

******************************************************************************/

// Given a handle, return the corresponding loopback address.
// Returns loop_RES_EMPTY on failure.

int LOOPWIN_Handle2Address(LOOPINSTANCE *loop, LOOPHANDLE handle, LOOPADDR *addr)

{
  *addr = loop->peers[handle].addr;

  return(loop_RES_OK);
}

/*****************************************************************************

                                  LOOPWIN_PutPacket()

******************************************************************************/

// Send a packet via loopback to the given destination.
// Copies len bytes from buf into internal packet queue.
// Must have previously gotten handle for destination by calling loop_adr2hdl.

int LOOPWIN_PutPacket(LOOPINSTANCE *loop, void *bufptr, ULONG len, LOOPHANDLE hdest)

{
  int			nobytes;
  LOOPPEER		*peer;

//  DPRINT(("LOOPWIN_PutPacket() Called..\n"));

  // do sanity checks
  if ((len < 1) || (hdest >= loop->nexthandle) || (len > LOOP_MAX_USERDATALEN) || (!bufptr))  return loop_RES_BAD;

  // get ptr to peer address from peer handle
  peer = &loop->peers[hdest];

  //printaddr(peerptr, "LOOPWIN_PutPacket(): ");

  // send the packet
  if (peer->addr == BROADCAST_LOOPADDR)
	  nobytes = loopbroadcast(loop->addr, (char *) bufptr, (int) len, 0);
  else
	  nobytes = loopsend(loop->addr, (char *) bufptr, (int) len, 0, peer);
  if ((nobytes == 0) || (nobytes == LOOP_ERROR))  {
	  getlooperror();
	  return(loop_RES_FULL);
  }

  return(loop_RES_OK);
}

/*****************************************************************************

                                  LOOPWIN_GetPacket()

******************************************************************************/

// Receive a packet via loopback
// Caller must put size of buffer in len before calling.
// Packets larger than buffer will be silently ignored.
// On failure, returns
//  loop_RES_BAD if *len looked bogus, or buf or srcHdl were NULL,
//         or packet was too big to fit in buffer, or 0 size.
//  loop_RES_EMPTY if no good packet was availible.
// On success,
//  returns loop_RES_OK,
//  sets *len to the packet length,
//  sets *hsrc to the handle of the sender (or loop_HDL_NONE if one
//    hasn't been assigned yet with loop_adr2hdl), and
//  sets *srcAdr to the netware address of the sender if srcAdr is not NULL.
//
// To reply if source was loop_HDL_NONE, you must first call loop_adr2hdl
// to turn the source's address into a handle.
// (loop_get could do this for you, but then the peer table might fill up
// with all sorts of garbage addresses from hosts who sent you junk mail.)

int LOOPWIN_GetPacket(LOOPINSTANCE *loop, void *bufptr, ULONG *plen, LOOPHANDLE *hsrc, LOOPADDR *srcaddr)

{
  int     addrlen;
  ULONG   nobytes;
  LOOPADDR addr;

//  DPRINT(("LOOPWIN_GetPacket() Called..\n"));

  // do sanity checks.  Only complain if buffer length looks really crazy.
  if ((*plen < 1) || (*plen > LOOP_MAX_USERDATALEN * 10) || (!bufptr) || (!hsrc)) {
	DPRINT(("LOOPWIN_GetPacket: insane: *plen %d, bufptr %p, hsrc %p.\n",
	  *plen, bufptr, hsrc));
	return(loop_RES_BAD);
  }

  // read next packet
  addrlen = sizeof(LOOPADDR);
  nobytes = looprecv(loop->addr, (char *) bufptr, (int) *plen, 0, &addr);
  if ((nobytes == 0) || (nobytes == LOOP_ERROR))  {
	if (nobytes == LOOP_ERROR) {
		DPRINT(("LOOPWIN_GetPacket: \n"));
		getlooperror();
	}
	return(loop_RES_EMPTY);
  }

  if ((nobytes < 1) || (nobytes > LOOP_MAX_USERDATALEN) || (nobytes > *plen)) {
	DPRINT(("LOOPWIN_GetPacket: pkt -1 or too big for buffer.\n"));
	return(loop_RES_BAD);
  }

  // setup return info
  plen[0] = nobytes;
  hsrc[0] = LOOPWIN_Address2Handle(loop, addr, 0);
  if (srcaddr)  srcaddr[0] = addr;

  //printaddr(&addr, "LOOPWIN_GetPacket(): ");
  DPRINT(("LOOPWIN_GetPacket: got packet, len %d, src adr %d, src hdl %d\n",
		  nobytes, addr, hsrc[0]));
  return(loop_RES_OK);
}

//*****************************************************************************
// Static functions

// FUNCTION:         add_adr_into_dir
// AUTHOR:           Dan Kegel
// LAST REVISION:    9 July 1997
// PURPOSE:          Adds an address into the broadcast destination directory
// ARGUMENTS:        newaddr: address we want to stuff into directory
// RETURNS:          TRUE on success, FALSE on table full
// GENERAL COMMENTS:
//  Only called when allocating your own address.

static int		add_adr_into_dir(LOOPADDR newaddr)
{
	DWORD		i;
	DWORD hole;

	WaitForSingleObject(hDirMutex, INFINITE);
	DPRINT(("add_adr_into_dir: got dir mutex\n"));

	hole = NUM_DIRENTRIES;
	for (i=0; i < NUM_DIRENTRIES; i++) {
		if (dir->entries[i] == newaddr) {
			DPRINT(("add_adr_into_dir: found old dir entry %d for adr %x\n", i, newaddr));
			break;
		}
		if (dir->entries[i] == INVALID_LOOPADDR)
			hole = i;
	}
	if ((i == NUM_DIRENTRIES) && (hole != NUM_DIRENTRIES)) {
		DPRINT(("add_adr_into_dir: using new dir entry %d for adr %x\n", hole, newaddr));
		dir->entries[hole] = newaddr;
	}
	ReleaseMutex(hDirMutex);
	DPRINT(("add_adr_into_dir: released dir mutex\n"));
	if ((i == NUM_DIRENTRIES) && (hole == NUM_DIRENTRIES)) {
		DPRINT(("add_adr_into_dir: can't allocate directory entry\n"));
		return FALSE;
	}
	return TRUE;
}

// FUNCTION:         open_loopback
// AUTHOR:           Marshall Robin
// LAST REVISION:    14 May 1997
// PURPOSE:          Opens loopback network service
// ARGUMENTS:        mypeer:		Pointer to my peer structure
//   sessionId: random number chosen by app; preserved across freeze/thaw.
// RETURNS:          LOOPADDR:		Address of this instance
// EXTERN INPUTS:    none
// EXTERN OUTPUTS:   none
// GENERAL COMMENTS:
//

static LOOPADDR		open_loopback(LOOPPEER *mypeer, long sessionId)

{
	LOOPADDR	myaddr;
	BOOL		new_dir;
	char		buffer[80];

	hDirFileMap = hDirMutex = dir = NULL;
	hPbufFileMap = hPbufMutex = pbuffer = NULL;

// Create semaphore to synchronize access to the directory

	hDirMutex = CreateMutex(NULL, FALSE, MUTEX_NAME);
	if (!hDirMutex) {
		strcpy(error_text, "Could not create directory mutex");
		return INVALID_LOOPADDR;
	}

// Opens the network shared directory. If it doesn't exist yet, initialize it here

	WaitForSingleObject(hDirMutex, INFINITE);
	DPRINT(("open_loopback: got dir mutex\n"));

	hDirFileMap = CreateFileMapping((HANDLE)0xffffffff, NULL, PAGE_READWRITE, 0, sizeof(LOOP_DIRECTORY), MEMMAP_NAME);
	if (!hDirFileMap) {
		strcpy(error_text, "Could not open directory shared memory region.");
		ReleaseMutex(hDirMutex);
		DPRINT(("open_loopback: released dir mutex\n"));
		return INVALID_LOOPADDR;
	}
	new_dir = (GetLastError() != ERROR_ALREADY_EXISTS);
	dir = (LOOP_DIRECTORY *)MapViewOfFile(hDirFileMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if (!dir) {
		strcpy(error_text, "Could not map directory shared memory region.");
		ReleaseMutex(hDirMutex);
		DPRINT(("open_loopback: released dir mutex\n"));
		return INVALID_LOOPADDR;
	}
	if (new_dir) {
		memset(dir, 0, sizeof(*dir));		// Zero out all addresses
		dir->next_addr = 1;
	}

// Get my address from the shared directory and increment the next address

	myaddr = dir->next_addr++;
	if (!dir->next_addr) dir->next_addr++;	// Can't have address 0 being assigned!

	// Caller can specify his own address!
	// This lets you reclaim same address second time you init the driver.
	// We really should check somehow for address collision ...
	if (sessionId)
		myaddr = sessionId;

	ReleaseMutex(hDirMutex);
	DPRINT(("open_loopback: released dir mutex\n"));

// Add my address to the directory
	if (!add_adr_into_dir(myaddr)) {
		strcpy(error_text, "Could not create directory entry");
		return INVALID_LOOPADDR;
	}

// Create semaphore to access receive buffer

	sprintf(buffer, "%s%x", MUTEX_NAME, myaddr);
	hPbufMutex = CreateMutex(NULL, TRUE, buffer);
	DPRINT(("open_loopback: got buffer mutex for %x (me)\n", myaddr));
	if (!hPbufMutex) {
		strcpy(error_text, "Could not create packet buffer mutex");
		return INVALID_LOOPADDR;
	}

// Creates my packet receive buffer in shared memory

	sprintf(buffer, "%s%x", MEMMAP_NAME, myaddr);
	hPbufFileMap = CreateFileMapping((HANDLE)0xffffffff, NULL, PAGE_READWRITE, 0, sizeof(LOOP_PACKBUFFER), buffer);
	if (!hPbufFileMap) {
		strcpy(error_text, "Could not open packet buffer shared memory region.");
		ReleaseMutex(hPbufMutex);
		DPRINT(("open_loopback: released buffer mutex for %x (me)\n", myaddr));
		return INVALID_LOOPADDR;
	}
	pbuffer = (LOOP_PACKBUFFER *)MapViewOfFile(hPbufFileMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if (!pbuffer) {
		strcpy(error_text, "Could not map packet buffer shared memory region.");
		ReleaseMutex(hPbufMutex);
		DPRINT(("open_loopback: released buffer mutex for %x (me)\n", myaddr));
		return INVALID_LOOPADDR;
	}
	memset(pbuffer, 0, sizeof(*pbuffer));

	ReleaseMutex(hPbufMutex);
	DPRINT(("open_loopback: released buffer mutex for %x (me)\n", myaddr));

// Fill in my peer structure

	mypeer->addr = myaddr;
	mypeer->mutex = hPbufMutex;
	mypeer->filemap = hPbufFileMap;
	mypeer->pbuf = pbuffer;
	DPRINT(("open_loopback: myaddr %x, mybuffermutex %x\n", mypeer->addr, mypeer->mutex));

// Return my address

	return myaddr;
}

//*****************************************************************************

// FUNCTION:         close_loopback
// AUTHOR:           Marshall Robin
// LAST REVISION:    14 May 1997
// PURPOSE:          Closes loopback network service
// ARGUMENTS:        myaddr:		My address
// RETURNS:          void
// EXTERN INPUTS:    none
// EXTERN OUTPUTS:   none
// GENERAL COMMENTS:
//

static void			close_loopback(LOOPADDR myaddr)

{
	DWORD	i;

// Find my entry in the directory and remove it.

	WaitForSingleObject(hDirMutex, INFINITE);
	DPRINT(("close_loopback: got dir mutex\n"));
	for (i = 0; i < NUM_DIRENTRIES; i++) {
		if (dir->entries[i] == myaddr) {
			dir->entries[i] = INVALID_LOOPADDR;
			break;
		}
	}
	if (i == NUM_DIRENTRIES) {
		DPRINT(("close_loopback: can't find my address %x in the directory!\n", myaddr));
	}

	ReleaseMutex(hDirMutex);
	DPRINT(("close_loopback: released dir mutex\n"));

// Unmap the directory and close my handles to it

	UnmapViewOfFile((LPVOID) dir);
	CloseHandle(hDirMutex);
	CloseHandle(hDirFileMap);

// Unmap my packet buffer and close my handles. Set the closed flag in the buffer
// to notify other processes to release their handles

	WaitForSingleObject(hPbufMutex, INFINITE);
	DPRINT(("close_loopback: got buffer mutex for %x (me)\n", myaddr));
	pbuffer->flags |= PBUF_CLOSED;
	ReleaseMutex(hPbufMutex);
	DPRINT(("close_loopback: released buffer mutex for %x (me)\n", myaddr));

	UnmapViewOfFile((LPVOID) pbuffer);
	CloseHandle(hPbufFileMap);
	CloseHandle(hPbufMutex);
}

//*****************************************************************************

// FUNCTION:         loopsend
// AUTHOR:           Marshall Robin
// LAST REVISION:    14 May 1997
// PURPOSE:          Sends a packet to a single specified recipient
// ARGUMENTS:        myaddr:		My address
//					 buf:			Buffer containing packet data to be sent
//					 len:			Length of packet data
//					 flags:			Packet flags
//					 to:			Destination info
// RETURNS:          int:			Size of data sent
// EXTERN INPUTS:    none
// EXTERN OUTPUTS:   none
// GENERAL COMMENTS:
//

static int			loopsend(LOOPADDR myaddr, char *buf, int len, int flags, LOOPPEER *to)

{
	LOOP_PACKBUFFER	*to_buf;
	LOOP_PACKET		*pkt;
	DWORD			newback;

// Return if we've closed our handles to this player and he isn't in
// the directory; otherwise try to reinitialize handle to this player.
// Note that we close handles to a player when we try to send to him after he
// has closed his handle due to a dpDestroy(); if he reopens his handle and
// enters himself back into the directory (eg. he had frozen himself and then
// done a thaw), we can detect his directory record and reinitialize our
// handle to him.

	if (!to->mutex)  {
		int i;
		WaitForSingleObject(hDirMutex, INFINITE);
		DPRINT(("loopsend: got dir mutex\n"));
		for (i = 0; i < NUM_DIRENTRIES; i++) {
			if (dir->entries[i] == to->addr) {
				if (!address_to_peer(to->addr, to))
					i = NUM_DIRENTRIES;
				break;
			}
		}
		ReleaseMutex(hDirMutex);
		DPRINT(("loopsend: released dir mutex\n"));
		if (i >= NUM_DIRENTRIES) return len;		// We're throwing away the packet, but it was "delivered"
	}

// Get access to the receive buffer of the destination. If the destination has terminated and closed the buffer,
// release our handle to it and exit.

	WaitForSingleObject(to->mutex, INFINITE);
	DPRINT(("loopsend: got buffer mutex %x for dest %x\n", to->mutex, to->addr));
	to_buf = (LOOP_PACKBUFFER *)to->pbuf;
	if (to_buf->flags & PBUF_CLOSED) {
		ReleaseMutex(to->mutex);
		DPRINT(("loopsend: released buffer mutex %x for dest %x\n", to->mutex, to->addr));
		UnmapViewOfFile((LPVOID) to->pbuf);
		CloseHandle(to->mutex);
		CloseHandle(to->filemap);
		to->mutex = to->filemap = to->pbuf = NULL;
		return len;
	}

// Get the next packet slot, and return if the receive buffer is full

	newback = (to_buf->back + 1) % LOOP_BUF_SIZE;
	if (to_buf->front == newback) {
		ReleaseMutex(to->mutex);
		DPRINT(("loopsend: released buffer mutex %x for dest %x\n", to->mutex, to->addr));
		return len;
	}

// Put the packet in the new slot

	pkt = &to_buf->packets[newback];
	pkt->to = to->addr;
	pkt->from = myaddr;
	pkt->flags = flags;
	if (len > LOOP_MAX_USERDATALEN) len = LOOP_MAX_USERDATALEN;
	pkt->len = len;
	memcpy(pkt->data, buf, len);
	to_buf->back = newback;

// Release the mutex

	ReleaseMutex(to->mutex);
	DPRINT(("loopsend: released buffer mutex %x for dest %x\n", to->mutex, to->addr));

	return len;
}

//*****************************************************************************

// FUNCTION:         loopbroadcast
// AUTHOR:           Marshall Robin
// LAST REVISION:    14 May 1997
// PURPOSE:          Sends a packet to everyone in the world
// ARGUMENTS:        myaddr:		My address
//					 buf:			Buffer containing packet data to be sent
//					 len:			Length of packet data
//					 flags:			Packet flags
// RETURNS:          int:			Size of data sent
// EXTERN INPUTS:    none
// EXTERN OUTPUTS:   none
// GENERAL COMMENTS:
//

static int			loopbroadcast(LOOPADDR myaddr, char *buf, int len, int flags)

{
	LOOPPEER	temp;
	int			newlen;
	DWORD		i;

// Get access to the directory

	WaitForSingleObject(hDirMutex, INFINITE);
	DPRINT(("loopbroadcast: got dir mutex\n"));

// For each valid address in the directory, create a temporary LOOPPEER structure for
// the destination and send the packet to it.

	for (i = 0; i < NUM_DIRENTRIES; i++) {
		if ((dir->entries[i] != myaddr) && (dir->entries[i] != INVALID_LOOPADDR)) {
			if (!address_to_peer(dir->entries[i], &temp))
				continue;
			newlen = loopsend(myaddr, buf, len, flags, &temp);
			UnmapViewOfFile((LPVOID) temp.pbuf);
			CloseHandle(temp.mutex);
			CloseHandle(temp.filemap);
			if (newlen != len) {
				ReleaseMutex(hDirMutex);
				DPRINT(("loopbroadcast: released dir mutex\n"));
				return newlen;
			}
		}
	}
	ReleaseMutex(hDirMutex);
	DPRINT(("loopbroadcast: released dir mutex\n"));
	return len;
}

//*****************************************************************************

// FUNCTION:         looprecv
// AUTHOR:           Marshall Robin
// LAST REVISION:    14 May 1997
// PURPOSE:          Receives a packet from the queue
// ARGUMENTS:        myaddr:		My address
//					 buf:			Buffer containing packet data to be sent
//					 len:			Length of packet data
//					 flags:			Packet flags
//					 from:			Source address
// RETURNS:          int:			Size of data sent
// EXTERN INPUTS:    none
// EXTERN OUTPUTS:   none
// GENERAL COMMENTS:
//

static int			looprecv(LOOPADDR myaddr, char *buf, int len, int flags, LOOPADDR *from)

{
	LOOP_PACKET		*pkt;
	DWORD			newfront;

// Get access to our recieve buffer.
// This shouldn't actually be necessary, since the receiver is the only one!! 5/20/97

//	WaitForSingleObject(hPbufMutex, INFINITE);

// Return if our input buffer is empty

	if (pbuffer->front == pbuffer->back) {
		Sleep(100);
//		ReleaseMutex(hPbufMutex);
		return 0;
	}

// Get all data from the packet

	newfront = (pbuffer->front + 1) % LOOP_BUF_SIZE;
	pkt = &pbuffer->packets[newfront];

	*from = pkt->from;
	if (len < pkt->len) {
//		ReleaseMutex(hPbufMutex);
		DPRINT(("looprecv: buffer size %d too small for packet %d\n", len, pkt->len));
		return pkt->len;
	}
	memcpy(buf, pkt->data, pkt->len);

// Release the mutex

//	ReleaseMutex(hPbufMutex);

// Set the new front after we have cleared the packet, so that senders won't stomp on the packet

	pbuffer->front = newfront;
	DPRINT(("looprecv: Got packet, source %d, size %d\n", *from, pkt->len));
	return pkt->len;
}

//*****************************************************************************

// FUNCTION:         address_to_peer
// AUTHOR:           Marshall Robin
// LAST REVISION:    14 May 1997
// PURPOSE:          Creates a peer structure from an address
// ARGUMENTS:        addr:			Target address
//					 peer:			Pointer to peer structure
// RETURNS:          BOOL:			True if successful
// EXTERN INPUTS:    none
// EXTERN OUTPUTS:   none
// GENERAL COMMENTS:
//

static BOOL		address_to_peer(LOOPADDR addr, LOOPPEER *peer)

{
	char	buffer[80];
	int created;

	peer->addr = addr;

// Open semaphore to access receive buffer

	sprintf(buffer, "%s%x", MUTEX_NAME, addr);
	peer->mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, buffer);
	created = FALSE;
	if (!peer->mutex) {
		peer->mutex = CreateMutex(NULL, FALSE, buffer);
		created = TRUE;
	}
	if (!peer->mutex) {
		sprintf(error_text, "Could not create packet buffer mutex for %x", addr);
		return FALSE;
	}

// Open packet receive buffer in shared memory

	sprintf(buffer, "%s%x", MEMMAP_NAME, addr);
	//peer->filemap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, buffer);
	peer->filemap = CreateFileMapping((HANDLE)0xffffffff, NULL, PAGE_READWRITE, 0, sizeof(LOOP_PACKBUFFER), buffer);
	if (!peer->filemap) {
		sprintf(error_text, "Could not open packet buffer shared memory region for %x", addr);
		CloseHandle(peer->mutex);
		return FALSE;
	}
	peer->pbuf = MapViewOfFile(peer->filemap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if (!peer->pbuf) {
		sprintf(error_text, "Could not map packet buffer shared memory region for %x", addr);
		CloseHandle(peer->mutex);
		CloseHandle(peer->filemap);
		return FALSE;
	}

// If this is the first open of that address, initialize the buffer
// but don't add the address to the broadcast destination list (aka
// directory), as we don't care if we drop broadcast packets on the floor
// while someone's away.
	if (created)
		memset(pbuffer, 0, sizeof(*pbuffer));

	return TRUE;
}
