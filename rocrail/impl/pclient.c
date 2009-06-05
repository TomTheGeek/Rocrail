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
#include "rocrail/public/model.h"
#include "rocrail/public/clntcon.h"
#include "rocrail/public/loc.h"
#include "rocrail/public/switch.h"
#include "rocrail/public/app.h"
#include "rocrail/public/control.h"
#include "rocrail/impl/pclient_impl.h"
#include "rocrail/impl/web/web.h"
#include "rocrail/impl/web/webme.h"

#include "rocrail/wrapper/public/Global.h"
#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/Trace.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Track.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/WebClient.h"
#include "rocrail/wrapper/public/WebCam.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/AutoCmd.h"

#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/trace.h"
#include "rocs/public/system.h"
#include "rocs/public/thread.h"
#include "rocs/public/map.h"
#include "rocs/public/strtok.h"
#include "rocs/public/node.h"


static int instCnt = 0;

/** ----- OBase ----- */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static void __del( void* inst ) {
  if( inst != NULL ) {
    iOPClientData data = Data(inst);
    /* Cleanup data->xxx members...*/
    ListOp.base.del( data->locIDs );
    freeMem( data );
    freeMem( inst );
    instCnt--;
  }
  return;
}

static const char* __name( void ) {
  return name;
}

static unsigned char* __serialize( void* inst, long* size ) {
  return NULL;
}

static void __deserialize( void* inst,unsigned char* bytestream ) {
  return;
}

static char* __toString( void* inst ) {
  return NULL;
}

static int __count( void ) {
  return instCnt;
}

static struct OBase* __clone( void* inst ) {
  return NULL;
}

static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}

static void* __properties( void* inst ) {
  return NULL;
}

/** ----- OPClient ----- */




/** PClient ID. */
static const char* _getId( struct OPClient* inst ) {
  iOPClientData data = Data(inst);
  return data->cid;
}


/** Object creator. */
static struct OPClient* _inst( iOSocket socket ,iONode ini ) {
  iOPClient __PClient = allocMem( sizeof( struct OPClient ) );
  iOPClientData data = allocMem( sizeof( struct OPClientData ) );
  MemOp.basecpy( __PClient, &PClientOp, 0, sizeof( struct OPClient ), data );

  /* Initialize data->xxx members... */
  data->ini           = ini;
  data->socket        = socket;
  data->cid           = StrOp.fmt( "%08X", __PClient );
  data->refresh       = wWebClient.getrefresh(ini);
  data->locIDs        = ModelOp.getLocIDs( AppOp.getModel() );

  instCnt++;
  return __PClient;
}


/** ------------------------------------------------------------
  * __getFavicon()
  * Writes the internal si_gif[] array to the client.
  *
  * @param inst     HClient instance.
  * @return
  */
extern unsigned char rocrail_gif[];
extern int rocrail_gif_len;
static void __getFavicon( iOPClient inst ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999, "__getFavicon( inst=0x%08X )", inst );
  if( inst != NULL ) {
    iOPClientData data = Data(inst);
    SocketOp.write( data->socket, (char*)rocrail_gif, rocrail_gif_len );
  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL!" );
    return;
  }
}


static Boolean __httpHeader( iOSocket s ) {
  Boolean ok = True;
  if(ok) ok=SocketOp.fmt( s, "HTTP/1.0 200 OK\r\n" );
  if(ok) ok=SocketOp.fmt( s, "Content-type: text/html\r\n\r\n" );
  return  ok;
}

static Boolean __httpHeader4Image( iOSocket s, char* type ) {
  Boolean ok = True;
  TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "header for image %s", type );
  if(ok) ok=SocketOp.fmt( s, "HTTP/1.0 200 OK\r\n" );
  if(ok) ok=SocketOp.fmt( s, "Content-type: image/%s\r\n\r\n", type );
  return  ok;
}



static char* __getAddrImage( char* s, int addr, const char* dir, const char* symbolpath, const char* symboltype ) {
  int x100 = addr / 100;
  int x10  = (addr % 100) / 10;
  int x1   = addr % 10;
  const char* brk = "";
  if( dir[0] == 'v' )
    brk = "<br>";
  else
    dir = "";
  if( addr > 0 ) {
    StrOp.fmtb( s, "<img border=\"0\" src=\"%s/%d%s.%s\">%s<img border=\"0\" src=\"%s/%d%s.%s\">%s<img border=\"0\" src=\"%s/%d%s.%s\">",
                symbolpath, x100, dir, symboltype, brk,
                symbolpath,  x10, dir, symboltype, brk,
                symbolpath,   x1, dir, symboltype );
  }
  else {
    StrOp.fmtb( s, "<img border=\"0\" src=\"%s/%s%s.%s\">%s<img border=\"0\" src=\"%s/%s%s.%s\">%s<img border=\"0\" src=\"%s/%s%s.%s\">",
                symbolpath, "hyphen", dir, symboltype, brk,
                symbolpath, "hyphen", dir, symboltype, brk,
                symbolpath, "hyphen", dir, symboltype );
  }

  return s;
}

static int __counter = 0;
static int __createItemTD( iOPClient inst, iONode item, int level, const char* selected, int x, int y, const char* symbolpath, const char* symboltype, iOMap extraItems ) {
  iOPClientData data = Data(inst);
  Boolean ok = True;
  char* id = StrOp.encode4URL( wItem.getid(item) );
  /*char* id = StrOp.dup( wItem.getid(item) );*/
  const char* itemname = NodeOp.getName( item );
  char sbuffer[256];
  int colspan = 1;
  int ori = 1; /* 1 = west, 2 = north, 3 = east, 4 = south */

  __counter++;
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "createItemTD for level %d", level );

  if( StrOp.equals( wTrack.getori(item), wItem.west ) )
    ori = 1;
  else if( StrOp.equals( wTrack.getori(item), wItem.north ) )
    ori = 2;
  else if( StrOp.equals( wTrack.getori(item), wItem.east ) )
    ori = 3;
  else if( StrOp.equals( wTrack.getori(item), wItem.south ) )
    ori = 4;


  TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "creating item [%s]", id );

  if( StrOp.equals( itemname, wTrack.name() ) ) {
    if( StrOp.equals( wTrack.gettype(item), wTrack.curve ) ) {
      if(ok) ok=SocketOp.fmt( data->socket, "  <td><img height=\"16\" width=\"16\" border=\"0\" alt=\" + \" src=\"%s/curve-%d.%s\"/></td>\n", symbolpath, ori, symboltype );
    }
    else if( StrOp.equals( wTrack.gettype(item), wTrack.buffer ) ) {
      if( ori == 1 )
        ori = 3;
      else if( ori == 3 )
        ori = 1;
      if(ok) ok=SocketOp.fmt( data->socket, "  <td><img height=\"16\" width=\"16\" border=\"0\" alt=\" # \" src=\"%s/buffer-%d.%s\"/></td>\n", symbolpath, ori, symboltype );
    }
    else {
      if(ok) ok=SocketOp.fmt( data->socket, "  <td><img height=\"16\" width=\"16\" border=\"0\" alt=\"%s\" src=\"%s/track-%d.%s\"/></td>\n", ori % 2 == 0 ? " | ":"---", symbolpath, ori % 2 == 0 ? 2:1, symboltype );
    }
  }
  else if( StrOp.equals( itemname, wSwitch.name() ) ) {
    /* TODO: all type */
    Boolean st = StrOp.equals( wSwitch.getstate(item), wSwitch.straight );
    Boolean dr = StrOp.equals( wSwitch.gettype(item), wSwitch.right );
    if( ori == 1 )
      ori = 3;
    else if( ori == 3 )
      ori = 1;
    if( StrOp.equals( wSwitch.gettype(item), wSwitch.right ) || StrOp.equals( wSwitch.gettype(item), wSwitch.left ) ) {
      if(ok) ok=SocketOp.fmt( data->socket, "  <td><a href=\"?sw-flip=%s&level=%d&cnt=%d&selected=%s\"><img height=\"16\" width=\"16\" border=\"0\" alt=\"=//\" title=\"%s\" src=\"%s/turnout-%c%c-%d.%s\"/></a></td>\n",
                    id, level, __counter, selected, wItem.getid(item), symbolpath, dr?'r':'l', st?'s':'t', ori, symboltype  );
    }
    else if( StrOp.equals( wSwitch.gettype(item), wSwitch.turnout ) ) {
      dr = wSwitch.isdir(item);
      if(ok) ok=SocketOp.fmt( data->socket, "  <td><a href=\"?sw-flip=%s&level=%d&cnt=%d&selected=%s\"><img height=\"16\" width=\"16\" border=\"0\" alt=\"=//\" title=\"%s\" src=\"%s/turnout-%c%c-%d.%s\"/></a></td>\n",
                    id, level, __counter, selected, wItem.getid(item), symbolpath, dr?'r':'l', st?'s':'t', ori, symboltype  );
    }
    else if( StrOp.equals( wSwitch.gettype(item), wSwitch.crossing ) ) {
      if(ok) ok=SocketOp.fmt( data->socket, "  <td><a href=\"?sw-flip=%s&level=%d&cnt=%d&selected=%s\"><img height=\"16\" width=\"16\" border=\"0\" alt=\"=//\" title=\"%s\" src=\"%s/cross.%s\"/></a></td>\n",
                    id, level, __counter, selected, wItem.getid(item), symbolpath, symboltype );
    }
    else if( StrOp.equals( wSwitch.gettype(item), wSwitch.dcrossing ) ) {
      dr = wSwitch.isdir(item);
      colspan = 2;
      if(ok) ok=SocketOp.fmt( data->socket, "  <td colspan=\"2\"><a href=\"?sw-flip=%s&level=%d&cnt=%d&selected=%s\"><img height=\"16\" width=\"32\" border=\"0\" alt=\"=//\" title=\"%s\" src=\"%s/dcross-%c%c-%d.%s\"/></a></td>\n",
                    id, level, __counter, selected, wItem.getid(item), symbolpath, dr?'r':'l', st?'s':'t', ori, symboltype  );
    }
    else if( StrOp.equals( wSwitch.gettype(item), wSwitch.decoupler ) ) {
      if(ok) ok=SocketOp.fmt( data->socket, "  <td><a href=\"?sw-flip=%s&level=%d&cnt=%d&selected=%s\"><img height=\"16\" width=\"16\" border=\"0\" alt=\"=//\" title=\"%s\" src=\"%s/decoupler-%d.%s\"/></a></td>\n",
                    id, level, __counter, selected, wItem.getid(item), symbolpath, ori % 2 == 0 ? 2:1, symboltype  );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unknown sw type [%s][%s] (assume turnout)", id, wSwitch.gettype(item) );
      dr = wSwitch.isdir(item);
      if(ok) ok=SocketOp.fmt( data->socket, "  <td><a href=\"?sw-flip=%s&level=%d&cnt=%d&selected=%s\"><img height=\"16\" width=\"16\" border=\"0\" alt=\"=//\" title=\"%s\" src=\"%s/turnout-%c%c-%d.%s\"/></a></td>\n",
                    id, level, __counter, selected, wItem.getid(item), symbolpath, dr?'r':'l', st?'s':'t', ori, symboltype  );
    }
  }
  else if( StrOp.equals( itemname, wFeedback.name() ) ) {
    char* state = wFeedback.isstate(item) ? "on":"off";
    if(ok) ok=SocketOp.fmt( data->socket, "  <td><img height=\"16\" width=\"16\" border=\"0\" alt=\"===\" title=\"%s\" src=\"%s/sensor-%s-%d.%s\"/></td>\n",
                  id, symbolpath, state, ori % 2 == 0 ? 2:1, symboltype );
  }
  else if( StrOp.equals( itemname, wSignal.name() ) ) {
    char* type = "signal";
    char state = 'r';
    if( StrOp.equals( wSignal.red, wSignal.getstate(item) ) )
      state = 'r';
    else if( StrOp.equals( wSignal.green, wSignal.getstate(item) ) )
      state = 'g';
    else if( StrOp.equals( wSignal.yellow, wSignal.getstate(item) ) )
      state = 'y';
    if( ori == 1 )
      ori = 3;
    else if( ori == 3 )
      ori = 1;
    if( StrOp.equals( wSignal.semaphore, wSignal.gettype(item) ) )
      type = "semaphore";
    if(ok) ok=SocketOp.fmt( data->socket, "  <td><a href=\"?sg-flip=%s&level=%d&cnt=%d&selected=%s\"><img height=\"16\" width=\"16\" border=\"0\" alt=\"===\" src=\"%s/%s-%c-%d.%s\"/></a></td>\n",
                                 id, level, __counter, selected, symbolpath, type, state, ori, symboltype );
  }
  else if( StrOp.equals( itemname, wBlock.name() ) ) {
    int addr = 0;
    const char* locid = wBlock.getlocid(item);

    if( StrOp.equals( wTrack.getori(item), wItem.north ) || StrOp.equals( wTrack.getori(item), wItem.south ) ) {
      /* vertical block */
      char key[32];
      iOExtraItem extraItem = NULL;

      if(ok) ok=SocketOp.fmt( data->socket, "  <td><img height=\"16\" width=\"16\" border=\"0\" alt=\"===\" src=\"%s/block-l-2.%s\"/></td>\n", symbolpath, symboltype );

      StrOp.fmtb( key, "%d,%d", x, y+1 );
      extraItem = allocMem( sizeof( struct extraItem ) );
      extraItem->skip = False;

      if( locid != NULL && StrOp.len(locid) > 0 ) {
        iOLoc loc = ModelOp.getLoc( AppOp.getModel(), locid );
        /*char* normID = LocOp.normalizeID(locid);*/
        char* normID = StrOp.encode4URL(locid);
        if( loc != NULL )
          addr = wLoc.getaddr( LocOp.base.properties(loc) );
        StrOp.fmtb( extraItem->html, "  <td align=\"center\" valign=\"center\" rowspan=\"2\"><a title=\"%s\" href=\"?stopgo=%s&level=%d\">%s</a></td>\n",
                    locid, normID, level, __getAddrImage( sbuffer, addr, "v", symbolpath, symboltype) );
        StrOp.free(normID);
      }
      else
        StrOp.fmtb( extraItem->html, "  <td rowspan=\"2\" align=\"center\" valign=\"center\" title=\"%s\">%s</td>\n",
            wItem.getid(item), __getAddrImage( sbuffer, 0, "v", symbolpath, symboltype) );
      MapOp.put( extraItems, key, (obj)extraItem );

      StrOp.fmtb( key, "%d,%d", x, y+2 );
      extraItem = allocMem( sizeof( struct extraItem ) );
      extraItem->skip = True;
      MapOp.put( extraItems, key, (obj)extraItem );

      StrOp.fmtb( key, "%d,%d", x, y+3 );
      extraItem = allocMem( sizeof( struct extraItem ) );
      extraItem->skip = False;
      StrOp.fmtb( extraItem->html, "  <td><img height=\"16\" width=\"16\" border=\"0\" alt=\"===\" src=\"%s/block-r-2.%s\"/></td>\n", symbolpath, symboltype );
      MapOp.put( extraItems, key, (obj)extraItem );
    }
    else {
      colspan = 4;
      if(ok) ok=SocketOp.fmt( data->socket, "  <td><img height=\"16\" width=\"16\" border=\"0\" alt=\"===\" src=\"%s/block-l-1.%s\"/></td>\n", symbolpath, symboltype );
      if( locid != NULL && StrOp.len(locid) > 0 ) {
        iOLoc loc = ModelOp.getLoc( AppOp.getModel(), locid );
        char* normID = LocOp.normalizeID(locid);
        if( loc != NULL )
          addr = wLoc.getaddr( LocOp.base.properties(loc) );
        if(ok) ok=SocketOp.fmt( data->socket, "  <td colspan=\"2\" align=\"center\"><a title=\"%s\" href=\"?stopgo=%s&level=%d\">%s</a></td>\n",
                      locid, normID, level, __getAddrImage( sbuffer, addr, "h", symbolpath, symboltype) );
      }
      else {
        if(ok) ok=SocketOp.fmt( data->socket, "  <td colspan=\"2\" align=\"center\" title=\"%s\">%s</td>\n",
            wItem.getid(item), __getAddrImage( sbuffer, 0, "h", symbolpath, symboltype) );
      }
      if(ok) ok=SocketOp.fmt( data->socket, "  <td><img height=\"16\" width=\"16\" border=\"0\" alt=\"===\" src=\"%s/block-r-1.%s\"/></td>\n", symbolpath, symboltype );
    }

  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unsupported item [%s][%s]", wItem.getid(item), NodeOp.getName(item) );
    if(ok) ok=SocketOp.fmt( data->socket, "  <td></td>\n" );
  }
  StrOp.free(id);
  return colspan;
}


static Boolean __generateLayout( iOPClient inst, int level, const char* selected, const char* symbolpath, const char* symboltype ) {
  iOPClientData data = Data(inst);
  Boolean ok = True;
  int cx, cy, x, y, idx, size, ix, iy = 0;

  /* list sorted by location */
  iOList list = ModelOp.getLevelItems( AppOp.getModel(), level, &cx, &cy, False );
  iOMap extraItems = MapOp.inst();
  iONode item = NULL;
  size = ListOp.size( list );
  idx = 0;

  TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "generateLayout for level %d", level );

  TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "got %d items for level %d plansize=%d,%d", size, level, cx, cy );

  if( idx < size ) {
    item = (iONode)ListOp.get( list, idx );
    ix = wItem.getx( item );
    iy = wItem.gety( item );
    idx++;
    TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "item on level %d at %d,%d [%d,%d]", level, ix, iy, x, y );
  }

  for( y = 0; y <= cy && ok; y++ ) {
    /* start table row: */
    if(ok) ok=SocketOp.fmt( data->socket, "<tr>\n" );

    for( x = 0; x <= cx && ok; x++ ) {
      char key[32];
      iOExtraItem extraItem = NULL;
      StrOp.fmtb(key,"%d,%d",x,y);
      extraItem = (iOExtraItem)MapOp.get( extraItems, key );
      if( extraItem != NULL ) {
        if( extraItem->skip )
          continue;
        if(ok) ok=SocketOp.fmt( data->socket, extraItem->html );
        MapOp.remove( extraItems, key );
        freeMem( extraItem );
        continue;
      }

      if( item != NULL && ix == x && iy == y && ok) {
        /* create an item table data section: */
        int colspan = __createItemTD( inst, item, level, selected, x, y, symbolpath, symboltype, extraItems );
        x = x + colspan-1;

        item = NULL;
        /* get the next item: */
        do {
          if( idx < size ) {
            int trclevel = TRCLEVEL_USER2;
            item = (iONode)ListOp.get( list, idx );
            ix = wItem.getx( item );
            iy = wItem.gety( item );
            idx++;
            if( ix == x && iy == y )
              trclevel = TRCLEVEL_WARNING;
            TraceOp.trc( name, trclevel, __LINE__, 9999, "duplicate item %s on level %d at %d,%d [%d,%d]", wItem.getid(item), level, ix, iy, x, y );
          }
        } while( idx < size && ix == x && iy == y && ok);
      }
      else {
        /* create an empty table data section: */
        if(ok) ok=SocketOp.fmt( data->socket, "  <td></td>\n" );
      }
    }

    /* end table row: */
    if(ok) ok=SocketOp.fmt( data->socket, "</tr>\n" );

  }

  MapOp.base.del( extraItems );
  TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "generateLayout for level %d ready", level );
  return ok;
}


static Boolean __doWebCamBottom(iOPClient inst) {
  iOPClientData data = Data(inst);
  Boolean ok = True;
  iONode webcam = wWebClient.getwebcam( data->ini );
  TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "doWebCamBottom" );
  if( webcam != NULL ) {
    if(ok) ok=SocketOp.fmt( data->socket, "<br><table border=\"1\" rules=\"cols\" cellspacing=\"0\" cellpadding=\"5\">\n  <tr>\n" );
    while( webcam != NULL && ok) {
      if(ok) ok=SocketOp.fmt( data->socket, "    <td>%s<br><img src=\"%s/%s\"></td>\n",
                    wWebCam.gettitle( webcam ), wWebClient.getwebcampath( data->ini ), wWebCam.getimagefile( webcam ) );
      webcam = wWebClient.nextwebcam( data->ini, webcam );
    }

    if(ok) ok=SocketOp.fmt( data->socket, "  </tr>\n</table>\n" );

  }
  return ok;
}



static Boolean __doWebCamRight(iOPClient inst) {
  iOPClientData data = Data(inst);
  Boolean ok = True;
  iONode webcam = wWebClient.getwebcam( data->ini );
  TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "doWebCamRight" );
  if( webcam != NULL ) {
    if(ok) ok=SocketOp.fmt( data->socket, "<table border=\"1\" rules=\"cols\" cellspacing=\"0\" cellpadding=\"5\">\n\n" );
    while( webcam != NULL && ok) {
      if(ok) ok=SocketOp.fmt( data->socket, "    <tr><td>%s<br><img src=\"%s/%s\"></td></tr>\n",
                    wWebCam.gettitle( webcam ), wWebClient.getwebcampath( data->ini ), wWebCam.getimagefile( webcam ) );
      webcam = wWebClient.nextwebcam( data->ini, webcam );
    }

    if(ok) ok=SocketOp.fmt( data->socket, "</table>\n" );

  }
  return ok;
}


static Boolean __getHome( iOPClient inst, int level, const char* selected, Boolean direction ) {
  Boolean ok = True;
  TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "home for level %d", level );
  if( inst != NULL ) {
    iOPClientData data = Data(inst);
    const char* symbolpath = wWebClient.getsymbolpath( data->ini );
    const char* symboltype = wWebClient.getsymboltype( data->ini );
    const char* light = "off";
    int V = 0;
    const char* V_mode = "";

    if( selected != NULL && StrOp.len(selected) > 0 ) {
      iOLoc loc = ModelOp.getLoc( AppOp.getModel(), selected );
      if( loc != NULL ) {
        iONode lc = LocOp.base.properties(loc);
        light = wLoc.isfn(lc)?"on":"off";
        V = wLoc.getV(lc);
        if( V < 0 )
          V = 0;
        V_mode = wLoc.getV_mode(lc);
        if( StrOp.equals( V_mode, wLoc.V_mode_percent ) )
          V_mode = "%";
      }
    }

    if(ok) ok= webHeader( data->socket );

    if( !StrOp.equals( wWebClient.getwebcampos( data->ini ), wWebClient.bottom ) ) {
      if(ok) ok=SocketOp.fmt( data->socket, "\n<!-- WebCam on the right -->\n<table><tr><td valign=\"top\">\n\n" );
    }

    {
      int cx = wWebClient.getxsize( data->ini );
      int cy = wWebClient.getysize( data->ini );
      char strX[64] = {'\0'};
      char strY[64] = {'\0'};
      if( cx > 0 )
        StrOp.fmtb( strX, "width=\"%d\"", cx );
      if( cy > 0 )
        StrOp.fmtb( strY, "height=\"%d\"", cy );
      if(ok) ok=SocketOp.fmt( data->socket, "<table bgcolor=\"%s\" border=\"1\" rules=\"cols\" %s %s cellspacing=\"0\" cellpadding=\"0\">\n",
                    wWebClient.getplanbackground(data->ini), strX, strY );
    }
    if(ok) ok=SocketOp.fmt( data->socket, "<tr vvalign=\"top\"><td>" );

    if(ok) ok=SocketOp.fmt( data->socket, "<form method=\"get\" action=\"\">" );
    if(ok) ok=SocketOp.fmt( data->socket, "<table bgcolor=\"%s\" cellspacing=\"0\" rules=\"rows\" cellpadding=\"1\" border=\"1\" width=\"100%\"><tr>",
                  wWebClient.getrulerbackground(data->ini) );

    if(ok) ok=SocketOp.fmt( data->socket, "<td><input type=\"hidden\" name=\"level\" value=\"%d\"></td>", level );
    if(ok) ok=SocketOp.fmt( data->socket, "<td><input type=\"image\" title=\"Level Down\" src=\"%s/arrow-down.%s\" name=\"leveldown\"></td>", symbolpath, symboltype );
    if(ok) ok=SocketOp.fmt( data->socket, "<td><input type=\"image\" title=\"Level Up\" src=\"%s/arrow-up.%s\" name=\"levelup\"></td>", symbolpath, symboltype );

    if(ok) ok=SocketOp.fmt( data->socket, "<td><input type=\"image\" title=\"Power ON\" src=\"%s/power-on.%s\" name=\"poweron\"></td>", symbolpath, symboltype );
    if(ok) ok=SocketOp.fmt( data->socket, "<td><input type=\"image\" title=\"Power OFF\" src=\"%s/power-off.%s\" name=\"poweroff\"></td>", symbolpath, symboltype );
    if(ok) ok=SocketOp.fmt( data->socket, "<td><input type=\"image\" title=\"Automat\" src=\"%s/auto.%s\" name=\"autoon\"></td>", symbolpath, symboltype );

    if(ok) ok=SocketOp.fmt( data->socket, "<td><input type=\"hidden\" name=\"selected\" value=\"%s\"></td>", selected );
    if(ok) ok=SocketOp.fmt( data->socket, "<td aalign=\"center\"><select name=\"locsel\">" );
    if(ok) ok=SocketOp.fmt( data->socket, "  <option>%s</option>", "-" );
    {
      int i;
      int n = ListOp.size( data->locIDs );
      for( i = 0; i < n; i++ ) {
        const char* id = (const char*)ListOp.get(data->locIDs, i);
        char* encID = StrOp.encode4URL(id);
        /*char* encID = StrOp.dup(id);*/
        Boolean bselected = False;
        if( selected != NULL && StrOp.len(selected) > 0 )
          bselected = StrOp.equals( id, selected );
        if(ok) ok=SocketOp.fmt( data->socket, "  <option value=\"%s\" %s>%s</option>", encID, bselected?"selected":"", id );
        StrOp.free( encID );
      }
    }
    if(ok) ok=SocketOp.fmt( data->socket, "</select></td>" );

/*    SocketOp.fmt( data->socket, "<td><input type=\"image\" src=\"%s/enter.png\" value=\"enter;\" name=\"enter\"></td>", symbolpath );*/
    if(ok) ok=SocketOp.fmt( data->socket, "<td><small>%d%s</small></td>", V, V_mode );
    if(ok) ok=SocketOp.fmt( data->socket, "<td><input type=\"image\" src=\"%s/ric.%s\" name=\"ricselect\"></td>", symbolpath, symboltype );
    if(ok) ok=SocketOp.fmt( data->socket, "<td><input type=\"image\" src=\"%s/arrow-left.%s\" name=\"speeddown\"></td>", symbolpath, symboltype );
    if(ok) ok=SocketOp.fmt( data->socket, "<td><input type=\"image\" src=\"%s/stop.%s\" name=\"speedstop\"></td>", symbolpath, symboltype );
    if(ok) ok=SocketOp.fmt( data->socket, "<td><input type=\"image\" src=\"%s/arrow-right.%s\" name=\"speedup\"></td>", symbolpath, symboltype );
    if(ok) ok=SocketOp.fmt( data->socket, "<td><input type=\"image\" src=\"%s/direction.%s\" name=\"dirchange\"></td>", symbolpath, symboltype );
    if(ok) ok=SocketOp.fmt( data->socket, "<td><input type=\"hidden\" name=\"direction\" value=\"%s\"></td>", direction?"true":"false" );
    if(ok) ok=SocketOp.fmt( data->socket, "<td><input type=\"hidden\" name=\"selected\" value=\"%s\"></td>", selected );

    if(ok) ok=SocketOp.fmt( data->socket, "<td><input type=\"image\" src=\"%s/light-%s.%s\" name=\"function0\"></td>", symbolpath, light, symboltype );
    if(ok) ok=SocketOp.fmt( data->socket, "<td><input type=\"image\" src=\"%s/f1.%s\" name=\"function1\"></td>", symbolpath, symboltype );
    if(ok) ok=SocketOp.fmt( data->socket, "<td><input type=\"image\" src=\"%s/f2.%s\" name=\"function2\"></td>", symbolpath, symboltype );
    if(ok) ok=SocketOp.fmt( data->socket, "<td><input type=\"image\" src=\"%s/f3.%s\" name=\"function3\"></td>", symbolpath, symboltype );
    if(ok) ok=SocketOp.fmt( data->socket, "<td><input type=\"image\" src=\"%s/f4.%s\" name=\"function4\"></td>", symbolpath, symboltype );

    if(ok) ok=SocketOp.fmt( data->socket, "</tr>" );
    if(ok) ok=SocketOp.fmt( data->socket, "</table></form>" );

    if(ok) ok=SocketOp.fmt( data->socket, "</td></tr>" );

    if(ok) ok=SocketOp.fmt( data->socket, "<tr valign=\"top\" bgcolor=\"%s\"><td>\n",
                  wWebClient.getplanbackground(data->ini) );
    if(ok) ok=SocketOp.fmt( data->socket, "<table cellspacing=\"0\" cellpadding=\"0\" bgcolor=\"%s\">\n",
                  wWebClient.getplanbackground(data->ini) );

    if(ok) ok=__generateLayout( inst, level, selected, symbolpath, symboltype );

    if(ok) ok=SocketOp.fmt( data->socket, "</table>\n" );
    if(ok) ok=SocketOp.fmt( data->socket, "</td></tr>\n" );
    if(ok) ok=SocketOp.fmt( data->socket, "</table>\n" );

    if( StrOp.equals( wWebClient.getwebcampos( data->ini ), wWebClient.bottom ) )
      if(ok) ok=__doWebCamBottom( inst );
    else {
      if(ok) ok=SocketOp.fmt( data->socket, "\n\n<!-- WebCam on the right --></td><td valign=\"top\">\n" );
      if(ok) ok=__doWebCamRight( inst );
      if(ok) ok=SocketOp.fmt( data->socket, "</td></tr></table>\n\n" );
    }

    if(ok) ok= webFooter( data->socket );
  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL!" );
    return ok;
  }
  return ok;
}


static Boolean __getSymbol( iOPClient inst, const char* symbolfile ) {
  iOPClientData data = Data(inst);
  Boolean ok = True;
  if( FileOp.exist( symbolfile ) ) {
    long size = FileOp.fileSize( symbolfile );
    char* sym = allocMem( size + 1 );
    iOFile f = FileOp.inst( symbolfile, OPEN_READONLY );
    if( f != NULL ) {
      char* type = "png";
      if( StrOp.endsWithi(symbolfile,"gif") )
        type = "gif";
      else if( StrOp.endsWithi(symbolfile,"jpg") || StrOp.endsWithi(symbolfile,"jpeg") )
        type = "jpeg";
      __httpHeader4Image(data->socket, type);
      FileOp.read( f, sym, size );
      FileOp.base.del( f );
      TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "write symbol %d", size );
      if(ok) ok=SocketOp.write( data->socket, (char*)sym, size );
    }
    freeMem( sym );
  }
  return ok;
}


static char* __getID( const char* offset, const char* str ) {
  char* idp = StrOp.find( str, offset );
  if( idp != NULL ) {
    char* decodedID = NULL;
    char* nid = StrOp.dup( idp + StrOp.len(offset) );
    char* p = StrOp.findc( nid, '&' );
    if( p == NULL )
      p = StrOp.findc( nid, ' ' );
    *p = '\0';
    p = nid;
    while( *p != '\0' ) {
      if( *p == '_' )
        *p = ' ';
      p++;
    };

    decodedID = StrOp.decode4URL(nid);
    StrOp.free(nid);
    return decodedID;

    /*return nid;*/
  }
  return NULL;
}


static int __getLevel( const char* str ) {
  int level = 0;
  char* lp = StrOp.find( str, "level=" );

  if( lp != NULL ) {
    char* nid = StrOp.dup( lp + StrOp.len("level=") );
    char* p = StrOp.findc( nid, '&' );
    if( p == NULL )
      p = StrOp.findc( nid, ' ' );
    *p = '\0';
    p = nid;
    while( *p != '\0' ) {
      if( *p == '_' )
        *p = ' ';
      p++;
    };
    level = atoi(nid);
    StrOp.free(nid);
  }
  return level;
}


static void __doPower( iOPClient inst, const char* req ) {
  iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
  if( StrOp.find( req, "poweron" ) )
    wSysCmd.setcmd( cmd, wSysCmd.go );
  else
    wSysCmd.setcmd( cmd, wSysCmd.stop );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "power on" );
  ControlOp.cmd( AppOp.getControl(), cmd, NULL );
}


static void __doAuto( iOPClient inst, const char* req ) {
  iONode cmd = NodeOp.inst( wAutoCmd.name(), NULL, ELEMENT_NODE );
  if( StrOp.find( req, "autoon" ) )
    wAutoCmd.setcmd( cmd, wAutoCmd.on );
  else
    wAutoCmd.setcmd( cmd, wAutoCmd.off );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "auto on" );
  ControlOp.cmd( AppOp.getControl(), cmd, NULL );
}


static void __doSW( iOPClient inst, const char* req ) {
  char* id = __getID( "sw-flip=", req );
  iOSwitch sw = ModelOp.getSwitch( AppOp.getModel(), id );
  if( sw != NULL ) {
    iONode cmd = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
    wSwitch.setid( cmd, id );
    wSwitch.setcmd( cmd, wSwitch.flip );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "flip switch ID [%s]", id );
    SwitchOp.cmd( sw, cmd, True, NULL );
  }
  else
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unknown switch ID [%s]", id );

  StrOp.free( id );

}


static void __doStopGo( iOPClient inst, const char* req ) {
  char* id = __getID( "stopgo=", req );
  iOLoc loc = ModelOp.getLoc( AppOp.getModel(), id );
  if( loc != NULL ) {
    iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    wLoc.setid( cmd, id );
    wLoc.setcmd( cmd, wLoc.stopgo );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "stopgo loc ID [%s]", id );
    LocOp.cmd( loc, cmd );
  }
  else
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unknown loc ID [%s]", id );

  StrOp.free( id );

}


static void __doSG( iOPClient inst, const char* req ) {
}


static void __doFunction( iOLoc loc, const char* req ) {
  const char* id = LocOp.getId(loc);

  if( StrOp.find(req, "function0") ) {
    iONode props = LocOp.base.properties(loc);
    iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    wLoc.setid( cmd, id );
    wLoc.setfn( cmd, !wLoc.isfn(props) );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "flip loc lights ID [%s]", id );
    LocOp.cmd( loc, cmd );
  }
  else if( StrOp.find(req, "function1") ) {
    iONode cmd = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
    wFunCmd.setid( cmd, id );
    wFunCmd.setf1( cmd, True );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set loc f1 ID [%s]", id );
    LocOp.cmd( loc, cmd );
  }
  else if( StrOp.find(req, "function2") ) {
    iONode cmd = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
    wFunCmd.setid( cmd, id );
    wFunCmd.setf2( cmd, True );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set loc f2 ID [%s]", id );
    LocOp.cmd( loc, cmd );
  }
  else if( StrOp.find(req, "function3") ) {
    iONode cmd = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
    wFunCmd.setid( cmd, id );
    wFunCmd.setf3( cmd, True );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set loc f3 ID [%s]", id );
    LocOp.cmd( loc, cmd );
  }
  else if( StrOp.find(req, "function4") ) {
    iONode cmd = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
    wFunCmd.setid( cmd, id );
    wFunCmd.setf4( cmd, True );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set loc f4 ID [%s]", id );
    LocOp.cmd( loc, cmd );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unknown function %s ID [%s]", req, id );
  }
}


static void __doSpeed( iOLoc loc, const char* req, Boolean direction ) {
  const char* id = LocOp.getId(loc);

  if( StrOp.find(req, "speedstop") ) {
    iONode props = LocOp.base.properties(loc);
    iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    wLoc.setid( cmd, id );
    wLoc.setV( cmd, 0 );
    wLoc.setdir( cmd, direction );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "V loc 0 ID [%s]", id );
    LocOp.cmd( loc, cmd );
  }
  else if( StrOp.find(req, "speedup") ) {
    iONode props = LocOp.base.properties(loc);
    iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    wLoc.setid( cmd, id );
    wLoc.setV_hint( cmd, wLoc.cruise );
    wLoc.setdir( cmd, direction );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "V loc cruise ID [%s]", id );
    LocOp.cmd( loc, cmd );
  }
  else if( StrOp.find(req, "speeddown") ) {
    iONode props = LocOp.base.properties(loc);
    iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    wLoc.setid( cmd, id );
    wLoc.setV_hint( cmd, wLoc.min );
    wLoc.setdir( cmd, direction );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "V loc min ID [%s]", id );
    LocOp.cmd( loc, cmd );
  }
  else if( StrOp.find(req, "dirchange") ) {
    iONode props = LocOp.base.properties(loc);
    iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    wLoc.setid( cmd, id );
    wLoc.setV( cmd, 0 );
    wLoc.setdir( cmd, direction );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "V loc direction ID [%s]", id );
    LocOp.cmd( loc, cmd );
  }
  else if( StrOp.find(req, "ricselect") ) {
    iONode props = LocOp.base.properties(loc);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "### ToDo: RIC select for ID [%s]", id );
  }
}


static void __doLC( iOPClient inst, const char* req, const char* selected, Boolean direction ) {
  iOPClientData data = Data(inst);
  if( selected != NULL ) {
    iOLoc loc = ModelOp.getLoc( AppOp.getModel(), selected );
    if( loc != NULL ) {
      __doFunction( loc, req );
      __doSpeed( loc, req, direction );
    }
  }
}


static int __doLevel( iOPClient inst, const char* req ) {
  int level = __getLevel( req );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "current level=%d", level );

  if( StrOp.find( req, "levelup" ) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "new level=%d", level, level+1 );
    return level + 1;
  }
  else if( StrOp.find( req, "leveldown" ) && level > 0 ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "new level=%d", level, level-1 );
    return level - 1;
  }
  return level;
}


typedef void(*postcall)(iOPClient,const char*,const char*);
/** Work slice. */
static Boolean _work( struct OPClient* inst ) {
  if( inst != NULL ) {
    iOPClientData data = Data(inst);
    char str[1025] = {'\0'};
    char postdata[1025] = {'\0'};
    int contlen = 0;
    Boolean readPost = False;
    char *p = postdata;
    postcall pc = NULL;
    char* postid = NULL;
    char* selected = NULL;
    const char* direction = "true";
    Boolean bdirection = True;

    SocketOp.setRcvTimeout( data->socket, 1000 );
    /* Read first HTTP header line: */
    if( !SocketOp.readln( data->socket, str ) ) {
      SocketOp.disConnect( data->socket );
      return True;
    }
    TraceOp.dump( name, TRCLEVEL_USER2, str, StrOp.len(str) );


    /* Flag if post data is comming: */
    if( StrOp.find( str, "POST" ) )
      readPost = True;

    if( StrOp.find( str, "GET" ) && StrOp.find( str, "/rocrail.gif" ) ) {
      __getFavicon( inst );
    }
    else if( StrOp.find( str, "GET" ) && StrOp.find( str, "/favicon.ico" ) ) {
      __getFavicon( inst );
    }
    else if( StrOp.find( str, "GET" ) && StrOp.find( str, wWebClient.getsymbolpath( data->ini ) ) && StrOp.find( str, " /" ) ||
             StrOp.find( str, "GET" ) && StrOp.find( str, wWebClient.getwebcampath( data->ini ) ) && StrOp.find( str, " /" ) ||
             StrOp.find( str, "GET" ) && StrOp.find( str, "images" ) && StrOp.find( str, " /" ) )
    {
      char* symbolfile = StrOp.dup( StrOp.find( str, " /" ) + 2 ) ;
      char* p = StrOp.find( symbolfile, "HTTP" );

      if( p != NULL ) {
        p--;
        *p = '\0';
        __getSymbol( inst, symbolfile );
      }
      StrOp.free( symbolfile );
    }
    else if( wWebClient.isme( data->ini ) ) {
      rocWebME( inst, str );
    }
    else {

      selected = __getID( "locsel=", str );
      if( selected == NULL )
        selected = __getID( "selected=", str );

      direction = __getID( "direction=", str );
      if( direction == NULL )
        direction = "true";

      bdirection = StrOp.equals( direction, "true" );

      if( StrOp.find( str, "GET" ) && StrOp.find( str, " / " ) ) {
        __getHome( inst, __getLevel(str), selected, bdirection );
      }
      else if( StrOp.find( str, "GET" ) && StrOp.find( str, "levelup" ) ) {
        __getHome( inst, __doLevel( inst, str ), selected, bdirection );
      }
      else if( StrOp.find( str, "GET" ) && StrOp.find( str, "leveldown" ) ) {
        __getHome( inst, __doLevel( inst, str ), selected, bdirection );
      }
      else if( StrOp.find( str, "GET" ) && StrOp.find( str, "poweron" ) ) {
        __doPower( inst, str );
        __getHome( inst, __getLevel(str), selected, bdirection );
      }
      else if( StrOp.find( str, "GET" ) && StrOp.find( str, "poweroff" ) ) {
        __doPower( inst, str );
        __getHome( inst, __getLevel(str), selected, bdirection );
      }
      else if( StrOp.find( str, "GET" ) && StrOp.find( str, "autoon" ) ) {
        __doAuto( inst, str );
        __getHome( inst, __getLevel(str), selected, bdirection );
      }
      else if( StrOp.find( str, "GET" ) && StrOp.find( str, "dirchange" ) ) {
        bdirection = !bdirection;
        __doLC( inst, str, selected, bdirection );
        __getHome( inst, __getLevel(str), selected, bdirection );
      }
      else if( StrOp.find( str, "GET" ) && StrOp.find( str, " /?sw-flip" ) ) {
        __doSW( inst, str );
        __getHome( inst, __getLevel(str), selected, bdirection );
      }
      else if( StrOp.find( str, "GET" ) && StrOp.find( str, " /?sg-flip" ) ) {
        __doSG( inst, str );
        __getHome( inst, __getLevel(str), selected, bdirection );
      }
      else if( StrOp.find( str, "GET" ) && StrOp.find( str, " /?stopgo" ) ) {
        __doStopGo( inst, str );
        __getHome( inst, __getLevel(str), selected, bdirection );
      }
      else {
        __doLC( inst, str, selected, bdirection );
        __getHome( inst, __getLevel(str), selected, bdirection );
      }

    }

    /* Reading rest of HTTP header: */
    TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "Reading rest of HTTP header... " );
    while( SocketOp.readln( data->socket, str ) && !SocketOp.isBroken( data->socket ) ) {
      if( str[0] == '\r' || str[0] == '\n' ) {
        break;
      }
      if( StrOp.find( str, "Content-Length:" ) ) {
        char* p = StrOp.find( str, ": " ) + 2;
        contlen = atoi( p );
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "contlen = %d\n", contlen );
      }
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, str );
    };
    if( SocketOp.isBroken( data->socket ) )
      return True;

    if( readPost ) {
      /* Read the post data: */
      TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "Read the post data... " );
      while( SocketOp.peek( data->socket, p, 1 ) && !SocketOp.isBroken( data->socket ) ) {
        if( !SocketOp.read( data->socket, p, 1 ) )
          break;
        /*printf( "%c", *p );*/
        p++;
        *p = '\0';
      };
      if( SocketOp.isBroken( data->socket ) )
        return True;
      TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "postdata=\"%s\"\n", postdata );
      /* Call the post data handler: */
      if( pc != NULL )
        pc( inst, postid, postdata );
    }

    StrOp.free( selected );
    StrOp.free( postid );
    /*if( SocketOp.isBroken( data->socket ) ) {*/
    TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "disconnect... " );
    SocketOp.disConnect( data->socket );
/*      return True;
    }*/
  }

  return True;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/pclient.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
