#ifdef _WIN32

#include "../inject.hh"

#define UNICODE

#include <windows.h>
#include <tchar.h>
#include <psapi.h>
#include <codecvt>
#include <string>
#include <locale>
#include <iostream>

Napi::Array getRunningProcesses(const Napi::CallbackInfo &info)
{
    INIT_METHOD

    Napi::Array returnArray = Napi::Array::New(env);

    DWORD processes[1024];
    DWORD bytesNeeded;

    if (!EnumProcesses(processes, sizeof(processes), &bytesNeeded))
    {
        Napi::Error::New(env, "Could not enumerate processes").ThrowAsJavaScriptException();
        return returnArray;
    }

    DWORD processCount = bytesNeeded / sizeof(DWORD);

    for (unsigned int i = 0; i < processCount; i++)
    {
        if (processes[i])
        {
            Napi::Object process = Napi::Object::New(env);

            TCHAR processName[MAX_PATH] = TEXT("<unknown>");

            HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, processes[i]);

            if (processHandle)
            {
                HMODULE moduleHandle;
                DWORD bytesNeeded;

                if (EnumProcessModules(processHandle, &moduleHandle, sizeof(moduleHandle), &bytesNeeded))
                {
                    GetModuleBaseName(processHandle, moduleHandle, processName, sizeof(processName) / sizeof(TCHAR));
                }
            }

            std::wstring wideString(processName);

            process.Set("pid", processes[i]);
            process.Set("binary", Napi::String::New(env, std::string(wideString.begin(), wideString.end())));

            CloseHandle(processHandle);

            returnArray.Set(i, process);
        }
    }

    return returnArray;
}

Napi::Value getProcessByWindowClass(const Napi::CallbackInfo &info)
{
    INIT_METHOD

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::string narrowClassName = info[0].As<Napi::String>().Utf8Value();
    std::wstring wideClassName = converter.from_bytes(narrowClassName);

    const TCHAR *className = wideClassName.c_str();

    HWND windowHandle = FindWindowEx(NULL, NULL, className, NULL);

    if (!windowHandle)
    {
        windowHandle = FindWindow(className, NULL);
    }

    if (windowHandle)
    {
        DWORD processId;

        GetWindowThreadProcessId(windowHandle, &processId);

        Napi::Object process = Napi::Object::New(env);

        TCHAR processName[MAX_PATH] = TEXT("<unknown>");

        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, processId);

        if (processHandle)
        {
            HMODULE moduleHandle;
            DWORD bytesNeeded;

            if (EnumProcessModules(processHandle, &moduleHandle, sizeof(moduleHandle), &bytesNeeded))
            {
                GetModuleBaseName(processHandle, moduleHandle, processName, sizeof(processName) / sizeof(TCHAR));
            }
        }

        std::wstring wideString(processName);

        process.Set("pid", processId);
        process.Set("binary", Napi::String::New(env, std::string(wideString.begin(), wideString.end())));

        CloseHandle(processHandle);

        return process;
    }

    return env.Undefined();
}

Napi::Boolean injectSharedLibrary(const Napi::CallbackInfo &info)
{
    INIT_METHOD

    DWORD processId = info[0].As<Napi::Object>().Get("pid").As<Napi::Number>().Uint32Value();

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::string narrowDllPath = info[1].As<Napi::String>().Utf8Value();
    std::wstring wideDllPath = converter.from_bytes(narrowDllPath);

    unsigned int allocSize = wideDllPath.length() + 1;

    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, processId);

    if (processHandle)
    {
        void *dllPathLocation = VirtualAllocEx(processHandle, NULL, allocSize, MEM_COMMIT, PAGE_READWRITE);

        WriteProcessMemory(processHandle, dllPathLocation, (void *)wideDllPath.c_str(), allocSize, NULL);
        HANDLE loadThread = CreateRemoteThread(processHandle, NULL, NULL,
            (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"), dllPathLocation, NULL, NULL);

        DWORD exitCode;

        WaitForSingleObject(loadThread, INFINITE);
        GetExitCodeThread(loadThread, &exitCode);
        VirtualFreeEx(processHandle, dllPathLocation, allocSize, MEM_RELEASE);

        return Napi::Boolean::New(env, true);
    }

    return Napi::Boolean::New(env, false);
}

#endif