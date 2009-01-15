#ifndef __ROCS_DIRENT_H
#define __ROCS_DIRENT_H

#if defined __unix__ || defined _AIX || defined __linux__ || defined __OS2__
  #include <dirent.h>
#elif defined _WIN32
   #include <windows.h>
   #include <io.h>
   struct dirent {
           char   d_name[_MAX_DIR + 1];        /* filename without path */
   };
   typedef struct _DIR {
      struct _finddata_t DirFindBlk;
      long   handle;
      int    openflag;
      struct dirent Dirent;
   } DIR;
#endif

#endif
