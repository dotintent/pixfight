//
//  main.m
//  PixEditor
//
//  Created by Marcin Małysz on 23/04/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Core-pch.hpp"
#include "PFMapEditor.hpp"

#include <limits.h>
#include <unistd.h>
#include <chrono>
#include <thread>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 800

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING

#define NK_IMPLEMENTATION

#include <stdarg.h>
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

//UI
struct nk_context *ctx;
struct nk_vec2 scroll;
double last_button_click;
int is_double_click_down;
struct nk_vec2 double_click_pos;

#ifndef NK_GLFW_DOUBLE_CLICK_LO
#define NK_GLFW_DOUBLE_CLICK_LO 0.02
#endif
#ifndef NK_GLFW_DOUBLE_CLICK_HI
#define NK_GLFW_DOUBLE_CLICK_HI 0.2
#endif

struct nk_font *latosmall = nullptr;

PFMapEditor *mapeditor = nullptr;

std::string getexepath() {

#ifdef _WIN32

    return "data/";

#elif defined(__APPLE__)

    return "data/";

#else
    char result[ PATH_MAX ];

    ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
    std::string path = std::string( result, (count > 0) ? count : 0 );


    return path.substr(0, path.find_last_of("\\/")) + "/data/";
#endif

}

static void error_callback(int e, const char *d) {

    std::cout << "Error " << e <<": " << d << std::endl;
}

void scroll_callback(GLFWwindow *win, double xoff, double yoff) {

    scroll.x += (float)xoff;
    scroll.y += (float)yoff;

    mapeditor->handleScroll(xoff, yoff);
}

void mouse_callback(GLFWwindow *win, int button, int action, int mods) {

    double x, y;

    glfwGetCursorPos(win, &x, &y);

    mapeditor->handleMouse(x, y, button, action);

    if (button != GLFW_MOUSE_BUTTON_LEFT) {
        return;
    }

    if (action == GLFW_PRESS)  {

        double dt = glfwGetTime() - last_button_click;

        if (dt > NK_GLFW_DOUBLE_CLICK_LO && dt < NK_GLFW_DOUBLE_CLICK_HI) {

            is_double_click_down = nk_true;
            double_click_pos = nk_vec2((float)x, (float)y);
        }

        last_button_click = glfwGetTime();

    }
    else {

        is_double_click_down = nk_false;
    }
}

void text_callback(GLFWwindow *win, unsigned int codepoint) {

    nk_input_unicode(ctx, codepoint);
}

int main(int argc, const char * argv[]) {

    std::string rootPath = getexepath();
    
    last_button_click = 0;
    is_double_click_down = nk_false;
    double_click_pos = nk_vec2(0, 0);

    static GLFWwindow *win;
    int width = 0, height = 0;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {

        std::cout << "[GFLW] failed to init!" << std::endl;
        exit(1);
    }

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    win = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "PixEditor", NULL, NULL);

    glfwMakeContextCurrent(win);
    glfwGetWindowSize(win, &width, &height);

    glViewport(0, 0, width, height);

    glfwSetScrollCallback(win, scroll_callback);
    glfwSetMouseButtonCallback(win, mouse_callback);
    glfwSetCharCallback(win, text_callback);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {

        std::cout <<"Failed to setup GLEW" << std::endl;
        exit(1);
    }

    ctx = nk_glfw3_init(win);

    mapeditor = new PFMapEditor(win, rootPath, ctx);
    mapeditor->loadBlank();

    std::string latopath = rootPath + "Lato-Black.ttf";

    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&atlas);
    latosmall = nk_font_atlas_add_from_file(atlas, latopath.c_str(), 16, 0);
    nk_glfw3_font_stash_end();
    nk_style_set_font(ctx, &latosmall->handle);

    while (!glfwWindowShouldClose(win)) {

        double x, y;
        nk_input_begin(ctx);

        glfwPollEvents();
        
        nk_input_key(ctx, NK_KEY_DEL, glfwGetKey(win, GLFW_KEY_DELETE) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_ENTER, glfwGetKey(win, GLFW_KEY_ENTER) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_TAB, glfwGetKey(win, GLFW_KEY_TAB) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_BACKSPACE, glfwGetKey(win, GLFW_KEY_BACKSPACE) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_LEFT, glfwGetKey(win, GLFW_KEY_LEFT) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_RIGHT, glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_UP, glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_DOWN, glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS);

#ifdef __APPLE__
        if(glfwGetKey(win, GLFW_KEY_LEFT_SUPER) == GLFW_PRESS ||
           glfwGetKey(win, GLFW_KEY_RIGHT_SUPER) == GLFW_PRESS) {
#else
        if (glfwGetKey(win, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
            glfwGetKey(win, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
#endif
            nk_input_key(ctx, NK_KEY_COPY, glfwGetKey(win, GLFW_KEY_C) == GLFW_PRESS);
            nk_input_key(ctx, NK_KEY_PASTE, glfwGetKey(win, GLFW_KEY_V) == GLFW_PRESS);
            nk_input_key(ctx, NK_KEY_CUT, glfwGetKey(win, GLFW_KEY_X) == GLFW_PRESS);
            nk_input_key(ctx, NK_KEY_SHIFT, 1);
        } else {
            nk_input_key(ctx, NK_KEY_COPY, 0);
            nk_input_key(ctx, NK_KEY_PASTE, 0);
            nk_input_key(ctx, NK_KEY_CUT, 0);
            nk_input_key(ctx, NK_KEY_SHIFT, 0);
        }

        glfwGetCursorPos(win, &x, &y);
        nk_input_motion(ctx, (int)x, (int)y);
        nk_input_button(ctx, NK_BUTTON_LEFT, (int)x, (int)y, glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
        nk_input_button(ctx, NK_BUTTON_MIDDLE, (int)x, (int)y, glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
        nk_input_button(ctx, NK_BUTTON_RIGHT, (int)x, (int)y, glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
        nk_input_button(ctx, NK_BUTTON_DOUBLE, (int)double_click_pos.x, (int)double_click_pos.y, is_double_click_down);
        nk_input_scroll(ctx, scroll);
        nk_input_end(ctx);
        scroll = nk_vec2(0,0);

        mapeditor->handleMouse(x, y, glfwGetMouseButton(win, 1), -1);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.4, 0.4, 0.4, 1);

        mapeditor->Render();

        nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
        glfwSwapBuffers(win);

#ifndef _WIN32
        //slow down
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
#endif
    }

    delete mapeditor;

    nk_glfw3_shutdown();
    glfwDestroyWindow(win);
    glfwTerminate();

    return 0;
}
