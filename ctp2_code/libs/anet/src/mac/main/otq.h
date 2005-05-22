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

#ifndef _OTQ_
#define	_OTQ_

#include <Types.h>

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct {
		Ptr					storage;
		unsigned long		in;				//	offset into buffer
		unsigned long		out;			//	offset into buffer
	} otq_t;
	
	void otq_put(otq_t* q, unsigned char* data, unsigned long theSize);
	Boolean otq_get(otq_t* q, unsigned char* data, unsigned long* theSize);otq_t* otq_create(void);
void otq_destroy(otq_t *otq); 


#ifdef __cplusplus
}
#endif

#endif
