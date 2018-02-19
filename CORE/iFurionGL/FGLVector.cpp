

//-----------------------------------------------------------------------------------//

#include "FurionGL.hpp"

float PI = 3.1415926f;

//---------------------------------------------------------------------------------------------

cFGLVECTOR2D cFGLVECTOR2D ::operator+( cFGLVECTOR2D vec )
{
	return gVec( vec.x + x, vec.y + y );
}

//---------------------------------------------------------------------------------------------

cFGLVECTOR2D cFGLVECTOR2D ::operator-( cFGLVECTOR2D vec )
{
	return gVec( vec.x - x, vec.y - y );
}

//---------------------------------------------------------------------------------------------

float cFGLMATH::fRadToDeg( float Rad )
{
	float Deg;

	Deg = Rad * 180 / PI;

	return Deg;
}

//---------------------------------------------------------------------------------------------

int cFGLMATH::iRadToDeg( float Rad )
{
	float Deg;

	Deg = Rad * 180 / PI;

	int iDeg = (int)Deg;

	return iDeg;
}

//---------------------------------------------------------------------------------------------

float cFGLMATH::fDegToRad( float Deg )
{
	float Rad;

	Rad = ( Deg * PI  ) / 180;

	return Rad;
}

//---------------------------------------------------------------------------------------------

int cFGLMATH::iDegToRad( float Deg )
{
	float Rad;

	Rad = ( Deg * PI  ) / 180;

	int iRad = (int)Rad;

	return iRad;
}

//---------------------------------------------------------------------------------------------

cFGLVEC gVec( float x, float y )
{
	cFGLVEC V;
	V.x = x;
	V.y = y;
	V.ox = x;
	V.oy = y;

	return V;
}

//---------------------------------------------------------------------------------------------

cFGLVEC gVec( cFGLPOINT P )
{
	cFGLVEC V;
	V.x = P.x;
	V.y = P.y;
	V.ox = P.x;
	V.oy = P.y;

	return V;
}

//---------------------------------------------------------------------------------------------

cFGLVEC gNormVec( float x, float y )
{
	cFGLVEC V;
	V.x = x;
	V.y = y;
	V.ox = x;
	V.oy = y;

	float len = sqrt( ( V.x * V.x ) + ( V.y * V.y ) );

	if( len > 0 )
	{
		V.x = V.x / len;
		V.y = V.y / len;
		V.norm = true;
	}

	return V;
}

//---------------------------------------------------------------------------------------------

float cFGLMATH::fVecLength( cFGLVEC V )
{

	float len = sqrt( ( V.x * V.x ) + ( V.y * V.y ) );
	return len;
}

//---------------------------------------------------------------------------------------------

int cFGLMATH::iVecLength( cFGLVEC V )
{

	float len = sqrt( ( V.x * V.x ) + ( V.y * V.y ) );
	int ilen = len;
	return ilen;
}

//---------------------------------------------------------------------------------------------

void cFGLMATH::NormalizeVec( cFGLVEC * V )
{
	float magnitude = 1;//fMagnitude( gVec(V->x, V->y) );

	V->ox = V->x;
	V->oy = V->y;

	V->x /= magnitude;
	V->y /= magnitude;
}

//---------------------------------------------------------------------------------------------

float cFGLMATH::fVecAngle( cFGLVEC V1, cFGLVEC V2 )
{

	float angle;

	if( V1.norm && V2.norm )
	{
        float param = V1.x*V2.x + V1.y*V2.y;
        if( (param > 1) || (param<-1)) param = 0;

		angle = acos( param );
	}
	else
	{
        float param = (V1.x*V2.x + V1.y*V2.y)/(fVecLength(V1)*fVecLength(V2));
        if( (param > 1) || (param<-1)) param = 0;

        angle = acos( param );
    }
    
	return angle;
}

//---------------------------------------------------------------------------------------------

int cFGLMATH::iVecAngle( cFGLVEC V1, cFGLVEC V2 )
{

	float angle;

	if( V1.norm && V2.norm )
	{
        float param = V1.x*V2.x + V1.y*V2.y;
        if( (param > 1) || (param<-1)) param = 0;
		angle = acos( param );
	}
	else
	{
        float param = (V1.x*V2.x + V1.y*V2.y)/(fVecLength(V1)*fVecLength(V2));
        if( (param > 1) || (param<-1)) param = 0;
		angle = acos( param );
	}

	int iangle = fRadToDeg(angle);

	return iangle;

}

//---------------------------------------------------------------------------------------------

float cFGLMATH::fVecCosinus( cFGLVEC V1, cFGLVEC V2 )
{

	float cosinus;

	if( V1.norm && V2.norm )
	{
		cosinus = ( V1.x*V2.x + V1.y*V2.y );
	}
	else
	{
		cosinus = ( (V1.x*V2.x + V1.y*V2.y)/(fVecLength(V1)*fVecLength(V2)) );
	}
    
	return cosinus;
}

//---------------------------------------------------------------------------------------------

float cFGLMATH::fVecScalarProd( cFGLVEC V1, cFGLVEC V2 )
{

	float result;

	result = ( V1.x*V2.x + V1.y*V2.y );

	return result;
}

//---------------------------------------------------------------------------------------------

float cFGLMATH::fVecVectorProd( cFGLVEC V1, cFGLVEC V2 )
{

	float result;

	result = ( fVecLength(V1)*fVecLength(V2)*sin(fVecAngle(V1,V2)) );

	return result;
}

//---------------------------------------------------------------------------------------------

float cFGLMATH::fVecClock( cFGLVEC V )
{

	cFGLVEC SV;
	SV.x = 0;
	SV.y = 1;

	float angle;

	angle = fVecAngle( SV, V );

	if( V.x == 0 ){ V.x += 0.001f; }

	if( V.x > 0 ){ angle = PI - angle; }
	if( V.x < 0 ){ angle = PI + angle; }

	return angle;
}

//---------------------------------------------------------------------------------------------

float cFGLMATH::fVecPosClock( float x, float y, cFGLVEC V )
{

	cFGLVEC SV;
	SV.x = 0;
	SV.y = 1;

	if( !V.norm ){ V.x -= x; }else{ V.x = V.ox; V.x -= x; }
	if( !V.norm ){ V.y -= y; }else{ V.y = V.oy; V.y -= y; }

	float angle;

	angle = fVecAngle( SV, V );

	if( V.x == 0 ){ V.x += 0.001f; }

	if( V.x > 0 ){ angle = PI - angle; }
	if( V.x < 0 ){ angle = PI + angle; }

	return angle;
}

//---------------------------------------------------------------------------------------------

float cFGLMATH::fLookAt( int rx, int ry, int ex, int ey, int tx, int ty )
{

	cFGLVEC V1 = gVec( ex - rx, ey - ry );
	cFGLVEC V2 = gVec( tx - rx, ty - ry );

	float a1 = fVecClock( V1 );
	float a2 = fVecClock( V2 );

	float a = (2*PI) + (a2 - a1);

	if( a > (2*PI ) ){ a = a - 2*PI; }

	return a;
}

//---------------------------------------------------------------------------------------------

float cFGLMATH::fLookAt( int x, int y, int rx, int ry, int ex, int ey, int tx, int ty )
{

	rx = rx + x;
	ry = ry + y;
	ex = ex + x;
	ey = ey + y;

	cFGLVEC V1 = gVec( ex - rx, ey - ry );
	cFGLVEC V2 = gVec( tx - rx, ty - ry );

	double a1 = fVecClock( V1 );
	double a2 = fVecClock( V2 );

	double a = (2*PI) + (a2 - a1);

	if( a > (2*PI ) ){ a = a - 2*PI; }

	return float(a);
}

//---------------------------------------------------------------------------------------------

int cFGLMATH::iLookAt( int rx, int ry, int ex, int ey, int tx, int ty )
{

	cFGLVEC V1 = gVec( ex - rx, ey - ry );
	cFGLVEC V2 = gVec( tx - rx, ty - ry );

	float a1 = fVecClock( V1 );
	float a2 = fVecClock( V2 );

	float a = (2*PI) + (a2 - a1);

	if( a > (2*PI ) ){ a = a - 2*PI; }

	int i = (int)fRadToDeg( a );

	return i;
}

//---------------------------------------------------------------------------------------------

int cFGLMATH::iLookAt( int x, int y, int rx, int ry, int ex, int ey, int tx, int ty )
{

	rx = rx + x;
	ry = ry + y;
	ex = ex + x;
	ey = ey + y;

	cFGLVEC V1 = gVec( ex - rx, ey - ry );
	cFGLVEC V2 = gVec( tx - rx, ty - ry );

	double a1 = fVecClock( V1 );
	double a2 = fVecClock( V2 );

	double a = (2*PI) + (a2 - a1);

	if( a > (2*PI ) ){ a = a - 2*PI; }

	int i = (int)fRadToDeg ( (float)a );

	return i;
}

//---------------------------------------------------------------------------------------------
