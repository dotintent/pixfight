package com.noclip.marcinmalysz.pixfight;

import android.graphics.Color;
import android.graphics.Typeface;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentTransaction;
import android.util.SparseArray;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;

import java.lang.reflect.Field;
import java.util.HashMap;
import java.util.Map;


public class PFNewGameFragment extends Fragment implements OnItemSelectedListener {

    private Typeface font = null;
    private Spinner teamButton = null;
    private Button startButton = null;
    private ImageView mapImage = null;
    private TableLayout tableView = null;
    private SparseArray<Map<String, String>> data = new SparseArray<>();

    private int selectedPlayer = 1;
    private int playersPlaying = 2;
    private String mapName = "tutorial";

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_pfnew_game, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        font = Typeface.createFromAsset(getContext().getAssets(), "FFFATLAN.TTF");

        //back
        Button backButton = getView().findViewById(R.id.newgame_back);

        backButton.setOnClickListener(arg0 -> getFragmentManager().popBackStack());

        teamButton = getView().findViewById(R.id.teamspinner);
        teamButton.setOnItemSelectedListener(this);
        teamButton.setBackground(getResources().getDrawable(R.drawable.buttonn2x));
        createAdapter();

        startButton = getView().findViewById(R.id.startgamebutton);
        startButton.setTypeface(font);

        startButton.setOnClickListener(arg0 -> {
            if (mapName == null) {
                return;
            }

            Bundle bundle = new Bundle();
            bundle.putInt("playerID", selectedPlayer);
            bundle.putInt("players", playersPlaying);
            bundle.putString("mapname", mapName);

            PFRenderFragment renderFragment = new PFRenderFragment();
            renderFragment.setArguments(bundle);

            FragmentTransaction fragmentTransaction = getFragmentManager().beginTransaction();
            fragmentTransaction.replace(R.id.fragmentContainer, renderFragment);
            fragmentTransaction.addToBackStack(null);
            fragmentTransaction.commit();
        });

        mapImage = getView().findViewById(R.id.mapImage);
        tableView = getView().findViewById(R.id.mapPoolTable);
        createTablePool();
    }

    private void createAdapter() {

        String[] paths = null;

        switch (playersPlaying) {

            case 2 : {

                paths = new String[2];
                paths[0] = "BLUE";
                paths[1] = "RED";
            }
                break;
            case 3 : {

                paths = new String[3];
                paths[0] = "BLUE";
                paths[1] = "RED";
                paths[2] = "GREEN";
            }
                break;
            case 4 : {

                paths = new String[4];
                paths[0] = "BLUE";
                paths[1] = "RED";
                paths[2] = "GREEN";
                paths[3] = "YELLOW";
            }
                break;
        }

        ArrayAdapter<String>adapter = new ArrayAdapter<String>(getContext(),
                android.R.layout.simple_spinner_item, paths) {

            @NonNull
            public View getView(int position, View convertView, @NonNull ViewGroup parent) {

                View v = super.getView(position, convertView, parent);
                ((TextView) v).setTypeface(font);
                v.setTextAlignment(View.TEXT_ALIGNMENT_CENTER);

                return v;
            }

            public View getDropDownView(int position, View convertView, @NonNull ViewGroup parent) {

                View v = super.getDropDownView(position, convertView, parent);
                ((TextView) v).setTypeface(font);
                v.setTextAlignment(View.TEXT_ALIGNMENT_CENTER);

                v.setBackgroundColor(getResources().getColor(R.color.rowSelected));

                return v;
            }
        };

        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        teamButton.setAdapter(adapter);
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

    private void createTablePool() {

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

        TableRow tr;
        TextView tv;

        for (int i = 0; i < data.size(); ++i) {

            tr = new TableRow(getContext());
            tv = new TextView(getContext());

            Map<String, String> mapInfo = data.get(i);

            tv.setTypeface(font);
            tv.setTextSize(20);
            tv.setTextAlignment(View.TEXT_ALIGNMENT_CENTER);
            tv.setText(mapInfo.get("name"));
            tv.setTextColor(Color.BLACK);

            tr.setClickable(true);
            tr.setMinimumHeight(32);
            tr.setPadding(0, 15, 0, 15);
            tr.addView(tv);
            tr.setOnClickListener(tablerowOnClickListener);

            tableView.addView(tr);
        }

        View row = tableView.getChildAt(0);
        row.setBackgroundColor(getResources().getColor(R.color.rowSelected));
    }

    public void onItemSelected(@NonNull AdapterView<?> parent, @NonNull View v, int position, long id) {

        selectedPlayer = position + 1;
    }

    @Override
    public void onNothingSelected(@NonNull AdapterView<?> parent) {
        
    }

    private OnClickListener tablerowOnClickListener = v -> {

        for (int i = 0; i < tableView.getChildCount(); ++i) {

            View row = tableView.getChildAt(i);

            if (row.equals(v)) {

                row.setBackgroundColor(getResources().getColor(R.color.rowSelected));
            }
            else {

                row.setBackgroundColor(getResources().getColor(android.R.color.transparent));
            }
        }

        Integer index = tableView.indexOfChild(v);
        String resourceString = data.get(index).get("image");

        mapImage.setImageDrawable(getResources().getDrawable(getId(resourceString, R.drawable.class)));

        teamButton.setSelection(0);
        selectedPlayer = 1;
        playersPlaying = Integer.parseInt(data.get(index).get("players"));
        mapName = data.get(index).get("mapname");

        createAdapter();
    };
}
