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

/*
 *  Copyright (c) 1992 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 * Munged by Dan Kegel (dank@blacks.jpl.nasa.gov) for use in uwho.
 */
#include <stdio.h>
#include "raw.h"

#include <ssdef.h>
#include <rmsdef.h>
#include <dvidef.h>
#include <jpidef.h>
#include <libdef.h>
#include <descrip.h>
#include <iodef.h>
#include <ttdef.h>
/* #include <unixlib.h> */
/* #include <rms.h> */

static int KeyBoard_Quit = 0;
int INPUT_BUFFER_LEN;
char INPUT_BUFFER[255];

typedef struct {                /* I/O status block     */
        short i_cond;           /* Condition value      */
        short i_xfer;           /* Transfer count     */
        long  i_info;           /* Device information     */
} iosb;
 
typedef struct {                /* Terminal characteristics   */
        char  t_class;          /* Terminal class     */
        char  t_type;           /* Terminal type      */
        short t_width;          /* Terminal width in characters   */
        long  t_mandl;          /* Terminal's mode and length   */
        long  t_extend;         /* Extended terminal characteristics  */
}  termchar;                    

short TTY_CHANNEL_GLOBAL;


/*
 *      Exit Handler Control Block
 */
static struct argument_block
  {
      int forward_link;
      int (*exit_routine)();
      int arg_count;
      int *status_address;
      int exit_status;
  }
exit_block = 
  {
      0,
      NULL,
      1,
      &exit_block.exit_status,
      0
  };

int vms_input_buffer;

static struct vms_ast_iosb
{
    short status;
    short offset;
    short termlen;
    short term;
} vms_ast_iosb;

extern void vms_que_key_ast();
int vms_event_flag;
int event_flag_mask;
int ast_stop_input = 0;
int waiting_for_ast;

int getkey_ast(int x)
{
   char c = -1;

   if (waiting_for_ast)  SYS$SETEF (vms_event_flag);
   waiting_for_ast = 0;
   
   if (vms_ast_iosb.offset)
     {
	c = vms_input_buffer;
     }
   
   if (c >= 0) {
	if (c == 7) {
	     KeyBoard_Quit = 1;
	}
	 
	INPUT_BUFFER[INPUT_BUFFER_LEN++] = c;
   }
   vms_que_key_ast();
}


void vms_que_key_ast()
{
   static int trmmsk [2] = { 0, 0 };
   int status;
    
   if (ast_stop_input) return;
   status = SYS$QIO (0, TTY_CHANNEL_GLOBAL,
		     IO$_READVBLK + IO$M_NOECHO | IO$_TTYREADALL,
		     &vms_ast_iosb, getkey_ast, 1,
		     &vms_input_buffer, 1, 0, trmmsk, 0, 0);    
}

/* needed for callback.  hmm.  */
void reset_tty()
{
    raw_end();
}


/*--------------------------------------------------------------------------
 Call with 1 to install, 0 to deinstall raw mode
--------------------------------------------------------------------------*/
void raw_set_stdio(init)
    int init;
{
    $DESCRIPTOR ( Term, "SYS$ERROR");

    if (init) {
	INPUT_BUFFER_LEN = 0;

	if (sys$assign ( &Term, &TTY_CHANNEL_GLOBAL, 0, 0 )
	   != SS$_NORMAL)
	 {
	    fprintf(stderr,"Unable to assign input channel\n");
	    exit(0);
	 }
		    
	if (NULL == exit_block.exit_routine)
	 {
	    
	    exit_block.exit_routine = reset_tty;
	    SYS$DCLEXH(&exit_block);
	 }

	/* allocate an event flag and clear it--- used by ast routines */
	if (!vms_event_flag) LIB$GET_EF (&vms_event_flag);
	SYS$CLREF (vms_event_flag);
	event_flag_mask = ((unsigned) 1 << (vms_event_flag % 32));

	waiting_for_ast = 0;
	ast_stop_input = 0;
	vms_que_key_ast();   /* set up the key ast */

    } else {
	; /* don't need to do anything? */
    }
}

static char getkey()
{
   int i, imax;
   char ch;
   if (!INPUT_BUFFER_LEN) return(3); /* kludge; was sys_getkey()); */
   ch = INPUT_BUFFER[0];
   INPUT_BUFFER_LEN--;
   imax = INPUT_BUFFER_LEN;
   for (i = 0; i < imax; i++)
     {
	INPUT_BUFFER[i] = INPUT_BUFFER[i + 1];
	imax = INPUT_BUFFER_LEN;
     }
   return(ch);
}

/*--------------------------------------------------------------------------
 Return -1 if no key, 3 if user hit interrupt key, otherwise return key.
--------------------------------------------------------------------------*/
int raw_getc()
{
   char c;
   
   if (KeyBoard_Quit) {
	KeyBoard_Quit = 0;
	return 3;
   }
   /* clear the flag which ast will set */
   waiting_for_ast = 0;
   if (INPUT_BUFFER_LEN) return(getkey());
   SYS$CLREF (vms_event_flag);
   
   /* set up a flag for the ast so it knows to set the event flag */
   waiting_for_ast = 1;
   
   while (!INPUT_BUFFER_LEN)
     {
	/* this will return when ast does its job */
	SYS$WFLOR (vms_event_flag, event_flag_mask);
     }
   waiting_for_ast = 0;
   c = getkey();
   return(c);
}

    
/*--------------------------------------------------------------------------
 Return 0 if no key ready, 1 if key ready.
--------------------------------------------------------------------------*/
int raw_kbhit()
{
    if (KeyBoard_Quit) return 1;
    return(INPUT_BUFFER_LEN);
}

/*--------------------------------------------------------------------------
 Get the size of the terminal
--------------------------------------------------------------------------*/
void raw_get_term_dimensions(cols, rows)
    int *cols;
    int *rows;
{
   int status;
   iosb iostatus;
   $DESCRIPTOR(devnam, "SYS$ERROR");
   struct
     {
	short row_buflen;
	short row_itmcod;
	int *row_bufadr;
	short *row_retlen;
	short col_buflen;
	short col_itmcod;
	int *col_bufadr;
	short *col_retlen;
	int listend;
     } 
   itmlst =
     {
	sizeof(*rows), DVI$_TT_PAGE, rows, 0,
	sizeof(*cols), DVI$_DEVBUFSIZ, cols, 0,
	0
     };
    
   /* Get current terminal characteristics */
   status = sys$getdviw(0,           /* Wait on event flag zero  */
			0,           /* Channel to input terminal  */
			&devnam,     /* device name */
			&itmlst,	  /* Item descriptor List */
			&iostatus,   /* Status after operation */
			0, 0,        /* No AST service   */
			0);          /* nullarg */
			
   if (status&1) status = iostatus.i_cond;
   /* Jump out if bad status */
   if ((status & 1) == 0) exit(status);
}
