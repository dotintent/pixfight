//
//  glTextureLoader.hpp
//  Untitled
//
//  Created by Marcin Małysz on 03/01/18.
//  Copyright 2018 NoClip. All rights reserved.
//

#pragma once

#include "Core-pch.hpp"

class glTextureLoader final {
    
public:
    
    glTextureLoader() {}
    ~glTextureLoader() noexcept { releaseTextures(); }
    
    GLuint isUsed(const char * name);
    GLuint loadFile(const std::string & filepath,
                    const GLint & filter = GL_LINEAR,
                    const GLint & anizothrophy = 0,
                    const GLint & param = GL_REPEAT,
                    bool generateMipmap = false);

    const char* name(GLuint textId);

    void releaseTextures();
    
private:
    
    typedef struct {
        
        char name[256];
        GLuint textId;
        
    } texInfo;
    
    std::vector<texInfo> _loadedTextures;
};

extern glTextureLoader textureLoader;
