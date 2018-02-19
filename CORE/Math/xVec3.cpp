#include "xVec3.h"

const _x3F32& x3F32::operator=(const xVec3& d)
{
	x=d.x;
	y=d.y;
	z=d.z;
	return *this;
}
/** \endcond */

//  implementations:
xVec3::xVec3(xReal v) : x(v), y(v), z(v)
{
}

xVec3::xVec3(xReal _x, xReal _y, xReal _z) : x(_x), y(_y), z(_z)
{
}

xVec3::xVec3(const x3F32 &d) : x(d.x), y(d.y), z(d.z)
{
}

xVec3::xVec3(const xReal v[]) : x(v[0]), y(v[1]), z(v[2])
{
}


xVec3::xVec3(const xVec3 &v) : x(v.x), y(v.y), z(v.z)
{
}


xVec3::xVec3()
{
	//default constructor leaves data uninitialized.
}


const xVec3& xVec3::operator=(const xVec3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

const xVec3& xVec3::operator=(const x3F32& d)
{
	x = d.x;
	y = d.y;
	z = d.z;
	return *this;
}

// Access the data as an array.

const xReal* xVec3::get() const
{
	return &x;
}


xReal* xVec3::get()
{
	return &x;
}


void  xVec3::get(xF32 * v) const
{
	v[0] = (xF32)x;
	v[1] = (xF32)y;
	v[2] = (xF32)z;
}


void  xVec3::get(xF64 * v) const
{
	v[0] = (xF64)x;
	v[1] = (xF64)y;
	v[2] = (xF64)z;
}


xReal& xVec3::operator[](int index)
{
	
	return (&x)[index];
}


xReal  xVec3::operator[](int index) const
{
	
	return (&x)[index];
}


void xVec3::setx(const xReal & d) 
{ 
	x = d; 
}


void xVec3::sety(const xReal & d) 
{ 
	y = d; 
}


void xVec3::setz(const xReal & d) 
{ 
	z = d; 
}

//Operators

bool xVec3::operator< (const xVec3&v) const
{
	return ((x < v.x)&&(y < v.y)&&(z < v.z));
}


bool xVec3::operator==(const xVec3& v) const
{
	return ((x == v.x)&&(y == v.y)&&(z == v.z));
}


bool xVec3::operator!=(const xVec3& v) const
{
	return ((x != v.x)||(y != v.y)||(z != v.z));
}

//Methods

void  xVec3::set(const xVec3 & v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}


void  xVec3::setNegative(const xVec3 & v)
{
	x = -v.x;
	y = -v.y;
	z = -v.z;
}


void  xVec3::setNegative()
{
	x = -x;
	y = -y;
	z = -z;
}



void  xVec3::set(const xF32 * v)
{
	x = (xReal)v[0];
	y = (xReal)v[1];
	z = (xReal)v[2];
}


void  xVec3::set(const xF64 * v)
{
	x = (xReal)v[0];
	y = (xReal)v[1];
	z = (xReal)v[2];
}



void  xVec3::set(xReal _x, xReal _y, xReal _z)
{
	this->x = _x;
	this->y = _y;
	this->z = _z;
}


void xVec3::set(xReal v)
{
	x = v;
	y = v;
	z = v;
}


void  xVec3::zero()
{
	x = y = z = 0.0;
}


void  xVec3::setPlusInfinity()
{
	x = y = z = X_MAX_F32; //TODO: this may be double too, but here we can't tell!
}


void  xVec3::setMinusInfinity()
{
	x = y = z = X_MIN_F32; //TODO: this may be double too, but here we can't tell!
}


void xVec3::max(const xVec3 & v)
{
	x = xMath::max(x, v.x);
	y = xMath::max(y, v.y);
	z = xMath::max(z, v.z);
}


void xVec3::min(const xVec3 & v)
{
	x = xMath::min(x, v.x);
	y = xMath::min(y, v.y);
	z = xMath::min(z, v.z);
}




void  xVec3::add(const xVec3 & a, const xVec3 & b)
{
	x = a.x + b.x;
	y = a.y + b.y;
	z = a.z + b.z;
}


void  xVec3::subtract(const xVec3 &a, const xVec3 &b)
{
	x = a.x - b.x;
	y = a.y - b.y;
	z = a.z - b.z;
}


void  xVec3::arrayMultiply(const xVec3 &a, const xVec3 &b)
{
	x = a.x * b.x;
	y = a.y * b.y;
	z = a.z * b.z;
}


void  xVec3::multiply(xReal s,  const xVec3 & a)
{
	x = a.x * s;
	y = a.y * s;
	z = a.z * s;
}


void  xVec3::multiplyAdd(xReal s, const xVec3 & a, const xVec3 & b)
{
	x = s * a.x + b.x;
	y = s * a.y + b.y;
	z = s * a.z + b.z;
}


xReal xVec3::normalize()
{
	xReal m = magnitude();
	if (m)
	{
		const xReal il =  xReal(1.0) / m;
		x *= il;
		y *= il;
		z *= il;
	}
	return m;
}


void xVec3::setMagnitude(xReal length)
{
	xReal m = magnitude();
	if(m)
	{
		xReal newLength = length / m;
		x *= newLength;
		y *= newLength;
		z *= newLength;
	}
}


xAxisType xVec3::snapToClosestAxis()
{
	const xReal almostOne = 0.999999f;
	if(x >=  almostOne) { set( 1.0f,  0.0f,  0.0f);	return X_AXIS_PLUS_X ; }
	else	if(x <= -almostOne) { set(-1.0f,  0.0f,  0.0f);	return X_AXIS_MINUS_X; }
	else	if(y >=  almostOne) { set( 0.0f,  1.0f,  0.0f);	return X_AXIS_PLUS_Y ; }
	else	if(y <= -almostOne) { set( 0.0f, -1.0f,  0.0f);	return X_AXIS_MINUS_Y; }
	else	if(z >=  almostOne) { set( 0.0f,  0.0f,  1.0f);	return X_AXIS_PLUS_Z ; }
	else	if(z <= -almostOne) { set( 0.0f,  0.0f, -1.0f);	return X_AXIS_MINUS_Z; }
	else													return X_AXIS_ARBITRARY;
}


xU32 xVec3::closestAxis() const
{
	const xF32* vals = &x;
	xU32 m = 0;
	if(xMath::abs(vals[1]) > xMath::abs(vals[m])) m = 1;
	if(xMath::abs(vals[2]) > xMath::abs(vals[m])) m = 2;
	return m;
}


//const methods

bool xVec3::isFinite() const
{
	return xMath::isFinite(x) && xMath::isFinite(y) && xMath::isFinite(z);
}


xReal xVec3::dot(const xVec3 &v) const
{
	return x * v.x + y * v.y + z * v.z;
}


bool xVec3::sameDirection(const xVec3 &v) const
{
	return x*v.x + y*v.y + z*v.z >= 0.0f;
}


xReal xVec3::magnitude() const
{
	return xMath::sqrt(x * x + y * y + z * z);
}


xReal xVec3::magnitudeSquared() const
{
	return x * x + y * y + z * z;
}


xReal xVec3::distance(const xVec3 & v) const
{
	xReal dx = x - v.x;
	xReal dy = y - v.y;
	xReal dz = z - v.z;
	return xMath::sqrt(dx * dx + dy * dy + dz * dz);
}


xReal xVec3::distanceSquared(const xVec3 &v) const
{
	xReal dx = x - v.x;
	xReal dy = y - v.y;
	xReal dz = z - v.z;
	return dx * dx + dy * dy + dz * dz;
}


void xVec3::cross(const xVec3 &left, const xVec3 & right)	//prefered version, w/o temp object.
{
	// temps needed in case left or right is this.
	xReal a = (left.y * right.z) - (left.z * right.y);
	xReal b = (left.z * right.x) - (left.x * right.z);
	xReal c = (left.x * right.y) - (left.y * right.x);
	
	x = a;
	y = b;
	z = c;
}


bool xVec3::equals(const xVec3 & v, xReal epsilon) const
{
	return 
	xMath::equals(x, v.x, epsilon) &&
	xMath::equals(y, v.y, epsilon) &&
	xMath::equals(z, v.z, epsilon);
}



xVec3 xVec3::operator -() const
{
	return xVec3(-x, -y, -z);
}


xVec3 xVec3::operator +(const xVec3 & v) const
{
	return xVec3(x + v.x, y + v.y, z + v.z);	// RVO version
}


xVec3 xVec3::operator -(const xVec3 & v) const
{
	return xVec3(x - v.x, y - v.y, z - v.z);	// RVO version
}



xVec3 xVec3::operator *(xReal f) const
{
	return xVec3(x * f, y * f, z * f);	// RVO version
}


xVec3 xVec3::operator /(xReal f) const
{
	f = xReal(1.0) / f; return xVec3(x * f, y * f, z * f);
}


xVec3& xVec3::operator +=(const xVec3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}


xVec3& xVec3::operator -=(const xVec3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}


xVec3& xVec3::operator *=(xReal f)
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}


xVec3& xVec3::operator /=(xReal f)
{
	f = 1.0f/f;
	x *= f;
	y *= f;
	z *= f;
	return *this;
}


xVec3 xVec3::cross(const xVec3& v) const
{
	xVec3 temp;
	temp.cross(*this,v);
	return temp;
}


xVec3 xVec3::operator^(const xVec3& v) const
{
	xVec3 temp;
	temp.cross(*this,v);
	return temp;
}


xReal xVec3::operator|(const xVec3& v) const
{
	return x * v.x + y * v.y + z * v.z;
}

/**
 scalar pre-multiplication
 */

xVec3 operator *(xReal f, const xVec3& v)
{
	return xVec3(f * v.x, f * v.y, f * v.z);
}

