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

jclass renderClass = 0;
jclass menuClass = 0;
jclass multiplayerClass = 0;
jclass makeRoomClass = 0;
jclass joinRoomClass = 0;

jmethodID onWinEventBridge = 0;
jmethodID onLoseEventBridge = 0;
jmethodID botsStartThinkEventBridge = 0;
jmethodID botsEndThinkEventBridge = 0;
jmethodID onBaseSelectedBridge = 0;
jmethodID onRoomEventBridge = 0;
jmethodID onDisconnectBridge = 0;
jmethodID onUpdateMap = 0;
jmethodID onUpdatePlayerBridge = 0;
jmethodID onPlayerTurnBridge = 0;
jmethodID onMutliplayerEndGameBridge = 0;
jmethodID onLoadMapBridge = 0;
jmethodID onRoomRefresh = 0;

jint JNI_OnLoad(JavaVM* aVm, void* aReserved) {

    javaVM = aVm;
    javaVM->GetEnv((void**)&env, JNI_VERSION_1_6);

    jclass render = env->FindClass("com/noclip/marcinmalysz/pixfight/PFRenderFragment");
    jclass menu = env->FindClass("com/noclip/marcinmalysz/pixfight/PFMainMenuFragment");
    jclass multi = env->FindClass("com/noclip/marcinmalysz/pixfight/PFMultiplayerFragment");
    jclass make = env->FindClass("com/noclip/marcinmalysz/pixfight/PFMakeRoomFragment");
    jclass join = env->FindClass("com/noclip/marcinmalysz/pixfight/PFJoinRoomFragment");

    renderClass = reinterpret_cast<jclass>(env->NewGlobalRef(render));
    menuClass = reinterpret_cast<jclass>(env->NewGlobalRef(menu));
    multiplayerClass = reinterpret_cast<jclass>(env->NewGlobalRef(multi));
    makeRoomClass = reinterpret_cast<jclass>(env->NewGlobalRef(make));
    joinRoomClass = reinterpret_cast<jclass>(env->NewGlobalRef(join));

    env->DeleteLocalRef(render);
    env->DeleteLocalRef(menu);
    env->DeleteLocalRef(multi);
    env->DeleteLocalRef(make);
    env->DeleteLocalRef(join);

    onWinEventBridge = env->GetStaticMethodID(renderClass, "onWinEventBridge", "()V");
    onLoseEventBridge = env->GetStaticMethodID(renderClass, "onLoseEventBridge", "()V");
    botsStartThinkEventBridge = env->GetStaticMethodID(renderClass, "botsStartThinkEventBridge", "()V");
    botsEndThinkEventBridge = env->GetStaticMethodID(renderClass, "botsEndThinkEventBridge", "()V");
    onBaseSelectedBridge = env->GetStaticMethodID(renderClass, "onBaseSelectedBridge", "(II)V");
    onRoomEventBridge = env->GetStaticMethodID(multiplayerClass, "onRoomEventBridge", "()V");
    onDisconnectBridge = env->GetStaticMethodID(menuClass, "onDisconnectBridge", "()V");
    onUpdateMap = env->GetStaticMethodID(makeRoomClass, "onUpdateMap", "(Ljava/lang/String;)V");
    onUpdatePlayerBridge = env->GetStaticMethodID(renderClass, "onUpdatePlayerBridge", "(Z)V");
    onPlayerTurnBridge = env->GetStaticMethodID(renderClass, "onPlayerTurnBridge", "()V");
    onMutliplayerEndGameBridge = env->GetStaticMethodID(renderClass, "onMutliplayerEndGameBridge", "(I)V");
    onLoadMapBridge = env->GetStaticMethodID(makeRoomClass, "onLoadMapBridge", "(IILjava/lang/String;)V");
    onRoomRefresh = env->GetStaticMethodID(joinRoomClass, "onRoomRefresh", "([Ljava/lang/String;[I)V");

    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM* vm, void* reserved) {

    env->DeleteGlobalRef(renderClass);
    env->DeleteGlobalRef(menuClass);
    env->DeleteGlobalRef(multiplayerClass);
    env->DeleteGlobalRef(makeRoomClass);
    env->DeleteGlobalRef(joinRoomClass);

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

void callNativeVoidMethod(jclass globalClass, jmethodID mid) {

    attachThread();
    env->CallStaticVoidMethod(globalClass, mid);
}

void callNativeVoidMethodParams(jclass globalClass, jmethodID mid, int param1, int param2) {

    attachThread();
    env->CallStaticVoidMethod(globalClass, mid, param1, param2);
}

void callNativeVoidMethodParamString(jclass globalClass, jmethodID mid, std::string str) {

    attachThread();

    jstring jStringParam = env->NewStringUTF(str.c_str());

    env->CallStaticVoidMethod(globalClass, mid, jStringParam);
    env->DeleteLocalRef(jStringParam);
}

void callNativeVoidMethodParamStringIntInt(jclass globalClass, jmethodID mid, std::string str, int param1, int param2) {

    attachThread();

    jstring jStringParam = env->NewStringUTF(str.c_str());

    env->CallStaticVoidMethod(globalClass, mid, param1, param2, jStringParam);
    env->DeleteLocalRef(jStringParam);
}

void callNativeVoidMethodArray(std::vector<PFRoomInfo> rooms) {

    attachThread();

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

        time_t localTime = room.createdDate;
        timeinfo = localtime(&localTime);
        strftime(buffer, sizeof(buffer), "%m/%d/%Y %I:%M", timeinfo);

        std::string roomstr(room.mapname);

        roomstr += " - ";
        roomstr += buffer;

        str = env->NewStringUTF(roomstr.c_str());
        env->SetObjectArrayElement(stringArray, i, str);
        env->DeleteLocalRef(str);
    }

    env->SetIntArrayRegion(portArray, 0, size, tmp);

    env->CallStaticVoidMethod(joinRoomClass, onRoomRefresh, stringArray, portArray);

    env->DeleteLocalRef(stringArray);
    env->DeleteLocalRef(portArray);
}

void callNativeVoidMethodInteger(jmethodID mid, int param1) {

    attachThread();
    env->CallStaticVoidMethod(renderClass, mid, param1);
}

void callNativeVoidMethodBool(jmethodID mid, jboolean param1) {

    attachThread();
    env->CallStaticVoidMethod(renderClass, mid, param1);
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

        callNativeVoidMethod(renderClass, onWinEventBridge);
    };

    gameLogic->loseGameCallback = [](void* context) {

        callNativeVoidMethod(renderClass, onLoseEventBridge);
    };

    gameLogic->botsStartThinkCallback = [](void* context) {

        callNativeVoidMethod(renderClass, botsStartThinkEventBridge);
    };

    gameLogic->botsEndThinkCallback = [](void* context) {

        callNativeVoidMethod(renderClass, botsEndThinkEventBridge);
    };

    gameLogic->baseSelectedCallback = [](void* context, GameBase *base) {

        selectedBase = base;

        int teamID = selectedBase->getTeamID();
        int cash = gameLogic->getPlayerCash();

        callNativeVoidMethodParams(renderClass, onBaseSelectedBridge, teamID, cash);
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

                callNativeVoidMethod(multiplayerClass, onRoomEventBridge);
            }
                break;

            case PFSocketCommandTypeDisconnect: {

                client->disconnect();
                callNativeVoidMethod(multiplayerClass, onDisconnectBridge);
            }
                break;

            case PFSocketCommandTypeGameInfo: {

                PFRoomInfo info;
                memcpy(&info, data.data(), data.size() * sizeof(uint8_t));

                client->setRoomInfo(info);

                callNativeVoidMethodParamString(makeRoomClass, onUpdateMap, info.mapname);
            }
                break;

            case PFSocketCommandTypeSendTurn: {

                uint32_t currentPlayerTurn = 0;
                memcpy(&currentPlayerTurn, data.data(), data.size() * sizeof(uint8_t));
                uint32_t playerID = currentPlayerTurn + 1;

                bool unlock = PLAYERTEAMSELECTED == playerID;

                callNativeVoidMethodBool(onUpdatePlayerBridge, static_cast<jboolean>(unlock));

                if (unlock) {

                    gameLogic->startTurn();
                    callNativeVoidMethod(renderClass, onPlayerTurnBridge);
                }
            }
                break;

            case PFSocketCommandTypeEndGame: {

                uint32_t winnnerID = 0;
                memcpy(&winnnerID, data.data(), data.size() * sizeof(uint8_t));

                client->disconnect();
                gameLogic->startTurn();

                callNativeVoidMethodInteger(onMutliplayerEndGameBridge, winnnerID);
            }
                break;

            case PFSocketCommandTypeLoad: {

                uint32_t playerId = 0;
                memcpy(&playerId, data.data(), sizeof(uint32_t));

                PFRoomInfo info = client->getRoomInfo();

                callNativeVoidMethodParamStringIntInt(makeRoomClass, onLoadMapBridge, info.mapname, playerId+1, info.players);
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

                uint32_t idA = 0;
                uint32_t idB = 0;
                uint32_t sizeA = 0;
                uint32_t sizeB = 0;

                memcpy(&idA, data.data(), sizeof(uint32_t));
                memcpy(&idB, data.data() + sizeof(uint32_t), sizeof(uint32_t));
                memcpy(&sizeA, data.data() + sizeof(uint32_t) * 2, sizeof(uint32_t));
                memcpy(&sizeB, data.data() + sizeof(uint32_t) * 3, sizeof(uint32_t));

                gameLogic->remoteAttackUnit(idA, idB, sizeA, sizeB);
            }
                break;

            case PFSocketCommandTypeMove: {

                uint32_t unitID = 0;
                float posX = 0;
                float posY = 0;

                memcpy(&unitID, data.data(), sizeof(uint32_t));
                memcpy(&posX, data.data() + sizeof(uint32_t), sizeof(float));
                memcpy(&posY, data.data() + sizeof(uint32_t) + sizeof(float), sizeof(float));

                gameLogic->remoteMoveUnit(unitID, posX, posY);
            }
                break;

            case PFSocketCommandTypeBuild: {

                uint32_t baseID = 0;
                uint16_t unit = 0;

                memcpy(&baseID, data.data(), sizeof(uint32_t));
                memcpy(&unit, data.data() + sizeof(uint32_t), sizeof(uint16_t));

                gameLogic->remoteBuildUnit(baseID, unit);
            }
                break;

            case PFSocketCommandTypeCapture: {

                uint32_t baseID = 0;
                uint32_t unitID = 0;

                memcpy(&baseID, data.data(), sizeof(uint32_t));
                memcpy(&unitID, data.data() + sizeof(uint32_t), sizeof(uint32_t));

                gameLogic->remoteCaptureBase(baseID, unitID);
            }
                break;

            case PFSocketCommandTypeRepair: {

                uint32_t baseID = 0;
                uint32_t unitID = 0;

                memcpy(&baseID, data.data(), sizeof(uint32_t));
                memcpy(&unitID, data.data() + sizeof(uint32_t), sizeof(uint32_t));

                gameLogic->remoteRepairUnit(baseID, unitID);
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

    if (client == nullptr) {

        return;
    }

    client->listRooms();
}

JNIEXPORT jboolean JNICALL Java_com_noclip_marcinmalysz_pixfight_PFJoinRoomFragment_joinRoom(JNIEnv* jenv, jobject obj, jint roomID) {

    if (client == nullptr) {

        return false;
    }

    return static_cast<jboolean>(client->joinRoom(roomID));
}


JNIEXPORT jboolean JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_isMultiplayerMode(JNIEnv* jenv, jobject obj) {

    return static_cast<jboolean >(client != nullptr);
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_sendLoaded(JNIEnv* jenv, jobject obj) {

    if (client == nullptr) {

        return;
    }

    client->setLoaded();
}