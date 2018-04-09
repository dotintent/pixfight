package com.noclip.marcinmalysz.pixfight;

import android.graphics.Typeface;
import android.os.Build;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.Switch;
import android.widget.CompoundButton;
import android.content.SharedPreferences;

public class PFSettingsActivity extends AppCompatActivity implements CompoundButton.OnCheckedChangeListener {

    private Typeface font = null;
    private Switch aiSwitch = null;
    private Switch muteSwitch = null;
    private SharedPreferences preferences = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        font = Typeface.createFromAsset(getAssets(), "FFFATLAN.TTF");

        getSupportActionBar().hide();
        setContentView(R.layout.activity_pfsettings);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        PFImmersiveMode.SetImmersiveMode(getWindow());

        Button backButton = findViewById(R.id.settings_back);

        backButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View arg0) {

               finish();
            }
        });

        muteSwitch = findViewById(R.id.mute_switch);
        aiSwitch = findViewById(R.id.ai_switch);

        muteSwitch.setTypeface(font);
        aiSwitch.setTypeface(font);

        //setup initial values

        preferences = this.getSharedPreferences("PixFightPreferences", this.MODE_PRIVATE);

        muteSwitch.setChecked(preferences.getBoolean("mute", false));
        aiSwitch.setChecked(preferences.getBoolean("hardai", false));

        //listeners
        muteSwitch.setOnCheckedChangeListener(this);
        aiSwitch.setOnCheckedChangeListener(this);
    }

    @Override
    protected void onPostResume() {
        super.onPostResume();

        PFImmersiveMode.SetImmersiveMode(getWindow());
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {

        SharedPreferences.Editor editor = preferences.edit();

        if (buttonView.equals(muteSwitch)) {

            editor.putBoolean("mute", isChecked);

            if (isChecked) {

                PFAudioWrapper.muteMusic();
            }
            else {

                PFAudioWrapper.unmuteMusic();
            }

        }
        else if (buttonView.equals(aiSwitch)) {

            editor.putBoolean("hardai", isChecked);

        }

        if (!editor.commit()) {

            Log.d("[SETTINGS]", "Could not save preferences");
        }
    }

}
