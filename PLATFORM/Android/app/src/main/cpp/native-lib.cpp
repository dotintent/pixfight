#include "native-lib.h"
#include "GameLogic.hpp"
#include "Audio.hpp"

#include <android/log.h>
#include <cstdio>
#include <unistd.h>
#include <pthread.h>

#include <android/native_activity.h>
#include <EGL/egl.h>

std::string rootDataDirectory = "";

Audio *audioUnit = nullptr;
Audio::SoundID selectSound = 0;

std::string jstringTostring(JNIEnv *env, jstring jStr){
    const char *cstr = env->GetStringUTFChars(jStr, NULL);
    std::string str = std::string(cstr);
    env->ReleaseStringUTFChars(jStr, cstr);
    return str;
}

static int pfd[2];
static pthread_t thr;
static const char *tag = "myapp";

static void *thread_func(void*)
{
    ssize_t rdsz;
    char buf[128];
    while((rdsz = read(pfd[0], buf, sizeof buf - 1)) > 0) {
        if(buf[rdsz - 1] == '\n') --rdsz;
        buf[rdsz] = 0;  /* add null-terminator */
        __android_log_write(ANDROID_LOG_DEBUG, tag, buf);
    }
    return 0;
}

int start_logger(const char *app_name)
{
    tag = app_name;

    /* make stdout line-buffered and stderr unbuffered */
    setvbuf(stdout, 0, _IOLBF, 0);
    setvbuf(stderr, 0, _IONBF, 0);

    /* create the pipe and redirect stdout and stderr */
    pipe(pfd);
    dup2(pfd[1], 1);
    dup2(pfd[1], 2);

    /* spawn the logging thread */
    if(pthread_create(&thr, 0, thread_func, 0) == -1)
        return -1;
    pthread_detach(thr);
    return 0;
}

JavaVM* javaVM = nullptr;
JNIEnv* env = nullptr;

jint JNI_OnLoad(JavaVM* aVm, void* aReserved) {

    javaVM = aVm;
    javaVM->GetEnv((void**)&env, JNI_VERSION_1_6);

    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM* vm, void* reserved) {

    javaVM->DetachCurrentThread();
}

void attachThread() {

    javaVM->AttachCurrentThread(&env, NULL);
}

void detachThread() {

    env = nullptr;
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFAudioWrapper_initializeAudio(JNIEnv* jenv, jobject instance, jobject activity, jstring rootpath) {

    start_logger("[PIXFIGHT]");

    audioUnit = new Audio();

    rootDataDirectory = jstringTostring(jenv, rootpath);

    std::string selectPath = rootDataDirectory + "/select.wav";
    selectSound = audioUnit->loadSound(selectPath);

    audioUnit->pauseMusic();
    std::string musicPath = rootDataDirectory + "/menu.mp3";

    audioUnit->loadMusic(musicPath);
    audioUnit->playMusic();

    audioUnit->setVolume(1.0);

    //JAVA REFLECT
    jclass spcls = jenv->FindClass("android/content/SharedPreferences");
    jclass contextcls = jenv->FindClass("android/content/Context");
    jobject mainClass = jenv->NewGlobalRef(activity);
    jmethodID mid = jenv->GetMethodID(contextcls, "getSharedPreferences", "(Ljava/lang/String;I)Landroid/content/SharedPreferences;");
    jmethodID midbool = jenv->GetMethodID(spcls, "getBoolean", "(Ljava/lang/String;Z)Z");
    jobject jobjectshared = jenv->CallObjectMethod(mainClass, mid, jenv->NewStringUTF("PixFightPreferences") , 0);
    jboolean jboolean1 = jenv->CallBooleanMethod(jobjectshared, midbool, jenv->NewStringUTF("mute"), JNI_FALSE);
    jenv->DeleteLocalRef(spcls);
    jenv->DeleteLocalRef(contextcls);

    if (jboolean1 != 0) {

        audioUnit->mute();
    }
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFAudioWrapper_playSelectSound(JNIEnv* jenv, jobject instance) {

    if (audioUnit == nullptr) {
        return;
    }

    audioUnit->playSound(selectSound);
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFAudioWrapper_playMenuMusic(JNIEnv* jenv, jobject instance) {

    if (audioUnit == nullptr) {
        return;
    }

    audioUnit->pauseMusic();
    std::string musicPath = rootDataDirectory + "/menu.mp3";
    audioUnit->loadMusic(musicPath);
    audioUnit->playMusic();
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFAudioWrapper_muteMusic(JNIEnv* jenv, jobject instance) {

    if (audioUnit == nullptr) {
        return;
    }

    audioUnit->mute();
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFAudioWrapper_unmuteMusic(JNIEnv* jenv, jobject instance) {

    if (audioUnit == nullptr) {
        return;
    }

    audioUnit->unmute();
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFAudioWrapper_playGameMusic(JNIEnv* jenv, jobject instance) {

    if (audioUnit == nullptr) {
        return;
    }

    audioUnit->pauseMusic();
    std::string musicPath = rootDataDirectory + "/map.mp3";
    audioUnit->loadMusic(musicPath);
    audioUnit->playMusic();
}

//gamelogic
static GameLogic *gameLogic = nullptr;
static GameBase *selectedBase = nullptr;
static bool useHardAI = false;

void callNativeVoidMethod(std::string method) {

    attachThread();

    jclass cls = env->FindClass("com/noclip/marcinmalysz/pixfight/PFRenderActivity");

    if (!cls) {
        return;
    }

    jclass globalClass = reinterpret_cast<jclass>(env->NewGlobalRef(cls));

    if (!globalClass) {
        return;
    }

    env->DeleteLocalRef(cls);

    jmethodID mid = env->GetStaticMethodID(globalClass, method.c_str(), "()V");

    if (!mid) {
        return;
    }

    env->CallStaticVoidMethod(globalClass, mid);
    env->DeleteGlobalRef(globalClass);
}

void callNativeVoidMethodParams(std::string method, int param1, int param2) {

    attachThread();

    jclass cls = env->FindClass("com/noclip/marcinmalysz/pixfight/PFRenderActivity");

    if (!cls) {
        return;
    }

    jclass globalClass = reinterpret_cast<jclass>(env->NewGlobalRef(cls));

    if (!globalClass) {
        return;
    }

    env->DeleteLocalRef(cls);

    jmethodID mid = env->GetStaticMethodID(globalClass, method.c_str(), "(II)V");

    if (!mid) {
        return;
    }

    env->CallStaticVoidMethod(globalClass, mid, param1, param2);
    env->DeleteGlobalRef(globalClass);
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderActivity_initializeOpenGL(JNIEnv* jenv, jobject obj, int width, int height) {

    __android_log_print(ANDROID_LOG_DEBUG, "[PIXFIGHT]", "initializeOpenGL %d %d", width, height);

    eglBuildVertexArray();

    std::string rootPath = rootDataDirectory + "/";

    gameLogic = new GameLogic(width,
                              height,
                              rootPath,
                              audioUnit);

    //no need to use context, we are calling static methods from Java
    gameLogic->context = nullptr;

    gameLogic->winGameCallback = [](void* context) {

        callNativeVoidMethod("onWinEventBridge");
    };

    gameLogic->loseGameCallback = [](void* context) {

        callNativeVoidMethod("onLoseEventBridge");
    };

    gameLogic->botsStartThinkCallback = [](void* context) {

        callNativeVoidMethod("botsStartThinkEventBridge");
    };

    gameLogic->botsEndThinkCallback = [](void* context) {

        callNativeVoidMethod("botsEndThinkEventBridge");
    };

    gameLogic->baseSelectedCallback = [](void* context, GameBase *base) {

        selectedBase = base;

        int teamID = selectedBase->getTeamID();
        int cash = gameLogic->getPlayerCash();

        callNativeVoidMethodParams("onBaseSelectedBridge", teamID, cash);
    };

    gameLogic->setHardAI(useHardAI);

}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFGameLib_renderFrame(JNIEnv* jenv, jobject obj) {

    if (gameLogic == nullptr) {
        return;
    }

    gameLogic->Render();
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFGameLib_nativeStartGame(JNIEnv* jenv, jobject obj, jstring map, int playerSelected, int playersPlaying) {

    __android_log_print(ANDROID_LOG_DEBUG, "[PIXFIGHT]", "nativeStartGame");

    if (gameLogic == nullptr) {
        return;
    }

    std::string mapName = jstringTostring(jenv, map);
    gameLogic->createNewGame(mapName, playerSelected, playersPlaying);
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFGameLib_nativeLoadGame(JNIEnv* jenv, jobject obj, jstring path) {

    __android_log_print(ANDROID_LOG_DEBUG, "[PIXFIGHT]", "nativeLoadGame");

    if (gameLogic == nullptr) {
        return;
    }

    std::string savePath = jstringTostring(jenv, path);
    gameLogic->loadGame(savePath);
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFGameLib_nativeOnStart(JNIEnv* jenv, jobject obj, jobject activity) {

    __android_log_print(ANDROID_LOG_DEBUG, "[PIXFIGHT]", "nativeOnStart");

    if (gameLogic != nullptr) {
        return;
    }

    //JAVA REFLECT
    jclass spcls = jenv->FindClass("android/content/SharedPreferences");
    jclass contextcls = jenv->FindClass("android/content/Context");
    jobject mainClass = jenv->NewGlobalRef(activity);
    jmethodID mid = jenv->GetMethodID(contextcls, "getSharedPreferences", "(Ljava/lang/String;I)Landroid/content/SharedPreferences;");
    jmethodID midbool = jenv->GetMethodID(spcls, "getBoolean", "(Ljava/lang/String;Z)Z");
    jobject jobjectshared = jenv->CallObjectMethod(mainClass, mid, jenv->NewStringUTF("PixFightPreferences") , 0);
    jboolean jboolean1 = jenv->CallBooleanMethod(jobjectshared, midbool, jenv->NewStringUTF("hardai"), JNI_FALSE);
    jenv->DeleteLocalRef(spcls);
    jenv->DeleteLocalRef(contextcls);

    useHardAI = jboolean1 != 0;
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFGameLib_nativeOnStop(JNIEnv* jenv, jobject obj) {

    __android_log_print(ANDROID_LOG_DEBUG, "[PIXFIGHT]", "nativeOnStop");

    detachThread();

    delete gameLogic;
    gameLogic = nullptr;
}

JNIEXPORT bool JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderActivity_canEndTurn(JNIEnv* jenv, jobject obj) {

    if (gameLogic == nullptr) {
        return false;
    }

    return gameLogic->canEndTurn();
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderActivity_endTurn(JNIEnv* jenv, jobject obj) {

    if (gameLogic == nullptr) {
        return;
    }

    gameLogic->endTurn();
}

JNIEXPORT int JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderActivity_multiplyTime(JNIEnv* jenv, jobject obj) {

    if (gameLogic == nullptr) {
        return 1;
    }

    return (int)gameLogic->multiplyTime();
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderActivity_finishBuilding(JNIEnv* jenv, jobject obj, int unitid, int cash) {

    //we are coming from java UI thread!
    syncToMainLoop([unitid, cash](void *context, GameLogic *logic){

        selectedBase->setUnitToBuild(unitid);
        gameLogic->setPlayerCash(cash);
        gameLogic->buildUnit(selectedBase);
        selectedBase = nullptr;
    });
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderActivity_tapAction(JNIEnv* jenv, jobject obj, float x, float y) {

    if (gameLogic == nullptr) {
        return;
    }

    xVec2 touchPoint = xVec2(x, y);

    syncToMainLoop([touchPoint](void *context, GameLogic *logic) {

        gameLogic->touchDownAtPoint(touchPoint);
    });
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderActivity_scaleAction(JNIEnv* jenv, jobject obj, float scale) {

    if (gameLogic == nullptr) {
        return;
    }

    syncToMainLoop([scale](void *context, GameLogic *logic) {

        gameLogic->setCurrentScale(scale);
    });
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderActivity_panAction(JNIEnv* jenv, jobject obj, float x, float y) {

    if (gameLogic == nullptr) {
        return;
    }

    xVec2 dir = xVec2(x, y);

    syncToMainLoop([dir](void *context, GameLogic *logic) {

        gameLogic->setDirectionVec(dir);
    });
}

JNIEXPORT bool JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderActivity_saveGame(JNIEnv* jenv, jobject obj, jstring savepath) {

    if (gameLogic == nullptr) {
        return false;
    }

    std::string path = jstringTostring(jenv, savepath);

    return gameLogic->saveGame(path);
}

JNIEXPORT jstring JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderActivity_getMapName(JNIEnv* jenv, jobject obj) {

    if (gameLogic == nullptr) {
        return jenv->NewStringUTF("");
    }

    jstring result = jenv->NewStringUTF(gameLogic->getCurrentMapName().c_str());

    return result;
}