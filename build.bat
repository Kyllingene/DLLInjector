@echo off

echo Building main executable...
g++ -m32 -o inject.exe inject.cpp --std=c++17 -Llib -lprocess32 -lpsapi

echo Building 32-bit injector...
g++ -m32 -o inject32.exe injector.cpp --std=c++17 -Llib -lprocess32 -lpsapi

echo Building 64-bit injector...
g++ -o inject64.exe injector.cpp --std=c++17 -Llib -lprocess64 -lpsapi