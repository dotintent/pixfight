package com.noclip.marcinmalysz.pixfight;

import android.Manifest;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Gravity;
import android.view.WindowManager;
import android.widget.Toast;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class PFActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("fmod");
        System.loadLibrary("fmodL");
        System.loadLibrary("native-lib");
    }

    private static final int WRITE_STORAGEREQUESTCODE = 1337; //This should be unique somehow
    private boolean audioInitialized = false;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);

        // ResourcesCompat.getFont(this, R.font.fffatlan);

        setContentView(R.layout.activity_main);
        getSupportFragmentManager().beginTransaction().replace(R.id.fragmentContainer, new PFMainMenuFragment()).commit();

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        PFImmersiveMode.SetImmersiveMode(getWindow());

        if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED) {

            ActivityCompat.requestPermissions(this, new String[]{ Manifest.permission.WRITE_EXTERNAL_STORAGE}, WRITE_STORAGEREQUESTCODE);
            return;
        }

        preapreForCopy();
    }

    @Override
    protected void onPause() {
        super.onPause();

        if (isAudioPlaying() && audioInitialized) {
            PFAudioWrapper.muteMusic();
        }
    }

    @Override
    public void onRequestPermissionsResult(final int requestCode, @NonNull final String[] permissions, @NonNull final int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        if (requestCode == WRITE_STORAGEREQUESTCODE) {

            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {

                preapreForCopy();
            }
        }
    }

    private void preapreForCopy() {

        Log.d("[STORAGE]", Environment.getExternalStorageDirectory().toString());

        //TOOD: dodac plik .nomedia przy kopiowaniu
        File dir = new File(Environment.getExternalStorageDirectory(), "PIXFIGHTDATA");

        if(!dir.exists()) {

            if ( dir.mkdir() ) {

                //Copy assets
                copyAssets();
            }
            else {

                CharSequence text = "Please enable external storage access for game";
                int duration = Toast.LENGTH_LONG;

                Toast toast = Toast.makeText(getApplicationContext(), text, duration);
                toast.setGravity(Gravity.TOP|Gravity.CENTER, 0, 0);
                toast.show();

                Log.d("[ERROR]", "COULD NOT CREATE DIRECTORY FOR ASSETS");
            }
        }
        else {

            Log.d("[INFO]", "DIRECTORY ALREADY EXIST");
        }

        String noMediaLocation = Environment.getExternalStorageDirectory().toString() + "/PIXFIGHTDATA/.nomedia";
        File noMediaFile = new File(noMediaLocation);

        if (!noMediaFile.exists()) {
            try {
                //noinspection ResultOfMethodCallIgnored
                noMediaFile.createNewFile();
            } catch (IOException ignored) { }
        }

        org.fmod.FMOD.init(this);

        if (dir.exists()) {

            PFAudioWrapper.initializeAudio(this, dir.toString());
            audioInitialized = true;
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        org.fmod.FMOD.close();
    }

    private void copyFile(InputStream in, OutputStream out) throws IOException {
        byte[] buffer = new byte[1024];
        int read;
        while((read = in.read(buffer)) != -1){
            out.write(buffer, 0, read);
        }
    }

    private void copyAssets() {

        CharSequence text = "COPING ASSETS...";
        int duration = Toast.LENGTH_SHORT;

        Toast toast = Toast.makeText(getApplicationContext(), text, duration);
        toast.setGravity(Gravity.TOP|Gravity.CENTER, 0, 0);
        toast.show();

        AssetManager assetManager = getAssets();

        String[] files = null;

        try {

            files = assetManager.list("");

        } catch (IOException e) {

            Log.e("[ERROR]", "Failed to get asset file list.", e);
        }

        if (files != null) {
            Log.d("Files", "Size: "+ files.length);

            for (String filename : files) {
                Log.d("Files", "FileName:" + filename);
                
                if (filename.startsWith("images") || filename.startsWith("webkit") || filename.startsWith("sounds")) {
                    continue;
                }

                File outFile = new File(Environment.getExternalStorageDirectory() + "/PIXFIGHTDATA/", filename);
                try (InputStream in = assetManager.open(filename); OutputStream out = new FileOutputStream(outFile)) {

                    copyFile(in, out);

                } catch (IOException e) {

                    Log.e("[ERROR]", "Failed to copy asset file: " + filename, e);
                }
            }
        }

        CharSequence textFinish = "COPING ASSETS FINISHED.";

        toast = Toast.makeText(getApplicationContext(), textFinish, duration);
        toast.setGravity(Gravity.TOP|Gravity.CENTER, 0, 0);
        toast.show();

        Log.d("[INFO]", "COPING ASSETS FINISHED.");
    }

    @Override
    protected void onPostResume() {
        super.onPostResume();

        PFImmersiveMode.SetImmersiveMode(getWindow());

        if (isAudioPlaying() && audioInitialized) {
            PFAudioWrapper.unmuteMusic();
        }
    }

    private boolean isAudioPlaying() {
        SharedPreferences preferences = getSharedPreferences("PixFightPreferences", MODE_PRIVATE);
        return !preferences.getBoolean("mute", false);
    }

}
