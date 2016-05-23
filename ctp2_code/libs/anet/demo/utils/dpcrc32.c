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

/*--------------------------------------------------------------------------
 32-bit CRC routine.

 $Log: dpcrc32.c $
 Revision 1.2  1997/02/23 23:54:31  dkegel
 Refer to another implementation that looks faster.
 Revision 1.1  1997/02/23 05:38:24  dkegel
 Initial revision

 [See also: http://www.cdrom.com/pub/infozip/zlib/ for an incremental
 crc updater.]

 Newsgroups: comp.compression,comp.compression.research,news.answers,comp.answers
 Subject: comp.compression Frequently Asked Questions (part 1/3)
 Supersedes: <compr1_21dec96@prep.ai.mit.edu>
 Followup-To: comp.compression
 Date: 20 Jan 1997 21:33:05 GMT

 ...
 Subject: [51] I need a CRC algorithm

 As its name implies (Cyclic Redundancy Check) a crc adds redundancy whereas
 the topic of this group is to remove it. Yet this question comes up often in
 comp.compression.

 The file ftp://ftp.rocksoft.com/clients/rocksoft/papers/crc_v3.txt is a pretty
 comprehensive description of the whole CRC concept, including a C program.

 See also:
 - Schwaderer W.D., "CRC Calculation", April 85 PC Tech Journal, pp.118-133.
 - "Calculating CRCs by Bits and Bytes", BYTE Magazine, September 1986
 - Ramabadran T.V., Gaitonde S.S., "A tutorial on CRC computations", IEEE
   Micro, Aug 1988.
 - the source of all archivers, such as the file makecrc.c in the Info-ZIP
   sources (see extension .zip in item 2)

 The following C code (by Rob Warnock <rpw3@sgi.com>) does CRC-32 in
 BigEndian/BigEndian byte/bit order.  That is, the data is sent most
 significant byte first, and each of the bits within a byte is sent most
 significant bit first, as in FDDI. You will need to twiddle with it to do
 Ethernet CRC, i.e., BigEndian/LittleEndian byte/bit order. [Left as an
 exercise for the reader.]

 The CRCs this code generates agree with the vendor-supplied Verilog models
 of several of the popular FDDI "MAC" chips.
--------------------------------------------------------------------------*/

#include <stdio.h>
#include "dpcrc32.h"

static unsigned long crc32_table[256];
/* Initialized first time "crc32()" is called. If you prefer, you can
 * statically initialize it at compile time. [Another exercise.]
 */

/*
 * Build auxiliary table for parallel byte-at-a-time CRC-32.
 */
#define CRC32_POLY 0x04c11db7     /* AUTODIN II, Ethernet, & FDDI */

static void init_crc32()
{
	int i, j;
	unsigned long c;

	for (i = 0; i < 256; ++i) {
		for (c = i << 24, j = 8; j > 0; --j)
			c = c & 0x80000000 ? (c << 1) ^ CRC32_POLY : (c << 1);
		crc32_table[i] = c;
	}
}

unsigned long dp_crc32_inc(unsigned char *buf, int len, unsigned long crc)
{
	unsigned char *p;

	if (!crc32_table[1])    /* if not already done, */
		init_crc32();   /* build table */
	for (p = buf; len > 0; ++p, --len)
		crc = (crc << 8) ^ crc32_table[(crc >> 24) ^ *p];
	return crc;
}

unsigned long dp_crc32(unsigned char *buf, int len)
{
	/* preload shift register, per CRC-32 spec */
	/* transmit complement, per CRC-32 spec */
	return ~dp_crc32_inc(buf, len, 0xffffffff);
}

#define BUF_SIZE 8192
unsigned long dp_fil_crc32(FILE *fp)
{
	int nread;
	unsigned char buf[BUF_SIZE];
	unsigned long  crc;

	if (!crc32_table[1])    /* if not already done, */
		init_crc32();   /* build table */
	crc = 0xffffffff;       /* preload shift register, per CRC-32 spec */
	while ((nread = fread(buf, sizeof(unsigned char), BUF_SIZE, fp)) > 0) {
		crc = dp_crc32_inc(buf, nread, crc);
	}
	return ~crc;            /* transmit complement, per CRC-32 spec */
}
