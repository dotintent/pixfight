package com.noclip.marcinmalysz.pixfight;

import android.content.Intent;
import android.graphics.Color;
import android.graphics.Typeface;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;
import java.io.File;

public class PFLoadGameActivity extends AppCompatActivity {

    private Typeface font = null;
    private TableLayout tableView = null;
    private Button loadButton = null;
    private Button deleteButton = null;
    private File[] files = null;
    private int selectedFile = -1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        font = Typeface.createFromAsset(getAssets(), "FFFATLAN.TTF");
        setContentView(R.layout.activity_pfload_game);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        PFImmersiveMode.SetImmersiveMode(getWindow());

        tableView = findViewById(R.id.saveFilesTable);

        Button backButton = findViewById(R.id.loadgame_back);
        backButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View arg0) {

                finish();
            }
        });


        loadButton = findViewById(R.id.loadgamebuttonstart);
        loadButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View arg0) {

                loadGame();
            }
        });

        deleteButton = findViewById(R.id.loadgamebuttondelete);
        deleteButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View arg0) {

                deleteSave();
            }
        });

        loadButton.setTypeface(font);
        deleteButton.setTypeface(font);

        loadSaveFiles();
    }

    @Override
    protected void onPostResume() {
        super.onPostResume();

        PFImmersiveMode.SetImmersiveMode(getWindow());
    }

    private void loadGame() {

        if (selectedFile == -1) {
            return;
        }

        File dir = new File(Environment.getExternalStorageDirectory() + "/PIXFIGHTDATA", "save");
        File fileToLoad = files[selectedFile];
        File fullPath = new File(dir, fileToLoad.getName());

        if (!fullPath.exists()) {
            return;
        }

        Intent intent = new Intent(PFLoadGameActivity.this, PFRenderActivity.class);
        intent.putExtra("savepath", fullPath.toString());

        startActivity(intent);
    }

    private void deleteSave() {

        if (selectedFile == -1) {
            return;
        }

        File dir = new File(Environment.getExternalStorageDirectory() + "/PIXFIGHTDATA", "save");
        File fileToDelete = files[selectedFile];

        File fullPath = new File(dir, fileToDelete.getName());

        if (!fullPath.delete()) {
            return;
        }

        loadSaveFiles();
    }

    private void loadSaveFiles() {

        File dir = new File(Environment.getExternalStorageDirectory() + "/PIXFIGHTDATA", "save");

        if (!dir.exists()) {
            loadButton.setEnabled(false);
            deleteButton.setEnabled(false);
            return;
        }

        tableView.removeAllViews();
        files = dir.listFiles();

        TableRow tr = null;
        TextView tv = null;

        for (int i = 0; i < files.length; i++)  {

            tr = new TableRow(this);
            tv = new TextView(this);

            tv.setTextSize(18);
            tv.setTextAlignment(View.TEXT_ALIGNMENT_CENTER);
            tv.setText(files[i].getName());
            tv.setTextColor(Color.BLACK);

            tr.setClickable(true);
            tr.setMinimumHeight(32);
            tr.setPadding(0, 15, 0, 15);
            tr.addView(tv);
            tr.setOnClickListener(tablerowOnClickListener);

            tableView.addView(tr);
        }

        boolean isEmpty = files.length == 0;

        if (isEmpty) {
            loadButton.setEnabled(false);
            deleteButton.setEnabled(false);
            return;
        }

        selectedFile = 0;
        View row = tableView.getChildAt(0);
        row.setBackgroundColor(getResources().getColor(R.color.rowSelected));
    }

    private View.OnClickListener tablerowOnClickListener = new View.OnClickListener() {

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

            selectedFile = tableView.indexOfChild(v);
        }
    };
}
