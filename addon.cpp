#include <napi.h>
#include "annoyindexwrapper.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    return AnnoyIndexWrapper::Init(env, exports);
}

NODE_API_MODULE(addon, InitAll)

//void InitAll(v8::Local<v8::Object> exports) {
//  AnnoyIndexWrapper::Init(exports);
//}
//
//NODE_MODULE_INIT() {
//    InitAll(exports);
//}
