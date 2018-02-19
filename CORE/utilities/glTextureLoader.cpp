//
//  glTextureLoader.cpp
//  Untitled
//
//  Created by Marcin Małysz on 03/01/18.
//  Copyright 2018 NoClip. All rights reserved.
//

#include "glTextureLoader.hpp"
#include "BasePathExtraction.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

glTextureLoader textureLoader;

GLuint glTextureLoader::isUsed(const char * name) {

    if (_loadedTextures.size() == 0) {
        return 0;
    }
    
    for (auto tobj : _loadedTextures) {
        
        if ( strcmp(tobj.name, name) == 0 ) {
            
            return tobj.textId;
        }
    }
    
    return 0;
}

GLuint glTextureLoader::loadFile(const std::string & filepath,
                                 const GLint & filter,
                                 const GLint & anizothrophy,
                                 const GLint & param,
                                 bool generateMipmap) {
    
    assert(filepath.length() > 0);
    
    auto filename = removeExtension(basename(filepath));
    
    GLuint tex = this->isUsed(filename.c_str());

    if (tex != 0) {
        std::cout << "[INFO] Reusing texture: " << filename << std::endl;
        return tex;
    }
    
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    
    if ( filter != GL_NONE ) {
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter); // always use this for long range objects
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param);
    
    if (anizothrophy > 0) {
        
        int maxAnisotropy;
        int currentAnisotropy;
        
        glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
        
        currentAnisotropy = anizothrophy;
        if(maxAnisotropy < anizothrophy) currentAnisotropy = maxAnisotropy;
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, currentAnisotropy);
    }
    
    int width, height, comp;

    std::cout << "[INFO] Loading texture: " << filename << std::endl;

    unsigned char *imageData = stbi_load(filepath.c_str(), &width, &height, &comp, 4);

    if (imageData == nullptr) {

        std::cerr << "[ERROR] No image data provided: " << filepath << std::endl;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    
    if(generateMipmap == true) {
        
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    
    stbi_image_free(imageData);
    
    texInfo info;
    
    info.textId = tex;
    strcpy(info.name, filename.c_str());
    
    _loadedTextures.push_back(info);
    
    return tex;
}

void glTextureLoader::releaseTextures() {
    
    if (_loadedTextures.size() == 0) {
        return;
    }

    for (auto t : _loadedTextures) {
        
        glDeleteTextures(1, &t.textId);
    }
    
    _loadedTextures.clear();
    std::vector<texInfo>().swap(_loadedTextures);
}

const char* glTextureLoader::name(GLuint textId) {
    
    if (_loadedTextures.size() == 0) {
        return nullptr;
    }

    auto predicate = [textId] (const auto & o) {
        return o.textId == textId;
    };
    
    auto it = std::find_if(_loadedTextures.begin(), _loadedTextures.end(), predicate);
    
    if (it != _loadedTextures.end()) {
        
        return (*it).name;
    }
    
    return nullptr;
}
