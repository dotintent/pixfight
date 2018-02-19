#ifndef FOUNDATION_xQuatT
#define FOUNDATION_xQuatT
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
#include "xVec3.h"

/**
\brief This is a quaternion class. For more information on quaternion mathematics
consult a mathematics source on complex numbers.
 
*/

class xQuat
	{
	public:
	/**
	\brief Default constructor, does not do any initialization.
	*/
	X_INLINE xQuat();

	/**
	\brief Copy constructor.
	*/
	X_INLINE xQuat(const xQuat&);

	/**
	\brief copies xyz elements from v, and scalar from w (defaults to 0).
	*/
	X_INLINE xQuat(const xVec3& v, xReal w = 0);

	/**
	\brief creates from angle-axis representation.

	note that if Angle > 360 the resulting rotation is Angle mod 360.
	
	<b>Unit:</b> Degrees
	*/
	X_INLINE xQuat(const xReal angle, const xVec3 & axis);

	/**
	\brief Creates from orientation matrix.

	\param[in] m Rotation matrix to extract quaternion from.
	*/
	X_INLINE xQuat(const class xMat33 &m); /* defined in xMat33.h */


	/**
	\brief Set the quaternion to the identity rotation.
	*/
	X_INLINE void id();

	/**
	\brief Test if the quaternion is the identity rotation.
	*/
	X_INLINE bool isIdentityRotation() const;

	//setting:

	/**
	\brief Set the members of the quaternion, in order WXYZ
	*/
	X_INLINE void setWXYZ(xReal w, xReal x, xReal y, xReal z);

	/**
	\brief Set the members of the quaternion, in order XYZW
	*/
	X_INLINE void setXYZW(xReal x, xReal y, xReal z, xReal w);

	/**
	\brief Set the members of the quaternion, in order WXYZ
	*/
	X_INLINE void setWXYZ(const xReal *);

	/**
	\brief Set the members of the quaternion, in order XYZW
	*/
	X_INLINE void setXYZW(const xReal *);

	X_INLINE xQuat& operator=  (const xQuat&);

	/**
	\brief Implicitly extends vector by a 0 w element.
	*/
	X_INLINE xQuat& operator=  (const xVec3&);

	X_INLINE void setx(const xReal& d);
	X_INLINE void sety(const xReal& d);
	X_INLINE void setz(const xReal& d);
	X_INLINE void setw(const xReal& d);

	X_INLINE void getWXYZ(xF32 *) const;
	X_INLINE void getXYZW(xF32 *) const;

	X_INLINE void getWXYZ(xF64 *) const;
	X_INLINE void getXYZW(xF64 *) const;

	/**
	\brief returns true if all elements are finite (not NAN or INF, etc.)
	*/
	X_INLINE bool isFinite() const;

	/**
	\brief sets to the quat [0,0,0,1]
	*/
	X_INLINE void zero();

	/**
	\brief creates a random unit quaternion.
	*/
	X_INLINE void random();
	/**
	\brief creates from angle-axis representation.

	Note that if Angle > 360 the resulting rotation is Angle mod 360.
	
	<b>Unit:</b> Degrees
	*/
	X_INLINE void fromAngleAxis(xReal angle, const xVec3 & axis);

	/**
	\brief Creates from angle-axis representation.

	Axis must be normalized!
	
	<b>Unit:</b> Radians
	*/
	X_INLINE void fromAngleAxisFast(xReal AngleRadians, const xVec3 & axis);

	/**
	\brief Sets this to the opposite rotation of this.
	*/
	X_INLINE void invert();

	/**
	\brief Fetches the Angle/axis given by the xQuat.

	<b>Unit:</b> Degrees
	*/
	X_INLINE void getAngleAxis(xReal& Angle, xVec3 & axis) const;

	/**
	\brief Gets the angle between this quat and the identity quaternion.

	<b>Unit:</b> Degrees
	*/
	X_INLINE xReal getAngle() const;

	/**
	\brief Gets the angle between this quat and the argument

	<b>Unit:</b> Degrees
	*/
	X_INLINE xReal getAngle(const xQuat &) const;

	/**
	\brief This is the squared 4D vector length, should be 1 for unit quaternions.
	*/
	X_INLINE xReal magnitudeSquared() const;

	/**
	\brief returns the scalar product of this and other.
	*/
	X_INLINE xReal dot(const xQuat &other) const;

	//modifiers:
	/**
	\brief maps to the closest unit quaternion.
	*/
	X_INLINE void normalize();

	/*
	\brief assigns its own conjugate to itself.

	\note for unit quaternions, this is the inverse.
	*/
	X_INLINE void conjugate();

	/**
	this = a * b
	*/
	X_INLINE void multiply(const xQuat& a, const xQuat& b);

	/**
	this = a * v
	v is interpreted as quat [xyz0]
	*/
	X_INLINE void multiply(const xQuat& a, const xVec3& v);

	/**
	this = slerp(t, a, b)
	*/
	X_INLINE void slerp(const xReal t, const xQuat& a, const xQuat& b);

	/**
	rotates passed vec by rot expressed by unit quaternion.  overwrites arg with the result.
	*/
	X_INLINE void rotate(xVec3 &) const;

	/**
	rotates passed vec by this (assumed unitary)
	*/
	X_INLINE const xVec3 rot(const xVec3 &) const;

	/**
	inverse rotates passed vec by this (assumed unitary)
	*/
	X_INLINE const xVec3 invRot(const xVec3 &) const;

	/**
	transform passed vec by this rotation (assumed unitary) and translation p
	*/
	X_INLINE const xVec3 transform(const xVec3 &v, const xVec3 &p) const;

	/**
	inverse rotates passed vec by this (assumed unitary)
	*/
	X_INLINE const xVec3 invTransform(const xVec3 &v, const xVec3 &p) const;


	/**
	rotates passed vec by opposite of rot expressed by unit quaternion.  overwrites arg with the result.
	*/
	X_INLINE void inverseRotate(xVec3 &) const;



	/**
	negates all the elements of the quat.  q and -q represent the same rotation.
	*/
	X_INLINE void negate();
	X_INLINE xQuat operator -() const; 

	X_INLINE xQuat& operator*= (const xQuat&);
	X_INLINE xQuat& operator+= (const xQuat&);
	X_INLINE xQuat& operator-= (const xQuat&);
	X_INLINE xQuat& operator*= (const xReal s);

	/** the quaternion elements */
    xReal x,y,z,w;

	/** quaternion multiplication */
	X_INLINE xQuat operator *(const xQuat &) const; 

	/** quaternion addition */
	X_INLINE xQuat operator +(const xQuat &) const; 

	/** quaternion subtraction */
	X_INLINE xQuat operator -(const xQuat &) const; 

	/** quaternion conjugate */
	X_INLINE xQuat operator !() const; 

    /* 
	ops we decided not to implement:
	bool  operator== (const xQuat&) const;
	xVec3  operator^  (const xQuat& r_h_s) const;//same as normal quat rot, but casts itself into a vector.  (doesn't compute w term)
	xQuat  operator*  (const xVec3& v) const;//implicitly extends vector by a 0 w element.
	xQuat  operator*  (const xReal Scale) const;
	*/

	friend class xMat33;
	private:
		X_INLINE xQuat(xReal ix, xReal iy, xReal iz, xReal iw);
	};




X_INLINE xQuat::xQuat()
	{
	//nothing
	}


X_INLINE xQuat::xQuat(const xQuat& q) : x(q.x), y(q.y), z(q.z), w(q.w)
	{
	}


X_INLINE xQuat::xQuat(const xVec3& v, xReal s)						// copy constructor, assumes w=0 
	{
	x = v.x;
	y = v.y;
	z = v.z;
	w = s;
	}


X_INLINE xQuat::xQuat(const xReal angle, const xVec3 & axis)				// creates a xQuat from an Angle axis -- note that if Angle > 360 the resulting rotation is Angle mod 360
	{
	fromAngleAxis(angle,axis);
	}


X_INLINE void xQuat::id()
	{
	x = xReal(0);
	y = xReal(0);
	z = xReal(0);
	w = xReal(1);
	}

X_INLINE  bool xQuat::isIdentityRotation() const
{
	return x==0 && y==0 && z==0 && fabsf(w)==1;
}


X_INLINE void xQuat::setWXYZ(xReal sw, xReal sx, xReal sy, xReal sz)
	{
	x = sx;
	y = sy;
	z = sz;
	w = sw;
	}


X_INLINE void xQuat::setXYZW(xReal sx, xReal sy, xReal sz, xReal sw)
	{
	x = sx;
	y = sy;
	z = sz;
	w = sw;
	}


X_INLINE void xQuat::setWXYZ(const xReal * d)
	{
	x = d[1];
	y = d[2];
	z = d[3];
	w = d[0];
	}


X_INLINE void xQuat::setXYZW(const xReal * d)
	{
	x = d[0];
	y = d[1];
	z = d[2];
	w = d[3];
	}


X_INLINE void xQuat::getWXYZ(xF32 *d) const
	{
	d[1] = (xF32)x;
	d[2] = (xF32)y;
	d[3] = (xF32)z;
	d[0] = (xF32)w;
	}


X_INLINE void xQuat::getXYZW(xF32 *d) const
	{
	d[0] = (xF32)x;
	d[1] = (xF32)y;
	d[2] = (xF32)z;
	d[3] = (xF32)w;
	}


X_INLINE void xQuat::getWXYZ(xF64 *d) const
	{
	d[1] = (xF64)x;
	d[2] = (xF64)y;
	d[3] = (xF64)z;
	d[0] = (xF64)w;
	}


X_INLINE void xQuat::getXYZW(xF64 *d) const
	{
	d[0] = (xF64)x;
	d[1] = (xF64)y;
	d[2] = (xF64)z;
	d[3] = (xF64)w;
	}

//const methods
 
X_INLINE bool xQuat::isFinite() const
	{
	return xMath::isFinite(x) 
		&& xMath::isFinite(y) 
		&& xMath::isFinite(z)
		&& xMath::isFinite(w);
	}



X_INLINE void xQuat::zero()
	{
	x = xReal(0.0);
	y = xReal(0.0);
	z = xReal(0.0);
	w = xReal(1.0);
	}


X_INLINE void xQuat::negate()
	{
	x = -x;
	y = -y;
	z = -z;
	w = -w;
	}

X_INLINE xQuat xQuat::operator-() const
	{
	return xQuat(-x,-y,-z,-w);
	}


X_INLINE void xQuat::random()
	{
	x = xMath::rand(xReal(0.0),xReal(1.0));
	y = xMath::rand(xReal(0.0),xReal(1.0));
	z = xMath::rand(xReal(0.0),xReal(1.0));
	w = xMath::rand(xReal(0.0),xReal(1.0));
	normalize();
	}


X_INLINE void xQuat::fromAngleAxis(xReal Angle, const xVec3 & axis)			// set the xQuat by Angle-axis (see AA constructor)
	{
	x = axis.x;
	y = axis.y;
	z = axis.z;

	// required: Normalize the axis

	const xReal i_length =  xReal(1.0) / xMath::sqrt( x*x + y*y + z*z );
	
	x = x * i_length;
	y = y * i_length;
	z = z * i_length;

	// now make a clQuaternionernion out of it
	xReal Half = xMath::degToRad(Angle * xReal(0.5));

	w = xMath::cos(Half);//this used to be w/o deg to rad.
	const xReal sin_theta_over_two = xMath::sin( Half );
	x = x * sin_theta_over_two;
	y = y * sin_theta_over_two;
	z = z * sin_theta_over_two;
	}

X_INLINE void xQuat::fromAngleAxisFast(xReal AngleRadians, const xVec3 & axis)
	{
	xReal s;
	xMath::sinCos(AngleRadians * 0.5f, s, w);
	x = axis.x * s;
	y = axis.y * s;
	z = axis.z * s;
	}

X_INLINE void xQuat::invert()
	{
	x = -x;
	y = -y;
	z = -z;
	}

X_INLINE void xQuat::setx(const xReal& d) 
	{ 
	x = d;
	}


X_INLINE void xQuat::sety(const xReal& d) 
	{ 
	y = d;
	}


X_INLINE void xQuat::setz(const xReal& d) 
	{ 
	z = d;
	}


X_INLINE void xQuat::setw(const xReal& d) 
	{ 
	w = d;
	}


X_INLINE void xQuat::getAngleAxis(xReal& angle, xVec3 & axis) const
	{
	//return axis and angle of rotation of quaternion
    angle = xMath::acos(w) * xReal(2.0);		//this is getAngle()
    xReal sa = xMath::sqrt(xReal(1.0) - w*w);
	if (sa)
		{
		axis.set(x/sa,y/sa,z/sa);
		angle = xMath::radToDeg(angle);
		}
	else
		axis.set(xReal(1.0),xReal(0.0),xReal(0.0));

	}



X_INLINE xReal xQuat::getAngle() const
	{
	return xMath::acos(w) * xReal(2.0);
	}



X_INLINE xReal xQuat::getAngle(const xQuat & q) const
	{
	return xMath::acos(dot(q)) * xReal(2.0);
	}


X_INLINE xReal xQuat::magnitudeSquared() const

//modifiers:
	{
	return x*x + y*y + z*z + w*w;
	}


X_INLINE xReal xQuat::dot(const xQuat &v) const
	{
	return x * v.x + y * v.y + z * v.z  + w * v.w;
	}


X_INLINE void xQuat::normalize()											// convert this xQuat to a unit clQuaternionernion
	{
	const xReal mag = xMath::sqrt(magnitudeSquared());
	if (mag)
		{
		const xReal imag = xReal(1.0) / mag;
		
		x *= imag;
		y *= imag;
		z *= imag;
		w *= imag;
		}
	}


X_INLINE void xQuat::conjugate()											// convert this xQuat to a unit clQuaternionernion
	{
	x = -x;
	y = -y;
	z = -z;
	}


X_INLINE void xQuat::multiply(const xQuat& left, const xQuat& right)		// this = a * b
	{
	xReal a,b,c,d;

	a =left.w*right.w - left.x*right.x - left.y*right.y - left.z*right.z;
	b =left.w*right.x + right.w*left.x + left.y*right.z - right.y*left.z;
	c =left.w*right.y + right.w*left.y + left.z*right.x - right.z*left.x;
	d =left.w*right.z + right.w*left.z + left.x*right.y - right.x*left.y;

	w = a;
	x = b;
	y = c;
	z = d;
	}


X_INLINE void xQuat::multiply(const xQuat& left, const xVec3& right)		// this = a * b
	{
	xReal a,b,c,d;

	a = - left.x*right.x - left.y*right.y - left.z *right.z;
	b =   left.w*right.x + left.y*right.z - right.y*left.z;
	c =   left.w*right.y + left.z*right.x - right.z*left.x;
	d =   left.w*right.z + left.x*right.y - right.x*left.y;

	w = a;
	x = b;
	y = c;
	z = d;
	}

X_INLINE void xQuat::slerp(const xReal t, const xQuat& left, const xQuat& right) // this = slerp(t, a, b)
	{
	const xReal	quatEpsilon = (xReal(1.0e-8f));

	*this = left;

	xReal cosine = 
		x * right.x + 
		y * right.y + 
		z * right.z + 
		w * right.w;		//this is left.dot(right)

	xReal sign = xReal(1);
	if (cosine < 0)
		{
		cosine = - cosine;
		sign = xReal(-1);
		}

	xReal Sin = xReal(1) - cosine*cosine;

	if(Sin>=quatEpsilon*quatEpsilon)	
		{
		Sin = xMath::sqrt(Sin);
		const xReal angle = xMath::atan2(Sin, cosine);
		const xReal i_sin_angle = xReal(1) / Sin;



		xReal lower_weight = xMath::sin(angle*(xReal(1)-t)) * i_sin_angle;
		xReal upper_weight = xMath::sin(angle * t) * i_sin_angle * sign;

		w = (w * (lower_weight)) + (right.w * (upper_weight));
		x = (x * (lower_weight)) + (right.x * (upper_weight));
		y = (y * (lower_weight)) + (right.y * (upper_weight));
		z = (z * (lower_weight)) + (right.z * (upper_weight));
		}
	}


X_INLINE void xQuat::rotate(xVec3 & v) const						//rotates passed vec by rot expressed by quaternion.  overwrites arg ith the result.
	{
	//xReal msq = xReal(1.0)/magnitudeSquared();	//assume unit quat!
	xQuat myInverse;
	myInverse.x = -x;//*msq;
	myInverse.y = -y;//*msq;
	myInverse.z = -z;//*msq;
	myInverse.w =  w;//*msq;

	//v = ((*this) * v) ^ myInverse;

	xQuat left;
	left.multiply(*this,v);
	v.x =left.w*myInverse.x + myInverse.w*left.x + left.y*myInverse.z - myInverse.y*left.z;
	v.y =left.w*myInverse.y + myInverse.w*left.y + left.z*myInverse.x - myInverse.z*left.x;
	v.z =left.w*myInverse.z + myInverse.w*left.z + left.x*myInverse.y - myInverse.x*left.y;
	}


X_INLINE void xQuat::inverseRotate(xVec3 & v) const				//rotates passed vec by opposite of rot expressed by quaternion.  overwrites arg ith the result.
	{
	//xReal msq = xReal(1.0)/magnitudeSquared();	//assume unit quat!
	xQuat myInverse;
	myInverse.x = -x;//*msq;
	myInverse.y = -y;//*msq;
	myInverse.z = -z;//*msq;
	myInverse.w =  w;//*msq;

	//v = (myInverse * v) ^ (*this);
	xQuat left;
	left.multiply(myInverse,v);
	v.x =left.w*x + w*left.x + left.y*z - y*left.z;
	v.y =left.w*y + w*left.y + left.z*x - z*left.x;
	v.z =left.w*z + w*left.z + left.x*y - x*left.y;
	}


X_INLINE xQuat& xQuat::operator=  (const xQuat& q)
	{
	x = q.x;
	y = q.y;
	z = q.z;
	w = q.w;
	return *this;
	}

#if 0
X_INLINE xQuat& xQuat::operator=  (const xVec3& v)		//implicitly extends vector by a 0 w element.
	{
	x = v.x;
	y = v.y;
	z = v.z;
	w = xReal(1.0);
	return *this;
	}
#endif

X_INLINE xQuat& xQuat::operator*= (const xQuat& q)
	{
	xReal xx[4]; //working Quaternion
	xx[0] = w*q.w - q.x*x - y*q.y - q.z*z;
	xx[1] = w*q.x + q.w*x + y*q.z - q.y*z;
	xx[2] = w*q.y + q.w*y + z*q.x - q.z*x;
	z=w*q.z + q.w*z + x*q.y - q.x*y;

	w = xx[0];
	x = xx[1];
	y = xx[2];
	return *this;
	}


X_INLINE xQuat& xQuat::operator+= (const xQuat& q)
	{
	x+=q.x;
	y+=q.y;
	z+=q.z;
	w+=q.w;
	return *this;
	}


X_INLINE xQuat& xQuat::operator-= (const xQuat& q)
	{
	x-=q.x;
	y-=q.y;
	z-=q.z;
	w-=q.w;
	return *this;
	}


X_INLINE xQuat& xQuat::operator*= (const xReal s)
	{
	x*=s;
	y*=s;
	z*=s;
	w*=s;
	return *this;
	}

X_INLINE xQuat::xQuat(xReal ix, xReal iy, xReal iz, xReal iw)
{
	x = ix;
	y = iy;
	z = iz;
	w = iw;
}

X_INLINE xQuat xQuat::operator*(const xQuat &q) const
{
	return xQuat(w*q.x + q.w*x + y*q.z - q.y*z,
				  w*q.y + q.w*y + z*q.x - q.z*x,
				  w*q.z + q.w*z + x*q.y - q.x*y,
				  w*q.w - x*q.x - y*q.y - z*q.z);
}

X_INLINE xQuat xQuat::operator+(const xQuat &q) const
{
	return xQuat(x+q.x,y+q.y,z+q.z,w+q.w);
}

X_INLINE xQuat xQuat::operator-(const xQuat &q) const
{
	return xQuat(x-q.x,y-q.y,z-q.z,w-q.w);
}

X_INLINE xQuat xQuat::operator!() const
{
	return xQuat(-x,-y,-z,w);
}



X_INLINE const xVec3 xQuat::rot(const xVec3 &v) const
    {
	xVec3 qv(x,y,z);

	return (v*(w*w-0.5f) + (qv^v)*w + qv*(qv|v))*2;
    }

X_INLINE const xVec3 xQuat::invRot(const xVec3 &v) const
    {
	xVec3 qv(x,y,z);

	return (v*(w*w-0.5f) - (qv^v)*w + qv*(qv|v))*2;
    }



X_INLINE const xVec3 xQuat::transform(const xVec3 &v, const xVec3 &p) const
    {
	return rot(v)+p;
    }

X_INLINE const xVec3 xQuat::invTransform(const xVec3 &v, const xVec3 &p) const
    {
	return invRot(v-p);
    }

 /** @} */
#endif
//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND
