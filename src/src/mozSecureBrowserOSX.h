#ifndef __mozSecureBrowser_h
#define __mozSecureBrowser_h

#if defined(__OBJC__)
#import <Cocoa/Cocoa.h>
#import <AppKit/NSSpeechSynthesizer.h>
#import <AudioToolbox/AudioServices.h>
#endif

#include "mozISecureBrowser.h"

#include "nsCOMPtr.h"
#include "nsAutoPtr.h"
#include "nsStringAPI.h"
#include "nsServiceManagerUtils.h"
#include "nsIPrefService.h"
#include "nsIPrefBranch.h"

class mozSecureBrowser : public mozISecureBrowser
{
  public:
    NS_DECL_ISUPPORTS
    NS_DECL_MOZISECUREBROWSER

    mozSecureBrowser();
    virtual ~mozSecureBrowser();

#if defined(__OBJC__)
    AudioDeviceID GetDefaultAudioDevice();
#endif

    nsresult Init();
    nsresult SetUIMode(bool aPermissive);

  // DEBUG
  void PrintPointer(const char* aName, nsISupports* aPointer);

  private:
#if defined(__OBJC__)
  NSSpeechSynthesizer* mSpeechSynthesizer;
#endif

  nsAutoString mStatus;

  bool mPermissive, mKillProcess, mShutDown, mListenerInitialized;

};

#endif /* __mozSecureBrowser_h */

