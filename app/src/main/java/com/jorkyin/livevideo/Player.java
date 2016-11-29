package com.jorkyin.livevideo;

import android.graphics.Point;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.view.Display;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.RelativeLayout;

import com.jorkyin.livevideo.events.VideoEventListener;

/**
 * Created by YinJian on 2016/11/28.
 */

public class Player extends AppCompatActivity implements VideoEventListener, View.OnClickListener {
    // Used to load the 'native-lib' library on application startup.
    //http://www.jianshu.com/p/56de0e463ef4
    static {
        System.loadLibrary("native-lib");
    }

    private Handler mainHandler = new Handler();
    private SurfaceView surfaceView;
    private SurfaceHolder surfaceViewHolder;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_player);

        surfaceView = (SurfaceView) findViewById(R.id.surface);
        surfaceViewHolder = surfaceView.getHolder();
        surfaceViewHolder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                new Thread(new Player.Play()).start();
            }
            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {    }
            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {    }
        });

        findViewById(R.id.pause).setOnClickListener(this);
    }

    @Override
    protected void onPause() {
        super.onPause();
        //pause();
    }

    @Override
    public void onResolutionChange(final int width, final int height) {
        Display display = getWindowManager().getDefaultDisplay();
        final Point size = new Point();
        display.getSize(size);
        mainHandler.post(new Runnable(){
            @Override
            public void run() {
                // TODO Auto-generated method stub
                RelativeLayout.LayoutParams params = (RelativeLayout.LayoutParams) surfaceView.getLayoutParams();
                params.height = (size.x)*height/width;
                surfaceView.setLayoutParams(params);
            }
        });
    }

    /**
     * Called when a view has been clicked.
     *
     * @param v The view that was clicked.
     */
    @Override
    public void onClick(View v) {
       // pause();
    }

    class Play implements Runnable {
        @Override
        public void run() {
            //获取文件路径，这里将文件放置在手机根目录下(支持协议：本地、http、RTMP)
            String folderurl = Environment.getExternalStorageDirectory().getPath();
            //  String inputurl = "http://1251659802.vod2.myqcloud.com/vod1251659802/9031868222807497694/f0.mp4";
            //String inputurl = "http://192.168.0.208/tf/rec/20161129/rec001/rec011_20161129093541_02_0051_0030.avi";
            //String inputurl = "rtmp://123.108.164.71/etv2/phd1058";
            //String inputurl = "rtsp://c.itvitv.com/axn";
             String inputurl = folderurl+"/a.mp4";
            play(inputurl, surfaceViewHolder.getSurface());
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native void play(String url, Surface surface);
    //public native void pause();
}
