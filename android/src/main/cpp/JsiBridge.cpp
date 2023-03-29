//
// Created by Sergei Golishnikov on 08/03/2022.
//

#include "JsiBridge.h"

#include <utility>
#include "iostream"

using namespace facebook;
using namespace facebook::jni;

using TSelf = local_ref<HybridClass<JsiBridge>::jhybriddata>;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *) {
    return facebook::jni::initialize(vm, [] {
        JsiBridge::registerNatives();
    });
}

// JNI binding
void JsiBridge::registerNatives() {
    __android_log_print(ANDROID_LOG_VERBOSE, "😇", "registerNatives");
    registerHybrid({
                           makeNativeMethod("initHybrid",
                                            JsiBridge::initHybrid),
                           makeNativeMethod("installJSIBindings",
                                            JsiBridge::installJSIBindings),
                           makeNativeMethod("emitJs",
                                            JsiBridge::emitJs),
                   });
}

JsiBridge::JsiBridge(
        jni::alias_ref<JsiBridge::javaobject> jThis,
        jsi::Runtime *rt,
        std::shared_ptr<facebook::react::CallInvoker> jsCallInvoker)
        : javaPart_(jni::make_global(jThis)),
          runtime_(rt),
          jsCallInvoker_(std::move(jsCallInvoker)) {}

// JNI init
TSelf JsiBridge::initHybrid(
        alias_ref<jhybridobject> jThis,
        jlong jsContext,
        jni::alias_ref<facebook::react::CallInvokerHolder::javaobject>
        jsCallInvokerHolder) {

    __android_log_write(ANDROID_LOG_INFO, "🥲", "initHybrid...");
    auto jsCallInvoker = jsCallInvokerHolder->cthis()->getCallInvoker();
    return makeCxxInstance(jThis, (jsi::Runtime *) jsContext, jsCallInvoker);
}

void JsiBridge::emitJs(jstring name, jstring data) {
    __android_log_print(ANDROID_LOG_VERBOSE, "😇", "emit");

    auto stdName = jni::make_local(name)->toStdString();
    auto stdData = jni::make_local(data)->toStdString();

    if (jsListeners_.find(stdName) != jsListeners_.end()) {
        jsCallInvoker_->invokeAsync([=, d = stdData]() {
            jsListeners_[stdName]->call(*runtime_,jsi::String::createFromUtf8(*runtime_, d));
        });
    }
}

void JsiBridge::installJSIBindings() {
    __android_log_print(ANDROID_LOG_VERBOSE, "😇", "installJSIBindings");

    auto registerCallback = jsi::Function::createFromHostFunction(
            *runtime_,
            jsi::PropNameID::forUtf8(*runtime_, "registerCallback"),
            2,
            [=](jsi::Runtime &runtime,
                const jsi::Value &thisArg,
                const jsi::Value *args,
                size_t count) -> jsi::Value {


                auto name = args[0].asString(runtime).utf8(runtime);

                __android_log_print(ANDROID_LOG_VERBOSE, "😇", "registerCallback %s", name.c_str());

                auto callback = args[1].asObject(runtime).asFunction(runtime);
                jsListeners_[name] = std::make_shared<jsi::Function>(std::move(callback));
                return jsi::Value::undefined();
            });

    auto removeCallback = jsi::Function::createFromHostFunction(
            *runtime_,
            jsi::PropNameID::forUtf8(*runtime_, "removeCallback"),
            1,
            [=](jsi::Runtime &runtime,
                const jsi::Value &thisArg,
                const jsi::Value *args,
                size_t count) -> jsi::Value {

                auto name = args[0].asString(runtime).utf8(runtime);

                __android_log_print(ANDROID_LOG_VERBOSE, "😇", "removeCallback %s", name.c_str());

                jsListeners_.erase(name);
                return jsi::Value::undefined();
            });


    auto emit = jsi::Function::createFromHostFunction(
            *runtime_,
            jsi::PropNameID::forUtf8(*runtime_, "emit"),
            2,
            [=](jsi::Runtime &runtime,
                const jsi::Value &thisArg,
                const jsi::Value *args,
                size_t count) -> jsi::Value {

                auto name = args[0].asString(runtime).utf8(runtime);
                auto data = args[1].asString(runtime).utf8(runtime);
                auto localData = jni::make_jstring(data);
                auto localName = jni::make_jstring(name);

                __android_log_print(ANDROID_LOG_VERBOSE, "😇", "emit %s", name.c_str());

                auto method = javaPart_->getClass()->getMethod<void(jni::local_ref<jstring>, jni::local_ref<jstring>)>(
                        "emitNative");
                method(javaPart_.get(), localName, localData);
                return jsi::Value::undefined();
            });



    jsi::Object _jsiBridge = jsi::Object(*runtime_);
    _jsiBridge.setProperty(*runtime_, "registerCallback", std::move(registerCallback));
    _jsiBridge.setProperty(*runtime_, "removeCallback", std::move(removeCallback));
    _jsiBridge.setProperty(*runtime_, "emit", std::move(emit));
    runtime_->global().setProperty(*runtime_, "_JsiBridge", std::move(_jsiBridge));

}
