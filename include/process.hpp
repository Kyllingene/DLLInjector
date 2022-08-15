
#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>

#include <vector>
#include <string>

#define UNICODE FALSE
#define MODNAME_SIZE 64

namespace Process {
    typedef std::vector<byte> BYTES;

#if !_WIN32 && !_WIN64
#error Can only run on Windows!
#else

#if _WIN64
    typedef long long unsigned int ADDR;
#else
    typedef long unsigned int ADDR;
#endif

#endif

    class ProcessException: public std::exception {
    public:
        // Create exception with message
        ProcessException(std::string message="Failed to open process") : msg(message) {};

        // Create formatted exception with PID
        ProcessException(DWORD pid);

        // Create formatted exception with name
        ProcessException(LPCSTR name, bool is_title);
        
        ~ProcessException() throw() {};
        const char* what() const throw() { return msg.c_str(); };

    private:
        std::string msg;
    };

    class Process {
    public:
        // Open process with handle
        Process(HANDLE handle);

        // Open process with PID
        Process(DWORD pid);

        // Open process with name
        Process(LPSTR name);

        // Close process handle
        ~Process();

        // Set base module name; default is process name
        void setBaseModule(LPSTR name);

        // Turn relative address into absolute
        LPVOID getAbsoluteAddr(LPVOID address);

        // Read bytes from process
        BYTES readMemory(LPVOID address, int size);

        // Write bytes to process
        void writeMemory(LPVOID address, BYTES* data);

        // Inject DLL into process
        DWORD injectDLL(LPSTR dll, int wait_time=1000);

        // Get underlying handle
        HANDLE* handle();

    private:
        void getModBaseAddr();

        HANDLE hproc;
        LPSTR mod_base = NULL;
        LPVOID mod_base_addr = NULL;
    };
};

#endif