// SB header file
#include "mozSecureBrowserOSX.h"

// Mozilla includes
#include "nsEmbedString.h"
#include "nsIClassInfoImpl.h"
#include "nsObjCExceptions.h"
#include "nsIObserverService.h"

// For GetMACAddress
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/if_dl.h>

#include <Carbon/Carbon.h>

@interface SpeechSynthDelegate : NSObject <NSSpeechSynthesizerDelegate> 
{
  nsCOMPtr<nsIObserverService> mOS;
}
@end

@implementation SpeechSynthDelegate
- (void)initDelegate
{
  mOS = do_GetService("@mozilla.org/observer-service;1");
}

- (void)speechSynthesizer:(NSSpeechSynthesizer *)aSender willSpeakWord:(NSRange)aWordToSpeak ofString:(NSString *)aText
{
  nsAutoString msg;
  msg.Assign(NS_LITERAL_STRING("WordStart:"));

  PRInt64 start = aWordToSpeak.location;
  msg.AppendInt(start);

  msg.Append(NS_LITERAL_STRING(", WordLength:"));

  PRInt64 length = aWordToSpeak.length;
  msg.AppendInt(length);

  if (mOS) mOS->NotifyObservers(nullptr, "sb-word-speak", msg.get());
}

- (void) speechSynthesizer:(NSSpeechSynthesizer *)aSender didEncounterSyncMessage:(NSString *)aMessage
{
  nsAutoString msg;
  msg.Assign(NS_LITERAL_STRING("Sync"));

  if (mOS) mOS->NotifyObservers(nullptr, "sb-word-speak", msg.get());
}

- (void)speechSynthesizer:(NSSpeechSynthesizer *)aSender willSpeakPhoneme:(short)aPhonemeOpcode
{
  return;

  nsAutoString msg;
  msg.Assign(NS_LITERAL_STRING("SpeechPhonem"));

  if (mOS) mOS->NotifyObservers(nullptr, "sb-word-speak", msg.get());
}

- (void)speechSynthesizer:(NSSpeechSynthesizer *)aSender didFinishSpeaking:(BOOL)aSuccess
{
  return;

  nsAutoString msg;
  msg.Assign(NS_LITERAL_STRING("SpeechDone"));

  if (mOS) mOS->NotifyObservers(nullptr, "sb-word-speak", msg.get());
}
@end

mozSecureBrowser::mozSecureBrowser() :
  mSpeechSynthesizer(nullptr)
{
  printf("-------- mozSecureBrowser::CREATE --------\n");

  // Other ways to initialize the Speech Synthesizer
  // mSpeechSynthesizer = [[NSSpeechSynthesizer alloc] init]; // start with default voice
  // mSpeechSynthesizer = [[NSSpeechSynthesizer alloc] initWithVoice:@"com.apple.speech.synthesis.voice.Victoria"];
  mSpeechSynthesizer = [[NSSpeechSynthesizer alloc] initWithVoice:[NSSpeechSynthesizer defaultVoice]];

  mStatus.Assign(NS_LITERAL_STRING("Stopped"));

  if (!mSpeechSynthesizer) printf("------ ERROR INITIALIING SPEECH SYNTHESIZER --------\n");
}

mozSecureBrowser::~mozSecureBrowser() 
{
  printf("-------- mozSecureBrowser::DESTROY --------\n");

  if (mSpeechSynthesizer) [mSpeechSynthesizer release];
}

NS_IMPL_ISUPPORTS1(mozSecureBrowser, mozISecureBrowser)

nsresult
mozSecureBrowser::Init()
{
  nsCOMPtr<nsIPrefBranch> mPrefs;
  nsCOMPtr<nsIPrefService> prefService = do_GetService(NS_PREFSERVICE_CONTRACTID);

  mPermissive = false;

  if (prefService) prefService->GetBranch("", getter_AddRefs(mPrefs));

  if (mPrefs)
  {
    // printf("------ mKillProcess(%d) ------\n", mKillProcess);  

    mPrefs->GetBoolPref("bmakiosk.system.enableKillProcess", &mKillProcess);
    mPrefs->GetBoolPref("bmakiosk.system.shutdownOnNewProcess", &mShutDown);

    // printf("------ mKillProcess(%d) ------\n", mKillProcess);  
    // printf("------ mShutDown(%d) ------\n", mShutDown);  
  }

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::GetVersion(nsAString & aVersion)
{
  aVersion = NS_LITERAL_STRING(MOZ_APP_UA_VERSION);

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::GetServices(nsAString & _retval)
{
  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::GetRunningProcessList(nsAString & _retval) 
{
  nsAutoString out;

  for (NSRunningApplication *currApp in [[NSWorkspace sharedWorkspace] runningApplications]) 
  {
    // NSLog(@"GetRunningProcessList:  %@", [currApp localizedName]);

    if (out.Length()) out.Append(NS_LITERAL_STRING(","));

    out.AppendLiteral([[currApp localizedName] UTF8String]);
    out.Append(NS_LITERAL_STRING("|NA"));			// Parent processname
  }

  _retval.Assign(out); 

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::GetMACAddress(nsAString & _retval)
{
  NSTask *task = [[NSTask alloc] init];
  [task setLaunchPath:@"/bin/sh"];
  [task setArguments:@[@"-c", @"/sbin/ifconfig | grep ether | sed '2,$d' | sed -e's:ether::g'"]];

  NSPipe * out = [NSPipe pipe];
  [task setStandardOutput:out];

  [task launch];
  [task waitUntilExit];
  [task release];

  NSFileHandle * read = [out fileHandleForReading];
  NSData * dataRead = [read readDataToEndOfFile];
  NSString * eth0 = [[[NSString alloc] initWithData:dataRead encoding:NSUTF8StringEncoding] autorelease];
  NSLog(@"output: %@", eth0);

  nsAutoString eth0_MACAddress;
  eth0_MACAddress.AppendLiteral([eth0 UTF8String]);

  task = [[NSTask alloc] init];
  [task setLaunchPath:@"/bin/sh"];
  [task setArguments:@[@"-c", @"/sbin/ifconfig | grep ether | sed '1d' | sed '2,$d' | sed -e's:ether::g'"]];

  out = [NSPipe pipe];
  [task setStandardOutput:out];

  [task launch];
  [task waitUntilExit];
  [task release];

  read = [out fileHandleForReading];
  dataRead = [read readDataToEndOfFile];
  NSString * eth1 = [[[NSString alloc] initWithData:dataRead encoding:NSUTF8StringEncoding] autorelease];
  NSLog(@"output: %@", eth1);

  nsAutoString eth1_MACAddress;
  eth1_MACAddress.AppendLiteral([eth1 UTF8String]);

  task = [[NSTask alloc] init];
  [task setLaunchPath:@"/bin/sh"];
  [task setArguments:@[@"-c", @"/sbin/ifconfig | grep status | sed '3d' | sed '2d' | sed -e's/status://g'"]];

  out = [NSPipe pipe];
  [task setStandardOutput:out];

  [task launch];
  [task waitUntilExit];
  [task release];

  read = [out fileHandleForReading];
  dataRead = [read readDataToEndOfFile];
  NSString * eth0_Active = [[[NSString alloc] initWithData:dataRead encoding:NSUTF8StringEncoding] autorelease];
  NSLog(@"output: %@", eth0_Active);

  nsAutoString eth0_isActive;
  eth0_isActive.AppendLiteral([eth0_Active UTF8String]);
  eth0_isActive.StripWhitespace();

  nsAutoString outString;

  if (eth0_isActive.Equals(NS_LITERAL_STRING("active"))) outString.Assign(eth0_MACAddress);
  else outString.Assign(eth1_MACAddress);

  outString.StripWhitespace();

  _retval.Assign(outString); 

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::Initialize()
{
  nsCOMPtr<nsIPrefBranch> mPrefs;
  nsCOMPtr<nsIPrefService> prefService = do_GetService(NS_PREFSERVICE_CONTRACTID);

  if (prefService) prefService->GetBranch("", getter_AddRefs(mPrefs));

  bool dispose;
  mPrefs->GetBoolPref("bmakiosk.speech.disposeSpeechChannel", &dispose);

  if (dispose)
  {
    mSpeechSynthesizer = nullptr;
    mSpeechSynthesizer = [[NSSpeechSynthesizer alloc] init]; // reinitialize
  }

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::Uninitialize()
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
mozSecureBrowser::InitializeListener(const nsAString & type)
{
  if (!mListenerInitialized)
  {
    SpeechSynthDelegate* speechDelegate = [[SpeechSynthDelegate alloc] init];
    [mSpeechSynthesizer setDelegate:speechDelegate];
    [speechDelegate initDelegate];

    mListenerInitialized = true;
  }

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::Play(const nsAString & text)
{
  if (!mSpeechSynthesizer) return NS_ERROR_NOT_INITIALIZED;

  NSString* string = [[NSString alloc] initWithUTF8String:NS_ConvertUTF16toUTF8(text).get()];
  [mSpeechSynthesizer startSpeakingString:string];

  mStatus.Assign(NS_LITERAL_STRING("Playing"));

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::Pause()
{
  if (!mSpeechSynthesizer) return NS_ERROR_NOT_INITIALIZED;

  if (mStatus.Equals(NS_LITERAL_STRING("Stopped"))) return NS_OK;

  [mSpeechSynthesizer pauseSpeakingAtBoundary:NSSpeechImmediateBoundary];

  mStatus.Assign(NS_LITERAL_STRING("Paused"));

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::Stop()
{
  if (!mSpeechSynthesizer) return NS_ERROR_NOT_INITIALIZED;

  [mSpeechSynthesizer stopSpeaking];

  mStatus.Assign(NS_LITERAL_STRING("Stopped"));

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::Resume()
{
  if (!mSpeechSynthesizer) return NS_ERROR_NOT_INITIALIZED;

  if (mStatus.Equals(NS_LITERAL_STRING("Stopped")) || mStatus.Equals(NS_LITERAL_STRING("Playing"))) return NS_OK;

  [mSpeechSynthesizer continueSpeaking];

  mStatus.Assign(NS_LITERAL_STRING("Playing"));

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::GetVoiceName(nsAString & _retval)
{
  if (!mSpeechSynthesizer) return NS_ERROR_NOT_INITIALIZED;

  _retval.AssignLiteral([[mSpeechSynthesizer voice] UTF8String]);

  // printf("-------- GetVoiceName (%s) --------\n", NS_ConvertUTF16toUTF8(_retval).get());

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::SetVoiceName(const nsAString & aVoiceName)
{
  if (!mSpeechSynthesizer) return NS_ERROR_NOT_INITIALIZED;

  printf("-------- SetVoiceName (%s) --------\n", NS_ConvertUTF16toUTF8(aVoiceName).get());

  NSString* voice = [[NSString alloc] initWithUTF8String:NS_ConvertUTF16toUTF8(aVoiceName).get()];

  [mSpeechSynthesizer setVoice:voice];

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::GetVolume(int32_t *aVolume)
{
  if (!mSpeechSynthesizer) return NS_ERROR_NOT_INITIALIZED;

  // Speaking volume range: From 0.0 to 1.0
  float volume = [mSpeechSynthesizer volume];

  *aVolume = lroundf(volume*10);

  // NSLog(@"GetVolume Sys(%.2f)", volume);
  // NSLog(@"GetVolume Out(%d)", *aVolume);

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::SetVolume(int32_t aVolume)
{
  if (!mSpeechSynthesizer) return NS_ERROR_NOT_INITIALIZED;

  if (aVolume < 0) aVolume = 0;
  if (aVolume > 10) aVolume = 10;

  float volume = (aVolume*.10);

  // NSLog(@"SetVolume In(%d)", aVolume);
  // NSLog(@"SetVolume Sys(%.2f)", volume);

  // Speaking volume range: From 0.0 to 1.0
  [mSpeechSynthesizer setVolume:volume];

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::GetRate(int32_t *aRate)
{
  if (!mSpeechSynthesizer) return NS_ERROR_NOT_INITIALIZED;

  float rate = [mSpeechSynthesizer rate];

  float res = (rate - 100)/6;

  *aRate = (int)res;

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::SetRate(int32_t aRate)
{
  if (!mSpeechSynthesizer) return NS_ERROR_NOT_INITIALIZED;

  // Average human speech occurs at a rate of 180 to 220 words per minute

  if (aRate < 0) aRate = 0;

  if (aRate > 20) aRate = 20;

  float rate = (6 * aRate) + 100;

  [mSpeechSynthesizer setRate:rate];

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::GetPitch(int32_t *aPitch)
{
  if (!mSpeechSynthesizer) return NS_ERROR_NOT_INITIALIZED;

  NSError *err;
  NSNumber *pitch = [mSpeechSynthesizer objectForProperty:NSSpeechPitchBaseProperty error:&err];

  // pitch values in the ranges of 30 to 65
  float newVal = ([pitch floatValue]-30)/1.75;
  
  if (newVal < 1) *aPitch = 0; 
  else *aPitch = (int)lroundf(newVal);

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::SetPitch(int32_t aPitch)
{
  if (!mSpeechSynthesizer) return NS_ERROR_NOT_INITIALIZED;

  if (aPitch <= 0) aPitch = 0;
  if (aPitch > 20) aPitch = 20;

  float fin = (float)aPitch;
  float newVal = (1.75*fin) + 30;

  NSError *err;
  NSNumber *pitch = [NSNumber numberWithFloat:newVal];
  [mSpeechSynthesizer setObject:pitch forProperty:NSSpeechPitchBaseProperty error:&err];

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::GetStatus(nsAString & _retval)
{
  _retval.Assign(mStatus);

  if (![NSSpeechSynthesizer isAnyApplicationSpeaking]) _retval.Assign(NS_LITERAL_STRING("Stopped"));

  return NS_OK;
}


NS_IMETHODIMP
mozSecureBrowser::GetVoices(nsAString & _retval)
{
  NSArray *voices = [NSSpeechSynthesizer availableVoices];

  nsAutoString str, out;

  for (id el in voices)
  {
    if (out.Length()) out.Append(NS_LITERAL_STRING(","));

    out.AppendLiteral([el UTF8String]); 
  }

  _retval.Assign(out);

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::GetKey(nsAString & _retval)
{
  // XKTEEDmtrDznFw312vbxW4bhlaKYxVYL0Eo6pQo6PzbYy0Fl4PbDXOKd227KsL7
  _retval.Assign(NS_LITERAL_STRING("WEtURUVEbXRyRHpuRnczMTJ2YnhXNGJobGFLWXhWWUwwRW82cFFvNlB6Yll5MEZsNFBiRFhPS2QyMjdLc0w3"));

  return NS_OK;
}

AudioDeviceID 
mozSecureBrowser::GetDefaultAudioDevice()
{
  OSStatus err;
  AudioDeviceID device = 0;
  UInt32 size = sizeof(AudioDeviceID);

  AudioObjectPropertyAddress address = {
      kAudioHardwarePropertyDefaultOutputDevice,
      kAudioObjectPropertyScopeGlobal,
      kAudioObjectPropertyElementMaster
  };

  err = AudioObjectGetPropertyData(kAudioObjectSystemObject,
                                   &address,
                                   0,
                                   NULL,
                                   &size,
                                   &device);

  if (err) NSLog(@"could not get default audio output device");

  return device;
}

NS_IMETHODIMP
mozSecureBrowser::GetSystemVolume(int32_t *aSystemVolume)
{
  Float32 volume = 0;
  UInt32 size = sizeof(Float32);

  AudioObjectPropertyAddress address = { 
    kAudioDevicePropertyVolumeScalar,
    kAudioDevicePropertyScopeOutput,
    1 // I am unsure of channel BUT it works... 
  };

  AudioDeviceID device = GetDefaultAudioDevice();

  OSStatus err = AudioObjectGetPropertyData(device,
                                             &address,
                                             0,
                                             NULL,
                                             &size,
                                             &volume);

  // NSLog(@"volume(%.2f)", volume);

  if (err != noErr) 
  {
    NSLog(@"No volume returned for device");
    return NS_ERROR_FAILURE;
  }

  Float32 f = volume * 10;
  *aSystemVolume = (int)round(f);

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::SetSystemVolume(int32_t aSystemVolume)
{
  Float32 volume = (Float32)aSystemVolume/10;
  UInt32 size = sizeof(Float32);

  AudioObjectPropertyAddress address = { 
    kAudioDevicePropertyVolumeScalar,
    kAudioDevicePropertyScopeOutput,
    1 
  };

  AudioDeviceID device = GetDefaultAudioDevice();

  // set channel 0
  OSStatus err = AudioObjectSetPropertyData(device, &address, 0, NULL, size, &volume);

  address = { 
    kAudioDevicePropertyVolumeScalar,
    kAudioDevicePropertyScopeOutput,
    2 
  };

  // set channel 1
  err = AudioObjectSetPropertyData(device, &address, 0, NULL, size, &volume);

  // NSLog(@"volume(%.2f)", volume);

  if (err != noErr) 
  {
    NSLog(@"No volume returned for device");
    return NS_ERROR_FAILURE;
  }

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::GetSystemMute(bool *aSystemMute)
{
  UInt32 size = sizeof(UInt32);
  UInt32 muteVal;

  AudioObjectPropertyAddress address = {
      kAudioDevicePropertyMute,
      kAudioDevicePropertyScopeOutput,
      0
  };

  AudioDeviceID device = GetDefaultAudioDevice();

  OSStatus err;
  err = AudioObjectGetPropertyData(device,
                                   &address,
                                   0,
                                   NULL,
                                   &size,
                                   &muteVal);

 // NSLog(@"muteVal %u", (unsigned int)muteVal);

  if (err != noErr) 
  {
    NSLog(@"error while getting mute status");
    return NS_ERROR_FAILURE;
  }

  *aSystemMute =  (bool)muteVal;

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::SetSystemMute(bool aSystemMute)
{
  UInt32 muteVal = (UInt32)aSystemMute;

  AudioObjectPropertyAddress address = {
      kAudioDevicePropertyMute,
      kAudioDevicePropertyScopeOutput,
      0
  };

  AudioDeviceID device = GetDefaultAudioDevice();

  OSStatus err;
  err = AudioObjectSetPropertyData(device,
                                   &address,
                                   0,
                                   NULL,
                                   sizeof(UInt32),
                                   &muteVal);
  if (err != noErr)
  {
    NSLog(@"error while %@muting", (aSystemMute ? @"" : @"un"));
    return NS_ERROR_FAILURE;
  }

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::GetPermissive(bool *aPermissive)
{
  *aPermissive = mPermissive;

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::SetPermissive(bool aPermissive)
{
  nsresult rv = SetUIMode(aPermissive);

  nsCOMPtr<nsIObserverService> os;
  os = do_GetService("@mozilla.org/observer-service;1");

  nsAutoString msg;
  if (aPermissive) msg.Assign(NS_LITERAL_STRING("ON"));
  else msg.Assign(NS_LITERAL_STRING("OFF"));

  if (os) os->NotifyObservers(nullptr, "sb-permissive-mode", msg.get());

  return rv;
}

nsresult
mozSecureBrowser::SetUIMode(bool aPermissive)
{ 
  OSStatus status = noErr;
  
  nsCOMPtr<nsIPrefBranch> mPrefs;
  nsCOMPtr<nsIPrefService> prefService = do_GetService(NS_PREFSERVICE_CONTRACTID);

  if (prefService) prefService->GetBranch("", getter_AddRefs(mPrefs));

  if (!aPermissive)
  {
    printf("-------- SETTING TO NOT PERMISSIVE --------\n");
    status = SetSystemUIMode(kUIModeAllHidden, kUIOptionDisableProcessSwitch     |
                                               kUIOptionDisableForceQuit         |
                                               kUIOptionDisableSessionTerminate  |
                                               kUIOptionDisableAppleMenu);
    if (status != noErr) 
    {
      printf("-------- ERROR --------\n");
      return NS_ERROR_FAILURE;
    }

    // reset back to default
    if (mPrefs)
    {
      mPrefs->SetBoolPref("bmakiosk.system.enableKillProcess", mKillProcess);
      mPrefs->SetBoolPref("bmakiosk.system.shutdownOnNewProcess", mShutDown);
    }
  }
    else
  {
    printf("-------- SETTING TO PERMISSIVE --------\n");
    status = SetSystemUIMode(kUIModeContentHidden, kUIOptionDisableAppleMenu|kUIOptionDisableProcessSwitch|kUIOptionDisableForceQuit);

    if (status != noErr)
    {
      printf("-------- ERROR --------\n");
      return NS_ERROR_FAILURE;
    }

    for (NSRunningApplication *currApp in [[NSWorkspace sharedWorkspace] runningApplications]) 
    {
      NSLog(@"GetRunningProcessList:  %@", [currApp localizedName]);

      if ([[currApp localizedName] isEqualToString:@"Dock"])
      {
        // NSLog(@"DOCK FOUND!");

        // unhide and activate the Dock 
        [currApp unhide];
        // [currApp activateWithOptions:NSApplicationActivateAllWindows];
        [currApp activateWithOptions:NSApplicationActivateIgnoringOtherApps];

        break;
      }
    }

    if (mPrefs)
    {
      mPrefs->SetBoolPref("bmakiosk.system.enableKillProcess", false);
      mPrefs->SetBoolPref("bmakiosk.system.shutdownOnNewProcess", false);
    }
  }

  mPermissive = aPermissive;

  if (mPrefs) mPrefs->SetBoolPref("bmakiosk.mode.permissive", mPermissive);

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::GetSoxVersion(nsAString & aSoxVersion)
{
  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::GetClearTypeEnabled(bool *aClearTypeEnabled)
{
  return NS_OK;
}

void
mozSecureBrowser::PrintPointer(const char* aName, nsISupports* aPointer)
{
  printf ("%s (%p)\n", aName, (void*)aPointer);
}


