package com.jorkyin.livevideo;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class MainActivity extends AppCompatActivity {
    // Used to load the 'native-lib' library on application startup.
    //http://www.jianshu.com/p/56de0e463ef4
    static {
        System.loadLibrary("native-lib");
    }

    private SurfaceView surfaceView;
    private SurfaceHolder surfaceViewHolder;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        surfaceView = (SurfaceView) findViewById(R.id.surface);
        surfaceViewHolder = surfaceView.getHolder();
        surfaceViewHolder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                new Thread(new Play()).start();
            }
            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {    }
            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {    }
        });
    }
    class Play implements Runnable {

        @Override
        public void run() {
            //获取文件路径，这里将文件放置在手机根目录下
            String folderurl = Environment.getExternalStorageDirectory().getPath();
            String inputurl = folderurl+"/a.mp4";
            play(inputurl, surfaceViewHolder.getSurface());
        }
    }
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native void play(String url, Surface surface);
}
