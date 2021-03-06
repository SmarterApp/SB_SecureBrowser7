@echo off

SETLOCAL
SET MOZ_MSVCVERSION=10
SET MOZBUILDDIR=%~dp0
SET MOZILLABUILD=%MOZBUILDDIR%

echo "Mozilla tools directory: %MOZBUILDDIR%"

REM Get MSVC paths
call "%MOZBUILDDIR%guess-msvc.bat"

REM Use the "new" moztools-static
set MOZ_TOOLS=%MOZBUILDDIR%moztools

rem append moztools to PATH
SET PATH=%PATH%;%MOZ_TOOLS%\bin

if "%VC10DIR%"=="" (
    if "%VC10EXPRESSDIR%"=="" (
        ECHO "Microsoft Visual C++ version 10 (2010) was not found. Exiting."
        pause
        EXIT /B 1
    )

    if "%SDKDIR%"=="" (
        ECHO "Microsoft Platform SDK was not found. Exiting."
        pause
        EXIT /B 1
    )

    rem Prepend MSVC paths
    call "%VC10EXPRESSDIR%\Bin\vcvars32.bat" 2>nul
    if "%DevEnvDir%"=="" (
      rem Might be using a compiler that shipped with an SDK, so manually set paths
      SET "PATH=%VC10EXPRESSDIR%\Bin;%VC10EXPRESSDIR%\..\Common7\IDE;%PATH%"
      SET "INCLUDE=%VC10EXPRESSDIR%\Include;%VC10EXPRESSDIR%\Include\Sys;C:\WinDDK\7600.16385.1\inc\atl71\;%INCLUDE%"
      SET "LIB=%VC10EXPRESSDIR%\Lib;C:\WinDDK\7600.16385.1\lib\ATL\i386\%LIB%"
    )

    rem Don't set SDK paths in this block, because blocks are early-evaluated.

    rem Fix problem with VC++Express Edition
    if "%SDKVER%" GEQ "6" (
        rem SDK Ver.6.0 (Windows Vista SDK) and newer
        rem do not contain ATL header files.
        rem We need to use the Platform SDK's ATL header files.
        SET USEPSDKATL=1
    )
    rem SDK ver.6.0 does not contain OleAcc.idl
    rem We need to use the Platform SDK's OleAcc.idl
    if "%SDKVER%" == "6" (
        if "%SDKMINORVER%" == "0" (
          SET USEPSDKIDL=1
        )
    )
) else (
    if "%SDKVER%"=="8" (
      rem For VS 2010 with the 8.0 SDK, setup the VC 2010 env and then manually add sdk path info. below.
      call "%VC10DIR%\Bin\vcvars32.bat"
    ) else (
      rem Prepend MSVC paths
      rem The Win7 SDK (or newer) should automatically integrate itself into vcvars32.bat
      ECHO Using VC 2010 built-in SDK
      call "%VC10DIR%\Bin\vcvars32.bat"
    )
)

rem The call to VS 2010 vcvars32 adds 7.x SDK paths, so prepend the 8.0 kit to give it priority
if "%SDKVER%"=="8" (
    if "%SDKMINORVER%"=="1" (
        ECHO Using the Windows 8.1 Developer Kit
        set "PATH=%SDKDIR%bin\x86;%PATH%"
        set "LIB=%SDKDIR%Lib\winv6.3\um\x86;%LIB%"
        set "LIBPATH=%SDKDIR%Lib\winv6.3\um\x86;%LIBPATH%"
        set "INCLUDE=%SDKDIR%Include\shared;%SDKDIR%Include\um;%SDKDIR%Include\winrt;%SDKDIR%Include\winrt\wrl;%SDKDIR%Include\winrt\wrl\wrappers;%INCLUDE%"
        set "WINDOWSSDKDIR=%SDKDIR%"
    ) else (
        ECHO Using the Windows 8.0 Developer Kit
        set "PATH=%SDKDIR%bin\x86;%PATH%"
        set "LIB=%SDKDIR%Lib\win8\um\x86;%LIB%"
        set "LIBPATH=%SDKDIR%Lib\win8\um\x86;%LIBPATH%"
        set "INCLUDE=%SDKDIR%Include\shared;%SDKDIR%Include\um;%SDKDIR%Include\winrt;%SDKDIR%Include\winrt\wrl;%SDKDIR%Include\winrt\wrl\wrappers;%INCLUDE%"
        set "WINDOWSSDKDIR=%SDKDIR%"
    )
)

if "%VC10DIR%"=="" (
    rem Prepend SDK paths - Don't use the SDK SetEnv.cmd because it pulls in
    rem random VC paths which we don't want.
    rem Add the atlthunk compat library to the end of our LIB
    set "PATH=%SDKDIR%\bin;%PATH%"
    set "LIB=%SDKDIR%\lib;%LIB%;%MOZBUILDDIR%atlthunk_compat"

    if "%USEPSDKATL%"=="1" (
        if "%USEPSDKIDL%"=="1" (
            set "INCLUDE=%SDKDIR%\include;%PSDKDIR%\include\atl;%PSDKDIR%\include;%INCLUDE%"
        ) else (
            set "INCLUDE=%SDKDIR%\include;%PSDKDIR%\include\atl;%INCLUDE%"
        )
    ) else (
        if "%USEPSDKIDL%"=="1" (
            set "INCLUDE=%SDKDIR%\include;%SDKDIR%\include\atl;%PSDKDIR%\include;%INCLUDE%"
        ) else (
            set "INCLUDE=%SDKDIR%\include;%SDKDIR%\include\atl;%INCLUDE%"
        )
    )
)

cd "%USERPROFILE%"

"%MOZILLABUILD%\msys\bin\bash" --login -i
