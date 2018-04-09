#pragma once

#include "Core-pch.hpp"

enum NC_ARRAY_ATTRIBUTES_ENUM {
    ATTRIB_VERTEX,
    ATTRIB_COORDS,
    ATTRIB_COLOR,
    ATTRIB_NORMAL,
    ATTRIB_TANGENT,
    ATTRIB_BONE,
    ATTRIB_WEIGHT,
    NUM_ATTRIBUTES
};

enum NC_UNIFORM_ENUM {
    UNI_PROJECTION_MAT,         //projection
    UNI_MODELVIEW_WORLD_MAT,    //modelview + world
    UNI_NORMAL_MAT,             //normal matrix
    UNI_TEX0,                   //textures
    UNI_TEX1,
    UNI_TEX2,
    UNI_TEX3,
    NUM_UNIFORM
};

//-------------------------------------------------------------------------

class ShaderProgram {
  
public:
    
    ShaderProgram() : _shaderId(0) {} 
    ~ShaderProgram() {}
    
    void createShader(const std::string & filePath, GLenum type);
    const GLuint & getShaderId() const { return _shaderId; }
    
private:
    
    GLuint _shaderId;
};

class GLShader {
    
public:
    
    GLShader();
    ~GLShader();
    
    //path to files without extension both files should have same name and vsh / fsh extension
    bool loadShader(const std::string & path);
    
    // Remember that linking need to take after binding Attributes location (not uniforms!)
    void linkProgram();
    
    const GLuint & getProgramId() const;
    GLint uniformLocation(const GLchar * name) const;
    
    void begin() const;
    void end() const;
    
protected:
    
    void deinitShader();
    void printInfoLog() const;
    
private:
    
    GLuint _shaderProgram;
    GLuint _vertexProgram;
    GLuint _fragmentProgram;
};
