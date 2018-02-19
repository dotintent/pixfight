//
//  FontRender.hpp
//  Untitled
//
//  Created by Marcin Małysz on 04/01/18.
//  Copyright 2018 NoClip. All rights reserved.
//

#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Core-pch.hpp"
#include "FurionGL.hpp"
#include "ShaderProgram.hpp"

class FontRender {

public:
    
    FontRender(const std::string & fontPath);
    ~FontRender();
    
    void setupOpenGL(const std::string & shaderPath);
    void setFontSize(const uint32_t & size);
    void setFontColor(cFGLC c);
    
    void begin();
    
    void drawText(const char *text, float x, float y, float sx, float sy);
    void end();
    
private:
    
    bool isLoaded() { return _libraryLoaded == true; } 
    
    GLuint _tex;
    GLuint _vbo;
    GLuint _vao;
    GLint _uniformTex;
    GLint _uniformColor;
    
    GLShader *_program;
    bool _libraryLoaded;
    
    GLfloat _color[4];
    
    FT_Library _ft;
    FT_Face _face;
};
