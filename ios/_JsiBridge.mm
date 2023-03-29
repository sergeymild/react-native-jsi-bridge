#import "_JsiBridge.h"
#import <React/RCTBridge+Private.h>
#import <ReactCommon/RCTTurboModule.h>
#import <jsi/jsi.h>
#import "JsiBridgeEmitter.h"

#include "iostream"
#include "map"

using namespace facebook;

namespace jsBridge {
    jsi::Value convertNSStringToJSIString(jsi::Runtime &runtime, NSString *value)
    {
      return jsi::String::createFromUtf8(runtime, [value UTF8String] ?: "");
    }
}

@implementation JsiBridge

RCT_EXPORT_MODULE()

std::map<std::string, std::shared_ptr<facebook::jsi::Function>> jsListeners_;
RCTBridge *jsBridge_bridge;
jsi::Runtime *jsBridge_runtime;

RCT_EXPORT_BLOCKING_SYNCHRONOUS_METHOD(install) {
    NSLog(@"Installing JsiBridge polyfill Bindings...");
    jsBridge_bridge = [RCTBridge currentBridge];
    auto jsBridge_cxxBridge = (RCTCxxBridge*)jsBridge_bridge;
    if (jsBridge_cxxBridge == nil) return @false;
    jsBridge_runtime = (jsi::Runtime*) jsBridge_cxxBridge.runtime;
    if (jsBridge_runtime == nil) return @false;
    auto& runtime = *jsBridge_runtime;
    
    [JsiBridgeEmitter.shared registerJsiBridge:self];
    
    
    auto registerCallback = jsi::Function::createFromHostFunction(runtime,
                                                                  jsi::PropNameID::forUtf8(runtime, "registerCallback"),
                                                                  2,
                                                                  [](jsi::Runtime& runtime,
                                                                     const jsi::Value& thisArg,
                                                                     const jsi::Value* args,
                                                                     size_t count) -> jsi::Value {
        
        auto name = args[0].asString(runtime).utf8(runtime);
        std::cout<<"😀addCallback " + name << std::endl;
        auto callback = args[1].asObject(runtime).asFunction(runtime);
        jsListeners_[name] = std::make_shared<jsi::Function>(std::move(callback));
        return jsi::Value::undefined();
    });
    
    auto removeCallback = jsi::Function::createFromHostFunction(runtime,
                                                                jsi::PropNameID::forUtf8(runtime, "removeCallback"),
                                                                1,
                                                                [](jsi::Runtime& runtime,
                                                                   const jsi::Value& thisArg,
                                                                   const jsi::Value* args,
                                                                   size_t count) -> jsi::Value {
        
        auto name = args[0].asString(runtime).utf8(runtime);
        std::cout<<"😀removecallback " + name << std::endl;
        jsListeners_.erase(name);
        return jsi::Value::undefined();
    });
    
    auto emit = jsi::Function::createFromHostFunction(runtime,
                                                      jsi::PropNameID::forUtf8(runtime, "emit"),
                                                      2,
                                                      [=](jsi::Runtime &runtime,
                                                          const jsi::Value &thisArg,
                                                          const jsi::Value *args,
                                                          size_t count) -> jsi::Value {
        
        auto name = args[0].asString(runtime).utf8(runtime);
        auto data = args[1].asString(runtime).utf8(runtime);
        
        auto nameString = [NSString stringWithUTF8String:name.c_str()];
        auto dataString = [NSString stringWithUTF8String:data.c_str()];
        
        [JsiBridgeEmitter.shared emitNative:nameString with:dataString];
        return jsi::Value::undefined();
    });
    
    
    jsi::Object _jsiBridge = jsi::Object(runtime);
    _jsiBridge.setProperty(runtime, "registerCallback", std::move(registerCallback));
    _jsiBridge.setProperty(runtime, "removeCallback", std::move(removeCallback));
    _jsiBridge.setProperty(runtime, "emit", std::move(emit));
    runtime.global().setProperty(runtime, "_JsiBridge", std::move(_jsiBridge));
    
    return @true;
}

- (void)emitJs:(NSString *)name with:(NSString *)data {
    
    auto stdName = [name UTF8String];
    if (jsListeners_.find(stdName) != jsListeners_.end()) {
            auto& runtime = *jsBridge_runtime;
            jsBridge_bridge.jsCallInvoker->invokeAsync([&runtime, n = stdName, d = data] () {
                auto dd = jsBridge::convertNSStringToJSIString(runtime, d);
                jsListeners_[n]->call(runtime, dd);
            });
        }
}

@end
