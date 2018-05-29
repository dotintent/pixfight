package com.noclip.marcinmalysz.pixfight;

import android.content.Context;
import android.content.SharedPreferences;
import android.graphics.Typeface;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CompoundButton;
import android.widget.Switch;

public class PFSettingsFragment extends Fragment implements CompoundButton.OnCheckedChangeListener {

    private Switch aiSwitch = null;
    private Switch muteSwitch = null;
    private SharedPreferences preferences = null;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View layout = inflater.inflate(R.layout.fragment_pfsettings, container, false);

        muteSwitch = layout.findViewById(R.id.mute_switch);
        aiSwitch = layout.findViewById(R.id.ai_switch);

        Typeface font = Typeface.createFromAsset(getContext().getAssets(), "FFFATLAN.TTF");
        muteSwitch.setTypeface(font);
        aiSwitch.setTypeface(font);

        View backButton = layout.findViewById(R.id.settings_back);
        backButton.setOnClickListener(v -> getFragmentManager().popBackStack());

        preferences = getContext().getSharedPreferences("PixFightPreferences", Context.MODE_PRIVATE);

        muteSwitch.setChecked(preferences.getBoolean("mute", false));
        aiSwitch.setChecked(preferences.getBoolean("hardai", false));

        //listeners
        muteSwitch.setOnCheckedChangeListener(this);
        aiSwitch.setOnCheckedChangeListener(this);

        return layout;
    }

    @Override
    public void onCheckedChanged(@NonNull CompoundButton buttonView, boolean isChecked) {

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
