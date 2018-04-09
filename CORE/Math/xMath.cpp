#include "xMath.h"



  bool xMath::equals(xF32 a,xF32 b,xF32 eps)
{
	const xF32 diff = xMath::abs(a - b);
	return (diff < eps);
}

  bool xMath::equals(xF64 a,xF64 b,xF64 eps)
{
	const xF64 diff = xMath::abs(a - b);
	return (diff < eps);
}

  xF32 xMath::floor(xF32 a)
{
	return ::floorf(a);
}

  xF64 xMath::floor(xF64 a)
{
	return ::floor(a);
}

  xF32 xMath::ceil(xF32 a)
{
	return ::ceilf(a);
}

  xF64 xMath::ceil(xF64 a)
{
	return ::ceil(a);
}

  xI32 xMath::trunc(xF32 a)
{
	return (xI32) a;	// ### PT: this actually depends on FPU settings
}

  xI32 xMath::trunc(xF64 a)
{
	return (xI32) a;	// ### PT: this actually depends on FPU settings
}

  xF32 xMath::abs(xF32 a)
{
	return ::fabsf(a);
}

  xF64 xMath::abs(xF64 a)
{
	return ::fabs(a);
}

  xI32 xMath::abs(xI32 a)
{
	return ::abs(a);
}

  xF32 xMath::sign(xF32 a)
{
	return (a >= 0.0f) ? 1.0f : -1.0f;
}

  xF64 xMath::sign(xF64 a)
{
	return (a >= 0.0) ? 1.0 : -1.0;
}

  xI32 xMath::sign(xI32 a)
{
	return (a >= 0) ? 1 : -1;
}

  xF32 xMath::max(xF32 a,xF32 b)
{
#ifdef _XBOX
	return (xF32)fpmax(a, b);
#else
	return (a < b) ? b : a;
#endif
}

  xF64 xMath::max(xF64 a,xF64 b)
{
#ifdef _XBOX
	return (xF64)fpmax(a, b);
#else
	return (a < b) ? b : a;
#endif
}

  xI32 xMath::max(xI32 a,xI32 b)
{
	return (a < b) ? b : a;
}

  xU32 xMath::max(xU32 a,xU32 b)
{
	return (a < b) ? b : a;
}

  xU16 xMath::max(xU16 a,xU16 b)
{
	return (a < b) ? b : a;
}

  xF32 xMath::min(xF32 a,xF32 b)
{
#ifdef _XBOX
	return (xF32)fpmin(a, b);
#else
	return (a < b) ? a : b;
#endif
}

  xF64 xMath::min(xF64 a,xF64 b)
{
#ifdef _XBOX
	return (xF64)fpmin(a, b);
#else
	return (a < b) ? a : b;
#endif
}

  xI32 xMath::min(xI32 a,xI32 b)
{
	return (a < b) ? a : b;
}

  xU32 xMath::min(xU32 a,xU32 b)
{
	return (a < b) ? a : b;
}

  xU16 xMath::min(xU16 a,xU16 b)
{
	return (a < b) ? a : b;
}

  xF32 xMath::mod(xF32 x, xF32 y)
{
	return (xF32)::fmod(x,y);
}

  xF64 xMath::mod(xF64 x, xF64 y)
{
	return ::fmod(x,y);
}

  xF32 xMath::clamp(xF32 v, xF32 hi, xF32 low)
{
	if (v > hi) 
		return hi;
	else if (v < low) 
		return low;
	else
		return v;
}

  xF64 xMath::clamp(xF64 v, xF64 hi, xF64 low)
{
	if (v > hi) 
		return hi;
	else if (v < low) 
		return low;
	else
		return v;
}

  xU32 xMath::clamp(xU32 v, xU32 hi, xU32 low)
{
	if (v > hi) 
		return hi;
	else if (v < low) 
		return low;
	else
		return v;
}

  xI32 xMath::clamp(xI32 v, xI32 hi, xI32 low)
{
	if (v > hi) 
		return hi;
	else if (v < low) 
		return low;
	else
		return v;
}

#ifdef _XBOX
  xF32 xMath::sqrt(xF32 a)
{
	return __fsqrts(a);
}

  xF64 xMath::sqrt(xF64 a)
{
	return __fsqrt(a);
}
#else
  xF32 xMath::sqrt(xF32 a)
{
	return ::sqrtf(a);
}

  xF64 xMath::sqrt(xF64 a)
{
	return ::sqrt(a);
}
#endif

  xF32 xMath::recipSqrt(xF32 a)
{
	return 1.0f/::sqrtf(a);
}

  xF64 xMath::recipSqrt(xF64 a)
{
	return 1.0/::sqrt(a);
}


  xF32 xMath::pow(xF32 x, xF32 y)
{
	return ::powf(x,y);
}

  xF64 xMath::pow(xF64 x, xF64 y)
{
	return ::pow(x,y);
}

  xF32 xMath::exp(xF32 a)
{
	return ::expf(a);
}

  xF64 xMath::exp(xF64 a)
{
	return ::exp(a);
}

  xF32 xMath::logE(xF32 a)
{
	return ::logf(a);
}

  xF64 xMath::logE(xF64 a)
{
	return ::log(a);
}

  xF32 xMath::log2(xF32 a)
{
	const xF32 ln2 = (xF32)0.693147180559945309417;
    return ::logf(a) / ln2;
}

  xF64 xMath::log2(xF64 a)
{
	const xF64 ln2 = (xF64)0.693147180559945309417;
    return ::log(a) / ln2;
}

  xF32 xMath::log10(xF32 a)
{
	return (xF32)::log10(a);
}

  xF64 xMath::log10(xF64 a)
{
	return ::log10(a);
}

  xF32 xMath::degToRad(xF32 a)
{
	return (xF32)0.01745329251994329547 * a;
}

  xF64 xMath::degToRad(xF64 a)
{
	return (xF64)0.01745329251994329547 * a;
}

  xF32 xMath::radToDeg(xF32 a)
{
	return (xF32)57.29577951308232286465 * a;
}

  xF64 xMath::radToDeg(xF64 a)
{
	return (xF64)57.29577951308232286465 * a;
}

  xF32 xMath::sin(xF32 a)
{
	return ::sinf(a);
}

  xF64 xMath::sin(xF64 a)
{
	return ::sin(a);
}

  xF32 xMath::cos(xF32 a)
{
	return ::cosf(a);
}

  xF64 xMath::cos(xF64 a)
{
	return ::cos(a);
}

// Calling fsincos instead of fsin+fcos
  void xMath::sinCos(xF32 f, xF32& s, xF32& c)
{
#ifdef WIN32
#ifndef X64
	xF32 localCos, localSin;
	xF32 local = f;
	_asm	fld		local
	_asm	fsincos
	_asm	fstp	localCos
	_asm	fstp	localSin
	c = localCos;
	s = localSin;
#define ___TMP_D
#endif
#endif
#ifndef ___TMP_D
	c = cosf(f);
	s = sinf(f);
#endif
}

  void xMath::sinCos(xF64 a, xF64 & s, xF64 & c)
{
	s = ::sin(a);
	c = ::cos(a);
}

  xF32 xMath::tan(xF32 a)
{
	return ::tanf(a);
}

  xF64 xMath::tan(xF64 a)
{
	return ::tan(a);
}

  xF32 xMath::asin(xF32 f)
{
	// Take care of FPU inaccuracies
	if(f>=1.0f)	return (xF32)xHalfPiF32;
	if(f<=-1.0f)return -(xF32)xHalfPiF32;
	return ::asinf(f);
}

  xF64 xMath::asin(xF64 f)
{
	// Take care of FPU inaccuracies
	if(f>=1.0)	return (xF32)xHalfPiF64;
	if(f<=-1.0)	return -(xF32)xHalfPiF64;
	return ::asin(f);
}

  xF32 xMath::acos(xF32 f)
{
	// Take care of FPU inaccuracies
	if(f>=1.0f)	return 0.0f;
	if(f<=-1.0f)return (xF32)xPiF32;
	return ::acosf(f);
}

  xF64 xMath::acos(xF64 f)
{
	// Take care of FPU inaccuracies
	if(f>=1.0)	return 0.0;
	if(f<=-1.0)	return (xF64)xPiF64;
	return ::acos(f);
}

  xF32 xMath::atan(xF32 a)
{
	return ::atanf(a);
}

  xF64 xMath::atan(xF64 a)
{
	return ::atan(a);
}

  xF32 xMath::atan2(xF32 x, xF32 y)
{
	return ::atan2f(x,y);
}

  xF64 xMath::atan2(xF64 x, xF64 y)
{
	return ::atan2(x,y);
}

  xF32 xMath::rand(xF32 a,xF32 b)
{
	const xF32 r = (xF32)::rand()/((xF32)RAND_MAX+1);
	return r*(b-a) + a;
}

  xI32 xMath::rand(xI32 a,xI32 b)
{
	return a + (xI32)(::rand()%(b-a));
}

/*
 --------------------------------------------------------------------
 lookup2.c, by Bob Jenkins, December 1996, Public Domain.
 hash(), hash2(), hash3, and mix() are externally useful functions.
 Routines to test the hash are included if SELF_TEST is defined.
 You can use this free for any purpose.  It has no warranty.
 --------------------------------------------------------------------
 --------------------------------------------------------------------
 mix -- mix 3 32-bit values reversibly.
 For every delta with one or two bit set, and the deltas of all three
 high bits or all three low bits, whether the original value of a,b,c
 is almost all zero or is uniformly distributed,
 * If mix() is run forward or backward, at least 32 bits in a,b,c
 have at least 1/4 probability of changing.
 * If mix() is run forward, every bit of c will change between 1/3 and
 2/3 of the time.  (Well, 22/100 and 78/100 for some 2-bit deltas.)
 mix() was built out of 36 single-cycle latency instructions in a 
 structure that could supported 2x parallelism, like so:
 a -= b; 
 a -= c; x = (c>>13);
 b -= c; a ^= x;
 b -= a; x = (a<<8);
 c -= a; b ^= x;
 c -= b; x = (b>>13);
 ...
 Unfortunately, superscalar Pentiums and Sparcs can't take advantage 
 of that parallelism.  They've also turned some of those single-cycle
 latency instructions into multi-cycle latency instructions.  Still,
 this is the fastest good hash I could find.  There were about 2^^68
 to choose from.  I only looked at a billion or so.
 --------------------------------------------------------------------
 */
#define X_HASH_MIX(a,b,c) \
{ \
a -= b; a -= c; a ^= (c>>13); \
b -= c; b -= a; b ^= (a<<8); \
c -= a; c -= b; c ^= (b>>13); \
a -= b; a -= c; a ^= (c>>12);  \
b -= c; b -= a; b ^= (a<<16); \
c -= a; c -= b; c ^= (b>>5); \
a -= b; a -= c; a ^= (c>>3);  \
b -= c; b -= a; b ^= (a<<10); \
c -= a; c -= b; c ^= (b>>15); \
}

/*
 --------------------------------------------------------------------
 This works on all machines.  hash2() is identical to hash() on 
 little-endian machines, except that the length has to be measured
 in ub4s instead of bytes.  It is much faster than hash().  It 
 requires
 -- that the key be an array of ub4's, and
 -- that all your machines have the same endianness, and
 -- that the length be the number of ub4's in the key
 --------------------------------------------------------------------
 */
  xU32 xMath::hash(const xU32 *k, xU32 length)
//register ub4 *k;        /* the key */
//register ub4  length;   /* the length of the key, in ub4s */
{
	xU32 a,b,c,len;
	
	/* Set up the internal state */
	len = length;
	a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
	c = 0;           /* the previous hash value */
	
	/*---------------------------------------- handle most of the key */
	while (len >= 3)
	{
		a += k[0];
		b += k[1];
		c += k[2];
		X_HASH_MIX(a,b,c);
		k += 3; len -= 3;
	}
	
	/*-------------------------------------- handle the last 2 ub4's */
	c += length;
	switch(len)              /* all the case statements fall through */
	{
			/* c is reserved for the length */
		case 2 : b+=k[1];
		case 1 : a+=k[0];
			/* case 0: nothing left to add */
	}
	X_HASH_MIX(a,b,c);
	/*-------------------------------------------- report the result */
	return c;
}
#undef X_HASH_MIX

  int xMath::hash32(int key)
{
	key += ~(key << 15);
	key ^=  (key >> 10);
	key +=  (key << 3);
	key ^=  (key >> 6);
	key += ~(key << 11);
	key ^=  (key >> 16);
	return key;
}


  bool xMath::isFinite(xF32 f)
{
#if defined(_MSC_VER)
	return (0 == ((_FPCLASS_SNAN | _FPCLASS_QNAN | _FPCLASS_NINF | _FPCLASS_PINF) & _fpclass(f) ));
#elif defined(__CELLOS_LV2__)
	return (!(isnan(f)||isinf(f)));
#else
	return true;
#endif
	
}

  bool xMath::isFinite(xF64 f)
{
#if defined(_MSC_VER)
	return (0 == ((_FPCLASS_SNAN | _FPCLASS_QNAN | _FPCLASS_NINF | _FPCLASS_PINF) & _fpclass(f) ));
#elif defined(__CELLOS_LV2__)
	return (!(isnan(f)||isinf(f)));
#else
	return true;
#endif
}

