#ifndef FOUNDATION_X9F32
#define FOUNDATION_X9F32
/*----------------------------------------------------------------------------*\
|
|						Public Interface to Ageia PhysX Technology
|
|							     www.ageia.com
|
\*----------------------------------------------------------------------------*/
/** \addtogroup foundation
  @{
*/

//Exclude file from docs
/** \cond */

#include "xf.h"

//the file name of this header is legacy due to pain of renaming file in repository.

class x9Real
	{
	
	public:
        struct S
			{
#ifndef TRANSPOSED_MAT33
			xReal        _11, _12, _13;
			xReal        _21, _22, _23;
			xReal        _31, _32, _33;
#else
			xReal        _11, _21, _31;
			xReal        _12, _22, _32;
			xReal        _13, _23, _33;
#endif
			};
	
    union 
		{
		S s;
		xReal m[3][3];
		};
	};

/** \endcond */
 /** @} */
#endif
//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND
