package com.noclip.marcinmalysz.pixfight;

import android.content.Intent;
import android.graphics.Color;
import android.graphics.Typeface;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;

import java.util.HashMap;
import java.util.Map;
import java.lang.reflect.Field;


public class PFNewGameActivity extends AppCompatActivity implements OnItemSelectedListener {

    private Typeface font = null;
    private Spinner teamButton = null;
    private Button startButton = null;
    private ImageView mapImage = null;
    private TableLayout tableView = null;
    private Map<Integer, Map<String, String>> data = new HashMap<Integer, Map<String, String>>();

    private int selectedPlayer = 1;
    private int playersPlaying = 2;
    private String mapName = "tutorial";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        font = Typeface.createFromAsset(getAssets(), "FFFATLAN.TTF");

        getSupportActionBar().hide();

        setContentView(R.layout.activity_pfnew_game);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        PFImmersiveMode.SetImmersiveMode(getWindow());

        //back
        Button backButton = findViewById(R.id.newgame_back);

        backButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View arg0) {

                finish();
            }
        });

        teamButton = findViewById(R.id.teamspinner);
        teamButton.setOnItemSelectedListener(this);
        teamButton.setBackground(getResources().getDrawable(R.drawable.buttonn2x));
        createAdapter();

        startButton = findViewById(R.id.startgamebutton);
        startButton.setTypeface(font);

        startButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View arg0) {

                Intent intent = new Intent(PFNewGameActivity.this, PFRenderActivity.class);

                if (mapName == null) {
                    return;
                }

                intent.putExtra("playerID", selectedPlayer);
                intent.putExtra("players", playersPlaying);
                intent.putExtra("mapname", mapName);

                startActivity(intent);
            }
        });

        mapImage = findViewById(R.id.mapImage);
        tableView = findViewById(R.id.mapPoolTable);
        createTablePool();
    }

    @Override
    protected void onPostResume() {
        super.onPostResume();

        PFImmersiveMode.SetImmersiveMode(getWindow());
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

        ArrayAdapter<String>adapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_spinner_item, paths) {

            public View getView(int position, View convertView, ViewGroup parent) {

                View v = super.getView(position, convertView, parent);
                ((TextView) v).setTypeface(font);
                ((TextView) v).setTextAlignment(View.TEXT_ALIGNMENT_CENTER);

                return v;
            }

            public View getDropDownView(int position,  View convertView,  ViewGroup parent) {

                View v = super.getDropDownView(position, convertView, parent);
                ((TextView) v).setTypeface(font);
                ((TextView) v).setTextAlignment(View.TEXT_ALIGNMENT_CENTER);

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

        TableRow tr = null;
        TextView tv = null;

        Integer i;
        for (i = 0; i < data.size(); ++i) {

            tr = new TableRow(this);
            tv = new TextView(this);

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

    public void onItemSelected(AdapterView<?> parent, View v, int position, long id) {

        selectedPlayer = position + 1;
    }

    @Override
    public void onNothingSelected(AdapterView<?> parent) {
        
    }

    private OnClickListener tablerowOnClickListener = new OnClickListener() {

        public void onClick(View v) {

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
        }
    };
}
