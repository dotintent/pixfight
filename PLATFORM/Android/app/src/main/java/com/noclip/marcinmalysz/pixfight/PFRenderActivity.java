package com.noclip.marcinmalysz.pixfight;

import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.Color;
import android.graphics.Point;
import android.os.Environment;
import android.support.constraint.ConstraintLayout;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Display;
import android.view.GestureDetector;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.FrameLayout;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ListAdapter;
import android.widget.TextView;
import android.widget.Toast;

import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.io.File;
import java.util.Date;
import java.util.TimeZone;

public class PFRenderActivity extends AppCompatActivity {

    private PFGL2View glView = null;

    private ScaleGestureDetector scaleDetector;
    private GestureDetector tapDetector;
    private GestureDetector panDetector;

    private float scaleFactor = 1.f;

    private Button backButton = null;
    private Button endTurnButton = null;
    private Button multiplyButton = null;
    private ProgressDialog progressDialog = null;

    private static PFRenderActivity renderInstance = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getSupportActionBar().hide();
        setContentView(R.layout.activity_pfrender);

        glView = new PFGL2View(getApplication());

        Display display = getWindowManager().getDefaultDisplay();
        Point size = new Point();
        display.getRealSize(size);
        int widthPixels = size.x;
        int heightPixels = size.y;

        glView.getHolder().setFixedSize(widthPixels / 2, heightPixels / 2);

        tapDetector = new GestureDetector(this, new PFGestureTapListener());
        scaleDetector =  new ScaleGestureDetector(this, new PFScaleListener());
        panDetector = new GestureDetector(this, new PFPanGestureListener());

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        PFImmersiveMode.SetImmersiveMode(getWindow());

        backButton = findViewById(R.id.rendergame_back);
        endTurnButton = findViewById(R.id.rendergame_endtrun);
        multiplyButton = findViewById(R.id.rendergame_timemultiply);

        ConstraintLayout layout = findViewById(R.id.contraintLayout);

        layout.addView(glView, 0, new FrameLayout.LayoutParams(widthPixels, heightPixels));

        backButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View arg0) {

                buildMainMenu();
            }
        });

        endTurnButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View arg0) {

                if (canEndTurn() == false) {
                    return;
                }

                endTurn();
            }
        });

        multiplyButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View arg0) {

                String timeText = "X" + Integer.toString(multiplyTime());
                multiplyButton.setText(timeText);
            }
        });

        renderInstance = this;

        glView.callback = new PFSetupCallback() {

            @Override
            public void initalize(int width, int height) {

                initializeOpenGL(width, height);
            }
        };

        glView.setBundle(getIntent().getExtras());

        progressDialog = new ProgressDialog(PFRenderActivity.this);
        progressDialog.setMessage("Thinking...");
        progressDialog.setCancelable(false);
    }

    public void buildMainMenu() {

        final String[] items = {
                "QUIT",
                "SAVE"
        };

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("MENU");
        builder.setCancelable(true);

        builder.setItems(items, new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialogInterface, int i) {

                switch (i) {

                    case 0: {

                        finish();
                    }
                        break;

                    case 1: {

                        saveCurrentGame();
                    }
                        break;
                }
            }
        });

        builder.create();
        builder.show();
    }

    public void saveCurrentGame() {

        File dir = new File(Environment.getExternalStorageDirectory() + "/PIXFIGHTDATA", "save");

        if (!dir.exists()) {
            dir.mkdir();
        }

        if (!dir.exists()) {

            CharSequence text = "Error save directory does not exist";

            Toast toast = Toast.makeText(getApplicationContext(), text, Toast.LENGTH_SHORT);
            toast.setGravity(Gravity.TOP|Gravity.CENTER, 0, 0);
            toast.show();

            return;
        }

        Date currentTime = Calendar.getInstance().getTime();

        SimpleDateFormat sdf = new SimpleDateFormat("MM_dd_yyyy_HH:mm:ss");
        sdf.setTimeZone(TimeZone.getTimeZone("GMT"));

        String dateString = sdf.format(currentTime);
        String mapString = getMapName();

        String savePath = dir.toString() + "/" + mapString + "_" + dateString + ".sav";

        CharSequence text = null;

        if (saveGame(savePath)) {

            text = "Game saved successfully!";
        }
        else {

            text = "Error saving game.";
        }

        Toast saveToast = Toast.makeText(getApplicationContext(), text, Toast.LENGTH_SHORT);
        saveToast.setGravity(Gravity.TOP|Gravity.CENTER, 0, 0);
        saveToast.show();
    }

    public void onWinEvent() {

        Log.d("[PIXFIGHT]", "onWinEvent");

        this.runOnUiThread(new Runnable() {

            @Override
            public void run() {

                CharSequence text = "Congratulations, You win!";
                int duration = Toast.LENGTH_SHORT;
                Toast toast = Toast.makeText(getApplicationContext(), text, duration);
                toast.setGravity(Gravity.CENTER, 0, 0);
                toast.show();
            }
        });
    }

    public void onLoseEvent() {

        Log.d("[PIXFIGHT]", "onLoseEvent");

        this.runOnUiThread(new Runnable() {

            @Override
            public void run() {

                CharSequence text = "Sorry, You lose!";
                int duration = Toast.LENGTH_SHORT;
                Toast toast = Toast.makeText(getApplicationContext(), text, duration);
                toast.setGravity(Gravity.CENTER, 0, 0);
                toast.show();
            }
        });
    }

    public void botsStartThinkEvent() {

        Log.d("[PIXFIGHT]", "botsStartThinkEvent");

        this.runOnUiThread(new Runnable() {

            @Override
            public void run() {

                progressDialog.show();
            }
        });
    }

    public void botsEndThinkEvent() {

        Log.d("[PIXFIGHT]", "botsEndThinkEvent");

        this.runOnUiThread(new Runnable() {

            @Override
            public void run() {

                progressDialog.hide();
                PFImmersiveMode.SetImmersiveMode(getWindow());
            }
        });
    }

    private void buildDialog(final int teamID, final int cash) {

        final int[] costs = {
                75,
                150,
                200,
                300,
                200
        };

        final String[] types = {
                "Infantry - 75",
                "Bazooka - 150",
                "Jeep - 200",
                "Tank - 300",
                "Artillery - 200"
        };

        final int[] icons = {
                R.drawable.icon_infantry1,
                R.drawable.icon_bazooka1,
                R.drawable.icon_jeep1,
                R.drawable.icon_tank1,
                R.drawable.icon_artillery1,

                R.drawable.icon_infantry2,
                R.drawable.icon_bazooka2,
                R.drawable.icon_jeep2,
                R.drawable.icon_tank2,
                R.drawable.icon_artillery2,

                R.drawable.icon_infantry3,
                R.drawable.icon_bazooka3,
                R.drawable.icon_jeep3,
                R.drawable.icon_tank3,
                R.drawable.icon_artillery3,

                R.drawable.icon_infantry4,
                R.drawable.icon_bazooka4,
                R.drawable.icon_jeep4,
                R.drawable.icon_tank4,
                R.drawable.icon_artillery4
        };

        ListAdapter adapter = new ArrayAdapter<String>(getApplicationContext(), R.drawable.list_item, types) {

            ViewHolder holder;

            class ViewHolder {
                ImageView icon;
                TextView title;
            }

            public boolean isEnabled(int position) {
                return !(costs[position] > cash);
            }

            public View getView(int position, View convertView, ViewGroup parent) {

                final LayoutInflater inflater = (LayoutInflater) getApplicationContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);

                if (convertView == null) {

                    convertView = inflater.inflate(R.drawable.list_item, null);

                    holder = new ViewHolder();

                    holder.icon = (ImageView)convertView.findViewById(R.id.icon);
                    holder.title = (TextView)convertView.findViewById(R.id.title);

                    convertView.setTag(holder);

                } else {

                    // view already defined, retrieve view holder
                    holder = (ViewHolder) convertView.getTag();
                }

                boolean disable = costs[position] > cash;

                holder.title.setTextColor(disable ? Color.RED : Color.BLACK);
                holder.title.setText(types[position]);
                holder.icon.setImageResource(icons[5 * (teamID-1) + position]);

                return convertView;
            }
        };

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Build Unit - Cash: " + Integer.toString(cash));
        builder.setCancelable(true);
        builder.setAdapter(adapter, new DialogInterface.OnClickListener() {

            public void onClick(DialogInterface dialog, int which) {

                if (costs[which] > cash) {
                    return;
                }

                int localCash = cash;
                int selectedUnit = which;

                localCash -= costs[which];

                finishBuilding(selectedUnit, localCash);
                dialog.dismiss();
            }
        });

        builder.create();
        builder.show();
    }

    public void onBaseSelected(final int team, final int cash) {

        this.runOnUiThread(new Runnable() {

            @Override
            public void run() {

               buildDialog(team, cash);
            }
        });
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {

        tapDetector.onTouchEvent(event);
        scaleDetector.onTouchEvent(event);
        panDetector.onTouchEvent(event);

        return true;
    }

    @Override
    protected void onPostResume() {
        super.onPostResume();

        PFImmersiveMode.SetImmersiveMode(getWindow());
    }

    @Override
    protected void onStart() {
        super.onStart();

        PFGameLib.nativeOnStart(this);
        PFAudioWrapper.playGameMusic();
    }

    @Override
    protected void onStop() {
        super.onStop();

        progressDialog.cancel();

        renderInstance = null;

        PFGameLib.nativeOnStop();
        PFAudioWrapper.playMenuMusic();
    }

    //Private classes
    private class PFGestureTapListener
            extends GestureDetector.SimpleOnGestureListener {

        @Override
        public boolean onSingleTapConfirmed(MotionEvent e) {

            tapAction(e.getX() / 2.0f, e.getY() / 2.0f);

            return true;
        }
    }

    private class PFScaleListener
            extends ScaleGestureDetector.SimpleOnScaleGestureListener {
        @Override
        public boolean onScale(ScaleGestureDetector detector) {

            scaleFactor *= detector.getScaleFactor();
            scaleFactor = Math.max(0.75f, Math.min(scaleFactor, 2.0f));

            scaleAction(scaleFactor);

            return true;
        }
    }

    private class PFPanGestureListener
            extends GestureDetector.SimpleOnGestureListener {

        @Override
        public boolean onScroll(MotionEvent e1,
                                MotionEvent e2,
                                float distanceX,
                                float distanceY) {

            panAction(distanceX * 50.0f, distanceY * 50.0f);

            return true;
        }
    }

    //NDK to Java Bridge

    public static void onWinEventBridge() {

        if (renderInstance == null) {
            return;
        }

        renderInstance.onWinEvent();
    }

    public static void onLoseEventBridge() {

        if (renderInstance == null) {
            return;
        }

        renderInstance.onLoseEvent();
    }

    public static void botsStartThinkEventBridge() {

        if (renderInstance == null) {
            return;
        }

        renderInstance.botsStartThinkEvent();
    }

    public static void botsEndThinkEventBridge() {

        if (renderInstance == null) {
            return;
        }

        renderInstance.botsEndThinkEvent();
    }

    public static void onBaseSelectedBridge(int team, int cash) {

        if (renderInstance == null) {
            return;
        }

        renderInstance.onBaseSelected(team, cash);
    }

    //NDK
    public static native void initializeOpenGL(int width, int height);
    public static native boolean canEndTurn();
    public static native void endTurn();
    public static native int multiplyTime();
    public static native void finishBuilding(int unitid, int cash);
    public static native void tapAction(float x, float y);
    public static native void scaleAction(float scale);
    public static native void panAction(float dx, float dy);
    public static native boolean saveGame(String savepath);
    public static native String getMapName();
}
