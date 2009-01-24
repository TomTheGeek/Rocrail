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
#ifdef __GNUG__
    #pragma implementation "rocgui.cpp"
    #pragma interface "rocgui.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#ifdef __linux__
#include <sys/resource.h>
#endif

#include <wx/image.h>
#include <wx/filedlg.h>
#include <wx/splash.h>
#include <wx/fs_zip.h>
#include <wx/help.h>
#include <wx/cshelp.h>
#include "wx/notebook.h"


#include "rocs/public/str.h"
#include "rocs/public/file.h"
#include "rocs/public/doc.h"
#include "rocs/public/node.h"
#include "rocs/public/mem.h"
#include "rocs/public/res.h"
#include "rocs/public/wrpinf.h"
#include "rocs/public/system.h"
#include "rocs/public/strtok.h"

#include "rocgui/public/guiapp.h"

#include "rocgui/wrapper/public/Gui.h"
#include "rocgui/wrapper/public/RRCon.h"
#include "rocgui/wrapper/public/PlanPanel.h"
#include "rocgui/wrapper/public/SplitPanel.h"
#include "rocgui/wrapper/public/Window.h"
#include "rocgui/wrapper/public/Cmdline.h"
#include "rocgui/wrapper/public/LcCtrl.h"


#include "rocrail/wrapper/public/Trace.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/ModPlan.h"
#include "rocrail/wrapper/public/Module.h"
#include "rocrail/wrapper/public/TrackList.h"
#include "rocrail/wrapper/public/RouteList.h"
#include "rocrail/wrapper/public/BlockList.h"
#include "rocrail/wrapper/public/FeedbackList.h"
#include "rocrail/wrapper/public/SwitchList.h"
#include "rocrail/wrapper/public/SignalList.h"
#include "rocrail/wrapper/public/OutputList.h"
#include "rocrail/wrapper/public/TurntableList.h"
#include "rocrail/wrapper/public/LocList.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/Exception.h"
#include "rocrail/wrapper/public/Response.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/AutoCmd.h"
#include "rocrail/wrapper/public/ZLevel.h"
#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/Clock.h"
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/Route.h"
#include "rocrail/wrapper/public/Schedule.h"
#include "rocrail/wrapper/public/Location.h"
#include "rocrail/wrapper/public/LinkList.h"
#include "rocrail/wrapper/public/SvnLog.h"
#include "rocrail/wrapper/public/SvnLogEntry.h"
#include "rocrail/wrapper/public/DataReq.h"

#include "common/version.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------
// the application icon
#include "rocgui/xpm/rocrail-logo-net.xpm"


IMPLEMENT_APP(RocGui)

static const int itemSize = 32;
int RocGui::getItemSize() {
  return itemSize;
}

int RocGui::getTabAlign() {
  int tabalign = wGui.gettabalign( m_Ini );
  switch( tabalign ){
    case 0:
      return wxNB_DEFAULT;
    case 1:
      return wxNB_LEFT;
    case 2:
      return wxNB_MULTILINE;
  }
  return wxNB_DEFAULT;
}

bool RocGui::isRestrictedEdit() {
  return wGui.isrestrictedit4auto( m_Ini );
}

static void rocrailCallback( obj me, iONode node );

// ============================================================================
// implementation
// ============================================================================

static void ExceptionCallback( int level, char* msg ) {
  /* Added check for VSC++ uninitialized pointer in debug mode */
  if ( ( wxGetApp().getFrame() != NULL ) && ( wxGetApp().getFrame() != (RocGuiFrame *)0xcdcdcdcd )) {
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ADDEXCEPTION_EVENT );
    // Make a copy of the node for using it out of this scope:
    iONode node = NodeOp.inst( wException.name(), NULL, ELEMENT_NODE );
    wException.settext( node, msg );
    event.SetClientData( node );
    wxPostEvent( wxGetApp().getFrame(), event );
  }
}

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

iONode RocGui::getModel() {
  if( m_Model == NULL ) {
    TraceOp.trc( "app", TRCLEVEL_INFO, __LINE__, 9999, "Creating Model..." );
    m_Frame->setLocalPlan( _T("") );
  }
  return m_Model;
}

bool RocGui::isModelSet() {
  return m_Model != NULL ? true:false;
}

int RocGui::OnExit() {
  if( !m_bOnExit ) {
    TraceOp.setExceptionListener( m_Trace, NULL, False );

    RConOp.close( m_RCon );

    wxPoint point = this->getFrame()->GetPosition();
    if( wGui.getwindow( m_Ini ) == NULL ) {
      iONode node = NodeOp.inst( wWindow.name(), m_Ini, ELEMENT_NODE );
      NodeOp.addChild( m_Ini, node );
    }
    wWindow.setx( wGui.getwindow( m_Ini ), point.x );
    wWindow.sety( wGui.getwindow( m_Ini ), point.y );

    wxSize size = this->getFrame()->GetSize();
    wWindow.setcx( wGui.getwindow( m_Ini ), size.GetWidth() );
    wWindow.setcy( wGui.getwindow( m_Ini ), size.GetHeight() );

    if( wGui.getrrcon( m_Ini ) == NULL ) {
      iONode node = NodeOp.inst( wRRCon.name(), m_Ini, ELEMENT_NODE );
      NodeOp.addChild( m_Ini, node );
    }
    wRRCon.sethost( wGui.getrrcon( m_Ini ), m_Host );
    wRRCon.setport( wGui.getrrcon( m_Ini ), m_Port );

    if( wGui.getplanpanel( m_Ini ) == NULL ) {
      iONode node = NodeOp.inst( wPlanPanel.name(), m_Ini, ELEMENT_NODE );
      NodeOp.addChild( m_Ini, node );
    }

    wPlanPanel.setlocobook( wGui.getplanpanel( m_Ini ), m_Frame->isLocoBook() ? True:False );
    wPlanPanel.setscale( wGui.getplanpanel( m_Ini ), m_Frame->getScale() );
    wPlanPanel.setfill( wGui.getplanpanel( m_Ini ), m_Frame->isFill() ? True:False );
    wPlanPanel.setshowid( wGui.getplanpanel( m_Ini ), m_Frame->isShowID() ? True:False );
    wPlanPanel.setraster( wGui.getplanpanel( m_Ini ), m_Frame->isRaster() ? True:False );

    if( wGui.getsplitpanel( m_Ini ) == NULL ) {
      iONode node = NodeOp.inst( wSplitPanel.name(), m_Ini, ELEMENT_NODE );
      NodeOp.addChild( m_Ini, node );
    }
    wSplitPanel.setmain( wGui.getsplitpanel( m_Ini ), m_Frame->getSplitter()->GetSashPosition() );
    wSplitPanel.setmsg( wGui.getsplitpanel( m_Ini ), m_Frame->getTraceSplitter()->GetSashPosition() );

    if( m_Frame->getPlanSplitter()->GetSashPosition() > 1 )
      wSplitPanel.setplan( wGui.getsplitpanel( m_Ini ), m_Frame->getPlanSplitter()->GetSashPosition() );


    // Save LocControlDialog positions:

    // remove previous list:
    iONode lcctrl = wGui.getlcctrl(m_Ini);
    while( lcctrl != NULL ) {
      NodeOp.removeChild( m_Ini, lcctrl);
      lcctrl->base.del(lcctrl);
      lcctrl = wGui.getlcctrl(m_Ini);
    };


    if( m_Model != NULL ) {
      iONode lclist = wPlan.getlclist( m_Model );
      if( lclist != NULL ) {
        int i;
        int cnt = NodeOp.getChildCnt( lclist );
        for( i = 0; i < cnt; i++ ) {
          iONode lc = NodeOp.getChild( lclist, i );
          char* pos = (char*)MapOp.get( m_Frame->getLocDlgMap(), wLoc.getid(lc) );
          if( pos != NULL ) {
            TraceOp.trc( "app", TRCLEVEL_INFO, __LINE__, 9999, "dialog position [%s] for [%s]", pos, wLoc.getid(lc) );

            iOStrTok tok = StrTokOp.inst( pos, ',' );
            int x = atoi( StrTokOp.nextToken(tok) );
            int y = atoi( StrTokOp.nextToken(tok) );
            StrTokOp.base.del(tok);
            lcctrl = NodeOp.inst( wLcCtrl.name(), m_Ini, ELEMENT_NODE );
            wLcCtrl.setid(lcctrl, wLoc.getid(lc));
            wLcCtrl.setx(lcctrl, x);
            wLcCtrl.sety(lcctrl, y);
            NodeOp.addChild( m_Ini, lcctrl);
          }
        }
      }
    }


    char* l_Ini = NodeOp.base.toString( m_Ini );
    printf( "ini=%s\n", l_Ini );
    m_IniFileName = CmdLnOp.getStrDef( m_CmdLn,"-i", wGui.inifile );
    iOFile iniFile = FileOp.inst( m_IniFileName, False );
    if( iniFile != NULL ) {
      FileOp.write( iniFile, l_Ini, StrOp.len( l_Ini ) );
      FileOp.close( iniFile );
    }
    StrOp.free( l_Ini );
    m_bOnExit = true;
  }
  return 0;
}

// Resource messages:
extern "C" {
  extern const char svnLog[];
  extern const char messages[];
  extern const char wrapperinfo[];
  extern const char guiwrapperinfo[];
}

iONode RocGui::getWrpInf( const char* wrpname ) {
  return WrpInfOp.getWrapper( m_WrpInf, wrpname );
}
wxString RocGui::getMsg( const char* key ) {
  return wxString(ResOp.getMsg( m_Res, key ),wxConvUTF8);
}
wxString RocGui::getMenu( const char* key ) {
  return wxString(ResOp.getMenu( m_Res, key ),wxConvUTF8);
}
wxString RocGui::getTip( const char* key ) {
  return wxString(ResOp.getTip( m_Res, key ),wxConvUTF8);
}


void RocGui::readArgs() {
  // Read in all commandline filenames:
  for( int i = 1; i < argc; i++ ) {
    if( StrOp.endsWith( (const char*)wxString(argv[i]).mb_str(wxConvUTF8), ".rrp" ) ||
        StrOp.endsWith( (const char*)wxString(argv[i]).mb_str(wxConvUTF8), ".xml" ) ) {
      m_LocalPlan = argv[i];
      m_bStayOffline = true;
    }
  }
}


void RocGui::pushUndoItem( iONode item ) {
   ListOp.insert(m_UndoItems, 0, (obj)item);
   if( ListOp.size(m_UndoItems) == 1 )
     wxGetApp().getFrame()->GetToolBar()->EnableTool(ME_Undo, true );
}

iONode RocGui::popUndoItem() {
  if( ListOp.size( m_UndoItems ) > 0 ) {
    iONode item = (iONode)ListOp.get( m_UndoItems, 0 );
    ListOp.remove( m_UndoItems, 0 );
    wxGetApp().getFrame()->GetToolBar()->EnableTool(ME_Undo, ListOp.size( m_UndoItems ) > 0 );
    return item;
  }
  else
    return NULL;
}


bool RocGui::OnInit() {

#ifdef __linux__
  {
    struct rlimit rl;
    getrlimit( RLIMIT_CORE, &rl );
    rl.rlim_cur = 1024 * 1024 * 100;
    setrlimit( RLIMIT_CORE, &rl );
  }
#endif
  m_bInit = false;
  m_bOnExit = false;
  m_bOffline = true;
  m_bStayOffline = false;
  m_LocalPlan = _T("");
  m_Model = NULL;
  m_OldModel = NULL;
  m_UndoItems = ListOp.inst();

  // we could need some of these:
  wxInitAllImageHandlers();

  // Required for advanced HTML help
  wxFileSystem::AddHandler(new wxZipFSHandler);

  // show the splash screen:
  wxBitmap bitmap;
  wxSplashScreen* splash = NULL;
  splash = new wxSplashScreen(wxIcon(rocrail_logo_net_xpm),
      wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
      3000, NULL, -1, wxDefaultPosition, wxDefaultSize,
      wxSUNKEN_BORDER|wxSTAY_ON_TOP);

  // convert command line arguments to UTF8 strings:
  char** argv_c = (char**)allocMem( argc * sizeof( char* ) );
  for( int cnt = 0; cnt < argc; cnt++ ) {
    argv_c[cnt] = StrOp.dup( (const char*)wxString(argv[cnt]).mb_str(wxConvUTF8) );
  }
  m_CmdLn = CmdLnOp.inst( argc, (const char**)argv_c );

  // check for plan file:
  readArgs();

  tracelevel  debug   = CmdLnOp.hasKey( m_CmdLn, wCmdline.debug ) ? TRCLEVEL_DEBUG:(tracelevel)0;
  tracelevel  parse   = CmdLnOp.hasKey( m_CmdLn, wCmdline.parse ) ? TRCLEVEL_PARSE:(tracelevel)0;
  const char* tf      = CmdLnOp.getStr( m_CmdLn, wCmdline.trcfile );

  bool modview = CmdLnOp.hasKey( m_CmdLn, wCmdline.modview );

  // create a trace object:
  m_Trace = TraceOp.inst( (tracelevel)(debug | parse | TRCLEVEL_INFO | TRCLEVEL_WARNING), tf, True );
  TraceOp.setAppID( m_Trace, "g" );

  // check for alternative ini file:
  m_IniFileName = CmdLnOp.getStrDef( m_CmdLn, wCmdline.inifile, wGui.inifile );

  // check for tab selection:
  m_TabSel = CmdLnOp.getIntDef( m_CmdLn, wCmdline.tabsel, -1 );

  // process inifile:
  iOFile iniFile = FileOp.inst( m_IniFileName, True );
  char* iniXml = NULL;
  if( iniFile != NULL ) {
    iniXml = (char*)allocMem( FileOp.size( iniFile ) + 1 );
    FileOp.read( iniFile, iniXml, FileOp.size( iniFile ) );
    FileOp.close( iniFile );
    if( StrOp.len( iniXml ) == 0 )
      iniXml = NULL;
  }

  m_Ini = NULL;
  if( iniXml != NULL ) {
    iODoc doc = DocOp.parse( iniXml );
    if( doc != NULL )
      m_Ini = DocOp.getRootNode( doc );
  }

  if( m_Ini == NULL )
    m_Ini = NodeOp.inst( wGui.name(), NULL, ELEMENT_NODE );

  m_bModView = wGui.ismodview( m_Ini ) || modview;

  // prepare the rocrail connection:
  m_Host = wRRCon.gethost( wGui.getrrcon( m_Ini ) );
  m_Host = CmdLnOp.getStrDef( m_CmdLn,wCmdline.host, m_Host );

  m_Port = wRRCon.getport( wGui.getrrcon( m_Ini ) );
  m_Port = CmdLnOp.getIntDef( m_CmdLn,wCmdline.port, m_Port );

  // create the language object:
  m_Res = ResOp.inst( messages, wGui.getlang( m_Ini ) );
  {
    const char* xmls[2] = {wrapperinfo,guiwrapperinfo};
    m_WrpInf = WrpInfOp.inst( xmls, 2 );
  }

  m_RCon  = NULL;
  m_Model = NULL;

  iONode trace = wGui.gettrace( m_Ini );
  if( trace == NULL ) {
    trace = NodeOp.inst( wTrace.name(), m_Ini, ELEMENT_NODE );
    NodeOp.addChild( m_Ini, trace );
  }

  if( wTrace.isdebug( trace ) || debug )
    TraceOp.setLevel( m_Trace, (tracelevel)(TraceOp.getLevel( m_Trace ) | TRCLEVEL_DEBUG) );
  if( wTrace.isparse( trace ) || parse )
    TraceOp.setLevel( m_Trace, (tracelevel)(TraceOp.getLevel( m_Trace ) | TRCLEVEL_PARSE) );

  TraceOp.setFilename( m_Trace, tf?tf:wTrace.getgfile( trace ) );

  /* Logo. */
  TraceOp.println( "--------------------------------------------------" );
  TraceOp.println( "                                            lll   " );
  TraceOp.println( "                                        iii lll   " );
  TraceOp.println( "                                            lll   " );
  TraceOp.println( " rrrrrr   ooooo   ccccc rrrrrr   aaaaaa iii lll   " );
  TraceOp.println( " rrr rrr ooo ooo ccc    rrr rrr aaa aaa iii lll   " );
  TraceOp.println( " rrr     ooo ooo ccc    rrr     aaa aaa iii lll   " );
  TraceOp.println( " rrr      ooooo   ccccc rrr      aaaaaa iii  lll  " );
  TraceOp.println( "--------------------------------------------------" );
  TraceOp.println( " Copyright (c) 2002-2008 Rob Versluis"              );
  TraceOp.println( "   r.j.versluis@rocrail.net"                        );
  TraceOp.println( "   http://www.rocrail.net"                          );
  TraceOp.println( " License: GNU GPL 2"                                );
  TraceOp.println( "   http://www.gnu.org/licenses/gpl.txt"             );
  TraceOp.println( "--------------------------------------------------" );

  // trace version
  TraceOp.trc( "app", TRCLEVEL_INFO, __LINE__, 9999, " %s %d.%d.%d \"%s\" %s",
      wGui.productname,
      wGui.vmajor,
      wGui.vminor,
      wGui.patch,
      wGui.releasename, wGui.releasesuffix );
  TraceOp.trc( "app", TRCLEVEL_INFO, __LINE__, 9999, " build %s %s",
      wGui.buildDate,
      wGui.buildTime );

  if( bzr > 0 ) {
    m_Svn = bzr;
    TraceOp.trc( "app", TRCLEVEL_INFO, __LINE__, 9999," bzr %d", m_Svn );
  }
  else {
    iODoc doc = DocOp.parse(svnLog);
    if( doc != NULL ) {
      iONode log = DocOp.getRootNode(doc);
      DocOp.base.del(doc);
      if( log != NULL ) {
        m_Svn = wSvnLogEntry.getrevision( wSvnLog.getlogentry(log) );
        TraceOp.trc( "app", TRCLEVEL_INFO, __LINE__, 9999," svn %d", m_Svn );
        NodeOp.base.del(log);
      }
    }
  }


  // create the frame:
  TraceOp.trc( "app", TRCLEVEL_INFO, __LINE__, 9999, "Creating Frame..." );

  int iX = wWindow.getx( wGui.getwindow( m_Ini ) );
  int iY = wWindow.gety( wGui.getwindow( m_Ini ) );
  int iWidth  = wWindow.getcx( wGui.getwindow( m_Ini ) );
  int iHeight = wWindow.getcy( wGui.getwindow( m_Ini ) );

  // check for offline mode:
  if( !CmdLnOp.hasKey( m_CmdLn, wCmdline.offline ) && m_LocalPlan.Len() == 0 ) {
    // connect to the rocrail daemon:
    int tries = 0;
    int retry = wRRCon.getretry( wGui.getrrcon( m_Ini ) );
    int retryinterval = wRRCon.getretryinterval( wGui.getrrcon( m_Ini ) );

    while( m_RCon == NULL && tries < retry ) {
      TraceOp.trc( "app", TRCLEVEL_INFO, __LINE__, 9999,
        "Try[%d] connecting to rocrail at %s:%d...",tries+1, m_Host, m_Port );
      m_RCon = RConOp.inst( m_Host, m_Port );
      if( m_RCon == NULL )
        ThreadOp.sleep( retryinterval );
      else {
        RConOp.setCallback( m_RCon, &rocrailCallback, (obj)this );
      }
      tries++;
    }
  }

  if( m_RCon != NULL ) {
    m_bOffline = false;
  }

  m_Frame = new RocGuiFrame( _T("Rocrail"),
        wxPoint(iX, iY),
        wxSize(iWidth, iHeight), m_Ini
        );
  m_bInit = true;

  if( CmdLnOp.hasKey( m_CmdLn, wCmdline.offline ) || m_LocalPlan.Len() > 0 ) {
    m_Frame->setLocalPlan( m_LocalPlan );
    m_bOffline = true;
  }

  if( m_RCon != NULL ) {
    m_bOffline = false;
    char* val = StrOp.fmt( "%s:%d", m_Host, m_Port );
    m_Frame->SetStatusText( wxString(val,wxConvUTF8), status_rcon );
    m_Frame->setOnline(true);

    // Initial connection.
    iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.plan );
    char* guiid = StrOp.fmt( "%s,%d,%d", wGui.getid(m_Ini),
        SocketOp.getMAC(NULL)!=NULL?SocketOp.getMAC(NULL):0, SystemOp.getpid() );
    wModelCmd.setcmdfrom( cmd, guiid );
    sendToRocrail( cmd );
    StrOp.free(guiid);
    cmd->base.del( cmd );
  }
  else {
    m_Frame->SetStatusText( getMsg("offline"), status_rcon );
    m_bOffline = true;
  }

  // stop the splash screen:
  if( splash != NULL ) {
    //splash->Show(false);
    //splash->Destroy();
  }

  TraceOp.setExceptionListener( m_Trace, ExceptionCallback, False );

  return TRUE;
}

void RocGui::Callback( obj me, iONode node ) {
  rocrailCallback( me, node );
}

static void rocrailCallback( obj me, iONode node ) {
  RocGui* guiApp = (RocGui*)me;

  TraceOp.trc( "app", TRCLEVEL_DEBUG, __LINE__, 9999, "rocrailCallback node=%s",
               NodeOp.getName( node ) );

  /* Plan */
  if( StrOp.equals( wPlan.name(), NodeOp.getName( node ) ) ) {
    TraceOp.trc( "app", TRCLEVEL_INFO, __LINE__, 9999, "guiApp->setModel( %s )", NodeOp.getName( node ) );

    wxGetApp().getFrame()->setEditMode(false);


    if( guiApp->getFrame()->getNotebook() != NULL ) {
      if( !guiApp->isModelSet() || guiApp->isStayOffline() ) {
        wxGetApp().setStayOffline( false );
        guiApp->setModel( node );
        wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, INIT_NOTEBOOK );
        wxPostEvent( guiApp->getFrame(), event );
      }
      else {
        TraceOp.trc( "app", TRCLEVEL_WARNING, __LINE__, 9999, "*** already had the plan model!!!" );
      }
    }
    else {
      TraceOp.trc( "app", TRCLEVEL_WARNING, __LINE__, 9999, "notebook not yet initialized" );
      guiApp->getFrame()->getPlanPanel()->clean();
      guiApp->getFrame()->getPlanPanel()->init();
    }

    return;
  }
  /* rocrail.ini */
  else if( StrOp.equals( wRocRail.name(), NodeOp.getName( node ) ) ) {
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ME_RocrailIni );
    // Make a copy of the node for using it out of this scope:
    event.SetClientData( node->base.clone( node ) );
    wxPostEvent( guiApp->getFrame(), event );
  }
  /* modplan */
  else if( StrOp.equals( wModPlan.name(), NodeOp.getName( node ) ) ) {
    if( guiApp->getFrame()->getModPanel() != NULL ) {
      wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ME_ModPlanProperties );
      // Make a copy of the node for using it out of this scope:
      event.SetClientData( node->base.clone( node ) );
      wxPostEvent( guiApp->getFrame()->getModPanel(), event );
    }
  }
  /* module */
  else if( StrOp.equals( wModule.name(), NodeOp.getName( node ) ) ) {
    if( guiApp->getFrame()->getModPanel() != NULL ) {
      wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ME_ModuleProperties );
      // Make a copy of the node for using it out of this scope:
      event.SetClientData( node->base.clone( node ) );
      wxPostEvent( guiApp->getFrame()->getModPanel(), event );
    }
  }
  /* Controller */
  else if( StrOp.equals( wDigInt.name(), NodeOp.getName( node ) ) ) {
    char* msg = StrOp.fmt( "%s:%s", wResponse.getsender( node ), wResponse.getmsg( node ) );
    // TODO: send an event to update a status field
    //wxGetApp().getFrame()->SetStatusText( wxString(msg,wxConvUTF8), status_digint );
    StrOp.free( msg );
  }
  /* Clock */
  else if( StrOp.equals( wClock.name(), NodeOp.getName( node ) ) && wxGetApp().getFrame() != NULL && guiApp->isInit()) {
    wxGetApp().getFrame()->SyncClock( node );
  }
  /* State */
  else if( StrOp.equals( wState.name(), NodeOp.getName( node ) ) && guiApp->isInit() ) {
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, STATE_EVENT );
    // Make a copy of the node for using it out of this scope:
    event.SetClientData( node->base.clone( node ) );
    wxPostEvent( guiApp->getFrame(), event );
  }
  /* PT */
  else if( StrOp.equals( wProgram.name(), NodeOp.getName( node ) ) ) {
    char* msg = StrOp.fmt( "CV value=%d", wProgram.getvalue( node ) );
    StrOp.free( msg );
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, CV_EVENT );
    // Make a copy of the node for using it out of this scope:
    event.SetClientData( node->base.clone( node ) );
    wxPostEvent( guiApp->getFrame(), event );
  }
  /* Exception */
  else if( StrOp.equals( wException.name(), NodeOp.getName( node ) ) ) {
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ADDEXCEPTION_EVENT );
    // Make a copy of the node for using it out of this scope:
    event.SetClientData( node->base.clone( node ) );
    wxPostEvent( guiApp->getFrame(), event );
  }
  /* Loc */
  else if( StrOp.equals( wLoc.name(), NodeOp.getName( node ) ) ) {
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, LOC_EVENT );
    // Make a copy of the node for using it out of this scope:
    event.SetClientData( node->base.clone( node ) );
    wxPostEvent( guiApp->getFrame(), event );
  }
  /* DataReq (loco image?) */
  else if( StrOp.equals( wDataReq.name(), NodeOp.getName( node ) ) ) {
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, UPDATE_ACTIVELOCS_EVENT );
    // Make a copy of the node for using it out of this scope:
    event.SetClientData( node->base.clone( node ) );
    wxPostEvent( guiApp->getFrame(), event );
  }
  /* Auto */
  else if( StrOp.equals( wAutoCmd.name(), NodeOp.getName( node ) ) ) {
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, AUTO_EVENT );
    // Make a copy of the node for using it out of this scope:
    event.SetClientData( node->base.clone( node ) );
    wxPostEvent( guiApp->getFrame(), event );
  }
  /* System on/off */
  else if( StrOp.equals( wSysCmd.name(), NodeOp.getName( node ) ) ) {
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, SYSTEM_EVENT );
    // Make a copy of the node for using it out of this scope:
    event.SetClientData( node->base.clone( node ) );
    wxPostEvent( guiApp->getFrame(), event );
  }
  /* ZLevel */
  else if( StrOp.equals( wZLevel.name(), NodeOp.getName( node ) ) ) {
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ZLEVEL_EVENT_ADD );
    // Make a copy of the node for using it out of this scope:
    event.SetClientData( node->base.clone( node ) );
    wxPostEvent( guiApp->getFrame(), event );
  }
  /* Model remove: */
  else if( StrOp.equals( wModelCmd.name(), NodeOp.getName( node ) ) &&
           StrOp.equals( wModelCmd.getcmd( node ), wModelCmd.remove ) ) {
    int childCnt = NodeOp.getChildCnt( node );
    Boolean hadZLevel = False;
    int i = 0;
    for( i = 0; i < childCnt; i++ ) {
      iONode child = NodeOp.getChild( node, i );
      if( StrOp.equals( wZLevel.name(), NodeOp.getName( child ) ) ) {
        wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ZLEVEL_EVENT_REMOVE );
        // Make a copy of the node for using it out of this scope:
        event.SetClientData( node->base.clone( child ) );
        wxPostEvent( guiApp->getFrame(), event );
        hadZLevel = True;
      }
    }
    if( !hadZLevel ) {
      int pagecnt = guiApp->getFrame()->getNotebook()->GetPageCount();
      for( int i = 0; i < pagecnt; i++ ) {
        PlanPanel* p = (PlanPanel*)guiApp->getFrame()->getNotebook()->GetPage(i);
        wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, UPDATEITEM_EVENT );
        event.SetClientData( node->base.clone( node ) );
        wxPostEvent( p, event );

        //p->modelEvent( node );
      }
    }
  }
  /* default */
  else if(wxGetApp().getFrame() != NULL && guiApp->isInit() ){
    const char* nodeName = NodeOp.getName( node );
    TraceOp.trc( "app", TRCLEVEL_DEBUG, __LINE__, 9999, "rocrailCallback node=%s", nodeName );

    iONode model = wxGetApp().getModel();
    bool planItems = true;
    int childCnt = NodeOp.getChildCnt( node );
    int i = 0;
    for( i = 0; i < childCnt && model != NULL; i++ ) {
      iONode child = NodeOp.getChild( node, i );
      const char* childName = NodeOp.getName( child );

      if( StrOp.equals( childName, wLoc.name() ) ) {
        iONode lclist = wPlan.getlclist( model );
        if( lclist != NULL ) {
          /* check if a loco already exist with this ID */
          iONode lc = wxGetApp().getFrame()->findLoc(wLoc.getid(child));
          if(lc == NULL ) {
            NodeOp.addChild( lclist, (iONode)NodeOp.base.clone(child) );
          }
          else {
            /* TODO: update existing */
            //NodeOp.mergeNode(lc, child, True, True );
          }
          wxGetApp().getFrame()->InitActiveLocs();
        }
        planItems = false;
      }
      else if( StrOp.equals( childName, wRoute.name() ) ) {
        iONode stlist = wPlan.getstlist( model );
        if( stlist != NULL )
          NodeOp.addChild( stlist, (iONode)NodeOp.base.clone(child) );
        planItems = false;
      }
      else if( StrOp.equals( childName, wSchedule.name() ) ) {
        iONode sclist = wPlan.getsclist( model );
        if( sclist != NULL )
          NodeOp.addChild( sclist, (iONode)NodeOp.base.clone(child) );
        planItems = false;
      }
      else if( StrOp.equals( childName, wLocation.name() ) ) {
        iONode locationlist = wPlan.getlocationlist( model );
        if( locationlist != NULL )
          NodeOp.addChild( locationlist, (iONode)NodeOp.base.clone(child) );
        planItems = false;
      }
      else if( StrOp.equals( childName, wLinkList.name() ) ) {
        iONode linklist = wPlan.getlinklist( model );
        if( linklist != NULL )
          NodeOp.addChild( linklist, (iONode)NodeOp.base.clone(child) );
        planItems = false;
      }
      /* ZLevel */
      else if( StrOp.equals( childName, wZLevel.name() ) ) {
        wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ZLEVEL_EVENT_ADD );
        // Make a copy of the node for using it out of this scope:
        event.SetClientData( node->base.clone( child ) );
        wxPostEvent( guiApp->getFrame(), event );
        planItems = false;
      }
    }

    if(planItems) {
      if( StrOp.equals( wModelCmd.name(), NodeOp.getName( node ) ) &&
          StrOp.equals( wModelCmd.getcmd( node ), wModelCmd.merge ) )
      {
        for( i = 0; i < childCnt && model != NULL; i++ ) {
          iONode child = NodeOp.getChild( node, i );
          const char* childName = NodeOp.getName( child );
          iONode list = NULL;
          char* listname = StrOp.fmt( "%slist", childName );

          list = NodeOp.findNode( model, listname );

          if( list != NULL ) {
            NodeOp.addChild( list, (iONode)NodeOp.base.clone(child) );
          }

          StrOp.free(listname);
        }

      }
      else if( guiApp->isInit() ) {
        int pagecnt = guiApp->getFrame()->getNotebook()->GetPageCount();
        for( int i = 0; i < pagecnt; i++ ) {
          PlanPanel* p = (PlanPanel*)guiApp->getFrame()->getNotebook()->GetPage(i);
          p->modelEvent( node );
        }
        wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, UPDATE_ACTIVELOCS_EVENT );
        event.SetClientData( node->base.clone( node ) );
        wxPostEvent( guiApp->getFrame(), event );
      }
    }
  }
}


void RocGui::setModel( iONode node ) {
  if( m_Model != NULL && !isOffline() ) {
    // Delete the old model node:
    m_Model->base.del( m_Model );
  }
  else if( m_Model != NULL && isOffline() ) {
    m_OldModel = m_Model;
  }
  // Save a copy of the node:
  m_Model = (iONode)node->base.clone( node );
}


void RocGui::cleanupOldModel() {
  if( m_OldModel != NULL ) {
    // Delete the old model node:
    m_OldModel->base.del( m_OldModel );
  }
}


void RocGui::sendToRocrail( iONode cmd, bool disconnect ) {
  char* strCmd = NodeOp.base.toString( cmd );
  sendToRocrail( strCmd, disconnect );
  StrOp.free( strCmd );

  if( m_bOffline ) {
    if( StrOp.equals( wProgram.name(), NodeOp.getName( cmd ) ) ) {
      return;
    }
    TraceOp.trc( "app", TRCLEVEL_INFO, __LINE__, 9999, "Bounce to add to local model..." );

    // Bounce to add to local model...
    if( StrOp.equals( wModelCmd.name(), NodeOp.getName( cmd ) ) ) {
      if( StrOp.equals( wModelCmd.modify, wModelCmd.getcmd( cmd ) ) )
        wxGetApp().setLocalModelModified(true);
      else if( StrOp.equals( wModelCmd.remove, wModelCmd.getcmd( cmd ) ) )
        wxGetApp().setLocalModelModified(true);
      else if( StrOp.equals( wModelCmd.add, wModelCmd.getcmd( cmd ) ) )
        wxGetApp().setLocalModelModified(true);
      else if( StrOp.equals( wModelCmd.plantitle, wModelCmd.getcmd( cmd ) ) )
        wxGetApp().setLocalModelModified(true);
    }
    Callback( (obj)&wxGetApp(), cmd );
  }
}

void RocGui::sendToRocrail( char* szCmd, bool disconnect ) {
  if( m_bStayOffline )
    return;

  if( szCmd == NULL && m_RCon != NULL ) {
    // force disconnect:
    RConOp.close( m_RCon );
    RConOp.base.del( m_RCon );
    m_RCon = NULL;
  }

  if( m_RCon == NULL ) {
    m_RCon = RConOp.inst( m_Host, m_Port );
    if( m_RCon != NULL ) {
      m_bOffline = false;
      RConOp.setCallback( m_RCon, &rocrailCallback, (obj)this );
      char* val = StrOp.fmt( "%s:%d", m_Host, m_Port );
      m_Frame->SetStatusText( wxString(val,wxConvUTF8), status_rcon );
      StrOp.free(val);
    }
    else {
      // show popup with message
      if( m_Frame != NULL ) {
        char* val = StrOp.fmt( " (%s:%d)", m_Host, m_Port );
        wxMessageDialog( m_Frame,
            wxGetApp().getMsg("connectionwarning") +
            wxString(val,wxConvUTF8),
            _T("Rocrail"), wxOK | wxICON_EXCLAMATION ).ShowModal();
        StrOp.free(val);
      }
    }
  }
  if( szCmd != NULL ) {
    TraceOp.trc( "app", TRCLEVEL_INFO, __LINE__, 9999, "sendToRocrail( %.1024s )", szCmd );
    if( m_RCon != NULL ) {
      RConOp.write( m_RCon, szCmd );
      if( disconnect )
        RConOp.close( m_RCon );
    }
  }

}
