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
#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "rocrailinidialog.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "rocrailinidialog.h"
#include "rocgui/dialogs/controllers/gendlg.h"
#include "rocgui/dialogs/controllers/ddxdlg.h"
#include "rocgui/dialogs/controllers/hsi88dlg.h"
#include "rocgui/dialogs/controllers/barjutdlg.h"
#include "rocgui/dialogs/controllers/dinamodlg.h"
#include "rocgui/dialogs/controllers/srcpdlg.h"
#include "rocgui/dialogs/controllers/loconetdlg.h"
#include "rocgui/dialogs/controllers/ecosdlg.h"
#include "rocgui/dialogs/controllers/opendcc.h"
#include "rocgui/dialogs/controllers/lenzdlg.h"

#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/Clock.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Trace.h"
#include "rocrail/wrapper/public/HttpService.h"
#include "rocrail/wrapper/public/WebClient.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/Tcp.h"
#include "rocrail/wrapper/public/Ctrl.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Program.h"

#include "rocgui/public/guiapp.h"

////@begin XPM images
////@end XPM images

/*!
 * RocrailIniDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( RocrailIniDialog, wxDialog )

/*!
 * RocrailIniDialog event table definition
 */

BEGIN_EVENT_TABLE( RocrailIniDialog, wxDialog )

////@begin RocrailIniDialog event table entries
    EVT_NOTEBOOK_PAGE_CHANGED( ID_NOTEBOOK_RR, RocrailIniDialog::OnNotebookRrPageChanged )

    EVT_LISTBOX( ID_LISTBOX_RR_CONTROLLERS, RocrailIniDialog::OnListboxRrControllersSelected )

    EVT_BUTTON( ID_BUTTON_RR_DELETE, RocrailIniDialog::OnButtonRrDeleteClick )

    EVT_BUTTON( ID_BUTTON_RR_PROPS, RocrailIniDialog::OnButtonRrPropsClick )

    EVT_BUTTON( ID_BUTTON_RR_ADD, RocrailIniDialog::OnButtonRrAddClick )

    EVT_BUTTON( wxID_OK, RocrailIniDialog::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, RocrailIniDialog::OnCancelClick )

    EVT_BUTTON( wxID_APPLY, RocrailIniDialog::OnApplyClick )

////@end RocrailIniDialog event table entries

END_EVENT_TABLE()

/*!
 * RocrailIniDialog constructors
 */

RocrailIniDialog::RocrailIniDialog( )
{
}

RocrailIniDialog::RocrailIniDialog( wxWindow* parent, iONode props )
{
  m_TabAlign = wxGetApp().getTabAlign();
  Create(parent, -1, wxGetApp().getMsg( "rocrailini" ) );
  m_Props = props;
  m_CSDialog = NULL;
  initLabels();
  initValues();
  m_Apply->Enable( false );

  m_GeneralPanel->GetSizer()->Layout();
  m_TracePanel->GetSizer()->Layout();
  m_ServicePanel->GetSizer()->Layout();
  m_AtomatPanel->GetSizer()->Layout();
  m_ControllersPanel->GetSizer()->Layout();

  m_PlanType->Fit();

  m_RRNotebook->Fit();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
}

void RocrailIniDialog::initLabels() {
  m_RRNotebook->SetPageText( 0, wxGetApp().getMsg( "general" ) );
  m_RRNotebook->SetPageText( 1, wxGetApp().getMsg( "trace" ) );
  m_RRNotebook->SetPageText( 2, wxGetApp().getMsg( "service" ) );
  m_RRNotebook->SetPageText( 3, wxGetApp().getMsg( "auto" ) );
  m_RRNotebook->SetPageText( 4, wxGetApp().getMsg( "controller" ) );

  // General
  m_LabelPlanfile->SetLabel( wxGetApp().getMsg( "planfile" ) );
  m_labPlanType->SetLabel( wxGetApp().getMsg( "type" ) );
  m_LabelLibPath->SetLabel( wxGetApp().getMsg( "libpath" ) );
  m_labImgPath->SetLabel( wxGetApp().getMsg( "imagepath" ) );
  m_LabelTraceFile->SetLabel( wxGetApp().getMsg( "tracefile" ) );
  m_labPTIID->SetLabel( wxGetApp().getMsg( "ptiid" ) );
  m_PTIID->SetToolTip( wxGetApp().getTip( "ptiid" ) );
  m_labLCIID->SetLabel( wxGetApp().getMsg( "lciid" ) );
  m_LCIID->SetToolTip( wxGetApp().getTip( "lciid" ) );
  m_labDPIID->SetLabel( wxGetApp().getMsg( "dpiid" ) );
  m_DPIID->SetToolTip( wxGetApp().getTip( "dpiid" ) );
  m_labSVIID->SetLabel( wxGetApp().getMsg( "sviid" ) );
  m_SVIID->SetToolTip( wxGetApp().getTip( "sviid" ) );

  m_PlanType->SetString( 0, wxGetApp().getMsg( "default" ) );
  m_PlanType->SetString( 1, wxGetApp().getMsg( "modpanel" ) );


  // Trace
  m_TraceLevel->GetStaticBox()->SetLabel( _T("User ") + wxGetApp().getMsg( "tracelevel" ) );
  m_DevTraceLevel->SetLabel( _T("Developer ") + wxGetApp().getMsg( "tracelevel" ) );
  m_Debug->SetLabel( wxGetApp().getMsg( "debug" ) );
  m_Auto->SetLabel( wxGetApp().getMsg( "auto" ) );
  m_Byte->SetLabel( wxGetApp().getMsg( "byte" ) );
  //m_Parse->SetLabel( wxGetApp().getMsg( "parse" ) );

  // Service
  m_LabelClientPort->SetLabel( wxGetApp().getMsg( "clientport" ) );
  m_LabelMaxCon->SetLabel( wxGetApp().getMsg( "maxclientcon" ) );
  m_HttpBox->GetStaticBox()->SetLabel( wxGetApp().getMsg( "http" ) );
  m_LabelHttpPort->SetLabel( wxGetApp().getMsg( "port" ) );
  m_LabServiceRefresh->SetLabel( wxGetApp().getMsg( "refresh" ) );
  m_OnlyFirstMaster->SetLabel( wxGetApp().getMsg( "onlyfirstmaster" ) );

  // WebService
  m_WebServiceBox->GetStaticBox()->SetLabel( wxGetApp().getMsg( "webclient" ) );
  m_LabWebServicePort->SetLabel( wxGetApp().getMsg( "port" ) );
  m_LabWebServiceRefresh->SetLabel( wxGetApp().getMsg( "refresh" ) );

  // Automat
  m_LabelSwTime->SetLabel( wxGetApp().getMsg( "switchtime" ) );
  m_LabelMinBKLen->SetLabel( wxGetApp().getMsg( "minblocklen" ) );
  m_LabelIgnEvt->SetLabel( wxGetApp().getMsg( "ignevent" ) );
  m_labInitPause->SetLabel( wxGetApp().getMsg( "initfieldpause" ) );
  m_labInitPower->SetLabel( wxGetApp().getMsg( "initfieldpower" ) );
  m_labSeed->SetLabel( wxGetApp().getMsg( "randomseed" ) );
  m_labSavePosTime->SetLabel( wxGetApp().getMsg( "savepostime" ) );
  m_labSwitchRetryTime->SetLabel( wxGetApp().getMsg( "switchretrytime" ) );
  m_labSwitchRetryCount->SetLabel( wxGetApp().getMsg( "switchretrycount" ) );
  m_Check2In->SetLabel( wxGetApp().getMsg( "check2in" ) );
  m_secondNextBlock->SetLabel( wxGetApp().getMsg( "secondnextblock" ) );
  m_EnableSwFb->SetLabel( wxGetApp().getMsg( "enableswfb" ) );
  m_labEventTimeout->SetLabel( wxGetApp().getMsg( "eventtimeout" ) );
  m_labSignalReset->SetLabel( wxGetApp().getMsg( "signalreset" ) );
  m_labRouteSwTime->SetLabel( wxGetApp().getMsg( "routeswtime" ) );
  m_DisableRouteVreduce->SetLabel( wxGetApp().getMsg( "disablerouteVreduce" ) );
  m_DefAspect->SetLabel( wxGetApp().getMsg( "defaspect" ) );
  m_DefAspect->SetString( 0, wxGetApp().getMsg( "red" ) );
  m_DefAspect->SetString( 1, wxGetApp().getMsg( "green" ) );
  m_DefAspect->SetString( 2, wxGetApp().getMsg( "yellow" ) );
  m_DefAspect->SetString( 3, wxGetApp().getMsg( "white" ) );
  m_GreenAspect->SetLabel( wxGetApp().getMsg( "greenaspect" ) );
  m_SkipSetSw->SetLabel( wxGetApp().getMsg( "skipsetsw" ) );
  m_SkipSetSw->SetToolTip(wxGetApp().getTip( "skipsetsw" ));
  m_UseBiCom->SetLabel( wxGetApp().getMsg( "usebicom" ) );
  m_labSemaphoreWait->SetLabel( wxGetApp().getMsg( "semaphorewait" ) );
  m_StopAtIdentMisMatch->SetLabel( wxGetApp().getMsg( "stopatidentmismatch" ) );

  // Controller
  m_ControllerDelete->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_ControllerProps->SetLabel( wxGetApp().getMsg( "properties" ) );
  m_ControllerAdd->SetLabel( wxGetApp().getMsg( "add" ) );
  m_AddControllerBox->GetStaticBox()->SetLabel( wxGetApp().getMsg( "new" ) );



  // Buttons
  m_OK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
  m_Apply->SetLabel( wxGetApp().getMsg( "apply" ) );
}


void RocrailIniDialog::initValues() {
  // General
  m_PlanFile->SetValue( wxString(wRocRail.getplanfile( m_Props ),wxConvUTF8) );
  m_LibPath->SetValue( wxString(wRocRail.getlibpath( m_Props ),wxConvUTF8) );
  m_ImgPath->SetValue( wxString(wRocRail.getimgpath( m_Props ),wxConvUTF8) );
  iONode trace = wRocRail.gettrace( m_Props );
  if( trace == NULL ) {
    trace = NodeOp.inst( wTrace.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild( m_Props, trace );
  }
  m_TraceFile->SetValue( wxString(wTrace.getrfile( trace ),wxConvUTF8) );
  m_PTIID->SetValue( wxString(wRocRail.getptiid( m_Props ),wxConvUTF8) );
  m_LCIID->SetValue( wxString(wRocRail.getlciid( m_Props ),wxConvUTF8) );
  m_DPIID->SetValue( wxString(wRocRail.getdpiid( m_Props ),wxConvUTF8) );
  m_SVIID->SetValue( wxString(wRocRail.getsviid( m_Props ),wxConvUTF8) );

  m_PlanType->SetSelection( wRocRail.iscreatemodplan( m_Props ) ? 1:0 );

  // Service
  iONode tcp = wRocRail.gettcp( m_Props );
  if( tcp == NULL ) {
    tcp = NodeOp.inst( wTcp.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild( m_Props, tcp );
  }
  char* val = StrOp.fmt( "%d", wTcp.getport( tcp ) );
  m_ClientPort->SetValue( wxString( val,wxConvUTF8) );
  StrOp.free( val );
  val = StrOp.fmt( "%d", wTcp.getmaxcon( tcp ) );
  m_MaxCon->SetValue( wxString( val,wxConvUTF8) );
  StrOp.free( val );

  m_OnlyFirstMaster->SetValue( wTcp.isonlyfirstmaster( tcp ) ? true:false );

  iONode http = wRocRail.gethttp( m_Props );
  if( http == NULL ) {
    val = StrOp.fmt( "%d", 0 );
    m_HttpPort->SetValue( wxString( val,wxConvUTF8) );
    StrOp.free( val );
    val = StrOp.fmt( "%d", 0 );
    m_ServiceRefresh->SetValue( wxString( val,wxConvUTF8) );
    StrOp.free( val );
  }
  else {
    val = StrOp.fmt( "%d", wHttpService.getport( http ) );
    m_HttpPort->SetValue( wxString( val,wxConvUTF8) );
    StrOp.free( val );
    val = StrOp.fmt( "%d", wHttpService.getrefresh( http ) );
    m_ServiceRefresh->SetValue( wxString( val,wxConvUTF8) );
    StrOp.free( val );

    iONode web = wHttpService.getwebclient( http );
    if( web != NULL ) {
      val = StrOp.fmt( "%d", wWebClient.getport( web ) );
      m_WebServicePort->SetValue( wxString( val,wxConvUTF8) );
      StrOp.free( val );
      val = StrOp.fmt( "%d", wWebClient.getrefresh( web ) );
      m_WebServiceRefresh->SetValue( wxString( val,wxConvUTF8) );
      StrOp.free( val );
    }
  }

  // Clock Service
  iONode clock = wRocRail.getclock( m_Props );
  if( clock == NULL ) {
    clock = NodeOp.inst( wClock.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild( m_Props, clock );
  }
  val = StrOp.fmt( "%d", wClock.getdivider( clock ) );
  m_Divider->SetValue( wxString( val,wxConvUTF8) );
  StrOp.free( val );
  val = StrOp.fmt( "%d", wClock.gethour( clock ) );
  m_Hour->SetValue( wxString( val,wxConvUTF8) );
  StrOp.free( val );
  val = StrOp.fmt( "%d", wClock.getminute( clock ) );
  m_Minute->SetValue( wxString( val,wxConvUTF8) );
  StrOp.free( val );

  // Trace
  m_Debug->SetValue( wTrace.isdebug( trace ) );
  m_Byte->SetValue( wTrace.isbyte( trace ) );
  //m_Parse->SetValue( wTrace.isparse( trace ) );
  m_Auto->SetValue( wTrace.isautomatic( trace ) );
  m_Monitor->SetValue( wTrace.ismonitor( trace ) );

  // Automat
  iONode ctrl = wRocRail.getctrl( m_Props );
  if( ctrl == NULL ) {
    ctrl = NodeOp.inst( wCtrl.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild( m_Props, ctrl );
  }
  val = StrOp.fmt( "%d", wCtrl.getswtimeout( ctrl ) );
  m_SwTime->SetValue( wxString( val,wxConvUTF8) );
  StrOp.free( val );
  val = StrOp.fmt( "%d", wCtrl.getminbklc( ctrl ) );
  m_MinBKLen->SetValue( wxString( val,wxConvUTF8) );
  StrOp.free( val );
  val = StrOp.fmt( "%d", wCtrl.getignevt( ctrl ) );
  m_IgnEvt->SetValue( wxString( val,wxConvUTF8) );
  StrOp.free( val );
  val = StrOp.fmt( "%d", wCtrl.getinitfieldpause( ctrl ) );
  m_PauseInit->SetValue( wxString( val,wxConvUTF8) );
  StrOp.free( val );
  m_Seed->SetValue( wCtrl.getseed( ctrl ) );
  m_SavePosTime->SetValue( wCtrl.getsavepostime( ctrl ) );
  m_SwitchRetryTime->SetValue( wCtrl.getswitchretrytime( ctrl ) );
  m_SwitchRetryCount->SetValue( wCtrl.getswitchretrycount( ctrl ) );
  m_Check2In->SetValue( wCtrl.ischeck2in( ctrl ) );
  m_secondNextBlock->SetValue( wCtrl.issecondnextblock( ctrl ) );
  m_InitfieldPower->SetValue( wCtrl.isinitfieldpower( ctrl ) );
  m_EnableSwFb->SetValue( wCtrl.isenableswfb( ctrl ) );
  val = StrOp.fmt( "%d", wCtrl.geteventtimeout( ctrl ) );
  m_EventTimeout->SetValue( wxString( val,wxConvUTF8) );
  StrOp.free( val );
  m_SignalReset->SetValue( wCtrl.getsignalreset( ctrl ) );
  m_RouteSwTime->SetValue( wCtrl.getrouteswtime( ctrl ) );
  m_DisableRouteVreduce->SetValue( wCtrl.isdisablerouteVreduce( ctrl ) );
  m_SkipSetSw->SetValue( wCtrl.isskipsetsw( ctrl ) );
  m_UseBiCom->SetValue( wCtrl.isusebicom( ctrl ) );
  m_SemaphoreWait->SetValue( wCtrl.getsemaphorewait( ctrl ) );
  m_StopAtIdentMisMatch->SetValue( wCtrl.ispoweroffonidentmismatch( ctrl ) );

  if( StrOp.equals( wSignal.red, wCtrl.getdefaspect( ctrl ) ) )
    m_DefAspect->SetSelection(0);
  else if( StrOp.equals( wSignal.green, wCtrl.getdefaspect( ctrl ) ) )
    m_DefAspect->SetSelection(1);
  else if( StrOp.equals( wSignal.yellow, wCtrl.getdefaspect( ctrl ) ) )
    m_DefAspect->SetSelection(2);
  else if( StrOp.equals( wSignal.white, wCtrl.getdefaspect( ctrl ) ) )
    m_DefAspect->SetSelection(3);

  m_GreenAspect->SetValue( wCtrl.isgreenaspect( ctrl ) );


  // Controllers
  initControllerList();

  m_Lib->Append( wxString( wDigInt.barjut, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.clock, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.ddx, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.dinamo, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.ecos, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.hsi88, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.inter10, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.lenz, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.loconet, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.mcs2, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.om32, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.opendcc, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.p50, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.p50x, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.railcom, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.roco, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.slx, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.srcp, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.vcs, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.zimo, wxConvUTF8 ) );

  m_Lib->SetSelection( 0 );
  m_Controller = NULL;

}

void RocrailIniDialog::initControllerList() {
  // Controllers
  m_Controllers->Clear();
  iONode controller = wRocRail.getdigint( m_Props );
  while( controller != NULL ) {
    char* val = StrOp.fmt( "%s - %s", wDigInt.getiid( controller ), wDigInt.getlib( controller ) );
    m_Controllers->Append( wxString( val, wxConvUTF8 ), (void*)controller );
    StrOp.free( val );

    controller = wRocRail.nextdigint( m_Props, controller );
  };
}

void RocrailIniDialog::event( iONode node ) {
  int cv       = wProgram.getcv (node);
  int val      = wProgram.getvalue(node);

  TraceOp.trc( "rocrailini", TRCLEVEL_INFO, __LINE__, 9999, "SO event %d=%d", cv, val );

  if( m_CSDialog != NULL ) {
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, PT_EVENT );
    // Make a copy of the node for using it out of this scope:
    event.SetClientData( node );
    wxPostEvent( m_CSDialog, event );
  }
}



void RocrailIniDialog::evaluate() {
  if( m_Props == NULL )
    return;
  // General
  wRocRail.setplanfile( m_Props, m_PlanFile->GetValue().mb_str(wxConvUTF8) );
  wRocRail.setlibpath( m_Props, m_LibPath->GetValue().mb_str(wxConvUTF8) );
  wRocRail.setimgpath( m_Props, m_ImgPath->GetValue().mb_str(wxConvUTF8) );
  iONode trace = wRocRail.gettrace( m_Props );
  wTrace.setrfile( trace, m_TraceFile->GetValue().mb_str(wxConvUTF8) );
  wRocRail.setptiid( m_Props, m_PTIID->GetValue().mb_str(wxConvUTF8) );
  wRocRail.setlciid( m_Props, m_LCIID->GetValue().mb_str(wxConvUTF8) );
  wRocRail.setdpiid( m_Props, m_DPIID->GetValue().mb_str(wxConvUTF8) );
  wRocRail.setsviid( m_Props, m_SVIID->GetValue().mb_str(wxConvUTF8) );

  wRocRail.setcreatemodplan( m_Props, m_PlanType->GetSelection() == 1 ? True:False );

  // Service
  iONode tcp = wRocRail.gettcp( m_Props );
  wTcp.setport( tcp, atoi( m_ClientPort->GetValue().mb_str(wxConvUTF8) ) );
  wTcp.setmaxcon( tcp, atoi( m_MaxCon->GetValue().mb_str(wxConvUTF8) ) );
  wTcp.setonlyfirstmaster( tcp, m_OnlyFirstMaster->IsChecked() ? True:False );

  iONode http = wRocRail.gethttp( m_Props );
  if( http == NULL ) {
    // add node:
    http = NodeOp.inst( wHttpService.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild( m_Props, http );
  }
  wHttpService.setport( http, atoi( m_HttpPort->GetValue().mb_str(wxConvUTF8) ) );
  wHttpService.setrefresh( http, atoi( m_ServiceRefresh->GetValue().mb_str(wxConvUTF8) ) );

  iONode web = wHttpService.getwebclient( http );
  if( web == NULL ) {
    // add node:
    web = NodeOp.inst( wWebClient.name(), http, ELEMENT_NODE );
    NodeOp.addChild( http, web );
  }
  wWebClient.setport( web, atoi( m_WebServicePort->GetValue().mb_str(wxConvUTF8) ) );
  wWebClient.setrefresh( web, atoi( m_WebServiceRefresh->GetValue().mb_str(wxConvUTF8) ) );

  iONode clock = wRocRail.getclock( m_Props );
  if( clock == NULL ) {
    // add node:
    clock = NodeOp.inst( wClock.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild( m_Props, clock );
  }
  wClock.setdivider( clock, atoi( m_Divider->GetValue().mb_str(wxConvUTF8) ) );
  wClock.sethour( clock, atoi( m_Hour->GetValue().mb_str(wxConvUTF8) ) );
  wClock.setminute( clock, atoi( m_Minute->GetValue().mb_str(wxConvUTF8) ) );

  // Trace
  trace = wRocRail.gettrace( m_Props );
  wTrace.setdebug( trace, m_Debug->IsChecked() ? True:False );
  wTrace.setbyte( trace, m_Byte->IsChecked() ? True:False );
  //wTrace.setparse( trace, m_Parse->IsChecked() ? True:False );
  wTrace.setautomatic( trace, m_Auto->IsChecked() ? True:False );
  wTrace.setmonitor( trace, m_Monitor->IsChecked() ? True:False );

  // Automat
  iONode ctrl = wRocRail.getctrl( m_Props );
  wCtrl.setswtimeout( ctrl, atoi( m_SwTime->GetValue().mb_str(wxConvUTF8) ) );
  wCtrl.setminbklc( ctrl, atoi( m_MinBKLen->GetValue().mb_str(wxConvUTF8) ) );
  wCtrl.setignevt( ctrl, atoi( m_IgnEvt->GetValue().mb_str(wxConvUTF8) ) );
  wCtrl.setinitfieldpause( ctrl, atoi( m_PauseInit->GetValue().mb_str(wxConvUTF8) ) );
  wCtrl.setseed( ctrl, m_Seed->GetValue() );
  wCtrl.setsavepostime( ctrl, m_SavePosTime->GetValue() );
  wCtrl.setswitchretrytime( ctrl, m_SwitchRetryTime->GetValue() );
  wCtrl.setswitchretrycount( ctrl, m_SwitchRetryCount->GetValue() );
  wCtrl.setcheck2in( ctrl, m_Check2In->IsChecked() ? True:False );
  wCtrl.setsecondnextblock( ctrl, m_secondNextBlock->IsChecked() ? True:False );
  wCtrl.setinitfieldpower( ctrl, m_InitfieldPower->IsChecked() ? True:False );
  wCtrl.setenableswfb( ctrl, m_EnableSwFb->IsChecked() ? True:False );
  wCtrl.seteventtimeout( ctrl, atoi( m_EventTimeout->GetValue().mb_str(wxConvUTF8) ) );
  wCtrl.setsignalreset( ctrl, m_SignalReset->GetValue() );
  wCtrl.setrouteswtime( ctrl, m_RouteSwTime->GetValue() );
  wCtrl.setdisablerouteVreduce( ctrl, m_DisableRouteVreduce->IsChecked() ? True:False  );
  wCtrl.setgreenaspect( ctrl, m_GreenAspect->IsChecked() ? True:False );
  wCtrl.setsemaphorewait( ctrl, m_SemaphoreWait->GetValue() );

  const char* defaspect[] = {wSignal.red,wSignal.green,wSignal.yellow,wSignal.white};
  wCtrl.setdefaspect( ctrl, defaspect[m_DefAspect->GetSelection()] );
  wCtrl.setskipsetsw( ctrl, m_SkipSetSw->IsChecked() ? True:False );
  wCtrl.setusebicom( ctrl, m_UseBiCom->IsChecked() ? True:False );
  wCtrl.setpoweroffonidentmismatch( ctrl, m_StopAtIdentMisMatch->IsChecked() ? True:False );


}




/*!
 * RocrailIniDialog creator
 */

bool RocrailIniDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin RocrailIniDialog member initialisation
    m_RRNotebook = NULL;
    m_GeneralPanel = NULL;
    m_LabelPlanfile = NULL;
    m_PlanFile = NULL;
    m_labPlanType = NULL;
    m_PlanType = NULL;
    m_LabelLibPath = NULL;
    m_LibPath = NULL;
    m_labImgPath = NULL;
    m_ImgPath = NULL;
    m_LabelTraceFile = NULL;
    m_TraceFile = NULL;
    m_labPTIID = NULL;
    m_PTIID = NULL;
    m_labLCIID = NULL;
    m_LCIID = NULL;
    m_labDPIID = NULL;
    m_DPIID = NULL;
    m_labSVIID = NULL;
    m_SVIID = NULL;
    m_TracePanel = NULL;
    m_TraceLevel = NULL;
    m_Auto = NULL;
    m_Monitor = NULL;
    m_DevTraceLevel = NULL;
    m_Byte = NULL;
    m_Debug = NULL;
    m_ServicePanel = NULL;
    m_HttpBox = NULL;
    m_LabelHttpPort = NULL;
    m_HttpPort = NULL;
    m_LabServiceRefresh = NULL;
    m_ServiceRefresh = NULL;
    m_WebServiceBox = NULL;
    m_LabWebServicePort = NULL;
    m_WebServicePort = NULL;
    m_LabWebServiceRefresh = NULL;
    m_WebServiceRefresh = NULL;
    m_ClockBox = NULL;
    m_labDevider = NULL;
    m_Divider = NULL;
    m_labHour = NULL;
    m_Hour = NULL;
    m_labMinute = NULL;
    m_Minute = NULL;
    m_ClientBox = NULL;
    m_LabelClientPort = NULL;
    m_ClientPort = NULL;
    m_LabelMaxCon = NULL;
    m_MaxCon = NULL;
    m_OnlyFirstMaster = NULL;
    m_AtomatPanel = NULL;
    m_LabelSwTime = NULL;
    m_SwTime = NULL;
    m_LabelMinBKLen = NULL;
    m_MinBKLen = NULL;
    m_LabelIgnEvt = NULL;
    m_IgnEvt = NULL;
    m_labInitPause = NULL;
    m_PauseInit = NULL;
    m_labInitPower = NULL;
    m_InitfieldPower = NULL;
    m_labSeed = NULL;
    m_Seed = NULL;
    m_labSavePosTime = NULL;
    m_SavePosTime = NULL;
    m_labSwitchRetryTime = NULL;
    m_SwitchRetryTime = NULL;
    m_labSwitchRetryCount = NULL;
    m_SwitchRetryCount = NULL;
    m_labEventTimeout = NULL;
    m_EventTimeout = NULL;
    m_labSignalReset = NULL;
    m_SignalReset = NULL;
    m_labRouteSwTime = NULL;
    m_RouteSwTime = NULL;
    m_labSemaphoreWait = NULL;
    m_SemaphoreWait = NULL;
    m_Check2In = NULL;
    m_secondNextBlock = NULL;
    m_EnableSwFb = NULL;
    m_DisableRouteVreduce = NULL;
    m_GreenAspect = NULL;
    m_SkipSetSw = NULL;
    m_UseBiCom = NULL;
    m_StopAtIdentMisMatch = NULL;
    m_DefAspect = NULL;
    m_ControllersPanel = NULL;
    m_Controllers = NULL;
    m_ControllerDelete = NULL;
    m_ControllerProps = NULL;
    m_AddControllerBox = NULL;
    m_Lib = NULL;
    m_ControllerAdd = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
    m_Apply = NULL;
////@end RocrailIniDialog member initialisation

////@begin RocrailIniDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end RocrailIniDialog creation
    return true;
}

/*!
 * Control creation for RocrailIniDialog
 */

void RocrailIniDialog::CreateControls()
{
////@begin RocrailIniDialog content construction
    RocrailIniDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_RRNotebook = new wxNotebook( itemDialog1, ID_NOTEBOOK_RR, wxDefaultPosition, wxDefaultSize, m_TabAlign );

    m_GeneralPanel = new wxPanel( m_RRNotebook, ID_PANEL_RR_INI, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxFlexGridSizer* itemFlexGridSizer5 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer5->AddGrowableCol(1);
    m_GeneralPanel->SetSizer(itemFlexGridSizer5);

    m_LabelPlanfile = new wxStaticText( m_GeneralPanel, wxID_STATIC_RR_PLANFILE, _("PlanFile"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_LabelPlanfile, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_PlanFile = new wxTextCtrl( m_GeneralPanel, ID_TEXTCTRL_RR_PLANFILE, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_PlanFile, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labPlanType = new wxStaticText( m_GeneralPanel, wxID_ANY, _("Plan type"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_labPlanType, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_PlanTypeStrings;
    m_PlanTypeStrings.Add(_("&Standard"));
    m_PlanTypeStrings.Add(_("&Modules"));
    m_PlanType = new wxRadioBox( m_GeneralPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, m_PlanTypeStrings, 1, wxRA_SPECIFY_ROWS );
    m_PlanType->SetSelection(0);
    itemFlexGridSizer5->Add(m_PlanType, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_LabelLibPath = new wxStaticText( m_GeneralPanel, wxID_STATIC_RR_LIBPATH, _("LibPath"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_LabelLibPath, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_LibPath = new wxTextCtrl( m_GeneralPanel, ID_TEXTCTRL_RR_LIBPATH, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_LibPath, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labImgPath = new wxStaticText( m_GeneralPanel, wxID_ANY, _("ImgPath"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_labImgPath, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ImgPath = new wxTextCtrl( m_GeneralPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_ImgPath, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelTraceFile = new wxStaticText( m_GeneralPanel, wxID_STATIC_RR_TRACEFILE, _("TraceFile"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_LabelTraceFile, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_TraceFile = new wxTextCtrl( m_GeneralPanel, ID_TEXTCTRL_RR_TRACEFILE, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_TraceFile, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labPTIID = new wxStaticText( m_GeneralPanel, wxID_ANY, _("PT IID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_labPTIID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_PTIID = new wxTextCtrl( m_GeneralPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_PTIID, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labLCIID = new wxStaticText( m_GeneralPanel, wxID_ANY, _("LC IID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_labLCIID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LCIID = new wxTextCtrl( m_GeneralPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_LCIID, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labDPIID = new wxStaticText( m_GeneralPanel, wxID_ANY, _("DP IID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_labDPIID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_DPIID = new wxTextCtrl( m_GeneralPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_DPIID, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labSVIID = new wxStaticText( m_GeneralPanel, wxID_ANY, _("SV IID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_labSVIID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SVIID = new wxTextCtrl( m_GeneralPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_SVIID, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_RRNotebook->AddPage(m_GeneralPanel, _("General"));

    m_TracePanel = new wxPanel( m_RRNotebook, ID_PANEL_RR_TRACE, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer25 = new wxBoxSizer(wxVERTICAL);
    m_TracePanel->SetSizer(itemBoxSizer25);

    wxStaticBox* itemStaticBoxSizer26Static = new wxStaticBox(m_TracePanel, wxID_ANY, _("User Levels"));
    m_TraceLevel = new wxStaticBoxSizer(itemStaticBoxSizer26Static, wxVERTICAL);
    itemBoxSizer25->Add(m_TraceLevel, 0, wxGROW|wxALL, 5);
    m_Auto = new wxCheckBox( m_TracePanel, ID_CHECKBOX_RR_AUTO, _("Automatic"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_Auto->SetValue(false);
    m_TraceLevel->Add(m_Auto, 0, wxALIGN_LEFT|wxALL, 5);

    m_Monitor = new wxCheckBox( m_TracePanel, ID_CHECKBOX_RR_MONITOR, _("Monitor"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Monitor->SetValue(false);
    m_TraceLevel->Add(m_Monitor, 0, wxALIGN_LEFT|wxALL, 5);

    m_DevTraceLevel = new wxStaticBox(m_TracePanel, wxID_ANY, _("Developer Levels"));
    wxStaticBoxSizer* itemStaticBoxSizer29 = new wxStaticBoxSizer(m_DevTraceLevel, wxVERTICAL);
    itemBoxSizer25->Add(itemStaticBoxSizer29, 0, wxGROW|wxALL, 5);
    m_Byte = new wxCheckBox( m_TracePanel, ID_CHECKBOX_RR_BYTE, _("Byte"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_Byte->SetValue(false);
    itemStaticBoxSizer29->Add(m_Byte, 0, wxALIGN_LEFT|wxALL, 5);

    m_Debug = new wxCheckBox( m_TracePanel, ID_CHECKBOX_RR_DEBUG, _("Debug"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_Debug->SetValue(false);
    itemStaticBoxSizer29->Add(m_Debug, 0, wxALIGN_LEFT|wxALL, 5);

    m_RRNotebook->AddPage(m_TracePanel, _("Trace"));

    m_ServicePanel = new wxPanel( m_RRNotebook, ID_PANEL_RR_SERVICE, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer33 = new wxBoxSizer(wxVERTICAL);
    m_ServicePanel->SetSizer(itemBoxSizer33);

    wxStaticBox* itemStaticBoxSizer34Static = new wxStaticBox(m_ServicePanel, wxID_ANY, _("HTTP Service"));
    m_HttpBox = new wxStaticBoxSizer(itemStaticBoxSizer34Static, wxHORIZONTAL);
    itemBoxSizer33->Add(m_HttpBox, 0, wxGROW|wxALL, 5);
    m_LabelHttpPort = new wxStaticText( m_ServicePanel, wxID_STATIC_RR_HTTPPORT, _("Port"), wxDefaultPosition, wxDefaultSize, 0 );
    m_HttpBox->Add(m_LabelHttpPort, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_HttpPort = new wxTextCtrl( m_ServicePanel, ID_TEXTCTRL_RR_HTTPPORT, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_HttpPort->SetMaxLength(5);
    m_HttpBox->Add(m_HttpPort, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabServiceRefresh = new wxStaticText( m_ServicePanel, wxID_STATIC_RR_SEVICE_REFRESH, _("Refresh"), wxDefaultPosition, wxDefaultSize, 0 );
    m_HttpBox->Add(m_LabServiceRefresh, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_ServiceRefresh = new wxTextCtrl( m_ServicePanel, ID_TEXTCTRL_RR_SERVICE_REFRESH, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_HttpBox->Add(m_ServiceRefresh, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer39Static = new wxStaticBox(m_ServicePanel, wxID_ANY, _("Web Service"));
    m_WebServiceBox = new wxStaticBoxSizer(itemStaticBoxSizer39Static, wxHORIZONTAL);
    itemBoxSizer33->Add(m_WebServiceBox, 0, wxGROW|wxALL, 5);
    m_LabWebServicePort = new wxStaticText( m_ServicePanel, wxID_STATIC_RR_WEBSERVICE_PORT, _("Port"), wxDefaultPosition, wxDefaultSize, 0 );
    m_WebServiceBox->Add(m_LabWebServicePort, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_WebServicePort = new wxTextCtrl( m_ServicePanel, ID_TEXTCTRL_RR_WEBSERVICE_PORT, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_WebServicePort->SetMaxLength(5);
    m_WebServiceBox->Add(m_WebServicePort, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabWebServiceRefresh = new wxStaticText( m_ServicePanel, wxID_STATIC_RR_WEBSERVICE_REFRESH, _("Refresh"), wxDefaultPosition, wxDefaultSize, 0 );
    m_WebServiceBox->Add(m_LabWebServiceRefresh, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_WebServiceRefresh = new wxTextCtrl( m_ServicePanel, ID_TEXTCTRL_RR_WEBSERVICE_REFRESH, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_WebServiceBox->Add(m_WebServiceRefresh, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer44Static = new wxStaticBox(m_ServicePanel, wxID_ANY, _("Clock Service"));
    m_ClockBox = new wxStaticBoxSizer(itemStaticBoxSizer44Static, wxHORIZONTAL);
    itemBoxSizer33->Add(m_ClockBox, 0, wxGROW|wxALL, 5);
    m_labDevider = new wxStaticText( m_ServicePanel, wxID_STATIC_RR_CLOCK_DEVIDER, _("divider"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ClockBox->Add(m_labDevider, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Divider = new wxTextCtrl( m_ServicePanel, ID_TEXTCTRL_RR_CLOCK_DEVIDER, _("1"), wxDefaultPosition, wxSize(40, -1), wxTE_CENTRE );
    m_Divider->SetMaxLength(2);
    m_ClockBox->Add(m_Divider, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labHour = new wxStaticText( m_ServicePanel, wxID_STATIC_RR_CLOCK_HOUR, _("hour"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ClockBox->Add(m_labHour, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Hour = new wxTextCtrl( m_ServicePanel, ID_TEXTCTRL_RR_CLOCK_HOUR, _("24"), wxDefaultPosition, wxSize(40, -1), wxTE_CENTRE );
    m_Hour->SetMaxLength(2);
    if (RocrailIniDialog::ShowToolTips())
        m_Hour->SetToolTip(_("24 disabled the start setting"));
    m_ClockBox->Add(m_Hour, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labMinute = new wxStaticText( m_ServicePanel, wxID_STATIC_RR_CLOCK_MINUTE, _("minute"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ClockBox->Add(m_labMinute, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Minute = new wxTextCtrl( m_ServicePanel, ID_TEXTCTRL_RR_CLOCK_MINUTE, _("60"), wxDefaultPosition, wxSize(40, -1), wxTE_CENTRE );
    m_Minute->SetMaxLength(2);
    if (RocrailIniDialog::ShowToolTips())
        m_Minute->SetToolTip(_("60 disabled the start setting"));
    m_ClockBox->Add(m_Minute, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer51Static = new wxStaticBox(m_ServicePanel, wxID_ANY, _("Client Service"));
    m_ClientBox = new wxStaticBoxSizer(itemStaticBoxSizer51Static, wxVERTICAL);
    itemBoxSizer33->Add(m_ClientBox, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer52 = new wxFlexGridSizer(2, 2, 0, 0);
    m_ClientBox->Add(itemFlexGridSizer52, 0, wxGROW|wxALL, 5);
    m_LabelClientPort = new wxStaticText( m_ServicePanel, wxID_STATIC_RR_CLIENTPORT, _("port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer52->Add(m_LabelClientPort, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_ClientPort = new wxTextCtrl( m_ServicePanel, ID_TEXTCTRL_RR_CLIENTPORT, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_ClientPort->SetMaxLength(5);
    itemFlexGridSizer52->Add(m_ClientPort, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelMaxCon = new wxStaticText( m_ServicePanel, wxID_STATIC_RR_MAXCON, _("max."), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer52->Add(m_LabelMaxCon, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_MaxCon = new wxTextCtrl( m_ServicePanel, ID_TEXTCTRL_RR_MAXCONN, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_MaxCon->SetMaxLength(2);
    itemFlexGridSizer52->Add(m_MaxCon, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer57 = new wxFlexGridSizer(2, 1, 0, 0);
    m_ClientBox->Add(itemFlexGridSizer57, 0, wxGROW|wxALL, 5);
    m_OnlyFirstMaster = new wxCheckBox( m_ServicePanel, wxID_ANY, _("Only first is master"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OnlyFirstMaster->SetValue(false);
    itemFlexGridSizer57->Add(m_OnlyFirstMaster, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_RRNotebook->AddPage(m_ServicePanel, _("Service"));

    m_AtomatPanel = new wxPanel( m_RRNotebook, ID_PANEL_RR_AUTO, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer60 = new wxBoxSizer(wxVERTICAL);
    m_AtomatPanel->SetSizer(itemBoxSizer60);

    wxFlexGridSizer* itemFlexGridSizer61 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer61->AddGrowableCol(1);
    itemBoxSizer60->Add(itemFlexGridSizer61, 0, wxGROW, 5);
    m_LabelSwTime = new wxStaticText( m_AtomatPanel, wxID_STATIC_RR_SWTIME, _("Switch time"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer61->Add(m_LabelSwTime, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_SwTime = new wxTextCtrl( m_AtomatPanel, ID_TEXTCTRL_RR_SWTIME, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer61->Add(m_SwTime, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_LabelMinBKLen = new wxStaticText( m_AtomatPanel, wxID_STATIC__MINBKLEN, _("Min Block length"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer61->Add(m_LabelMinBKLen, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 5);

    m_MinBKLen = new wxTextCtrl( m_AtomatPanel, ID_TEXTCTRL_RR_MINBKLEN, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer61->Add(m_MinBKLen, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_LabelIgnEvt = new wxStaticText( m_AtomatPanel, wxID_STATIC_RR_IGNEVT, _("Ignore dup. sensor events"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer61->Add(m_LabelIgnEvt, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 5);

    m_IgnEvt = new wxTextCtrl( m_AtomatPanel, ID_TEXTCTRL_RR_IGNEVT, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer61->Add(m_IgnEvt, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labInitPause = new wxStaticText( m_AtomatPanel, wxID_STATIC_RR_INITPAUSE, _("Initfield pause"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer61->Add(m_labInitPause, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 5);

    m_PauseInit = new wxTextCtrl( m_AtomatPanel, ID_TEXTCTRL_RR_PAUSEINIT, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer61->Add(m_PauseInit, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labInitPower = new wxStaticText( m_AtomatPanel, wxID_ANY, _("Initfield power on"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer61->Add(m_labInitPower, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_InitfieldPower = new wxCheckBox( m_AtomatPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_InitfieldPower->SetValue(false);
    itemFlexGridSizer61->Add(m_InitfieldPower, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labSeed = new wxStaticText( m_AtomatPanel, wxID_ANY, _("Random seed"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer61->Add(m_labSeed, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Seed = new wxSpinCtrl( m_AtomatPanel, wxID_ANY, _T("4711"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 65535, 4711 );
    itemFlexGridSizer61->Add(m_Seed, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labSavePosTime = new wxStaticText( m_AtomatPanel, wxID_ANY, _("Save position time"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer61->Add(m_labSavePosTime, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_SavePosTime = new wxSpinCtrl( m_AtomatPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer61->Add(m_SavePosTime, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labSwitchRetryTime = new wxStaticText( m_AtomatPanel, wxID_ANY, _("Switch retry time"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer61->Add(m_labSwitchRetryTime, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_SwitchRetryTime = new wxSpinCtrl( m_AtomatPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer61->Add(m_SwitchRetryTime, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labSwitchRetryCount = new wxStaticText( m_AtomatPanel, wxID_ANY, _("Switch retry count"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer61->Add(m_labSwitchRetryCount, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_SwitchRetryCount = new wxSpinCtrl( m_AtomatPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer61->Add(m_SwitchRetryCount, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labEventTimeout = new wxStaticText( m_AtomatPanel, wxID_ANY, _("Event timeout"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer61->Add(m_labEventTimeout, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EventTimeout = new wxTextCtrl( m_AtomatPanel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer61->Add(m_EventTimeout, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labSignalReset = new wxStaticText( m_AtomatPanel, wxID_ANY, _("Signal reset"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer61->Add(m_labSignalReset, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_SignalReset = new wxSpinCtrl( m_AtomatPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 10, 0 );
    itemFlexGridSizer61->Add(m_SignalReset, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labRouteSwTime = new wxStaticText( m_AtomatPanel, wxID_ANY, _("Route switch time"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer61->Add(m_labRouteSwTime, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_RouteSwTime = new wxSpinCtrl( m_AtomatPanel, wxID_ANY, _T("10"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 10, 10000, 10 );
    itemFlexGridSizer61->Add(m_RouteSwTime, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labSemaphoreWait = new wxStaticText( m_AtomatPanel, wxID_ANY, _("Semaphore wait"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer61->Add(m_labSemaphoreWait, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_SemaphoreWait = new wxSpinCtrl( m_AtomatPanel, wxID_ANY, _T("1"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 10, 1 );
    itemFlexGridSizer61->Add(m_SemaphoreWait, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxFlexGridSizer* itemFlexGridSizer84 = new wxFlexGridSizer(2, 1, 0, 0);
    itemBoxSizer60->Add(itemFlexGridSizer84, 0, wxGROW, 5);
    m_Check2In = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Check for free destination until reaching IN"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Check2In->SetValue(false);
    itemFlexGridSizer84->Add(m_Check2In, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_secondNextBlock = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Reserve second next block"), wxDefaultPosition, wxDefaultSize, 0 );
    m_secondNextBlock->SetValue(false);
    itemFlexGridSizer84->Add(m_secondNextBlock, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EnableSwFb = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Enable Switch Feedback"), wxDefaultPosition, wxDefaultSize, 0 );
    m_EnableSwFb->SetValue(false);
    itemFlexGridSizer84->Add(m_EnableSwFb, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_DisableRouteVreduce = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Disable route speed reduce"), wxDefaultPosition, wxDefaultSize, 0 );
    m_DisableRouteVreduce->SetValue(false);
    itemFlexGridSizer84->Add(m_DisableRouteVreduce, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_GreenAspect = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Green aspect if next is red"), wxDefaultPosition, wxDefaultSize, 0 );
    m_GreenAspect->SetValue(false);
    itemFlexGridSizer84->Add(m_GreenAspect, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_SkipSetSw = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Skip switch commands in routes if already set"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SkipSetSw->SetValue(false);
    itemFlexGridSizer84->Add(m_SkipSetSw, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_UseBiCom = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Use Bi-Directional Communication"), wxDefaultPosition, wxDefaultSize, 0 );
    m_UseBiCom->SetValue(false);
    itemFlexGridSizer84->Add(m_UseBiCom, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_StopAtIdentMisMatch = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("power off at ident mismatch "), wxDefaultPosition, wxDefaultSize, 0 );
    m_StopAtIdentMisMatch->SetValue(false);
    itemFlexGridSizer84->Add(m_StopAtIdentMisMatch, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxArrayString m_DefAspectStrings;
    m_DefAspectStrings.Add(_("&red"));
    m_DefAspectStrings.Add(_("&green"));
    m_DefAspectStrings.Add(_("&yellow"));
    m_DefAspectStrings.Add(_("&white"));
    m_DefAspect = new wxRadioBox( m_AtomatPanel, wxID_ANY, _("Default Signal Aspect"), wxDefaultPosition, wxDefaultSize, m_DefAspectStrings, 1, wxRA_SPECIFY_ROWS );
    m_DefAspect->SetSelection(0);
    itemFlexGridSizer84->Add(m_DefAspect, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_RRNotebook->AddPage(m_AtomatPanel, _("Automat"));

    m_ControllersPanel = new wxPanel( m_RRNotebook, ID_PANEL_RR_CONTROLLERS, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer95 = new wxBoxSizer(wxVERTICAL);
    m_ControllersPanel->SetSizer(itemBoxSizer95);

    wxArrayString m_ControllersStrings;
    m_Controllers = new wxListBox( m_ControllersPanel, ID_LISTBOX_RR_CONTROLLERS, wxDefaultPosition, wxDefaultSize, m_ControllersStrings, wxLB_SINGLE );
    itemBoxSizer95->Add(m_Controllers, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer97 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer95->Add(itemBoxSizer97, 0, wxGROW|wxALL, 5);
    m_ControllerDelete = new wxButton( m_ControllersPanel, ID_BUTTON_RR_DELETE, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer97->Add(m_ControllerDelete, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ControllerProps = new wxButton( m_ControllersPanel, ID_BUTTON_RR_PROPS, _("Properties"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer97->Add(m_ControllerProps, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer100Static = new wxStaticBox(m_ControllersPanel, wxID_ANY, _("Add Controller"));
    m_AddControllerBox = new wxStaticBoxSizer(itemStaticBoxSizer100Static, wxHORIZONTAL);
    itemBoxSizer95->Add(m_AddControllerBox, 0, wxGROW|wxALL, 5);
    wxArrayString m_LibStrings;
    m_Lib = new wxComboBox( m_ControllersPanel, ID_COMBOBOX_RR_LIB, _T(""), wxDefaultPosition, wxDefaultSize, m_LibStrings, wxCB_DROPDOWN );
    m_AddControllerBox->Add(m_Lib, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ControllerAdd = new wxButton( m_ControllersPanel, ID_BUTTON_RR_ADD, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    m_AddControllerBox->Add(m_ControllerAdd, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_RRNotebook->AddPage(m_ControllersPanel, _("Controllers"));

    itemBoxSizer2->Add(m_RRNotebook, 1, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer103 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer103, 0, wxALIGN_RIGHT|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer103->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer103->AddButton(m_Cancel);

    m_Apply = new wxButton( itemDialog1, wxID_APPLY, _("&Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    if (RocrailIniDialog::ShowToolTips())
        m_Apply->SetToolTip(_("Apply Controller settings locally."));
    itemStdDialogButtonSizer103->AddButton(m_Apply);

    itemStdDialogButtonSizer103->Realize();

////@end RocrailIniDialog content construction
}

/*!
 * Should we show tooltips?
 */

bool RocrailIniDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap RocrailIniDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin RocrailIniDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end RocrailIniDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon RocrailIniDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin RocrailIniDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end RocrailIniDialog icon retrieval
}
/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_RR_CONTROLLERS
 */

void RocrailIniDialog::OnListboxRrControllersSelected( wxCommandEvent& event )
{
  int sel = m_Controllers->GetSelection();
  if( sel != wxNOT_FOUND ) {
    m_Controller = (iONode)m_Controllers->GetClientData( sel );
  }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_RR_ADD
 */

void RocrailIniDialog::OnButtonRrAddClick( wxCommandEvent& event )
{
  iONode digint = NodeOp.inst( wDigInt.name(), m_Props, ELEMENT_NODE );
  NodeOp.addChild( m_Props, digint );
  wDigInt.setiid( digint, "NEW" );
  wDigInt.setlib( digint, m_Lib->GetStringSelection().mb_str(wxConvUTF8) );
  m_Controller = digint;
  char* val = StrOp.fmt( "%s - %s", wDigInt.getiid( digint ), wDigInt.getlib( digint ) );
  m_Controllers->Append( wxString( val, wxConvUTF8 ), (void*)digint );
  StrOp.free( val );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_RR_DELETE
 */

void RocrailIniDialog::OnButtonRrDeleteClick( wxCommandEvent& event )
{
  int sel = m_Controllers->GetSelection();
  if( sel != wxNOT_FOUND ) {
    m_Controller = (iONode)m_Controllers->GetClientData( sel );
    NodeOp.removeChild( m_Props, m_Controller );
    m_Controller = NULL;
    initControllerList();
  }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_RR_PROPS
 */

void RocrailIniDialog::OnButtonRrPropsClick( wxCommandEvent& event )
{
  m_CSDialog = NULL;
  if( m_Controller == NULL )
    return;

  if( StrOp.equals( wDigInt.ddx, wDigInt.getlib( m_Controller ) ) )
    m_CSDialog = new DDXCtrlDlg(this,m_Controller);
  else if( StrOp.equals( wDigInt.srcp, wDigInt.getlib( m_Controller ) ) )
    m_CSDialog = new SRCPCtrlDlg(this,m_Controller);
  else if( StrOp.equals( wDigInt.dinamo, wDigInt.getlib( m_Controller ) ) )
    m_CSDialog = new DinamoCtrlDlg(this,m_Controller);
  else if( StrOp.equals( wDigInt.hsi88, wDigInt.getlib( m_Controller ) ) )
    m_CSDialog = new Hsi88CntrlDlg(this,m_Controller);
  else if( StrOp.equals( wDigInt.barjut, wDigInt.getlib( m_Controller ) ) )
    m_CSDialog = new BarJuTCntrlDlg(this,m_Controller);
  else if( StrOp.equals( wDigInt.loconet, wDigInt.getlib( m_Controller ) ) )
    m_CSDialog = new LocoNetCtrlDlg(this,m_Controller);
  else if( StrOp.equals( wDigInt.ecos, wDigInt.getlib( m_Controller ) ) )
    m_CSDialog = new ECoSCtrlDialog(this,m_Controller);
  else if( StrOp.equals( wDigInt.opendcc, wDigInt.getlib( m_Controller ) ) )
    m_CSDialog = new OpenDCCCtrlDlg(this,m_Controller);
  else if( StrOp.equals( wDigInt.mcs2, wDigInt.getlib( m_Controller ) ) )
    m_CSDialog = new ECoSCtrlDialog(this,m_Controller);
  else if( StrOp.equals( wDigInt.lenz, wDigInt.getlib( m_Controller ) ) )
    m_CSDialog = new LenzDlg(this,m_Controller);
  else if( StrOp.equals( wDigInt.roco, wDigInt.getlib( m_Controller ) ) )
    m_CSDialog = new GenericCtrlDlg(this,m_Controller,wDigInt.getlib( m_Controller ));
  else
    m_CSDialog = new GenericCtrlDlg(this,m_Controller,wDigInt.getlib( m_Controller ));

  if( wxID_OK == m_CSDialog->ShowModal() ) {
    initControllerList();
  }
  m_CSDialog->Destroy();
  m_CSDialog = NULL;

}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void RocrailIniDialog::OnOkClick( wxCommandEvent& event )
{
  evaluate();
  /* Notify RocRail. */
  iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
  wSysCmd.setcmd( cmd, wSysCmd.setini );
  NodeOp.addChild( cmd, (iONode)m_Props->base.clone( m_Props ) );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);

  EndModal( wxID_OK );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void RocrailIniDialog::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
 */

void RocrailIniDialog::OnApplyClick( wxCommandEvent& event )
{
  evaluate();
}


/*!
 * wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED event handler for ID_NOTEBOOK_RR
 */

void RocrailIniDialog::OnNotebookRrPageChanged( wxNotebookEvent& event )
{
}


