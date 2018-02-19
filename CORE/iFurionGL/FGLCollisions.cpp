

//-----------------------------------------------------------------------------------//

#include "FurionGL.hpp"

//---------------------------------------------------------------------------

bool cFGLMATH::bCollision( cFGLPOINT Point, cFGLBOX Box )
{

	bool result = false;

	if( (Point.x >= Box.v[0].x) && (Point.x <= Box.v[1].x ) )
	{
		if( (Point.y >= Box.v[0].y) && (Point.y <= Box.v[1].y ) )
		{
			result = true;
        }
    }

	return result;

}

//---------------------------------------------------------------------------

bool cFGLMATH::bCollision( cFGLBOX Box1, cFGLBOX Box2)
{
	if(
	Box1.v[1].x < Box2.v[0].x	||
	Box1.v[0].x > Box2.v[1].x	||
	Box1.v[0].y > Box2.v[1].y	||
	Box1.v[1].y < Box2.v[0].y )
	{ return false; }
	else
	{ return true; }
}

//---------------------------------------------------------------------------

bool cFGLMATH::bCollision( cFGLLINE Line1, cFGLLINE Line2 )
{
	return true;
}

//---------------------------------------------------------------------------

bool cFGLMATH::bCollision( cFGLPOINT P, cFGLCIRCLE C )
{

	bool result = false;

	if( iDistance( P.x, P.y, C.x, C.y ) <= C.r ) result = true;

	return result;

}

//---------------------------------------------------------------------------

bool cFGLMATH::bCollision( cFGLABOX Box1, cFGLABOX Box2 )
{
	bool result = false;

		for( int x=0; x<4; x++ )
		{
			if( bCollision( gPoint( Box1.v[x].x, Box1.v[x].y ), Box2 )) result = true;
			if( bCollision( gPoint( Box2.v[x].x, Box2.v[x].y ), Box1 )) result = true;
		}

	return result;
}

//---------------------------------------------------------------------------

bool cFGLMATH::bCollision( cFGLBOX Box1, cFGLABOX Box2 )
{
	bool result;

	cFGLABOX ABox1 = gABox( 0, Box1 );

	result = bCollision( ABox1, Box2 );

	return result;
}

//---------------------------------------------------------------------------

bool8 cFGLMATH::bCollisionData( cFGLABOX Box1, cFGLABOX Box2 )
{
	bool8 result;
        int x;
		for( x=0; x<4; x++ )
		{
			if( bCollision( gPoint( Box1.v[x].x, Box1.v[x].y ), Box2 )) result.v[x] = true;
		}
		for( x=0; x<4; x++ )
		{
			if( bCollision( gPoint( Box2.v[x].x, Box2.v[x].y ), Box1 )) result.v[x+4] = true;
		}

	return result;
}

//---------------------------------------------------------------------------

cFGLCOLLISION cFGLMATH::bCollisionC( cFGLABOX Box1, cFGLABOX Box2 )
{
	cFGLCOLLISION result;

		int x;
		for( x=0; x<4; x++ )
		{
			if( bCollision( gPoint( Box1.v[x].x, Box1.v[x].y ), Box2 )){ result.v[x] = true;  }
		}
		for( x=0; x<4; x++ )
		{
			if( bCollision( gPoint( Box2.v[x].x, Box2.v[x].y ), Box1 )){ result.v[x+4] = true; }
		}

	return result;
}

//---------------------------------------------------------------------------

bool cFGLMATH::bCollision( cFGLPOINT Point, cFGLABOX Box )
{
	bool result = false;

	int p[4];

	cFGLVEC V1;
	cFGLVEC V2;

	for( int x=0; x<3; x++ )
	{
		V1  = gVec( Box.v[x].x-Box.v[x+1].x, Box.v[x].y-Box.v[x+1].y );
		V2  = gVec( Point.x-Box.v[x+1].x, Point.y-Box.v[x+1].y);
		p[x] = (int)fVecScalarProd( V1, V2 );
	}
		V1  = gVec( Box.v[3].x-Box.v[0].x, Box.v[3].y-Box.v[0].y );
		V2  = gVec( Point.x-Box.v[0].x, Point.y-Box.v[0].y);
		p[3] = (int)fVecScalarProd( V1, V2 );


	if( ( p[0]>0 && p[1]>0 && p[2]>0 && p[3]>0 ) ||
		( p[0]<0 && p[1]<0 && p[2]<0 && p[3]<0 ))
	{ result = true; }

	return result;
}

//---------------------------------------------------------------------------

bool cFGLMATH::bCollision( cFGLPOINT Point, cFGLLINE Line )
{
	bool result;

	result = false;

	return result;
}

//---------------------------------------------------------------------------

bool cFGLMATH::bCollision( cFGLPOINT Point, cFGLPOLY Poly )
{

		int y = Point.y;
        int x = Point.x;
        int i, j = 0;

		bool oddNODES = false;
	for (i=0; i<Poly.count; i++){
	j++; if (j==Poly.count) j=0;
    if ((Poly.v[i].y<y && Poly.v[j].y>=y)
    ||  (Poly.v[j].y<y && Poly.v[i].y>=y)) {
      if(Poly.v[i].x+float(y-Poly.v[i].y)/float(Poly.v[j].y-Poly.v[i].y)*float(Poly.v[j].x-Poly.v[i].x)<x) {
        oddNODES=!oddNODES; }}}

	return oddNODES;

}

//---------------------------------------------------------------------------

bool cFGLMATH::bCollision( cFGLPOLY PolyA, cFGLPOLY PolyB )
{
    int x;

	for( x=0; x<PolyA.count; x++ )
	{
		if( bCollision( gPoint(PolyA.v[x].x,PolyA.v[x].y), PolyB )) return true;
	}

	for( x=0; x<PolyB.count; x++ )
	{
		if( bCollision( gPoint(PolyB.v[x].x,PolyB.v[x].y), PolyA )) return true;
	}

	return false;
}

//---------------------------------------------------------------------------
