package com.jorkyin.livevideo;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

import com.jorkyin.livevideo.utils.RuntimePermissionManager;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    //http://www.jianshu.com/p/56de0e463ef4

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        RuntimePermissionManager r = new RuntimePermissionManager(this);
        r.requestPermissions();

        findViewById(R.id.start_play).setOnClickListener(this);
    }

    @Override
    public void onClick(View view) {

        Intent intent =new Intent();
        intent.setClass(MainActivity.this,Player.class);
        startActivity(intent);
    }
}
