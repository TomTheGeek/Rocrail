/** ------------------------------------------------------------
  * $Source: /cvsroot/rojav/rocs/public/rocs.h,v $
  * $Author: robvrs $
  * $Date: 2004/02/27 08:12:39 $
  * $Revision: 1.10 $
  * $Name:  $
  * ------------------------------------------------------------ */
#ifndef __ROCS_H
#define __ROCS_H

#if defined _WIN32
  #pragma message("--- [WIN32] ---")
#elif defined _AIX
  #ifndef __unix__
    #define __unix__
  #endif
  #pragma message("--- [AIX] ---")
  #define __USE_POSIX
  #define _REENTRENT
  #define _ALL_SOURCE
#elif defined __OS2__
  #pragma message("--- [OS/2] ---")
#elif defined __linux__
  #pragma message("--- [LINUX] ---")
#elif defined __APPLE__
  #pragma message("--- [APPLE] ---")
  #define __apple__
  #define __linux__
  #define __unix__
#elif defined __hpux
  #pragma message("--- [HP-UX] ---")
  #define _HPUX_SOURCE
  #define _REENTRENT
  #define _INCLUDE_POSIX_SOURCE
  #define _TERMIOS_INCLUDED
  typedef long  sbsize_t;
  typedef unsigned long bsize_t;
#elif defined __sun
  #pragma message("--- [Sun Solaris] ---")
  #define _POSIX_SOURCE
  #define __EXTENSIONS__
#elif defined __unix__
  #pragma message("--- [Unix] ---")
#else
  #error *** Unsupported Operating System ***
#endif

enum rocs_const {ROCS_MAXPATHLEN=256};
typedef unsigned char byte;
typedef enum {True=1,False=0,Yes=1,No=0,Set=1,Reset=0,OK=1} Boolean;

/* OS Dependent object activation switches: */
#define __ROCS_EVENT__
#define __ROCS_LIB__
#define __ROCS_MUTEX__
#define __ROCS_SERIAL__
#define __ROCS_SOCKET__
#define __ROCS_SYSTEM__
#define __ROCS_THREAD__

#endif
