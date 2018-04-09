#ifndef FOUNDATION_XSIMPLETYPES
#define FOUNDATION_XSIMPLETYPES
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

#include "x.h"
// Platform specific types:
//Design note: Its OK to use int for general loop variables and temps.

#ifdef _WIN32
	typedef __int64				xI64;
	typedef signed int			xI32;
	typedef signed short		xI16;
	typedef signed char			xI8;

	typedef unsigned __int64	xU64;
	typedef unsigned int		xU32;
	typedef unsigned short		xU16;
	typedef unsigned char		xU8;

	typedef float				xF32;
	typedef double				xF64;
		
#elif __linux__
	typedef long long			xI64;
	typedef signed int			xI32;
	typedef signed short		xI16;
	typedef signed char			xI8;

	typedef unsigned long long	xU64;
	typedef unsigned int		xU32;
	typedef unsigned short		xU16;
	typedef unsigned char		xU8;

	typedef float				xF32;
	typedef double				xF64;

#elif __APPLE__
	typedef long long			xI64;
	typedef signed int			xI32;
	typedef signed short		xI16;
	typedef signed char			xI8;

	typedef unsigned long long	xU64;
	typedef unsigned int		xU32;
	typedef unsigned short		xU16;
	typedef unsigned char		xU8;

	typedef float				xF32;
	typedef double				xF64;

#elif __ANDROID__

    typedef long long            xI64;
    typedef signed int            xI32;
    typedef signed short        xI16;
    typedef signed char            xI8;

    typedef unsigned long long    xU64;
    typedef unsigned int        xU32;
    typedef unsigned short        xU16;
    typedef unsigned char        xU8;

    typedef float                xF32;
    typedef double                xF64;

#elif __CELLOS_LV2__
	typedef long long			xI64;
	typedef signed int			xI32;
	typedef signed short		xI16;
	typedef signed char			xI8;

	typedef unsigned long long	xU64;
	typedef unsigned int		xU32;
	typedef unsigned short		xU16;
	typedef unsigned char		xU8;

	typedef float				xF32;
	typedef double				xF64;

#elif _XBOX
	typedef __int64				xI64;
	typedef signed int			xI32;
	typedef signed short		xI16;
	typedef signed char			xI8;

	typedef unsigned __int64	xU64;
	typedef unsigned int		xU32;
	typedef unsigned short		xU16;
	typedef unsigned char		xU8;

	typedef float				xF32;
	typedef double				xF64;

#else
	#error Unknown platform!
#endif

union xU32F32
{
	xU32 u;
	xF32 f;
};


	// Type ranges
	#define	X_MAX_I8			0x7f			//max possible sbyte value
	#define	X_MIN_I8			0x80			//min possible sbyte value
	#define	X_MAX_U8			0xff			//max possible ubyte value
	#define	X_MIN_U8			0x00			//min possible ubyte value
	#define	X_MAX_I16			0x7fff			//max possible sword value
	#define	X_MIN_I16			0x8000			//min possible sword value
	#define	X_MAX_U16			0xffff			//max possible uword value
	#define	X_MIN_U16			0x0000			//min possible uword value
	#define	X_MAX_I32			0x7fffffff		//max possible sdword value
	#define	X_MIN_I32			0x80000000		//min possible sdword value
	#define	X_MAX_U32			0xffffffff		//max possible udword value
	#define	X_MIN_U32			0x00000000		//min possible udword value
	#define	X_MAX_F32			FLT_MAX			//max possible float value
	#define	X_MIN_F32			(-FLT_MAX)		//min possible float value
	#define	X_MAX_F64			DBL_MAX			//max possible double value
	#define	X_MIN_F64			(-DBL_MAX)		//min possible double value

	#define X_EPS_F32			FLT_EPSILON		//smallest number not zero
	#define X_EPS_F64			DBL_EPSILON		//smallest number not zero

	#define X_IEEE_1_0			0x3f800000		//integer representation of 1.0
	#define X_IEEE_255_0		0x437f0000		//integer representation of 255.0
	#define X_IEEE_MAX_F32		0x7f7fffff		//integer representation of MAX_XFLOAT
	#define X_IEEE_MIN_F32		0xff7fffff		//integer representation of MIN_XFLOAT

	typedef int	X_BOOL;
	#define X_FALSE			0
	#define X_TRUE				1

	#define	X_MIN(a, b)		((a) < (b) ? (a) : (b))			//Returns the min value between a and b
	#define	X_MAX(a, b)		((a) > (b) ? (a) : (b))			//Returns the max value between a and b

 /** @} */
#endif
//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND
