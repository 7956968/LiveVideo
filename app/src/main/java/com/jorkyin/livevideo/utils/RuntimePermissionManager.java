package com.jorkyin.livevideo.utils;

/**
 * Created by YinJian on 2016/11/28.
 */
/**
 * 运行时权限管理类，针对安卓6.x版本
 * 对运行时的权限申请进行了分装
 *
 * 用法1:
 *
 * 在BaseActivity中声明 RuntimePermissionManager r 变量，
 *创建方法 public RuntimePermissionsManager getRuntimePermissionManager(Activity activity) {
 *if (runtimePermissionsManager==null) {
 *runtimePermissionsManager = new RuntimePermissionsManager(activity);
 *}
 *return runtimePermissionsManager;
 *}
 *继承方法 onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
 *@NonNull int[] grantResults)，在其中调用如下方法即可。
 *runtimePermissionsManager.handle(requestCode, permissions, grantResults);
 *然后在BaseActivity的子类中通过通过创建r实例和r.workWithPermission(String ,Callback),
 *具体的业务逻辑<b>需且只需</b>实现Callbakc接口既可。
 *
 * 方法2：
 *
 * 如果没有baseActivity父类，则需在当前Activity中声明RuntimePermissionManager r变量，
 * 并通过 r=new RuntimePermissionManager（thisActivity)
 * 创建实例，
 * 然后重写本类的onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
 *@NonNull int[] grantResults)，在其中调用如下方法即可。
 *runtimePermissionsManager.handle(requestCode, permissions, grantResults);
 * 其它步骤如上。
 *
 *
 *
 */

import android.Manifest;
import android.app.Activity;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;

import java.util.ArrayList;

public class RuntimePermissionManager {
    private Activity activity;
    private int requestCode=8888;
    private RequestCallback requestCallback;

    /**
     * 本程序中需要用到的权限
     */
    private final static String[] requestedPermissions={
            Manifest.permission.RECORD_AUDIO,
            Manifest.permission.MODIFY_AUDIO_SETTINGS,
            Manifest.permission.INTERNET,
            Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.WAKE_LOCK
    };

    public RuntimePermissionManager(Activity activity) {
        this.activity=activity;
    }

    /**
     * 检查所给的权限是否已被授权
     * @param permission 要检查的权限名字
     * @return true:已被授权；false:未被授权
     */
    public boolean checkPermission(String permission) {
        return  permission!=null&&(ContextCompat.checkSelfPermission(activity,permission)== PackageManager.PERMISSION_GRANTED);
    }

    /**
     * 请求授权,限内部调用
     * @param permissions 当为null时，表示遍历整个程序需要的权限，
     *                    当不为null时，表示请求传入的权限，此时permissions的有效length是1，其余大于1的权限请求将不会处理
     */
    private void requestPermissions(String[] permissions) {
        if (permissions==null) {
            permissions = requestedPermissions;
        }
        //过滤出目前还未被授权的权限
        String[] noGrantedPermissions;
        ArrayList<String> l=new ArrayList<String>();
        for (String s:permissions) {
            if (!checkPermission(s)) {
                l.add(s);
            }
        }
        if (l.size()==0) {
            return;
        }
        //一次性请求所有未被授权的权限
        noGrantedPermissions=(String[])l.toArray(new String[l.size()]);
        ActivityCompat.requestPermissions(activity,
                noGrantedPermissions,
                requestCode);
    }

    /**
     * 请求程序所有运行时需要申请的权限
     */
    public void requestPermissions() {
        requestPermissions(null);
    }

    /**
     * 请求指定的单个权限
     * @param permission 给定的权限
     */
    public void requestPermission(String permission) {
        if (!ActivityCompat.shouldShowRequestPermissionRationale(activity, permission)) {
            showDialog();
        } else {
            requestPermissions(new String[]{permission});
        }
    }

    /**
     * 处理权限申请的回调方法
     * @param requestCode
     * @param permissions
     * @param grantResults
     */
    public void handle(int requestCode,String[] permissions,int[] grantResults) {
        if (requestCallback==null) {
            return;
        }
        if (requestCode == this.requestCode) {
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                //Permission success
                requestCallback.requestSuccess();
            } else {
                // Permission Denied
                requestCallback.requestFailed();
            }

        } else {
//            Log.i("error","requestCode not match");
        }

    }

    /**
     * 检测权限，如未被授权则申请，并继续处理业务
     * @param permission 需要使用的权限
     * @param requestCallback 回调类，负责处理具体业务，包含权限已被授权和申请后的情况
     */
    public void workwithPermission(String permission,RequestCallback requestCallback) {
        if (permission==null||requestCallback==null) {
            return;
        }
        this.requestCallback=requestCallback;
        if (checkPermission(permission)) {
            requestCallback.requestSuccess();
        } else {
            requestPermission(permission);

        }

    }

    public void showDialog(){
        AlertDialog dialog = new AlertDialog.Builder(activity)
                .setMessage("该操作需要被赋予相应的权限，不开启将无法正常工作！")
                .setPositiveButton("开启", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                        Intent i=new Intent();
                        i.setAction("android.settings.APPLICATION_DETAILS_SETTINGS");
                        i.setData(Uri.fromParts("package", activity.getPackageName(), null));
                        activity.startActivity(i);
                    }
                })
                .setNegativeButton("取消", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                    }
                }).create();
        dialog.show();
    }

    interface RequestCallback {
        public void requestSuccess();
        public void requestFailed();
    }
}