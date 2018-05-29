package com.noclip.marcinmalysz.pixfight;

import android.app.Activity;
import android.support.annotation.NonNull;

/**
 * Created by marcinmalysz on 13/02/2018.
 */

public class PFGameLib {

    public static native void nativeOnStart(@NonNull Activity activity);
    public static native void nativeOnStop();

    public static native void nativeStartGame(@NonNull String map, int playerSelected, int playersPlaying);
    public static native void nativeLoadGame(@NonNull String savePath);

    public static native void renderFrame();
}
