This page covers the steps needed to prepare your machine to build a development version of Firefox on Windows.
Adapted from [Mozdocs](https://developer.mozilla.org/en-US/docs/Mozilla/Developer_guide/Build_Instructions/Windows_Prerequisites) circa May 1, 2014, just after Firefox 29.0 release.

## Hardware requirements

Mozilla's software can take a long time to build, even on modern hardware. Minimal hardware required for Mozilla development is:

*   At least 3 GB of RAM for recent Firefox builds; 8 GB or more is recommended.
*   20 GB free disk space (for Visual Studio 2010 Express, the SDKs, the MozillaBuild package, the source code and free space needed to compile.)
*   Windows 7 (Service Pack 1) or later. Windows Vista or later is recommended, though it is possible to build on Windows XP. Windows 2000 and earlier are not supported.

## Overview

The Mozilla build process requires many tools that are not installed on most Windows systems. In addition to Visual Studio, you must install MozillaBuild, which is a bundle of software including the right versions of bash, GNU make, autoconf, Mercurial, and much more.

The Mozilla codebase works with Visual C++ 2010 (VC10), and Visual C++ 2012 (VC11). Official releases of Mozilla products are compiled with Visual C++ 2010 (VC10).

## Install build prerequisites

Complete each of these steps, otherwise you may not be able to build successfully. There are notes on these software requirements below.

*   Make sure your system is up-to-date through&nbsp;Windows Update. (Windows XP needs at least [Service Pack 2](http://www.microsoft.com/windowsxp/sp2/default.mspx) and [.NET Framework 2.0](http://www.microsoft.com/downloads/info.aspx?na=47&amp;p=3&amp;SrcDisplayLang=en&amp;SrcCategoryId=&amp;SrcFamilyId=4377f86d-c913-4b5c-b87e-ef72e5b4e065&amp;u=details.aspx?familyid=0856EACB-4362-4B0D-8EDD-AAB15C5E04F5&amp;displaylang=en)
*   If you are on Windows 7 and want to use the Express version of Visual Studio 2010, install the [Windows 7 SDK](https://developer.mozilla.org/en-US/docs/Windows_SDK_versions#Windows_7_SDK "Windows SDK versions")
*   If you are building with Visual C++ 2010 you will either need to install the [Windows 8 SDK](https://developer.mozilla.org/en-US/docs/Windows_SDK_versions#Windows_8_SDK_(Official)) or configure with `--disable-gamepad`.
*   Install the [June 2010 DirectX SDK](http://www.microsoft.com/download/en/details.aspx?displaylang=en&amp;id=6812 "http://www.microsoft.com/download/en/details.aspx?displaylang=en&amp;id=6812") (even VS 2012 requires this). (If you have previously installed Visual C++ 2010, you may encounter a "S1023" error during installation. For corrective steps, see [Known Issue: DirectX SDK (June 2010) Setup and the S1023 error](http://blogs.msdn.com/b/chuckw/archive/2011/12/09/known-issue-directx-sdk-june-2010-setup-and-the-s1023-error.aspx "http://blogs.msdn.com/b/chuckw/archive/2011/12/09/known-issue-directx-sdk-june-2010-setup-and-the-s1023-error.aspx") on the "Games for Windows and the DirectX SDK" blog.)
*   Install one of: [Visual Studio 2012 for Windows Desktop](http://www.microsoft.com/visualstudio/eng/downloads "http://www.microsoft.com/visualstudio/eng/downloads") Pro or Express (free), or Visual Studio 2010 Pro or [Express (free)](http://go.microsoft.com/?linkid=9709949 "http://go.microsoft.com/?linkid=9709949").

3.  Download and run the [MozillaBuild](https://ftp.mozilla.org/pub/mozilla.org/mozilla/libraries/win32/MozillaBuildSetup-Latest.exe "http://ftp.mozilla.org/pub/mozilla.org/mozilla/libraries/win32/MozillaBuildSetup-Latest.exe") package, containing additional build tools. If you have cygwin installed, read the note in the tips section. (After MozillaBuild's installer exits, if you see a Windows error dialog giving you the option to re-install with the 'correct settings', choose that option and after that all should be well.) More information about MozillaBuild and links to newer versions are available at [https://wiki.mozilla.org/MozillaBuild](https://wiki.mozilla.org/MozillaBuild "/en-US/docs/").
4.  If you intend to build the Firefox Windows 8 UI (formerly known as Metro), see the additional information located at [Windows 8 Integration - Building Locally](https://wiki.mozilla.org/index.php?title=Firefox/Windows_8_Integration#Building_Locally "Firefox/Windows_8_Integration#Building_Locally").

In some circumstances, the following problems can arise:

*   When building Thunderbird or SeaMonkey, you need to install the MAPI header files from [http://www.microsoft.com/en-us/download/details.aspx?id=12905](http://www.microsoft.com/en-us/download/details.aspx?id=12905) because the MAPI header files are not bundled with Visual Studio 2012 (Windows SDK 8.0). You should copy the header files to a Windows SDK include directory so that the build process will find the files, for example to `C:\Program Files (x86)\Windows Kits\8.0\Include\shared.`

Please note that **Mozilla will not build** if some of the tools are installed at a path that contains **spaces** or other breaking characters such as pluses, quotation marks, or metacharacters.  The Visual Studio tools and SDKs are an exception -- they may be installed in a directory which contains spaces. It is strongly recommended that you accept the default settings for all installation locations.

## Opening a build command prompt

After the prerequisites are installed, launch one of the following batch files from the directory to which you installed MozillaBuild (`c:\mozilla-build` by default):

*   `start-shell-msvc2010.bat (for Visual Studio 2010)`
*   `start-shell-msvc2012.bat (for Visual Studio 2012)`

**Even if you are on 64-bit Windows, do not use the `start-shell-msvcNNNN-x64.bat` files **(unless you know what you're doing). Those files are **experimental** and **unsupported**. See the [Build:MozillaBuild For x64 wiki page](http://wiki.mozilla-x86-64.com/How_To_Build_Windows_x64_Build "http://wiki.mozilla-x86-64.com/How_To_Build_Windows_x64_Build").

This will launch an MSYS / BASH command prompt properly configured to build one of the aforementioned code bases. All further commands should be executed in this command prompt window. (Note that this is not the same as what you get with the Windows CMD.EXE shell.)

## Create a directory for the source

**Note:** You won't be able to build the Firefox source code if it's under a directory with spaces in the path (e.g., don't use "Documents and Settings"). You can pick any other location, such as a new directory C:\mozilla-central.

It's a sensible idea to create a new directory dedicated solely to the code:

`cd /c; mkdir mozilla-source; cd mozilla-source`

Now you are ready to get the Firefox source and build; continue on to [Simple Firefox build (Get_the_source).](/en-US/docs/Simple_Firefox_build#Get_the_source "Simple_Firefox_build#Get_the_source")

## Command Prompt Tips and Caveats

*   To paste into this window, you must right-click on the window's title bar, move your cursor to the **Edit **menu, and click **Paste**. You can also set Quick Edit Mode in the Properties menu and use a right-click in the window to paste your selection.
*   If you have cygwin installed, make sure that the MozillaBuild directories come before any cygwin directories in the search path (use `echo $PATH` to see your search path).
*   In the MSYS / BASH shell, the c: drive looks like a directory called `c` under the root directory (/). So if you want to change to the directory `c:\mydir`, in the MSYS shell you would use `cd /c/mydir`.  Note the UNIX-style forward slashes (/) in the prompt instead of the Windows-style backward slashes (\).
*   The MSYS root directory is located in `c:\mozilla-build\msys` if you used the default installation directory. It's a good idea not to build anything under this directory. Instead use something like `/c/mydir`.

## Common problems, hints, and restrictions

*   [Debugging Mozilla on Windows FAQ](/en-US/docs/Debugging_Mozilla_on_Windows_FAQ "Debugging_Mozilla_on_Windows_FAQ"): Tips on how to debug Mozilla on Windows.
*   The build may fail if your `PATH` environment variable contains quotes ("). Quotes are not properly translated when passed down to MozillaBuild sub-shells. Quotes are usually not needed so they can be removed.
*   The build may fail if you have a `PYTHON` environment variable set. You will see an error almost immediately that says "`The system cannot find the file specified`". In a command shell, typing "`SET PYTHON=`" before running the Mozilla build tools in that same shell should fix this. Make sure that `PYTHON` is actually unset, rather than set to an empty value. In the command shell, "`SET PYTHON`" (without an equal sign or quotes) should respond: "`Environment variable PYTHON is not defined`".
*   The build may fail if you have cygwin installed. Make sure that the MozillaBuild directories (`c:\mozilla-build` and subdirectories) come before any cygwin directories in your PATH environment variable. If this does not help, remove the cygwin directories from PATH, or try building on a clean PC with no cygwin.
*   Building with versions of NSIS other than the version that comes with the latest supported version of MozillaBuild is not supported and will likely fail.
*   If you intend to distribute your build to others, you will need to set `WIN32_REDIST_DIR=$VCINSTALLDIR\redist\x86\Microsoft.VC80.CRT` in your mozconfig to get the Microsoft CRT DLLs packaged along with the application.
*   The [Microsoft Antimalware service](http://www.microsoft.com/security/malwareremove/default.aspx "http://www.microsoft.com/security/malwareremove/default.aspx") can interfere with compilation, often manifesting as an error related to `conftest.exe` during build. To remedy this, add at least your object directory to the [exclusion settings](http://social.answers.microsoft.com/Forums/en-US/msescan/thread/0867c974-cf19-460d-a75d-e35f283d4ad3 "http://social.answers.microsoft.com/Forums/en-US/msescan/thread/0867c974-cf19-460d-a75d-e35f283d4ad3").
*   If you encounter an error that atlbase.h cannot be found and you have installed Visual Studio 10 Express together with the platform SDK, you may have to delete a registry entry so that guess-msvc.bat doesn't detect VC10 installed. The key is HKLM\SOFTWARE\Microsoft\VisualStudio\10.0\Setup\VC.
*   If you encounter an error like "second C linkage of overloaded function '_interlockedbittestandset' not allowed", it happens when intrin.h and windows.h are included together. Use a #define to redefine one instance of the function's name. See more on [using intrin.h](/en-US/docs/Developer_Guide/Build_Instructions/Intrin.h "https://developer.mozilla.org/En/Developer_Guide/Build_Instructions/Intrin.h").
*   Parallel builds (`-jN`) do not work with GNU make on Windows. You should use the `mozmake` command included with current versions of MozillaBuild. Building with the `mach` command will always use the best available make command.


**Warning: **If you follow the below advice, your computer will **not **be protected against attacks that exploit this vulnerability. **Make sure you take adequate precautions.**

*   If you still get random crashes when running `make`, you may be encountering interference from Windows Security Updates that prevents proper operation of the Bash shell on some Windows systems. In this case, if you're on Windows XP, you will need to uninstall Windows XP Security Updates KB933729 and KB970238 using Add or Remove Programs from the Control Panel. The first of these Security Updates is also incorporated into Windows XP Service Pack 3 (KB936929), so if you have already installed SP3 you'll need to uninstall it and then make sure that Service Pack 2, including all Windows updates (EXCEPT KB933729, KB936929, and KB970238) get installed. To prevent these updates from being installed automatically, select "Notify me but don't automatically download or install them" in the Control Panel's "Automatic Updates" dialog. Then de-select them when any of these items appear in the list of recommended updates, and when asked, indicate that you don't want to be asked about these de-selected updates in the future.

### Microsoft DirectX SDK
The [June 2010 DirectX&nbsp;SDK](http://www.microsoft.com/en-us/download/details.aspx?id=6812 "http://www.microsoft.com/en-us/download/details.aspx?id=6812") (specifically) is required for building the [ANGLE](http://code.google.com/p/angleproject/ "http://code.google.com/p/angleproject/") GLES-on-D3D9 renderer, which is required on Windows for Web GL rendering. (If you don't care about compatibility with official Firefox builds and want to avoid installing the large SDK, you can disable WebGL by building Mozilla with the --disable-webgl option.) As part of the DirectX SDK install, you must install the End-User Redistributable Packages; don't uncheck it in the installer. (At least prior to Win7+VS2012?)

(Note that other versions of the DirectX SDK on Microsoft's Developer Center that claim to have been released after June 2010 are actually <u>older</u> versions that are misdated, some of which will result in an error if you try and download them due to the installer files themselves having been removed from the MS servers. There will not be any further standalone DirectX SDKs released by MS, since DirectX has been transitioned from an independent distribution model to an integrated (into Windows and Visual Studio) model, and the DirectX SDK is now integrated into the Windows 8 SDK (which is bundled with Visual Studio as of VS2012) [[ref](http://blogs.msdn.com/b/chuckw/archive/2012/03/22/where-is-the-directx-sdk.aspx "http://blogs.msdn.com/b/chuckw/archive/2012/03/22/where-is-the-directx-sdk.aspx")]. The reason that we require the June SDK even on newer Win7+VS2012 systems is because ANGLE depends on D3DX9, but MS considers D3DX to have been obsoleted by newer technologies and therefore VS2012 doesn't include the d3dx9.h and d3dx9tex.h headers that are required to compile ANGLE. Any queries about this paragraph, talk to [Jonathan Watt](/en-US/profiles/Jonathan_Watt "profiles/Jonathan_Watt").)

**Note**: The DirectX SDK installer may display an "S1023" error during the final installation step. You may be able to simply ignore this error and continue with the Mozilla build process. If the Mozilla build system still cannot locate the DirectX SDK, [see this Microsoft support page](http://support.microsoft.com/kb/2728613).


### MozillaBuild

The MozillaBuild package contains the other software prerequisites necessary for building Mozilla. This includes the MSYS build environment, [Mercurial](/en-US/docs/Mercurial "Mercurial"), autoconf-2.13, CVS, Python, YASM, NSIS, and UPX, as well as optional but useful tools such as wget and emacs.

[Download the current MozillaBuild package.](http://ftp.mozilla.org/pub/mozilla.org/mozilla/libraries/win32/MozillaBuildSetup-Latest.exe "http://ftp.mozilla.org/pub/mozilla.org/mozilla/libraries/win32/MozillaBuildSetup-Latest.exe")

By default, the package installs to `c:\mozilla-build`. It is recommended to use the default path. Don't use a path that contains spaces. The installer does not modify the Windows registry. Note that some binaries may require [Visual C++ Redistributable package](http://www.microsoft.com/downloads/en/details.aspx?FamilyID=a5c84275-3b97-4ab7-a40d-3802b2af5fc2&amp;displaylang=en "http://www.microsoft.com/downloads/en/details.aspx?FamilyID=a5c84275-3b97-4ab7-a40d-3802b2af5fc2&amp;displaylang=en") to run.

**Expectation setting:** Note that the "UNIX-like" environment provided by MozillaBuild is only really useful for building and committing to the Mozilla source. Most command line tools you would expect in a modern Linux distribution are not present, and those tools that are provided can be as much as a decade or so old (especially those provided by MSYS). It's the old tools in particular that can cause problems, since they often don't behave as expected, are buggy, or don't support command line arguments that have been taken for granted for years. For example, copying a source tree using `cp -rf src1 src2` does not work correctly because of an old version of cp (it gives "cp: will not create hard link" errors for some files). In short, MozillaBuild supports essential developer interactions with the Mozilla code, but beyond that don't be surprised if it trips you up in all sorts of exciting and unexpected ways.
