#include <Windows.h>
#include <stdio.h>
#include <filesystem>

#include "include/process.hpp"

namespace fs = std::filesystem;

#define UNICODE FALSE
#define MAX_NAME 64

#if !_WIN32 && !_WIN64
#error Can only run on Windows!
#else

#if _WIN64
    #define DLL_ARCH "x64"
#else
    #define DLL_ARCH "x86"
#endif

#endif

// returns full path to DLL
std::string getDll() {
    fs::path out;

    char file[MAX_PATH];
    GetPrivateProfileString("dll", DLL_ARCH, "NOPATH", file, MAX_PATH, ".\\inject.ini");

    if (!strcmp(file, "NOPATH")) {
        puts("[-] ERROR: No DLL specified in inject.ini");
        exit(1);
    }

    out = fs::current_path();
    out.append(file);

    if (out.string().length() >= MAX_PATH) {
        printf("[-] ERROR: Path to DLL (%s) is too long", DLL_ARCH);
        exit(1);
    }

    int printPath = GetPrivateProfileInt("config", "printdllpath", 1, ".\\inject.ini");
    
    if (!fs::exists(out)) {
        puts("[-] ERROR: DLL not found (is it in the current directory?)");
        if (printPath)
            printf(" [|] (was looking for:) - %s\n", out.string().c_str());

        exit(1);
    }

    if (printPath)
        printf("[+] DLL found, path:\n [|] %s\n", out.string().c_str());
    else
        puts("[+] DLL found");

    return out.string();
}

void inject(Process::Process* proc, LPCSTR dll) {
    try {
        puts("[|] Injecting DLL...");
        proc->injectDLL((LPSTR)dll);
    } catch (Process::ProcessException e) {
        printf("[-] ERROR: Failed to inject DLL: %s\n", e.what());
        exit(1);
    }
}

int main() {
    std::string file = getDll();
    LPCSTR path = file.c_str();

    LPSTR name = (LPSTR)malloc(MAX_NAME);
    GetPrivateProfileString("target", "name", "NONAME", name, MAX_NAME, ".\\inject.ini");
    if (!strcmp(name, "NONAME")) {
        memset(name, 0, MAX_NAME);
        GetPrivateProfileString("target", "title", "NONAME", name, MAX_NAME, ".\\inject.ini");

        if (!strcmp(name, "NONAME")) {
            puts("[-] ERROR: No target name/title supplied");

            free(name);
            return 1;
        }

        printf("[|] Attempting to find window named '%s'\n", name);

        HWND hwnd = FindWindow(NULL, name);
        if (!hwnd) {
            printf("[-] ERROR: No window named '%s' found\n", name);

            free(name);
            return 1;
        }

        free(name);
        puts("[+] Found window");

        DWORD pid;
        GetWindowThreadProcessId(hwnd, &pid);
        if (!pid) {
            puts("[-] ERROR: Could not extract PID from process window");

            return 1;
        }

        puts("[+] Got PID");

        try {
            Process::Process proc(pid);
            inject(&proc, path);
        } catch (Process::ProcessException e) {
            printf("[-] ERROR: %s\n", e.what());

            return 1;
        }
    } else {
        printf("[|] Trying to find process named '%s'\n", name);
        try {
            Process::Process proc(name);
            free(name);
            inject(&proc, path);
        } catch (Process::ProcessException e) {
            printf("[-] ERROR: %s\n", e.what());

            free(name);
            return 1;
        }
    }

    puts("[+] Injection successful!");

    return 0;
}