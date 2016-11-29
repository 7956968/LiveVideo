#include <jni.h>
#include <string>
#include <thread>
#include <mutex>
#include <android/log.h>
#include "easyPlayer.h"
#include <opensles.h>

#include <android/native_window.h>
#include <android/native_window_jni.h>

#define LOGD(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "native-lib", format, ##__VA_ARGS__)

ANativeWindow* nativeWindow;
ANativeWindow_Buffer windowBuffer;
jmethodID gOnResolutionChange = NULL;
EasyPlayer easyPlayer;

void showPic() {
    easyPlayer.wait_state(PlayerState::READY);

    if (!easyPlayer.has_video()){ return;}

    if (0 > ANativeWindow_setBuffersGeometry(nativeWindow,easyPlayer.viddec.get_width(),easyPlayer.viddec.get_height(),WINDOW_FORMAT_RGBA_8888)){
        LOGD("Couldn't set buffers geometry.\n");
        ANativeWindow_release(nativeWindow);
        return;
    }

    //分配一个帧指针，指向解码后的原始帧
    AVFrame *frameRGBA = av_frame_alloc();
    //绑定输出buffer
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA,
                                            easyPlayer.viddec.get_width(),
                                            easyPlayer.viddec.get_height(),1);

    uint8_t *vOutBuffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

    av_image_fill_arrays(frameRGBA->data, frameRGBA->linesize,
                         vOutBuffer, AV_PIX_FMT_RGBA,
                         easyPlayer.viddec.get_width(),
                         easyPlayer.viddec.get_height(), 1);
    //读取帧
    while (easyPlayer.get_img_frame(frameRGBA)) {
        if (ANativeWindow_lock(nativeWindow, &windowBuffer, NULL) < 0) {
            LOGD("cannot lock window");
        } else {
            uint8_t *dst = (uint8_t *) windowBuffer.bits;
            for (int h = 0; h < easyPlayer.viddec.get_height(); h++)
            {
                memcpy(dst + h * windowBuffer.stride * 4,vOutBuffer + h * frameRGBA->linesize[0],frameRGBA->linesize[0]);
            }
            ANativeWindow_unlockAndPost(nativeWindow);
        }
    }

}

void playAudio() {
    easyPlayer.wait_state(PlayerState::READY);
    createAudioEngine();
    createBufferQueueAudioPlayer(easyPlayer.auddec.get_sample_rate(), easyPlayer.auddec.get_channels());
    audioStart();
}

void log(void* ptr, int level, const char* fmt,va_list vl) {
    switch (level) {
        case AV_LOG_VERBOSE:
            __android_log_vprint(ANDROID_LOG_DEBUG,  "native-lib", fmt, vl);
            break;
        case AV_LOG_INFO:
            __android_log_vprint(ANDROID_LOG_INFO,  "native-lib", fmt, vl);
            break;
        case AV_LOG_WARNING:
            __android_log_vprint(ANDROID_LOG_WARN,  "native-lib", fmt, vl);
            break;
        case AV_LOG_ERROR:
            __android_log_vprint(ANDROID_LOG_ERROR,  "native-lib", fmt, vl);
            break;
        case AV_LOG_FATAL:
        case AV_LOG_PANIC:
            __android_log_vprint(ANDROID_LOG_FATAL,  "native-lib", fmt, vl);
            break;
        case AV_LOG_QUIET:
            __android_log_vprint(ANDROID_LOG_SILENT,  "native-lib", fmt, vl);
            break;
        default:
            break;
    }
}


extern "C"
void
Java_com_jorkyin_livevideo_Player_play(JNIEnv* env,  jobject obj, jstring url, jobject surface) {
    char inputStr[500] = {0};
    //读取输入的视频频文件地址
    sprintf(inputStr, "%s", env->GetStringUTFChars(url, NULL));
    av_log_set_callback(log);
    //初始化
    easyPlayer.init(inputStr);
    init(&easyPlayer);

    //获取界面传下来的surface
    nativeWindow = ANativeWindow_fromSurface(env, surface);
    if (0 == nativeWindow){
        LOGD("Couldn't get native window from surface.\n");
        return;
    }

    std::thread videoThread(showPic);
    std::thread audioThread(playAudio);

    easyPlayer.wait_state(PlayerState::READY);

    if (easyPlayer.has_video()) {

        //通过回调接口设置窗口大小
        if (NULL == gOnResolutionChange){
            jclass clazz = env->GetObjectClass(obj);
            gOnResolutionChange = env->GetMethodID(clazz,"onResolutionChange","(II)V");
            if (NULL == gOnResolutionChange){
                LOGD("Couldn't find onResolutionChange method.\n");
                return;
            } else{
                env->CallVoidMethod(obj, gOnResolutionChange, easyPlayer.viddec.get_width(), easyPlayer.viddec.get_height());
            }
        }
    }

    audioThread.join();
    videoThread.join();
}