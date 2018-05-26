#include "Core-pch.hpp"

#ifdef __ANDROID__

PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES = nullptr;
PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOES = nullptr;
PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES = nullptr;

void eglBuildVertexArray() {

    glBindVertexArrayOES = (PFNGLBINDVERTEXARRAYOESPROC)eglGetProcAddress("glBindVertexArrayOES");
    glGenVertexArraysOES = (PFNGLGENVERTEXARRAYSOESPROC)eglGetProcAddress("glGenVertexArraysOES");
    glDeleteVertexArraysOES = (PFNGLDELETEVERTEXARRAYSOESPROC)eglGetProcAddress("glDeleteVertexArraysOES");
}

#endif

int PLAYERTEAMSELECTED = 1;
int GAME_IDCOUNTER = 0;

int UNITS_RATINGS[5][3] = {
    { 4, 6, 4}, //M_INFANTRY
    { 6, 6, 4}, //M_BAZOOKA
    { 8, 8, 5}, //M_JEEP
    {10,10, 4}, //M_LTANK
    {10, 3, 3}  //M_ARTILLERY
};

bool AlmostEqual(const xVec2 & a, const xVec2 & b) {

    if( (fabs(a.x-b.x) <= 2) && (fabs(a.y-b.y) <= 2) ){

        return true;
    }

    return false;
}

bool FLT_EQUAL(float a, float b) {

    return fabs(a - b) < FLT_EPSILON;
}

