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

/*==========================================================================
 *
 *  Copyright (C) 1995-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dsetup.h
 *  Content:    DirectXSetup, error codes and flags
 ***************************************************************************/

#define DSETUPERR_BADWINDOWSVERSION     -1
#define DSETUPERR_SOURCEFILENOTFOUND    -2
#define DSETUPERR_BADSOURCESIZE         -3
#define DSETUPERR_BADSOURCETIME         -4
#define DSETUPERR_NOCOPY                -5
#define DSETUPERR_OUTOFDISKSPACE        -6
#define DSETUPERR_CANTFINDINF           -7
#define DSETUPERR_CANTFINDDIR           -8
#define DSETUPERR_INTERNAL              -9
//#define DSETUPERR_NTWITHNO3D          -10  // obsolete, see docs
#define DSETUPERR_UNKNOWNOS             -11
#define DSETUPERR_USERHITCANCEL         -12
#define DSETUPERR_NOTPREINSTALLEDONNT   -13

//
// DSETUP Flags
//
#define DSETUP_DDRAW            0x00000001      /* install DirectDraw           */
#define DSETUP_DSOUND           0x00000002      /* install DirectSound          */
#define DSETUP_DPLAY            0x00000004      /* install DirectPlay           */
#define DSETUP_DDRAWDRV         0x00000008      /* install DirectDraw Drivers   */
#define DSETUP_DSOUNDDRV        0x00000010      /* install DirectSound Drivers  */
#define DSETUP_DPLAYSP          0x00000020      /* install DirectPlay Providers */
#define DSETUP_DVIDEO           0x00000040      /* install DirectVideo          */

#define DSETUP_D3D              0x00000200      /* install Direct3D                                                     */
//
// This flag is obsolete.
// For NT, we now return either 0 for pre installed or
// DSETUPERR_NOTPREINSTALLEDONNT which implies this version of DirectX
// is not installed on this machine.
//
//#define DSETUP_REQUIRESD3D    0x00000400 | DSETUP_D3D /* install Direct3D, pop up dialog box on NT, if no D3D present         */
#define DSETUP_DINPUT           0x00000800      /* install DirectInput                                                     */
#define DSETUP_DIRECTXSETUP     0x00001000      /* install DirectXSetup DLL's   */
#define DSETUP_PROMPTFORDRIVERS 0x10000000      /* prompt when replacing display/audio drivers  */
#define DSETUP_RESTOREDRIVERS   0x20000000      /* restore display/audio drivers  */
