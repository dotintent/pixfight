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
import android.widget.Switch;


public class PFMultiplayerFragment extends Fragment {

    private Button makeRoomButton = null;
    private Button joinRoomButton = null;
    private Switch privateSwitch = null;

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_pfmultiplayer, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        //back
        Button backButton = getView().findViewById(R.id.multibutton_back);
        backButton.setOnClickListener(arg0 -> getFragmentManager().popBackStack());

        makeRoomButton = getView().findViewById(R.id.imageButtonMakeRoom);
        joinRoomButton = getView().findViewById(R.id.imageButtonJoinRoom);
        privateSwitch = getView().findViewById(R.id.privateSwitch);

        makeRoomButton.setOnClickListener(v -> goToFragment(new PFMakeRoomFragment()));
        joinRoomButton.setOnClickListener(v -> goToFragment(new PFJoinRoomFragment()));

        disableLayout();
    }

    private void disableLayout() {

        makeRoomButton.setEnabled(false);
        joinRoomButton.setEnabled(false);
        privateSwitch.setEnabled(false);
    }

    private void enableLayout() {

        makeRoomButton.setEnabled(true);
        joinRoomButton.setEnabled(true);
        privateSwitch.setEnabled(true);
    }

    private void goToFragment(Fragment fragment) {
        PFAudioWrapper.playSelectSound();
        FragmentTransaction fragmentTransaction = getFragmentManager().beginTransaction();
        fragmentTransaction.replace(R.id.fragmentContainer, fragment);
        fragmentTransaction.addToBackStack(null);
        fragmentTransaction.commit();
    }
}
