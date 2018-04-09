#ifndef FOUNDATION_xMat34T
#define FOUNDATION_xMat34T
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

#include "xMat33.h"

/**
\brief Combination of a 3x3 rotation matrix and a translation vector.

homogenous transform class composed of a matrix and a vector.
*/

class xMat34
	{
	public:
	/**
	\brief [ M t ]
	*/
	xMat33 M;
	xVec3 t;

	/**
	\brief by default M is inited and t isn't.  Use this ctor to either init or not init in full.
	*/
	X_INLINE explicit xMat34(bool init = true);

	X_INLINE xMat34(const xMat33& rot, const xVec3& trans) : M(rot), t(trans)
		{
		}

	X_INLINE void zero();

	X_INLINE void id();

	/**
	\brief returns true for identity matrix
	*/
	X_INLINE bool isIdentity() const;

	/**
	\brief returns true if all elems are finite (not NAN or INF, etc.)
	*/
	X_INLINE bool isFinite() const;

	/**
	\brief assigns inverse to dest. 
	
	Returns false if singular (i.e. if no inverse exists), setting dest to identity.  dest may equal this.
	*/
	X_INLINE bool getInverse(xMat34& dest) const;

	/**
	\brief same as #getInverse(), but assumes that M is orthonormal
	*/
	X_INLINE bool getInverseRT(xMat34& dest) const;

	/**
	\brief dst = this * src
	*/
	X_INLINE void multiply(const xVec3 &src, xVec3 &dst) const;

	/**
	\brief operator wrapper for multiply
	*/
	X_INLINE xVec3 operator*  (const xVec3 & src) const { xVec3 dest; multiply(src, dest); return dest; }
	/**
	\brief dst = inverse(this) * src	-- assumes M is rotation matrix!!!
	*/
	X_INLINE void multiplyByInverseRT(const xVec3 &src, xVec3 &dst) const;

	/**
	\brief operator wrapper for multiplyByInverseRT
	*/
	X_INLINE xVec3 operator%  (const xVec3 & src) const { xVec3 dest; multiplyByInverseRT(src, dest); return dest; }

	/**
	\brief this = left * right	
	*/
	X_INLINE void multiply(const xMat34& left, const xMat34& right);

	/**
	\brief this = inverse(left) * right	-- assumes M is rotation matrix!!!
	*/
	X_INLINE void multiplyInverseRTLeft(const xMat34& left, const xMat34& right);

	/**
	\brief this = left * inverse(right)	-- assumes M is rotation matrix!!!
	*/
	X_INLINE void multiplyInverseRTRight(const xMat34& left, const xMat34& right);

	/**
	\brief operator wrapper for multiply
	*/
	X_INLINE xMat34 operator*  (const xMat34 & right) const
	{
		xMat34 dest(false);
		dest.multiply(*this, right);
		return dest;
	}

	X_INLINE xMat34 operator*  (const float f) const
	{
		xMat34 dest(false);
		dest.t = t*f;
		dest.M = M*f;
		return dest;
	}

	X_INLINE xMat34 operator+  (const xMat34 & m) const
	{
		xMat34 dest(false);
		dest.t = t+m.t;
		dest.M = M+m.M;
		return dest;
	}

	X_INLINE void mLookAt( xVec3 &Eye, xVec3 &dir, xVec3 &Up )
	{
		xVec3 &zaxis = dir;
		xVec3 xaxis = Up.cross( zaxis );
		xaxis.normalize();
		xVec3 yaxis = zaxis.cross( xaxis );

		float m[16] =
		{
			 xaxis.x,           yaxis.x,           zaxis.x,          0,
			 xaxis.y,           yaxis.y,           zaxis.y,          0,
			 xaxis.z,           yaxis.z,           zaxis.z,          0,
			 -xaxis.dot(Eye),   -yaxis.dot(Eye),   -zaxis.dot(Eye),  1
		};

		setColumnMajor44( m );
	}


	/**
	\brief convert from a matrix format appropriate for rendering
	*/
	X_INLINE void setColumnMajor44(const xF32 *);
	/**
	\brief convert from a matrix format appropriate for rendering
	*/
	X_INLINE void setColumnMajor44(const xF32 d[4][4]);
	/**
	\brief convert to a matrix format appropriate for rendering
	*/
	X_INLINE void getColumnMajor44(xF32 *) const;
	/**
	\brief convert to a matrix format appropriate for rendering
	*/
	X_INLINE void getColumnMajor44(xF32 d[4][4]) const;
	/**
	\brief set the matrix given a row major matrix.
	*/
	X_INLINE void setRowMajor44(const xF32 *);
	/**
	\brief set the matrix given a row major matrix.
	*/
	X_INLINE void setRowMajor44(const xF32 d[4][4]);
	/**
	\brief retrieve the matrix in a row major format.
	*/
	X_INLINE void getRowMajor44(xF32 *) const;
	/**
	\brief retrieve the matrix in a row major format.
	*/
	X_INLINE void getRowMajor44(xF32 d[4][4]) const;
	};


X_INLINE xMat34::xMat34(bool init)
	{
	if (init)
	{
		t.zero();
		M.id();
	}
	}


X_INLINE void xMat34::zero()
	{
	M.zero();
	t.zero();
	}


X_INLINE void xMat34::id()
	{
	M.id();
	t.zero();
	}


X_INLINE bool xMat34::isIdentity() const
	{
	if(!M.isIdentity())	return false;
	if(!t.isZero())		return false;
	return true;
	}


X_INLINE bool xMat34::isFinite() const
	{
	if(!M.isFinite())	return false;
	if(!t.isFinite())	return false;
	return true;
	}


X_INLINE bool xMat34::getInverse(xMat34& dest) const
	{
	// inv(this) = [ inv(M) , inv(M) * -t ]
	bool status = M.getInverse(dest.M);
	dest.M.multiply(t * -1.0f, dest.t); 
	return status;
	}


X_INLINE bool xMat34::getInverseRT(xMat34& dest) const
	{
	// inv(this) = [ M' , M' * -t ]
	dest.M.setTransposed(M);
	dest.M.multiply(t * -1.0f, dest.t); 
	return true;
	}



X_INLINE void xMat34::multiply(const xVec3 &src, xVec3 &dst) const
	{
	dst = M * src + t;
	}


X_INLINE void xMat34::multiplyByInverseRT(const xVec3 &src, xVec3 &dst) const
	{
	//dst = M' * src - M' * t = M' * (src - t)
	M.multiplyByTranspose(src - t, dst);
	}


X_INLINE void xMat34::multiply(const xMat34& left, const xMat34& right)
	{
	//[aR at] * [bR bt] = [aR * bR		aR * bt + at]  NOTE: order of operations important so it works when this ?= left ?= right.
	t = left.M * right.t + left.t;
	M.multiply(left.M, right.M);
	}


X_INLINE void xMat34::multiplyInverseRTLeft(const xMat34& left, const xMat34& right)
	{
	//[aR' -aR'*at] * [bR bt] = [aR' * bR		aR' * bt  - aR'*at]	//aR' ( bt  - at )	NOTE: order of operations important so it works when this ?= left ?= right.
	t = left.M % (right.t - left.t);
	M.multiplyTransposeLeft(left.M, right.M);
	}


X_INLINE void xMat34::multiplyInverseRTRight(const xMat34& left, const xMat34& right)
	{
	//[aR at] * [bR' -bR'*bt] = [aR * bR'		-aR * bR' * bt + at]	NOTE: order of operations important so it works when this ?= left ?= right.
	M.multiplyTransposeRight(left.M, right.M);
	t = left.t - M * right.t;
	}

X_INLINE void xMat34::setColumnMajor44(const xF32 * d) 
	{
	M.setColumnMajorStride4(d);
    t.x = d[12];
	t.y = d[13];
	t.z = d[14];
	}

X_INLINE void xMat34::setColumnMajor44(const xF32 d[4][4]) 
	{
	M.setColumnMajorStride4(d);
    t.x = d[3][0];
	t.y = d[3][1];
	t.z = d[3][2];
	}

X_INLINE void xMat34::getColumnMajor44(xF32 * d) const
	{
	M.getColumnMajorStride4(d);
    d[12] = t.x;
	d[13] = t.y;
	d[14] = t.z;
	d[3] = d[7] = d[11] = 0.0f;
	d[15] = 1.0f;
	}

X_INLINE void xMat34::getColumnMajor44(xF32 d[4][4]) const
	{
	M.getColumnMajorStride4(d);
    d[3][0] = t.x;
	d[3][1] = t.y;
	d[3][2] = t.z;
	d[0][3] = d[1][3] = d[2][3] = 0.0f;
	d[3][3] = 1.0f;
	}

X_INLINE void xMat34::setRowMajor44(const xF32 * d) 
	{
	M.setRowMajorStride4(d);
    t.x = d[3];
	t.y = d[7];
	t.z = d[11];
	}

X_INLINE void xMat34::setRowMajor44(const xF32 d[4][4])
	{
	M.setRowMajorStride4(d);
    t.x = d[0][3];
	t.y = d[1][3];
	t.z = d[2][3];
	}

X_INLINE void xMat34::getRowMajor44(xF32 * d) const
	{
	M.getRowMajorStride4(d);
    d[3] = t.x;
	d[7] = t.y;
	d[11] = t.z;
	d[12] = d[13] = d[14] = 0.0f;
	d[15] = 1.0f;
	}

X_INLINE void xMat34::getRowMajor44(xF32 d[4][4]) const
	{
	M.getRowMajorStride4(d);
    d[0][3] = t.x;
	d[1][3] = t.y;
	d[2][3] = t.z;
	d[3][0] = d[3][1] = d[3][2] = 0.0f;
	d[3][3] = 1.0f;
	}

 /** @} */
#endif
//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND
