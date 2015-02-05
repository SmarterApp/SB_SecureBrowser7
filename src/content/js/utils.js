const OPEN_KIOSK_DEBUG = false;

function debug () { dump(Array.join(arguments, ": ")+"\n"); }

function debugError () 
{ 
  var caller = debugError.caller ? debugError.caller.name : "top level";
  debug("ERROR", caller, Array.join(arguments, ": "));
}

function debugAlert () { alert(Array.join(arguments, ": ")); }

/**
 * Convert a string containing binary values to hex.
 */

function binaryToHex (input)
{
  return ("0" + input.toString(16)).slice(-2);
}

function md5 (str, aUseRuntime)
{
  var converter = jslibCreateInstance("@mozilla.org/intl/scriptableunicodeconverter", "nsIScriptableUnicodeConverter");

  converter.charset = "UTF-8";

  if (aUseRuntime)
  {
    var r = Components.classes["@mozilla.org/securebrowser;1"].createInstance(Components.interfaces.mozISecureBrowser);

    str += atob(r.key);
  }

  var data = converter.convertToByteArray(str, {});

  var ch = jslibCreateInstance("@mozilla.org/security/hash;1", "nsICryptoHash");
  ch.init(Components.interfaces.nsICryptoHash.MD5);

  ch.update(data, data.length);

  var hash = ch.finish(false);

  hash = [binaryToHex(hash.charCodeAt(i)) for (i in hash)].join("");

  return hash;
}

function hash (aString)
{
  return md5(aString);
}

function hashSB (aString)
{
  return md5(aString, true);
}

function emptyClipBoard ()
{
  try
  {
    var ch = jslibGetService("@mozilla.org/widget/clipboardhelper;1", "nsIClipboardHelper");

    try
    {
      // OS's that support selection like Linux
      var supportsSelect = jslibGetService("@mozilla.org/widget/clipboard;1", "nsIClipboard").supportsSelectionClipboard();

      if (supportsSelect) ch.copyStringToClipboard("", jslibI.nsIClipboard.kSelectionClipboard);

      ch.copyStringToClipboard("", jslibI.nsIClipboard.kGlobalClipboard);

    }
      catch (e) { debugError(e); }

  }
    catch (e) { debugError(e); }
}

function stripWhiteSpace (aString)
{
  var stripWhitespace = /^\s*(.*)\s*$/;

  return aString.replace(stripWhitespace, "$1");
}

function fixUpURI (aURL)
{
  // tidy up the request URL for compatibility
  var fu = jslibGetService("@mozilla.org/docshell/urifixup;1", "nsIURIFixup");

  var rv = fu.createFixupURI(aURL, jslibI.nsIURIFixup.FIXUP_FLAGS_MAKE_ALTERNATE_URI);

  return rv;
}

function sizeWindow ()
{
  var height = screen.height;
  var width = screen.width;

  window.resizeTo(width, height);
  window.moveTo(0, 0);
}

function resizeWindow ()
{
  var height = screen.height;
  var width = screen.availWidth;

  window.resizeTo(width, height);
}

function clearGlobalHistory ()
{
  PlacesUtils.history.removeAllPages();
}

function goQuitApp ()
{
  var appStartup = Components.classes['@mozilla.org/toolkit/app-startup;1'].
                   getService(Components.interfaces.nsIAppStartup);

  appStartup.quit(Components.interfaces.nsIAppStartup.eAttemptQuit);
}

function clearLoginData ()
{
  var lm = Components.classes["@mozilla.org/login-manager;1"].getService(Components.interfaces.nsILoginManager);
  lm.removeAllLogins();
}

function clearFormData ()
{
  let s = new Sanitizer;
  s.sanitize();
}

function validatePassword (aPass)
{
  return (Services.prefs.getCharPref("bmakiosk.admin.login") == md5(btoa(aPass)));
}

function clearUserData ()
{
  try
  {
    // clear the browser global history which appears in the urlbar dropdown
    clearGlobalHistory();
    emptyClipBoard();
    // clearLoginData();
    // clearFormData();
  }
    catch (e) { debugError(e); }
}

function confirmAdminAccess ()
{
  var ps = Components.classes["@mozilla.org/embedcomp/prompt-service;1"].getService(Components.interfaces.nsIPromptService);

  var pwd = { value: "" };

  if (!ps.promptPassword(null,
                         "Admin Access",
                         "Please enter password.",
                         pwd,
                         null,
                         {value: null}))
    return false;


  if (!validatePassword(pwd.value))
  {
    ps.alert(null, "Invalid Password", "Access Denied");
    return false;
  }

  return true;
}

