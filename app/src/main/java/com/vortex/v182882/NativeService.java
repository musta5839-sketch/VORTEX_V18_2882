
package com.vortex.v182882;
import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
public class NativeService extends Service {
 static { System.loadLibrary("native-lib"); }
 @Override public int onStartCommand(Intent i,int f,int id){
  nativeServiceTick();
  return START_STICKY;
 }
 private native void nativeServiceTick();
 @Override public IBinder onBind(Intent i){ return null; }
}
