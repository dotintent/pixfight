#pragma once

/**

 This library was created by Przemysław Rezoner Sikorski
 You can find some of his work here http://rezoner.net/

 Redistribution or modification must be agreed.

 Yes this library is old, yes it does not follow current coding standards, deal with it.
 */

#include <string.h>
#include "xSimpleTypes.h"
#include "glMatrix4x4.hpp"
#include "xMat34.h"
#include "xVec2.h"
#include "xVec3.h"
#include "xQuat.h"

#include "ShaderProgram.hpp"

//-----------------------------------------------------------------------------------
// FGL
//-----------------------------------------------------------------------------------

#define cFGLT		cFGLTEXTURE					// Texture
#define cFGLM		cFGLMATH					// Math module
#define cFGLDD		cFGLDRAWINGDESCRIPTOR		// Drawing Params Pack
#define cFGLDDEX	cFGLDRAWINGDESCRIPTOREX		// Drawing ParamsEx Pack
#define cFGLC		cFGLCOLOR					// Lol :P
#define cFGLVEC		cFGLVECTOR2D				// 2D Vector

// comment defines below if they cause conflict with existing names
#define cBox	 cFGLBOX
#define cABox	 cFGLABOX
#define cLine	 cFGLLINE
#define cPoint	 cFGLPOINT
#define cCircle	 cFGLCIRCLE

typedef xU16 xbyte;
// defines what kind of box will you pass to cFGLDD structure
const xbyte DD_BOX  = 1;
const xbyte DD_ABOX = 2;

// Selectors
const int FGL_DDL = 1;

#define FGUI_3D			2

//-----------------------------------------------------------------------------------//
struct bool4{ bool v[4]; };
struct bool8{ bool v[8]; };

// class int8{ public: int v[8]; void operator=( bool8 b); };
//-----------------------------------------------------------------------------------//
struct cFGLVERTEX{ float x, y; };
cFGLVERTEX gVertex( float x, float y );
//-----------------------------------------------------------------------------------//
struct cFGLBOX {
	    cFGLVERTEX v[2];
        cFGLBOX operator+( float i );
        cFGLBOX operator/( float i );
};

cFGLBOX   gBox( float L, float T, float R, float B );			// Box from left, top, right, bottom
cFGLBOX	  gWHBox( float L, float T, float W, float H );			// Box from (x,y) width and height
cFGLBOX	  gCenteredBox( float x, float y, float xc, float yc, float W, float H );
														// x, y   = center on screen
														// xc, yc = center on box
//-----------------------------------------------------------------------------------//
struct cFGLABOX {
	cFGLVERTEX v[4];
};

cFGLABOX   gABox( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4 );
cFGLABOX   gABox( int angle, cFGLBOX box );
cFGLABOX   gABox( int x, int y,int angle, cFGLBOX box );
//-----------------------------------------------------------------------------------//
struct cFGLPOINT{ int x, y; };
cFGLPOINT gPoint( int x, int y );
//-----------------------------------------------------------------------------------//
struct cFGLLINE{ cFGLVERTEX v[2]; };
cFGLLINE gLine ( int x1, int y1, int x2, int y2  );
//-----------------------------------------------------------------------------------//
struct cFGLCIRCLE{ int x, y; int r; };
cFGLCIRCLE gCircle( int x, int y, int r );
//-----------------------------------------------------------------------------------//
struct cFGLPOLY{ cFGLVERTEX * v; int count; void I(int c); void P(int x, int y); void I( int c, cFGLVERTEX * nv ); };
cFGLPOLY gPoly( int c, cFGLVERTEX * v );
//-----------------------------------------------------------------------------------//
typedef unsigned int u32;
//-----------------------------------------------------------------------------------//

struct cFGLCOLOR {

	float r,g,b,a;
	cFGLCOLOR operator+( cFGLCOLOR C1 );
	cFGLCOLOR operator-( cFGLCOLOR C1 );
	cFGLCOLOR operator+( float i );
	cFGLCOLOR operator-( float i );
    bool      operator==( cFGLCOLOR C );
	cFGLCOLOR operator*(float i);

	cFGLCOLOR itself();
};

//-----------------------------------------------------------------------------------//

cFGLC gColor4i( int r, int g, int b, int a );			// GLCOLOR4f from 4 values ( 0 - 255 )
cFGLC gColor4f( float r, float g, float b, float a );	// GLCOLOR4f from 4 values ( 0.0f - 1.0f )
cFGLC gColor( cFGLC CL, float a );						// GLCOLOR4f with changed alpha
cFGLC gColorVM( cFGLC CL, float M );					// Multiply 'M' with RGB values
cFGLC gColorVA( cFGLC CL, float A );					// Add 'A' to RGB values

void glColor4f( cFGLC CL );		// if u know what is glColor4f(...).... u know what it is

extern cFGLC FGLLERP(cFGLC c1, cFGLC c2, float bias);

extern cFGLC FGLFC;	// FGL FULL COLOR a.k.a clWhite
extern cFGLC FGLZC;	// FGL ZERO COLOR
extern cFGLC FGLHC;	// FGL HALF COLOR = clWhite with 50% alpha

// a rest of predefined colors
extern cFGLC FGLRed;
extern cFGLC FGLGreen;
extern cFGLC FGLBlue;
extern cFGLC FGLYellow;
extern cFGLC FGLWhite;
extern cFGLC FGLBlack;
extern cFGLC FGLGray;
extern cFGLC FGLSilver;
extern cFGLC FGLUgly;

//-----------------------------------------------------------------------------------//

#define FGL_DASHLINE 0xaaaa		//	a dashed line
#define FGL_SOLIDLINE 0xffff	//  a solid line

//-----------------------------------------------------------------------------------//

#define FGL_NORMAL		0		// NORMAL LOOP
#define FGL_BIDI		1		// BIDI LOOP
#define FGL_BIDI_WAIT	2		// BIDI WAIT LOOP

//-----------------------------------------------------------------------------------//

class cFGLCOLLISION {
public :

    cFGLCOLLISION() {
        memset(v, 0, 8);
    }

	bool v[8];

	bool operator==( bool b );

};

//-----------------------------------------------------------------------------------//

class cViewport	{	// Class describing a Viewport size
public :

    cViewport()
    : Left(0)
    , Top(0)
    , NewWidth(0)
    , NewHeight(0)
    , Width(0)
    , Height(0)
    , scale(0) {}

    int Left, Top;
    int NewWidth, NewHeight;
	int Width, Height;
	float scale;

};

//-----------------------------------------------------------------------------------//

class cFGLVECTOR2D {
public :

    cFGLVECTOR2D()
    : norm(false)
    , x(0) , y(0)
    , ox(0), oy(0) {}

	bool norm;		// is vector normalized ?
	float x, y;		// vector coordinates
	float ox, oy;	// keeps original vector coordinates ( when normalized )

	cFGLVECTOR2D operator+( cFGLVECTOR2D i );
	cFGLVECTOR2D operator-( cFGLVECTOR2D i );
};

//-----------------------------------------------------------------------------------//

cFGLVEC gVec( float x, float y );
cFGLVEC gVec( cFGLPOINT P );
cFGLVEC gNormVec( float x, float y );

cFGLVEC VerToVec( cFGLVERTEX );
cFGLVERTEX VecToVer( cFGLVEC );

//---------------------------------------------------------------------------

class cFGLMATH {
public :

    // inc, dec functions
	void PingPong(int &ivar, bool &bvar, int Max, int Speed); // ^<>^
    void PingPongW8(int &ivar, bool &bvar, int Max, int Wait, int &iWait, int Speed); // |^<>^|
    void Ping(int &ivar, int Max); // ^>^ : inc
    void Pong(int &ivar, int Max); // ^<^ : dec
    void PingW8(int &ivar, int Max, int Wait, int &iWait ); // ^>^ : inc
    void PongW8(int &ivar, int Max, int Wait, int &iWait ); // ^<^ : dec

	// point rotated around another point
	cFGLPOINT gRotatedPoint( int angle, cFGLPOINT p1, cFGLPOINT p2);

	// distance between 2 points
	int   iDistance( cFGLPOINT p1, cFGLPOINT p2 );
	int   iDistance( int x1, int y1, int x2, int y2 );
	float fDistance( float x1, float y1, float x2, float y2 );
	float fDistance( cFGLPOINT p1, cFGLPOINT p2 );

	// some collisions, there is a rule that the first argument is
	// always a structure with the less parameters
	bool  bCollision( cFGLPOINT Point , cFGLBOX Box );
	bool  bCollision( cFGLPOINT Point , cFGLABOX ABox );
	bool  bCollision( cFGLPOINT Point , cFGLLINE Line );
	bool  bCollision( cFGLPOINT Point , cFGLPOLY Poly );
	bool  bCollision( cFGLBOX Box1, cFGLBOX Box2 );
	bool  bCollision( cFGLBOX Box1, cFGLABOX Box2 );
	bool  bCollision( cFGLLINE Line1, cFGLLINE Line2 );
	bool  bCollision( cFGLPOINT Point, cFGLCIRCLE Circle );
	bool  bCollision( cFGLABOX Box1, cFGLABOX Box2);
	bool  bCollision( cFGLPOLY PolyA , cFGLPOLY PolyB );
	bool8  bCollisionData( cFGLABOX Box1, cFGLABOX Box2);
	cFGLCOLLISION bCollisionC( cFGLABOX Box1, cFGLABOX Box2 );


	// Scalar Product of two vectors
	float fVecScalarProd( cFGLVEC V1, cFGLVEC V2 );
	float fVecVectorProd( cFGLVEC V1, cFGLVEC V2 );
	// Length of Vector
	float fVecLength( cFGLVEC V );
	int   iVecLength( cFGLVEC V );
	// Angle between two vectors. It's always positive
	float fVecAngle( cFGLVEC V1, cFGLVEC V2 );
	int   iVecAngle( cFGLVEC V1, cFGLVEC V2 );
	// Cosinus of angle between two vectors
	float fVecCosinus( cFGLVEC V1, cFGLVEC V2 );
	// It's angle between vector [ 0, 1 ] and custom vector
	// functiion returns a clock angle ( 0 to 2*PI )
	float fVecClock( cFGLVEC V );
	// Same but vector is translated to [ x, y+1 ]
	float fVecPosClock( float x, float y, cFGLVEC V );
	// this procedure normalizes any vector
	void  NormalizeVec( cFGLVEC * V );

	// Degress to Radians
	float fDegToRad( float Deg );
	int   iDegToRad( float Deg );
	// Vice Versa
	float fRadToDeg( float Rad );
	int   iRadToDeg( float Rad );

	// use it to get rotation angle around point [rx, ry] to make
	// vectors [ ex, ey ] and [ tx, ty ] equal
	float fLookAt( int rx, int ry, int ex, int ey, int tx, int ty );
	int   iLookAt( int rx, int ry, int ex, int ey, int tx, int ty );
	// same but here all vectors are translated by vector [ x, y ]
	int   iLookAt( int x, int y, int rx, int ry, int ex, int ey, int tx, int ty );
	float fLookAt( int x, int y, int rx, int ry, int ex, int ey, int tx, int ty );

};

//-----------------------------------------------------------------------------------/
// IMAGE SUPPORT

class cFGLIMAGE {
public:

    cFGLIMAGE()
    : data(nullptr)
    , size(0)
    , type(0)
    , width(0)
    , height(0) {}

	char * data;
	unsigned int size;
	GLuint type;

	int width, height;
};

//-----------------------------------------------------------------------------------//

class cFGLTEXTURE {		// Texture Object
public :

    cFGLTEXTURE()
    : active(false)
    , cs(false)
    , tile_separator(false)
    , iTexture(0) { }

	~cFGLTEXTURE();

	bool active;			// if texture was loaded or created then active = true
    bool cs;                // if texture was used for FGL::CopyScreen(...)
                            // then CS = true

	int Width, Height;			// should there be a comment ?
	int TileWidth, TileHeight;	// or here:P
	int tiles_x, tiles_y;		// how many tiles are there ( horizontal / vertical )

	bool tile_separator;		// are the tiles separated with single line ?

                                        // Set Tileset values
    void SetTile( int width, int height, bool separator );

	GLuint iTexture;		// identificator used to glBindTexture

    bool mask;                      // mask loaded ?
    bool * bMask;                   // Mask

	cFGLBOX ClientBox();
    int iTilesDistance( int x, int y, int ex, int ey );

    // creates an empty texture
    bool Create( int _Width, int _Height, long Filter );

	// Load texture from specified GL_UNSIGNED_BYTE data
	void Load( GLubyte * data, int width, int height, int bpp, long Filter );

	// basic loading method
	void Free();
	void FreeMask();

	int ivalue;
};

//-----------------------------------------------------------------------------------//

class cFGLDRAWINGDESCRIPTOR {		// class containing all info needed to display a texture
public:

    cFGLDRAWINGDESCRIPTOR ()
    : tex(nullptr)
    , RefCount(0)
    , Source(cFGLBOX())
    , Dest(cFGLBOX())
    , ADest(cFGLABOX())
    , rot(0)
    , passedRGBA(false)
    , C(FGLWhite)
    , destination(0)
    , buffered(false)
    {}

	cFGLTEXTURE *tex;			// a pointer to cFGLTEXTURE object. Some functions
								// won't be working until you set it. For sure
	int RefCount;				// fill this value as first.

	cFGLBOX Source;				// Box which will be copied
	cFGLBOX  Dest;				// And pasted into this Box
	cFGLABOX ADest;

	void SelectTile(int x, int y);	// if the 'tex' is a tileset you can use this
									// method to automaticly adjust the Source Box

	void PassRGBA(float nr, float ng, float nb, float na);
	void PassRGBA(cFGLC CL);

	void Clean();

	float rot;					// rotation in degress ( 0 - 360 )

	bool passedRGBA;
	cFGLC C;

	cFGLDRAWINGDESCRIPTOR itself();

    void operator=(cFGLDRAWINGDESCRIPTOR temp){

        temp.C = C;
        temp.Dest = Dest;
        temp.ADest = ADest;
        temp.passedRGBA = passedRGBA;
        temp.rot = rot;
        temp.Source = Source;
        temp.tex = tex;
        temp.buffered = buffered;
        temp.destination = destination;
    }

	xbyte destination;			// Dest or ADest ?

	bool buffered;				// when buffered is true, class is not able to
								// be Push()'ed by FGLDDLOOP
};

//-----------------------------------------------------------------------------------//

class cFGLDRAWINGDESCRIPTOREX {	// class containing all info needed to display a texture								// plus extra rotation informations
public :						// most od elements are from cGLDRAWINGDESCRIPTOR

    cFGLDRAWINGDESCRIPTOREX ()
    : tex(nullptr)
    , Source(cFGLBOX())
    , x(0), y(0)
    , hpointx(0), hpointy(0)
    , rot(0)
    , passedRGBA(false)
    , C(FGLWhite)
    , buffered(false)
    {}

	cFGLTEXTURE *tex;

	cFGLBOX Source;

    int x; int y;			// where the center should be displayed ?

	int hpointx, hpointy;	// where is the center on texture ?

	void SelectTile(int x, int y);

	void PassRGBA(float nr, float ng, float nb, float na);
	void PassRGBA(cFGLC CL);
	void Clean();

    void SetHPoint( int nx, int ny );			// set only the center on texture

    void SetPos( int nx, int ny, int nhx, int nhy );	// display pos. and center pos.

    float rot;

	bool passedRGBA;
	cFGLC C;

	cFGLDRAWINGDESCRIPTOREX itself();		// returns itself

    void operator=(cFGLDRAWINGDESCRIPTOREX temp){

        temp.C = C;
        temp.x = x; temp.y = y;
        temp.hpointx = hpointx;
        temp.hpointy = hpointy;
        temp.passedRGBA = passedRGBA;
        temp.rot = rot;
        temp.Source = Source;
        temp.tex = tex;
        temp.buffered = buffered;
    }

	bool buffered;				// when buffered is true, class is not able to
                                // be Push()'ed by FGLDDLOOP

};

//-----------------------------------------------------------------------------------

extern cFGLDD eSDD;
extern cFGLDDEX eSDDEX;
extern cFGLMATH FGLM;

//owner is responsible for deleting!
extern cFGLDD *DescriptorFromTexture(cFGLT * texture);

//-----------------------------------------------------------------------------------

