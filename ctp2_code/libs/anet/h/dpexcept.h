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

/*----------------------------------------------------------------------
 Private functions providing an interface between aeh and dp functions,
  including sending/receiving exception records using dptab.
----------------------------------------------------------------------*/

#ifndef dpexcept_h
#define dpexcept_h

#include "dptab.h"
#include "aeh.h"
#include "aehlog.h"

/*--------------------------------------------------------------------------
 Set callback for the exception table. Used by the game server to process
  exception records when received by dptab.
 Input: dptab, cb, context
 Return: dp_RES_BAD if exception table doesn't exist or can't be created
         otherwise return status of dptab_setTableCallback()
--------------------------------------------------------------------------*/
dp_result_t dp_setExceptionCb(dptab_t *dptab, dptab_status_cb cb, void *context);

/*--------------------------------------------------------------------------
 Send exception records from file to the game server.
 Exception file is deleted afterwards if exception records were succesfully
  queued for delivery to game server.
 Input: dptab
        h (set to game server handle)
        aehlog (if NULL, uses default exception file;
               otherwise, specifies exception file set using aehlog_Init())
 Return: dp_RES_BAD if exception table doesn't exist or can't be created
         dp_RES_EMPTY if there are no exception records found
         otherwise return status of dptab_addSubscriber(), dptab_set()
--------------------------------------------------------------------------*/
dp_result_t dp_publishExceptions(dptab_t *dptab, playerHdl_t h, aehlog_t *aehlog);

/*--------------------------------------------------------------------------
 Allow exception records from host.
 Input: dptab
        h (handle of host to allow records from)
 Return: dp_RES_BAD if exception table doesn't exist or can't be created
         otherwise return status of dptab_addPublisher()
--------------------------------------------------------------------------*/
dp_result_t dp_subscribeExceptions(dptab_t *dptab, playerHdl_t h);

/*--------------------------------------------------------------------------
 Save an exception record to file.
 Input: buf (exception record packet)
        buflen (length of buf)
        fmaxsize (if file larger than fmaxsize, delete file contents before
                  writing packet)
        aehlog (specifies exception file set using aehlog_Init();cannot be NULL)
 Return: dp_RES_BAD if couldn't write buf to file
         otherwise, dp_RES_OK
--------------------------------------------------------------------------*/
dp_result_t dp_handleExceptionPacket(unsigned char *buf, unsigned int buflen, unsigned int fmaxsize, aehlog_t *aehlog);

#endif
