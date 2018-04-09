//
//  glMatrix4x4.hpp
//  Untitled
//
//  Created by Marcin Małysz on 03/01/18.
//  Copyright 2018 NoClip. All rights reserved.
//

#pragma once

#include "Core-pch.hpp"
#include "xf.h"
#include "xVec3.h"

class glMatrix4x4 final {
  
public:
    
    glMatrix4x4() { loadIdentity(); }
    
    xReal* getMatrix() { return &matrix[0]; }
    void set(const xReal *m); //m[16]
    void translate(const xReal & x, const xReal & y, const xReal & z);
    void scale(const xReal & x, const xReal & y, const xReal & z);
    void loadIdentity();

    void setPerspective(const xReal & fovyInDegrees,
                        const xReal & aspect,
                        const xReal & nearZ,
                        const xReal & farZ);
    
    void setFrustum(const xReal & left,
                    const xReal & right,
                    const xReal & bottom,
                    const xReal & top,
                    const xReal & nearZ,
                    const xReal & farZ);
    
    void setOrtho(const xReal & left,
                  const xReal & right,
                  const xReal & bottom,
                  const xReal & top,
                  const xReal & nearZ,
                  const xReal & farZ);
    
    void lookAt(const xVec3 & eyePos, const xVec3 & dir, const xVec3 & up);
    void setOrtho2D(const xReal & left, const xReal & right, const xReal & bottom, const xReal & top);
    void rotatef(const xReal & x, const xReal & y, const xReal & z);
    
protected:
    
    void multiply(xReal *result, xReal *left, xReal *right);
    
private:
    
    xReal matrix[16];
};


