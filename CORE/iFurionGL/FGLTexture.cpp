#include "FurionGL.hpp"

//----------------------------------------------------------------------------

cFGLTEXTURE::~cFGLTEXTURE()
{
	Free();
}

//----------------------------------------------------------------------------

void cFGLTEXTURE::SetTile( int width, int height, bool separator )
{
    if( width > 0 && height > 0 )
    {
        
        TileWidth  = width;
        TileHeight = height;
        
        tile_separator = separator;
        
        if( !tile_separator )
        {
            tiles_x = Width / TileWidth;
            tiles_y = Height / TileHeight;
        }
        else
        {
            tiles_x = Width / (TileWidth+1);
            tiles_y = Height / (TileHeight+1);
        }
        
    }
    else {
        
        tiles_x = 0;
        tiles_y = 0;
    }
}

//----------------------------------------------------------------------------

void cFGLTEXTURE::Free()
{
	active = false;
}

//----------------------------------------------------------------------------

cFGLBOX cFGLTEXTURE::ClientBox()
{
	if( TileWidth > 0 ) return gBox( 0, 0, TileWidth, TileHeight );
	else return gBox( 0, 0, Width, Height );
}

//----------------------------------------------------------------------------

int cFGLTEXTURE::iTilesDistance( int x, int y, int ex, int ey )
{
    int result = 0;

    if( (tiles_x > 0) && (tiles_y>0) )
    {

            if( ey-y == 0 ) result = ex - x;
            if( ey-y == 1 ) result = (tiles_x-x) + ex;


            if( ey-y > 1 )
            {
                    result = (tiles_x-x) + ex;
                    result = result + (ey-y-1)*tiles_x;
            }
    }

    return result;
}

//----------------------------------------------------------------------------
