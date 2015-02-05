#ifdef DEBUG
const DEBUG                 = 1;
#else
const DEBUG                 = 0;
#endif

Components.utils.import("resource://gre/modules/XPCOMUtils.jsm");
Components.utils.import("resource://gre/modules/Services.jsm");

const nsISupports           = Components.interfaces.nsISupports;
const nsICommandLineHandler = Components.interfaces.nsICommandLineHandler;
const nsIWindowWatcher      = Components.interfaces.nsIWindowWatcher;
const nsISupportsString     = Components.interfaces.nsISupportsString;

const nsIObserver           = Components.interfaces.nsIObserver;
const nsIObserverService    = Components.interfaces.nsIObserverService;

const nsIComponentManager   = Components.interfaces.nsIComponentManager;
const nsIComponentRegistrar = Components.interfaces.nsIComponentRegistrar;
const nsICategoryManager    = Components.interfaces.nsICategoryManager;

var gJSLibInited = false;

function debug ()
{
  if (DEBUG) dump(Array.join(arguments, ": ") + "\n");
}

function initJSLib ()
{
  if (gJSLibInited) return;

  Components.classes["@mozilla.org/moz/jssubscript-loader;1"].
                  getService(Components.interfaces.mozIJSSubScriptLoader).
                  loadSubScript("chrome://jslib/content/jslib.js");

  include(jslib_dirutils);
  include(jslib_file);
  include(jslib_fileutils);
  include(jslib_dir);
  
  gJSLibInited = true;
}

function getPrefs ()
{
  var rv = Components.classes["@mozilla.org/preferences-service;1"]
                             .getService(Components.interfaces.nsIPrefService).getBranch(null);

  return rv;
}

function showUI (aShow)
{
  var pref = getPrefs();
  pref.setBoolPref("securebrowser.showUI", aShow);
}

function handleBase64URL (aURL)
{
  try
  {
    debug("handleBase64URL", aURL);
    if (/-options|toolbar|debug|admin|proxy|cache/.test(aURL)) return;

    var url = atob(aURL);

    debug("handleBase64URL", url);

    if (!url) return;

    var pref = getPrefs();
    pref.setCharPref("bmakiosk.startup.base64page", url);
  }
    catch (e) { debug(e); }
}

function debugConsole (aShow)
{
  var pref = getPrefs();
  pref.setBoolPref("securebrowser.debug.console", aShow);
}

function setStartupTimestamp ()
{
  try
  {
    var d = new Date;
  
    // debug("-------- "+ d.toString()  +" --------\n");

    var pref = getPrefs();

    var dateFormat = d.getUTCFullYear()+", "+d.getMonth()+", "+d.getDate()+", "+d.getHours()+", "+d.getSeconds()+", "+d.getMilliseconds();
    pref.setCharPref("bmakiosk.startup.timestamp", d);
    pref.setCharPref("bmakiosk.startup.timestamp.now", Date.now());
  }
    catch (e) { debug("ERROR", e); }
}

var handleQuit =
{
  observe : function (subject, topic, data)
  {
    if (topic == "quit-application") 
    {
      showUI(false);
      debugConsole(false);

      var du = new DirUtils;
      var f = new File(du.getMozUserHomeDir());

      var d = new Dir(f.parent);

      // debug(d.exists());

      var c = d.readDir();

      // remove screenshot png files
      for (var i=0; i<c.length; i++)
      {
       if (/png/i.test(c[i].ext)) c[i].remove(0);
      }

#ifdef XP_MACOSX
      f = new File(du.getHomeDir());
      f.append("Desktop");

      d = new Dir(f);

      var pref = getPrefs();

      var ds = pref.getCharPref("bmakiosk.startup.timestamp.now");
      var ts = new Date(Number(ds));
      debug("TIMESTAMP", ts.getTime());

      c = d.readDir();

      for (i=0; i<c.length; i++)
      {
       // Desktop File
       var df = c[i];

       if (/png/i.test(df.ext) && /Screen/.test(df.leaf)) 
       {
         // debug("File Time", df.dateModified.getTime());

         if (df.dateModified.getTime() > ts.getTime()) 
         {
           // debug("REMOVING FILE", df.leaf);
           df.remove();
         }
       }
      }
#endif
    }
  }
};

function init ()
{
  initJSLib();

  var os = Components.classes["@mozilla.org/observer-service;1"].getService(Components.interfaces.nsIObserverService);
  os.addObserver(handleQuit, "quit-application", false);
}

function handleProxy (aArg)
{
  // debug("handleProxy", "arg", aArg);

  try
  {
    var pref = Components.classes["@mozilla.org/preferences-service;1"]
                               .getService(Components.interfaces.nsIPrefService);

    // debug("pref"+pref);

    pref = pref.getBranch("network.proxy.");

    var num = pref.getIntPref("type");

    // 4 is default
    // debug("cur pref val", num);

    var type = aArg;

    if (/^1/.test(aArg)) 
    {
      // debug("parse for IP address");

      var a = aArg.split(":");

      // debug("length", a.length);

      if (a.length != 4) 
      {
        dump("    usage: -proxy type:protocol:host:port \n");
        goQuitApp();
        return;
      }

      type  = a[0];
      var proto = a[1];
      var host  = a[2];
      var port  = a[3];

      debug("proto", proto, "host", host, "port", port);

      switch (proto)
      {
        case "http":
         pref.setCharPref("http", host);
         pref.setIntPref("http_port", port);
         break;
        
        case "ssl":
         pref.setCharPref("ssl", host);
         pref.setIntPref("ssl_port", port);
         break;
        
        case "ftp":
         pref.setCharPref("ftp", host);
         pref.setIntPref("ftp_port", port);
         break;
        
        case "gopher":
         pref.setCharPref("gopher", host);
         pref.setIntPref("gopher_port", port);
         break;
        
        case "socks":
         pref.setCharPref("socks", host);
         pref.setIntPref("socks_port", port);
         break;

        case "*":
         pref.setCharPref("http", host);
         pref.setIntPref("http_port", port);
         pref.setCharPref("ssl", host);
         pref.setIntPref("ssl_port", port);
         pref.setCharPref("ftp", host);
         pref.setIntPref("ftp_port", port);
         pref.setCharPref("gopher", host);
         pref.setIntPref("gopher_port", port);
         pref.setCharPref("socks", host);
         pref.setIntPref("socks_port", port);
         break;
      }

      pref.setIntPref("type", type);
    }
      else if (/^2/.test(aArg)) 
    {
      debug("parse for URL");

      var a = aArg.split(":");

      if (a.length != 2) 
      {
        dump("    usage: -proxy type:url \n");
        goQuitApp();
        return;
      }

      // debug("length", a.length);

      type  = a[0];
      var url = a[1];

      debug("URL", url);

      pref.setCharPref("autoconfig_url", url);
      pref.setIntPref("type", type);
    }
      else
    {
      pref.setIntPref("type", type);
    }

    // for testing only
    // goQuitApp();
  }
    catch (e) { debug("SET PREF ERROR", e); }
}

function handleArgs (aArg, aCmdLine)
{
  // debug("handleArgs", "arg", aArg);

  var ww = Components.classes['@mozilla.org/embedcomp/window-watcher;1']
                     .getService(nsIWindowWatcher);

  var args = Components.classes['@mozilla.org/supports-string;1']
                       .getService(nsISupportsString);

  switch (aArg)
  {
    case "admin":
      var ps = Components.classes["@mozilla.org/embedcomp/prompt-service;1"].getService(Components.interfaces.nsIPromptService);
      ps.alert(null, "Not Supported", "admin is no longer supported");
      break;

    case "debug":
      showUI(true);
      debugConsole(true);
      aCmdLine.preventDefault = false;
      break;

    case "toolbar":
      showUI(true);
      aCmdLine.preventDefault = false;
      break;

    case "proxy":
      ww.openWindow(null, "chrome://browser/content/preferences/connection.xul", "_blank", 
                    "centerscreen,resizable=no", args);
      break;

    case "cache":
      ww.openWindow(null, // make this an app-modal window on Mac
                "chrome://browser/content/sanitize.xul",
                "Sanitize",
                "chrome,titlebar,centerscreen,modal",
                null);
      break;

    default:
      ww.openWindow(null, "chrome://bmakiosk/content/", "_blank", 
                    "all,chrome,dialog=no", args);
  }

  return 0;
}

function goQuitApp ()
{
  var appStartup = Components.classes['@mozilla.org/toolkit/app-startup;1'].
                   getService(Components.interfaces.nsIAppStartup);

  appStartup.quit(Components.interfaces.nsIAppStartup.eAttemptQuit);
}

function nsSBCommandLineHandler () {}

nsSBCommandLineHandler.prototype = 
{
  /* nsISupports */
  // QueryInterface : XPCOMUtils.generateQI([nsICommandLineHandler]),

  classID: Components.ID("{601ac075-ab89-41c1-a732-a835dd1c7442}"),

  /* nsISupports */
  QueryInterface : function clh_QI (iid) 
  {
    if (!iid.equals(nsISupports) &&
        !iid.equals(nsICommandLineHandler))
      throw Components.results.NS_ERROR_NO_INTERFACE;

    return this;
  },

  handle : function clh_handle (cmdLine) 
  {
    try
    {
      init();

      // debug("LENGTH", cmdLine.length);

      for (let i=0; i<cmdLine.length; i++) 
      {
        // debug("arg", i, atob(cmdLine.getArgument(i)) ? atob(cmdLine.getArgument(i)) : cmdLine.getArgument(i)); 
        handleBase64URL(cmdLine.getArgument(i));
      }

      setStartupTimestamp();

      var flag = cmdLine.findFlag("options", false);

      var isProxy = cmdLine.findFlag("proxy", false);

      // debug("flag", flag, "isProxy", isProxy);

      // pass to default clh 
      if (flag < 0 && isProxy < 0) return;

      if (flag >= 0) var arg = cmdLine.getArgument(++flag);

      if (isProxy >= 0) var arg2 = cmdLine.getArgument(++isProxy);
   
      if (!arg) arg = "default";

      if (/^-/.test(arg))
      {
        dump("Warning: unrecognized command line flag [" +arg+ "]\n");
        goQuitApp();
        return;
      }

      cmdLine.preventDefault = true;

      if (isProxy >= 0) handleProxy(arg2);

      handleArgs(arg, cmdLine);
    }
      catch (e) { debug("ERROR", e); }
  },

  helpInfo : "Usage: securebrowser -options admin       \n"
}

const NSGetFactory = XPCOMUtils.generateNSGetFactory([nsSBCommandLineHandler]);

