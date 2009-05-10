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
// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/event.h"
#endif

#ifdef __linux__
#include <sys/resource.h>
#endif

#include "rocs/public/str.h"
#include "rocs/public/doc.h"
#include "rocs/public/node.h"
#include "rocs/public/js.h"

#include "rocgui/public/guiapp.h"

#include "rocgui/dialogs/locseldlg.h"
#include "rocgui/dialogs/loccontroldlg.h"

#include "rocgui/public/jssupport.h"

#include "rocgui/wrapper/public/Gui.h"

#include "rocrail/wrapper/public/JsEvent.h"
#include "rocrail/wrapper/public/JsMap.h"

#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/SysCmd.h"

BEGIN_EVENT_TABLE(JsSupport, wxEvtHandler)
    EVT_MENU( JSSUPPORT_EVENT, JsSupport::OnJsEvent)
END_EVENT_TABLE()

static void myjsListener( int device, int type, int number, int value, unsigned long msec );
static void jsRepeater( void* threadinst );

static JsSupport* __inst = NULL;

JsSupport* JsSupport::inst( iONode ini ) {
  if( __inst == NULL ) {
    __inst = new JsSupport( ini );
  }
  return __inst;
}

int JsSupport::getDev4ID( const char* locID) {
  int i = 0;
  for( i = 0; i < 4; i++ ) {
    if( m_Selected[i] != NULL && StrOp.equals( locID, wLoc.getid( m_Selected[i] ) ) ) {
      return i;
    }
  }
  return -1;
}


void JsSupport::funCmd( int device )
{
  iONode cmd = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
  wFunCmd.setid( cmd, wLoc.getid(m_Selected[device]) );
  wFunCmd.setf1( cmd, m_bF1[device]?True:False );
  wFunCmd.setf2( cmd, m_bF2[device]?True:False );
  wFunCmd.setf3( cmd, m_bF3[device]?True:False );
  wFunCmd.setf4( cmd, m_bF4[device]?True:False );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void JsSupport::speedCmd( int device )
{
  iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  wLoc.setid( cmd, wLoc.getid(m_Selected[device]) );
  wLoc.setV( cmd, wLoc.getV(m_Selected[device]) );
  wLoc.setfn( cmd, wLoc.isfn(m_Selected[device]) );
  wLoc.setdir( cmd, wLoc.isdir(m_Selected[device]) );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

JsSupport::JsSupport( iONode ini ) {
  m_LocDialog[0] = NULL;
  m_LocDialog[1] = NULL;
  m_LocDialog[2] = NULL;
  m_LocDialog[3] = NULL;
  m_Selected[0] = NULL;
  m_Selected[1] = NULL;
  m_Selected[2] = NULL;
  m_Selected[3] = NULL;
  m_Ini = ini;
  initJS();
}

void JsSupport::initJS() {
  int map = 0;
  iOJS js = JSOp.inst();
  JSOp.setListener( js, &myjsListener, 0 );
  JSOp.setListener( js, &myjsListener, 1 );
  JSOp.setListener( js, &myjsListener, 2 );
  JSOp.setListener( js, &myjsListener, 3 );
  JSOp.init(js, &map);
  JSOp.start( js );
  m_bRun = true;
  repeater = ThreadOp.inst("jsRepeater", &jsRepeater, this );
  ThreadOp.start( repeater );
}


void JsSupport::OnJsEvent(wxCommandEvent& event) {
  iONode ini = wGui.getjsmap( wxGetApp().getIni() );
  iONode js = (iONode)event.GetClientData();
  int device = wJsEvent.getdevice(js);
  int type = wJsEvent.gettype(js);
  int number = wJsEvent.getnumber(js);
  int value = wJsEvent.getvalue(js);

  if( device < 0 || device > 3 ) {
    NodeOp.base.del(js);
    return;
  }

  TraceOp.trc( "js", TRCLEVEL_INFO, __LINE__, 9999,
    "JsEvent dev=%d, type=%d, number=%d, value=%d, msec=%d",
    device, type, number, value, wJsEvent.getmsec(js) );

  if( type == 1 ) {
    // buttons
    if( number == wJsMap.getselect( ini ) ) {
        if( value == 1 ) {
          TraceOp.trc( "js", TRCLEVEL_INFO, __LINE__, 9999, "select" );
          // select
          if( m_LocDialog[device] == NULL ) {
            m_LocDialog[device] = new LocSelDlg(wxGetApp().getFrame(), m_Selected[device], false );
            m_LocDialog[device]->Show();
          }
          else {
            if( m_Selected[device] != NULL ) {
              TraceOp.trc( "js", TRCLEVEL_INFO, __LINE__, 9999, "Deselected: %s", wLoc.getid(m_Selected[device]) );

              iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
              wLoc.setid( cmd, wLoc.getid(m_Selected[device]) );
              wxGetApp().sendToRocrail( cmd );
              cmd->base.del(cmd);
            }
            m_Selected[device] = m_LocDialog[device]->getProperties();
            m_LocDialog[device]->Destroy();
            m_LocDialog[device] = NULL;
            if( m_Selected[device] != NULL ) {
              TraceOp.trc( "js", TRCLEVEL_INFO, __LINE__, 9999, "Selected: %s", wLoc.getid(m_Selected[device]) );

              iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
              wLoc.setid( cmd, wLoc.getid(m_Selected[device]) );
              wxGetApp().sendToRocrail( cmd );
              cmd->base.del(cmd);
            }
          }

        }
    }
    else if( number == wJsMap.getpoweroff( ini ) ) {
        if( value == 1 ) {
          TraceOp.trc( "js", TRCLEVEL_INFO, __LINE__, 9999, "poweroff" );
          // poweroff
          iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
          wSysCmd.setcmd( cmd, wSysCmd.stop );
          wxGetApp().sendToRocrail( cmd );
          cmd->base.del(cmd);
        }
    }
    else if( number == wJsMap.getpoweron( ini ) ) {
        if( value == 1 ) {
          TraceOp.trc( "js", TRCLEVEL_INFO, __LINE__, 9999, "poweron" );
          // poweron
          iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
          wSysCmd.setcmd( cmd, wSysCmd.go );
          wxGetApp().sendToRocrail( cmd );
          cmd->base.del(cmd);
        }
    }

    if(  m_Selected[device] != NULL ) {

      if( number == wJsMap.getf1( ini ) ) {
          if( value == 1 ) {
            TraceOp.trc( "js", TRCLEVEL_INFO, __LINE__, 9999, "f1" );
            // f1
            m_bF1[device] = ! m_bF1[device];
            funCmd( device );
          }
      }
      else if( number == wJsMap.getf2( ini ) ) {
          if( value == 1 ) {
            TraceOp.trc( "js", TRCLEVEL_INFO, __LINE__, 9999, "f2" );
            // f2
            m_bF2[device] = ! m_bF2[device];
            funCmd( device );
          }
      }
      else if( number == wJsMap.getf3( ini ) ) {
          if( value == 1 ) {
            TraceOp.trc( "js", TRCLEVEL_INFO, __LINE__, 9999, "f3" );
            // f3
            m_bF3[device] = ! m_bF3[device];
            funCmd( device );
          }
      }
      else if( number == wJsMap.getf4( ini ) ) {
          if( value == 1 ) {
            TraceOp.trc( "js", TRCLEVEL_INFO, __LINE__, 9999, "f4" );
            // f4
            m_bF4[device] = ! m_bF4[device];
            funCmd( device );
          }
      }
      else if( number == wJsMap.getreverse( ini ) ) {
          if( value == 1 ) {
            TraceOp.trc( "js", TRCLEVEL_INFO, __LINE__, 9999, "reverse" );
            // reverse
            wLoc.setdir(m_Selected[device], wLoc.isdir(m_Selected[device])==True ? False:True );
            speedCmd( device );
          }
      }
      else if( number == wJsMap.getlight( ini ) ) {
          if( value == 1 ) {
            TraceOp.trc( "js", TRCLEVEL_INFO, __LINE__, 9999, "light" );
            // light
            wLoc.setfn(m_Selected[device], wLoc.isfn(m_Selected[device])==True ? False:True );
            speedCmd( device );
          }
      }
      else if( number == wJsMap.getstop( ini ) ) {
          if( value == 1 ) {
            TraceOp.trc( "js", TRCLEVEL_INFO, __LINE__, 9999, "stop" );
            // stop
            wLoc.setV( m_Selected[device], 0 );
            speedCmd( device );
          }
      }
    }

  }

  if( type == 2 ) {
    // axis
    if( number == wJsMap.getleftright( ini ) ) {
        if( value > 0 ) {
          TraceOp.trc( "js", TRCLEVEL_INFO, __LINE__, 9999, "right" );
          // right
        }
        else if( value < 0 ) {
          TraceOp.trc( "js", TRCLEVEL_INFO, __LINE__, 9999, "left" );
          // left
        }
    }
    else if( number == wJsMap.getupdown( ini ) ) {
      int step = wJsMap.getstep( ini );
        if( value > 0 ) {
          TraceOp.trc( "js", TRCLEVEL_INFO, __LINE__, 9999, "down" );
          // down
          m_bDown[device] = true;
          m_bUp[device] = false;
          m_DownTime[device] = 0;
          m_UpTime[device] = 0;
          int V = wLoc.getV(m_Selected[device]);

          if( m_LocDialog[device] != NULL )
            m_LocDialog[device]->SelectNext();
          else if( m_Selected[device] != NULL ) {
            if( V > 0 )
              V -= step;
            if( V < 0 )
              V = 0;

            wLoc.setV( m_Selected[device], V );
            speedCmd( device );
          }
        }
        else if( value < 0 ) {
          TraceOp.trc( "js", TRCLEVEL_INFO, __LINE__, 9999, "up" );
          // up
          m_bDown[device] = false;
          m_bUp[device] = true;
          m_DownTime[device] = 0;
          m_UpTime[device] = 0;
          int V     = wLoc.getV(m_Selected[device]);
          int V_max = wLoc.getV_max(m_Selected[device]);

          if( m_LocDialog[device] != NULL )
            m_LocDialog[device]->SelectPrev();
          else if( m_Selected != NULL ) {
            if( V < V_max )
              V += step;
            if( V > V_max )
              V = V_max;

            wLoc.setV( m_Selected[device] , V );
            speedCmd( device );
          }
        }
        else if( value == 0 ) {
          m_bDown[device] = false;
          m_bUp[device]   = false;
        }
    }
    //wxPostEvent( wxGetApp().GetTopWindow(), keyevent );
  }

  NodeOp.base.del(js);
}


static void jsRepeater( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  JsSupport* js = (JsSupport*)ThreadOp.getParm( th );
  do {
    ThreadOp.sleep( 100 );
    if( wJsMap.getrepeat( js->getIni() ) == 0 )
      continue;

    for( int i = 0; i < 4; i++ ) {
      if( js->m_Selected[i] == NULL )
        continue;

      if( js->m_bUp[i] && js->m_UpTime[i] >= wJsMap.getrepeat( js->getIni() ) ) {
        js->m_UpTime[i] = 0;
        wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, JSSUPPORT_EVENT );
        iONode jsEvent = NodeOp.inst( wJsEvent.name(), NULL, ELEMENT_NODE );
        wJsEvent.setdevice( jsEvent, i );
        wJsEvent.settype( jsEvent, 2 );
        wJsEvent.setnumber( jsEvent, wJsMap.getupdown( js->getIni() ) );
        wJsEvent.setvalue( jsEvent, -1 );
        wJsEvent.setmsec( jsEvent, 0 );
        event.SetClientData( jsEvent );
        wxPostEvent( __inst, event );
      }
      else if( js->m_bUp[i] ) {
        js->m_UpTime[i] += 100;
      }

      if( js->m_bDown[i] && js->m_DownTime[i] >= wJsMap.getrepeat( js->getIni() ) ) {
        js->m_DownTime[i] = 0;
        wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, JSSUPPORT_EVENT );
        iONode jsEvent = NodeOp.inst( wJsEvent.name(), NULL, ELEMENT_NODE );
        wJsEvent.setdevice( jsEvent, i );
        wJsEvent.settype( jsEvent, 2 );
        wJsEvent.setnumber( jsEvent, wJsMap.getupdown( js->getIni() ) );
        wJsEvent.setvalue( jsEvent, 1 );
        wJsEvent.setmsec( jsEvent, 0 );
        event.SetClientData( jsEvent );
        wxPostEvent( __inst, event );
      }
      else if( js->m_bDown[i] ) {
        js->m_DownTime[i] += 100;
      }

    }

  } while( js->isRunning() );
}


static void myjsListener( int device, int type, int number, int value, unsigned long msec ) {
  wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, JSSUPPORT_EVENT );
  iONode js = NodeOp.inst( wJsEvent.name(), NULL, ELEMENT_NODE );
  wJsEvent.setdevice( js, device );
  wJsEvent.settype( js, type );
  wJsEvent.setnumber( js, number );
  wJsEvent.setvalue( js, value );
  wJsEvent.setmsec( js, msec );
  event.SetClientData( js );
  wxPostEvent( __inst, event );
}

