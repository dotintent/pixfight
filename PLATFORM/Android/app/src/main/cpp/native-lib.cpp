#include "native-lib.h"
#include "GameLogic.hpp"
#include "Audio.hpp"

#include <android/log.h>
#include <cstdio>
#include <unistd.h>
#include <pthread.h>

#include <android/native_activity.h>
#include <EGL/egl.h>
#include <PFServerCommandType.hpp>

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
static std::shared_ptr<PFMultiplayerClient> client = nullptr;
static GameBase *selectedBase = nullptr;
static bool useHardAI = false;

void callNativeVoidMethod(std::string classname, std::string method) {

    attachThread();

    std::string findclass = "com/noclip/marcinmalysz/pixfight/" + classname;

    jclass cls = env->FindClass(findclass.c_str());

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

    jclass cls = env->FindClass("com/noclip/marcinmalysz/pixfight/PFRenderFragment");

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

void callNativeVoidMethodParamString(std::string classname, std::string method, std::string str) {

    attachThread();

    std::string findclass = "com/noclip/marcinmalysz/pixfight/" + classname;

    jclass cls = env->FindClass(findclass.c_str());

    if (!cls) {
        return;
    }

    jclass globalClass = reinterpret_cast<jclass>(env->NewGlobalRef(cls));

    if (!globalClass) {
        return;
    }

    env->DeleteLocalRef(cls);

    jmethodID mid = env->GetStaticMethodID(globalClass, method.c_str(), "(Ljava/lang/String;)V");

    if (!mid) {
        return;
    }

    jstring jStringParam = env->NewStringUTF(str.c_str());

    env->CallStaticVoidMethod(globalClass, mid, jStringParam);
    env->DeleteGlobalRef(globalClass);
    env->DeleteLocalRef(jStringParam);
}

void callNativeVoidMethodParamStringIntInt(std::string classname, std::string method, std::string str, int param1, int param2) {

    attachThread();

    std::string findclass = "com/noclip/marcinmalysz/pixfight/" + classname;

    jclass cls = env->FindClass(findclass.c_str());

    if (!cls) {
        return;
    }

    jclass globalClass = reinterpret_cast<jclass>(env->NewGlobalRef(cls));

    if (!globalClass) {
        return;
    }

    env->DeleteLocalRef(cls);

    jmethodID mid = env->GetStaticMethodID(globalClass, method.c_str(), "(IILjava/lang/String;)V");

    if (!mid) {
        return;
    }

    jstring jStringParam = env->NewStringUTF(str.c_str());

    env->CallStaticVoidMethod(globalClass, mid, param1, param2, jStringParam);
    env->DeleteGlobalRef(globalClass);
    env->DeleteLocalRef(jStringParam);
}

void callNativeVoidMethodArray(std::vector<PFRoomInfo> rooms) {

    attachThread();

    jclass cls = env->FindClass("com/noclip/marcinmalysz/pixfight/PFJoinRoomFragment");

    if (!cls) {
        return;
    }

    jclass globalClass = reinterpret_cast<jclass>(env->NewGlobalRef(cls));

    if (!globalClass) {
        return;
    }

    env->DeleteLocalRef(cls);

    jmethodID mid = env->GetStaticMethodID(globalClass, "onRoomRefresh", "([Ljava/lang/String;[I)V");

    if (!mid) {
        return;
    }

    int size = rooms.size();

    jint tmp[size];
    jstring str;

    jobjectArray stringArray = env->NewObjectArray(size, env->FindClass("java/lang/String"),0);
    jintArray portArray = env->NewIntArray(size);

    for (int i = 0; i < size; ++i) {

        PFRoomInfo room = rooms[i];

        tmp[i] = room.roomPort;

        struct tm * timeinfo;
        char buffer[30];
        timeinfo = localtime(&room.createdDate);
        strftime(buffer, sizeof(buffer), "%m/%d/%Y %I:%M", timeinfo);

        std::string roomstr(room.mapname);

        roomstr += " - ";
        roomstr += buffer;

        str = env->NewStringUTF(roomstr.c_str());
        env->SetObjectArrayElement(stringArray, i, str);
        env->DeleteLocalRef(str);
    }

    env->SetIntArrayRegion(portArray, 0, size, tmp);

    env->CallStaticVoidMethod(globalClass, mid, stringArray, portArray);
    env->DeleteGlobalRef(globalClass);

    env->DeleteLocalRef(stringArray);
    env->DeleteLocalRef(portArray);
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_initializeOpenGL(JNIEnv* jenv, jobject obj, jint width, jint height) {

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

        callNativeVoidMethod("PFRenderFragment", "onWinEventBridge");
    };

    gameLogic->loseGameCallback = [](void* context) {

        callNativeVoidMethod("PFRenderFragment", "onLoseEventBridge");
    };

    gameLogic->botsStartThinkCallback = [](void* context) {

        callNativeVoidMethod("PFRenderFragment", "botsStartThinkEventBridge");
    };

    gameLogic->botsEndThinkCallback = [](void* context) {

        callNativeVoidMethod("PFRenderFragment", "botsEndThinkEventBridge");
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

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFGameLib_nativeStartGame(JNIEnv* jenv, jobject obj, jstring map, jint playerSelected, jint playersPlaying) {

    __android_log_print(ANDROID_LOG_DEBUG, "[PIXFIGHT]", "nativeStartGame");

    if (gameLogic == nullptr) {
        return;
    }

    std::string mapName = jstringTostring(jenv, map);
    gameLogic->createNewGame(mapName, playerSelected, playersPlaying, client);
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

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_undo(JNIEnv* jenv, jobject obj) {

    if (gameLogic == nullptr) {
        return;
    }

    gameLogic->undo();
}

JNIEXPORT jboolean JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_canUndo(JNIEnv* jenv, jobject obj) {

    if (gameLogic == nullptr) {
        return JNI_FALSE;
    }

    return static_cast<jboolean>(gameLogic->canUndo() && gameLogic->canEndTurn());
}

JNIEXPORT jboolean JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_canEndTurn(JNIEnv* jenv, jobject obj) {

    if (gameLogic == nullptr) {
        return JNI_FALSE;
    }

    return static_cast<jboolean>(gameLogic->canEndTurn());
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_endTurn(JNIEnv* jenv, jobject obj) {

    if (gameLogic == nullptr) {
        return;
    }

    gameLogic->endTurn();
}

JNIEXPORT jint JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_multiplyTime(JNIEnv* jenv, jobject obj) {

    if (gameLogic == nullptr) {
        return 1;
    }

    return (int)gameLogic->multiplyTime();
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_finishBuilding(JNIEnv* jenv, jobject obj, jint unitid, jint cash) {

    //we are coming from java UI thread!
    syncToMainLoop([unitid, cash](void *context, GameLogic *logic){

        logic->buildNewUnitFromBase(selectedBase, unitid, cash);
    });
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_tapAction(JNIEnv* jenv, jobject obj, jfloat x, jfloat y) {

    if (gameLogic == nullptr) {
        return;
    }

    xVec2 touchPoint = xVec2(x, y);

    syncToMainLoop([touchPoint](void *context, GameLogic *logic) {

        gameLogic->touchDownAtPoint(touchPoint);
    });
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_scaleAction(JNIEnv* jenv, jobject obj, jfloat scale) {

    if (gameLogic == nullptr) {
        return;
    }

    syncToMainLoop([scale](void *context, GameLogic *logic) {

        gameLogic->setCurrentScale(scale);
    });
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_panAction(JNIEnv* jenv, jobject obj, jfloat x, jfloat y) {

    if (gameLogic == nullptr) {
        return;
    }

    xVec2 dir = xVec2(x, y);

    syncToMainLoop([dir](void *context, GameLogic *logic) {

        gameLogic->setDirectionVec(dir);
    });
}

JNIEXPORT jboolean JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_saveGame(JNIEnv* jenv, jobject obj, jstring savepath) {

    if (gameLogic == nullptr) {
        return JNI_FALSE;
    }

    std::string path = jstringTostring(jenv, savepath);

    return static_cast<jboolean>(gameLogic->saveGame(path));
}

JNIEXPORT jstring JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_getMapName(JNIEnv* jenv, jobject obj) {

    if (gameLogic == nullptr) {
        return jenv->NewStringUTF("");
    }

    jstring result = jenv->NewStringUTF(gameLogic->getCurrentMapName().c_str());

    return result;
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFMainMenuFragment_disposeClient(JNIEnv* jenv, jobject obj) {

    if (client) {
        client->callback = nullptr;
        client = nullptr;
    }
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFMultiplayerFragment_initializeClient(JNIEnv* jenv, jobject obj) {

    client = std::make_shared<PFMultiplayerClient>(DEFAULT_SERVER_ADDR);

    client->callback = [=](const PFSocketCommandType command, const vector<uint8_t> data){

        switch (command) {

            case PFSocketCommandTypeUnknown:
            case PFSocketCommandTypeHeartbeat:
            case PFSocketCommandTypeLeaveRoom:
            case PFSocketCommandTypeRemoveRoom:
            case PFSocketCommandTypeGetGameInfo:
            case PFSocketCommandTypeReady:
            case PFSocketCommandTypeOk:
                break;

            case PFSocketCommandTypeMakeRoom: {

                callNativeVoidMethod("PFMultiplayerFragment", "onRoomEventBridge");
            }
                break;

            case PFSocketCommandTypeDisconnect: {

                callNativeVoidMethod("PFMainMenuFragment", "onDisconnectBridge");
            }
                break;

            case PFSocketCommandTypeGameInfo: {

                PFRoomInfo info;
                memcpy(&info, data.data(), data.size() * sizeof(uint8_t));

                client->setRoomInfo(info);

                callNativeVoidMethodParamString("PFMakeRoomFragment", "onUpdateMap", info.mapname);
            }
                break;

            case PFSocketCommandTypeSendTurn: {

            }
                break;

            case PFSocketCommandTypeEndGame: {

            }
                break;

            case PFSocketCommandTypeLoad: {

                uint32_t playerId = 0;
                memcpy(&playerId, data.data(), sizeof(uint32_t));

                PFRoomInfo info = client->getRoomInfo();

                callNativeVoidMethodParamStringIntInt("PFMakeRoomFragment", "onLoadMapBridge", info.mapname, playerId+1, info.players);
            }
                break;

            case PFSocketCommandTypeRooms: {

                size_t roomsSize = (data.size() * sizeof(uint8_t)) / sizeof(PFRoomInfo);

                std::vector<PFRoomInfo> rooms(roomsSize);

                memcpy(rooms.data(), data.data(), data.size() * sizeof(uint8_t));

                callNativeVoidMethodArray(rooms);
            }
                break;

            case PFSocketCommandTypeFire: {

            }
                break;

            case PFSocketCommandTypeMove: {

            }
                break;

            case PFSocketCommandTypeBuild: {

            }
                break;

            case PFSocketCommandTypeCapture: {

            }
                break;

            case PFSocketCommandTypeRepair: {

            }
                break;
        }
    };
}

JNIEXPORT jboolean JNICALL Java_com_noclip_marcinmalysz_pixfight_PFMultiplayerFragment_connectToServer(JNIEnv* jenv, jobject obj) {

    if (client == nullptr) {

        return static_cast<jboolean>(false);
    }

    return static_cast<jboolean>(client->connect());
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFMultiplayerFragment_makeServerRoom(JNIEnv* jenv, jobject obj, jboolean privateroom) {


    if (client == nullptr) {

        return;
    }

    client->makeRoom(privateroom);
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFMakeRoomFragment_removeRoom(JNIEnv* jenv, jobject obj) {

    if (client == nullptr) {

        return;
    }

    client->removeRoom();
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFMakeRoomFragment_leaveRoom(JNIEnv* jenv, jobject obj) {

    if (client == nullptr) {

        return;
    }

    client->leaveRoom();
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFMakeRoomFragment_setReady(JNIEnv* jenv, jobject obj) {

    if (client == nullptr) {

        return;
    }

    client->setReady();
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFMakeRoomFragment_getRoomDetails(JNIEnv* jenv, jobject obj) {

    if (client == nullptr) {

        return;
    }

    client->getRoomDetails();
}

JNIEXPORT jint JNICALL Java_com_noclip_marcinmalysz_pixfight_PFMakeRoomFragment_getRoomPort(JNIEnv* jenv, jobject obj) {

    if (client == nullptr) {

        return static_cast<jint>(0);
    }

    return static_cast<jint>(client->getCurrentPort());
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFMakeRoomFragment_updateRoomInfo(JNIEnv* jenv, jobject obj, jstring mapname, jint players) {

    if (client == nullptr) {

        return;
    }

    PFRoomInfo roomInfo;

    std::string mapName = jstringTostring(jenv, mapname);

    strcpy(roomInfo.mapname, mapName.c_str());
    roomInfo.players = players;
    roomInfo.createdDate = time(0);
    roomInfo.roomPort = client->getCurrentPort();

    client->setRoomInfo(roomInfo);
    client->sendRoomDetails();
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFJoinRoomFragment_listRooms(JNIEnv* jenv, jobject obj) {

    client->listRooms();
}

JNIEXPORT jboolean JNICALL Java_com_noclip_marcinmalysz_pixfight_PFJoinRoomFragment_joinRoom(JNIEnv* jenv, jobject obj, jint roomID) {

    return static_cast<jboolean>(client->joinRoom(roomID));
}