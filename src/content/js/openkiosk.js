// Hooks for Open SecureBrowser

include("chrome://bmakiosk/content/js/utils.js");
include("chrome://bmakiosk/content/js/securebrowser.js");

function openKioskHooks ()
{
  try
  {
    if (isAdminMode()) 
    {
      if (!confirmAdminAccess()) goQuitApp();
    }

    // clear data
    clearUserData();

    // hide browser toolbar which is set by pref
    hideUI();

    // set window to full screen
    // setTimeout(sizeWindow, 1);

    SecureBrowserHooks();

    var base64URL = null;

    try 
    { 
      base64URL = Services.prefs.getCharPref("bmakiosk.startup.base64page"); 
    } 
      catch (e) { debugError(e); }

    if (base64URL) setTimeout(loadHomePage, 100, base64URL);

    if (window.arguments.length < 3) loadHomePage();

     window.removeEventListener("fullscreen", onFullScreen, true);
  }
    catch (e) { debugError(e); }
}

function isAdminMode ()
{
  var rv =  Services.prefs.getBoolPref("securebrowser.debug.console") || Services.prefs.getBoolPref("securebrowser.showUI");

  return rv;
}

function hideUI ()
{
  try
  {
    // hide menubar
    var el = document.getElementById("main-menubar");
    el.hidden = true;
    el.collapsed = true;

    el = document.getElementById("TabsToolbar");
    el.hidden = true;
    el.collapsed = true;

    // remove toolbar custimize context menu
    // el = document.getElementById("nav-bar");
    el = document.getElementById("navigator-toolbox");
    el.removeAttribute("context");

    if (!Services.prefs.getBoolPref("securebrowser.showUI"))
    {
      el.hidden = true;
      el.collapsed = true;
    }  

    if (Services.prefs.getBoolPref("securebrowser.debug.console")) 
    {
      gDevToolsBrowser.toggleToolboxCommand(gBrowser);
    }
      else
    {
      el = document.getElementById("browser-bottombox");
      el.hidden = true;
      el.collapsed = true;
    }

    // remove the key command Cmd+Option+C
    // which is generated dynamically
    function toolsMenu ()
    {
      el = document.getElementById("key_inspector");
      el.removeAttribute("command");
    }

    setTimeout(toolsMenu, 500);
    
  }
      catch (e) { debugError(e); }
}

function openKioskShutDown ()
{
  debug("-------- openKioskShutDown --------\n");
  SecureBrowserShutdown();
}

function loadHomePage (aURL)
{
  try
  {
    var url = aURL || atob(Services.prefs.getCharPref("bmakiosk.startup.homepage"));
    loadURI(url);
  }
    catch (e) { debugError(e); }
}

