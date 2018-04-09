

//-----------------------------------------------------------------------------------//

#include "FurionGL.hpp"

//-----------------------------------------------------------------------------------//

cFGLPOINT gPoint( int x, int y )
{

	cFGLPOINT P = { x, y };
	return P;
}

//------------------------------------------------------------------------------

cFGLLINE gLine ( int x1, int y1, int x2, int y2  )
{

	cFGLLINE L = { (float)x1, (float)y1, (float)x2, (float)y2 };
	return L;
}

//------------------------------------------------------------------------------

cFGLBOX gBox( float L, float T, float R, float B )
{

	cFGLBOX VBox = { L, T, R, B };

	return VBox;
}

//---------------------------------------------------------------------------------------------

cFGLBOX gWHBox( float L, float T, float W, float H )
{
	cFGLBOX VBox = { L, T, L+W, T+H };

	return VBox;
}

//---------------------------------------------------------------------------------------------

cFGLBOX gCenteredBox( float x, float y, float xc, float yc, float W, float H )
{
	cFGLBOX VBox;

    VBox.v[0].x = x - xc;
    VBox.v[0].y = y - yc;

    VBox.v[1].x = x + ( W - xc );
    VBox.v[1].y = y + ( H - xc );

	return VBox;
}

//---------------------------------------------------------------------------------------------

cFGLBOX cFGLBOX::operator/( float i )
{
    cFGLBOX VBox = { v[0].x/i, v[0].y/i, v[1].x/i, v[1].y/i };

    return VBox;
}

//---------------------------------------------------------------------------------------------

cFGLBOX cFGLBOX::operator+( float i )
{
    cFGLBOX VBox = { v[0].x+i, v[0].y+i, v[1].x+i, v[1].y+i };

    return VBox;
}

//---------------------------------------------------------------------------------------------

cFGLABOX gABox( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4 )
{
	cFGLABOX VBox = { (float)x1, (float)y1,
                      (float)x2, (float)y2,
                      (float)x3, (float)y3,
                      (float)x4, (float)y4 };

	return VBox;
}

//---------------------------------------------------------------------------------------------

cFGLABOX gABox( int angle, cFGLBOX box )
{
	cFGLABOX VBox;

    int Width  = box.v[1].x - box.v[0].x;
    int Height = box.v[1].y - box.v[0].y;


    VBox.v[0].x = FGLM.gRotatedPoint( angle, gPoint(box.v[0].x, box.v[0].y), gPoint(box.v[0].x+Width/2, box.v[0].y+Height/2) ).x;
    VBox.v[0].y = FGLM.gRotatedPoint( angle, gPoint(box.v[0].x, box.v[0].y), gPoint(box.v[0].x+Width/2, box.v[0].y+Height/2) ).y;

    VBox.v[1].x = FGLM.gRotatedPoint( angle, gPoint(box.v[1].x, box.v[0].y), gPoint(box.v[0].x+Width/2, box.v[0].y+Height/2) ).x;
    VBox.v[1].y = FGLM.gRotatedPoint( angle, gPoint(box.v[1].x, box.v[0].y), gPoint(box.v[0].x+Width/2, box.v[0].y+Height/2) ).y;

    VBox.v[2].x = FGLM.gRotatedPoint( angle, gPoint(box.v[1].x, box.v[1].y), gPoint(box.v[0].x+Width/2, box.v[0].y+Height/2) ).x;
    VBox.v[2].y = FGLM.gRotatedPoint( angle, gPoint(box.v[1].x, box.v[1].y), gPoint(box.v[0].x+Width/2, box.v[0].y+Height/2) ).y;

    VBox.v[3].x = FGLM.gRotatedPoint( angle, gPoint(box.v[0].x, box.v[1].y), gPoint(box.v[0].x+Width/2, box.v[0].y+Height/2) ).x;
    VBox.v[3].y = FGLM.gRotatedPoint( angle, gPoint(box.v[0].x, box.v[1].y), gPoint(box.v[0].x+Width/2, box.v[0].y+Height/2) ).y;

	return VBox;
}

//---------------------------------------------------------------------------------------------

cFGLABOX gABox(  int x, int y, int angle, cFGLBOX box )
{
	cFGLABOX VBox;

    VBox.v[0].x = FGLM.gRotatedPoint( angle, gPoint(box.v[0].x, box.v[0].y ), gPoint( x, y ) ).x;
    VBox.v[0].y = FGLM.gRotatedPoint( angle, gPoint(box.v[0].x, box.v[0].y ), gPoint( x, y ) ).y;

    VBox.v[1].x = FGLM.gRotatedPoint( angle, gPoint(box.v[1].x, box.v[0].y ), gPoint( x, y ) ).x;
    VBox.v[1].y = FGLM.gRotatedPoint( angle, gPoint(box.v[1].x, box.v[0].y ), gPoint( x, y ) ).y;

    VBox.v[2].x = FGLM.gRotatedPoint( angle, gPoint(box.v[1].x, box.v[1].y ), gPoint( x, y ) ).x;
    VBox.v[2].y = FGLM.gRotatedPoint( angle, gPoint(box.v[1].x, box.v[1].y ), gPoint( x, y ) ).y;

    VBox.v[3].x = FGLM.gRotatedPoint( angle, gPoint(box.v[0].x, box.v[1].y ), gPoint( x, y ) ).x;
    VBox.v[3].y = FGLM.gRotatedPoint( angle, gPoint(box.v[0].x, box.v[1].y ), gPoint( x, y ) ).y;

	return VBox;
}

//---------------------------------------------------------------------------------------------

cFGLCIRCLE gCircle( int x, int y, int r )
{
	cFGLCIRCLE C = { x, y, r };
	return C;
}

//---------------------------------------------------------------------------------------------

cFGLC gColor4i( int r, int g, int b, int a )
{
    cFGLC Color;

    Color.r = float(r)/255;
    Color.g = float(g)/255;
    Color.b = float(b)/255;
    Color.a = float(a)/255;

    return Color;
}

//---------------------------------------------------------------------------------------------

cFGLC gColor4f( float r, float g, float b, float a )
{
    cFGLC Color;

    Color.r = float(r);
    Color.g = float(g);
    Color.b = float(b);
    Color.a = float(a);

    return Color;
}

//---------------------------------------------------------------------------------------------

cFGLC gColor( cFGLC CL, float a )
{
	CL.a = a;

    return CL;
}

//---------------------------------------------------------------------------------------------

cFGLC gColorVM( cFGLC CL, float M )
{
	CL.r = CL.r * M;
	CL.g = CL.g * M;
	CL.b = CL.b * M;

	return CL;
}

//---------------------------------------------------------------------------------------------

cFGLC gColorVA( cFGLC CL, float A )
{
	CL.r = CL.r + A;
	CL.g = CL.g + A;
	CL.b = CL.b + A;

	return CL;
}

//-----------------------------------------------------------------------------------//

cFGLCOLOR cFGLCOLOR::operator+( cFGLCOLOR C1 )
{
	cFGLCOLOR Color;

	Color.a = C1.a + a;
	Color.r = C1.r + r;
	Color.g = C1.g + g;
	Color.b = C1.b + b;

	return Color;
}

//--------------------------------------------------------------------------------

cFGLCOLOR cFGLCOLOR::operator-( cFGLCOLOR C1)
{
	cFGLCOLOR Color;

	Color.a = a - C1.a;
	Color.r = r - C1.r;
	Color.g = g - C1.g;
	Color.b = b - C1.b;

	return Color;
}

//--------------------------------------------------------------------------------

cFGLCOLOR cFGLCOLOR::operator+( float i )
{
	cFGLCOLOR Color = itself();

	Color.r = Color.r + i;
	Color.g = Color.g + i;
	Color.b = Color.b + i;

	return Color;
}

//--------------------------------------------------------------------------------

cFGLCOLOR cFGLCOLOR::operator-( float i )
{
	cFGLCOLOR Color = itself();

	Color.r = Color.r - i;
	Color.g = Color.g - i;
	Color.b = Color.b - i;

	return Color;
}

//--------------------------------------------------------------------------------

bool cFGLCOLOR::operator==( cFGLCOLOR C )
{
	bool result = true;

    if( r != C.r ) return false;
    if( g != C.g ) return false;
    if( b != C.b ) return false;
    if( a != C.a ) return false;

	return result;
}

//--------------------------------------------------------------------------------

cFGLCOLOR cFGLCOLOR::operator*(float i)
{
	cFGLCOLOR Color = itself();
	
	Color.r = Color.r * i;
	Color.g = Color.g * i;
	Color.b = Color.b * i;
	
	return Color;
}

//--------------------------------------------------------------------------------

cFGLCOLOR cFGLCOLOR::itself()
{
	cFGLCOLOR Color;

	Color.r = r;
	Color.g = g;
	Color.b = b;
	Color.a = a;

	return Color;
}

//------------------------------------------------------------------------------

cFGLVERTEX gVertex( float x, float y )
{
        cFGLVERTEX v = { x, y };

        return v;
}

//------------------------------------------------------------------------------

cFGLVEC VerToVec( cFGLVERTEX V )
{
	return gVec( V.x, V.y );
}

//------------------------------------------------------------------------------

cFGLVERTEX VecToVer( cFGLVEC V )
{
	return gVertex( V.x, V.y );
}

//------------------------------------------------------------------------------

cFGLPOLY gPoly( int c, cFGLVERTEX * v )
{
	cFGLPOLY Poly;

	Poly.I( c, v );

	return Poly;
}

//------------------------------------------------------------------------------

void cFGLPOLY::P( int x, int y )
{
    cFGLVERTEX * vv = new cFGLVERTEX[count+1];
	for( int i=0; i<count; i++ )
	{
		vv[i] = v[i];
	}

	count++;
	vv[count] = gVertex( x, y );

	v = vv;
}

//------------------------------------------------------------------------------

void cFGLPOLY::I( int c )
{
	if(v) delete [] v;
	v = new cFGLVERTEX[c];
	count = c;
}

//------------------------------------------------------------------------------

void cFGLPOLY::I( int c, cFGLVERTEX * nv )
{
	if (v) delete [] v;
	count = c;
	v = nv;
}

//------------------------------------------------------------------------------

bool cFGLCOLLISION::operator==( bool b )
{
	bool result = false;

	for( int x=0; x<8; x++ )
	{
		if( v[x] == true ){ result = true; break; }
	}

	if( result == b ) result = true; else result = false;

	return result;
}

