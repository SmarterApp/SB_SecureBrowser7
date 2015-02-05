# Welcome to the Secure Browser 7 Project
The [SmarterApp](http://smarterapp.org) Secure Browser 7 project builds upon the Mozilla Firefox source code and creates a secure browser that is used to deliver student assessments. The Secure Browser implements security features such as not permitting multiple tabs, browsing arbitrary URLs and enforcing a white list of applications.

## License ##
This project is licensed under the [Mozilla open source license v.2.0](https://www.mozilla.org/MPL/2.0/).

## Getting Involved ##
We would be happy to receive feedback on its capabilities, problems, or future enhancements:

* For general questions or discussions, please use the [Forum](http://forum.opentestsystem.org/viewforum.php?f=17).
* Use the **Issues** link to file bugs or enhancement requests.
* Feel free to **Fork** this project and develop your changes!

## Configuration ###

The Secure Browser can be built for three platforms: Windows, Mac OS X, and Linux. Each build follows a similar process, as described below. From a high-level perspective, building a custom secure browser involves:

* Cloning the Mozilla Firefox source
* Cloning this SecureBrowser project
* Applying a few customizations such as the target URL
* Running the automated build script

### Windows 

####Prerequisites
1. Install all prerequisites shown at `https://developer.mozilla.org/en-US/docs/Mozilla/Developer_guide/Build_Instructions/Windows_Prerequisites`
1. Windows SB7.0 is using firefox version 29.0.1 which is not supported by Visual Studio 2013. Instead install Visual Studio 2010 Express packages shown at shown `http://www.visualstudio.com/en-us/downloads/download-visual-studio-vs#DownloadFamilies_4` (in Visual Studio 2010 Express section)
1. In addition to above prerequisites firefox SB build also requires windows SDK package and visual studio Service Pack 1
1. Install makeMsi 12.206 from `http://download.cnet.com/MAKEMSI/3000-2216_4-10468993.html`,  from source code get MakeMsi.zip file, extract this zip file in C:\Program Files (will overwrite files in MakeMSI folder which has customization to SB build )
1. Download Windows Driver Kit Version 7.1.0 from `http://www.microsoft.com/en-us/download/details.aspx?id=11800` and extract zip file under c directory

####Build Process
1. Clone this securebrowser7 repository to your local workspace
1. Clone Mozilla Firefox 29 to a directory at same level as securebrowser7 `$ hg clone -r FIREFOX_29_0_1_RELEASE http://hg.mozilla.org/releases/mozilla-release/ mozilla`
1. Generate a v4 GUID (https://www.uuidgenerator.net/version4) and place into `/src/branding/SBACSecureBrowser/uuid.txt` (only for a single custom build, not for every version. Don't forget to verify that all letters in UUID should be capital)
1. This windows SB build requires four dll packages including msvcirt.dll, msvcp71.dll, msvcr71.dll and msvcrt.dll. These packages are freely available online, download and place them under 'env/mozilla/kiosk/redist/'
1. Obtain the URL of the TDS Student home page for the browser. encode home page url and replace at `// SBAC PRODUCTION URL` in `/src/branding/SBACSecureBrowser/pref/kiosk-branding.js` (Ex: pref("bmakiosk.startup.homepage", "oiqoesqnkhHM6Ly9sb2dpbutgyutiwioiqduovdWQxLnRkcy5haXJhc3Qub3JnL3N0dWRlbnQvPiuoijxn0JBQw==");)
1. Replace `start-shell-msvc2010.bat` file under `/c/mozilla-build` with `start-shell-msvc2010` file in SB7.0 repo. It has paths referenced to ATL files
1. Add following path and alias variables to .profile file and source the file ( `PATH=/c/Program\ Files/MakeMsi/:/c/mozilla-build/mozmake:$PATH`
    `alias 'make=mozmake'` )
1. By default mozilla-build installs under "C" directory. Run the file `start-shell-msvc2010.bat` under mozilla-build, this will launch an MSYS / BASH command prompt
1. Go into the env directory in cloned securebrowser7 repository and run `./automate.sh` using this format
1. Once build is successful run `mozmake msi` under `env/opt*/kiosk directory` to create file
### Mac OS X

1. Clone this securebrowser7 repository to your local workspace.
1. Clone Mozilla Firefox 29 to a directory at same level as securebrowser7
`$ hg clone -r FIREFOX_29_0_1_RELEASE http://hg.mozilla.org/releases/mozilla-release/ mozilla`
1. Install all prerequisites such as Xcode, shown at https://developer.mozilla.org/en-US/docs/Mozilla/Developer_guide/Build_Instructions/Mac_OS_X_Prerequisites 
1. Install homebrew via http://brew.sh or install MacPorts.
1. Generate a v4 GUID (https://www.uuidgenerator.net/version4) and place into `/src/branding/SBACSecureBrowser/uuid.txt` (only for a single custom build, not for every version)
1. Obtain the URL of the TDS Student home page for the browser.
1. Go into the `env` directory and run `./build_securebrowser.sh` using this format


```
Usage: ./build_securebrowser.sh homepage-url [-b branding-info]
       homepage-url   The URL which the secure browser will open when started. If none is provided, a default test URL will be used (not recommended).
    -b branding-info  OPTIONAL: These parameters will be passed directly into the automate.sh script

```

### Linux 

*Not available for Secure Browser 7.0.*

#### TODO 
* Configuration
* Dependencies
* Database configuration
* How to run tests
* Deployment instructions