#ifndef INJECT_HH
#define INJECT_HH

#include <napi.h>

#define INIT_METHOD Napi::Env env = info.Env(); \
    Napi::HandleScope scope(env);

Napi::Array getRunningProcesses(const Napi::CallbackInfo &info);
Napi::Boolean injectSharedLibrary(const Napi::CallbackInfo &info);
Napi::Value getProcessByWindowClass(const Napi::CallbackInfo &info);

#endif