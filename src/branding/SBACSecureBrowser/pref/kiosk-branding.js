/* FF Default Settings */

pref("startup.homepage_override_url","about:blank");
pref("startup.homepage_welcome_url","about:blank");

// Interval: Time between checks for a new version (in seconds)
pref("app.update.interval", 43200); // 12 hours

// The time interval between the downloading of mar file chunks in the
// background (in seconds)
pref("app.update.download.backgroundInterval", 60);

// Give the user x seconds to react before showing the big UI. default=48 hours
pref("app.update.promptWaitTime", 172800);

// URL user can browse to manually if for some reason all update installation
// attempts fail.
pref("app.update.url.manual", "https://www.mozilla.org/firefox/");

// A default value for the "More information about this update" link
// supplied in the "An update is available" page of the update wizard. 
pref("app.update.url.details", "https://www.mozilla.org/%LOCALE%/firefox/notes");

// The number of days a binary is permitted to be old
// without checking for an update.  This assumes that
// app.update.checkInstallTime is true.
pref("app.update.checkInstallTime.days", 63);

pref("browser.search.param.ms-pc", "MOZI");
pref("browser.search.param.yahoo-fr", "moz35");
pref("browser.search.param.yahoo-fr-cjkt", "moz35"); // now unused
pref("browser.search.param.yahoo-fr-ja", "mozff");

// Turn on PrivateBRowsing by default
// pref("browser.privatebrowsing.autostart", true);

// BEGIN SB CUSTOM SETTINGS

// if we choose to allow tabs (HandScoring)
pref("securebrowser.allowtabs", false);

// show Nav Toolbar
pref("securebrowser.showUI", false);

// show Debug Console
pref("securebrowser.debug.console", false);

// SBAC PRODUCTION URL
pref("bmakiosk.startup.homepage", "aHR0cHM6Ly9sb2dpbjEuY2xvdWQxLnRkcy5haXJhc3Qub3JnL3N0dWRlbnQvP2M9U0JBQw==");
// pref("bmakiosk.startup.homepage", "aHR0cHM6Ly93d3cubW96ZGV2Z3JvdXAuY29tL2Ryb3Bib3gvdGVzdC8=");

pref("bmakiosk.startup.base64page", "");

// KILL LAUNCHED PROCESSES
pref("bmakiosk.system.enableKillProcess", true);

// SHUTDOWN WHEN OTHER PROCESS IS LAUNCHED
pref("bmakiosk.system.shutdownOnNewProcess", true);

pref("bmakiosk.mode.permissive", false);

// SHOW CHROME MODE
pref("bmakiosk.mode.showChrome", false);

// OSX Spaces is enabled
pref("bmakiosk.spaces.enabled", false);

// OSX Expose is enabled
pref("bmakiosk.expose.enabled", false);

// DISPOSE SPEECH CHANNEL
pref("bmakiosk.speech.disposeSpeechChannel", true);

// VISTA WARNINGS
pref("bmakiosk.settings.enableTaskManagerWarning", false);
pref("bmakiosk.settings.enableSwitchUserWarning", false);

/***********************************************************
// VISTA WARNINGS
pref("bmakiosk.settings.enableTaskManagerWarning", false);
pref("bmakiosk.settings.enableSwitchUserWarning", false);

// JAVA BUNDLE
pref("bmakiosk.settings.enableJavaBundle", true);

// SCAN PLUGINS
pref("bmakiosk.plugins.enablesystem.scan", false);

// FORCE QUIT (Commenting this out as this is causing browser crash in 10.8)
// pref("bmakiosk.system.detectForceQuit", true);

// SECURITY OVERRIDE EXPIRED SSL ERROR PAGE
pref("browser.ssl_override_behavior", 0);

// JAVA VERSION
// pref("bmakiosk.java.version", "1.6.0_25");
pref("bmakiosk.java.version", "1.6.0_29");

***********************************************************/
