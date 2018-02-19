//
//  Mesh2d.hpp
//  PixFight
//
//  Created by Marcin Małysz on 05/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include "Core-pch.hpp"
#include "FurionGL.hpp"
#include "DrawingContext.hpp"

class Mesh2d {

public:

    cFGLCOLOR Color;
    xVec2 pos;
    float rot;
    float scale;

    Mesh2d(cFGLT * Texture);
    ~Mesh2d() noexcept;

    void render(const DrawingContext & context);
    void setAnimation(const int & tx, const int & ty);

private:

    GLuint _vbo; //vertices
    GLuint _ibo; //indexes

    GLuint _VAO;

    xMat33 _myRot;
    xMat34 _MV;

    float _w;
    float _h;

    cFGLDD *_DD;

    float _ltx, _lty;
    float _mulx, _muly;
};
