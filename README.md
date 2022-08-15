# DLLInjector
## Windows Only (sorry, Unix)!

### Usage

Call `inject.exe`. Make sure that `inject32.exe`, `inject64.exe`, and `inject.ini`, and any DLL's to inject are in the same folder.

The options you can configure in `inject.ini` are the following:
#### target
 - `name  = string`          | Name of target executable (ends with .exe)
 - `title = string`          | Title of target window (only used if `name` isn't supplied)
 - `arch  = ["x86" | "x64"]` | Default arch of target executable (defaults to "x86", only used if arch cannot be detected)

#### dll
 - `x86 = string` | DLL to inject into 32-bit processes
 - `x64 = string` | DLL to inject into 64-bit processes
    - If there is no ambiguity about target arch, only one DLL is necessary
    - ***MAKE SURE these DLL's are compiled correctly! Failure to do so will result in silent failure.***

#### config
 - `printdllpath = int` | Whether or not to print the full path of detected DLL in debug messages (defaults to 1)