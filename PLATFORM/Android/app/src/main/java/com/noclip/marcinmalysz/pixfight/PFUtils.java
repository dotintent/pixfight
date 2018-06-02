package com.noclip.marcinmalysz.pixfight;

import android.content.res.AssetManager;
import android.graphics.Typeface;
import android.support.annotation.NonNull;

public class PFUtils {

    private static Typeface font;

    static void loadFontFromAssets(@NonNull AssetManager assetManager) {
        font = Typeface.createFromAsset(assetManager, "FFFATLAN.TTF");
    }

    @NonNull
    public static Typeface getFont() {
        return font;
    }

}
