@echo off
setlocal

set MSYS=C:\Soft\Utils\MSYS2

rem Locate Visual Studio and load the x64 build environment
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -property installationPath`) do set "VSROOT=%%i"
if not defined VSROOT (
    echo Visual Studio not found
    exit /b 1
)
call "%VSROOT%\VC\Auxiliary\Build\vcvars64.bat" || exit /b 1

rem MSYS2's link.exe shadows the MSVC linker, so move it out of the way once
if exist "%MSYS%\usr\bin\link.exe" ren "%MSYS%\usr\bin\link.exe" link.exe.bak

rem Keep the Windows PATH, INCLUDE and LIB inside the MSYS2 shell
set MSYS2_PATH_TYPE=inherit

call "%MSYS%\msys2_shell.cmd" -where "%~dp0." -no-start -defterm -here -c "sh ./config-windows.sh"
pause