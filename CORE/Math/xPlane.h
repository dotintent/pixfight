#pragma once

#include "xf.h"
#include "xMath.h"
#include "xVec3.h"

class xPlane
{
public:

	xPlane()
	{
	}

	xPlane( const xPlane &B )
	{
		a = B.a;
		b = B.b;
		c = B.c;
		d = B.d;
	}

	xPlane( const xVec3 &point, const xVec3 &normal )
	{
		set( point, normal );
	}

	xPlane( const xVec3 &v1, const xVec3 &v2, const xVec3 &v3 )
	{
		set( v1, v2, v3 );
	}

	inline void set( const xVec3 &v1, const xVec3 &v2, const xVec3 &v3 )
	{
		xVec3 n;
		n.cross( v2-v3, v1-v3 );
		set( v1, n );
	}

	inline void set( const xVec3 &point, const xVec3 &normal )
	{
		xVec3 n1;
		n1 = normal;
		n1.normalize();
		a = n1.x;
		b = n1.y;
		c = n1.z;
		d = -dotn( point );
	}

	inline float dotn( const xVec3 &v ) const
	{
		return a*v.x + b*v.y + c*v.z;
	}

	inline float dotc( const xVec3 &v ) const
	{
		return dotn( v ) + d;
	}

	inline xPlane &operator =( const xPlane &B )
	{
		a = B.a;
		b = B.b;
		c = B.c;
		d = B.d;

		return *this;
	}
/*
	__forceinline bool MYPlaneIntersectLine( MYVECTOR3 *out, MYPLANE *plane, MYVECTOR3 *v1, MYVECTOR3 *v2, double *proc )
	{
		MYVECTOR3 V;
		V = *v1 - *v2;

		double w = MYPlaneDotNormal( plane, &V );

		if( abs(w) < EPSILON )
			return false;

		double W = MYPlaneDotCoord( plane, v2 )/w;

		if( abs(W) > 1.0 + EPSILON )
			return false;

		*out = *v2 - V*W;

		*proc = W;

		return true;
	}*/

	float a, b, c, d;
};
