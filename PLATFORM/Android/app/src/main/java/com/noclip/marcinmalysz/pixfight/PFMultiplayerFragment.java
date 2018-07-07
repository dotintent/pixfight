package com.noclip.marcinmalysz.pixfight;

import android.os.Bundle;
import android.support.annotation.Keep;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentTransaction;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.Switch;
import android.widget.Toast;


public class PFMultiplayerFragment extends Fragment {

    private Button makeRoomButton = null;
    private Button joinRoomButton = null;
    private Switch privateSwitch = null;
    private boolean connected = false;

    private static PFMultiplayerFragment staticInstance = null;

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

        makeRoomButton.setOnClickListener(v -> {

            if (!connected) {
                return;
            }

            makeServerRoom(privateSwitch.isChecked());
        });

        joinRoomButton.setOnClickListener(v -> {

            goToFragment(new PFJoinRoomFragment());
        });

        disableLayout();
        initializeClient();

        connected = connectToServer();

        if (connected == false) {

            int duration = Toast.LENGTH_SHORT;
            CharSequence textFinish = "Could not connect to server, please try again.";

            Toast toast = Toast.makeText(getActivity().getApplicationContext(), textFinish, duration);
            toast.setGravity(Gravity.TOP|Gravity.CENTER, 0, 0);
            toast.show();

            getFragmentManager().popBackStack();

            return;
        }

        enableLayout();
    }

    @Override
    public void onResume() {
        super.onResume();

        staticInstance = this;
    }


    @Override
    public void onStop() {
        super.onStop();

        staticInstance = null;
    }

    private void navigateToRoom() {

        getActivity().runOnUiThread(() -> {

            Bundle bundle = new Bundle();
            bundle.putBoolean("master", true);

            PFMakeRoomFragment roomFragment = new PFMakeRoomFragment();
            roomFragment.setArguments(bundle);

            goToFragment(roomFragment);
        });
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

    //NDK

    public static native void initializeClient();
    public static native boolean connectToServer();
    public static native void makeServerRoom(boolean privateroom);

    //NDK Bridge

    @Keep
    public static void onRoomEventBridge() {

        if (staticInstance == null) {
            return;
        }

        staticInstance.navigateToRoom();
    }
}
