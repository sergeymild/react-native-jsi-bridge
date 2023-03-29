import { NativeModules, Platform } from 'react-native';

const LINKING_ERROR =
  `The package 'react-native-jsi-bridge' doesn't seem to be linked. Make sure: \n\n` +
  Platform.select({ ios: "- You have run 'pod install'\n", default: '' }) +
  '- You rebuilt the app after installing the package\n' +
  '- You are not using Expo managed workflow\n';

const _JsiBridge = NativeModules.JsiBridge
  ? NativeModules.JsiBridge
  : new Proxy(
      {},
      {
        get() {
          throw new Error(LINKING_ERROR);
        },
      }
    );

_JsiBridge.install();

export class JsiBridge {
  static on(name: string, callback: (data: string) => void) {
    //@ts-ignore
    global._JsiBridge.registerCallback(name, callback);
  }

  static off(name: string) {
    //@ts-ignore
    global._JsiBridge.removeCallback(name);
  }

  static emit(name: string, data: string) {
    //@ts-ignore
    global._JsiBridge.emit(name, data);
  }
}
