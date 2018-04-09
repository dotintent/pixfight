

//-----------------------------------------------------------------------------------//

#include "FurionGL.hpp"

//----------------------------------------------------------------------------

int cFGLMATH::iDistance( int x1, int y1, int x2, int y2 )
{
	int result;

	result = (int)sqrt( float( ( x2 - x1 )*( x2 - x1 ) + ( y2 - y1 )*( y2 - y1 ) ) );

	return result;
}

//----------------------------------------------------------------------------

int cFGLMATH::iDistance( cFGLPOINT p1, cFGLPOINT p2 )
{
    return iDistance( p1.x, p1.y, p2.x, p2.y );
}

//----------------------------------------------------------------------------

float cFGLMATH::fDistance( float x1, float y1, float x2, float y2 )
{
	float result;

	result = sqrt(( x2 - x1 )*( x2 - x1 ) + ( y2 - y1 )*( y2 - y1 ));

	return result;
}

//----------------------------------------------------------------------------

float cFGLMATH::fDistance( cFGLPOINT p1, cFGLPOINT p2 )
{
    return fDistance( p1.x, p1.y, p2.x, p2.y );
}

//----------------------------------------------------------------------------

cFGLPOINT cFGLMATH::gRotatedPoint( int angle, cFGLPOINT p1, cFGLPOINT p2)
{
	cFGLPOINT result;

    float fangle = fDegToRad( angle );
    // result.x = p2.x + sin( fangle + aangle ) * dist;
    // result.y = p2.y - cos( fangle + aangle ) * dist;

    result.x = p2.x + float((p2.x-p1.x)) * cos(fangle) - float((p2.y-p1.y)) * sin(fangle);
    result.y = p2.y + float((p2.x-p1.x)) * sin(fangle) + float((p2.y-p1.y)) * cos(fangle);

	return result;
}

//----------------------------------------------------------------------------

void cFGLMATH::PingPong(int &ivar, bool &bvar, int Max, int Speed = 1)
{
    if(Max > 0){
        switch((int)bvar)
        {
        case 0: // PING ^)^
            ivar+=Speed;
            if(ivar >= Max) bvar = true;
            break;
        case 1: // PONG ^(^
            ivar-=Speed;
            if(ivar <= 0) bvar = false;
            break;
        }
    }
    else ivar = 0;
}

//---------------------------------------------------------------------------

void cFGLMATH::PingPongW8(int &ivar, bool &bvar, int Max, int Wait, int &iWait, int Speed = 1)
{
    if(Max > 0){
        switch((int)bvar)
        {
        case 0: // PING ^)^
            if(ivar < Max){ ivar+=Speed; }
            if(ivar >= Max){ ivar = Max; iWait++; if(iWait >= Wait){ bvar = true; iWait = 0;}}
            break;
        case 1: // PONG ^(^
            if(ivar > 0){ ivar-=Speed; }
            if(ivar <= 0){ ivar = 0; iWait++; if(iWait >= Wait){ bvar = false; iWait = 0; }}
            break;
        }
    }
    else ivar = 0;
}

//---------------------------------------------------------------------------

void cFGLMATH::Ping(int &ivar, int Max)
{
    ivar++;
    if(ivar >= Max) ivar = 0;
}

//---------------------------------------------------------------------------

void cFGLMATH::Pong(int &ivar, int Max)
{
    ivar--;
    if(ivar == 0) ivar = Max;
}

//---------------------------------------------------------------------------

void cFGLMATH::PingW8(int &ivar, int Max, int Wait, int &iWait )
{
    ivar++;
    if(ivar >= Max) ivar = 0;
}

//---------------------------------------------------------------------------

void cFGLMATH::PongW8(int &ivar, int Max, int Wait, int &iWait )
{
    ivar--;
    if(ivar == 0) ivar = Max;
}

//---------------------------------------------------------------------------

