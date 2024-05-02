@echo off
setlocal
cd /D "%~dp0"
if not exist build mkdir build

for %%a in (%*) do set "%%a=1"

if not "%release%"=="1" set debug=1
if "%debug%"=="1" set release=0 && echo DEBUG
if "%release%"=="1" set debug=0 && echo RELEASE

if not exist build/lzham.o (
	echo Building lzham
	pushd build
	clang -g -DNDEBUG -D_LIB -DWIN32 -D_LARGEFILE64_SOURCE=1 -D_FILE_OFFSET_BITS=64 -O3 -fno-strict-aliasing -fomit-frame-pointer -I..\src\third-party\lzham_alpha\include -I..\src\third-party\lzham_alpha\lzhamdecomp ..\src\third-party\lzham_alpha\lzhamcomp\amalgamated.cpp -c -o lzham.o || exit /b 1
	REM who designs systems such as they can't be compiled in a single unit...
	clang -g -DNDEBUG -D_LIB -DWIN32 -D_LARGEFILE64_SOURCE=1 -D_FILE_OFFSET_BITS=64 -O3 -fno-strict-aliasing -fomit-frame-pointer -I..\src\third-party\lzham_alpha\include -I..\src\third-party\lzham_alpha\lzhamdecomp ..\src\third-party\lzham_alpha\lzhamdecomp\lzham_polar_codes.cpp -c -o lzham_polar.o || exit /b 1
	clang -g -DNDEBUG -D_LIB -DWIN32 -D_LARGEFILE64_SOURCE=1 -D_FILE_OFFSET_BITS=64 -O3 -fno-strict-aliasing -fomit-frame-pointer -I..\src\third-party\lzham_alpha\include -I..\src\third-party\lzham_alpha\lzhamdecomp ..\src\third-party\lzham_alpha\lzhamdecomp\lzham_huffman_codes.cpp -c -o lzham_huffman.o || exit /b 1
	popd
)

if "%release%" == "1" set copts=-O2 -DNDEBUG -flto -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -march=haswell
if "%debug%" == "1" set copts=-O0

echo Building main
pushd build
clang -fuse-ld=lld -g %copts% ..\src\main.c lzham.o lzham_polar.o lzham_huffman.o -o deltavpk.exe -lshell32
popd
