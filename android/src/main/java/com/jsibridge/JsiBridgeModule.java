package com.jsibridge;

import android.util.Log;

import androidx.annotation.NonNull;

import com.facebook.react.bridge.Promise;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.module.annotations.ReactModule;

@ReactModule(name = JsiBridgeModule.NAME)
public class JsiBridgeModule extends ReactContextBaseJavaModule {
  public static final String NAME = "JsiBridge";

  public JsiBridgeModule(ReactApplicationContext reactContext) {
    super(reactContext);
  }

  @Override
  @NonNull
  public String getName() {
    return NAME;
  }


  // Example method
  // See https://reactnative.dev/docs/native-modules-android
  @ReactMethod(isBlockingSynchronousMethod = true)
  public void install() {
    try {
      System.loadLibrary("jsiBridge");
      JsiBridge.instance.install(getReactApplicationContext());
    } catch (Exception exception) {
      Log.e(NAME, "Failed to install JSI Bindings!", exception);
    }
  }
}
