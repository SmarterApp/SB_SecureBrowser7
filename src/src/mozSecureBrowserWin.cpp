// SB header file
#include "mozSecureBrowserWin.h"
#include "stdio.h"
#include <math.h>
#include <direct.h>
#include <EndpointVolume.h>

// Mozilla includes
#include "nsEmbedString.h"
#include "nsIClassInfoImpl.h"
#include "nsIObserverService.h"

// #define DEBUG_PETE 1

void PrintLogMsg2 (char *aMsg)
{

#ifndef DEBUG_PETE
  return;
#endif

  _mkdir("c:\\tmp");

  char path[MAX_PATH] = "c:\\tmp\\out.log";

  FILE* mLog;
  mLog = fopen(path, "a");

  fprintf(mLog, "LOG MESSAGE: (%s)\n", aMsg);
  // fflush(mLog);

  fclose(mLog);
}

void PrintLogMsg2 (nsAString& aMsg)
{

#ifndef DEBUG_PETE
  return;
#endif

  _mkdir("c:\\tmp");

  nsAutoString s(aMsg);

  char path[MAX_PATH] = "c:\\tmp\\out.log";

  FILE* mLog;
  mLog = fopen(path, "a");

  fprintf(mLog, "-------- LOG MESSAGE: (%s) --------\n", NS_ConvertUTF16toUTF8(s).get());
  // fflush(mLog);

  fclose(mLog);
}

mozSecureBrowser::mozSecureBrowser() : 
    mVoice(NULL),
    mPitch(0),
    mRate(-1),
    mLastVolume(10),
    mInitialized(PR_FALSE),
    mListenerInited(PR_FALSE),
    mCallBackInited(PR_FALSE)
{
  printf("-------- mozSecureBrowser::CREATE --------\n");

  if (FAILED(::CoInitialize(NULL)))
  {
    mInitialized = PR_FALSE;
    PrintLogMsg2("FAILED TO CoInitialize ...");
  }
    else
  {
    mInitialized = PR_TRUE;
    SetVolume(10);
  }


  CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (void**)&mEnumerator);

  mPrefs = do_GetService(NS_PREFSERVICE_CONTRACTID);

  // get these prefs so we can then reset back after using permissive mode
  // mPrefs->GetBoolPref("bmakiosk.mode.permissive", &mPermissive);

  mPermissive = PR_FALSE;

  mStatus.Assign(NS_LITERAL_STRING("Stopped"));
}

mozSecureBrowser::~mozSecureBrowser() 
{
  printf("-------- mozSecureBrowser::DESTROY --------\n");

  ::CoUninitialize();

  if (mVoice)
  {
    mVoice->SetNotifySink(NULL);
    mVoice->Release();
    mVoice = NULL;
  }
}

NS_IMPL_ISUPPORTS1(mozSecureBrowser, mozISecureBrowser)


static void __stdcall SBSpeechCallBack(WPARAM wParam, LPARAM lParam)
{
  nsAutoString msg;

  if (wParam == SPEI_START_INPUT_STREAM) msg.Assign(NS_LITERAL_STRING("SPEI_START_INPUT_STREAM"));
  if (wParam == SPEI_END_INPUT_STREAM) msg.Assign(NS_LITERAL_STRING("SPEI_END_INPUT_STREAM"));
  if (wParam == SPEI_VOICE_CHANGE) msg.Assign(NS_LITERAL_STRING("SPEI_VOICE_CHANGE"));
  if (wParam == SPEI_PHONEME) msg.Assign(NS_LITERAL_STRING("SPEI_PHONEME"));
  if (wParam == SPEI_SENTENCE_BOUNDARY) msg.Assign(NS_LITERAL_STRING("SPEI_SENTENCE_BOUNDARY"));
  if (wParam == SPEI_VISEME) msg.Assign(NS_LITERAL_STRING("SPEI_VISEME"));
  if (wParam == SPEI_TTS_AUDIO_LEVEL) msg.Assign(NS_LITERAL_STRING("SPEI_TTS_AUDIO_LEVEL"));


  if (lParam) 
  {
    ((mozSecureBrowser*)lParam)->ResetCallBack();

    if (wParam == SPEI_WORD_BOUNDARY) 
    {
      msg.Assign(NS_LITERAL_STRING("WordStart:"));

      SPVOICESTATUS eventStatus;
      ((mozSecureBrowser*)lParam)->GetStatus(&eventStatus);

      ULONG inWordPos, inWordLen;
      inWordPos = eventStatus.ulInputWordPos;
      inWordLen = eventStatus.ulInputWordLen;

      // there appears to be an 18 char offset ...
      PRInt64 start = inWordPos - 18;
      msg.AppendInt(start);

      msg.Append(NS_LITERAL_STRING(", WordLength:"));

      PRInt64 length = inWordLen;
      msg.AppendInt(length);
      
    }

    if (wParam == SPEI_TTS_BOOKMARK) 
    {
      msg.Assign(NS_LITERAL_STRING("Bookmark:"));

      CSpEvent spevent;
      ISpVoice *voice;

      ((mozSecureBrowser*)lParam)->GetVoice(&voice);

      spevent.GetFrom(voice);

      LPCWSTR bookmarkName = spevent.BookmarkName();

      msg.Append(bookmarkName);

    }
  }

  nsCOMPtr<nsIObserverService> observerService = do_GetService("@mozilla.org/observer-service;1");
  nsresult rv = observerService->NotifyObservers(nullptr, "sb-word-speak", msg.get());
}

nsresult
mozSecureBrowser::Init()
{
  if (!mVoice) Initialize();

  return NS_OK;
}

nsresult
mozSecureBrowser::ReInit()
{
  if (mVoice) return NS_OK;
 
  nsresult rv = Initialize();

  if (NS_FAILED(rv)) return rv;

  if (mLastVolume >= 0) SetVolume(mLastVolume);

  if (mLastVoice) mVoice->SetVoice(mLastVoice);

  if (mRate >= 0) SetRate(mRate);

  return NS_OK;
}

nsresult
mozSecureBrowser::ResetCallBack()
{
  mCallBackInited = PR_FALSE;

  return NS_OK;
}


NS_IMETHODIMP
mozSecureBrowser::GetVersion(nsAString & aVersion)
{
  aVersion = NS_LITERAL_STRING(MOZ_APP_UA_VERSION);

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::GetRunningProcessList(nsAString & _retval) 
{
  HANDLE hProcessSnap;
  PROCESSENTRY32 pe32;
  nsAutoString out;

  // Take a snapshot of all processes in the system.
  hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
  if( hProcessSnap == INVALID_HANDLE_VALUE )
  {
    printf("-------- CreateToolhelp32Snapshot (of processes) failed --------");
  } 
    else 
  {
    // Set the size of the structure before using it.
    pe32.dwSize = sizeof( PROCESSENTRY32 );

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if( !Process32First( hProcessSnap, &pe32 ) )
    {
      CloseHandle( hProcessSnap );          // clean the snapshot object    
    } 
      else 
    {
      // Now walk the snapshot of processes, and
      // display information about each process in turn    
      do
      {
        HANDLE hProcess;

        if (IsVistaOrGreater())
          hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe32.th32ProcessID);
        else
          hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);

        if (hProcess)
        {
          if (out.Length()) out.Append(NS_LITERAL_STRING(","));
          nsAutoString processName(NS_ConvertUTF8toUTF16(pe32.szExeFile).get());
          out.Append(processName);  

          // get the name of the parent process
          char parentProcName[2048];

          HANDLE parentProcess;

          if (IsVistaOrGreater())
            parentProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe32.th32ParentProcessID);
          else
            parentProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ParentProcessID);

          out.Append(NS_LITERAL_STRING("|"));

          if (parentProcess)
          {
            nsAutoString parentProcessName;

            if (GetProcessImageFileName(parentProcess, parentProcName, sizeof(parentProcName)) > 0)
              parentProcessName.AppendLiteral(parentProcName);  

              if (!parentProcessName.IsEmpty()) out.Append(parentProcessName);  
              else out.Append(NS_LITERAL_STRING("noname"));

            CloseHandle(parentProcess);
          }
            else
          {
            out.Append(NS_LITERAL_STRING("unavailable"));
          }

          CloseHandle(hProcess);
        } 
      } 

      while( Process32Next( hProcessSnap, &pe32 ) );

      CloseHandle( hProcessSnap );
    }
  }

  _retval = out;


  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::GetServices(nsAString & _retval)
{
  SC_HANDLE hSCM    = NULL;
  PUCHAR  pBuf    = NULL;
  ULONG  dwBufSize   = 0x00;
  ULONG  dwBufNeed   = 0x00;
  ULONG  dwNumberOfService = 0x00;


  LPENUM_SERVICE_STATUS_PROCESS pInfo = NULL;


  hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE | SC_MANAGER_CONNECT);


  if (hSCM == NULL)
  {
    printf_s("OpenSCManager fail \n");
    return NS_ERROR_FAILURE;
  }


  EnumServicesStatusEx(
   hSCM,
   SC_ENUM_PROCESS_INFO,
   SERVICE_WIN32, // SERVICE_DRIVER
   SERVICE_STATE_ALL,
   NULL,
   dwBufSize,
   &dwBufNeed,
   &dwNumberOfService,
   NULL,
   NULL);


  if (dwBufNeed < 0x01)
  {
   printf_s("EnumServicesStatusEx fail ?? \n");
   return NS_ERROR_FAILURE;
  }


  dwBufSize = dwBufNeed + 0x10;
  pBuf  = (PUCHAR) malloc(dwBufSize);


  EnumServicesStatusEx(
   hSCM,
   SC_ENUM_PROCESS_INFO,
   SERVICE_WIN32,  // SERVICE_DRIVER,
   SERVICE_ACTIVE,  //SERVICE_STATE_ALL,
   pBuf,
   dwBufSize,
   &dwBufNeed,
   &dwNumberOfService,
   NULL,
   NULL);


  pInfo = (LPENUM_SERVICE_STATUS_PROCESS)pBuf;
  nsAutoString processName;
  for (ULONG i=0;i<dwNumberOfService;i++)
  {
    if (processName.Length()) processName.Append(NS_LITERAL_STRING(","));
    processName.AppendLiteral(T2A(pInfo[i].lpDisplayName));
    // printf("Display Name : %s \n", T2A(pInfo[i].lpDisplayName));
    // wprintf_s(L"Display Name \t : %s \n", pInfo[i].lpDisplayName);
    // wprintf_s(L"Service Name \t : %s \n", pInfo[i].lpServiceName);
    // wprintf_s(L"Process Id \t : %04x (%d) \n", pInfo[i].ServiceStatusProcess.dwProcessId, pInfo[i].ServiceStatusProcess.dwProcessId);
  }


  free(pBuf);


  CloseServiceHandle(hSCM);

  _retval.Assign(processName);

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::GetMACAddress(nsAString & _retval)
{
  // PrintLogMsg2("mozSecureBrowser::GetMACAddress"); 

  ULONG BufferLength = 0;
  BYTE* pBuffer = 0;

  if( ERROR_BUFFER_OVERFLOW == GetAdaptersInfo( 0, &BufferLength ))
  {
    // Now the BufferLength contain the required buffer length.
    // Allocate necessary buffer.
    pBuffer = new BYTE[ BufferLength ];
  }
    else
  {
    // Error occurred. handle it accordingly.
    PrintLogMsg2("mozSecureBrowser::GetMACAddress:ERROR"); 

    return NS_ERROR_FAILURE;
  }

  // Get the Adapter Information.
  PIP_ADAPTER_INFO pAdapterInfo =
      reinterpret_cast<PIP_ADAPTER_INFO>(pBuffer);
  GetAdaptersInfo( pAdapterInfo, &BufferLength );

  char out[256];

  // Iterate the network adapters and print their MAC address.
  while( pAdapterInfo )
  {
    // Assuming pAdapterInfo->AddressLength is 6.

    sprintf(out, "%02x:%02x:%02x:%02x:%02x:%02x", pAdapterInfo->Address[0], pAdapterInfo->Address[1], 
                                                  pAdapterInfo->Address[2], pAdapterInfo->Address[3], 
                                                  pAdapterInfo->Address[4], pAdapterInfo->Address[5] );

    break;
  }

  // printf("MAC ADDRESS: (%s)\n", out);

  // deallocate the buffer.
  delete[] pBuffer;

  _retval = NS_ConvertUTF8toUTF16(out).get();

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::Initialize()
{
  if (!mVoice) 
  {
    HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&mVoice);

    if (FAILED(hr))
    {
      // PrintLogMsg2("FAILED TO INITIALIZE VOICE ...");

      return NS_ERROR_NOT_INITIALIZED;
    }
  }

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::Uninitialize()
{
  // PrintLogMsg2("mozSecurebrowser::Uninitialize"); 

  if (mVoice)
  {
    mVoice->SetNotifySink(NULL);
    mVoice->Release();
    mVoice = NULL;
  }

  ::CoUninitialize();

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::InitializeListener(const nsAString & aType)
{
  nsresult rv = NS_OK;

  if (mListenerInited) return rv;

  mListenerString.Assign(aType);
  mListenerInited = PR_TRUE;

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::Play(const nsAString & text)
{
  if (!mInitialized) return NS_ERROR_NOT_INITIALIZED;

  if (text.IsEmpty()) return NS_OK;

  HRESULT hr;
  nsresult rv;

  Stop();

  if (!mVoice) 
  {
    rv = Initialize();

    if (NS_FAILED(rv)) return rv;
  }

  // Set the audio format
  if (mVoice)
  {
    char buf[33];
    itoa(mPitch, buf, 10);

    // to get pitch to work we need to wrap it in an XML <pitch> tag
    nsAutoString s(NS_LITERAL_STRING("<pitch middle=\""));
    s.Append(NS_ConvertUTF8toUTF16(buf));
    s.Append(NS_LITERAL_STRING("\">"));
    s.Append(text);
    s.Append(NS_LITERAL_STRING("</pitch>"));

    // PrintLogMsg2(s);

    mStatus.Assign(NS_LITERAL_STRING("Playing"));

    if (mLastVolume >= 0) SetVolume(mLastVolume);

    if (mLastVoice) hr = mVoice->SetVoice(mLastVoice);

    if (mRate >= 0) SetRate(mRate);
     
    if (mListenerInited && !mCallBackInited)
    {
      hr = mVoice->SetNotifySink(NULL);

      /** Potential Listener types
       * //--- TTS engine
       *  SPEI_START_INPUT_STREAM,
       *  SPEI_END_INPUT_STREAM,
       *  SPEI_VOICE_CHANGE,
       *  SPEI_TTS_BOOKMARK,
       *  SPEI_WORD_BOUNDARY,
       *  SPEI_PHONEME,
       *  SPEI_SENTENCE_BOUNDARY,
       *  SPEI_VISEME,
       *  SPEI_TTS_AUDIO_LEVEL
       */


      PrintLogMsg2(mListenerString);

      ULONGLONG type;
      WPARAM data;

      if (mListenerString.Equals(NS_LITERAL_STRING("SPEI_START_INPUT_STREAM"))) 
      {
        type = SPFEI(SPEI_START_INPUT_STREAM);
        data = SPEI_START_INPUT_STREAM;
      }
        
      if (mListenerString.Equals(NS_LITERAL_STRING("SPEI_END_INPUT_STREAM"))) 
      {
        type = SPFEI(SPEI_END_INPUT_STREAM);
        data = SPEI_END_INPUT_STREAM;
      }

      if (mListenerString.Equals(NS_LITERAL_STRING("SPEI_VOICE_CHANGE"))) 
      {
        type = SPFEI(SPEI_VOICE_CHANGE);
        data = SPEI_VOICE_CHANGE;
      }

        
      if (mListenerString.Equals(NS_LITERAL_STRING("SPEI_TTS_BOOKMARK"))) 
      {
        type = SPFEI(SPEI_TTS_BOOKMARK);
        data = SPEI_TTS_BOOKMARK;
      }

      if (mListenerString.Equals(NS_LITERAL_STRING("SPEI_WORD_BOUNDARY"))) 
      {
        type = SPFEI(SPEI_WORD_BOUNDARY);
        data = SPEI_WORD_BOUNDARY;
      }

      if (mListenerString.Equals(NS_LITERAL_STRING("SPEI_PHONEME"))) 
      {
        type = SPFEI(SPEI_PHONEME);
        data = SPEI_PHONEME;
      }

      if (mListenerString.Equals(NS_LITERAL_STRING("SPEI_SENTENCE_BOUNDARY"))) 
      {
        type = SPFEI(SPEI_SENTENCE_BOUNDARY);
        data = SPEI_SENTENCE_BOUNDARY;
      }

      if (mListenerString.Equals(NS_LITERAL_STRING("SPEI_VISEME"))) 
      {
        type = SPFEI(SPEI_VISEME);
        data = SPEI_VISEME;
      }

      if (mListenerString.Equals(NS_LITERAL_STRING("SPEI_TTS_AUDIO_LEVEL"))) 
      {
        type = SPFEI(SPEI_TTS_AUDIO_LEVEL);
        data = SPEI_TTS_AUDIO_LEVEL;
      }

      hr = mVoice->SetNotifyCallbackFunction(SBSpeechCallBack, data, (LPARAM)this);
      if (!SUCCEEDED(hr)) PrintLogMsg2("SetNotifyCallbac FAILED ...");
      else PrintLogMsg2("SUCCESS: SetNotifyCallbackFunction"); 

      hr = mVoice->SetInterest(type, type);

      if (!SUCCEEDED(hr)) 
      {
        PrintLogMsg2("SetInterest FAILED ...");
        return NS_ERROR_FAILURE;
      }
        else PrintLogMsg2("SUCCESS: SetInterest"); 

      mCallBackInited = PR_TRUE;
    }

    hr = mVoice->Speak(s.get(), SPF_ASYNC, NULL);

    if (FAILED(hr)) return NS_ERROR_FAILURE;
  }
  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::Pause()
{
  if (!mVoice) return NS_OK;

  // PrintLogMsg2(mStatus);

  if (!mStatus.Equals(NS_LITERAL_STRING("Stopped")))
    mStatus.Assign(NS_LITERAL_STRING("Paused"));

  HRESULT hr = mVoice->Pause();

  if (FAILED(hr)) return NS_ERROR_FAILURE;

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::Stop()
{
  if (!mVoice) return NS_OK;

  HRESULT hr = mVoice->Speak( NULL, SPF_PURGEBEFORESPEAK, 0 );

  mStatus.Assign(NS_LITERAL_STRING("Stopped"));

  // mVoice->Pause();

  if (FAILED(hr)) 
  {
    // PrintLogMsg2("mozSecurebrowser::Stop FAILED"); 

    return NS_ERROR_FAILURE;
  }

  mVoice->Release();
  mVoice = NULL;

  ReInit();

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::Resume()
{
  if (!mVoice) return NS_OK;

  if (!mStatus.Equals(NS_LITERAL_STRING("Stopped")))
    mStatus.Assign(NS_LITERAL_STRING("Playing"));

  HRESULT hr = mVoice->Resume();

  if (FAILED(hr)) return NS_ERROR_FAILURE;

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::GetVoiceName(nsAString & _retval)
{
  if (!mVoice) 
  {
    if (mLastVoice) 
    {
      WCHAR *tokenID = NULL;

      mLastVoice->GetId(&tokenID);

      _retval.Assign(tokenID);
    }

    
    return NS_OK;
  }

  ISpObjectToken *pToken;

  HRESULT hr = mVoice->GetVoice(&pToken);

  if (SUCCEEDED(hr)) 
  {
    WCHAR *pszCurTokenId = NULL;
    pToken->GetId(&pszCurTokenId);
    pToken->Release();

    _retval.Assign(pszCurTokenId);
  }

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::SetVoiceName(const nsAString & aVoiceName)
{
  if (!mVoice) return NS_OK;

  // printf("mozSecurebrowser::SetVoiceName (%s)\n", NS_ConvertUTF16toUTF8(aVoiceName).get()); 

  // fix for issue 11907
  Stop();

  HRESULT hr;

  CComPtr<ISpObjectToken>        cpVoiceToken;
  CComPtr<IEnumSpObjectTokens>   cpEnum;
  ULONG                          ulCount = 0;

  hr = SpEnumTokens(SPCAT_VOICES, NULL, NULL, &cpEnum);

  if (SUCCEEDED (hr))
  {
    // Get the number of voices.
    hr = cpEnum->GetCount(&ulCount);
  }

  nsAutoString out;

  // Obtain a list of available voice tokens, set
  // the voice to the token, and call Speak.
  while (SUCCEEDED(hr) && ulCount--)
  {
    cpVoiceToken.Release();

    if (SUCCEEDED (hr))
    {
      hr = cpEnum->Next(1, &cpVoiceToken, NULL);

      WCHAR *pszCurTokenId = NULL;

      cpVoiceToken->GetId(&pszCurTokenId);

      out.Assign(pszCurTokenId);

      // printf("CURRENT: %s\n", NS_ConvertUTF16toUTF8(out).get());
      // printf("IN: %s\n", NS_ConvertUTF16toUTF8(aVoiceName).get());

      // printf("EQUALS: %d\n", out.Equals(aVoiceName));

      if (out.Equals(aVoiceName))
      {
        hr = mVoice->SetVoice(cpVoiceToken);

        mLastVoice = cpVoiceToken;

        // if (SUCCEEDED (hr))
          // PrintLogMsg2("SetVoice SUCCESS ...");
      }
    }
  }

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::GetVolume(int32_t *_retval)
{
  NS_ENSURE_ARG(_retval);

  if (!mVoice) 
  {
    if (mLastVolume >= 0) *_retval = mLastVolume;
    return NS_OK;
  }

  USHORT volume = 0;

  HRESULT hr = mVoice->GetVolume(&volume);

  *_retval = volume/10;

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::SetVolume(int32_t aVolume)
{
  if (!mVoice) return NS_OK;

  if (aVolume >= 0 && aVolume <= 10) 
  {
    HRESULT hr = mVoice->SetVolume(aVolume*10);

    mLastVolume = aVolume;
  }

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::GetRate(int32_t *aRate)
{
  if (!mInitialized) 
  {
    // PrintLogMsg2("mozSecurebrowser:GetRate: RETURN NS_ERROR_NOT_INITIALIZED"); 
    return NS_ERROR_NOT_INITIALIZED;
  }

  if (!mVoice) 
  {
    // PrintLogMsg2("mozSecurebrowser:GetRate: mVoice is null ..."); 
    return NS_OK;
  }


  HRESULT hr = mVoice->GetRate((long*)aRate); 

  *aRate = *aRate + 10;

  if (FAILED(hr)) 
  {
    // PrintLogMsg2("mozSecurebrowser:GetRate: GetRate CALL FAILED ..."); 
    return NS_ERROR_FAILURE;
  }


  // PrintLogMsg2("mozSecurebrowser:GetRate: RETURN OK ..."); 

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::SetRate(int32_t aRate)
{
  if (!mInitialized) 
  {
    // PrintLogMsg2("mozSecurebrowser:SetRate: RETURN NS_ERROR_NOT_INITIALIZED"); 
    return NS_ERROR_NOT_INITIALIZED;
  }

  if (!mVoice) 
  {
    // PrintLogMsg2("mozSecurebrowser:SetRate: mVoice is null ..."); 
    return NS_OK;
  }

  if (aRate > 20) aRate = 20;
  if (aRate < 0) aRate = 0;

  mRate = aRate;

  PRInt32 rate = aRate - 10;

  HRESULT hr = mVoice->SetRate((long)rate); 

  if (FAILED(hr)) 
  {
    // PrintLogMsg2("mozSecurebrowser:SetRate: SetRate CALL FAILED ..."); 
    return NS_ERROR_FAILURE;
  }


  // PrintLogMsg2("mozSecurebrowser:SetRate: RETURN OK ..."); 

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::GetPitch(int32_t *aPitch)
{
  *aPitch = mPitch + 10;

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::SetPitch(int32_t aPitch)
{
  if (aPitch > 20) aPitch = 20;
  if (aPitch < 0) aPitch = 0;

  mPitch = aPitch - 10;

  return NS_OK;
}

void
mozSecureBrowser::GetStatus(SPVOICESTATUS *aEventStatus)
{

  if (!mVoice) return;

  mVoice->GetStatus(aEventStatus, NULL );  

}

void
mozSecureBrowser::GetVoice(ISpVoice **aVoice)
{

  if (!mVoice) return;

  *aVoice = mVoice;

}
NS_IMETHODIMP
mozSecureBrowser::GetStatus(nsAString & _retval)
{

  if (!mVoice) 
  {
    _retval = NS_LITERAL_STRING("Stopped");
    return NS_OK;
  }

  SPVOICESTATUS status;

  mVoice->GetStatus(&status,NULL);

  if (!mStatus.IsEmpty()) 
  {
    // PrintLogMsg2(mStatus);
    _retval.Assign(mStatus);

    if (!mStatus.Equals(NS_LITERAL_STRING("Stopped")))
      mStatus.Assign(NS_LITERAL_STRING(""));

    return NS_OK;
  }

  if (status.dwRunningState == SPRS_IS_SPEAKING)
    _retval = NS_LITERAL_STRING("Playing");
  else if (status.dwRunningState == SPRS_DONE)
    _retval = NS_LITERAL_STRING("Stopped");
  else
    _retval = NS_LITERAL_STRING("Paused");

  return NS_OK;
}


NS_IMETHODIMP
mozSecureBrowser::GetVoices(nsAString & _retval)
{
  if (!mVoice) return NS_OK;

  HRESULT                        hr = S_OK;
  CComPtr<ISpObjectToken>        cpVoiceToken;
  CComPtr<IEnumSpObjectTokens>   cpEnum;
  ULONG                          ulCount = 0;

  hr = SpEnumTokens(SPCAT_VOICES, NULL, NULL, &cpEnum);

  if (SUCCEEDED (hr))
  {
    // Get the number of voices.
    hr = cpEnum->GetCount(&ulCount);
  }

  nsAutoString out;

  // Obtain a list of available voice tokens, set
  // the voice to the token, and call Speak.
  while (SUCCEEDED(hr) && ulCount--)
  {
    cpVoiceToken.Release();

    if (SUCCEEDED (hr))
    {
      hr = cpEnum->Next(1, &cpVoiceToken, NULL);

      WCHAR *pszCurTokenId = NULL;

      cpVoiceToken->GetId(&pszCurTokenId);

      out.Append(pszCurTokenId);

      if (ulCount != 0) out.Append(NS_LITERAL_STRING(","));
    }

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

PRBool
mozSecureBrowser::IsVistaOrGreater()
{
  OSVERSIONINFOEX osvi;

  ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

  GetVersionEx((OSVERSIONINFO *) &osvi);

  return (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion >= 0);
}

NS_IMETHODIMP
mozSecureBrowser::GetSystemVolume(int32_t *aVolume)
{
  if (IsVistaOrGreater())
  {
    HRESULT hr;

    if (mEnumerator)
    {
      IMMDevice *pDevice;
      mEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);

      if (pDevice)
      {
        IAudioEndpointVolume *endpointVolume;
        hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<void **>(&endpointVolume));

        float currentVolume;
        hr = endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);

        *aVolume = (int)floor(currentVolume*10 + 0.5);
      }
    }
  }
    else
  {
    MMRESULT result;
    HMIXER hMixer;
    result = mixerOpen(&hMixer, MIXER_OBJECTF_MIXER, 0, 0, 0);

    MIXERLINE ml = {0};
    ml.cbStruct = sizeof(MIXERLINE);
    ml.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
    result = mixerGetLineInfo((HMIXEROBJ) hMixer, &ml, MIXER_GETLINEINFOF_COMPONENTTYPE);

    MIXERLINECONTROLS mlc = {0};
    MIXERCONTROL mc = {0};
    mlc.cbStruct = sizeof(MIXERLINECONTROLS);
    mlc.dwLineID = ml.dwLineID;
    mlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
    mlc.cControls = 1;
    mlc.pamxctrl = &mc;
    mlc.cbmxctrl = sizeof(MIXERCONTROL);
    result = mixerGetLineControls((HMIXEROBJ) hMixer, &mlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);

    MIXERCONTROLDETAILS mcd = {0};
    MIXERCONTROLDETAILS_UNSIGNED mcdu = {0};

    mcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
    mcd.hwndOwner = 0;
    mcd.dwControlID = mc.dwControlID;
    mcd.paDetails = &mcdu;
    mcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
    mcd.cChannels = 1;
    result = mixerGetControlDetails((HMIXEROBJ) hMixer, &mcd, MIXER_SETCONTROLDETAILSF_VALUE);

    float f = 10 * ((float)mcdu.dwValue/65535); // the volume is a number between 0 and 65535 so convert it to a whole number between 1 and 10

    *aVolume = (int)ceil(f);

    mixerClose(hMixer);
  }

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::SetSystemVolume(int32_t aVolume)
{
  if (aVolume > 10) aVolume = 10;
  if (aVolume < 0) aVolume = 0;

  if (IsVistaOrGreater())
  {
    HRESULT hr;

    if (mEnumerator)
    {
      IMMDevice *pDevice;
      mEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);

      if (pDevice)
      {
        IAudioEndpointVolume *endpointVolume;
        hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<void **>(&endpointVolume));

        
        float newVolume = float(aVolume / 10.0f);

        if (endpointVolume) 
        {
          hr = endpointVolume->SetMute(FALSE, NULL);
          hr = endpointVolume->SetMasterVolumeLevelScalar(newVolume, NULL);

          endpointVolume->Release();
        }
      }
    }
  }
    else
  {
    MMRESULT result;
    HMIXER hMixer;
    result = mixerOpen(&hMixer, MIXER_OBJECTF_MIXER, 0, 0, 0);

    MIXERLINE ml = {0};
    ml.cbStruct = sizeof(MIXERLINE);
    ml.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
    result = mixerGetLineInfo((HMIXEROBJ) hMixer, &ml, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE);

    MIXERLINECONTROLS mlc = {0};
    MIXERCONTROL mc = {0};
    mlc.cbStruct = sizeof(MIXERLINECONTROLS);
    mlc.dwLineID = ml.dwLineID;
    mlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
    mlc.cControls = 1;
    mlc.pamxctrl = &mc;
    mlc.cbmxctrl = sizeof(MIXERCONTROL);
    result = mixerGetLineControls((HMIXEROBJ) hMixer, &mlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);

    MIXERCONTROL mxc;
    MIXERLINECONTROLS mxlc;
    mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
    mxlc.dwLineID = ml.dwLineID;
    mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
    mxlc.cControls = 1;
    mxlc.cbmxctrl = sizeof(MIXERCONTROL);
    mxlc.pamxctrl = &mxc;
    result = mixerGetLineControls((HMIXEROBJ) hMixer, &mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE);

    MIXERCONTROLDETAILS_BOOLEAN mxcdMute = { 0 };
    MIXERCONTROLDETAILS mxcd;
    mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
    mxcd.dwControlID = mxc.dwControlID;
    mxcd.cChannels = 1;
    mxcd.cMultipleItems = 0;
    mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
    mxcd.paDetails = &mxcdMute;
    mixerSetControlDetails(reinterpret_cast<HMIXEROBJ>(hMixer), &mxcd, MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE);

    MIXERCONTROLDETAILS mcd = {0};
    MIXERCONTROLDETAILS_UNSIGNED mcdu = {0};
    mcdu.dwValue = (65535 * aVolume)/10; // the volume is a number between 0 and 65535

    mcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
    mcd.hwndOwner = 0;
    mcd.dwControlID = mc.dwControlID;
    mcd.paDetails = &mcdu;
    mcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
    mcd.cChannels = 1;

    // set master volume
    result = mixerSetControlDetails((HMIXEROBJ) hMixer, &mcd, MIXER_SETCONTROLDETAILSF_VALUE);
    mixerClose(hMixer);
  }

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::GetSystemMute(bool *aSystemMute)
{
  if (IsVistaOrGreater())
  {
    HRESULT hr;

    if (mEnumerator)
    {
      IMMDevice *pDevice;
      mEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);

      if (pDevice)
      {
        IAudioEndpointVolume *endpointVolume;
        hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<void **>(&endpointVolume));

        if (endpointVolume) 
        {
          hr = endpointVolume->GetMute((BOOL*)aSystemMute);

          endpointVolume->Release();
        }
      }
    }
  }

  return NS_OK;
}

NS_IMETHODIMP
mozSecureBrowser::SetSystemMute(bool aSystemMute)
{
  if (IsVistaOrGreater())
  {
    HRESULT hr;

    if (mEnumerator)
    {
      IMMDevice *pDevice;
      mEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);

      if (pDevice)
      {
        IAudioEndpointVolume *endpointVolume;
        hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<void **>(&endpointVolume));

        if (endpointVolume) 
        {
          hr = endpointVolume->SetMute(aSystemMute, NULL);

          endpointVolume->Release();
        }
      }
    }
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
  HWND hwnd = FindWindow("Shell_traywnd", NULL);

  // ShowWindow(FindWindow("Shell_TrayWnd", NULL), SW_HIDE);
  // ShowWindow(FindWindowEx(hwnd, 0, "Button", NULL), SW_HIDE);

  if (!aPermissive)
  {
    // printf("******** LOCK ********\n");

    ShowWindow(hwnd, SW_HIDE); // hide it
    EnableWindow(hwnd, FALSE); // disable it
    EnableWindow(FindWindowEx(hwnd, 0, "Button", NULL), FALSE); // disable it

    if (IsVistaOrGreater())
    {
      HWND startOrb = FindWindowEx(NULL, NULL, MAKEINTATOM(0xC017), NULL);
      ShowWindow(startOrb, SW_HIDE); // Hide Vista Start Orb
    }
  }
    else
  {
    // printf("******** OPEN ********\n");

    ShowWindow(hwnd, SW_SHOW); // show it
    EnableWindow(hwnd, TRUE);  // enable it
    EnableWindow(FindWindowEx(hwnd, 0, "Button", NULL), TRUE); // enable it
    ShowWindow(FindWindowEx(hwnd, 0, "Button", NULL), SW_SHOW);

    if (IsVistaOrGreater())
    {
      HWND startOrb = FindWindowEx(NULL, NULL, MAKEINTATOM(0xC017), NULL);
      ShowWindow(startOrb, SW_SHOW); // Show Vista Start Orb
    }
  }

  mPermissive = aPermissive;
  mPrefs->SetBoolPref("bmakiosk.mode.permissive", mPermissive);

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
  UINT type;
  BOOL bSuccess = SystemParametersInfo(SPI_GETFONTSMOOTHINGTYPE, 0, &type, 0);

  *aClearTypeEnabled = (type == FE_FONTSMOOTHINGCLEARTYPE);

  return NS_OK;
}

void
mozSecureBrowser::PrintPointer(const char* aName, nsISupports* aPointer)
{
  printf ("%s (%p)\n", aName, (void*)aPointer);
}

BOOL
mozSecureBrowser::GetLogonSID (HANDLE hToken, PSID *ppsid) 
{
  BOOL bSuccess = FALSE;
  DWORD dwIndex;
  DWORD dwLength = 0;
  PTOKEN_GROUPS ptg = NULL;

  // Verify the parameter passed in is not NULL.
  if (NULL == ppsid)
    goto Cleanup;

  // Get required buffer size and allocate the TOKEN_GROUPS buffer.

  if (!GetTokenInformation(
    hToken,         // handle to the access token
    TokenGroups,    // get information about the token's groups 
    (LPVOID) ptg,   // pointer to TOKEN_GROUPS buffer
    0,              // size of buffer
    &dwLength       // receives required buffer size
    )) 
  {
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) 
      goto Cleanup;

    ptg = (PTOKEN_GROUPS)HeapAlloc(GetProcessHeap(),
      HEAP_ZERO_MEMORY, dwLength);

    if (ptg == NULL) goto Cleanup;
  }

  // Get the token group information from the access token.

  if (!GetTokenInformation(
        hToken,         // handle to the access token
        TokenGroups,    // get information about the token's groups 
        (LPVOID) ptg,   // pointer to TOKEN_GROUPS buffer
        dwLength,       // size of buffer
        &dwLength       // receives required buffer size
        )) 
  {
    goto Cleanup;
  }

  // Loop through the groups to find the logon SID.

  for (dwIndex = 0; dwIndex < ptg->GroupCount; dwIndex++) 
  {
    if ((ptg->Groups[dwIndex].Attributes & SE_GROUP_LOGON_ID) ==  SE_GROUP_LOGON_ID) 
    {
      // Found the logon SID; make a copy of it.

         dwLength = GetLengthSid(ptg->Groups[dwIndex].Sid);
         *ppsid = (PSID) HeapAlloc(GetProcessHeap(),
                     HEAP_ZERO_MEMORY, dwLength);
         if (*ppsid == NULL)
             goto Cleanup;
         if (!CopySid(dwLength, *ppsid, ptg->Groups[dwIndex].Sid)) 
         {
             HeapFree(GetProcessHeap(), 0, (LPVOID)*ppsid);
             goto Cleanup;
         }
     bSuccess = TRUE;
         break;
    }
  }

  Cleanup: 

  // Free the buffer for the token groups.

  if (ptg != NULL) HeapFree(GetProcessHeap(), 0, (LPVOID)ptg);

  return bSuccess;
}

