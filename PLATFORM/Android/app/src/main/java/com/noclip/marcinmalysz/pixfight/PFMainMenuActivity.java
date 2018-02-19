package com.noclip.marcinmalysz.pixfight;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Build;
import android.support.annotation.NonNull;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.support.v4.content.res.ResourcesCompat;

import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.view.Gravity;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.Toast;
import android.util.Log;
import android.content.Intent;
import android.graphics.Typeface;
import android.content.res.AssetManager;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileOutputStream;

public class PFMainMenuActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("fmod");
        System.loadLibrary("fmodL");
        System.loadLibrary("native-lib");
    }

    private final int WRITE_STORAGEREQUESTCODE = 1337; //This should be unique somehow
    private Typeface font = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);

        font = Typeface.createFromAsset(getAssets(), "FFFATLAN.TTF");
        // ResourcesCompat.getFont(this, R.font.fffatlan);

        getSupportActionBar().hide();
        setContentView(R.layout.activity_pfmain_menu);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        PFImmersiveMode.SetImmersiveMode(getWindow());

        addListenerOnNewGameButton();
        addListenerOnLoadGameButton();
        addListenerOnSettingsButton();

        if ((Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) &&
                (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED)) {

            requestPermissions(new String[]{ Manifest.permission.WRITE_EXTERNAL_STORAGE}, WRITE_STORAGEREQUESTCODE);
            return;
        }

        preapreForCopy();
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

        org.fmod.FMOD.init(this);

        if (dir.exists()) {

            PFAudioWrapper.initializeAudio(this, dir.toString());
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

        Log.d("Files", "Size: "+ files.length);
        for (int i = 0; i < files.length; i++)
        {
            Log.d("Files", "FileName:" + files[i]);
        }

        if (files != null) for (String filename : files) {

            InputStream in = null;
            OutputStream out = null;

            if (filename.startsWith("images") || filename.startsWith("webkit") || filename.startsWith("sounds")) {
                continue;
            }

            try {

                in = assetManager.open(filename);
                File outFile = new File(Environment.getExternalStorageDirectory() + "/PIXFIGHTDATA/", filename);
                out = new FileOutputStream(outFile);
                copyFile(in, out);

            } catch(IOException e) {

                Log.e("[ERROR]", "Failed to copy asset file: " + filename, e);
            }
            finally {

                if (in != null) {

                    try {

                        in.close();

                    } catch (IOException e) {
                        // NOOP
                    }
                }
                if (out != null) {

                    try {

                        out.close();

                    } catch (IOException e) {
                        // NOOP
                    }
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
    }

    public void addListenerOnSettingsButton() {

        Button button = findViewById(R.id.imageButtonSettings);
        button.setTypeface(font);

        button.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View arg0) {

                PFAudioWrapper.playSelectSound();
                Intent intent = new Intent(PFMainMenuActivity.this, PFSettingsActivity.class);
                startActivity(intent);
            }
        });
    }

    public void addListenerOnLoadGameButton() {

        Button button = findViewById(R.id.imageButtonLoadGame);
        button.setTypeface(font);

        button.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View arg0) {

                PFAudioWrapper.playSelectSound();
                Intent intent = new Intent(PFMainMenuActivity.this, PFLoadGameActivity.class);
                startActivity(intent);
            }
        });
    }

    public void addListenerOnNewGameButton() {

        Button button = findViewById(R.id.imageButtonNewGame);
        button.setTypeface(font);

        button.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View arg0) {

                PFAudioWrapper.playSelectSound();
                Intent intent = new Intent(PFMainMenuActivity.this, PFNewGameActivity.class);
                startActivity(intent);
            }
        });
    }
}
