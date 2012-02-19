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
#include "wx/imaglist.h"
////@end includes

#include "rocrailinidialog.h"
#include "rocview/dialogs/controllers/gendlg.h"
#include "rocview/dialogs/controllers/ddxdlg.h"
#include "rocview/dialogs/controllers/hsi88dlg.h"
#include "rocview/dialogs/controllers/barjutdlg.h"
#include "rocview/dialogs/controllers/dinamodlg.h"
#include "rocview/dialogs/controllers/srcpdlg.h"
#include "rocview/dialogs/controllers/loconetdlg.h"
#include "rocview/dialogs/controllers/ecosdlg.h"
#include "rocview/dialogs/controllers/opendcc.h"
#include "rocview/dialogs/controllers/lenzdlg.h"
#include "rocview/dialogs/controllers/dcc232dlg.h"
#include "rocview/dialogs/controllers/bidibdlg.h"
#include "rocview/dialogs/controllers/rocnetdlg.h"
#include "rocview/dialogs/controllers/cbusdlg.h"

#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Clock.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Trace.h"
#include "rocrail/wrapper/public/HttpService.h"
#include "rocrail/wrapper/public/WebClient.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/Tcp.h"
#include "rocrail/wrapper/public/SrcpCon.h"
#include "rocrail/wrapper/public/Ctrl.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/R2RnetIni.h"
#include "rocrail/wrapper/public/SnmpService.h"

#include "rocview/wrapper/public/Gui.h"

#include "rocview/public/guiapp.h"

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

    EVT_BUTTON( ID_BUTTON_R2RNET_ROUTES, RocrailIniDialog::OnButtonR2rnetRoutesClick )

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
  m_Debug->Enable( false );


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

/* comparator for sorting by id: */
static int __sortStr(obj* _a, obj* _b)
{
    const char* a = (const char*)*_a;
    const char* b = (const char*)*_b;
    return strcmp( a, b );
}

void RocrailIniDialog::initLabels() {
  m_RRNotebook->SetPageText( 0, wxGetApp().getMsg( "general" ) );
  m_RRNotebook->SetPageText( 1, wxGetApp().getMsg( "trace" ) );
  m_RRNotebook->SetPageText( 2, wxGetApp().getMsg( "service" ) );
  m_RRNotebook->SetPageText( 3, wxGetApp().getMsg( "auto" ) );
  m_RRNotebook->SetPageText( 4, wxGetApp().getMsg( "controller" ) );

  // General
  m_LabelPlanfile->SetLabel( wxGetApp().getMsg( "planfile" ) );
  m_labOccFile->SetLabel( wxGetApp().getMsg( "blockoccupancy" ) );
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
  m_labScSensor->SetLabel( wxGetApp().getMsg( "sensor" ) );
  m_ScBox->SetLabel( wxGetApp().getMsg( "shortcut" ) );


  iONode model = wxGetApp().getModel();
  iOList list = ListOp.inst();
  if( model != NULL ) {
    iONode fblist = wPlan.getfblist( model );
    if( fblist != NULL ) {
      int cnt = NodeOp.getChildCnt( fblist );
      for( int i = 0; i < cnt; i++ ) {
        iONode fb = NodeOp.getChild( fblist, i );
        const char* id = wFeedback.getid( fb );
        if( id != NULL ) {
          ListOp.add(list, (obj)id);
        }
      }
      ListOp.sort(list, &__sortStr);
      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        const char* id = (const char*)ListOp.get( list, i );
        m_ScSensor->Append( wxString(id,wxConvUTF8) );
      }
    }
  }
  /* clean up the temp. list */
  ListOp.base.del(list);



  m_PlanType->SetString( 0, wxGetApp().getMsg( "default" ) );
  m_PlanType->SetString( 1, wxGetApp().getMsg( "modpanel" ) );


  // Trace
  m_TraceLevel->GetStaticBox()->SetLabel( _T("User ") + wxGetApp().getMsg( "tracelevel" ) );
  m_DevTraceLevel->SetLabel( _T("Developer ") + wxGetApp().getMsg( "tracelevel" ) );
  m_Debug->SetLabel( wxGetApp().getMsg( "debug" ) );
  m_Info->SetLabel( wxGetApp().getMsg( "info" ) );
  m_Auto->SetLabel( wxGetApp().getMsg( "auto" ) );
  m_Byte->SetLabel( wxGetApp().getMsg( "byte" ) );
  m_SendAllTraces->SetLabel( wxGetApp().getMsg( "sendalltraces" ) );
  //m_Parse->SetLabel( wxGetApp().getMsg( "parse" ) );

  // Service
  m_LabelClientPort->SetLabel( wxGetApp().getMsg( "clientport" ) );
  m_LabelMaxCon->SetLabel( wxGetApp().getMsg( "maxclientcon" ) );
  m_labControlCode->SetLabel( wxGetApp().getMsg( "controlcode" ) );
  m_HttpBox->GetStaticBox()->SetLabel( wxGetApp().getMsg( "http" ) );
  m_LabelHttpPort->SetLabel( wxGetApp().getMsg( "port" ) );
  m_LabServiceRefresh->SetLabel( wxGetApp().getMsg( "refresh" ) );
  m_OnlyFirstMaster->SetLabel( wxGetApp().getMsg( "onlyfirstmaster" ) );
  m_labSrcpPort->SetLabel( wxGetApp().getMsg( "port" ) );
  m_SrcpServiceActive->SetLabel( wxGetApp().getMsg( "enable" ) );
  m_labSnmpPort->SetLabel( wxGetApp().getMsg( "port" ) );
  m_SnmpServiceActive->SetLabel( wxGetApp().getMsg( "enable" ) );
  m_labSnmpTrapHost->SetLabel( wxGetApp().getMsg( "host" ) );
  m_labSnmpTrapPort->SetLabel( wxGetApp().getMsg( "port" ) );

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
  m_SkipSetSg->SetLabel(wxGetApp().getMsg( "skipsetsg" ));
  m_SkipSetSg->SetToolTip(wxGetApp().getTip( "skipsetsg" ));
  m_UseBiCom->SetLabel( wxGetApp().getMsg( "usebicom" ) );
  m_labSemaphoreWait->SetLabel( wxGetApp().getMsg( "semaphorewait" ) );
  m_labSignalWait->SetLabel( wxGetApp().getMsg( "signalwait" ) );
  m_StopAtIdentMisMatch->SetLabel( wxGetApp().getMsg( "stopatidentmismatch" ) );
  m_PowerOffAtGhost->SetLabel( wxGetApp().getMsg( "poweroffatghost" ) );
  m_KeepGhost->SetLabel( wxGetApp().getMsg( "keepghost" ) );
  m_labLocoStartGap->SetLabel( wxGetApp().getMsg( "locostartgap" ) );
  m_DisableSteal->SetLabel( wxGetApp().getMsg( "disablesteal" ) );
  m_labBlockInitPause->SetLabel( wxGetApp().getMsg( "blockinitpause" ) );
  m_PowerOffAtReset->SetLabel( wxGetApp().getMsg( "poweroffatreset" ) );
  m_ZeroThrottleID->SetLabel( wxGetApp().getMsg( "allowzerothrottleid" ) );
  m_UseBlockSide->SetLabel( wxGetApp().getMsg( "useblockside" ) );
  m_StopNoneCommuter->SetLabel( wxGetApp().getMsg( "stopnonecommuter" ) );
  m_SyncRoutes->SetLabel( wxGetApp().getMsg( "syncroutes" ) );
  m_labSyncRouteTimeout->SetLabel( wxGetApp().getMsg( "syncroutetimeout" ) );
  m_TimedSensors->SetLabel( wxGetApp().getMsg( "timedsensors" ) );
  m_EnableAnalyzer->SetLabel( wxGetApp().getMsg( "enableanalyzer" ) );
  m_ForceUnlock->SetLabel( wxGetApp().getMsg( "forceunlockatebreak" ) );
  m_TriggerBlockEvents->SetLabel( wxGetApp().getMsg( "triggerblockevents" ) );
  m_ResetFxSp->SetLabel( wxGetApp().getMsg( "resetspfx" ) );
  m_ReleaseOnIdle->SetLabel( wxGetApp().getMsg( "releaseonidle" ) );
  m_CloseOnGhost->SetLabel( wxGetApp().getMsg( "closeonghost" ) );

  // Controller
  m_ControllerDelete->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_ControllerProps->SetLabel( wxGetApp().getMsg( "properties" ) );
  m_ControllerAdd->SetLabel( wxGetApp().getMsg( "add" ) );
  m_AddControllerBox->GetStaticBox()->SetLabel( wxGetApp().getMsg( "new" ) );
  m_PowerOffOnExit->SetLabel( wxGetApp().getMsg( "poweroffonexit" ) );


  // R2Rnet
  m_labR2RnetID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_labR2RnetAddr->SetLabel( wxGetApp().getMsg( "address" ) );
  m_labR2RnetPort->SetLabel( wxGetApp().getMsg( "port" ) );
  m_labR2RnetRoutes->SetLabel( wxGetApp().getMsg( "netroutes" ) );
  m_R2RnetEnable->SetLabel( wxGetApp().getMsg( "enable" ) );

  // Buttons
  m_OK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
  m_Apply->SetLabel( wxGetApp().getMsg( "apply" ) );
}


void RocrailIniDialog::initValues() {
  // General
  m_PlanFile->SetValue( wxString(wRocRail.getplanfile( m_Props ),wxConvUTF8) );
  m_OccFile->SetValue( wxString( wRocRail.getoccupancy( m_Props ),wxConvUTF8 ) );
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

  if( wRocRail.getscsensor( m_Props ) != NULL && StrOp.len( wRocRail.getscsensor( m_Props ) ) > 0 )
      m_ScSensor->SetStringSelection( wxString(wRocRail.getscsensor( m_Props ),wxConvUTF8) );
  else
    m_ScSensor->SetStringSelection( wxString("-",wxConvUTF8) );

  m_ScIID->SetValue( wxString(wRocRail.getsciid( m_Props ),wxConvUTF8) );

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
  m_ControlCode->SetValue( wxString( wTcp.getcontrolcode(tcp),wxConvUTF8) );

  m_OnlyFirstMaster->SetValue( wTcp.isonlyfirstmaster( tcp ) ? true:false );

  iONode srcpcon = wRocRail.getsrcpcon( m_Props );
  if( srcpcon == NULL ) {
    srcpcon = NodeOp.inst( wSrcpCon.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild( m_Props, srcpcon );
  }
  m_SrcpPort->SetValue(wSrcpCon.getport(srcpcon));
  m_SrcpServiceActive->SetValue(wSrcpCon.isactive(srcpcon)?true:false);


  iONode snmp = wRocRail.getSnmpService( m_Props );
  if( snmp == NULL ) {
    snmp = NodeOp.inst( wSnmpService.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild( m_Props, snmp );
  }
  m_SnmpPort->SetValue(wSnmpService.getport(snmp));
  m_SnmpServiceActive->SetValue(wSnmpService.isactive(snmp)?true:false);
  m_SnmpTrapHost->SetValue( wxString( wSnmpService.gettraphost(snmp), wxConvUTF8) );
  m_SnmpTrapPort->SetValue(wSnmpService.gettrapport(snmp));


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
  m_Info->SetValue( wTrace.isinfo( trace ) );
  m_Byte->SetValue( wTrace.isbyte( trace ) );
  //m_Parse->SetValue( wTrace.isparse( trace ) );
  m_Auto->SetValue( wTrace.isautomatic( trace ) );
  m_Monitor->SetValue( wTrace.ismonitor( trace ) );
  m_SendAllTraces->SetValue( wTrace.islisten2all( trace ) );

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
  m_SkipSetSg->SetValue( wCtrl.isskipsetsg( ctrl ) );
  m_UseBiCom->SetValue( wCtrl.isusebicom( ctrl ) );
  m_SemaphoreWait->SetValue( wCtrl.getsemaphorewait( ctrl ) );
  m_SignalWait->SetValue( wCtrl.getsignalwait( ctrl ) );
  m_StopAtIdentMisMatch->SetValue( wCtrl.ispoweroffonidentmismatch( ctrl ) );
  m_PowerOffAtGhost->SetValue( wCtrl.ispoweroffatghost( ctrl ) );
  m_KeepGhost->SetValue( wCtrl.iskeepghost( ctrl ) );
  m_LocoStartGap->SetValue( wCtrl.getlocostartgap( ctrl ) );
  m_DisableSteal->SetValue( wCtrl.isdisablesteal( ctrl ) );
  m_BlockInitPause->SetValue( wCtrl.getblockinitpause( ctrl ) );
  m_PowerOffAtReset->SetValue( wCtrl.ispoweroffatreset( ctrl ) );
  m_ZeroThrottleID->SetValue( wCtrl.isallowzerothrottleid( ctrl ) );
  m_UseBlockSide->SetValue( wCtrl.isuseblockside( ctrl ) ? true:false );
  m_StopNoneCommuter->SetValue( wCtrl.isstopnonecommuter( ctrl ) ? true:false );
  m_SyncRoutes->SetValue( wCtrl.issyncroutes( ctrl ) ? true:false );
  m_SyncRouteTimeout->SetValue( wCtrl.getsyncroutetimeout( ctrl ) );
  m_TimedSensors->SetValue( wCtrl.istimedsensors( ctrl ) ? true:false );
  m_EnableAnalyzer->SetValue( wCtrl.isenableanalyzer( ctrl ) ? true:false );
  m_ForceUnlock->SetValue( wCtrl.isebreakforceunlock( ctrl ) ? true:false );
  m_TriggerBlockEvents->SetValue( wCtrl.istriggerblockevents( ctrl ) ? true:false );
  m_ResetFxSp->SetValue( wRocRail.isresetspfx( m_Props ) ? true:false );
  m_ReleaseOnIdle->SetValue( wCtrl.isreleaseonidle( ctrl ) ? true:false );
  m_CloseOnGhost->SetValue( wCtrl.iscloseonghost( ctrl ) ? true:false );

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
  m_PowerOffOnExit->SetValue(wRocRail.ispoweroffonexit(m_Props)?true:false);

  m_Lib->Append( wxString( wDigInt.barjut, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.bidib, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.cbus, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.clock, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.cti, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.dcc232, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.ddx, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.dinamo, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.ecos, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.editspro, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.hsi88, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.infracar, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.inter10, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.io8255, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.loconet, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.lrc135, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.massoth, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.mcs2, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.mttmfcc, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.muet, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.nce, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.om32, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.opendcc, wxConvUTF8 ) );
  //m_Lib->Append( wxString( wDigInt.openlcb, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.p50, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.p50x, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.perir, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.rclink, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.rfid12, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.rmx, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.rocnet, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.roco, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.slx, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.sprog, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.srcp, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.tamsmc, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.vcs, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.xpressnet, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.zimo, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.zimobin, wxConvUTF8 ) );
  m_Lib->Append( wxString( wDigInt.zs2, wxConvUTF8 ) );

  m_Lib->SetSelection( 0 );
  m_Controller = NULL;

  // R2Rnet
  iONode r2rnet = wRocRail.getr2rnet( m_Props );
  if( r2rnet == NULL ) {
    r2rnet = NodeOp.inst( wR2RnetIni.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild( m_Props, r2rnet );
  }
  m_R2RnetID->SetValue( wxString( wR2RnetIni.getid(r2rnet),wxConvUTF8) );
  m_R2RnetAddr->SetValue( wxString( wR2RnetIni.getaddr(r2rnet),wxConvUTF8) );
  m_R2RnetPort->SetValue( wR2RnetIni.getport(r2rnet) );
  m_R2RnetRoutes->SetValue( wxString( wR2RnetIni.getroutes(r2rnet),wxConvUTF8) );
  m_R2RnetEnable->SetValue( wR2RnetIni.isenable(r2rnet) ? true:false );
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
  wRocRail.setoccupancy( m_Props, m_OccFile->GetValue().mb_str(wxConvUTF8) );
  wRocRail.setlibpath( m_Props, m_LibPath->GetValue().mb_str(wxConvUTF8) );
  wRocRail.setimgpath( m_Props, m_ImgPath->GetValue().mb_str(wxConvUTF8) );
  iONode trace = wRocRail.gettrace( m_Props );
  wTrace.setrfile( trace, m_TraceFile->GetValue().mb_str(wxConvUTF8) );
  wRocRail.setptiid( m_Props, m_PTIID->GetValue().mb_str(wxConvUTF8) );
  wRocRail.setlciid( m_Props, m_LCIID->GetValue().mb_str(wxConvUTF8) );
  wRocRail.setdpiid( m_Props, m_DPIID->GetValue().mb_str(wxConvUTF8) );
  wRocRail.setsviid( m_Props, m_SVIID->GetValue().mb_str(wxConvUTF8) );

  wRocRail.setsciid( m_Props, m_ScIID->GetValue().mb_str(wxConvUTF8) );

  if( StrOp.equals( "-", m_ScSensor->GetValue().mb_str(wxConvUTF8) ) )
    wRocRail.setscsensor( m_Props, "" );
  else
    wRocRail.setscsensor( m_Props, m_ScSensor->GetValue().mb_str(wxConvUTF8) );

  wRocRail.setcreatemodplan( m_Props, m_PlanType->GetSelection() == 1 ? True:False );
  wRocRail.setresetspfx( m_Props, m_ResetFxSp->IsChecked() ? True:False );

  // Service
  iONode tcp = wRocRail.gettcp( m_Props );
  wTcp.setport( tcp, atoi( m_ClientPort->GetValue().mb_str(wxConvUTF8) ) );
  wTcp.setmaxcon( tcp, atoi( m_MaxCon->GetValue().mb_str(wxConvUTF8) ) );
  wTcp.setcontrolcode( tcp, m_ControlCode->GetValue().mb_str(wxConvUTF8) );
  wTcp.setonlyfirstmaster( tcp, m_OnlyFirstMaster->IsChecked() ? True:False );
  iONode srcpcon = wRocRail.getsrcpcon( m_Props );
  wSrcpCon.setport( srcpcon, m_SrcpPort->GetValue() );
  wSrcpCon.setactive( srcpcon, m_SrcpServiceActive->IsChecked()?True:False);

  iONode snmp = wRocRail.getSnmpService( m_Props );
  wSnmpService.setport( snmp, m_SnmpPort->GetValue() );
  wSnmpService.setactive( snmp, m_SnmpServiceActive->IsChecked()?True:False);
  wSnmpService.settraphost(snmp, m_SnmpTrapHost->GetValue().mb_str(wxConvUTF8) );
  wSnmpService.settrapport(snmp, m_SnmpTrapPort->GetValue());


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
  wTrace.setinfo( trace, m_Info->IsChecked() ? True:False );
  wTrace.setbyte( trace, m_Byte->IsChecked() ? True:False );
  //wTrace.setparse( trace, m_Parse->IsChecked() ? True:False );
  wTrace.setautomatic( trace, m_Auto->IsChecked() ? True:False );
  wTrace.setmonitor( trace, m_Monitor->IsChecked() ? True:False );
  wTrace.setlisten2all( trace, m_SendAllTraces->IsChecked() ? True:False );

  // Controllers
  wRocRail.setpoweroffonexit(m_Props, m_PowerOffOnExit->IsChecked() ? True:False);

  // Automat
  iONode ctrl = wRocRail.getctrl( m_Props );
  wCtrl.setswtimeout( ctrl, atoi( m_SwTime->GetValue().mb_str(wxConvUTF8) ) );
  wCtrl.setminbklc( ctrl, atoi( m_MinBKLen->GetValue().mb_str(wxConvUTF8) ) );
  wCtrl.setignevt( ctrl, atoi( m_IgnEvt->GetValue().mb_str(wxConvUTF8) ) );
  wCtrl.setinitfieldpause( ctrl, atoi( m_PauseInit->GetValue().mb_str(wxConvUTF8) ) );
  wCtrl.setseed( ctrl, m_Seed->GetValue() );
  wCtrl.setsavepostime( ctrl, m_SavePosTime->GetValue() );
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
  wCtrl.setsignalwait( ctrl, m_SignalWait->GetValue() );
  wCtrl.setlocostartgap( ctrl, m_LocoStartGap->GetValue() );
  wCtrl.setblockinitpause( ctrl, m_BlockInitPause->GetValue() );
  wCtrl.setuseblockside( ctrl, m_UseBlockSide->GetValue() ? True:False );
  wCtrl.setstopnonecommuter( ctrl, m_StopNoneCommuter->GetValue() ? True:False );
  wCtrl.setsyncroutes( ctrl, m_SyncRoutes->GetValue() ? True:False );
  wCtrl.setsyncroutetimeout( ctrl, m_SyncRouteTimeout->GetValue() );
  wCtrl.settimedsensors( ctrl, m_TimedSensors->GetValue() ? True:False );
  wCtrl.setenableanalyzer( ctrl, m_EnableAnalyzer->GetValue() ? True:False );
  wCtrl.setebreakforceunlock( ctrl, m_ForceUnlock->GetValue() ? True:False );
  wCtrl.settriggerblockevents( ctrl, m_TriggerBlockEvents->GetValue() ? True:False );

  const char* defaspect[] = {wSignal.red,wSignal.green,wSignal.yellow,wSignal.white};
  wCtrl.setdefaspect( ctrl, defaspect[m_DefAspect->GetSelection()] );
  wCtrl.setskipsetsw( ctrl, m_SkipSetSw->IsChecked() ? True:False );
  wCtrl.setskipsetsg( ctrl, m_SkipSetSg->IsChecked() ? True:False );
  wCtrl.setusebicom( ctrl, m_UseBiCom->IsChecked() ? True:False );
  wCtrl.setpoweroffonidentmismatch( ctrl, m_StopAtIdentMisMatch->IsChecked() ? True:False );
  wCtrl.setpoweroffatghost( ctrl, m_PowerOffAtGhost->IsChecked() ? True:False );
  wCtrl.setkeepghost( ctrl, m_KeepGhost->IsChecked() ? True:False );
  wCtrl.setdisablesteal( ctrl, m_DisableSteal->IsChecked() ? True:False );
  wCtrl.setpoweroffatreset( ctrl, m_PowerOffAtReset->IsChecked() ? True:False );
  wCtrl.setallowzerothrottleid( ctrl, m_ZeroThrottleID->IsChecked() ? True:False );
  wCtrl.setreleaseonidle( ctrl, m_ReleaseOnIdle->IsChecked() ? True:False );
  wCtrl.setcloseonghost( ctrl, m_CloseOnGhost->IsChecked() ? True:False );


  // R2Rnet
  iONode r2rnet = wRocRail.getr2rnet(m_Props);
  wR2RnetIni.setid( r2rnet, m_R2RnetID->GetValue().mb_str(wxConvUTF8) );
  wR2RnetIni.setaddr( r2rnet, m_R2RnetAddr->GetValue().mb_str(wxConvUTF8) );
  wR2RnetIni.setport( r2rnet, m_R2RnetPort->GetValue() );
  wR2RnetIni.setroutes( r2rnet, m_R2RnetRoutes->GetValue().mb_str(wxConvUTF8) );
  wR2RnetIni.setenable( r2rnet, m_R2RnetEnable->IsChecked() ? True:False );
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
    m_labOccFile = NULL;
    m_OccFile = NULL;
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
    m_labAnalyze = NULL;
    m_ScBox = NULL;
    m_labScSensor = NULL;
    m_ScSensor = NULL;
    m_labScIID = NULL;
    m_ScIID = NULL;
    m_TracePanel = NULL;
    m_TraceLevel = NULL;
    m_Auto = NULL;
    m_Monitor = NULL;
    m_DevTraceLevel = NULL;
    m_Byte = NULL;
    m_Info = NULL;
    m_Debug = NULL;
    m_SendAllTraces = NULL;
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
    m_labControlCode = NULL;
    m_ControlCode = NULL;
    m_OnlyFirstMaster = NULL;
    m_SrcpService = NULL;
    m_labSrcpPort = NULL;
    m_SrcpPort = NULL;
    m_SrcpServiceActive = NULL;
    m_labSnmpPort = NULL;
    m_SnmpPort = NULL;
    m_SnmpServiceActive = NULL;
    m_SNMPTrapBox = NULL;
    m_labSnmpTrapHost = NULL;
    m_SnmpTrapHost = NULL;
    m_labSnmpTrapPort = NULL;
    m_SnmpTrapPort = NULL;
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
    m_labEventTimeout = NULL;
    m_EventTimeout = NULL;
    m_labSignalReset = NULL;
    m_SignalReset = NULL;
    m_labRouteSwTime = NULL;
    m_RouteSwTime = NULL;
    m_labSemaphoreWait = NULL;
    m_SemaphoreWait = NULL;
    m_labSignalWait = NULL;
    m_SignalWait = NULL;
    m_labLocoStartGap = NULL;
    m_LocoStartGap = NULL;
    m_labBlockInitPause = NULL;
    m_BlockInitPause = NULL;
    m_labSyncRouteTimeout = NULL;
    m_SyncRouteTimeout = NULL;
    m_ResetFxSp = NULL;
    m_Check2In = NULL;
    m_secondNextBlock = NULL;
    m_EnableSwFb = NULL;
    m_DisableRouteVreduce = NULL;
    m_GreenAspect = NULL;
    m_SkipSetSw = NULL;
    m_SkipSetSg = NULL;
    m_UseBiCom = NULL;
    m_StopAtIdentMisMatch = NULL;
    m_PowerOffAtGhost = NULL;
    m_KeepGhost = NULL;
    m_DisableSteal = NULL;
    m_PowerOffAtReset = NULL;
    m_ZeroThrottleID = NULL;
    m_UseBlockSide = NULL;
    m_StopNoneCommuter = NULL;
    m_SyncRoutes = NULL;
    m_TimedSensors = NULL;
    m_EnableAnalyzer = NULL;
    m_ForceUnlock = NULL;
    m_TriggerBlockEvents = NULL;
    m_ReleaseOnIdle = NULL;
    m_CloseOnGhost = NULL;
    m_DefAspect = NULL;
    m_ControllersPanel = NULL;
    m_Controllers = NULL;
    m_ControllerDelete = NULL;
    m_ControllerProps = NULL;
    m_AddControllerBox = NULL;
    m_Lib = NULL;
    m_ControllerAdd = NULL;
    m_ControllerOptionsBox = NULL;
    m_PowerOffOnExit = NULL;
    m_R2RnetPanel = NULL;
    m_labR2RnetID = NULL;
    m_R2RnetID = NULL;
    m_labR2RnetAddr = NULL;
    m_R2RnetAddr = NULL;
    m_labR2RnetPort = NULL;
    m_R2RnetPort = NULL;
    m_labR2RnetRoutes = NULL;
    m_R2RnetRoutes = NULL;
    m_R2RnetRoutesDlg = NULL;
    m_R2RnetEnable = NULL;
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
    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    m_GeneralPanel->SetSizer(itemBoxSizer5);

    wxFlexGridSizer* itemFlexGridSizer6 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer6->AddGrowableCol(1);
    itemBoxSizer5->Add(itemFlexGridSizer6, 0, wxGROW|wxALL, 5);
    m_LabelPlanfile = new wxStaticText( m_GeneralPanel, wxID_STATIC_RR_PLANFILE, _("PlanFile"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_LabelPlanfile, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_PlanFile = new wxTextCtrl( m_GeneralPanel, ID_TEXTCTRL_RR_PLANFILE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_PlanFile, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labOccFile = new wxStaticText( m_GeneralPanel, wxID_ANY, _("Occupation file"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labOccFile, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_OccFile = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_OccFile, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labPlanType = new wxStaticText( m_GeneralPanel, wxID_ANY, _("Plan type"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labPlanType, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_PlanTypeStrings;
    m_PlanTypeStrings.Add(_("&Standard"));
    m_PlanTypeStrings.Add(_("&Modules"));
    m_PlanType = new wxRadioBox( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_PlanTypeStrings, 1, wxRA_SPECIFY_ROWS );
    m_PlanType->SetSelection(0);
    itemFlexGridSizer6->Add(m_PlanType, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_LabelLibPath = new wxStaticText( m_GeneralPanel, wxID_STATIC_RR_LIBPATH, _("LibPath"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_LabelLibPath, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LibPath = new wxTextCtrl( m_GeneralPanel, ID_TEXTCTRL_RR_LIBPATH, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_LibPath, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labImgPath = new wxStaticText( m_GeneralPanel, wxID_ANY, _("ImgPath"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labImgPath, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ImgPath = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_ImgPath, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelTraceFile = new wxStaticText( m_GeneralPanel, wxID_STATIC_RR_TRACEFILE, _("TraceFile"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_LabelTraceFile, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_TraceFile = new wxTextCtrl( m_GeneralPanel, ID_TEXTCTRL_RR_TRACEFILE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_TraceFile, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labPTIID = new wxStaticText( m_GeneralPanel, wxID_ANY, _("PT IID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labPTIID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_PTIID = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_PTIID, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labLCIID = new wxStaticText( m_GeneralPanel, wxID_ANY, _("LC IID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labLCIID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LCIID = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_LCIID, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labDPIID = new wxStaticText( m_GeneralPanel, wxID_ANY, _("DP IID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labDPIID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_DPIID = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_DPIID, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labSVIID = new wxStaticText( m_GeneralPanel, wxID_ANY, _("SV IID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labSVIID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SVIID = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_SVIID, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labAnalyze = new wxStaticText( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labAnalyze, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ScBox = new wxStaticBox(m_GeneralPanel, wxID_ANY, _("Shortcut"));
    wxStaticBoxSizer* itemStaticBoxSizer28 = new wxStaticBoxSizer(m_ScBox, wxVERTICAL);
    itemBoxSizer5->Add(itemStaticBoxSizer28, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer29 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer29->AddGrowableCol(1);
    itemStaticBoxSizer28->Add(itemFlexGridSizer29, 0, wxGROW, 5);
    m_labScSensor = new wxStaticText( m_GeneralPanel, wxID_ANY, _("Shortcut"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer29->Add(m_labScSensor, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_ScSensorStrings;
    m_ScSensor = new wxComboBox( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_ScSensorStrings, wxCB_DROPDOWN );
    itemFlexGridSizer29->Add(m_ScSensor, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labScIID = new wxStaticText( m_GeneralPanel, wxID_ANY, _("IID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer29->Add(m_labScIID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ScIID = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer29->Add(m_ScIID, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_RRNotebook->AddPage(m_GeneralPanel, _("General"));

    m_TracePanel = new wxPanel( m_RRNotebook, ID_PANEL_RR_TRACE, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer35 = new wxBoxSizer(wxVERTICAL);
    m_TracePanel->SetSizer(itemBoxSizer35);

    wxStaticBox* itemStaticBoxSizer36Static = new wxStaticBox(m_TracePanel, wxID_ANY, _("User Levels"));
    m_TraceLevel = new wxStaticBoxSizer(itemStaticBoxSizer36Static, wxVERTICAL);
    itemBoxSizer35->Add(m_TraceLevel, 0, wxGROW|wxALL, 5);
    m_Auto = new wxCheckBox( m_TracePanel, ID_CHECKBOX_RR_AUTO, _("Automatic"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_Auto->SetValue(false);
    m_TraceLevel->Add(m_Auto, 0, wxALIGN_LEFT|wxALL, 5);

    m_Monitor = new wxCheckBox( m_TracePanel, ID_CHECKBOX_RR_MONITOR, _("Monitor"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Monitor->SetValue(false);
    m_TraceLevel->Add(m_Monitor, 0, wxALIGN_LEFT|wxALL, 5);

    m_DevTraceLevel = new wxStaticBox(m_TracePanel, wxID_ANY, _("Developer Levels"));
    wxStaticBoxSizer* itemStaticBoxSizer39 = new wxStaticBoxSizer(m_DevTraceLevel, wxVERTICAL);
    itemBoxSizer35->Add(itemStaticBoxSizer39, 0, wxGROW|wxALL, 5);
    m_Byte = new wxCheckBox( m_TracePanel, ID_CHECKBOX_RR_BYTE, _("Byte"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_Byte->SetValue(false);
    itemStaticBoxSizer39->Add(m_Byte, 0, wxALIGN_LEFT|wxALL, 5);

    m_Info = new wxCheckBox( m_TracePanel, wxID_ANY, _("Info"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Info->SetValue(false);
    itemStaticBoxSizer39->Add(m_Info, 0, wxALIGN_LEFT|wxALL, 5);

    m_Debug = new wxCheckBox( m_TracePanel, ID_CHECKBOX_RR_DEBUG, _("Debug"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_Debug->SetValue(false);
    itemStaticBoxSizer39->Add(m_Debug, 0, wxALIGN_LEFT|wxALL, 5);

    wxStaticLine* itemStaticLine43 = new wxStaticLine( m_TracePanel, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemStaticBoxSizer39->Add(itemStaticLine43, 0, wxGROW|wxALL, 5);

    m_SendAllTraces = new wxCheckBox( m_TracePanel, wxID_ANY, _("Send all traces to the clients"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SendAllTraces->SetValue(false);
    itemStaticBoxSizer39->Add(m_SendAllTraces, 0, wxALIGN_LEFT|wxALL, 5);

    m_RRNotebook->AddPage(m_TracePanel, _("Trace"));

    m_ServicePanel = new wxPanel( m_RRNotebook, ID_PANEL_RR_SERVICE, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer46 = new wxBoxSizer(wxHORIZONTAL);
    m_ServicePanel->SetSizer(itemBoxSizer46);

    wxBoxSizer* itemBoxSizer47 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer46->Add(itemBoxSizer47, 0, wxGROW|wxALL, 5);
    wxStaticBox* itemStaticBoxSizer48Static = new wxStaticBox(m_ServicePanel, wxID_ANY, _("HTTP Service"));
    m_HttpBox = new wxStaticBoxSizer(itemStaticBoxSizer48Static, wxHORIZONTAL);
    itemBoxSizer47->Add(m_HttpBox, 0, wxGROW|wxALL, 5);
    m_LabelHttpPort = new wxStaticText( m_ServicePanel, wxID_STATIC_RR_HTTPPORT, _("Port"), wxDefaultPosition, wxDefaultSize, 0 );
    m_HttpBox->Add(m_LabelHttpPort, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_HttpPort = new wxTextCtrl( m_ServicePanel, ID_TEXTCTRL_RR_HTTPPORT, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_HttpPort->SetMaxLength(5);
    m_HttpBox->Add(m_HttpPort, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabServiceRefresh = new wxStaticText( m_ServicePanel, wxID_STATIC_RR_SEVICE_REFRESH, _("Refresh"), wxDefaultPosition, wxDefaultSize, 0 );
    m_HttpBox->Add(m_LabServiceRefresh, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ServiceRefresh = new wxTextCtrl( m_ServicePanel, ID_TEXTCTRL_RR_SERVICE_REFRESH, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_HttpBox->Add(m_ServiceRefresh, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer53Static = new wxStaticBox(m_ServicePanel, wxID_ANY, _("Web Service"));
    m_WebServiceBox = new wxStaticBoxSizer(itemStaticBoxSizer53Static, wxHORIZONTAL);
    itemBoxSizer47->Add(m_WebServiceBox, 0, wxGROW|wxALL, 5);
    m_LabWebServicePort = new wxStaticText( m_ServicePanel, wxID_STATIC_RR_WEBSERVICE_PORT, _("Port"), wxDefaultPosition, wxDefaultSize, 0 );
    m_WebServiceBox->Add(m_LabWebServicePort, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_WebServicePort = new wxTextCtrl( m_ServicePanel, ID_TEXTCTRL_RR_WEBSERVICE_PORT, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_WebServicePort->SetMaxLength(5);
    m_WebServiceBox->Add(m_WebServicePort, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabWebServiceRefresh = new wxStaticText( m_ServicePanel, wxID_STATIC_RR_WEBSERVICE_REFRESH, _("Refresh"), wxDefaultPosition, wxDefaultSize, 0 );
    m_WebServiceBox->Add(m_LabWebServiceRefresh, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_WebServiceRefresh = new wxTextCtrl( m_ServicePanel, ID_TEXTCTRL_RR_WEBSERVICE_REFRESH, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_WebServiceBox->Add(m_WebServiceRefresh, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer58Static = new wxStaticBox(m_ServicePanel, wxID_ANY, _("Clock Service"));
    m_ClockBox = new wxStaticBoxSizer(itemStaticBoxSizer58Static, wxHORIZONTAL);
    itemBoxSizer47->Add(m_ClockBox, 0, wxGROW|wxALL, 5);
    m_labDevider = new wxStaticText( m_ServicePanel, wxID_STATIC_RR_CLOCK_DEVIDER, _("divider"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ClockBox->Add(m_labDevider, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Divider = new wxTextCtrl( m_ServicePanel, ID_TEXTCTRL_RR_CLOCK_DEVIDER, _("1"), wxDefaultPosition, wxSize(40, -1), wxTE_CENTRE );
    m_Divider->SetMaxLength(2);
    m_ClockBox->Add(m_Divider, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labHour = new wxStaticText( m_ServicePanel, wxID_STATIC_RR_CLOCK_HOUR, _("hour"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ClockBox->Add(m_labHour, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Hour = new wxTextCtrl( m_ServicePanel, ID_TEXTCTRL_RR_CLOCK_HOUR, _("24"), wxDefaultPosition, wxSize(40, -1), wxTE_CENTRE );
    m_Hour->SetMaxLength(2);
    if (RocrailIniDialog::ShowToolTips())
        m_Hour->SetToolTip(_("24 disabled the start setting"));
    m_ClockBox->Add(m_Hour, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labMinute = new wxStaticText( m_ServicePanel, wxID_STATIC_RR_CLOCK_MINUTE, _("minute"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ClockBox->Add(m_labMinute, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Minute = new wxTextCtrl( m_ServicePanel, ID_TEXTCTRL_RR_CLOCK_MINUTE, _("60"), wxDefaultPosition, wxSize(40, -1), wxTE_CENTRE );
    m_Minute->SetMaxLength(2);
    if (RocrailIniDialog::ShowToolTips())
        m_Minute->SetToolTip(_("60 disabled the start setting"));
    m_ClockBox->Add(m_Minute, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer65Static = new wxStaticBox(m_ServicePanel, wxID_ANY, _("Client Service"));
    m_ClientBox = new wxStaticBoxSizer(itemStaticBoxSizer65Static, wxVERTICAL);
    itemBoxSizer47->Add(m_ClientBox, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer66 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer66->AddGrowableCol(1);
    m_ClientBox->Add(itemFlexGridSizer66, 1, wxGROW|wxALL, 5);
    m_LabelClientPort = new wxStaticText( m_ServicePanel, wxID_STATIC_RR_CLIENTPORT, _("port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer66->Add(m_LabelClientPort, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ClientPort = new wxTextCtrl( m_ServicePanel, ID_TEXTCTRL_RR_CLIENTPORT, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_ClientPort->SetMaxLength(5);
    itemFlexGridSizer66->Add(m_ClientPort, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelMaxCon = new wxStaticText( m_ServicePanel, wxID_STATIC_RR_MAXCON, _("max."), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer66->Add(m_LabelMaxCon, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_MaxCon = new wxTextCtrl( m_ServicePanel, ID_TEXTCTRL_RR_MAXCONN, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_MaxCon->SetMaxLength(2);
    itemFlexGridSizer66->Add(m_MaxCon, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labControlCode = new wxStaticText( m_ServicePanel, wxID_ANY, _("Control code"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer66->Add(m_labControlCode, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ControlCode = new wxTextCtrl( m_ServicePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer66->Add(m_ControlCode, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer73 = new wxFlexGridSizer(0, 1, 0, 0);
    m_ClientBox->Add(itemFlexGridSizer73, 0, wxGROW|wxALL, 5);
    m_OnlyFirstMaster = new wxCheckBox( m_ServicePanel, wxID_ANY, _("Only first is master"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OnlyFirstMaster->SetValue(false);
    itemFlexGridSizer73->Add(m_OnlyFirstMaster, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine75 = new wxStaticLine( m_ServicePanel, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer46->Add(itemStaticLine75, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer76 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer46->Add(itemBoxSizer76, 0, wxGROW|wxALL, 5);
    m_SrcpService = new wxStaticBox(m_ServicePanel, wxID_ANY, _("SRCP Service"));
    wxStaticBoxSizer* itemStaticBoxSizer77 = new wxStaticBoxSizer(m_SrcpService, wxVERTICAL);
    itemBoxSizer76->Add(itemStaticBoxSizer77, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer78 = new wxFlexGridSizer(0, 3, 0, 0);
    itemStaticBoxSizer77->Add(itemFlexGridSizer78, 0, wxGROW|wxALL, 5);
    m_labSrcpPort = new wxStaticText( m_ServicePanel, wxID_ANY, _("port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer78->Add(m_labSrcpPort, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SrcpPort = new wxSpinCtrl( m_ServicePanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 0, 65536, 0 );
    itemFlexGridSizer78->Add(m_SrcpPort, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SrcpServiceActive = new wxCheckBox( m_ServicePanel, wxID_ANY, _("Active"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SrcpServiceActive->SetValue(false);
    itemFlexGridSizer78->Add(m_SrcpServiceActive, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer82Static = new wxStaticBox(m_ServicePanel, wxID_ANY, _("SNMP Service"));
    wxStaticBoxSizer* itemStaticBoxSizer82 = new wxStaticBoxSizer(itemStaticBoxSizer82Static, wxVERTICAL);
    itemBoxSizer76->Add(itemStaticBoxSizer82, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer83 = new wxFlexGridSizer(0, 3, 0, 0);
    itemStaticBoxSizer82->Add(itemFlexGridSizer83, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, 5);
    m_labSnmpPort = new wxStaticText( m_ServicePanel, wxID_ANY, _("port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer83->Add(m_labSnmpPort, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SnmpPort = new wxSpinCtrl( m_ServicePanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 0, 65536, 0 );
    itemFlexGridSizer83->Add(m_SnmpPort, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SnmpServiceActive = new wxCheckBox( m_ServicePanel, wxID_ANY, _("Active"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SnmpServiceActive->SetValue(false);
    itemFlexGridSizer83->Add(m_SnmpServiceActive, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SNMPTrapBox = new wxStaticBox(m_ServicePanel, wxID_ANY, _("Trap"));
    wxStaticBoxSizer* itemStaticBoxSizer87 = new wxStaticBoxSizer(m_SNMPTrapBox, wxVERTICAL);
    itemStaticBoxSizer82->Add(itemStaticBoxSizer87, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    wxFlexGridSizer* itemFlexGridSizer88 = new wxFlexGridSizer(0, 4, 0, 0);
    itemStaticBoxSizer87->Add(itemFlexGridSizer88, 0, wxGROW, 5);
    m_labSnmpTrapHost = new wxStaticText( m_ServicePanel, wxID_ANY, _("Host"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer88->Add(m_labSnmpTrapHost, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SnmpTrapHost = new wxTextCtrl( m_ServicePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(160, -1), 0 );
    itemFlexGridSizer88->Add(m_SnmpTrapHost, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labSnmpTrapPort = new wxStaticText( m_ServicePanel, wxID_ANY, _("Port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer88->Add(m_labSnmpTrapPort, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SnmpTrapPort = new wxSpinCtrl( m_ServicePanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 0, 65536, 0 );
    itemFlexGridSizer88->Add(m_SnmpTrapPort, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_RRNotebook->AddPage(m_ServicePanel, _("Service"));

    m_AtomatPanel = new wxPanel( m_RRNotebook, ID_PANEL_RR_AUTO, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer94 = new wxBoxSizer(wxHORIZONTAL);
    m_AtomatPanel->SetSizer(itemBoxSizer94);

    wxBoxSizer* itemBoxSizer95 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer94->Add(itemBoxSizer95, 0, wxALIGN_TOP|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer96 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer96->AddGrowableCol(1);
    itemBoxSizer95->Add(itemFlexGridSizer96, 0, wxGROW, 5);
    m_LabelSwTime = new wxStaticText( m_AtomatPanel, wxID_STATIC_RR_SWTIME, _("Switch time"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_LabelSwTime, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SwTime = new wxTextCtrl( m_AtomatPanel, ID_TEXTCTRL_RR_SWTIME, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer96->Add(m_SwTime, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelMinBKLen = new wxStaticText( m_AtomatPanel, wxID_STATIC__MINBKLEN, _("Min Block length"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_LabelMinBKLen, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_MinBKLen = new wxTextCtrl( m_AtomatPanel, ID_TEXTCTRL_RR_MINBKLEN, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer96->Add(m_MinBKLen, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_LabelIgnEvt = new wxStaticText( m_AtomatPanel, wxID_STATIC_RR_IGNEVT, _("Ignore dup. sensor events"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_LabelIgnEvt, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_IgnEvt = new wxTextCtrl( m_AtomatPanel, ID_TEXTCTRL_RR_IGNEVT, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer96->Add(m_IgnEvt, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labInitPause = new wxStaticText( m_AtomatPanel, wxID_STATIC_RR_INITPAUSE, _("Initfield pause"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_labInitPause, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_PauseInit = new wxTextCtrl( m_AtomatPanel, ID_TEXTCTRL_RR_PAUSEINIT, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer96->Add(m_PauseInit, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labInitPower = new wxStaticText( m_AtomatPanel, wxID_ANY, _("Initfield power on"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_labInitPower, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_InitfieldPower = new wxCheckBox( m_AtomatPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_InitfieldPower->SetValue(false);
    itemFlexGridSizer96->Add(m_InitfieldPower, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labSeed = new wxStaticText( m_AtomatPanel, wxID_ANY, _("Random seed"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_labSeed, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Seed = new wxSpinCtrl( m_AtomatPanel, wxID_ANY, _T("4711"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 65535, 4711 );
    itemFlexGridSizer96->Add(m_Seed, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labSavePosTime = new wxStaticText( m_AtomatPanel, wxID_ANY, _("Save position time"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_labSavePosTime, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_SavePosTime = new wxSpinCtrl( m_AtomatPanel, wxID_ANY, _T("10"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 100, 10 );
    itemFlexGridSizer96->Add(m_SavePosTime, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labEventTimeout = new wxStaticText( m_AtomatPanel, wxID_ANY, _("Event timeout"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_labEventTimeout, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EventTimeout = new wxTextCtrl( m_AtomatPanel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer96->Add(m_EventTimeout, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labSignalReset = new wxStaticText( m_AtomatPanel, wxID_ANY, _("Signal reset"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_labSignalReset, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_SignalReset = new wxSpinCtrl( m_AtomatPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 10, 0 );
    itemFlexGridSizer96->Add(m_SignalReset, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labRouteSwTime = new wxStaticText( m_AtomatPanel, wxID_ANY, _("Route switch time"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_labRouteSwTime, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_RouteSwTime = new wxSpinCtrl( m_AtomatPanel, wxID_ANY, _T("10"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 10, 10000, 10 );
    itemFlexGridSizer96->Add(m_RouteSwTime, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labSemaphoreWait = new wxStaticText( m_AtomatPanel, wxID_ANY, _("Semaphore wait"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_labSemaphoreWait, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_SemaphoreWait = new wxSpinCtrl( m_AtomatPanel, wxID_ANY, _T("1"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 10, 1 );
    itemFlexGridSizer96->Add(m_SemaphoreWait, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labSignalWait = new wxStaticText( m_AtomatPanel, wxID_ANY, _("Signal wait"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_labSignalWait, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_SignalWait = new wxSpinCtrl( m_AtomatPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 10, 0 );
    itemFlexGridSizer96->Add(m_SignalWait, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labLocoStartGap = new wxStaticText( m_AtomatPanel, wxID_ANY, _("Loco start gap"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_labLocoStartGap, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_LocoStartGap = new wxSpinCtrl( m_AtomatPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer96->Add(m_LocoStartGap, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labBlockInitPause = new wxStaticText( m_AtomatPanel, wxID_ANY, _("Block init pause"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_labBlockInitPause, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_BlockInitPause = new wxSpinCtrl( m_AtomatPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 5000, 0 );
    itemFlexGridSizer96->Add(m_BlockInitPause, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labSyncRouteTimeout = new wxStaticText( m_AtomatPanel, wxID_ANY, _("Sync route timeout"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_labSyncRouteTimeout, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_SyncRouteTimeout = new wxSpinCtrl( m_AtomatPanel, wxID_ANY, _T("2500"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 100, 10000, 2500 );
    itemFlexGridSizer96->Add(m_SyncRouteTimeout, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxStaticLine* itemStaticLine127 = new wxStaticLine( m_AtomatPanel, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer95->Add(itemStaticLine127, 0, wxGROW|wxALL, 5);

    m_ResetFxSp = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Reset Speed and Functions"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ResetFxSp->SetValue(false);
    itemBoxSizer95->Add(m_ResetFxSp, 0, wxALIGN_LEFT|wxALL, 5);

    wxStaticLine* itemStaticLine129 = new wxStaticLine( m_AtomatPanel, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer94->Add(itemStaticLine129, 0, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer130 = new wxFlexGridSizer(0, 1, 0, 0);
    itemBoxSizer94->Add(itemFlexGridSizer130, 0, wxALIGN_TOP, 5);
    m_Check2In = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Check for free destination until reaching IN"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Check2In->SetValue(false);
    itemFlexGridSizer130->Add(m_Check2In, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_secondNextBlock = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Reserve second next block"), wxDefaultPosition, wxDefaultSize, 0 );
    m_secondNextBlock->SetValue(false);
    itemFlexGridSizer130->Add(m_secondNextBlock, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EnableSwFb = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Enable Switch Feedback"), wxDefaultPosition, wxDefaultSize, 0 );
    m_EnableSwFb->SetValue(false);
    itemFlexGridSizer130->Add(m_EnableSwFb, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_DisableRouteVreduce = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Disable route speed reduce"), wxDefaultPosition, wxDefaultSize, 0 );
    m_DisableRouteVreduce->SetValue(false);
    itemFlexGridSizer130->Add(m_DisableRouteVreduce, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_GreenAspect = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Green aspect if next is red"), wxDefaultPosition, wxDefaultSize, 0 );
    m_GreenAspect->SetValue(false);
    itemFlexGridSizer130->Add(m_GreenAspect, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_SkipSetSw = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Skip switch commands in routes if already set"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SkipSetSw->SetValue(false);
    itemFlexGridSizer130->Add(m_SkipSetSw, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_SkipSetSg = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Skip signal commands if already set"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SkipSetSg->SetValue(false);
    itemFlexGridSizer130->Add(m_SkipSetSg, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_UseBiCom = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Use Bi-Directional Communication"), wxDefaultPosition, wxDefaultSize, 0 );
    m_UseBiCom->SetValue(false);
    itemFlexGridSizer130->Add(m_UseBiCom, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_StopAtIdentMisMatch = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("power off at ident mismatch "), wxDefaultPosition, wxDefaultSize, 0 );
    m_StopAtIdentMisMatch->SetValue(false);
    itemFlexGridSizer130->Add(m_StopAtIdentMisMatch, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_PowerOffAtGhost = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Power off at ghost"), wxDefaultPosition, wxDefaultSize, 0 );
    m_PowerOffAtGhost->SetValue(false);
    itemFlexGridSizer130->Add(m_PowerOffAtGhost, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_KeepGhost = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Keep ghost state"), wxDefaultPosition, wxDefaultSize, 0 );
    m_KeepGhost->SetValue(false);
    itemFlexGridSizer130->Add(m_KeepGhost, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_DisableSteal = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Disable steal loco control"), wxDefaultPosition, wxDefaultSize, 0 );
    m_DisableSteal->SetValue(false);
    itemFlexGridSizer130->Add(m_DisableSteal, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_PowerOffAtReset = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("power off at reset"), wxDefaultPosition, wxDefaultSize, 0 );
    m_PowerOffAtReset->SetValue(true);
    itemFlexGridSizer130->Add(m_PowerOffAtReset, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_ZeroThrottleID = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Allow zero throttle ID"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ZeroThrottleID->SetValue(false);
    itemFlexGridSizer130->Add(m_ZeroThrottleID, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_UseBlockSide = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Use block side for routes"), wxDefaultPosition, wxDefaultSize, 0 );
    m_UseBlockSide->SetValue(false);
    itemFlexGridSizer130->Add(m_UseBlockSide, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_StopNoneCommuter = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Stop none commuter in terminal"), wxDefaultPosition, wxDefaultSize, 0 );
    m_StopNoneCommuter->SetValue(false);
    itemFlexGridSizer130->Add(m_StopNoneCommuter, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_SyncRoutes = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Synchronize routes"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SyncRoutes->SetValue(false);
    itemFlexGridSizer130->Add(m_SyncRoutes, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_TimedSensors = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Timed sensors"), wxDefaultPosition, wxDefaultSize, 0 );
    m_TimedSensors->SetValue(false);
    itemFlexGridSizer130->Add(m_TimedSensors, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EnableAnalyzer = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Enable analyzer"), wxDefaultPosition, wxDefaultSize, 0 );
    m_EnableAnalyzer->SetValue(true);
    itemFlexGridSizer130->Add(m_EnableAnalyzer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_ForceUnlock = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Force unlock at EBreak"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ForceUnlock->SetValue(false);
    itemFlexGridSizer130->Add(m_ForceUnlock, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_TriggerBlockEvents = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Trigger block events"), wxDefaultPosition, wxDefaultSize, 0 );
    m_TriggerBlockEvents->SetValue(true);
    itemFlexGridSizer130->Add(m_TriggerBlockEvents, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_ReleaseOnIdle = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Release Loco on idle"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ReleaseOnIdle->SetValue(false);
    itemFlexGridSizer130->Add(m_ReleaseOnIdle, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_CloseOnGhost = new wxCheckBox( m_AtomatPanel, wxID_ANY, _("Close on ghost"), wxDefaultPosition, wxDefaultSize, 0 );
    m_CloseOnGhost->SetValue(false);
    itemFlexGridSizer130->Add(m_CloseOnGhost, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxArrayString m_DefAspectStrings;
    m_DefAspectStrings.Add(_("&red"));
    m_DefAspectStrings.Add(_("&green"));
    m_DefAspectStrings.Add(_("&yellow"));
    m_DefAspectStrings.Add(_("&white"));
    m_DefAspect = new wxRadioBox( m_AtomatPanel, wxID_ANY, _("Default Signal Aspect"), wxDefaultPosition, wxDefaultSize, m_DefAspectStrings, 1, wxRA_SPECIFY_ROWS );
    m_DefAspect->SetSelection(0);
    itemFlexGridSizer130->Add(m_DefAspect, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_RRNotebook->AddPage(m_AtomatPanel, _("Automat"));

    m_ControllersPanel = new wxPanel( m_RRNotebook, ID_PANEL_RR_CONTROLLERS, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer156 = new wxBoxSizer(wxVERTICAL);
    m_ControllersPanel->SetSizer(itemBoxSizer156);

    wxArrayString m_ControllersStrings;
    m_Controllers = new wxListBox( m_ControllersPanel, ID_LISTBOX_RR_CONTROLLERS, wxDefaultPosition, wxDefaultSize, m_ControllersStrings, wxLB_SINGLE );
    itemBoxSizer156->Add(m_Controllers, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer158 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer156->Add(itemBoxSizer158, 0, wxGROW|wxALL, 5);
    m_ControllerDelete = new wxButton( m_ControllersPanel, ID_BUTTON_RR_DELETE, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer158->Add(m_ControllerDelete, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ControllerProps = new wxButton( m_ControllersPanel, ID_BUTTON_RR_PROPS, _("Properties"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer158->Add(m_ControllerProps, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer161Static = new wxStaticBox(m_ControllersPanel, wxID_ANY, _("Add Controller"));
    m_AddControllerBox = new wxStaticBoxSizer(itemStaticBoxSizer161Static, wxHORIZONTAL);
    itemBoxSizer156->Add(m_AddControllerBox, 0, wxGROW|wxALL, 5);
    wxArrayString m_LibStrings;
    m_Lib = new wxComboBox( m_ControllersPanel, ID_COMBOBOX_RR_LIB, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_LibStrings, wxCB_DROPDOWN );
    m_AddControllerBox->Add(m_Lib, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ControllerAdd = new wxButton( m_ControllersPanel, ID_BUTTON_RR_ADD, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    m_AddControllerBox->Add(m_ControllerAdd, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ControllerOptionsBox = new wxStaticBox(m_ControllersPanel, wxID_ANY, _("Options"));
    wxStaticBoxSizer* itemStaticBoxSizer164 = new wxStaticBoxSizer(m_ControllerOptionsBox, wxVERTICAL);
    itemBoxSizer156->Add(itemStaticBoxSizer164, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer165 = new wxFlexGridSizer(0, 2, 0, 0);
    itemStaticBoxSizer164->Add(itemFlexGridSizer165, 0, wxGROW, 5);
    m_PowerOffOnExit = new wxCheckBox( m_ControllersPanel, wxID_ANY, _("Power off on exit"), wxDefaultPosition, wxDefaultSize, 0 );
    m_PowerOffOnExit->SetValue(false);
    itemFlexGridSizer165->Add(m_PowerOffOnExit, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_RRNotebook->AddPage(m_ControllersPanel, _("Controllers"));

    m_R2RnetPanel = new wxPanel( m_RRNotebook, ID_PANEL_R2RNET, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer168 = new wxBoxSizer(wxVERTICAL);
    m_R2RnetPanel->SetSizer(itemBoxSizer168);

    wxFlexGridSizer* itemFlexGridSizer169 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer169->AddGrowableCol(1);
    itemBoxSizer168->Add(itemFlexGridSizer169, 0, wxGROW|wxALL, 5);
    m_labR2RnetID = new wxStaticText( m_R2RnetPanel, wxID_ANY, _("ID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer169->Add(m_labR2RnetID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_R2RnetID = new wxTextCtrl( m_R2RnetPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer169->Add(m_R2RnetID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labR2RnetAddr = new wxStaticText( m_R2RnetPanel, wxID_ANY, _("Address"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer169->Add(m_labR2RnetAddr, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_R2RnetAddr = new wxTextCtrl( m_R2RnetPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer169->Add(m_R2RnetAddr, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labR2RnetPort = new wxStaticText( m_R2RnetPanel, wxID_ANY, _("Port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer169->Add(m_labR2RnetPort, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_R2RnetPort = new wxSpinCtrl( m_R2RnetPanel, wxID_ANY, _T("1234"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 65535, 1234 );
    itemFlexGridSizer169->Add(m_R2RnetPort, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer176 = new wxFlexGridSizer(0, 3, 0, 0);
    itemFlexGridSizer176->AddGrowableCol(1);
    itemBoxSizer168->Add(itemFlexGridSizer176, 0, wxGROW|wxALL, 5);
    m_labR2RnetRoutes = new wxStaticText( m_R2RnetPanel, wxID_ANY, _("Routes file"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer176->Add(m_labR2RnetRoutes, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_R2RnetRoutes = new wxTextCtrl( m_R2RnetPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer176->Add(m_R2RnetRoutes, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_R2RnetRoutesDlg = new wxButton( m_R2RnetPanel, ID_BUTTON_R2RNET_ROUTES, _("..."), wxDefaultPosition, wxSize(30, 25), 0 );
    itemFlexGridSizer176->Add(m_R2RnetRoutesDlg, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_R2RnetEnable = new wxCheckBox( m_R2RnetPanel, wxID_ANY, _("Enable"), wxDefaultPosition, wxDefaultSize, 0 );
    m_R2RnetEnable->SetValue(false);
    itemBoxSizer168->Add(m_R2RnetEnable, 0, wxALIGN_LEFT|wxALL, 5);

    m_RRNotebook->AddPage(m_R2RnetPanel, _("R2Rnet"));

    itemBoxSizer2->Add(m_RRNotebook, 1, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer181 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer181, 0, wxALIGN_RIGHT|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer181->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer181->AddButton(m_Cancel);

    m_Apply = new wxButton( itemDialog1, wxID_APPLY, _("&Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    if (RocrailIniDialog::ShowToolTips())
        m_Apply->SetToolTip(_("Apply Controller settings locally."));
    itemStdDialogButtonSizer181->AddButton(m_Apply);

    itemStdDialogButtonSizer181->Realize();

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
  else if( StrOp.equals( wDigInt.xpressnet, wDigInt.getlib( m_Controller ) ) )
    m_CSDialog = new LenzDlg(this,m_Controller);
  else if( StrOp.equals( wDigInt.roco, wDigInt.getlib( m_Controller ) ) )
    m_CSDialog = new GenericCtrlDlg(this,m_Controller,wDigInt.getlib( m_Controller ));
  else if( StrOp.equals( wDigInt.sprog, wDigInt.getlib( m_Controller ) ) )
    m_CSDialog = new BarJuTCntrlDlg(this,m_Controller);
  else if( StrOp.equals( wDigInt.dcc232, wDigInt.getlib( m_Controller ) ) )
    m_CSDialog = new DCC232Dlg(this,m_Controller);
  else if( StrOp.equals( wDigInt.bidib, wDigInt.getlib( m_Controller ) ) )
    m_CSDialog = new BidibDlg(this,m_Controller);
  else if( StrOp.equals( wDigInt.cbus, wDigInt.getlib( m_Controller ) ) )
    m_CSDialog = new CbusDlg(this,m_Controller);
  else if( StrOp.equals( wDigInt.rocnet, wDigInt.getlib( m_Controller ) ) )
    m_CSDialog = new RocNetDlg(this,m_Controller);
  else if( StrOp.equals( wDigInt.massoth, wDigInt.getlib( m_Controller ) ) )
    m_CSDialog = new GenericCtrlDlg(this,m_Controller,wDigInt.getlib( m_Controller ), 57600, wDigInt.cts);
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
  /* set donation key */
  if( wGui.getdonkey(wxGetApp().getIni()) != NULL && StrOp.len(wGui.getdonkey(wxGetApp().getIni())) > 0 ) {
    wRocRail.setdonkey(m_Props, wGui.getdonkey(wxGetApp().getIni()));
    wRocRail.setdoneml(m_Props, wGui.getdoneml(wxGetApp().getIni()));
  }
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



/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_R2RNET_ROUTES
 */

void RocrailIniDialog::OnButtonR2rnetRoutesClick( wxCommandEvent& event )
{
  wxString ms_FileExt = _T("NetRoutes (*.xml)|*.xml");
  const char* l_openpath = ".";
  wxFileDialog* fdlg = new wxFileDialog(this, wxGetApp().getMenu("opennetroutesfile"), wxString(l_openpath,wxConvUTF8) , _T(""), ms_FileExt, wxFD_OPEN);
  if( fdlg->ShowModal() == wxID_OK ) {
    iONode r2rnet = wRocRail.getr2rnet( m_Props );
    wR2RnetIni.setroutes( r2rnet, fdlg->GetPath().mb_str(wxConvUTF8) );
    m_R2RnetRoutes->SetValue( wxString( wR2RnetIni.getroutes(r2rnet),wxConvUTF8) );
  }
}

