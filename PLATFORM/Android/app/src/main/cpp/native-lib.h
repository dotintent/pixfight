//
// Created by Marcin Małysz on 30/01/2018.
//

#ifndef ANDROID_NATIVE_LIB_H
#define ANDROID_NATIVE_LIB_H

#include <stdint.h>
#include <jni.h>

extern "C" {

//AUDIO

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFAudioWrapper_initializeAudio(JNIEnv* jenv, jobject instance, jobject activity, jstring rootpath);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFAudioWrapper_playSelectSound(JNIEnv* jenv, jobject instance);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFAudioWrapper_playMenuMusic(JNIEnv* jenv, jobject instance);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFAudioWrapper_playGameMusic(JNIEnv* jenv, jobject instance);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFAudioWrapper_muteMusic(JNIEnv* jenv, jobject instance);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFAudioWrapper_unmuteMusic(JNIEnv* jenv, jobject instance);

//GAME
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFGameLib_renderFrame(JNIEnv* jenv, jobject obj);

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFGameLib_nativeOnStart(JNIEnv* jenv, jobject obj, jobject activity);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFGameLib_nativeOnStop(JNIEnv* jenv, jobject obj);

//LOGIC
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFGameLib_nativeStartGame(JNIEnv* jenv, jobject obj, jstring map, jint playerSelected, jint playersPlaying);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFGameLib_nativeLoadGame(JNIEnv* jenv, jobject obj, jstring path);

//INTERACTION

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_undo(JNIEnv* jenv, jobject obj);
JNIEXPORT jboolean JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_canUndo(JNIEnv* jenv, jobject obj);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_initializeOpenGL(JNIEnv* jenv, jobject obj, jint width, jint height);
JNIEXPORT jboolean JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_canEndTurn(JNIEnv* jenv, jobject obj);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_endTurn(JNIEnv* jenv, jobject obj);
JNIEXPORT jint  JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_multiplyTime(JNIEnv* jenv, jobject obj);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_finishBuilding(JNIEnv* jenv, jobject obj, jint unitid, jint cash);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_tapAction(JNIEnv* jenv, jobject obj, jfloat x, jfloat y);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_scaleAction(JNIEnv* jenv, jobject obj, jfloat scale);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_panAction(JNIEnv* jenv, jobject obj, jfloat x, jfloat y);
JNIEXPORT jboolean JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_saveGame(JNIEnv* jenv, jobject obj, jstring savepath);
JNIEXPORT jstring JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderFragment_getMapName(JNIEnv* jenv, jobject obj);

//MULTIPLAYER

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFMainMenuFragment_disposeClient(JNIEnv* jenv, jobject obj);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFMultiplayerFragment_initializeClient(JNIEnv* jenv, jobject obj);
JNIEXPORT jboolean JNICALL Java_com_noclip_marcinmalysz_pixfight_PFMultiplayerFragment_connectToServer(JNIEnv* jenv, jobject obj);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFMultiplayerFragment_makeServerRoom(JNIEnv* jenv, jobject obj, jboolean privateroom);

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFMakeRoomFragment_removeRoom(JNIEnv* jenv, jobject obj);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFMakeRoomFragment_leaveRoom(JNIEnv* jenv, jobject obj);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFMakeRoomFragment_setReady(JNIEnv* jenv, jobject obj);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFMakeRoomFragment_getRoomDetails(JNIEnv* jenv, jobject obj);
JNIEXPORT jint JNICALL Java_com_noclip_marcinmalysz_pixfight_PFMakeRoomFragment_getRoomPort(JNIEnv* jenv, jobject obj);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFMakeRoomFragment_updateRoomInfo(JNIEnv* jenv, jobject obj, jstring mapname, jint players);

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFJoinRoomFragment_listRooms(JNIEnv* jenv, jobject obj);
JNIEXPORT jboolean JNICALL Java_com_noclip_marcinmalysz_pixfight_PFJoinRoomFragment_joinRoom(JNIEnv* jenv, jobject obj, jint roomID);

};

#endif //ANDROID_NATIVE_LIB_H
