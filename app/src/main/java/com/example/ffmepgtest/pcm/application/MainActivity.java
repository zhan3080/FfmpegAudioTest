package com.example.ffmepgtest.pcm.application;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

public class MainActivity extends Activity {
    private static final String TAG = "MainActivity";
    public static String LIB_FFMPEG= "ffplayer";
    private Button startBtn;

    static {
        System.loadLibrary(LIB_FFMPEG);
    }

    public native void _pcmDecode();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        startBtn = findViewById(R.id.stn_id);
        startBtn.setOnClickListener(listener);
        checkPermission();
    }

    private View.OnClickListener listener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            switch (v.getId()){
                case R.id.stn_id:
                    _pcmDecode();
                    break;
            }
        }
    };


    private void checkPermission() {

        if (Build.VERSION.SDK_INT >= 23) {
            int write = checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE);
            int read = checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE);
            Log.i(TAG, "checkPermission write:" + write + ",read:" + read);
            if (write != PackageManager.PERMISSION_GRANTED || read != PackageManager.PERMISSION_GRANTED) {
                requestPermissions(new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_EXTERNAL_STORAGE}, 300);
            } else {
                Log.i(TAG, "hasPermission");
            }
        } else {
            Log.i(TAG, "------------- Build.VERSION.SDK_INT < 23 ------------");
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        if (requestCode == 300) {
            Log.i(TAG, "--------------requestCode == 300->" + requestCode + "," + permissions.length + "," + grantResults.length);
        } else {
            Log.i(TAG, "--------------requestCode != 300->" + requestCode + "," + permissions + "," + grantResults);
        }
    }
}