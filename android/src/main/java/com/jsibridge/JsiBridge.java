package com.jsibridge;

import android.os.Handler;
import android.os.Looper;

import androidx.collection.ArrayMap;

import com.facebook.jni.HybridData;
import com.facebook.jni.annotations.DoNotStrip;
import com.facebook.react.bridge.JavaScriptContextHolder;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.turbomodule.core.CallInvokerHolderImpl;
import com.facebook.react.turbomodule.core.interfaces.CallInvokerHolder;

@SuppressWarnings("JavaJniMissingFunction")
public class JsiBridge {
  @DoNotStrip
  @SuppressWarnings("unused")
  private HybridData mHybridData;

  private final ArrayMap<String, JsiBridgeCallback> nativeListeners = new ArrayMap<>(10);
  private final Handler handler = new Handler(Looper.getMainLooper());

  public static final JsiBridge instance = new JsiBridge();

  public boolean install(ReactApplicationContext context) {
    try {
      JavaScriptContextHolder jsContext = context.getJavaScriptContextHolder();
      CallInvokerHolder jsCallInvokerHolder = context.getCatalystInstance().getJSCallInvokerHolder();
      mHybridData = initHybrid(jsContext.get(), (CallInvokerHolderImpl)jsCallInvokerHolder);
      installJSIBindings();
      return true;
    } catch (Exception exception) {
      return false;
    }
  }

  @DoNotStrip
  @SuppressWarnings("unused")
  private void emitNative(final String name, final String data) {
    JsiBridgeCallback jsiBridgeCallback = nativeListeners.get(name);
    if (jsiBridgeCallback == null) return;
    handler.post(new Runnable() {
      @Override
      public void run() {
        jsiBridgeCallback.onJsEvent(data);
      }
    });
  }

  public static void on(String name, JsiBridgeCallback callback) {
    instance.nativeListeners.put(name, callback);
  }

  public static void off(String name) {
    instance.nativeListeners.remove(name);
  }

  public static void emit(String name, String data) {
    instance.emitJs(name, data);
  }

  private native void installJSIBindings();
  private native void emitJs(String name, String data);
  private native HybridData initHybrid(long jsContext, CallInvokerHolderImpl jsCallInvokerHolder);


  public interface JsiBridgeCallback {
    void onJsEvent(String data);
  }
}
