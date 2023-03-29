
#ifdef RCT_NEW_ARCH_ENABLED
#import "RNJsiBridgeSpec.h"

@interface JsiBridge : NSObject <NativeJsiBridgeSpec>
#else
#import <React/RCTBridgeModule.h>

@interface JsiBridge : NSObject <RCTBridgeModule>
#endif

@end
