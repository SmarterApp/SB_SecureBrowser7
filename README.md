# Welcome to the Secure Browser 7 Project
The [SmarterApp](http://smarterapp.org) Secure Browser 7 project builds upon the Mozilla Firefox source code and creates a secure browser that is used to deliver student assessments. The Secure Browser implements security features such as not permitting multiple tabs, browsing arbitrary URLs and enforcing a white list of applications.

## License ##
This project is licensed under the [Mozilla open source license v.2.0](https://www.mozilla.org/MPL/2.0/).

## Getting Involved ##
We would be happy to receive feedback on its capabilities, problems, or future enhancements:

* For general questions or discussions, please use the [Forum](http://forum.smarterbalanced.org/viewforum.php?f=17).
* Feel free to **Fork** this project and develop your changes!

## Configuration ###

The Secure Browser can be built for three platforms: Windows, Mac OS X, and Linux (although the Linux version of 7.0 is not available; see [SB6.5](https://bitbucket.org/sbacoss/securebrowser65_release) for details). Each build follows a similar process, as described below. From a high-level perspective, building a custom secure browser involves:

* Cloning the Mozilla Firefox source
* Cloning this SecureBrowser project
* Applying a few customizations such as the target URL
* Running the automated build script

### Windows 

#### Prerequisites
Please follow the below procedure step by step and make sure to install the required software before you start the actual process in order to run the build successfully without any problem.

1. If you are on Windows 7, download and install [WINDOWS7 SDK]( http://www.microsoft.com/en-us/download/details.aspx?id=3138).
2. Download and install [Mozilla Build]( https://ftp.mozilla.org/pub/mozilla.org/mozilla/libraries/win32/) package version **1.11.0**. Mozilla Build package will have additional Build tools. Once the Mozilla Build package is installed, you will have a **Mozilla-Build** directory under your `C:\`  Drive.
3. Download and install [MakeMSI Version 12.206]( http://download.cnet.com/MAKEMSI/3000-2216_4-10468993.html).       
4. Download and Install [June 2010 DirectX]( http://www.microsoft.com/en-us/download/details.aspx?id=6812) Version **9.29.1962**.
5. Download [Windows Driver Kit Version 7.1.0]( https://www.microsoft.com/en-us/download/details.aspx?id=11800)  and extract ISO file under **C directory**. Follow the install instructions in **install.htm** file.
   * Note: While running key setup file, please make sure to check the required options.
6. Download and install [Visual studio C++ 2010 express edition](http://getintopc.com/softwares/development/visual-studio-express-2010-edition-free-download/).
   * Note: This build does not support Visual studio 2013 version.
7. Download and install [Microsoft Visual Studio 2010 service pack1](http://www.microsoft.com/en-us/download/details.aspx?id=23691). 
8. Download and install [Python 2.7.5 Version](https://www.python.org/download/releases/2.7.5/)                                     
   * For Windows 64 Bit version system, download Windows X86-64 MSI Installer (2.7.5) [1] (sig)
   * For Windows 32 Bit version system, download Windows x86 MSI Installer (2.7.5) (sig)
9. Delete all the files under `C:/mozilla-build/Python/`, and  copy all the files from `C:/Python27/` to `C:/mozilla-build/python/`


#### Build Process

All commands should be executed in the batch file named as **start-shell-msvc2010**, which is located under `C:/mozilla-build/`. Even though if you are using 64 Bit version, use **start-shell-msvc2010.bat** file.

1. Clone this securebrowser7 repository to your local workspace under home directory by running the following command, `hg clone https://bitbucket.org/sbacoss/securebrowser7_release `. 
2. Clone the Mozilla directory under `C:/Users/Administrator/securebrowser7_release/env/` by running the following command, `hg clone -v –-debug -r FIREFOX_29_0_1_RELEASE http://hg.mozilla.org/releases/mozilla-release/ Mozilla`. It will take around 25-35 minutes.
3. Generate one new v4 GUID from https://www.uuidgenerator.net/version4 and place it into `C:/Users/Administrator/securebrowser7_release/src/branding/SBACSecureBrowser/uuid.txt` (only for a single custom build, not for every version. Don't forget to verify that all letters in UUID must be capital)
4. Obtain the URL of the TDS Student home page for the browser. Encode the home page URL using [Base 64 Encode method]( https://www.base64decode.org/) and replace at `//SBAC PRODUCTION URL` in `C:\Users\Administrator\securebrowser7_release\src\branding\SBACSecureBrowser\pref\kiosk-branding.js`.
   * For Example: pref("bmakiosk.startup.homepage", "oiqoesqnkhHM6Ly9sb2dpbutgyutiwioiqduovdWQxLnRkcy5haXJhc3Qub3JnL3N0dWRlbnQvPiuoijxn0JBQw==").   Do not change anything in bmakiosk.startup.homepage.
5. Replace `start-shell-msvc2010.bat` file under `C:\mozilla-build` with `start-shell-msvc2010` file in SB7.0 repo. It has paths referenced to ATL files.
6. Create a file called **.profile** using a text editor of your choice in your home directory (Ex: C:/Users/Administrator/)
    Add the following path and alias variables to a `.profile` file.
    **PATH=/c/Program\ Files\ \(x86\)/MakeMsi/:/c/mozilla-build/mozmake:$PATH**
    **alias 'make=mozmake'**
    Source the file by running following command, `source .profile`
7. By default, Mozilla-build installs under "C" directory. Run the file`start-shell-msvc2010.bat` under Mozilla-build, this will launch an MSYS / BASH command prompt.
8. Go to the **env** directory located in cloned **securebrowser7 repository** and run `./automate.sh `. Within few seconds if you encounter any error, try running it again. This process will take more than 2 and half hours to build the browser.
9. Once build is successful run the following command under `env/opt-oaks-i686-pc-mingw32 /kiosk directory` to create a secure browser file called `mozmake msi`. 
10. After running Mozmake msi successfully, it will generate a executable msi file under `C:\Users\Administrator\securebrowser7_release\env\opt-oaks-i686-pc-mingw32\dist\msi\out\kiosk.mm\MSI\`.
    Install the `SBACSecureBrowser7.0-MSI.msi`. You’ll get secure browser 7.

### Mac OS X

1.	Download and install XCode that is compatible to your mac version from URL: [Apple](https://developer.apple.com/downloads/) (For this you need an Apple ID). It may take an hour or so.
2.	Download and install Command Line Tools as well from same URL: [Apple](https://developer.apple.com/downloads/).
3.	Move XCode to applications using command `sudo xcode-select -switch /Applications/Xcode.app`.
4.	Install all the build prerequisites for OS X by running following command in terminal: 
`curl https://hg.mozilla.org/mozilla-central/raw-file/default/python/mozboot/bin/bootstrap.py > bootstrap.py && python bootstrap.py`
(**Note:** Your account will need administrator permission to succeed; you can verify that you have this permission in System Preferences -> Users & Groups)
5.	During execution of above command it will ask you to choose the version of Firefox (Desktop or Android); choose Desktop. It will also ask you to choose a package manager (Homebrew or MACPorts); Choose Homebrew. 
(**Note:** After executing this command, if it asks you to create a file called `~/.bash_profile`, create it and add the string `export PATH=/usr/local/bin:$PATH` into it).
6.	Install required dependencies using command `brew install yasm mercurial gawk ccache python`.
7.	Install autoconf213 using command `brew install https://raw.github.com/Homebrew/homebrew-versions/master/autoconf213.rb`.	
8.	Get the Mozilla Source Code using following command: `hg clone https://hg.mozilla.org/mozilla-central/ mozilla-central`.
9.	Clone the Secure Browser 7 to a directory at the same level as mozilla-central: `hg clone https://bitbucket.org/sbacoss/securebrowser7_release`.
10.	Clone Mozilla Firefox 29 to a directory at same level as secure browser7: `hg clone -r FIREFOX_29_0_1_RELEASE http://hg.mozilla.org/releases/mozilla-release/ Mozilla`.
11.	Create a file called **.mozconfig** under  **mozilla-central**  directory and add following content to it:
(1) Define where build files should go. This places them in the directory `obj-ff-dbg` under the current source directory: `mk_add_options MOZ_OBJDIR=@TOPSRCDIR@/obj-ff-dbg`. (2) use `-s` makes builds quieter by default. (3) use `-j4` allows 4 tasks to run in parallel. Set the number equal to the number of cores in your machine, for example 4: `mk_add_options MOZ_MAKE_FLAGS="-s -j4`. (4) Enable debug builds: `ac_add_options --enable-debug`
12.	 Check the SDK version (Path:`Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs`) and add that version and its path to **.mozconfig** file.
`ac_add_options --with-macos-sdk=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk`.
13.	Also add autoconf213 installation path 
`mk_add_options AUTOCONF=/usr/local/Cellar/autoconf213/2.13/bin/autoconf213`.
14.	Update the Mac SDK version in the following file at line 97: `securebrowser7_release/env/ kiosk-client.mk`.
15.	Run the following command twice to ensure mercurial is up to date: `./mach mercurial-setup`.
16.	Generate a v4 GUID [UUID generator](https://www.uuidgenerator.net/version4) and place into `securebrowser7_release/src/branding/SBACSecureBrowser/uuid.txt` (**Note:** Only for a single custom build, not for every version. Don't forget to verify that all letters in UUID are capital).
17.	Obtain the URL of the TDS Student home page for the browser.
18.	Go into the `securebrowser7_release/env directory` and run `./build_securebrowser.sh` using this format

```
Usage: ./build_securebrowser.sh homepage-url [-b branding-info]
       homepage-url   The URL which the secure browser will open when started. If none is provided, a default test URL will be used (not recommended).
    -b branding-info  OPTIONAL: These parameters will be passed directly into the automate.sh script

EX: `./build_securebrowser.sh https://google.com`

```
This process may take around 2 hours. Once the build is successful, it will generate a setup file (.dmg) in `securebrowser7_release/env/Release/`. Go there and install **SBACSecurebrowser7.0.dmg**.

 **Please refer to [Mozilla Foundation](https://developer.mozilla.org/en-US/docs/Mozilla/Developer_guide/Build_Instructions/Mac_OS_X_Prerequisites) for additional information**.

### Linux 

*Not available for Secure Browser 7.0. Please refer to the [secure browser 6.5](https://bitbucket.org/sbacoss/securebrowser65_release) repository.*