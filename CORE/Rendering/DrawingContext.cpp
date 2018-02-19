//
//  DrawingContext.cpp
//  PixFight
//
//  Created by Marcin Małysz on 04/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include "DrawingContext.hpp"

DrawingContext::DrawingContext(const std::string & shaderPath,
                               const int & widht,
                               const int & height)
: _size({widht, height}) {
    
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
    glActiveTexture(GL_TEXTURE0);
    
    _projection = new glMatrix4x4();
    _modelView = new glMatrix4x4();
    
    _tileShader = new GLShader();
    
    if (!_tileShader->loadShader(shaderPath)) {
        std::cerr << "Could not load shader: " << shaderPath << std::endl;
    }
    
    glBindAttribLocation(_tileShader->getProgramId(), ATTRIB_VERTEX, "position");
    glBindAttribLocation(_tileShader->getProgramId(), ATTRIB_COORDS, "coord");
    
    _tileShader->linkProgram();
    
    glUseProgram(_tileShader->getProgramId());
    
    _projLoc   = glGetUniformLocation(_tileShader->getProgramId(), "projection");
    _modelLoc  = glGetUniformLocation(_tileShader->getProgramId(), "modelView");
    _colorLoc  = glGetUniformLocation(_tileShader->getProgramId(), "rcolor");
    _tcoordLoc = glGetUniformLocation(_tileShader->getProgramId(), "tpos");
    
    _projection->loadIdentity();
    _projection->setOrtho2D(0, widht, height, 0);
    _modelView->loadIdentity();

    _frustum = new CFrustum();
}

DrawingContext::~DrawingContext() noexcept {

    delete _frustum;
    _frustum = nullptr;

    glUseProgram(0);
    unbindVertexArray();
    
    if (_projection) {
        delete _projection;
        _projection = nullptr;
    }
    
    if (_modelView) {
        delete _modelView;
        _modelView = nullptr;
    }
    
    if (_tileShader) {
        _tileShader->end();
        delete _tileShader;
        _tileShader = nullptr;
    }
    
    _projLoc = 0;
    _modelLoc = 0;
    _colorLoc = 0;
    _tcoordLoc = 0;
}

void DrawingContext::calculateFrustum() {

    _frustum->modelview = _modelView->getMatrix();
    _frustum->projection = _projection->getMatrix();

    _frustum->calculateFrustum();
}

void DrawingContext::unbindVertexArray() {
    
    glBindVertexArray(0);
}
