#include "inject.hh"

#define EXPORT_AS(func, name) exports.Set(##name, Napi::Function::New(env, ##func, #func));

Napi::Object init(Napi::Env env, Napi::Object exports) {
    EXPORT_AS(getRunningProcesses, "__getRunningProcesses")
    EXPORT_AS(getProcessByWindowClass, "__getProcessByWindowClass")
    EXPORT_AS(injectSharedLibrary, "__injectSharedLibrary")

    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, init)