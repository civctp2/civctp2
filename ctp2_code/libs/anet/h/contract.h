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

/*-----------------------------------------------------------------------
 File to hold various techniques to allow treating functions as if they
 were contracts.

 Each method defines its entry conditions ("preconditions"); the caller
 must guarantee that each of them is true when the method is invoked.
 If each precondition is true, the method guarantees that the exit
 conditions ("postconditions") will be true upon exit.

 A method may additionally require that certain conditions are true
 during execution; these may be used to check on the subcontracting
 methods, or as checks on program logic.

 Precondition and postcondition enforcement is typically removed from a
 release build of a product.  Correct program behavior should NOT depend
 on the existance of these checks; for such conditions, a standard
 language construct should be used.

 However, since we have runtime assertion logging that collects
 assertion failure info from endusers' machines, we will actually
 keep preconditions in, and log them with dpReportAssertionFailure.
-----------------------------------------------------------------------*/

#if !defined(_DP_CONTRACT_H)
#define _DP_CONTRACT_H 1

/* Select how to enforce preconditions, conditions, and postconditions */
/* Preconditions should be used to handle calling requirements (e.g., never
   call this function with a NULL first parameter).  Postconditions should
   be used to monitor things that must be true on exit from a function.
   Conditions handle monitoring the state inside a function. */
#if 1		/* Precondition implemented; others just assert. */

#ifdef _WIN32
#define precondition_r(x,r)	if(!(x)){dpReportAssertionFailure(__LINE__, __FILE__, #x);return (r);}
#elif defined(UNIX)
#define precondition_r(x,r)	if(!(x)){dpReportAssertionFailure(__LINE__, __FILE__, __STRING(x));return (r);}
#else
#define precondition_r(x,r)	if(!(x)){dpReportAssertionFailure(__LINE__, __FILE__, "?");return (r);}
#endif

#define condition(x,s)		assert(x)
#define postcondition(x)	assert(x)

#elif 0		/* others just assert. */

#define precondition_r(x,r)	assert(x)
#define condition(x,s)		assert(x)
#define postcondition(x)	assert(x)

#elif 0
#define precondition_r(x,r)	if(!(x)){DPRINT(("%s(%d) bug: !(%s).\n",__FILE__,__LINE__,#x));return (r);}
#define condition(x,s)		if(!(x)){DPRINT(("%s(%d) bug: !(%s) %s.\n",__FILE__,__LINE__,#x,(s)));return dp_RES_BUG;}
#define postcondition(x)	if(!(x)){DPRINT(("%s(%d) bug: !(%s).\n",__FILE__,__LINE__,#x));return dp_RES_BUG;}

#else
#define precondition(x)
#define condition(x,s)
#define postcondition(x)
#endif

#define precondition(x)		precondition_r(x, dp_RES_BAD)
#endif /* _DP_CONTRACT_H */
