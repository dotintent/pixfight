#include <iostream>

#include "Core-pch.hpp"
#include "Audio.hpp"

using namespace std;

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

static void error_callback(int e, const char *d) {

    std::cout << "Error " << e <<": " << d << std::endl;
}

int main() {

    static GLFWwindow *win;
    int width = 0, height = 0;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {

        std::cout << "[GFLW] failed to init!" << std::endl;
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    win = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "PixFight", NULL, NULL);

    glfwMakeContextCurrent(win);
    glfwGetWindowSize(win, &width, &height);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    if (glewInit() != GLEW_OK) {

        std::cout <<"Failed to setup GLEW" << std::endl;
        exit(1);
    }

    while (!glfwWindowShouldClose(win)) {

        /* Input */
        //glfwPollEvents();

        /* Draw */
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0, 1, 0, 1);



        glfwSwapBuffers(win);
    }

    glfwTerminate();

    return 0;
}
