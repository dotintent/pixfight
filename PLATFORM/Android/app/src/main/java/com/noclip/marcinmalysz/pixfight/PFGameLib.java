package com.noclip.marcinmalysz.pixfight;

import android.app.Activity;

/**
 * Created by marcinmalysz on 13/02/2018.
 */

public class PFGameLib {

    public static native void nativeOnStart(Activity activity);
    public static native void nativeOnStop();

    public static native void nativeStartGame(String map, int playerSelected, int playersPlaying);
    public static native void nativeLoadGame(String savePath);

    public static native void renderFrame();
}
