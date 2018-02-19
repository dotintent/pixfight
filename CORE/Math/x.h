#ifndef FOUNDATION_X
#define FOUNDATION_X
/*----------------------------------------------------------------------------*\
|
|						Public Interface to Ageia PhysX Technology
|
|							     www.ageia.com
|
\*----------------------------------------------------------------------------*/

#define	X_UNREFERENCED_PARAMETER(P) (P)
#define X_INLINE inline

#define X_DELETE(x)	delete x
#define X_DELETE_SINGLE(x)	if (x) { delete x;		x = NULL; }
#define X_DELETE_ARRAY(x)	if (x) { delete []x;	x = NULL; }

template<class Type> void X_Swap(Type& a, Type& b)
{
	const Type c = a; a = b; b = c;
}

#include <time.h>
#include "xSimpleTypes.h"
#include <alloca.h>
#include <stdlib.h>
#define xAlloca(x)	alloca(x)

#endif
//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND
