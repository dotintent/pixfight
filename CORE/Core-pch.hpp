#pragma once

#define NUMBER_OF_COMPONENTS_PER_VERTEX 2
#define NUMBER_OF_INDICES 6

#ifdef _WIN32
	
	#define NOMINMAX
	#include <windows.h>
	#include <GL/glew.h>
	#include <GL/gl.h>
	#include <GLFW/glfw3.h>
	#define GL_RED_EXT GL_RED
	
#endif

#ifdef __APPLE__

    #if TARGET_OS_IPHONE

        #ifdef __OBJC__
            #import <Foundation/Foundation.h>
            #import <QuartzCore/QuartzCore.h>
            #import <OpenGLES/EAGL.h>
        #endif

        #import <OpenGLES/ES1/gl.h>
        #import <OpenGLES/ES1/glext.h>

        #import <OpenGLES/ES2/gl.h>
        #import <OpenGLES/ES2/glext.h>

        #define glBindVertexArray glBindVertexArrayOES
        #define glGenVertexArrays glGenVertexArraysOES
        #define glDeleteVertexArrays glDeleteVertexArraysOES

    #else

        #ifdef __OBJC__
            #import <AppKit/AppKit.h>
            #import <Cocoa/Cocoa.h>
            #import <QuartzCore/CVDisplayLink.h>
        #endif

        #import <OpenGL/gl3.h>
        #import <OpenGL/gl3ext.h>

        #define glBindVertexArray glBindVertexArray
        #define glGenVertexArrays glGenVertexArrays
        #define glDeleteVertexArrays glDeleteVertexArrays
        #define GL_RED_EXT GL_RED

    #endif

#endif

#ifdef __ANDROID__

    #include <GLES/gl.h>
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
    #include <string.h>

    extern PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES;
    extern PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOES;
    extern PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES;

    extern void eglBuildVertexArray();

    #define glBindVertexArray glBindVertexArrayOES
    #define glGenVertexArrays glGenVertexArraysOES
    #define glDeleteVertexArrays glDeleteVertexArraysOES

#elif defined(__linux__)

    #include <GL/glew.h>
    #include <GL/gl.h>
    #include <GLFW/glfw3.h>
    #define GL_RED_EXT GL_RED

#endif

#define OFFSET(st, m) \
((size_t) ( (char *)&((st *)(0))->m - (char *)0 ))

#include <cassert>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "EnumOptionMacro.hpp"
#include "CFrustum.hpp"

#include "xVec2.h"

extern int PLAYERTEAMSELECTED;
extern int UNITS_RATINGS[5][3];
extern int GAME_IDCOUNTER;
extern bool AlmostEqual(const xVec2 &a, const xVec2 &b);
extern bool FLT_EQUAL(float a, float b);

