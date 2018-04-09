//
//  Mesh2d.cpp
//  PixFight
//
//  Created by Marcin Małysz on 05/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include "Mesh2d.hpp"

typedef struct mdata {
    xVec2 Vert;
    xVec2 UV;
} mdata;

Mesh2d::Mesh2d(cFGLT * Texture) {

    Color = FGLFC;
    pos = xVec2(0, 0);
    rot = 0;
    scale = 1;

    _DD = DescriptorFromTexture(Texture);

    if (_DD->Source.v[0].x == 0 && _DD->Source.v[1].x == 0 &&
        _DD->Source.v[0].y == 0 && _DD->Source.v[1].y == 0) {

        _DD->Source.v[1].x = _DD->tex->Width;
        _DD->Source.v[1].y = _DD->tex->Height;
    }

    _w = _DD->Dest.v[1].x - _DD->Dest.v[0].x;
    _h = _DD->Dest.v[1].y - _DD->Dest.v[0].y;

    const GLushort squareIndex[6] = {
        0,1,2,1,3,2,
    };

    _DD->SelectTile(0, 0);

    //calculate coords
    float tx1 = float(_DD->Source.v[0].x) / float(_DD->tex->Width);
    float ty1 = float(_DD->Source.v[0].y) / float(_DD->tex->Height);
    float tx2 = float(_DD->Source.v[1].x) / float(_DD->tex->Width);
    float ty2 = float(_DD->Source.v[1].y) / float(_DD->tex->Height);

    if (tx1 != 0) {
        _mulx = tx1;
    }
    else if (tx2 != 0) {
        _mulx = tx2;
    }
    else {
        _mulx = 1.0;
    }

    if (ty1 != 0) {
        _muly = ty1;
    }
    else if (ty2 != 0) {
        _muly = ty2;
    }else {
        _muly = 1.0;
    }

    mdata data[4];

    data[0].Vert = xVec2( -_w*0.5f,   _h*0.5f);
    data[1].Vert = xVec2(  _w*0.5f,   _h*0.5f);
    data[2].Vert = xVec2( -_w*0.5f,  -_h*0.5f);
    data[3].Vert = xVec2(  _w*0.5f,  -_h*0.5f);

    data[0].UV = xVec2(tx1,ty2);
    data[1].UV = xVec2(tx2,ty2);
    data[2].UV = xVec2(tx1,ty1);
    data[3].UV = xVec2(tx2,ty1);

    _ltx = 0;
    _lty = 0;

    //create VBO quad

    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, 4*sizeof(mdata), data, GL_STATIC_DRAW);
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, sizeof(mdata), (void*)OFFSET(mdata,Vert));
    glEnableVertexAttribArray(ATTRIB_VERTEX);
    glVertexAttribPointer(ATTRIB_COORDS, 2, GL_FLOAT, 0, sizeof(mdata), (void*)OFFSET(mdata,UV));
    glEnableVertexAttribArray(ATTRIB_COORDS);

    glGenBuffers(1, &_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUMBER_OF_INDICES*sizeof(GLushort), squareIndex, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Mesh2d::~Mesh2d() noexcept {

    if (_DD) {

        delete _DD;
        _DD = nullptr;
    }

    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_ibo);

    glDeleteVertexArrays(1, &_VAO);
}

void Mesh2d::render(const DrawingContext & context) {

    _DD->Dest = gBox( pos.x, pos.y, pos.x+_DD->tex->TileWidth, pos.y+_DD->tex->TileHeight );

    _w = _DD->Dest.v[1].x - _DD->Dest.v[0].x;
    _h = _DD->Dest.v[1].y - _DD->Dest.v[0].y;

    auto modelView = context.getModelView();
    auto projection = context.getProjection();

    modelView->loadIdentity();

    _myRot.rotZ(rot);

    _MV.id();
    _MV.t = xVec3(_DD->Dest.v[0].x + _w/2.0 , _DD->Dest.v[0].y + _h/2.0, 0.0);
    _MV.M = _myRot;

    _MV.getColumnMajor44(modelView->getMatrix());

    modelView->scale(scale, scale, scale);

    glBindTexture(GL_TEXTURE_2D, _DD->tex->iTexture);

    glUniformMatrix4fv(context.getProjLocation(), 1 ,false , projection->getMatrix());
    glUniformMatrix4fv(context.getModelLocation(), 1 ,false , modelView->getMatrix());
    glUniform4f(context.getColorLocation(), Color.r, Color.g, Color.b, Color.a);
    glUniform4f(context.getCoordLocation(), _ltx, _lty, _mulx, _muly);

    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, NUMBER_OF_INDICES, GL_UNSIGNED_SHORT, (void*)0);
}

void Mesh2d::setAnimation(const int & tx, const int & ty) {

    if ((_ltx != tx) || (_lty != ty)) { //update coordinate buffer only if needed

        _ltx = tx;
        _lty = ty;
    }
}
