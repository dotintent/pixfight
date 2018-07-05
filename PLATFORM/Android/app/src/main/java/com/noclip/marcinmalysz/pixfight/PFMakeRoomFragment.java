package com.noclip.marcinmalysz.pixfight;

import android.os.Bundle;
import android.support.annotation.Keep;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentTransaction;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AlertDialog;
import android.util.SparseArray;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import java.lang.reflect.Field;
import java.util.HashMap;
import java.util.Map;

public class PFMakeRoomFragment extends Fragment {

    private boolean isMaster = false;
    private Button readyButton = null;
    private Button mapButton = null;
    private TextView infoLabel = null;
    private ImageView mapImage = null;
    private SparseArray<Map<String, String>> data = new SparseArray<>();

    private static PFMakeRoomFragment staticInstance = null;

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_pfmake_room, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        isMaster = getArguments().getBoolean("master");

        //data
        setupData();

        //back
        Button backButton = getView().findViewById(R.id.makebutton_back);
        backButton.setOnClickListener(arg0 -> {

            if (isMaster) {

                removeRoom();
            }
            else {

                leaveRoom();
            }

            getFragmentManager().popBackStack();
        });

        mapButton = getView().findViewById(R.id.choosemap);
        mapButton.setEnabled(isMaster);
        mapButton.setOnClickListener(arg0 -> {

            selectMapMenu();
        });

        readyButton = getView().findViewById(R.id.readybutton);
        readyButton.setOnClickListener(arg0 -> {

            readyButton.setEnabled(false);
            setReady();
        });

        mapImage = getView().findViewById(R.id.imageView);

        infoLabel = getView().findViewById(R.id.roomTextView);
        infoLabel.setText("Room ID: " + Integer.toString(getRoomPort()));

        if (isMaster) {

            updateGameInfoForMap(0);
        }
        else {

            getRoomDetails();
        }

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

    private void setupData() {

        data.put(0 , new HashMap<String, String>(){{

            put("name", "TUTORIAL");
            put("image", "tutorial");
            put("mapname", "tutorial");
            put("players", "2");
        }});

        data.put(1 , new HashMap<String, String>(){{

            put("name", "HUNT");
            put("image", "hunt");
            put("mapname", "Hunt");
            put("players", "3");
        }});

        data.put(2 , new HashMap<String, String>(){{

            put("name", "PASAGE");
            put("image", "pasage");
            put("mapname", "pasage");
            put("players", "2");
        }});

        data.put(3 , new HashMap<String, String>(){{

            put("name", "WATERWAY");
            put("image", "waterway");
            put("mapname", "waterway");
            put("players", "2");
        }});

        data.put(4, new HashMap<String, String>(){{

            put("name", "KINGOFHILL");
            put("image", "kingofhill");
            put("mapname", "kingofhill");
            put("players", "4");
        }});

    }

    private void updateGameInfoForMap(int mapid) {

        Map<String, String> mapInfo = data.get(mapid);

        mapButton.setText(mapInfo.get("name"));

        String resourceString = mapInfo.get("image");
        mapImage.setImageDrawable(ContextCompat.getDrawable(getContext(), getId(resourceString, R.drawable.class)));

        updateRoomInfo(mapInfo.get("mapname"), Integer.parseInt(mapInfo.get("players")));

        readyButton.setEnabled(true);
    }

    private void selectMapMenu() {

        final String[] items = {
                "Tutorial",
                "Hunt",
                "Pasage",
                "Waterway",
                "KingOfHill"
        };

        AlertDialog.Builder builder = new AlertDialog.Builder(getContext());
        builder.setTitle("Maps");
        builder.setCancelable(true);

        builder.setItems(items, (dialogInterface, i) -> {

            updateGameInfoForMap(i);
        });

        builder.create();
        builder.show();
    }

    private static int getId(String resourceName, Class<?> c) {

        try {

            Field idField = c.getDeclaredField(resourceName);
            return idField.getInt(idField);

        } catch (Exception e) {

            throw new RuntimeException("No resource ID found for: "
                    + resourceName + " / " + c, e);
        }
    }

    private void startGame(int playerID, int players, String mapname) {

        getActivity().runOnUiThread(() -> {

            Bundle bundle = new Bundle();
            bundle.putInt("playerID", playerID);
            bundle.putInt("players", players);
            bundle.putString("mapname", mapname);

            PFRenderFragment renderFragment = new PFRenderFragment();
            renderFragment.setArguments(bundle);

            FragmentTransaction fragmentTransaction = getFragmentManager().beginTransaction();
            fragmentTransaction.replace(R.id.fragmentContainer, renderFragment);
            fragmentTransaction.addToBackStack(null);
            fragmentTransaction.commit();
        });
    }

    private void updateInteface(String mapname) {

        getActivity().runOnUiThread(() -> {

            for (int i = 0; i < data.size(); ++i) {

                Map<String, String> mapInfo = data.get(i);

                if (mapname.equals(mapInfo.get("mapname"))) {

                    mapButton.setText(mapInfo.get("name"));

                    String resourceString = mapInfo.get("image");
                    mapImage.setImageDrawable(ContextCompat.getDrawable(getContext(), getId(resourceString, R.drawable.class)));

                    break;
                }
            }

            readyButton.setEnabled(true);
        });
    }

    //BRIDGE

    @Keep
    public static void onLoadMapBridge(int playerID, int players, String mapname) {

        if (staticInstance == null) {
            return;
        }

        staticInstance.startGame(playerID, players, mapname);
    }

    @Keep
    public static void onUpdateMap(String mapname) {

        if (staticInstance == null) {
            return;
        }

        staticInstance.updateInteface(mapname);
    }

    //NDK

    public static native void removeRoom();
    public static native void leaveRoom();
    public static native void setReady();
    public static native void getRoomDetails();
    public static native int getRoomPort();
    public static native void updateRoomInfo(String mapname, int players);
}
