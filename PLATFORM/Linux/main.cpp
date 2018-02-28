#include <iostream>
#include <fstream>
#include <limits.h>
#include <unistd.h>

#include "Core-pch.hpp"
#include "Audio.hpp"
#include "SceneManager.hpp"

#include "NewGameScene.hpp"
#include "LoadGameScene.hpp"
#include "RenderScene.hpp"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING

#define NK_IMPLEMENTATION

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

static SceneManager *sceneManager;
static Audio *audioUnit;
static Audio::SoundID selectSound;

std::string getexepath() {
  char result[ PATH_MAX ];

  ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
  std::string path = std::string( result, (count > 0) ? count : 0 );

  return path.substr(0, path.find_last_of("\\/")) + "/data/";
}

static void error_callback(int e, const char *d) {

    std::cout << "Error " << e <<": " << d << std::endl;
}

void scroll_callback(GLFWwindow *win, double xoff, double yoff) {

    scroll.x += (float)xoff;
    scroll.y += (float)yoff;

    sceneManager->getCurrentScenePointer()->handleScroll(yoff);
}

void mouse_callback(GLFWwindow *win, int button, int action, int mods) {

    double x, y;

    glfwGetCursorPos(win, &x, &y);

    sceneManager->getCurrentScenePointer()->handleMouse(button, action, x, y);

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

void playMusic(std::string path) {

    audioUnit->pauseMusic();
    audioUnit->loadMusic(path);
    audioUnit->playMusic();
}

void initializeAudio(std::string rootPath) {

    audioUnit = new Audio();

    std::string soundPath = rootPath + "select.wav";
    std::string menuPath = rootPath + "menu.mp3";

    selectSound = audioUnit->loadSound(soundPath);

    audioUnit->setVolume(1.0);

    playMusic(menuPath);

    //TODO: add settings
    bool muted = false;

    if (muted) {

        audioUnit->mute();
    }
}

void tearDownAudio() {

    audioUnit->pauseMusic();

    delete audioUnit;
}

int main() {

    last_button_click = 0;
    is_double_click_down = nk_false;
    double_click_pos = nk_vec2(0, 0);

    std::string rootPath = getexepath();

    static GLFWwindow *win;
    int width = 0, height = 0;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {

        std::cout << "[GFLW] failed to init!" << std::endl;
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    win = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "PixFight", NULL, NULL);

    glfwMakeContextCurrent(win);
    glfwGetWindowSize(win, &width, &height);

    glViewport(0, 0, width, height);

    glfwSetScrollCallback(win, scroll_callback);
    glfwSetMouseButtonCallback(win, mouse_callback);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {

        std::cout <<"Failed to setup GLEW" << std::endl;
        exit(1);
    }

    initializeAudio(rootPath);

    ctx = nk_glfw3_init(win);

    std::string atlanpath = rootPath + "FFFATLAN.TTF";
    std::string latopath = rootPath + "Lato-Black.ttf";

    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&atlas);
    struct nk_font *atlan = nk_font_atlas_add_from_file(atlas, atlanpath.c_str(), 36, 0);
    struct nk_font *lato = nk_font_atlas_add_from_file(atlas, latopath.c_str(), 36, 0);
    nk_glfw3_font_stash_end();
    nk_style_set_font(ctx, &atlan->handle);

    sceneManager = new SceneManager(ctx, rootPath);

    auto startScene = sceneManager->getCurrentScenePointer();

    startScene->Init();

    while (!glfwWindowShouldClose(win)) {

        glfwPollEvents();

        double x, y;
        nk_input_begin(ctx);
        glfwGetCursorPos(win, &x, &y);
        nk_input_motion(ctx, (int)x, (int)y);
        nk_input_button(ctx, NK_BUTTON_LEFT, (int)x, (int)y, glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
        nk_input_button(ctx, NK_BUTTON_MIDDLE, (int)x, (int)y, glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
        nk_input_button(ctx, NK_BUTTON_RIGHT, (int)x, (int)y, glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
        nk_input_button(ctx, NK_BUTTON_DOUBLE, (int)double_click_pos.x, (int)double_click_pos.y, is_double_click_down);
        nk_input_scroll(ctx, scroll);
        nk_input_end(ctx);
        scroll = nk_vec2(0,0);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.4, 0.4, 0.4, 1);

        switch (sceneManager->Render()) {

        case SceneTypeMenu: {

            audioUnit->playSound(selectSound);

            auto ptr = sceneManager->getCurrentScenePointer();
            ptr->Destroy();

            if (ptr->isRender()) {
                playMusic(rootPath + "menu.mp3");
            }

            sceneManager->setCurrent("menu");

            auto nptr = sceneManager->getCurrentScenePointer();
            nptr->Init();

        }
            break;

        case SceneTypeNewGame: {

            audioUnit->playSound(selectSound);

            auto ptr = sceneManager->getCurrentScenePointer();
            ptr->Destroy();

            sceneManager->setCurrent("newgame");

            auto nptr = sceneManager->getCurrentScenePointer();
            nptr->Init();
        }
            break;

        case SceneTypeLoadGame: {

            audioUnit->playSound(selectSound);

            auto ptr = sceneManager->getCurrentScenePointer();
            ptr->Destroy();

            sceneManager->setCurrent("loadgame");

            auto nptr = sceneManager->getCurrentScenePointer();
            nptr->Init();
        }
            break;

        case SceneTypeSettings: {

            audioUnit->playSound(selectSound);

            auto ptr = sceneManager->getCurrentScenePointer();
            ptr->Destroy();

            sceneManager->setCurrent("settings");

            auto nptr = sceneManager->getCurrentScenePointer();
            nptr->Init();
        }
            break;

        case SceneTypeRender: {

            audioUnit->playSound(selectSound);

            auto ptr = sceneManager->getCurrentScenePointer();
            ptr->Destroy();

            playMusic(rootPath + "map.mp3");

            auto newGame = dynamic_cast<NewGameScene*>(ptr);
            auto loadGame = dynamic_cast<LoadGameScene*>(ptr);

            sceneManager->setCurrent("render");

            auto nptr = sceneManager->getCurrentScenePointer();
            nptr->Init();

            auto renderGame = dynamic_cast<RenderScene*>(nptr);

            if (newGame) {


            }
            else if(loadGame) {


            }

        }
            break;

        case SceneTypeNone: break;
        default: break;

        }

        nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
        glfwSwapBuffers(win);
    }

    delete sceneManager;

    tearDownAudio();
    nk_glfw3_shutdown();
    glfwDestroyWindow(win);
    glfwTerminate();

    return 0;
}
