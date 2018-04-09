#include "FurionGL.hpp"

int aTick;

cFGLM FGLM;

//---------------------------------------------------------------------------------------------

cFGLC FGLFC	=	{ 1.0f, 1.0f, 1.0f, 1.0f };
cFGLC FGLZC	=	{ 0.0f, 0.0f, 0.0f, 0.0f };
cFGLC FGLHC	=	{ 1.0f, 1.0f, 1.0f, 0.5f };
cFGLC FGLRed	=	{ 1.0f, 0.0f, 0.0f, 1.0f };
cFGLC FGLGreen	=	{ 0.0f, 1.0f, 0.0f, 1.0f };
cFGLC FGLBlue	=	{ 0.0f, 0.0f, 1.0f, 1.0f };
cFGLC FGLYellow =	{ 1.0f, 1.0f, 0.0f, 1.0f };
cFGLC FGLWhite	=	{ 1.0f, 1.0f, 1.0f, 1.0f };
cFGLC FGLBlack	=	{ 0.0f, 0.0f, 0.0f, 1.0f };
cFGLC FGLGray	=	{ 0.33f, 0.33f, 0.33f, 1.0f };
cFGLC FGLSilver =	{ 0.66f, 0.66f, 0.66f, 1.0f };
cFGLC FGLUgly	=	{ 1.0f,  0.0f, 1.0f, 1.0f };

//float PI = 3.1415926f;

void glColor4f( cFGLC CL )
{
#if TARGET_OS_IPHONE
	glColor4f( CL.r, CL.g, CL.b, CL.a );
#endif
}

cFGLC FGLLERP(cFGLC c1, cFGLC c2, float bias)
{
    cFGLC Out;
    
    Out.r = ((c1.r * (1.0 - bias)) + (c2.r * bias)); 
    Out.g = ((c1.g * (1.0 - bias)) + (c2.g * bias)); 
    Out.b = ((c1.b * (1.0 - bias)) + (c2.b * bias)); 
    
    return Out;
}

cFGLDD *DescriptorFromTexture(cFGLT * texture)
{
    if(texture == NULL){
        std::cerr<< "Descriptor Error FGLT is NULL" << std::endl;
        return nullptr;
    }

    cFGLDD* _eSDD = new cFGLDD;
    _eSDD->Clean();

    cFGLC C = FGLFC;

    _eSDD->tex = texture;
    _eSDD->destination = DD_BOX;
    _eSDD->SelectTile( 0, 0 );
    _eSDD->Dest = gBox( 0, 0, texture->TileWidth, texture->TileHeight );
    _eSDD->PassRGBA( C.r, C.g, C.b, C.a );
    _eSDD->rot = 0;

    return _eSDD;
}
