//
// Created by TD on 2021/10/18.
//

#include "doodle_server.h"

#include <doodle_lib/core/core_set.h>
#include <doodle_lib/rpc/rpc_server_handle.h>
namespace doodle {

doodle_server *doodle_server::s_service = nullptr;

//
//   FUNCTION: doodle_server::Run(doodle_server &)
//
//   PURPOSE: Register the executable for a service with the Service Control
//   Manager (SCM). After you call Run(ServiceBase), the SCM issues a Start
//   command, which results in a call to the OnStart method in the service.
//   This method blocks until the service has stopped.
//
//   PARAMETERS:
//   * service - the reference to a doodle_server object. It will become the
//     singleton service instance of this service application.
//
//   RETURN VALUE: If the function succeeds, the return value is TRUE. If the
//   function fails, the return value is FALSE. To get extended error
//   information, call GetLastError.
//

BOOL doodle_server::Run(doodle_server &service) {
  DOODLE_LOG_INFO("注册服务");
  s_service = &service;

  SERVICE_TABLE_ENTRY serviceTable[] =
      {
          {const_cast<LPWSTR>(service.m_name), ServiceMain},
          {NULL, NULL}};

  // Connects the main thread of a service process to the service control
  // manager, which causes the thread to be the service control dispatcher
  // thread for the calling process. This call returns when the service has
  // stopped. The process should simply terminate when the call returns.
  return StartServiceCtrlDispatcher(serviceTable);
}

//
//   FUNCTION: doodle_server::ServiceMain(DWORD, PWSTR *)
//
//   PURPOSE: Entry point for the service. It registers the handler function
//   for the service and starts the service.
//
//   PARAMETERS:
//   * dwArgc   - number of command line arguments
//   * lpszArgv - array of command line arguments
//
void WINAPI doodle_server::ServiceMain(DWORD dwArgc, PWSTR *pszArgv) {
  DOODLE_LOG_INFO("开始服务主函数");

  assert(s_service != NULL);

  // Register the handler function for the service
  s_service->m_statusHandle = RegisterServiceCtrlHandler(
      s_service->m_name, ServiceCtrlHandler);
  if (s_service->m_statusHandle == NULL) {
    DOODLE_LOG_INFO("服务错误 {}", GetLastError());

    throw doodle_error{fmt::format("{}", GetLastError())};
  }

  // Start the service.
  s_service->Start(dwArgc, pszArgv);
}

void WINAPI doodle_server::ServiceCtrlHandler(DWORD dwCtrl) {
  switch (dwCtrl) {
    case SERVICE_CONTROL_STOP:
      s_service->Stop();
      break;
    case SERVICE_CONTROL_PAUSE:
      s_service->Pause();
      break;
    case SERVICE_CONTROL_CONTINUE:
      s_service->Continue();
      break;
    case SERVICE_CONTROL_SHUTDOWN:
      s_service->Shutdown();
      break;
    case SERVICE_CONTROL_INTERROGATE:
      break;
    default:
      break;
  }
}
doodle_server::doodle_server(PCWSTR pszServiceName,
                             BOOL fCanStop,
                             BOOL fCanShutdown,
                             BOOL fCanPauseContinue,
                             DWORD dwErrorEventId,
                             WORD wErrorCategoryId) {
  // Service name must be a valid string and cannot be NULL.
  m_name                   = (pszServiceName == NULL) ? const_cast<PWSTR>(L"") : pszServiceName;

  m_statusHandle           = NULL;

  // The service runs in its own process.
  m_status.dwServiceType   = SERVICE_WIN32_OWN_PROCESS;

  // The service is starting.
  m_status.dwCurrentState  = SERVICE_START_PENDING;

  // The accepted commands of the service.
  DWORD dwControlsAccepted = 0;
  if (fCanStop)
    dwControlsAccepted |= SERVICE_ACCEPT_STOP;
  if (fCanShutdown)
    dwControlsAccepted |= SERVICE_ACCEPT_SHUTDOWN;
  if (fCanPauseContinue)
    dwControlsAccepted |= SERVICE_ACCEPT_PAUSE_CONTINUE;
  m_status.dwControlsAccepted        = dwControlsAccepted;

  m_status.dwWin32ExitCode           = NO_ERROR;
  m_status.dwServiceSpecificExitCode = 0;
  m_status.dwCheckPoint              = 0;
  m_status.dwWaitHint                = 0;

  m_dwErrorEventId                   = dwErrorEventId;
  m_wErrorCategoryId                 = wErrorCategoryId;
  p_h                                = new_object<rpc_server_handle>();
}

// doodle_server::~doodle_server(void) = default;

//
//   FUNCTION: doodle_server::OnStart(DWORD, PWSTR *)
//
//   PURPOSE: When implemented in a derived class, executes when a Start
//   command is sent to the service by the SCM or when the operating system
//   starts (for a service that starts automatically). Specifies actions to
//   take when the service starts. Be sure to periodically call
//   doodle_server::SetServiceStatus() with SERVICE_START_PENDING if the
//   procedure is going to take long time. You may also consider spawning a
//   new thread in OnStart to perform time-consuming initialization tasks.
//
//   PARAMETERS:
//   * dwArgc   - number of command line arguments
//   * lpszArgv - array of command line arguments
//

void doodle_server::OnStart(DWORD dwArgc, PWSTR *pszArgv) {
  auto &k_set = core_set::getSet();
  p_h->run_server(k_set.get_meta_rpc_port(), k_set.get_file_rpc_port());
}
//
//   FUNCTION: doodle_server::OnStop()
//
//   PURPOSE: When implemented in a derived class, executes when a Stop
//   command is sent to the service by the SCM. Specifies actions to take
//   when a service stops running. Be sure to periodically call
//   doodle_server::SetServiceStatus() with SERVICE_STOP_PENDING if the
//   procedure is going to take long time.
//
void doodle_server::OnStop() {
  p_h->stop();
}

//
//   FUNCTION: doodle_server::Start(DWORD, PWSTR *)
//
//   PURPOSE: The function starts the service. It calls the OnStart virtual
//   function in which you can specify the actions to take when the service
//   starts. If an error occurs during the startup, the error will be logged
//   in the Application event log, and the service will be stopped.
//
//   PARAMETERS:
//   * dwArgc   - number of command line arguments
//   * lpszArgv - array of command line arguments
//
void doodle_server::Start(DWORD dwArgc, PWSTR *pszArgv) {
  try {
    // Tell SCM that the service is starting.
    SetServiceStatus(SERVICE_START_PENDING);

    // Perform service-specific initialization.
    OnStart(dwArgc, pszArgv);

    // Tell SCM that the service is started.
    SetServiceStatus(SERVICE_RUNNING);
  } catch (doodle_error &dwError) {
    // Log the error.
    DOODLE_LOG_ERROR(dwError.what());

    // Set the service status to be stopped.
    // SetServiceStatus(SERVICE_STOPPED, L"");
  } catch (...) {
    // Log the error.
    WriteLogEntry(L"Service failed to start.", EVENTLOG_ERROR_TYPE, m_dwErrorEventId, m_wErrorCategoryId);

    // Set the service status to be stopped.
    SetServiceStatus(SERVICE_STOPPED);
  }
}

//
//   FUNCTION: doodle_server::Stop()
//
//   PURPOSE: The function stops the service. It calls the OnStop virtual
//   function in which you can specify the actions to take when the service
//   stops. If an error occurs, the error will be logged in the Application
//   event log, and the service will be restored to the original state.
//
void doodle_server::Stop() {
  DWORD dwOriginalState = m_status.dwCurrentState;
  try {
    // Tell SCM that the service is stopping.
    SetServiceStatus(SERVICE_STOP_PENDING);

    // Perform service-specific stop operations.
    OnStop();

    // Tell SCM that the service is stopped.
    SetServiceStatus(SERVICE_STOPPED);
  } catch (doodle_error &dwError) {
    // Log the error.
    DOODLE_LOG_ERROR(dwError.what());

    // Set the orginal service status.
    SetServiceStatus(dwOriginalState);
  } catch (...) {
    // Log the error.
    WriteLogEntry(L"Service failed to stop.", EVENTLOG_ERROR_TYPE, m_dwErrorEventId, m_wErrorCategoryId);

    // Set the orginal service status.
    SetServiceStatus(dwOriginalState);
  }
}




//
//   FUNCTION: doodle_server::Pause()
//
//   PURPOSE: The function pauses the service if the service supports pause
//   and continue. It calls the OnPause virtual function in which you can
//   specify the actions to take when the service pauses. If an error occurs,
//   the error will be logged in the Application event log, and the service
//   will become running.
//
void doodle_server::Pause() {
  try {
    // Tell SCM that the service is pausing.
    SetServiceStatus(SERVICE_PAUSE_PENDING);

    // Perform service-specific pause operations.
    OnPause();

    // Tell SCM that the service is paused.
    SetServiceStatus(SERVICE_PAUSED);
  } catch (DWORD dwError) {
    // Log the error.

    // Tell SCM that the service is still running.
    SetServiceStatus(SERVICE_RUNNING);
  } catch (...) {
    // Log the error.
    WriteLogEntry(L"Service failed to pause.", EVENTLOG_ERROR_TYPE, m_dwErrorEventId, m_wErrorCategoryId);

    // Tell SCM that the service is still running.
    SetServiceStatus(SERVICE_RUNNING);
  }
}

//
//   FUNCTION: doodle_server::OnPause()
//
//   PURPOSE: When implemented in a derived class, executes when a Pause
//   command is sent to the service by the SCM. Specifies actions to take
//   when a service pauses.
//
void doodle_server::OnPause() {
}

//
//   FUNCTION: doodle_server::Continue()
//
//   PURPOSE: The function resumes normal functioning after being paused if
//   the service supports pause and continue. It calls the OnContinue virtual
//   function in which you can specify the actions to take when the service
//   continues. If an error occurs, the error will be logged in the
//   Application event log, and the service will still be paused.
//
void doodle_server::Continue() {
  try {
    // Tell SCM that the service is resuming.
    SetServiceStatus(SERVICE_CONTINUE_PENDING);

    // Perform service-specific continue operations.
    OnContinue();

    // Tell SCM that the service is running.
    SetServiceStatus(SERVICE_RUNNING);
  } catch (DWORD dwError) {
    // Log the error.

    // Tell SCM that the service is still paused.
    SetServiceStatus(SERVICE_PAUSED);
  } catch (...) {
    // Log the error.
    WriteLogEntry(L"Service failed to resume.", EVENTLOG_ERROR_TYPE, m_dwErrorEventId, m_wErrorCategoryId);

    // Tell SCM that the service is still paused.
    SetServiceStatus(SERVICE_PAUSED);
  }
}

//
//   FUNCTION: doodle_server::OnContinue()
//
//   PURPOSE: When implemented in a derived class, OnContinue runs when a
//   Continue command is sent to the service by the SCM. Specifies actions to
//   take when a service resumes normal functioning after being paused.
//
void doodle_server::OnContinue() {
}

//
//   FUNCTION: doodle_server::Shutdown()
//
//   PURPOSE: The function executes when the system is shutting down. It
//   calls the OnShutdown virtual function in which you can specify what
//   should occur immediately prior to the system shutting down. If an error
//   occurs, the error will be logged in the Application event log.
//
void doodle_server::Shutdown() {
  try {
    // Perform service-specific shutdown operations.
    OnShutdown();

    // Tell SCM that the service is stopped.
    SetServiceStatus(SERVICE_STOPPED);
  } catch (DWORD dwError) {
    // Log the error.
  } catch (...) {
    // Log the error.
    WriteLogEntry(L"Service failed to shut down.", EVENTLOG_ERROR_TYPE, m_dwErrorEventId, m_wErrorCategoryId);
  }
}

//
//   FUNCTION: doodle_server::OnShutdown()
//
//   PURPOSE: When implemented in a derived class, executes when the system
//   is shutting down. Specifies what should occur immediately prior to the
//   system shutting down.
//
void doodle_server::OnShutdown() {
}

#pragma endregion

#pragma region Helper Functions

//
//   FUNCTION: doodle_server::SetServiceStatus(DWORD, DWORD, DWORD)
//
//   PURPOSE: The function sets the service status and reports the status to
//   the SCM.
//
//   PARAMETERS:
//   * dwCurrentState - the state of the service
//   * dwWin32ExitCode - error code to report
//   * dwWaitHint - estimated time for pending operation, in milliseconds
//
void doodle_server::SetServiceStatus(DWORD dwCurrentState,
                                     DWORD dwWin32ExitCode,
                                     DWORD dwWaitHint) {
  static DWORD dwCheckPoint = 1;

  // Fill in the SERVICE_STATUS structure of the service.

  m_status.dwCurrentState   = dwCurrentState;
  m_status.dwWin32ExitCode  = dwWin32ExitCode;
  m_status.dwWaitHint       = dwWaitHint;

  m_status.dwCheckPoint =
      ((dwCurrentState == SERVICE_RUNNING) ||
       (dwCurrentState == SERVICE_STOPPED))
          ? 0
          : dwCheckPoint++;

  // Report the status of the service to the SCM.
  ::SetServiceStatus(m_statusHandle, &m_status);
}

//
//   FUNCTION: doodle_server::WriteEventLogEntry(PWSTR, WORD)
//
//   PURPOSE: Log a message to the Application event log.
//
//   PARAMETERS:
//   * pszMessage - string(s) of the message to be logged.
//     to display multiple strings in Windows Event Viewer "Details" page separate them with '\n'
//   * wType - the type of event to be logged. The parameter can be one of
//     the following values.
//
//     EVENTLOG_SUCCESS
//     EVENTLOG_AUDIT_FAILURE
//     EVENTLOG_AUDIT_SUCCESS
//     EVENTLOG_ERROR_TYPE
//     EVENTLOG_INFORMATION_TYPE
//     EVENTLOG_WARNING_TYPE
//   * dwEventId - event id. A string with this id should exist in the message file
//     that gets compiled into the resources. The file that contains those resources
//     is then supposed to be registered with the system.
//   * wCategory - category id. This is similar to event id, but the corresponding string
//     is shown in a separate column in Windows Event Viewer.
//
//   NOTE: It can be overriden to do any other kind of logging in a sublcass
//

void doodle_server::WriteLogEntry(PCWSTR pszMessage, WORD wType, DWORD dwEventId, WORD wCategory) {
  HANDLE hEventSource = NULL;
  PWSTR pszSource;          // Copy of pszMessage for splitting
  PCWSTR *pszStrings;       // Message strings to shown on the "Details" tab in Event Viewer
  WCHAR *pContext;          // Tokenization context
  WCHAR delimiter = L'\n';  // Strings are delimited by new lines
  WORD nStrings   = 1;      // Number of strings in the message

  // Prepare event strings by splitting the message at new lines

  // Copy the input string as tokenizing modifies the source
  pszSource       = _wcsdup(pszMessage);

  if (pszSource == NULL) {
    return;
  }

  // First find the number of strings
  for (PCWSTR pOccur = wcschr(pszMessage, delimiter); pOccur != NULL; pOccur = wcschr(++pOccur, delimiter)) {
    nStrings++;
  }

  try {
    // Allocate the array of strings
    pszStrings = new PCWSTR[nStrings];
  } catch (...) {
    delete pszSource;
    return;
  }

  // Token index
  WORD i = 0;

  for (LPCWSTR token = wcstok_s(pszSource, &delimiter, &pContext); token != NULL; token = wcstok_s(NULL, &delimiter, &pContext)) {
    pszStrings[i++] = token;
  }

  hEventSource = RegisterEventSource(NULL, m_name);

  if (hEventSource) {
    ReportEvent(hEventSource,  // Event log handle
                wType,         // Event type
                wCategory,     // Event category
                dwEventId,     // Event identifier
                NULL,          // No security identifier
                nStrings,      // Size of lpszStrings array
                0,             // No binary data
                pszStrings,    // Array of strings
                NULL           // No binary data
    );

    DeregisterEventSource(hEventSource);
  }

  // Free heap memory
  delete[] pszStrings;

  delete pszSource;
}



}  // namespace doodle