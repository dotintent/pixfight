

//-----------------------------------------------------------------------------------//

#include "FurionGL.hpp"

//---------------------------------------------------------------------------

void cFGLDRAWINGDESCRIPTOREX::SetHPoint( int nx, int ny )
{
    hpointx = nx; hpointy = ny;
}

//----------------------------------------------------------------------------

void cFGLDRAWINGDESCRIPTOREX::SetPos( int nx, int ny, int nhx, int nhy )
{
    hpointx = nhx; hpointy = nhy;
    x = nx; y = ny;
}

//---------------------------------------------------------------------------

void cFGLDRAWINGDESCRIPTOR::SelectTile(int x, int y)
{
	if( x >= tex->tiles_x )
	{
		y = y + ( x / (tex->tiles_x != 0 ? tex->tiles_x : 1 ) );
		x = ( x - tex->tiles_x ) - (( x - tex->tiles_x )/ (tex->tiles_x != 0 ? tex->tiles_x : 1 ) )*tex->tiles_x;
	}

	if(tex->tile_separator)
	{
		Source = gBox( x*tex->TileWidth+x, y*tex->TileHeight+y,
					   x*tex->TileWidth+x+tex->TileWidth, y*tex->TileHeight+y+tex->TileHeight );
	}
	else
	{
  		Source = gBox( x*tex->TileWidth, y*tex->TileHeight,
  					   x*tex->TileWidth+tex->TileWidth, y*tex->TileHeight+tex->TileHeight );

	}
}

//----------------------------------------------------------------------------

void cFGLDRAWINGDESCRIPTOREX::SelectTile(int x, int y)
{

	if( x >= tex->tiles_x )
	{
		x = ( x - tex->tiles_x ) - (( x - tex->tiles_x )/(tex->tiles_x != 0 ? tex->tiles_x : 1 ))*tex->tiles_x;
		y = y + (( x - tex->tiles_x )/(tex->tiles_x != 0 ? tex->tiles_x : 1 )) + 1;
	}

	if(tex->tile_separator)
	{
		Source = gBox( x*tex->TileWidth+x, y*tex->TileHeight+y,
                       x*tex->TileWidth+x+tex->TileWidth, y*tex->TileHeight+y+tex->TileHeight );
	}

	else
	{

		Source = gBox( x*tex->TileWidth, y*tex->TileHeight,
                       x*tex->TileWidth+tex->TileWidth, y*tex->TileHeight+tex->TileHeight );
	}
}

//----------------------------------------------------------------------------

void cFGLDRAWINGDESCRIPTOR::PassRGBA(float nr, float ng, float nb, float na)
{
	C.a = na;
	C.r = nr;
	C.g = ng;
	C.b = nb;
	passedRGBA = true;
}

//----------------------------------------------------------------------------

void cFGLDRAWINGDESCRIPTOR::PassRGBA(cFGLC CL)
{
	C = CL;
	passedRGBA = true;
}

//----------------------------------------------------------------------------

void cFGLDRAWINGDESCRIPTOREX::PassRGBA(float nr, float ng, float nb, float na)
{
	C.a = na;
	C.r = nr;
	C.g = ng;
	C.b = nb;
	passedRGBA = true;
}

//----------------------------------------------------------------------------

void cFGLDRAWINGDESCRIPTOREX::PassRGBA(cFGLC CL)
{
	C = CL;
	passedRGBA = true;
}

//----------------------------------------------------------------------------

void cFGLDRAWINGDESCRIPTOREX::Clean()
{
	tex = NULL;

	Source = gBox( 0, 0, 0, 0 );

	x = 0;
	y = 0;
	hpointx = 0;
	hpointy = 0;
	rot = 0;

    PassRGBA( FGLFC );

}

//----------------------------------------------------------------------------

void cFGLDRAWINGDESCRIPTOR::Clean()
{
	tex = NULL;

	Source = gBox( 0, 0, 0, 0 );
	Dest   = gBox( 0, 0, 0, 0 );

	rot = 0;

    PassRGBA( FGLFC );
    RefCount = 0;
}

//----------------------------------------------------------------------------

cFGLDRAWINGDESCRIPTOR cFGLDRAWINGDESCRIPTOR::itself()
{
	cFGLDRAWINGDESCRIPTOR _DD;

	_DD.C = C;
	_DD.Dest = Dest;
	_DD.ADest = ADest;
	_DD.passedRGBA = passedRGBA;
	_DD.rot = rot;
	_DD.Source = Source;
	_DD.tex = tex;
	_DD.buffered = buffered;
	_DD.destination = destination;
	return _DD;
}

//----------------------------------------------------------------------------

cFGLDRAWINGDESCRIPTOREX cFGLDRAWINGDESCRIPTOREX::itself()
{
	cFGLDRAWINGDESCRIPTOREX _DD;

	_DD.C = C;
	_DD.x = x; _DD.y = y;
	_DD.hpointx = hpointx;
	_DD.hpointy = hpointy;
	_DD.passedRGBA = passedRGBA;
	_DD.rot = rot;
	_DD.Source = Source;
	_DD.tex = tex;
	_DD.buffered = buffered;
	return _DD;
}

//----------------------------------------------------------------------------
