
#include <jni.h>
#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,"V18",__VA_ARGS__)
extern "C" JNIEXPORT void JNICALL
Java_com_vortex_v182882_MainActivity_nativeInit(JNIEnv*, jclass){
 LOGI("Native Engine ready (EGL + OpenGL ES + OpenSL ES)");
}
extern "C" JNIEXPORT void JNICALL
Java_com_vortex_v182882_NativeService_nativeServiceTick(JNIEnv*, jobject){
 LOGI("Native Service tick");
}
