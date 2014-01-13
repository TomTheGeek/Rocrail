/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

 


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


#include "rocs/public/str.h"
#include "rocs/public/socket.h"

#if defined __linux__ || defined __APPLE__
#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

static char addressBuffer[INET_ADDRSTRLEN];

const char* getLocalIP(void) {
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa ->ifa_addr->sa_family==AF_INET) {
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            if( StrOp.len(addressBuffer) > 0 && !StrOp.equals(addressBuffer, "127.0.0.1") ) {
              if (ifAddrStruct!=NULL)
                freeifaddrs(ifAddrStruct);
              return addressBuffer;
            }
        }
    }
    if (ifAddrStruct!=NULL)
      freeifaddrs(ifAddrStruct);
    return SocketOp.gethostaddr();
}

#else
const char* getLocalIP(void) {
  return SocketOp.gethostaddr();
}
#endif
