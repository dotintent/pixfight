//
//  DrawingContext.hpp
//  PixFight
//
//  Created by Marcin Małysz on 04/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include "Core-pch.hpp"
#include "FurionGL.hpp"
#include "ShaderProgram.hpp"
#include "xMath.h"
#include "CFrustum.hpp"

class DrawingContext {
  
public:
    
    typedef struct {
        int w; int h;
    } screenSize;
    
    DrawingContext(const std::string & shaderPath,
                   const int & widht = 1024,
                   const int & height = 768);
    ~DrawingContext() noexcept;
    
    cFGLC fontColor;
    
    glMatrix4x4 * getProjection() const { return _projection; }
    glMatrix4x4 * getModelView() const { return _modelView; }
    GLShader * getTileShader() const { return _tileShader; }
    CFrustum * getFrustum() const { return _frustum; }
    
    const GLint & getProjLocation() const { return _projLoc; }
    const GLint & getModelLocation() const { return _modelLoc; }
    const GLint & getColorLocation() const { return _colorLoc; }
    const GLint & getCoordLocation() const { return _tcoordLoc; }

    void calculateFrustum();
    void unbindVertexArray();
    
    const screenSize & getScreenSize() const { return _size; }
    
private:

    CFrustum *_frustum;

    screenSize _size;
    
    glMatrix4x4 *_projection;
    glMatrix4x4 *_modelView;
    
    GLShader *_tileShader;
    
    GLint _projLoc;
    GLint _modelLoc;
    GLint _colorLoc;
    GLint _tcoordLoc;
};
