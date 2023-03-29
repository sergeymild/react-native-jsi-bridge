#import <React/RCTBridgeModule.h>
#import <React/RCTBridge.h>

@interface JsiBridge : NSObject <RCTBridgeModule>

-(void)emitJs:(NSString *)name with:(NSString*)data;
@end
