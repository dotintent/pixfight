package com.noclip.marcinmalysz.pixfight;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentTransaction;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

public class PFMainMenuFragment extends Fragment {

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_pfmain_menu, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        PFUtils.loadFontFromAssets(getContext().getAssets());

        addListenerOnNewGameButton();
        addListenerOnLoadGameButton();
        addListenerOnSettingsButton();
    }

    public void addListenerOnSettingsButton() {

        Button button = getView().findViewById(R.id.imageButtonSettings);
        button.setTypeface(PFUtils.getFont());

        button.setOnClickListener(arg0 -> goToFragment(new PFSettingsFragment()));
    }

    public void addListenerOnLoadGameButton() {

        Button button = getView().findViewById(R.id.imageButtonLoadGame);
        button.setTypeface(PFUtils.getFont());

        button.setOnClickListener(arg0 -> goToFragment(new PFLoadGameFragment()));
    }

    public void addListenerOnNewGameButton() {

        Button button = getView().findViewById(R.id.imageButtonNewGame);
        button.setTypeface(PFUtils.getFont());

        button.setOnClickListener(arg0 -> goToFragment(new PFNewGameFragment()));
    }

    private void goToFragment(Fragment fragment) {
        PFAudioWrapper.playSelectSound();
        FragmentTransaction fragmentTransaction = getFragmentManager().beginTransaction();
        fragmentTransaction.replace(R.id.fragmentContainer, fragment);
        fragmentTransaction.addToBackStack(null);
        fragmentTransaction.commit();
    }

}
