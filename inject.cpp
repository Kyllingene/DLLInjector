#include <Windows.h>
#include <stdio.h>
#include "include/process.hpp"

#define UNICODE FALSE
#define MAX_NAME 64

// simply detects arch of target, and spawns appropriate subprocess
int main() {
    LPSTR name = (LPSTR)malloc(MAX_NAME);

    GetPrivateProfileString("target", "name", "NONAME", name, MAX_NAME, ".\\inject.ini");

    HANDLE hproc;
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

        puts("[+] Found window");

        DWORD pid;
        GetWindowThreadProcessId(hwnd, &pid);
        if (!pid) {
            puts("[-] ERROR: Could not extract PID from process window");

            free(name);
            return 1;
        }

        puts("[+] Got PID");

        try {
            Process::Process proc(pid);
            hproc = *proc.handle();
        } catch (Process::ProcessException e) {
            printf("[-] ERROR: Failed to open process: %s\n", e.what());

            free(name);
            return 1;
        }
    } else {
        printf("[|] Trying to find process named '%s'\n", name);
        try {
            Process::Process proc(name);
            hproc = *proc.handle();
        } catch (Process::ProcessException e) {
            printf("[-] ERROR: Failed to open process: %s\n", e.what());

            free(name);
            return 1;
        }
    }

    printf("[+] Found '%s'\n", name);

    free(name);

    BOOL is_x86;
    if (!IsWow64Process(hproc, &is_x86)) {
        LPSTR arch = (LPSTR)malloc(4);
        GetPrivateProfileString("target", "arch", "NUL", arch, 4, ".\\inject.ini");

        if (!strcmp(arch, "x86")) {
            is_x86 = TRUE;
            puts("[|] WARN: Couldn't detect arch of process (using supplied default, x86)");
        } else if (!strcmp(arch, "x64")) {
            is_x86 = FALSE;
            puts("[|] WARN: Couldn't detect arch of process (using supplied default, x64)");
        } else {
            is_x86 = TRUE;
            puts("[|] WARN: Couldn't detect arch of process (no default supplied, assuming x86)");
        }
    }

    CloseHandle(hproc);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    if (is_x86) {
        puts("[|] Launching 32-bit injector");

        if (!CreateProcessA(".\\inject32.exe", NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi)) {
            puts("[-] ERROR: Failed to launch injector, `inject32.exe` (is it in the current directory?)");

            return 1;
        }
    } else {
        printf("[|] Launching 64-bit injector");

        if (!CreateProcessA(".\\inject64.exe", NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi)) {
            puts("[-] ERROR: Failed to launch injector, `inject64.exe` (is it in the current directory?)");

            return 1;
        }
    }

    puts("[+] Successfully launched injector");

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}