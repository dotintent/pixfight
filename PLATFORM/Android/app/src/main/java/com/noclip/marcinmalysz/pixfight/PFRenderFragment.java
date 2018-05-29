package com.noclip.marcinmalysz.pixfight;

import android.annotation.SuppressLint;
import android.app.ProgressDialog;
import android.content.Context;
import android.graphics.Color;
import android.graphics.Point;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.constraint.ConstraintLayout;
import android.support.v4.app.Fragment;
import android.support.v7.app.AlertDialog;
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
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.ListAdapter;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.TimeZone;

public class PFRenderFragment extends Fragment {

    private PFGL2View glView = null;

    private ScaleGestureDetector scaleDetector;
    private GestureDetector tapDetector;
    private GestureDetector panDetector;

    private float scaleFactor = 1.f;

    private Button backButton = null;
    private Button endTurnButton = null;
    private Button multiplyButton = null;
    private Button undoButton = null;
    private ProgressDialog progressDialog = null;

    private static PFRenderFragment renderInstance = null;

    private Bundle arguments = null;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        arguments = getArguments();
    }

    @SuppressLint("ClickableViewAccessibility")
    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_pfrender, container, false);
        view.setOnTouchListener((v, event) -> {
            tapDetector.onTouchEvent(event);
            scaleDetector.onTouchEvent(event);
            panDetector.onTouchEvent(event);

            return true;
        });
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        glView = new PFGL2View(getActivity());
        glView.setPreserveEGLContextOnPause(true);

        Display display = getActivity().getWindowManager().getDefaultDisplay();
        Point size = new Point();
        display.getRealSize(size);
        int widthPixels = size.x;
        int heightPixels = size.y;

        glView.getHolder().setFixedSize(widthPixels / 2, heightPixels / 2);

        tapDetector = new GestureDetector(getContext(), new PFGestureTapListener());
        scaleDetector =  new ScaleGestureDetector(getContext(), new PFScaleListener());
        panDetector = new GestureDetector(getContext(), new PFPanGestureListener());

        backButton = getView().findViewById(R.id.rendergame_back);
        endTurnButton = getView().findViewById(R.id.rendergame_endtrun);
        multiplyButton = getView().findViewById(R.id.rendergame_timemultiply);
        undoButton = getView().findViewById(R.id.rendergame_undo);

        ConstraintLayout layout = getView().findViewById(R.id.contraintLayout);

        layout.addView(glView, 0, new FrameLayout.LayoutParams(widthPixels, heightPixels));

        backButton.setOnClickListener(arg0 -> buildMainMenu());

        endTurnButton.setOnClickListener(arg0 -> {

            if (!canEndTurn()) {
                return;
            }

            endTurn();
        });

        multiplyButton.setOnClickListener(arg0 -> {

            String timeText = "X" + Integer.toString(multiplyTime());
            multiplyButton.setText(timeText);
        });

        undoButton.setOnClickListener(arg0 -> {

            if (!canUndo()) {
                return;
            }

            undo();
        });

        renderInstance = this;

        glView.callback = PFRenderFragment::initializeOpenGL;

        glView.setBundle(arguments);

        progressDialog = new ProgressDialog(getContext());
        progressDialog.setMessage("Thinking...");
        progressDialog.setCancelable(false);
        progressDialog.getWindow().setGravity(Gravity.TOP);   
    }



    public void buildMainMenu() {

        final String[] items = {
                "QUIT",
                "SAVE"
        };

        AlertDialog.Builder builder = new AlertDialog.Builder(getContext());
        builder.setTitle("MENU");
        builder.setCancelable(true);

        builder.setItems(items, (dialogInterface, i) -> {

            switch (i) {

                case 0: {

                    getFragmentManager().popBackStack();
                }
                    break;

                case 1: {

                    saveCurrentGame();
                }
                    break;
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

            Toast toast = Toast.makeText(getContext(), text, Toast.LENGTH_SHORT);
            toast.setGravity(Gravity.TOP|Gravity.CENTER, 0, 0);
            toast.show();

            return;
        }

        Date currentTime = Calendar.getInstance().getTime();

        SimpleDateFormat sdf = new SimpleDateFormat("MM_dd_yyyy_HH_mm_ss");
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

        Toast saveToast = Toast.makeText(getContext(), text, Toast.LENGTH_SHORT);
        saveToast.setGravity(Gravity.TOP|Gravity.CENTER, 0, 0);
        saveToast.show();
    }

    public void onWinEvent() {

        Log.d("[PIXFIGHT]", "onWinEvent");

        getActivity().runOnUiThread(() -> {

            CharSequence text = "Congratulations, You win!";
            int duration = Toast.LENGTH_SHORT;
            Toast toast = Toast.makeText(getContext(), text, duration);
            toast.setGravity(Gravity.CENTER, 0, 0);
            toast.show();
        });
    }

    public void onLoseEvent() {

        Log.d("[PIXFIGHT]", "onLoseEvent");

        getActivity().runOnUiThread(() -> {

            CharSequence text = "Sorry, You lose!";
            int duration = Toast.LENGTH_SHORT;
            Toast toast = Toast.makeText(getContext(), text, duration);
            toast.setGravity(Gravity.CENTER, 0, 0);
            toast.show();
        });
    }

    public void botsStartThinkEvent() {

        Log.d("[PIXFIGHT]", "botsStartThinkEvent");

        getActivity().runOnUiThread(() -> progressDialog.show());
    }

    public void botsEndThinkEvent() {

        Log.d("[PIXFIGHT]", "botsEndThinkEvent");

        getActivity().runOnUiThread(() -> progressDialog.hide());
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

        ListAdapter adapter = new ArrayAdapter<String>(getContext(), R.layout.list_item, types) {

            ViewHolder holder;

            class ViewHolder {
                ImageView icon;
                TextView title;
            }

            public boolean isEnabled(int position) {
                return !(costs[position] > cash);
            }

            @NonNull
            public View getView(int position, View convertView, @NonNull ViewGroup parent) {

                final LayoutInflater inflater = (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);

                if (convertView == null) {

                    convertView = inflater.inflate(R.layout.list_item, null);

                    holder = new ViewHolder();

                    holder.icon = convertView.findViewById(R.id.icon);
                    holder.title = convertView.findViewById(R.id.title);

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

        AlertDialog.Builder builder = new AlertDialog.Builder(getContext());
        builder.setTitle("Build Unit - Cash: " + Integer.toString(cash));
        builder.setCancelable(true);
        builder.setAdapter(adapter, (dialog, which) -> {

            if (costs[which] > cash) {
                return;
            }

            int localCash = cash;

            localCash -= costs[which];

            finishBuilding(which, localCash);
            dialog.dismiss();
        });

        builder.create();
        builder.show();
    }

    public void onBaseSelected(final int team, final int cash) {

        getActivity().runOnUiThread(() -> buildDialog(team, cash));
    }

    @Override
    public void onStart() {
        super.onStart();

        PFGameLib.nativeOnStart(getActivity());
        PFAudioWrapper.playGameMusic();
    }

    @Override
    public void onStop() {
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
    public static native void undo();
    public static native boolean canUndo();
    public static native boolean canEndTurn();
    public static native void endTurn();
    public static native int multiplyTime();
    public static native void finishBuilding(int unitid, int cash);
    public static native void tapAction(float x, float y);
    public static native void scaleAction(float scale);
    public static native void panAction(float dx, float dy);
    public static native boolean saveGame(@NonNull String savepath);
    @NonNull public static native String getMapName();
}
