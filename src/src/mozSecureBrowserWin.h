#ifndef __mozSecureBrowser_h
#define __mozSecureBrowser_h

#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <Iphlpapi.h>
#include <Mmdeviceapi.h>

#include <atlbase.h>
#include <sphelper.h>
#include <sapi.h>

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

    nsresult Init();
    nsresult ReInit();
    nsresult ResetCallBack();

    void GetStatus(SPVOICESTATUS *aEventStatus);
    void GetVoice(ISpVoice **aVoice);

    PRBool IsVistaOrGreater();

    // DEBUG
    void PrintPointer(const char* aName, nsISupports* aPointer);


  private:

    PRInt32 mNum;

    ISpVoice *mVoice;

    PRBool mInitialized, mListenerInited, mCallBackInited;

    bool mPermissive;

    CComPtr<ISpObjectToken> mLastVoice;

    nsAutoString mStatus, mListenerString;

    PRInt32 mLastVolume, mPitch, mRate;

    BOOL GetLogonSID(HANDLE hToken, PSID *ppsid);

    nsCOMPtr<nsIPrefBranch> mPrefs;

    IMMDeviceEnumerator *mEnumerator;

};

#endif /* __mozSecureBrowser_h */

