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

/* ***********************************************************************
 *                               RCS HEADER
 * $Locker: $
 * $Log: Userid.h $
 * Revision 1.1  1996/12/13 23:24:44  jgraebner
 * Initial revision
 * Revision 1.1  1996/02/20 05:17:41  dkegel
 * Initial revision
 * Revision 1.1  1995/11/10 21:50:02  dkegel
 * Initial revision
 *
 * *********************************************************************** */

/*
 *	UserID.h
 *
 *	User ID.
 */

#ifndef _WINDOWS
  #pragma once			// Avoid multiple inclusions
#endif

#ifndef _UserID_h_
#define _UserID_h_

#pragma	pack(1)			// Don't pad or align structures

/*
 *	The user ID is a value which is guaranteed to be unique for each player
 *	in a game.  It has a value between 0 and n-1, where n is the number of
 *	players in the game.  It can change from game to game, but it is constant
 *	across all players within a game.  In other words, the user with user ID
 *	j controls the gamepiece defined by Gamepieces[j] on all machines.
 */

typedef unsigned char	userID_t;			// User ID

/*
 *	The node ID is any value which is pretty much guaranteed to be unique
 *	for each node in the world.  It can change from game to game.
 */

typedef struct {							// Node ID
	unsigned long	_0,
					_1;
} nodeID_t;

/*
 *	Reserve a few special node IDs.  Whenever we generate a legitimate node
 *	ID, we must ensure that it doesn't equal any of these.  By convention,
 *	the first half is always zero.
 */

								// Values for nodeID_t
				// Out-of-band values
#define NODEID_SPECIAL_0	0					// Prefix for special node IDs
#define NODEID_NONE		(nodeID_NONE)		// No node ID
#define NODEID_NONE_1		  0					// Suffix
extern nodeID_t	nodeID_NONE;
#define NODEID_UNKNOWN	(nodeID_UNKNOWN)	// Unknown node ID
#define NODEID_UNKNOWN_1	  1					// Suffix
extern nodeID_t	nodeID_UNKNOWN;

/*
 *	Return our user ID, or -1 on error.
 */

userID_t
myUserID(
	void)
;

/*
 *	Generate what we hope is a globally unique identifier for this node.
 *
 *	Return the identifier.  Repeated calls return the same value.
 */

nodeID_t
myNodeID(
	void)
;

#endif // _UserID_h_
