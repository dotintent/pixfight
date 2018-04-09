#ifndef FOUNDATION_XVEC3
#define FOUNDATION_XVEC3
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



#include "xf.h"
#include "xMath.h"

class  xMat33;


/**
\brief Enum to classify an axis.
*/
	enum xAxisType
	{
		X_AXIS_PLUS_X,
		X_AXIS_MINUS_X,
		X_AXIS_PLUS_Y,
		X_AXIS_MINUS_Y,
		X_AXIS_PLUS_Z,
		X_AXIS_MINUS_Z,
		X_AXIS_ARBITRARY
	};


class xVec3;

/** \cond Exclude from documentation */
typedef struct _x3F32
{
	xReal x, y, z;
	
	  const _x3F32& operator=(const xVec3& d);
} x3F32;
/** \endcond */

/**
\brief 3 Element vector class.

This is a vector class with public data members.
This is not nice but it has become such a standard that hiding the xyz data members
makes it difficult to reuse external code that assumes that these are public in the library.
The vector class can be made to use float or double precision by appropriately defining xReal.
This has been chosen as a cleaner alternative to a template class.
*/
class xVec3
	{
	public:
	//!Constructors

	/**
	\brief default constructor leaves data uninitialized.
	*/
	  xVec3();

	/**
	\brief Assigns scalar parameter to all elements.
	
	Useful to initialize to zero or one.

	\param[in] a Value to assign to elements.
	*/
	explicit   xVec3(xReal a);

	/**
	\brief Initializes from 3 scalar parameters.

	\param[in] nx Value to initialize X component.
	\param[in] ny Value to initialize Y component.
	\param[in] nz Value to initialize Z component.
	*/
	  xVec3(xReal nx, xReal ny, xReal nz);
	
	/**
	\brief Initializes from x3F32 data type.

	\param[in] a Value to initialize with.
	*/
	  xVec3(const x3F32 &a);

	/**
	\brief Initializes from an array of scalar parameters.

	\param[in] v Value to initialize with.
	*/
	  xVec3(const xReal v[]);

	/**
	\brief Copy constructor.
	*/
	  xVec3(const xVec3& v);

	/**
	\brief Assignment operator.
	*/
	  const xVec3& operator=(const xVec3&);

	/**
	\brief Assignment operator.
	*/
	  const xVec3& operator=(const x3F32&);

	/**
	\brief Access the data as an array.

	\return Array of 3 floats.
	*/
	  const xReal *get() const;
	
	/**
	\brief Access the data as an array.

	\return Array of 3 floats.
	*/
	  xReal* get();

	/**
	\brief writes out the 3 values to dest.

	\param[out] dest Array to write elements to.
	*/
	  void get(xF32 * dest) const;

	/**
	\brief writes out the 3 values to dest.
	*/
	  void get(xF64 * dest) const;

	  xReal& operator[](int index);
	  xReal  operator[](int index) const;

	//Operators
	/**
	\brief true if all the members are smaller.
	*/
	  bool operator< (const xVec3&) const;
	/**
	\brief returns true if the two vectors are exactly equal.

	use equal() to test with a tolerance.
	*/
	  bool operator==(const xVec3&) const;
	/**
	\brief returns true if the two vectors are exactly unequal.

	use !equal() to test with a tolerance.
	*/
	  bool operator!=(const xVec3&) const;

/*	  const xVec3 &operator +=(const xVec3 &);
	  const xVec3 &operator -=(const xVec3 &);
	  const xVec3 &operator *=(xReal);
	  const xVec3 &operator /=(xReal);
*/
//Methods
	  void  set(const xVec3 &);

//legacy methods:
	  void setx(const xReal & d);
	  void sety(const xReal & d);
	  void setz(const xReal & d);

	/**
	\brief this = -a
	*/
	  void  setNegative(const xVec3 &a);

	/**
	\brief this = -this
	*/
	  void  setNegative();

	/**
	\brief reads 3 consecutive values from the ptr passed
	*/
	  void  set(const xF32 *);

	/**
	\brief reads 3 consecutive values from the ptr passed
	*/
	  void  set(const xF64 *);
	  void  set(xReal, xReal, xReal);
	  void  set(xReal);

	  void  zero();
	
	/**
	\brief tests for exact zero vector
	*/
	  X_BOOL isZero()	const
		{
		if((x != 0.0f) || (y != 0.0f) || (z != 0.0f))	return X_FALSE;
		return X_TRUE;
		}

	  void  setPlusInfinity();
	  void  setMinusInfinity();

	  void lerp( xVec3 &a, xVec3 &b, float coef )
	{
		*this = a + ( b - a )*coef;
	}

	operator xReal *()
	{
		return get();
	}

    operator const xReal *() const
	{
		return get();
	}

	/**
	\brief this = element wise min(this,other)
	*/
	  void min(const xVec3 &);
	/**
	\brief this = element wise max(this,other)
	*/
	  void max(const xVec3 &);

	/**
	\brief this = a + b
	*/
	  void  add(const xVec3 & a, const xVec3 & b);
	/**
	\brief this = a - b
	*/
	  void  subtract(const xVec3 &a, const xVec3 &b);
	/**
	\brief this = s * a;
	*/
	  void  multiply(xReal s,  const xVec3 & a);

	/**
	\brief this[i] = a[i] * b[i], for all i.
	*/
	  void  arrayMultiply(const xVec3 &a, const xVec3 &b);


	/**
	\brief this = s * a + b;
	*/
	  void  multiplyAdd(xReal s, const xVec3 & a, const xVec3 & b);

	/**
	\brief normalizes the vector
	*/
	  xReal normalize();

	/**
	\brief sets the vector's magnitude
	*/
	  void	setMagnitude(xReal);

	/**
	\brief snaps to closest axis
	*/
	  xU32			closestAxis()	const;

	/**
	\brief snaps to closest axis
	*/
	  xAxisType	snapToClosestAxis();

//const methods
	/**
	\brief returns true if all 3 elems of the vector are finite (not NAN or INF, etc.)
	*/
	  bool isFinite() const;

	/**
	\brief returns the scalar product of this and other.
	*/
	  xReal dot(const xVec3 &other) const;

	/**
	\brief compares orientations (more readable, user-friendly function)
	*/
	  bool sameDirection(const xVec3 &) const;

	/**
	\brief returns the magnitude
	*/
	  xReal magnitude() const;

	/**
	\brief returns the squared magnitude
	
	Avoids calling sqrt()!
	*/
	  xReal magnitudeSquared() const;

	/**
	\brief returns (this - other).magnitude();
	*/
	  xReal distance(const xVec3 &) const;

	/**
	\brief returns (this - other).magnitudeSquared();
	*/
	  xReal distanceSquared(const xVec3 &v) const;

	/**
	\brief this = left x right
	*/
	  void cross(const xVec3 &left, const xVec3 & right);

	/**
	\brief Stuff magic values in the point, marking it as explicitly not used.
	*/
	  void setNotUsed();

	/**
	\brief Checks the point is marked as not used
	*/
	X_BOOL isNotUsed() const;

	/**
	\brief returns true if this and arg's elems are within epsilon of each other.
	*/
	  bool equals(const xVec3 &, xReal epsilon) const;

	/**
	\brief negation
	*/
	xVec3 operator -() const;
	/**
	\brief vector addition
	*/
	xVec3 operator +(const xVec3 & v) const;
	/**
	\brief vector difference
	*/
	xVec3 operator -(const xVec3 & v) const;
	/**
	\brief scalar post-multiplication
	*/
	xVec3 operator *(xReal f) const;
	/**
	\brief scalar division
	*/
	xVec3 operator /(xReal f) const;
	/**
	\brief vector addition
	*/
	xVec3&operator +=(const xVec3& v);
	/**
	\brief vector difference
	*/
	xVec3&operator -=(const xVec3& v);
	/**
	\brief scalar multiplication
	*/
	xVec3&operator *=(xReal f);

	xVec3&operator *=(xVec3 v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}
	/**
	\brief scalar division
	*/
	xVec3&operator /=(xReal f);
	/**
	\brief cross product
	*/
	xVec3 cross(const xVec3& v) const;

	/**
	\brief cross product
	*/
	xVec3 operator^(const xVec3& v) const;
	/**
	\brief dot product
	*/
	xReal      operator|(const xVec3& v) const;
        
	xReal x,y,z;
	};

extern xVec3 operator *(xReal f, const xVec3& v);

/** \cond Exclude from documentation */
 /** @} */
#endif
//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND
