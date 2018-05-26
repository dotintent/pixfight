//
//  FontRender.cpp
//  Untitled
//
//  Created by Marcin Małysz on 04/01/18.
//  Copyright 2018 NoClip. All rights reserved.
//

#include "FontRender.hpp"

FontRender::FontRender(const std::string & fontPath)
: _tex(0)
, _vbo(0)
, _vao(0)
, _uniformTex(0)
, _uniformColor(0)
, _program(nullptr)
, _libraryLoaded(false) {

    memset(_color, 1, 4);

    if(FT_Init_FreeType(&_ft)) {
        std::cerr << "Could not init freetype library" << std::endl;
        return;
    }

    if(FT_New_Face(_ft, fontPath.c_str(), 0, &_face)) {
        std::cerr << "Could not open font: " << fontPath << std::endl;
        return;
    }

    this->setFontSize(24);

    _libraryLoaded = true;
}

FontRender::~FontRender() {

    if (!this->isLoaded()) {
        return;
    }

    if (_program) {
        delete _program;
        _program = nullptr;
    }

    if (_tex) {
        glDeleteTextures(1, &_tex);
    }

    if (_vbo) {
        glDeleteBuffers(1, &_vbo);
    }

    if (_vao) {
        glDeleteVertexArrays(1, &_vao);
    }

    FT_Done_Face(_face);
    FT_Done_FreeType(_ft);
}

void FontRender::setupOpenGL(const std::string & shaderPath) {

    if (!this->isLoaded()) {
        return;
    }

    if (_program) {
        _program->end();
        delete _program;
    }

    _program = new GLShader();

    if (!_program->loadShader(shaderPath)) {
        std::cerr << "Could not load shader: " << shaderPath << std::endl;
    }

    glBindAttribLocation(_program->getProgramId(), ATTRIB_COORDS, "coord");

    _program->linkProgram();

    glUseProgram(_program->getProgramId());

    _uniformTex = _program->uniformLocation("tex");
    _uniformColor = _program->uniformLocation("color");

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &_tex);
    glBindTexture(GL_TEXTURE_2D, _tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    glGenBuffers(1, &_vbo);
    glEnableVertexAttribArray(ATTRIB_COORDS);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glVertexAttribPointer(ATTRIB_COORDS, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);
}

void FontRender::setFontSize(const uint32_t & size) {

    if (!this->isLoaded()) {
        return;
    }

    FT_Set_Pixel_Sizes(_face, 0, size);
}

void FontRender::setFontColor(cFGLC c) {

    GLfloat cpy[4] = {c.r, c.g, c.b, c.a};
    memcpy(_color, cpy, 4 * sizeof(GLfloat));
}

void FontRender::begin() {

    if (!this->isLoaded()) {
        return;
    }

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);

    _program->begin();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _tex);

    glUniform1i(_uniformTex, 0);
}

void FontRender::drawText(const char *text, float x, float y, float sx, float sy) {

    if (!this->isLoaded()) {
        return;
    }

    glUniform4fv(_uniformColor, 1, _color);

    const char *p;

    for (p = text; *p; p++) {

        if (FT_Load_Char(_face, *p, FT_LOAD_RENDER)) {
            std::cerr << "Could not load face: " << *p << std::endl;
            continue;
        }

        FT_GlyphSlot g = _face->glyph;

        glTexImage2D(
                     GL_TEXTURE_2D,
                     0,
                     GL_RED_EXT,
                     g->bitmap.width,
                     g->bitmap.rows,
                     0,
                     GL_RED_EXT,
                     GL_UNSIGNED_BYTE,
                     g->bitmap.buffer
                     );

        float x2 = x + g->bitmap_left * sx;
        float y2 = -y - g->bitmap_top * sy;
        float w = g->bitmap.width * sx;
        float h = g->bitmap.rows * sy;

        GLfloat box[4][4] = {
            {       x2 * 0.5f,       -y2 * 0.5f , 0, 0},
            { (x2 + w) * 0.5f,       -y2 * 0.5f , 1, 0},
            {       x2 * 0.5f, (-y2 - h) * 0.5f , 0, 1},
            { (x2 + w) * 0.5f, (-y2 - h) * 0.5f , 1, 1},
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        x += (g->advance.x/64) * sx;
        y += (g->advance.y/64) * sy;
    }
}

void FontRender::end() {

    if (!this->isLoaded()) {
        return;
    }

    _program->end();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

