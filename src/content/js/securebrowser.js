// Hooks for SecureBrowser

include(jslib_dirutils);  
include(jslib_dir);  
include(jslib_file);  

function SecureBrowserHooks ()
{
  try
  {
    // this allows netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect") to work from remote web page
    enableXPConnect(true);

    rewrite();

    var os = Components.classes["@mozilla.org/observer-service;1"].getService(Components.interfaces.nsIObserverService);

    // rewrite js close to quit app --pete
    os.addObserver(rewriteClose, "StartDocumentLoad", false);
    os.addObserver(rewriteClose, "EndDocumentLoad", false);

#ifdef XP_MACOSX
    os.addObserver(permissiveListener, "sb-permissive-mode", false);
#endif

    gBrowser.addProgressListener(securebrowserProgressListener);

    createUserPluginsDir();

    /** 
     * Test Runtime Object Instantiation
     * var r = Components.classes["@mozilla.org/securebrowser;1"].createInstance(Components.interfaces.mozISecureBrowser);
     * debug("Runtime", r, r.version);
     *
     */
  }
    catch (e) { debugError(e); }
}

var rewriteClose = 
{
  observe: function (aSubject, aTopic, aState)
  {
    if (aTopic == "StartDocumentLoad" || aTopic == "EndDocumentLoad")
    {
      rewrite();
    }
  }
}

#ifdef XP_MACOSX
var permissiveListener = 
{
  observe: function (aSubject, aTopic, aState)
  {
    if (aTopic == "sb-permissive-mode") 
    {
      if (aState == "ON") resizeWindow();
      else sizeWindow();
    }
  }
}
#endif

function rewrite ()
{
  try
  {
    if (!gBrowser) return;

    var SecureBrowser =
    {
      __exposedProps__    : 
      { 
        CloseWindow       : "r", 
        Restart           : "r", 
        emptyClipBoard    : "r",
        clearCache        : "r",
        print             : "r",
        printWithDialog   : "r",
        hash              : "r",
        audioDevices      : "rw",
        callID            : "rw",
        selectAudioDevice : "r",
        test              : "r",
        showChrome        : "r"
      },

      CloseWindow        : goQuitApp,
      Restart            : jslibRestartApp,
      emptyClipBoard     : emptyClipBoard,

      // cache functions
      clearCache         : securebrowserClearCacheAll,
      clearCookies       : removeCookies,

      // print functions
      print              : openKioskPrint,
      printWithDialog    : SecureBrowserPrint,

      // md5 hash function
      hash               : hashSB,

      // audio devices
      audioDevices       : [],
      callID             : null,
      test               : function () 
      {
        try
        {

          var f = new File("C:\\Windows\\System32\\notepad.exe");

          debug("BEGIN LAUNCH OF NOTEPAD");
          f.nsIFile.launch();  
          debug("END LAUNCH OF NOTEPAD");
        }
          catch (e) { debugError(e); }

      },
      selectAudioDevice  : function (aDevice) 
                           {
                             try
                             {
                               let allowedDevices = Cc["@mozilla.org/supports-array;1"].createInstance(Ci.nsISupportsArray);
                               allowedDevices.AppendElement(aDevice.device);
                               Services.obs.notifyObservers(allowedDevices, "getUserMedia:response:allow", this.callID);
                             }
                               catch (e) { debugError(e); }
                           },

      showChrome         : SecureBrowserShowChrome
    };

    var win = gBrowser.contentWindow.wrappedJSObject;

    win.SecureBrowser = SecureBrowser;
  }
    catch (e) { debugError(e); }
}

function selectAudioDevice (aDevice)
{
}

function securebrowserClearCacheAll ()
{
  securebrowserClearCache(jslibI.nsICache.STORE_ANYWHERE);
}

function SecureBrowserPrint ()
{
  PrintUtils.print();
}

function SecureBrowserShowChrome (aShow)
{
  try
  {
    window.fullScreen = !aShow;
    var r = jslibCreateInstance("@mozilla.org/securebrowser;1", "mozISecurebrowser");

    Services.prefs.setBoolPref("bmakiosk.mode.showChrome", aShow);

    if (aShow)
    {
      var height = screen.height/2;
      var width = screen.availWidth/2;

      window.resizeTo(width, height);
      window.moveTo(0, 0);

      // r.permissive = true;
    }
      else 
    {
      // set window to full screen
      // setTimeout(resizeWindow, 1000);

      r.permissive = false;
    }
  }
    catch (e) { debugError(e); }
}

function createUserPluginsDir ()
{
  try
  {
    var du = new DirUtils;
    var d = new Dir(du.getPrefsDir());
  
    d.append("plugins");

    // jslibPrint("prefs dir", d.path);
    d.create();
  }
    catch (e) { debugError(e); }
}

function removeCookies ()
{
  try
  {
    var cookiemanager = jslibGetService("@mozilla.org/cookiemanager;1", "nsICookieManager");

    cookiemanager.removeAll();
  }
    catch (e) { debugError(e); }
}

function securebrowserClearCache (aType)
{
  try
  {
    var cacheService = jslibGetService("@mozilla.org/network/cache-service;1", "nsICacheService");

    cacheService.evictEntries(aType);
  }
    catch (e) { debugError(e); }
}

// **** Print function ****
function openKioskPrint ()
{
  var dialog = Services.prefs.getBoolPref("print.dialog");

  if (dialog) PrintUtils.print();
  else openKioskPrintSilent();
}

function openKioskPrintSilent ()
{
  var printSettings = PrintUtils.getPrintSettings();

  // debugAlert(printSettings);

try
{
  // No feedback to user
  printSettings.printSilent   = true;
  printSettings.showPrintProgress = false;

  // Other settings
  printSettings.printBGImages = true;
  printSettings.shrinkToFit = true;

  // Frame settings
  printSettings.howToEnableFrameUI = printSettings.kFrameEnableAll;
  printSettings.printFrameType = printSettings.kFramesAsIs;

  try 
  {
    var ifreq = jslibQI(content, "nsIInterfaceRequestor");
    var webBrowserPrint = ifreq.getInterface(jslibI.nsIWebBrowserPrint);
    var printProgress = new kioskPrintListener;
    webBrowserPrint.print(printSettings, printProgress);
  } 
    catch (e) 
  { 
    var failmessage = gKioskBundle.getString('failedprinting');
    statusMessage(failmessage);
  }
}
  catch (e) { jslibPrintError(e); }
}

function Print () 
{
  try
  {
    var printService = jslibGetService("@mozilla.org/gfx/printsettings-service;1", 
				       "nsIPrintSettingsService");

    var printSettings = printService.globalPrintSettings;

    if (!printSettings.printerName)
    {
      jslibQI(printService, "nsIPrintOptions");
      var ap = printService.availablePrinters();
      while (ap.hasMoreElements()) 
      {
	var pName = ap.getNext();
	jslibQI(pName, "nsISupportsString");
	break;
      }

      if (pName) printSettings.printerName = pName;
    }

    try 
    {
      var pn = printSettings.printerName;

      // Get any defaults from the printer 
      printService.initPrintSettingsFromPrinter(pn, printSettings);
    } 
      catch (e) 
    {
      printService.initPrintSettingsFromPrefs(printSettings, 
					      true, 
					      printSettings.kInitSaveAll);
    }

    // No feedback to user
    printSettings.printSilent   = true;
    printSettings.showPrintProgress = false;

    // Other settings
    printSettings.printBGImages = true;
    printSettings.shrinkToFit = true;

    // Frame settings
    printSettings.howToEnableFrameUI = printSettings.kFrameEnableAll;
    printSettings.printFrameType = printSettings.kFramesAsIs;

    try 
    {
      var ifreq = jslibQI(content, "nsIInterfaceRequestor");
      var webBrowserPrint = ifreq.getInterface(jslibI.nsIWebBrowserPrint);
      var printProgress = new kioskPrintListener;
      webBrowserPrint.print(printSettings, printProgress);
    } 
      catch (e) { debugError(e); }
  }
    catch (e) { debugError(e); }
}

function kioskPrintListener () { }

kioskPrintListener.prototype =
{
  status : "",

  init : function() { },
  
  onProgressChange : function() { },

  onStateChange : function() { },

  onStatusChange : function() { }
}

var securebrowserProgressListener = 
{
  onProgressChange : function (wp, req, cur, max, curtotal, maxtotal) {},

  onStateChange : function (wp, req, state, status) 
  {
    try { rewrite(); }
    catch (e) { debugError(e); }
  },

  onLocationChange : function (wp, req, loc) 
  {
    try { rewrite(); }
    catch (e) { debugError(e); }
  },

  onStatusChange : function (wp, req, status, message) {},

  onSecurityChange : function (wp, req, state) {}
}

function enableXPConnect (aEnable)
{
  try
  {
    const p = "securebrowser.enable_assessment_mode";

    if (aEnable) Services.prefs.setBoolPref(p, aEnable);
    else Services.prefs.clearUserPref(p);
  }
    catch (e) { debugError(e); }
}

function SecureBrowserShutdown ()
{
  enableXPConnect(false);
  clearUserData();
  Services.prefs.clearUserPref("bmakiosk.startup.base64page");
}

