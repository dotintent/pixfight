#pragma once

/** \addtogroup foundation
  @{
*/

#include "xf.h"
#include "xMath.h"

class xVec2
{
public:

	xVec2()
	{
	}
    
    xVec2( xVec2 const &t){
        x = t.x;
        y = t.y;
    }

	xVec2( float xx, float yy )
	{
		x = xx;
		y = yy;
	}

    void lerp( xVec2 &a, xVec2 &b, float coef )
	{
		*this = a + ( b - a )*coef;
	}

	 xVec2 &operator =(const xVec2 &b )
	{
		if( this == &b )
			return *this;

		x = b.x;
		y = b.y;
		return *this;
	}

	 xVec2 operator +(const xVec2 &b )
	{
		return xVec2(x+b.x,y+b.y);
	}

	 xVec2 operator -(const xVec2 &b )
	{
		return xVec2(x-b.x,y-b.y);
	}

	 xVec2 operator *( float b )
	{
		xVec2 tmp;
		tmp.x = x * b;
		tmp.y = y * b;
		return tmp;
	}
    
    xVec2& operator +=(const xVec2& v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }
    
    xVec2& operator -=(const xVec2& v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }
    
    xVec2& operator /=(const float& v)
    {
        x /= v;
        y /= v;
        return *this;
    }
    
    float mag(){
        return xMath::sqrt(x * x + y * y);
    }
    
    float normalize()
    {
        float m = mag();
        if (m)
        {
            const float il =  float(1.0) / m;
            x *= il;
            y *= il;
        }
        return m;
    }
    
    float dot(const xVec2 &v) const
    {
        return x * v.x + y * v.y;
    }

	float x, y;
};

/** @} */
