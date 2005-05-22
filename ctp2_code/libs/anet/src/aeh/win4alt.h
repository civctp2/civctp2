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

/* Defines from windows system files for non-windows systems */

#ifndef win4alt_h
#define win4alt_h

/* from winnt.h */
#define EXCEPTION_NONCONTINUABLE 0x1    // Noncontinuable exception
#define STATUS_GUARD_PAGE_VIOLATION      ((unsigned long)0x80000001L)    
#define STATUS_DATATYPE_MISALIGNMENT     ((unsigned long)0x80000002L)    
#define STATUS_BREAKPOINT                ((unsigned long)0x80000003L)    
#define STATUS_SINGLE_STEP               ((unsigned long)0x80000004L)    
#define STATUS_ACCESS_VIOLATION          ((unsigned long)0xC0000005L)    
#define STATUS_IN_PAGE_ERROR             ((unsigned long)0xC0000006L)    
#define STATUS_INVALID_HANDLE            ((unsigned long)0xC0000008L)    
#define STATUS_NO_MEMORY                 ((unsigned long)0xC0000017L)    
#define STATUS_ILLEGAL_INSTRUCTION       ((unsigned long)0xC000001DL)    
#define STATUS_NONCONTINUABLE_EXCEPTION  ((unsigned long)0xC0000025L)    
#define STATUS_INVALID_DISPOSITION       ((unsigned long)0xC0000026L)    
#define STATUS_ARRAY_BOUNDS_EXCEEDED     ((unsigned long)0xC000008CL)    
#define STATUS_FLOAT_DENORMAL_OPERAND    ((unsigned long)0xC000008DL)    
#define STATUS_FLOAT_DIVIDE_BY_ZERO      ((unsigned long)0xC000008EL)    
#define STATUS_FLOAT_INEXACT_RESULT      ((unsigned long)0xC000008FL)    
#define STATUS_FLOAT_INVALID_OPERATION   ((unsigned long)0xC0000090L)    
#define STATUS_FLOAT_OVERFLOW            ((unsigned long)0xC0000091L)    
#define STATUS_FLOAT_STACK_CHECK         ((unsigned long)0xC0000092L)    
#define STATUS_FLOAT_UNDERFLOW           ((unsigned long)0xC0000093L)    
#define STATUS_INTEGER_DIVIDE_BY_ZERO    ((unsigned long)0xC0000094L)    
#define STATUS_INTEGER_OVERFLOW          ((unsigned long)0xC0000095L)    
#define STATUS_PRIVILEGED_INSTRUCTION    ((unsigned long)0xC0000096L)    
#define STATUS_STACK_OVERFLOW            ((unsigned long)0xC00000FDL)    
#define STATUS_CONTROL_C_EXIT            ((unsigned long)0xC000013AL)    

/* from winbase.h */
#define EXCEPTION_ACCESS_VIOLATION          STATUS_ACCESS_VIOLATION
#define EXCEPTION_DATATYPE_MISALIGNMENT     STATUS_DATATYPE_MISALIGNMENT
#define EXCEPTION_BREAKPOINT                STATUS_BREAKPOINT
#define EXCEPTION_SINGLE_STEP               STATUS_SINGLE_STEP
#define EXCEPTION_ARRAY_BOUNDS_EXCEEDED     STATUS_ARRAY_BOUNDS_EXCEEDED
#define EXCEPTION_FLT_DENORMAL_OPERAND      STATUS_FLOAT_DENORMAL_OPERAND
#define EXCEPTION_FLT_DIVIDE_BY_ZERO        STATUS_FLOAT_DIVIDE_BY_ZERO
#define EXCEPTION_FLT_INEXACT_RESULT        STATUS_FLOAT_INEXACT_RESULT
#define EXCEPTION_FLT_INVALID_OPERATION     STATUS_FLOAT_INVALID_OPERATION
#define EXCEPTION_FLT_OVERFLOW              STATUS_FLOAT_OVERFLOW
#define EXCEPTION_FLT_STACK_CHECK           STATUS_FLOAT_STACK_CHECK
#define EXCEPTION_FLT_UNDERFLOW             STATUS_FLOAT_UNDERFLOW
#define EXCEPTION_INT_DIVIDE_BY_ZERO        STATUS_INTEGER_DIVIDE_BY_ZERO
#define EXCEPTION_INT_OVERFLOW              STATUS_INTEGER_OVERFLOW
#define EXCEPTION_PRIV_INSTRUCTION          STATUS_PRIVILEGED_INSTRUCTION
#define EXCEPTION_IN_PAGE_ERROR             STATUS_IN_PAGE_ERROR
#define EXCEPTION_ILLEGAL_INSTRUCTION       STATUS_ILLEGAL_INSTRUCTION
#define EXCEPTION_NONCONTINUABLE_EXCEPTION  STATUS_NONCONTINUABLE_EXCEPTION
#define EXCEPTION_STACK_OVERFLOW            STATUS_STACK_OVERFLOW
#define EXCEPTION_INVALID_DISPOSITION       STATUS_INVALID_DISPOSITION
#define EXCEPTION_GUARD_PAGE                STATUS_GUARD_PAGE_VIOLATION
#define EXCEPTION_INVALID_HANDLE            STATUS_INVALID_HANDLE
#define CONTROL_C_EXIT                      STATUS_CONTROL_C_EXIT

#endif /* win4alt_h */
