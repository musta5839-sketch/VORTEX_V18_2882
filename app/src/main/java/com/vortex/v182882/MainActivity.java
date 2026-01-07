
package com.vortex.v182882;
import android.app.Activity;
import android.os.Bundle;
public class MainActivity extends Activity {
 static { System.loadLibrary("native-lib"); }
 @Override protected void onCreate(Bundle b) {
  super.onCreate(b);
  nativeInit();
 }
 private static native void nativeInit();
}
