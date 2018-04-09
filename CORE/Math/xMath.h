#ifndef FOUNDATION_XMATH
#define FOUNDATION_XMATH
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

#include <math.h>
#include <float.h>
#include <stdlib.h>	//for rand()

#include "x.h"

#ifdef log2
#undef log2
#endif

//constants
static const xF64 xPiF64		= 3.141592653589793;
static const xF64 xHalfPiF64	= 1.57079632679489661923;
static const xF64 xTwoPiF64	= 6.28318530717958647692;
static const xF64 xInvPiF64	= 0.31830988618379067154;
//we can get bad range checks if we use double prec consts to check single prec results.
static const xF32 xPiF32		= 3.141592653589793f;
static const xF32 xHalfPiF32	= 1.57079632679489661923f;
static const xF32 xTwoPiF32	= 6.28318530717958647692f;
static const xF32 xInvPiF32	= 0.31830988618379067154f;


#if defined(min) || defined(max)
#error Error: min or max is #defined, probably in <windows.h>.  Put #define NOMINMAX before including windows.h to suppress windows global min,max macros.
#endif

/**
\brief Static class with stateless scalar math routines.
*/
class xMath
	{
	public:

// Type conversion and rounding
		/**
		\brief Returns true if the two numbers are within eps of each other.
		*/
		   static bool equals(xF32,xF32,xF32 eps);

		/**
		\brief Returns true if the two numbers are within eps of each other.
		*/
		   static bool equals(xF64,xF64,xF64 eps);
		/**
		\brief The floor function returns a floating-point value representing the largest integer that is less than or equal to x.
		*/
		   static xF32 floor(xF32);
		/**
		\brief The floor function returns a floating-point value representing the largest integer that is less than or equal to x.
		*/
		   static xF64 floor(xF64);


		/**
		\brief The ceil function returns a single value representing the smallest integer that is greater than or equal to x. 
		*/
		   static xF32 ceil(xF32);
		/**
		\brief The ceil function returns a double value representing the smallest integer that is greater than or equal to x. 
		*/
		   static xF64 ceil(xF64);

		/**
		\brief Truncates the float to an integer.
		*/
		   static xI32 trunc(xF32);
		/**
		\brief Truncates the double precision float to an integer.
		*/
		   static xI32 trunc(xF64);


		/**
		\brief abs returns the absolute value of its argument. 
		*/
		   static xF32 abs(xF32);
		/**
		\brief abs returns the absolute value of its argument. 
		*/
		   static xF64 abs(xF64);
		/**
		\brief abs returns the absolute value of its argument. 
		*/
		   static xI32 abs(xI32);


		/**
		\brief sign returns the sign of its argument. The sign of zero is undefined.
		*/
		   static xF32 sign(xF32);
		/**
		\brief sign returns the sign of its argument. The sign of zero is undefined.
		*/
		   static xF64 sign(xF64);
		/**
		\brief sign returns the sign of its argument. The sign of zero is undefined.
		*/
		   static xI32 sign(xI32);


		/**
		\brief The return value is the greater of the two specified values. 
		*/
		   static xF32 max(xF32,xF32);
		/**
		\brief The return value is the greater of the two specified values. 
		*/
		   static xF64 max(xF64,xF64);
		/**
		\brief The return value is the greater of the two specified values. 
		*/
		   static xI32 max(xI32,xI32);
		/**
		\brief The return value is the greater of the two specified values. 
		*/
		   static xU32 max(xU32,xU32);
		/**
		\brief The return value is the greater of the two specified values. 
		*/
		   static xU16 max(xU16,xU16);
		
		
		/**
		\brief The return value is the lesser of the two specified values. 
		*/
		   static xF32 min(xF32,xF32);
		/**
		\brief The return value is the lesser of the two specified values. 
		*/
		   static xF64 min(xF64,xF64);
		/**
		\brief The return value is the lesser of the two specified values. 
		*/
		   static xI32 min(xI32,xI32);
		/**
		\brief The return value is the lesser of the two specified values. 
		*/
		   static xU32 min(xU32,xU32);
		/**
		\brief The return value is the lesser of the two specified values. 
		*/
		   static xU16 min(xU16,xU16);
		
		/**
		\brief mod returns the floating-point remainder of x / y. 
		
		If the value of y is 0.0, mod returns a quiet NaN.
		*/
		   static xF32 mod(xF32 x, xF32 y);
		/**
		\brief mod returns the floating-point remainder of x / y. 
		
		If the value of y is 0.0, mod returns a quiet NaN.
		*/
		   static xF64 mod(xF64 x, xF64 y);

		/**
		\brief Clamps v to the range [hi,lo]
		*/
		   static xF32 clamp(xF32 v, xF32 hi, xF32 low);
		/**
		\brief Clamps v to the range [hi,lo]
		*/
		   static xF64 clamp(xF64 v, xF64 hi, xF64 low);
		/**
		\brief Clamps v to the range [hi,lo]
		*/
		   static xU32 clamp(xU32 v, xU32 hi, xU32 low);
		/**
		\brief Clamps v to the range [hi,lo]
		*/
		   static xI32 clamp(xI32 v, xI32 hi, xI32 low);

		//!powers
		/**
		\brief Square root.
		*/
		   static xF32 sqrt(xF32);
		/**
		\brief Square root.
		*/
		   static xF64 sqrt(xF64);
		
		/**
		\brief reciprocal square root.
		*/
		   static xF32 recipSqrt(xF32);
		/**
		\brief reciprocal square root.
		*/
		   static xF64 recipSqrt(xF64);
		
		/**
		\brief Calculates x raised to the power of y.
		*/
		   static xF32 pow(xF32 x, xF32 y);
		/**
		\brief Calculates x raised to the power of y.
		*/
		   static xF64 pow(xF64 x, xF64 y);
		
		
		/**
		\brief Calculates e^n
		*/
		   static xF32 exp(xF32);
		/**
		\brief Calculates e^n
		*/
		   static xF64 exp(xF64);
		
		/**
		\brief Calculates logarithms.
		*/
		   static xF32 logE(xF32);
		/**
		\brief Calculates logarithms.
		*/
		   static xF64 logE(xF64);
		/**
		\brief Calculates logarithms.
		*/
		   static xF32 log2(xF32);
		/**
		\brief Calculates logarithms.
		*/
		   static xF64 log2(xF64);
		/**
		\brief Calculates logarithms.
		*/
		   static xF32 log10(xF32);
		/**
		\brief Calculates logarithms.
		*/
		   static xF64 log10(xF64);

		//!trigonometry -- all angles are in radians.
		
		/**
		\brief Converts degrees to radians.
		*/
		   static xF32 degToRad(xF32);
		/**
		\brief Converts degrees to radians.
		*/
		   static xF64 degToRad(xF64);

		/**
		\brief Converts radians to degrees.
		*/
		   static xF32 radToDeg(xF32);
		/**
		\brief Converts radians to degrees.
		*/
		   static xF64 radToDeg(xF64);

		/**
		\brief Sine of an angle.

		<b>Unit:</b> Radians
		*/
		   static xF32 sin(xF32);
		/**
		\brief Sine of an angle.

		<b>Unit:</b> Radians
		*/
		   static xF64 sin(xF64);
		
		/**
		\brief Cosine of an angle.

		<b>Unit:</b> Radians
		*/
		   static xF32 cos(xF32);
		/**
		\brief Cosine of an angle.

		<b>Unit:</b> Radians
		*/
		   static xF64 cos(xF64);

		/**
		\brief Computes both the sin and cos.

		<b>Unit:</b> Radians
		*/
		   static void sinCos(xF32, xF32 & sin, xF32 & cos);

		/**
		\brief Computes both the sin and cos.

		<b>Unit:</b> Radians
		*/
		   static void sinCos(xF64, xF64 & sin, xF64 & cos);


		/**
		\brief Tangent of an angle.

		<b>Unit:</b> Radians
		*/
		   static xF32 tan(xF32);
		/**
		\brief Tangent of an angle.

		<b>Unit:</b> Radians
		*/
		   static xF64 tan(xF64);
		
		/**
		\brief Arcsine.
		
		Returns angle between -PI/2 and PI/2 in radians

		<b>Unit:</b> Radians
		*/
		   static xF32 asin(xF32);
		/**
		\brief Arcsine.
		
		Returns angle between -PI/2 and PI/2 in radians

		<b>Unit:</b> Radians
		*/
		   static xF64 asin(xF64);
		
		/**
		\brief Arccosine.
		
		Returns angle between 0 and PI in radians

		<b>Unit:</b> Radians
		*/
		   static xF32 acos(xF32);
		
		/**
		\brief Arccosine.
		
		Returns angle between 0 and PI in radians

		<b>Unit:</b> Radians
		*/
		   static xF64 acos(xF64);
		
		/**
		\brief ArcTangent.
		
		Returns angle between -PI/2 and PI/2 in radians

		<b>Unit:</b> Radians
		*/
		   static xF32 atan(xF32);
		/**
		\brief ArcTangent.
		
		Returns angle between -PI/2 and PI/2 in radians

		<b>Unit:</b> Radians
		*/
		   static xF64 atan(xF64);

		/**
		\brief Arctangent of (x/y) with correct sign.
		
		Returns angle between -PI and PI in radians

		<b>Unit:</b> Radians
		*/
		   static xF32 atan2(xF32 x, xF32 y);
		/**
		\brief Arctangent of (x/y) with correct sign.
		
		Returns angle between -PI and PI in radians

		<b>Unit:</b> Radians
		*/
		   static xF64 atan2(xF64 x, xF64 y);

		//random numbers
		
		/**
		\brief uniform random number in [a,b]
		*/
		   static xF32 rand(xF32 a,xF32 b);
		
		/**
		\brief uniform random number in [a,b]
		*/
		   static xI32 rand(xI32 a,xI32 b);

		/**
		\brief hashing: hashes an array of n 32 bit values	to a 32 bit value.
		
		Because the output bits are uniformly distributed, the caller may mask
		off some of the bits to index into a hash table	smaller than 2^32.
		*/
		   static xU32 hash(const xU32 * array, xU32 n);

		/**
		\brief hash32
		*/
		   static int hash32(int);

		/**
		\brief returns true if the passed number is a finite floating point number as opposed to INF, NAN, etc.
		*/
		   static bool isFinite(xF32 x);
		
		/**
		\brief returns true if the passed number is a finite floating point number as opposed to INF, NAN, etc.
		*/
		   static bool isFinite(xF64 x);
	};
 /** @} */
#endif
//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND
