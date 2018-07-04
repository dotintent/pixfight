package com.noclip.marcinmalysz.pixfight;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentTransaction;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

public class PFMainMenuFragment extends Fragment {

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_pfmain_menu, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        getView().findViewById(R.id.imageButtonNewGame).setOnClickListener(v -> goToFragment(new PFNewGameFragment()));
        getView().findViewById(R.id.imageButtonMultiplayer).setOnClickListener(v -> goToFragment(new PFMultiplayerFragment()));
        getView().findViewById(R.id.imageButtonLoadGame).setOnClickListener(v -> goToFragment(new PFLoadGameFragment()));
        getView().findViewById(R.id.imageButtonSettings).setOnClickListener(v -> goToFragment(new PFSettingsFragment()));
    }

    private void goToFragment(Fragment fragment) {
        PFAudioWrapper.playSelectSound();
        FragmentTransaction fragmentTransaction = getFragmentManager().beginTransaction();
        fragmentTransaction.replace(R.id.fragmentContainer, fragment);
        fragmentTransaction.addToBackStack(null);
        fragmentTransaction.commit();
    }

}
