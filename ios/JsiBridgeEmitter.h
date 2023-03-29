//
//  JsiBrigeEmitter.h
//  JsiBridge
//
//  Created by Sergei Golishnikov on 08/03/2022.
//  Copyright © 2022 Facebook. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "_JsiBridge.h"

typedef void (^JsiBridgeCallback)(NSString *data);

@interface JsiBridgeEmitter : NSObject

+ (JsiBridgeEmitter*)shared;

-(void)registerJsiBridge:(JsiBridge *)bridge;

-(void)on:(NSString *)name
     with:(JsiBridgeCallback)callback;

-(void)off:(NSString *)name;

-(void)emit:(NSString *)name
        with:(NSString *)data;

-(void)emitNative:(NSString *)name with:(NSString *) data;

@end
