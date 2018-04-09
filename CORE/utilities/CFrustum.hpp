//
//  CFrustum.hpp
//  Untitled
//
//  Created by Marcin Małysz on 03/01/18.
//  Copyright 2018 NoClip. All rights reserved.
//

#pragma once

#include "Core-pch.hpp"
#include "xMath.h"

class CFrustum final {
    
private:

    float frustum[6][4];

    void multMatrix(float a[16], float b[16], float r[16])
    {
        r[0] = (a[0] *b[0]) + (a[1] *b[4]) + (a[2] *b[8] ) + (a[3] *b[12]);
        r[1] = (a[0] *b[1]) + (a[1] *b[5]) + (a[2] *b[9] ) + (a[3] *b[13]);
        r[2] = (a[0] *b[2]) + (a[1] *b[6]) + (a[2] *b[10]) + (a[3] *b[14]);
        r[3] = (a[0] *b[3]) + (a[1] *b[7]) + (a[2] *b[11]) + (a[3] *b[15]);
        r[4] = (a[4] *b[0]) + (a[5] *b[4]) + (a[6] *b[8] ) + (a[7] *b[12]);
        r[5] = (a[4] *b[1]) + (a[5] *b[5]) + (a[6] *b[9] ) + (a[7] *b[13]);
        r[6] = (a[4] *b[2]) + (a[5] *b[6]) + (a[6] *b[10]) + (a[7] *b[14]);
        r[7] = (a[4] *b[3]) + (a[5] *b[7]) + (a[6] *b[11]) + (a[7] *b[15]);
        r[8] = (a[8] *b[0]) + (a[9] *b[4]) + (a[10]*b[8] ) + (a[11]*b[12]);
        r[9] = (a[8] *b[1]) + (a[9] *b[5]) + (a[10]*b[9] ) + (a[11]*b[13]);
        r[10]= (a[8] *b[2]) + (a[9] *b[6]) + (a[10]*b[10]) + (a[11]*b[14]);
        r[11]= (a[8] *b[3]) + (a[9] *b[7]) + (a[10]*b[11]) + (a[11]*b[15]);
        r[12]= (a[12]*b[0]) + (a[13]*b[4]) + (a[14]*b[8] ) + (a[15]*b[12]);
        r[13]= (a[12]*b[1]) + (a[13]*b[5]) + (a[14]*b[9] ) + (a[15]*b[13]);
        r[14]= (a[12]*b[2]) + (a[13]*b[6]) + (a[14]*b[10]) + (a[15]*b[14]);
        r[15]= (a[12]*b[3]) + (a[13]*b[7]) + (a[14]*b[11]) + (a[15]*b[15]);
    }

    void normPlane(float plane[4], float result[4])
    {
        
        double mag = sqrt((plane[0] * plane[0]) +
                          (plane[1] * plane[1]) +
                          (plane[2] * plane[2]));

        
        result[0] = float(plane[0] / mag);
        result[1] = float(plane[1] / mag);
        result[2] = float(plane[2] / mag);
        result[3] = float(plane[3] / mag);
    }

public:
    
    float *modelview;
    float *projection;
    
    float result[16];
 
    CFrustum(void) { modelview = NULL; projection = NULL; }

    virtual ~CFrustum(void) { }

    void calculateFrustum(void) {

        multMatrix(modelview, projection, result);

        // Right side:
        frustum[0][0] = result[3]  - result[0];
        frustum[0][1] = result[7]  - result[4];
        frustum[0][2] =	result[11] - result[8];
        frustum[0][3] =	result[15] - result[12];

        normPlane(frustum[0], frustum[0]);

        // Left side:
        frustum[1][0] = result[3]  + result[0];
        frustum[1][1] = result[7]  + result[4];
        frustum[1][2] =	result[11] + result[8];
        frustum[1][3] =	result[15] + result[12];

        normPlane(frustum[1], frustum[1]);

        // Bottom:
        frustum[2][0] = result[3]  + result[1];
        frustum[2][1] = result[7]  + result[5];
        frustum[2][2] =	result[11] + result[9];
        frustum[2][3] =	result[15] + result[13];

        normPlane(frustum[2], frustum[2]);

        // Top:
        frustum[3][0] = result[3]  - result[1];
        frustum[3][1] = result[7]  - result[5];
        frustum[3][2] =	result[11] - result[9];
        frustum[3][3] =	result[15] - result[13];

        normPlane(frustum[3], frustum[3]);

        // Back:
        frustum[4][0] = result[3]  - result[2];
        frustum[4][1] = result[7]  - result[6];
        frustum[4][2] =	result[11] - result[10];
        frustum[4][3] =	result[15] - result[14];

        normPlane(frustum[4], frustum[4]);

        // Front:
        frustum[5][0] = result[3]  + result[2];
        frustum[5][1] = result[7]  + result[6];
        frustum[5][2] =	result[11] + result[10];
        frustum[5][3] =	result[15] + result[14];

        normPlane(frustum[5], frustum[5]);
    }

    bool pointInFrustum(float x, float y, float z) {
        
        float dist = float(0.0f);
        
        for (int i = 0; i < 6; i++)
        {
            
            dist =  frustum[i][0] * x +
                    frustum[i][1] * y +
                    frustum[i][2] * z +
                    frustum[i][3];

            if (dist <= 0) {
             
                return false;
            }
        }

        return true;
    }

    bool SphereInFrustum(float x, float y, float z, float Radius) {
        
        int i;

        for(i = 0; i < 6; i++) {
            if (frustum[i][0] * x + frustum[i][1] * y + frustum[i][2] * z + frustum[i][3] <= -Radius)
            {
                return false;
            }
        }

        return true;
    }

};

