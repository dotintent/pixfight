//
//  glMatrix4x4.cpp
//  Untitled
//
//  Created by Marcin Małysz on 03/01/18.
//  Copyright 2018 NoClip. All rights reserved.
//

#include "glMatrix4x4.hpp"

void glMatrix4x4::set(const xReal *m) {
    
    memcpy(matrix, m, 16 * sizeof(xReal));
}

void glMatrix4x4::translate(const xReal & x, const xReal & y, const xReal & z) {
    
    matrix[12]=matrix[0]*x+matrix[4]*y+matrix[8]*z+matrix[12];
    matrix[13]=matrix[1]*x+matrix[5]*y+matrix[9]*z+matrix[13];
    matrix[14]=matrix[2]*x+matrix[6]*y+matrix[10]*z+matrix[14];
    matrix[15]=matrix[3]*x+matrix[7]*y+matrix[11]*z+matrix[15];
}

void glMatrix4x4::scale(const xReal & x, const xReal & y, const xReal & z) {
    
    matrix[0]*=x;
    matrix[4]*=y;
    matrix[8]*=z;
    
    matrix[1]*=x;
    matrix[5]*=y;
    matrix[9]*=z;
    
    matrix[2]*=x;
    matrix[6]*=y;
    matrix[10]*=z;
    
    matrix[3]*=x;
    matrix[7]*=y;
    matrix[11]*=z;
}

void glMatrix4x4::loadIdentity() {
    
    matrix[0]=1.0;
    matrix[1]=0.0;
    matrix[2]=0.0;
    matrix[3]=0.0;
    matrix[4]=0.0;
    matrix[5]=1.0;
    matrix[6]=0.0;
    matrix[7]=0.0;
    matrix[8]=0.0;
    matrix[9]=0.0;
    matrix[10]=1.0;
    matrix[11]=0.0;
    matrix[12]=0.0;
    matrix[13]=0.0;
    matrix[14]=0.0;
    matrix[15]=1.0;
}

void glMatrix4x4::setPerspective(const xReal & fovyInDegrees,
                                 const xReal & aspect,
                                 const xReal & nearZ,
                                 const xReal & farZ) {
    
    xReal ymax, xmax;
    ymax = nearZ * tanf(fovyInDegrees*M_PI/360.0);
    xmax = ymax * aspect;
    
    this->setFrustum(-xmax, xmax, -ymax, ymax, nearZ, farZ);
}

void glMatrix4x4::setFrustum(const xReal & left,
                             const xReal & right,
                             const xReal & bottom,
                             const xReal & top,
                             const xReal & nearZ,
                             const xReal & farZ) {
    
    xReal matrix2[16], temp, temp2, temp3, temp4, resultMatrix[16];
    temp=2.0*nearZ;
    temp2=right-left;
    temp3=top-bottom;
    temp4=farZ-nearZ;
    matrix2[0]=temp/temp2;
    matrix2[1]=0.0;
    matrix2[2]=0.0;
    matrix2[3]=0.0;
    matrix2[4]=0.0;
    matrix2[5]=temp/temp3;
    matrix2[6]=0.0;
    matrix2[7]=0.0;
    matrix2[8]=(right+left)/temp2;
    matrix2[9]=(top+bottom)/temp3;
    matrix2[10]=(-farZ-nearZ)/temp4;
    matrix2[11]=-1.0;
    matrix2[12]=0.0;
    matrix2[13]=0.0;
    matrix2[14]=(-temp*farZ)/temp4;
    matrix2[15]=0.0;
    
    this->multiply(resultMatrix, matrix, matrix2);
    
    memcpy(matrix, resultMatrix, 16*sizeof(xReal));
}

void glMatrix4x4::setOrtho(const xReal & left,
                           const xReal & right,
                           const xReal & bottom,
                           const xReal & top,
                           const xReal & nearZ,
                           const xReal & farZ) {
    
    xReal matrix2[16], temp2, temp3, temp4, resultMatrix[16];
    temp2=right-left;
    temp3=top-bottom;
    temp4=farZ-nearZ;
    matrix2[0]=2.0/temp2;
    matrix2[1]=0.0;
    matrix2[2]=0.0;
    matrix2[3]=0.0;
    matrix2[4]=0.0;
    matrix2[5]=2.0/temp3;
    matrix2[6]=0.0;
    matrix2[7]=0.0;
    matrix2[8]=0.0;
    matrix2[9]=0.0;
    matrix2[10]=-2.0/temp4;
    matrix2[11]=0.0;
    matrix2[12]=(-right-left)/temp2;
    matrix2[13]=(-top-bottom)/temp3;
    matrix2[14]=(-farZ-nearZ)/temp4;
    matrix2[15]=1.0;
    
    this->multiply(resultMatrix, matrix, matrix2);
    
    memcpy(matrix, resultMatrix, 16*sizeof(xReal));
}

void glMatrix4x4::lookAt(const xVec3 & eyePos, const xVec3 & dir, const xVec3 & up) {
    
    xVec3 forward;
    xVec3 side;
    xVec3 upvec;
    
    xReal matrix2[16], resultMatrix[16];
    
    forward = dir - eyePos;
    side.cross(forward, upvec);
    side.normalize();
    
    upvec.cross(side, forward);
    
    matrix2[0]=side.x;
    matrix2[4]=side.y;
    matrix2[8]=side.z;
    matrix2[12]=0.0;
    
    matrix2[1]=upvec.x;
    matrix2[5]=upvec.y;
    matrix2[9]=upvec.z;
    matrix2[13]=0.0;
    
    matrix2[2]=-forward.x;
    matrix2[6]=-forward.y;
    matrix2[10]=-forward.z;
    matrix2[14]=0.0;
    
    matrix2[3]=matrix2[7]=matrix2[11]=0.0;
    matrix2[15]=1.0;
    
    this->multiply(resultMatrix, matrix, matrix2);
    
    memcpy(matrix, resultMatrix, 16*sizeof(xReal));

    this->translate(-eyePos.x, -eyePos.y, -eyePos.z);
}

void glMatrix4x4::setOrtho2D(const xReal & left, const xReal & right, const xReal & bottom, const xReal & top) {
 
    this->setOrtho(left, right, bottom, top, -1.0, 1.0);
}

void glMatrix4x4::rotatef(const xReal & x, const xReal & y, const xReal & z) {
    
    float mRotX[16] = { 1, 0,      0,       0,
                        0, cos(x), -sin(x), 0,
                        0, sin(x), cos(x),  0,
                        0, 0,      0,       1};
    
    float mRotY[16] = { cos(y),  0, sin(y), 0,
                        0,       1, 0,      0,
                        -sin(y), 0, cos(y), 0,
                        0,       0, 0,      1};
    
    float mRotZ[16] = { cos(z), -sin(z), 0, 0,
                        sin(z), cos(z),  0, 0,
                        0,      0,       1, 0,
                        0,      0,       0, 1};
    
    glMatrix4x4 RotX, RotY, RotZ;
 
    RotX.set(mRotX);
    RotY.set(mRotY);
    RotZ.set(mRotZ);
 
    RotY.multiply(RotY.getMatrix(), RotY.getMatrix(), RotZ.getMatrix());
    RotX.multiply(RotX.getMatrix(), RotX.getMatrix(), RotY.getMatrix());

    this->loadIdentity();
    this->multiply(matrix, matrix, RotX.getMatrix());
}

void glMatrix4x4::multiply(xReal *result, xReal *left, xReal *right) {
    
    xReal *matrix1 = left;
    xReal *matrix2 = right;
    
    result[0]=matrix1[0]*matrix2[0]+
    matrix1[4]*matrix2[1]+
    matrix1[8]*matrix2[2]+
    matrix1[12]*matrix2[3];
    result[4]=matrix1[0]*matrix2[4]+
    matrix1[4]*matrix2[5]+
    matrix1[8]*matrix2[6]+
    matrix1[12]*matrix2[7];
    result[8]=matrix1[0]*matrix2[8]+
    matrix1[4]*matrix2[9]+
    matrix1[8]*matrix2[10]+
    matrix1[12]*matrix2[11];
    result[12]=matrix1[0]*matrix2[12]+
    matrix1[4]*matrix2[13]+
    matrix1[8]*matrix2[14]+
    matrix1[12]*matrix2[15];
    
    result[1]=matrix1[1]*matrix2[0]+
    matrix1[5]*matrix2[1]+
    matrix1[9]*matrix2[2]+
    matrix1[13]*matrix2[3];
    result[5]=matrix1[1]*matrix2[4]+
    matrix1[5]*matrix2[5]+
    matrix1[9]*matrix2[6]+
    matrix1[13]*matrix2[7];
    result[9]=matrix1[1]*matrix2[8]+
    matrix1[5]*matrix2[9]+
    matrix1[9]*matrix2[10]+
    matrix1[13]*matrix2[11];
    result[13]=matrix1[1]*matrix2[12]+
    matrix1[5]*matrix2[13]+
    matrix1[9]*matrix2[14]+
    matrix1[13]*matrix2[15];
    
    result[2]=matrix1[2]*matrix2[0]+
    matrix1[6]*matrix2[1]+
    matrix1[10]*matrix2[2]+
    matrix1[14]*matrix2[3];
    result[6]=matrix1[2]*matrix2[4]+
    matrix1[6]*matrix2[5]+
    matrix1[10]*matrix2[6]+
    matrix1[14]*matrix2[7];
    result[10]=matrix1[2]*matrix2[8]+
    matrix1[6]*matrix2[9]+
    matrix1[10]*matrix2[10]+
    matrix1[14]*matrix2[11];
    result[14]=matrix1[2]*matrix2[12]+
    matrix1[6]*matrix2[13]+
    matrix1[10]*matrix2[14]+
    matrix1[14]*matrix2[15];
    
    result[3]=matrix1[3]*matrix2[0]+
    matrix1[7]*matrix2[1]+
    matrix1[11]*matrix2[2]+
    matrix1[15]*matrix2[3];
    result[7]=matrix1[3]*matrix2[4]+
    matrix1[7]*matrix2[5]+
    matrix1[11]*matrix2[6]+
    matrix1[15]*matrix2[7];
    result[11]=matrix1[3]*matrix2[8]+
    matrix1[7]*matrix2[9]+
    matrix1[11]*matrix2[10]+
    matrix1[15]*matrix2[11];
    result[15]=matrix1[3]*matrix2[12]+
    matrix1[7]*matrix2[13]+
    matrix1[11]*matrix2[14]+
    matrix1[15]*matrix2[15];
}
