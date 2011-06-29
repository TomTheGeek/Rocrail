/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#ifdef __linux__
#include <sys/resource.h>
#endif
#ifdef _WIN32
#include <windows.h>
#endif

#include "rocrail/public/app.h"

#include "rocrail/wrapper/public/Cmdline.h"

#include "rocs/public/mem.h"
#include "rocs/public/cmdln.h"

/* ------------------------------------------------------------
 * Windows Service.
 */
#ifdef _WIN32
SERVICE_STATUS          RocrailServiceStatus; 
SERVICE_STATUS_HANDLE   RocrailServiceStatusHandle; 

VOID  WINAPI RocrailServiceCtrlHandler (DWORD opcode); 
VOID  WINAPI RocrailServiceStart (DWORD argc, LPTSTR *argv); 
static void SvcInstall();
static void SvcDelete();

static char* __rocrailService = "RocrailService";
#endif


static iOApp app = NULL;

static void __signalHandler( int sig ) {
  char* sigName = "";

  switch( sig ) {
  case SIGSEGV:
    sigName = "Segment violation.";
    break;
  case SIGTERM:
    sigName = "Software termination signal from kill.";
    break;
#ifdef SIGKILL
  case SIGKILL:
    sigName = "Software termination.";
    break;
#endif
  case SIGINT:
    sigName = "Interrupt.";
    break;
  case SIGABRT:
    sigName = "Abnormal termination triggered by abort call.";
    break;
  case SIGFPE:
    sigName = "Floating point exception.";
    break;
  case SIGILL:
    sigName = "Illegal instruction - invalid function image.";
    break;
#ifdef SIGPIPE
  case SIGPIPE:
    sigName = "Broken pipe.";
    return;
#endif
  }

  printf( "__signalHandler: %d %s\n", sig, sigName );

  printf( "__signalHandler: shutdown...\n" );
  if( sig != SIGSEGV ) {
    AppOp.shutdown();
  }
  else {
    /* try todo a power off... */
    AppOp.stop();
    /* Reactivate default handling. */
    printf( "__signalHandler: Reactivate default handling...\n" );
    signal( sig, SIG_DFL );
    printf( "__signalHandler: Raise signal...\n" );
    raise( sig );
  }

  printf( "__signalHandler: exit...\n" );
}


static int m_argc;
static char** m_argv;

int main( int argc, char* argv[] ) {
  /* Resets memory statistics. */
  MemOp.resetDump();

  /* make copy of arguments for later use: */
  m_argc = argc;
  {
    int n = 0;
    m_argv = allocMem( m_argc * sizeof( char* ) );
    for( n = 0; n < m_argc; n++ ) {
      m_argv[n] = StrOp.dup(argv[n]);
    }
  }

  /* Initialize the signal handler. */
  /* Not all OS's support this signal. */
#ifdef SIGBREAK
  signal( SIGBREAK, &__signalHandler );   /* Ctrl-Break sequence */
#endif
  signal( SIGTERM , &__signalHandler );   /* Software termination signal from kill */
  signal( SIGINT  , &__signalHandler );   /* interrupt */
  signal( SIGABRT , &__signalHandler );   /* abnormal termination triggered by abort call */
  signal( SIGFPE  , &__signalHandler );   /* floating point exception */
  signal( SIGILL  , &__signalHandler );   /* illegal instruction - invalid function image */
  signal( SIGSEGV , &__signalHandler );   /* segment violation */
#ifdef SIGKILL
  signal( SIGKILL , &__signalHandler );   /* kill */
#endif
#ifdef SIGPIPE
  signal (SIGPIPE, SIG_IGN);
#endif
#ifdef __linux__
  {
    struct rlimit rl;
    getrlimit( RLIMIT_CORE, &rl );
    rl.rlim_cur = 1024 * 1024 * 1024;
    setrlimit( RLIMIT_CORE, &rl );
    printf( "   --                     \n" );
    printf( "  / /  (_)__  __ ____  __ \n" );
    printf( " / /__/ / _ \\/ // /\\ \\/ / \n" );
    printf( "/____/_/_//_/\\_,_/ /_/\\_\\ \n" );
  }
#endif


#ifdef _WIN32
  {
    SERVICE_TABLE_ENTRY   DispatchTable[] = 
    { 
      { __rocrailService, RocrailServiceStart  }, 
      { NULL             , NULL                  } 
    }; 

    iOCmdLn arg = CmdLnOp.inst( argc, argv );

    if( CmdLnOp.hasKey( arg, wCmdline.service ) && !CmdLnOp.hasKey( arg, wCmdline.doc ) ) {
    
      printf( "Try to start as service..." );
      if( !StartServiceCtrlDispatcher( DispatchTable ) ) { 
        int rc = GetLastError();
        if( rc == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT ) { 
          printf( "Failed starting Rocrail as service. rc=%d", rc );
          return -1;
        }
        else { 
          printf( "StartServiceCtrlDispatcher rc = %d", rc );
          return -1;
        }
      }
      else {
        return 0;
      }
    }
    else if( CmdLnOp.hasKey( arg, wCmdline.installservice ) ) {
      SvcInstall();
      return 0;
    }
    else if( CmdLnOp.hasKey( arg, wCmdline.deleteservice ) ) {
      SvcDelete();
      return 0;
    }
    arg->base.del( arg );
  }
#endif  

  app = AppOp.inst();
  return AppOp.Main( app, argc, argv );
}


#ifdef _WIN32
void WINAPI RocrailServiceStart( DWORD argc, LPTSTR *argv) { 

  RocrailServiceStatus.dwServiceType             = SERVICE_WIN32; 
  RocrailServiceStatus.dwCurrentState            = SERVICE_START_PENDING; 
  RocrailServiceStatus.dwControlsAccepted        = SERVICE_ACCEPT_STOP; 
  RocrailServiceStatus.dwWin32ExitCode           = 0; 
  RocrailServiceStatus.dwServiceSpecificExitCode = 0; 
  RocrailServiceStatus.dwCheckPoint              = 0; 
  RocrailServiceStatus.dwWaitHint                = 0; 
 
  RocrailServiceStatusHandle = RegisterServiceCtrlHandler( __rocrailService, RocrailServiceCtrlHandler); 
 
  if( RocrailServiceStatusHandle == (SERVICE_STATUS_HANDLE)0 ) { 
    printf( "RegisterServiceCtrlHandler failed %d", GetLastError() );
    return; 
  } 
 
  /* Report running status. */ 
  RocrailServiceStatus.dwCurrentState       = SERVICE_RUNNING; 
  RocrailServiceStatus.dwCheckPoint         = 0; 
  RocrailServiceStatus.dwWaitHint           = 0; 
  SetServiceStatus( RocrailServiceStatusHandle, &RocrailServiceStatus );
    
  app = AppOp.inst();
  /* Call the main entry function. */
  AppOp.Main( app, m_argc, m_argv );

  /* Rocrail ended - report status. */ 
  RocrailServiceStatus.dwCurrentState            = SERVICE_STOPPED; 
  RocrailServiceStatus.dwCheckPoint              = 0; 
  RocrailServiceStatus.dwWaitHint                = 0; 
  RocrailServiceStatus.dwWin32ExitCode           = 0; 
  RocrailServiceStatus.dwServiceSpecificExitCode = 0; 
  SetServiceStatus( RocrailServiceStatusHandle, &RocrailServiceStatus ); 
  
  return; 
} 
 
 
void WINAPI RocrailServiceCtrlHandler( DWORD Opcode ) { 
  DWORD status; 
  
  switch( Opcode ) { 
    case SERVICE_CONTROL_STOP: 
    RocrailServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
    SetServiceStatus( RocrailServiceStatusHandle, &RocrailServiceStatus ); 
    AppOp.shutdown();
    return; 
    
    case SERVICE_CONTROL_PAUSE: 
    break; 
    case SERVICE_CONTROL_CONTINUE: 
    break; 
    case SERVICE_CONTROL_INTERROGATE: 
    SetServiceStatus( RocrailServiceStatusHandle, &RocrailServiceStatus ); 
    break; 
    
    default: 
    break; 
  } 
 
}

static char* __getPath( const char* p_filepath ) {
  char* filepath = NULL;
  char*        p = NULL;
  char*      tmp = NULL;
  char*      sep = NULL;

  if( StrOp.findc( p_filepath, '\\' ) == NULL ) {
    filepath = StrOp.dup( "." );
    return filepath;
  }
  else {
    filepath = StrOp.dup( p_filepath );
    p = filepath;
    tmp = p;
  }

  while( tmp ) {
    tmp = StrOp.findc( p, '\\' );
    if( tmp ) {
      sep = tmp;
      p = tmp + 1;
    }
  }
  if( sep != NULL )
    *sep = '\0';

  return filepath;
}


static void SvcInstall()
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    TCHAR szPath[MAX_PATH];
    TCHAR szPathCmd[MAX_PATH*2];

    if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
    {
        printf("Cannot install service (%d)\n", GetLastError());
        return;
    }

    /* Get a handle to the SCM database. */ 
 
    schSCManager = OpenSCManager( 
        NULL,                    /* local computer */
        NULL,                    /* ServicesActive database */
        SC_MANAGER_ALL_ACCESS);  /* full access rights */ 
 
    if (NULL == schSCManager) 
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return;
    }

    /* Create the service. */
    
    sprintf( szPathCmd, "\"%s\" -service -w \"%s\" -i \"%s\\rocrail.ini\"", szPath, __getPath(szPath), __getPath(szPath));

    schService = CreateService( 
        schSCManager,              /* SCM database */ 
        __rocrailService,          /* name of service */ 
        "Rocrail Service",         /* service name to display */ 
        SERVICE_ALL_ACCESS,        /* desired access */
        SERVICE_WIN32_OWN_PROCESS, /* service type */
        SERVICE_DEMAND_START,      /* start type */
        SERVICE_ERROR_NORMAL,      /* error control type */ 
        szPathCmd,                 /* path to service's binary */ 
        NULL,                      /* no load ordering group */ 
        NULL,                      /* no tag identifier */
        NULL,                      /* no dependencies */
        NULL,                      /* LocalSystem account */ 
        NULL);                     /* no password  */
 
    if (schService == NULL) 
    {
        printf("CreateService failed (%d)\n", GetLastError()); 
        CloseServiceHandle(schSCManager);
        return;
    }
    else printf("Service installed successfully\n"); 

    CloseServiceHandle(schService); 
    CloseServiceHandle(schSCManager);
}

static void SvcDelete()
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    SERVICE_STATUS ssStatus; 

    /* Get a handle to the SCM database. */ 
 
    schSCManager = OpenSCManager( 
        NULL,                    /* local computer */
        NULL,                    /* ServicesActive database */ 
        SC_MANAGER_ALL_ACCESS);  /* full access rights */
 
    if (NULL == schSCManager) 
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return;
    }

    /* Get a handle to the service. */

    schService = OpenService( 
        schSCManager,       /* SCM database */ 
        __rocrailService,   /* name of service */ 
        DELETE);            /* need delete access */ 
 
    if (schService == NULL)
    { 
        printf("OpenService failed (%d)\n", GetLastError()); 
        CloseServiceHandle(schSCManager);
        return;
    }

    /* Delete the service. */
 
    if (! DeleteService(schService) ) 
    {
        printf("DeleteService failed (%d)\n", GetLastError()); 
    }
    else printf("Service deleted successfully\n"); 
 
    CloseServiceHandle(schService); 
    CloseServiceHandle(schSCManager);
}

#endif  

