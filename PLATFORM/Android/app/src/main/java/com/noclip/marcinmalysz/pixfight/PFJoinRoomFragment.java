package com.noclip.marcinmalysz.pixfight;

import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.Keep;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentTransaction;
import android.support.v4.content.ContextCompat;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;
import android.widget.Toast;

public class PFJoinRoomFragment extends Fragment {

    private EditText portEditText = null;
    private TableLayout tableView = null;
    private Button joinButton = null;
    private Button refreshButton = null;

    private String[] roomStrings = null;
    private int[] roomPorts = null;

    private static PFJoinRoomFragment staticInstance = null;

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_pfjoin_room, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        //back
        Button backButton = getView().findViewById(R.id.joinbutton_back);
        backButton.setOnClickListener(arg0 -> getFragmentManager().popBackStack());

        joinButton = getView().findViewById(R.id.joinbutton);
        refreshButton = getView().findViewById(R.id.refreshbutton);
        tableView = getView().findViewById(R.id.roomsTableView);
        portEditText = getView().findViewById(R.id.editText);

        joinButton.setEnabled(false);

        joinButton.setOnClickListener(arg0 -> {

            int roomID = Integer.parseInt(portEditText.getText().toString());

            if (joinRoom(roomID)) {

                Bundle bundle = new Bundle();
                bundle.putBoolean("master", false);

                PFMakeRoomFragment roomFragment = new PFMakeRoomFragment();
                roomFragment.setArguments(bundle);

                FragmentTransaction fragmentTransaction = getFragmentManager().beginTransaction();
                fragmentTransaction.replace(R.id.fragmentContainer, roomFragment);
                fragmentTransaction.addToBackStack(null);
                fragmentTransaction.commit();

                return;
            }

            int duration = Toast.LENGTH_SHORT;
            CharSequence textFinish = "Could not connect to room.";

            Toast toast = Toast.makeText(getActivity().getApplicationContext(), textFinish, duration);
            toast.setGravity(Gravity.TOP|Gravity.CENTER, 0, 0);
            toast.show();

            getFragmentManager().popBackStack();
        });

        refreshButton.setOnClickListener(arg0 -> {

            refreshButton.setEnabled(false); //enable when data comes in

            listRooms();
        });

        portEditText.addTextChangedListener(new TextWatcher() {

            @Override
            public void afterTextChanged(Editable s) {}

            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {}

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {

                joinButton.setEnabled(portEditText.length() > 0);
            }
        });

        buildTableView(new String[]{}, new int[]{});

        listRooms();;
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

    void buildTableView(String[] rooms, int[] ports) {

        getActivity().runOnUiThread(() -> {

            roomStrings = rooms;
            roomPorts = ports;

            tableView.removeAllViews();

            if (roomStrings.length != roomPorts.length) {
                return;
            }

            TableRow tr;
            TextView tv;

            for (String room : roomStrings) {

                tr = new TableRow(getContext());
                tv = new TextView(getContext());

                tv.setTextSize(18);
                tv.setTextAlignment(View.TEXT_ALIGNMENT_CENTER);
                tv.setText(room);
                tv.setTextColor(Color.BLACK);

                tr.setClickable(true);
                tr.setMinimumHeight(32);
                tr.setPadding(0, 15, 0, 15);
                tr.addView(tv);
                tr.setOnClickListener(tablerowOnClickListener);

                tableView.addView(tr);
            }

            refreshButton.setEnabled(true);
        });
    }

    private View.OnClickListener tablerowOnClickListener = v -> {

        for (int i = 0; i < tableView.getChildCount(); ++i) {

            View row = tableView.getChildAt(i);

            if (row.equals(v)) {

                row.setBackgroundColor(ContextCompat.getColor(getContext(), R.color.rowSelected));
            }
            else {

                row.setBackgroundColor(ContextCompat.getColor(getContext(), android.R.color.transparent));
            }
        }

        int selectedRoom = tableView.indexOfChild(v);

        portEditText.setText(Integer.toString(roomPorts[selectedRoom]), TextView.BufferType.EDITABLE);
    };

    //BRIDGE

    @Keep
    public static void onRoomRefresh(String[] rooms, int[] ports) {

        if (staticInstance == null) {
            return;
        }

        staticInstance.buildTableView(rooms, ports);
    }

    //NDK

    public static native void listRooms();
    public static native boolean joinRoom(int roomID);
}
