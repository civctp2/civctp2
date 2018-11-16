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

#include	"otq.h"
#include	"TransportUtils.h"
#include	"Util.h"

#include <stdlib.h>   // for malloc()
#include <string.h>   // for memcpy()

#define	kQueueSize			(64*1024)

void otq_put(otq_t* q, unsigned char* data, unsigned long theSize) {
	unsigned long			spaceInBuffer;

	//	Empty Buffer
	//
	//	I-----------------------------------
	//	O-----------------------------------

	//	Space At End of Buffer
	//
	//	------------------I-----------------
	//	-----O------------------------------

	//	Space In Center of Buffer
	//
	//	---------I--------------------------
	//	-----------------------O------------

	//	check to see if there is enough room in the buffer

	if (q->in >= q->out) {

		//	compute the amount of space between here and the end of the buffer

		spaceInBuffer = kQueueSize - q->in;

		if ((theSize + 4) < spaceInBuffer) {

			//	there is room at the end of the buffer for the packet

			* ((unsigned long*) (q->storage + q->in)) = theSize;
			memcpy(q->storage + q->in + sizeof(unsigned long), data, theSize);
			q->in += theSize + sizeof(unsigned long);

		} else {

			//	there is not room at the end of the buffer

			spaceInBuffer = q->out - 1;

			if ((theSize + 4) < spaceInBuffer) {

				//	add the packet at the beginning of the buffer

				* ((unsigned long*) (q->storage + q->in)) = 0xffffffff;	//	end of buffer
				* ((unsigned long*) (q->storage + 0)) = theSize;
				memcpy(q->storage + 0 + sizeof(unsigned long), data, theSize);
				q->in = theSize + sizeof(unsigned long);
			} else {
				#ifdef MACDEBUG
					ColorDebugStr("\pError 1");
				#endif
			}

		}

	} else {

		//	compute the amount of space between the in pointer and the out pointer

		spaceInBuffer = q->out - q->in;

		if ((theSize + 4) < spaceInBuffer) {

			//	there is room at the end of the buffer for the packet

			* ((unsigned long*) (q->storage + q->in)) = theSize;
			memcpy(q->storage + q->in + sizeof(unsigned long), data, theSize);
			q->in += theSize + sizeof(unsigned long);

		} else {
			#ifdef MACDEBUG
				ColorDebugStr("\pError 2");
			#endif
		}
	}

}

Boolean otq_get(otq_t* q, unsigned char* data, unsigned long* theSize) {
	Boolean					gotData = false;
	unsigned long			dataSize;

	//	put the packet into the buffer pointed to by the data pointer. We also
	//	return the size of the packet

	//	is there any data in the queue

	if (q->in != q->out) {

		//	there is data in the queue, get one packet out

		dataSize = * (unsigned long*) (q->storage + q->out);
		if (dataSize == 0xffffffff) {

			//	this is the end of the data in the buffer, we must
			//	now wrap the output index

			q->out = 0;
			dataSize = * (unsigned long*) (q->storage + q->out);
		}

		if (dataSize > *theSize) {

			//	there is not enough room for this packet in the client's
			//	buffer, so, we don't return anything

			#ifdef MACDEBUG
				ColorDebugStr("\pPacket Too Big For Out Buffer");
			#endif

		} else {

			//	read the size and the packet out of the buffer

			*theSize = dataSize;
			memcpy(data, q->storage + q->out + sizeof(unsigned long), dataSize);

			gotData = true;
		}
		q->out += dataSize + sizeof(unsigned long);

	}

	return gotData;
}

#pragma segment Main
otq_t* otq_create(void) {
	otq_t*	otq;

	otq = (otq_t *)malloc(sizeof(otq_t));
	if (otq != nil) {
		otq->storage = (char *)malloc(kQueueSize + 4);		//	space for end of queue flag
		otq->in = 0;
		otq->out = 0;

		if (otq->storage == nil) {
			free(otq);
			otq = nil;
		}
	}

	return otq;
}

#pragma segment Main
void otq_destroy(otq_t *otq) {

	if (otq != nil) {

		//	dispose of the storage buffer

		if (otq->storage != nil) {
			free(otq->storage);
			otq->storage = nil;
		}

		//	dispose of the queue structure

		free(otq);
	}

}
