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
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFGameLib_nativeStartGame(JNIEnv* jenv, jobject obj, jstring map, int playerSelected, int playersPlaying);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFGameLib_nativeLoadGame(JNIEnv* jenv, jobject obj, jstring path);

//INTERACTION

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderActivity_initializeOpenGL(JNIEnv* jenv, jobject obj, int width, int height);
JNIEXPORT bool JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderActivity_canEndTurn(JNIEnv* jenv, jobject obj);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderActivity_endTurn(JNIEnv* jenv, jobject obj);
JNIEXPORT int  JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderActivity_multiplyTime(JNIEnv* jenv, jobject obj);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderActivity_finishBuilding(JNIEnv* jenv, jobject obj, int unitid, int cash);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderActivity_tapAction(JNIEnv* jenv, jobject obj, float x, float y);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderActivity_scaleAction(JNIEnv* jenv, jobject obj, float scale);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderActivity_panAction(JNIEnv* jenv, jobject obj, float x, float y);
JNIEXPORT bool JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderActivity_saveGame(JNIEnv* jenv, jobject obj, jstring savepath);
JNIEXPORT jstring JNICALL Java_com_noclip_marcinmalysz_pixfight_PFRenderActivity_getMapName(JNIEnv* jenv, jobject obj);

};

#endif //ANDROID_NATIVE_LIB_H
