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
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "rocguiinidlg.h"
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

#include "rocguiinidlg.h"

////@begin XPM images
////@end XPM images

#include "rocview/wrapper/public/Gui.h"
#include "rocview/wrapper/public/RRCon.h"
#include "rocview/wrapper/public/PlanPanel.h"
#include "rocview/wrapper/public/MIC.h"
#include "rocview/wrapper/public/CVconf.h"
#include "rocrail/wrapper/public/JsMap.h"
#include "rocrail/wrapper/public/Trace.h"

#include "rocview/public/guiapp.h"

/*!
 * RocguiIniDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( RocguiIniDialog, wxDialog )

/*!
 * RocguiIniDialog event table definition
 */

BEGIN_EVENT_TABLE( RocguiIniDialog, wxDialog )

////@begin RocguiIniDialog event table entries
    EVT_BUTTON( ID_WORKSPACE, RocguiIniDialog::OnWorkspaceClick )

    EVT_BUTTON( ID_IMAGEPATH_BUTTON, RocguiIniDialog::OnImagepathButtonClick )

    EVT_BUTTON( ID_UPDATESPATH_BUTTON, RocguiIniDialog::OnUpdatespathButtonClick )

    EVT_BUTTON( ID_XSLPATH_BUTTON, RocguiIniDialog::OnXslpathButtonClick )

    EVT_BUTTON( ID_SVGPATH_BUTTON, RocguiIniDialog::OnSvgPath1Click )

    EVT_BUTTON( ID_SVGPATH2_BUTTON, RocguiIniDialog::OnSvgPath2Click )

    EVT_BUTTON( ID_SVGPATH3_BUTTON, RocguiIniDialog::OnSvgPath3Click )

    EVT_BUTTON( ID_SVGPATH4_BUTTON, RocguiIniDialog::OnSvgPath4Click )

    EVT_BUTTON( ID_SVGPATH5_BUTTON, RocguiIniDialog::OnSvgPath5Click )

    EVT_BUTTON( wxID_OK, RocguiIniDialog::OnOKClick )

    EVT_BUTTON( wxID_CANCEL, RocguiIniDialog::OnCANCELClick )

////@end RocguiIniDialog event table entries

END_EVENT_TABLE()

/*!
 * RocguiIniDialog constructors
 */

RocguiIniDialog::RocguiIniDialog( )
{
}

RocguiIniDialog::RocguiIniDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

RocguiIniDialog::RocguiIniDialog( wxWindow* parent, iONode ini )
{
  m_TabAlign = wxGetApp().getTabAlign();
  Create(parent, -1, _("Rocview"));
  initLabels();
  m_Ini = ini;
  initValues();

  m_TracePanel->GetSizer()->Layout();
  m_MICpanel->GetSizer()->Layout();
  m_PTpanel->GetSizer()->Layout();
  m_SVGpanel->GetSizer()->Layout();
  m_RICpanel->GetSizer()->Layout();

  m_Notebook->Fit();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
}


void RocguiIniDialog::initLabels() {
  m_Notebook->SetPageText( 0, wxGetApp().getMsg( "general" ) );
  m_Notebook->SetPageText( 1, wxGetApp().getMsg( "trace" ) );
  m_Notebook->SetPageText( 2, wxGetApp().getMsg( "programming" ) );
  m_Notebook->SetPageText( 3, _T( "SVG" ) );
  m_Notebook->SetPageText( 4, wxGetApp().getMsg( "gamepad" ) );
  m_Notebook->SetPageText( 5, wxGetApp().getMsg( "mic" ) );

    // Trace:
  m_TraceLevel->GetStaticBox()->SetLabel( wxGetApp().getMsg( "tracelevel" ) );
  m_Debug->SetLabel( wxGetApp().getMsg( "debug" ) );
  m_Parse->SetLabel( wxGetApp().getMsg( "parse" ) );
  m_TraceFileBox->GetStaticBox()->SetLabel( wxGetApp().getMsg( "tracefile" ) );

  m_RestrictEdit4Auto->SetLabel( wxGetApp().getMsg( "restrictedit4auto" ) );
  m_JsSupport->SetLabel( wxGetApp().getMsg( "gamepad" ) + _T(" support") );
  m_DispatchMode->SetLabel( wxGetApp().getMsg( "dispatchmode" ) );

  // MIC:
  m_labMICStep->SetLabel( wxGetApp().getMsg( "speedsteps" ) );
  m_MICStopOnExit->SetLabel( wxGetApp().getMsg( "stoponexit" ) );

  // PT:
  m_labCVnrs->SetLabel( _("CV") );
  m_labTimeout->SetLabel( _("timeout") );

  // SVG:
  m_SVGPathBox->SetLabel( wxGetApp().getMsg( "theme" ) + _T(" 1") );
  m_SVGPath2Box->SetLabel( wxGetApp().getMsg( "theme" ) + _T(" 2") );
  m_SVGPath3Box->SetLabel( wxGetApp().getMsg( "theme" ) + _T(" 3") );
  m_SVGPath4Box->SetLabel( wxGetApp().getMsg( "theme" ) + _T(" 4") );
  m_SVGPath5Box->SetLabel( wxGetApp().getMsg( "theme" ) + _T(" 5") );
  m_labItemIDpointsize->SetLabel( wxGetApp().getMsg( "itemidps" ) );

  // RIC:
  m_labVelocity->GetStaticBox()->SetLabel( wxGetApp().getMsg( "speed" ) );
  m_labRepeat->SetLabel( wxGetApp().getMsg( "repeat" ) );
  m_labDir->SetLabel( wxGetApp().getMsg( "dir" ) );
  m_labStop->SetLabel( wxGetApp().getMsg( "stop" ) );
  m_labRICStep->SetLabel( wxGetApp().getMsg( "speedsteps" ) );

  m_FunctionsBox->GetStaticBox()->SetLabel( wxGetApp().getMsg( "functions" ) );
  m_labLights->SetLabel( wxGetApp().getMsg( "lights" ) );

  m_PowerBox->GetStaticBox()->SetLabel( wxGetApp().getMsg( "power" ) );
  m_labPowerOn->SetLabel( wxGetApp().getMsg( "on" ) );
  m_labPowerOff->SetLabel( wxGetApp().getMsg( "off" ) );

  m_ClockType->SetLabel( wxGetApp().getMsg( "clocktype" ) );
  m_labImagePath->SetLabel( wxGetApp().getMsg( "imagepath" ) );
  m_labUpdatesPath->SetLabel( wxGetApp().getMsg( "updatespath" ) );
  m_labXSLPath->SetLabel( wxGetApp().getMsg( "xslpath" ) );
  m_labPanelSize->SetLabel( wxGetApp().getMsg( "panel" ) + _T(" ") + wxGetApp().getMsg( "size" ) );
  m_labWorkspace->SetLabel( wxGetApp().getMsg( "defaultworkspace" ) );
  m_Workspace->SetToolTip( wxGetApp().getMsg( "defaultworkspace" ) );
  m_StartDefaultWorkspace->SetLabel( wxGetApp().getMsg( "startdefaultworkspace" ) );
  m_RestoreLcCtrl->SetLabel( wxGetApp().getMsg( "restore" ) + _T(" ") + wxGetApp().getMsg( "locctrl" ) );

  m_TabAlignType->SetLabel( wxGetApp().getMsg( "tabalign" ) );
  m_TabAlignType->SetString( 0, wxGetApp().getMsg( "default" ) );
  m_TabAlignType->SetString( 1, wxGetApp().getMsg( "left" ) );
  m_TabAlignType->SetString( 2, wxGetApp().getMsg( "multiline" ) );
}


void RocguiIniDialog::initValues() {
  if( m_Ini == NULL )
    return;

  // Trace
  iONode trace = wGui.gettrace( m_Ini );
  if( trace == NULL ) {
    trace = NodeOp.inst( wTrace.name(), m_Ini, ELEMENT_NODE );
    NodeOp.addChild( m_Ini, trace );
  }
  m_Debug->SetValue( wTrace.isdebug( trace ) ? true:false );
  m_Parse->SetValue( wTrace.isparse( trace ) ? true:false );
  m_TraceFile->SetValue( wxString(wTrace.getgfile( trace ),wxConvUTF8) );

  m_RestrictEdit4Auto->SetValue( wGui.isrestrictedit4auto( m_Ini ) ? true:false );
  m_Check4Updates->SetValue( wGui.ischeckupdates( m_Ini ) ? true:false );
  m_JsSupport->SetValue( wGui.isjssupport( m_Ini ) ? true:false );
  m_DispatchMode->SetValue( wGui.isdispatchmode( m_Ini ) ? true:false );
  m_StartDefaultWorkspace->SetValue( wGui.isstartdefaultworkspace( m_Ini ) ? true:false );
  m_RestoreLcCtrl->SetValue( wGui.isrestorelcctrl( m_Ini ) ? true:false );

  // MIC
  iONode mic = wGui.getmic( m_Ini );
  if( mic == NULL ) {
    mic = NodeOp.inst( wMIC.name(), m_Ini, ELEMENT_NODE );
    NodeOp.addChild( m_Ini, mic );
  }
  m_MICStep->SetValue( wMIC.getstep(mic) );
  m_MICStopOnExit->SetValue( wMIC.isexitstop(mic) ? true:false );


  // PT
  iONode cvconf = wGui.getcvconf( m_Ini );
  if( cvconf == NULL ) {
    cvconf = NodeOp.inst( wCVconf.name(), m_Ini, ELEMENT_NODE );
    NodeOp.addChild( m_Ini, cvconf );
  }
  m_CVnrs->SetValue( wxString(wCVconf.getnrs( cvconf ),wxConvUTF8) );
  char* val = StrOp.fmt( "%d", wCVconf.gettimeout(cvconf) );
  m_Timeout->SetValue( wxString(val,wxConvUTF8) );
  StrOp.free(val);
  m_Lissy->SetValue( wCVconf.islissy(cvconf) ? true:false );
  m_SVtab->SetValue( wGui.islncvtab(m_Ini) ? true:false );


  // SVG
  iONode planpanel = wGui.getplanpanel( m_Ini );
  if( planpanel == NULL ) {
    planpanel = NodeOp.inst( wPlanPanel.name(), m_Ini, ELEMENT_NODE );
    NodeOp.addChild( m_Ini, planpanel );
  }

  m_CX->SetValue( wPlanPanel.getcx(planpanel) );
  m_CY->SetValue( wPlanPanel.getcy(planpanel) );

  m_SVGPath->SetValue( wxString(wPlanPanel.getsvgpath( planpanel ),wxConvUTF8) );
  m_SVGPath2->SetValue( wxString(wPlanPanel.getsvgpath2( planpanel ),wxConvUTF8) );
  m_SVGPath3->SetValue( wxString(wPlanPanel.getsvgpath3( planpanel ),wxConvUTF8) );
  m_SVGPath4->SetValue( wxString(wPlanPanel.getsvgpath4( planpanel ),wxConvUTF8) );
  m_SVGPath5->SetValue( wxString(wPlanPanel.getsvgpath5( planpanel ),wxConvUTF8) );
  m_ItemIDpointsize->SetValue(wPlanPanel.getitemidps( planpanel ));

  // RIC
  iONode jsmap = wGui.getjsmap( m_Ini );
  if( jsmap == NULL ) {
    jsmap = NodeOp.inst( wJsMap.name(), m_Ini, ELEMENT_NODE );
    NodeOp.addChild( m_Ini, jsmap );
  }
  m_RICStep->SetValue( wJsMap.getstep(jsmap) );

  val = StrOp.fmt( "%d", wJsMap.getupdown(jsmap) );
  m_Velocity->SetValue( wxString(val,wxConvUTF8) );
  StrOp.free(val);
  val = StrOp.fmt( "%d", wJsMap.getrepeat(jsmap) );
  m_Repeat->SetValue( wxString(val,wxConvUTF8) );
  StrOp.free(val);
  val = StrOp.fmt( "%d", wJsMap.getreverse(jsmap) );
  m_Dir->SetValue( wxString(val,wxConvUTF8) );
  StrOp.free(val);
  val = StrOp.fmt( "%d", wJsMap.getstop(jsmap) );
  m_Stop->SetValue( wxString(val,wxConvUTF8) );
  StrOp.free(val);
  val = StrOp.fmt( "%d", wJsMap.getlight(jsmap) );
  m_Lights->SetValue( wxString(val,wxConvUTF8) );
  StrOp.free(val);
  val = StrOp.fmt( "%d", wJsMap.getf1(jsmap) );
  m_F1->SetValue( wxString(val,wxConvUTF8) );
  StrOp.free(val);
  val = StrOp.fmt( "%d", wJsMap.getf2(jsmap) );
  m_F2->SetValue( wxString(val,wxConvUTF8) );
  StrOp.free(val);
  val = StrOp.fmt( "%d", wJsMap.getf3(jsmap) );
  m_F3->SetValue( wxString(val,wxConvUTF8) );
  StrOp.free(val);
  val = StrOp.fmt( "%d", wJsMap.getf4(jsmap) );
  m_F4->SetValue( wxString(val,wxConvUTF8) );
  StrOp.free(val);
  val = StrOp.fmt( "%d", wJsMap.getf5(jsmap) );
  m_F5->SetValue( wxString(val,wxConvUTF8) );
  StrOp.free(val);
  val = StrOp.fmt( "%d", wJsMap.getf6(jsmap) );
  m_F6->SetValue( wxString(val,wxConvUTF8) );
  StrOp.free(val);
  val = StrOp.fmt( "%d", wJsMap.getf7(jsmap) );
  m_F7->SetValue( wxString(val,wxConvUTF8) );
  StrOp.free(val);
  val = StrOp.fmt( "%d", wJsMap.getf8(jsmap) );
  m_F8->SetValue( wxString(val,wxConvUTF8) );
  StrOp.free(val);
  val = StrOp.fmt( "%d", wJsMap.getf9(jsmap) );
  m_F9->SetValue( wxString(val,wxConvUTF8) );
  StrOp.free(val);
  val = StrOp.fmt( "%d", wJsMap.getf10(jsmap) );
  m_F10->SetValue( wxString(val,wxConvUTF8) );
  StrOp.free(val);
  val = StrOp.fmt( "%d", wJsMap.getf11(jsmap) );
  m_F11->SetValue( wxString(val,wxConvUTF8) );
  StrOp.free(val);
  val = StrOp.fmt( "%d", wJsMap.getf12(jsmap) );
  m_F12->SetValue( wxString(val,wxConvUTF8) );
  StrOp.free(val);
  val = StrOp.fmt( "%d", wJsMap.getpoweron(jsmap) );
  m_PowerOn->SetValue( wxString(val,wxConvUTF8) );
  StrOp.free(val);
  val = StrOp.fmt( "%d", wJsMap.getpoweroff(jsmap) );
  m_PowerOff->SetValue( wxString(val,wxConvUTF8) );
  StrOp.free(val);

  // Clocktype
  const char* clocktype = wGui.getclocktype( m_Ini );
  int type = 0;
  if( StrOp.equals( wGui.clock_ampm, clocktype ) ) type = 1;
  else if( StrOp.equals( wGui.clock_24h, clocktype ) ) type = 2;
  m_ClockType->SetSelection( type );

  m_TabAlignType->SetSelection( wGui.gettabalign( m_Ini ) );

  m_ImagePath->SetValue( wxString(wGui.getimagepath(m_Ini),wxConvUTF8) );
  m_UpdatesPath->SetValue( wxString(wGui.getupdatespath(m_Ini),wxConvUTF8) );
  m_XSLPath->SetValue( wxString(wGui.getxslpath(m_Ini),wxConvUTF8) );
  m_Workspace->SetValue( wxString(wGui.getdefaultworkspace(m_Ini),wxConvUTF8) );

}


void RocguiIniDialog::evaluate() {
  if( m_Ini == NULL )
    return;

  // Trace
  iONode trace = wGui.gettrace( m_Ini );
  if( trace == NULL ) {
    trace = NodeOp.inst( wTrace.name(), m_Ini, ELEMENT_NODE);
    NodeOp.addChild( m_Ini, trace );
  }

  wTrace.setdebug( trace, m_Debug->GetValue() ? True:False );
  wTrace.setparse( trace, m_Parse->GetValue() ? True:False );
  wTrace.setgfile( trace, m_TraceFile->GetValue().mb_str(wxConvUTF8) );

  wGui.setrestrictedit4auto( m_Ini, m_RestrictEdit4Auto->GetValue() ? True:False );
  wGui.setcheckupdates( m_Ini, m_Check4Updates->GetValue() ? True:False );
  wGui.setjssupport( m_Ini, m_JsSupport->GetValue() ? True:False );
  wGui.setdispatchmode( m_Ini, m_DispatchMode->GetValue() ? True:False );
  wGui.setstartdefaultworkspace( m_Ini, m_StartDefaultWorkspace->GetValue() ? True:False );
  wGui.setrestorelcctrl( m_Ini, m_RestoreLcCtrl->GetValue() ? True:False );

  // MIC
  iONode mic = wGui.getmic( m_Ini );
  if( mic == NULL ) {
    mic = NodeOp.inst( wMIC.name(), m_Ini, ELEMENT_NODE);
    NodeOp.addChild( m_Ini, mic );
  }
  wMIC.setstep(mic, m_MICStep->GetValue());
  wMIC.setexitstop( mic, m_MICStopOnExit->GetValue() ? True:False );


  // PT
  iONode cvconf = wGui.getcvconf( m_Ini );
  if( cvconf == NULL ) {
    cvconf = NodeOp.inst( wCVconf.name(), m_Ini, ELEMENT_NODE);
    NodeOp.addChild( m_Ini, cvconf );
  }
  wCVconf.setnrs( cvconf, m_CVnrs->GetValue().mb_str(wxConvUTF8) );
  wCVconf.settimeout(cvconf, atoi(m_Timeout->GetValue().mb_str(wxConvUTF8)));
  wCVconf.setlissy( cvconf, m_Lissy->GetValue() ? True:False );
  wGui.setlncvtab(m_Ini, m_SVtab->GetValue() ? True:False );

  // SVG
  iONode planpanel = wGui.getplanpanel( m_Ini );
  if( planpanel == NULL ) {
    planpanel = NodeOp.inst( wPlanPanel.name(), m_Ini, ELEMENT_NODE);
    NodeOp.addChild( m_Ini, planpanel );
  }
  wPlanPanel.setcx(planpanel, m_CX->GetValue() );
  wPlanPanel.setcy(planpanel, m_CY->GetValue() );

  wPlanPanel.setsvgpath( planpanel, m_SVGPath->GetValue().mb_str(wxConvUTF8) );
  wPlanPanel.setsvgpath2( planpanel, m_SVGPath2->GetValue().mb_str(wxConvUTF8) );
  wPlanPanel.setsvgpath3( planpanel, m_SVGPath3->GetValue().mb_str(wxConvUTF8) );
  wPlanPanel.setsvgpath4( planpanel, m_SVGPath4->GetValue().mb_str(wxConvUTF8) );
  wPlanPanel.setsvgpath5( planpanel, m_SVGPath5->GetValue().mb_str(wxConvUTF8) );
  wPlanPanel.setitemidps( planpanel, m_ItemIDpointsize->GetValue());

  // RIC
  iONode jsmap = wGui.getjsmap( m_Ini );
  if( jsmap == NULL ) {
    planpanel = NodeOp.inst( wJsMap.name(), m_Ini, ELEMENT_NODE);
    NodeOp.addChild( m_Ini, jsmap );
  }
  wJsMap.setstep(jsmap, m_RICStep->GetValue());
  wJsMap.setupdown(jsmap, atoi(m_Velocity->GetValue().mb_str(wxConvUTF8)));
  wJsMap.setrepeat(jsmap, atoi(m_Repeat->GetValue().mb_str(wxConvUTF8)));
  wJsMap.setreverse(jsmap, atoi(m_Dir->GetValue().mb_str(wxConvUTF8)));
  wJsMap.setstop(jsmap, atoi(m_Stop->GetValue().mb_str(wxConvUTF8)));
  wJsMap.setlight(jsmap, atoi(m_Lights->GetValue().mb_str(wxConvUTF8)));
  wJsMap.setf1(jsmap, atoi(m_F1->GetValue().mb_str(wxConvUTF8)));
  wJsMap.setf2(jsmap, atoi(m_F2->GetValue().mb_str(wxConvUTF8)));
  wJsMap.setf3(jsmap, atoi(m_F3->GetValue().mb_str(wxConvUTF8)));
  wJsMap.setf4(jsmap, atoi(m_F4->GetValue().mb_str(wxConvUTF8)));
  wJsMap.setf5(jsmap, atoi(m_F5->GetValue().mb_str(wxConvUTF8)));
  wJsMap.setf6(jsmap, atoi(m_F6->GetValue().mb_str(wxConvUTF8)));
  wJsMap.setf7(jsmap, atoi(m_F7->GetValue().mb_str(wxConvUTF8)));
  wJsMap.setf8(jsmap, atoi(m_F8->GetValue().mb_str(wxConvUTF8)));
  wJsMap.setf9(jsmap, atoi(m_F9->GetValue().mb_str(wxConvUTF8)));
  wJsMap.setf10(jsmap, atoi(m_F10->GetValue().mb_str(wxConvUTF8)));
  wJsMap.setf11(jsmap, atoi(m_F11->GetValue().mb_str(wxConvUTF8)));
  wJsMap.setf12(jsmap, atoi(m_F12->GetValue().mb_str(wxConvUTF8)));
  wJsMap.setpoweron(jsmap, atoi(m_PowerOn->GetValue().mb_str(wxConvUTF8)));
  wJsMap.setpoweroff(jsmap, atoi(m_PowerOff->GetValue().mb_str(wxConvUTF8)));

  // Clocktype
  int type = m_ClockType->GetSelection();
  if( type == 0 )
    wGui.setclocktype( m_Ini, wGui.clock_default );
  else if( type == 1 )
    wGui.setclocktype( m_Ini, wGui.clock_ampm );
  else if( type == 2 )
    wGui.setclocktype( m_Ini, wGui.clock_24h );

  wGui.settabalign( m_Ini, m_TabAlignType->GetSelection() );

  wGui.setimagepath( m_Ini, m_ImagePath->GetValue().mb_str(wxConvUTF8) );
  wGui.setupdatespath( m_Ini, m_UpdatesPath->GetValue().mb_str(wxConvUTF8) );
  wGui.setxslpath( m_Ini, m_XSLPath->GetValue().mb_str(wxConvUTF8) );
  wGui.setdefaultworkspace( m_Ini, m_Workspace->GetValue().mb_str(wxConvUTF8) );
}


/*!
 * Dialog creator
 */

bool RocguiIniDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin RocguiIniDialog member initialisation
    m_Notebook = NULL;
    m_GeneralTab = NULL;
    m_ClockType = NULL;
    m_TabAlignType = NULL;
    m_RestrictEdit4Auto = NULL;
    m_Check4Updates = NULL;
    m_JsSupport = NULL;
    m_DispatchMode = NULL;
    m_RestoreLcCtrl = NULL;
    m_labWorkspace = NULL;
    m_Workspace = NULL;
    m_WorkspaceDlg = NULL;
    m_StartDefaultWorkspace = NULL;
    m_labImagePath = NULL;
    m_ImagePath = NULL;
    m_ImagePathDlg = NULL;
    m_labUpdatesPath = NULL;
    m_UpdatesPath = NULL;
    m_UpdatePathDlg = NULL;
    m_labXSLPath = NULL;
    m_XSLPath = NULL;
    m_XSLPathDlg = NULL;
    m_labPanelSize = NULL;
    m_labCX = NULL;
    m_CX = NULL;
    m_labCY = NULL;
    m_CY = NULL;
    m_TracePanel = NULL;
    m_TraceLevel = NULL;
    m_Debug = NULL;
    m_Parse = NULL;
    m_TraceFileBox = NULL;
    m_TraceFile = NULL;
    m_PTpanel = NULL;
    m_labCVnrs = NULL;
    m_CVnrs = NULL;
    m_labTimeout = NULL;
    m_Timeout = NULL;
    m_Lissy = NULL;
    m_SVtab = NULL;
    m_SVGpanel = NULL;
    m_SVGPathBox = NULL;
    m_SVGPath = NULL;
    m_SvgPathDlg = NULL;
    m_SVGPath2Box = NULL;
    m_SVGPath2 = NULL;
    m_SvgPath2Dlg = NULL;
    m_SVGPath3Box = NULL;
    m_SVGPath3 = NULL;
    m_SvgPath3Dlg = NULL;
    m_SVGPath4Box = NULL;
    m_SVGPath4 = NULL;
    m_SvgPath4Dlg = NULL;
    m_SVGPath5Box = NULL;
    m_SVGPath5 = NULL;
    m_SvgPath5Dlg = NULL;
    m_labItemIDpointsize = NULL;
    m_ItemIDpointsize = NULL;
    m_RICpanel = NULL;
    m_labVelocity = NULL;
    m_labY = NULL;
    m_Velocity = NULL;
    m_labRepeat = NULL;
    m_Repeat = NULL;
    m_labDir = NULL;
    m_Dir = NULL;
    m_labStop = NULL;
    m_Stop = NULL;
    m_labRICStep = NULL;
    m_RICStep = NULL;
    m_FunctionsBox = NULL;
    m_labLights = NULL;
    m_Lights = NULL;
    m_labF1 = NULL;
    m_F1 = NULL;
    m_labF2 = NULL;
    m_F2 = NULL;
    m_labF3 = NULL;
    m_F3 = NULL;
    m_labF4 = NULL;
    m_F4 = NULL;
    m_labF5 = NULL;
    m_F5 = NULL;
    m_labF6 = NULL;
    m_F6 = NULL;
    m_labF7 = NULL;
    m_F7 = NULL;
    m_labF8 = NULL;
    m_F8 = NULL;
    m_labF9 = NULL;
    m_F9 = NULL;
    m_labF10 = NULL;
    m_F10 = NULL;
    m_labF11 = NULL;
    m_F11 = NULL;
    m_labF12 = NULL;
    m_F12 = NULL;
    m_PowerBox = NULL;
    m_labPowerOn = NULL;
    m_PowerOn = NULL;
    m_labPowerOff = NULL;
    m_PowerOff = NULL;
    m_MICpanel = NULL;
    m_labMICStep = NULL;
    m_MICStep = NULL;
    m_MICStopOnExit = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
////@end RocguiIniDialog member initialisation

////@begin RocguiIniDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end RocguiIniDialog creation
    return true;
}

/*!
 * Control creation for Dialog
 */

void RocguiIniDialog::CreateControls()
{
////@begin RocguiIniDialog content construction
    RocguiIniDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_Notebook = new wxNotebook( itemDialog1, ID_NOTEBOOK1, wxDefaultPosition, wxDefaultSize, m_TabAlign );

    m_GeneralTab = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    m_GeneralTab->SetSizer(itemBoxSizer5);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer5->Add(itemBoxSizer6, 0, wxALIGN_TOP|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer7 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer6->Add(itemFlexGridSizer7, 0, wxGROW|wxALL, 5);
    wxArrayString m_ClockTypeStrings;
    m_ClockTypeStrings.Add(_("&Default"));
    m_ClockTypeStrings.Add(_("&AM/PM"));
    m_ClockTypeStrings.Add(_("&24h"));
    m_ClockType = new wxRadioBox( m_GeneralTab, wxID_ANY, _("Clocktype"), wxDefaultPosition, wxDefaultSize, m_ClockTypeStrings, 1, wxRA_SPECIFY_COLS );
    m_ClockType->SetSelection(0);
    itemFlexGridSizer7->Add(m_ClockType, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_TabAlignTypeStrings;
    m_TabAlignTypeStrings.Add(_("&default"));
    m_TabAlignTypeStrings.Add(_("&left"));
    m_TabAlignTypeStrings.Add(_("&multiline"));
    m_TabAlignType = new wxRadioBox( m_GeneralTab, wxID_ANY, _("TabAlign"), wxDefaultPosition, wxDefaultSize, m_TabAlignTypeStrings, 1, wxRA_SPECIFY_COLS );
    m_TabAlignType->SetSelection(0);
    itemFlexGridSizer7->Add(m_TabAlignType, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_RestrictEdit4Auto = new wxCheckBox( m_GeneralTab, wxID_ANY, _("Restrict edit in automode"), wxDefaultPosition, wxDefaultSize, 0 );
    m_RestrictEdit4Auto->SetValue(false);
    itemBoxSizer6->Add(m_RestrictEdit4Auto, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_Check4Updates = new wxCheckBox( m_GeneralTab, wxID_ANY, _("Check for updates"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Check4Updates->SetValue(false);
    itemBoxSizer6->Add(m_Check4Updates, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_JsSupport = new wxCheckBox( m_GeneralTab, wxID_ANY, _("Gamepad support"), wxDefaultPosition, wxDefaultSize, 0 );
    m_JsSupport->SetValue(false);
    itemBoxSizer6->Add(m_JsSupport, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_DispatchMode = new wxCheckBox( m_GeneralTab, wxID_ANY, _("Use mode column to dispatch"), wxDefaultPosition, wxDefaultSize, 0 );
    m_DispatchMode->SetValue(false);
    itemBoxSizer6->Add(m_DispatchMode, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_RestoreLcCtrl = new wxCheckBox( m_GeneralTab, wxID_ANY, _("Restore throttle"), wxDefaultPosition, wxDefaultSize, 0 );
    m_RestoreLcCtrl->SetValue(true);
    itemBoxSizer6->Add(m_RestoreLcCtrl, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labWorkspace = new wxStaticBox(m_GeneralTab, wxID_ANY, _("Default Workspace"));
    wxStaticBoxSizer* itemStaticBoxSizer15 = new wxStaticBoxSizer(m_labWorkspace, wxHORIZONTAL);
    itemBoxSizer6->Add(itemStaticBoxSizer15, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer16 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer16->AddGrowableCol(0);
    itemStaticBoxSizer15->Add(itemFlexGridSizer16, 1, wxALIGN_CENTER_VERTICAL, 5);
    m_Workspace = new wxTextCtrl( m_GeneralTab, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer16->Add(m_Workspace, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_WorkspaceDlg = new wxButton( m_GeneralTab, ID_WORKSPACE, _("..."), wxDefaultPosition, wxSize(40, 25), 0 );
    itemFlexGridSizer16->Add(m_WorkspaceDlg, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxRIGHT, 5);

    m_StartDefaultWorkspace = new wxCheckBox( m_GeneralTab, wxID_ANY, _("Use at startup"), wxDefaultPosition, wxDefaultSize, 0 );
    m_StartDefaultWorkspace->SetValue(false);
    itemFlexGridSizer16->Add(m_StartDefaultWorkspace, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer20 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer5->Add(itemBoxSizer20, 0, wxALIGN_TOP|wxALL, 5);
    m_labImagePath = new wxStaticBox(m_GeneralTab, wxID_ANY, _("Image Path"));
    wxStaticBoxSizer* itemStaticBoxSizer21 = new wxStaticBoxSizer(m_labImagePath, wxHORIZONTAL);
    itemBoxSizer20->Add(itemStaticBoxSizer21, 0, wxGROW|wxALL, 5);
    m_ImagePath = new wxTextCtrl( m_GeneralTab, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0 );
    itemStaticBoxSizer21->Add(m_ImagePath, 1, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_ImagePathDlg = new wxButton( m_GeneralTab, ID_IMAGEPATH_BUTTON, _("..."), wxDefaultPosition, wxSize(40, 25), 0 );
    itemStaticBoxSizer21->Add(m_ImagePathDlg, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5);

    m_labUpdatesPath = new wxStaticBox(m_GeneralTab, wxID_ANY, _("Updates Path"));
    wxStaticBoxSizer* itemStaticBoxSizer24 = new wxStaticBoxSizer(m_labUpdatesPath, wxHORIZONTAL);
    itemBoxSizer20->Add(itemStaticBoxSizer24, 0, wxGROW|wxALL, 5);
    m_UpdatesPath = new wxTextCtrl( m_GeneralTab, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0 );
    itemStaticBoxSizer24->Add(m_UpdatesPath, 1, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_UpdatePathDlg = new wxButton( m_GeneralTab, ID_UPDATESPATH_BUTTON, _("..."), wxDefaultPosition, wxSize(40, 25), 0 );
    itemStaticBoxSizer24->Add(m_UpdatePathDlg, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5);

    m_labXSLPath = new wxStaticBox(m_GeneralTab, wxID_ANY, _("Stylesheets Path"));
    wxStaticBoxSizer* itemStaticBoxSizer27 = new wxStaticBoxSizer(m_labXSLPath, wxHORIZONTAL);
    itemBoxSizer20->Add(itemStaticBoxSizer27, 0, wxGROW|wxALL, 5);
    m_XSLPath = new wxTextCtrl( m_GeneralTab, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0 );
    itemStaticBoxSizer27->Add(m_XSLPath, 1, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_XSLPathDlg = new wxButton( m_GeneralTab, ID_XSLPATH_BUTTON, _("..."), wxDefaultPosition, wxSize(40, 25), 0 );
    itemStaticBoxSizer27->Add(m_XSLPathDlg, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5);

    m_labPanelSize = new wxStaticBox(m_GeneralTab, wxID_ANY, _("Planpanel size"));
    wxStaticBoxSizer* itemStaticBoxSizer30 = new wxStaticBoxSizer(m_labPanelSize, wxHORIZONTAL);
    itemBoxSizer20->Add(itemStaticBoxSizer30, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer31 = new wxFlexGridSizer(0, 2, 0, 0);
    itemStaticBoxSizer30->Add(itemFlexGridSizer31, 0, wxGROW, 5);
    m_labCX = new wxStaticText( m_GeneralTab, wxID_ANY, _("CX"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer31->Add(m_labCX, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_CX = new wxSpinCtrl( m_GeneralTab, wxID_ANY, _T("64"), wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 0, 256, 64 );
    itemFlexGridSizer31->Add(m_CX, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labCY = new wxStaticText( m_GeneralTab, wxID_ANY, _("CY"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer31->Add(m_labCY, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_CY = new wxSpinCtrl( m_GeneralTab, wxID_ANY, _T("48"), wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 0, 256, 48 );
    itemFlexGridSizer31->Add(m_CY, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Notebook->AddPage(m_GeneralTab, _("General"));

    m_TracePanel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer37 = new wxBoxSizer(wxVERTICAL);
    m_TracePanel->SetSizer(itemBoxSizer37);

    wxStaticBox* itemStaticBoxSizer38Static = new wxStaticBox(m_TracePanel, wxID_ANY, _("Level"));
    m_TraceLevel = new wxStaticBoxSizer(itemStaticBoxSizer38Static, wxVERTICAL);
    itemBoxSizer37->Add(m_TraceLevel, 0, wxGROW|wxALL, 5);
    m_Debug = new wxCheckBox( m_TracePanel, wxID_ANY, _("Debug"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_Debug->SetValue(false);
    m_TraceLevel->Add(m_Debug, 0, wxALIGN_LEFT|wxALL, 5);

    m_Parse = new wxCheckBox( m_TracePanel, wxID_ANY, _("XML Parser"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_Parse->SetValue(false);
    m_TraceLevel->Add(m_Parse, 0, wxALIGN_LEFT|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer41Static = new wxStaticBox(m_TracePanel, wxID_ANY, _("File"));
    m_TraceFileBox = new wxStaticBoxSizer(itemStaticBoxSizer41Static, wxHORIZONTAL);
    itemBoxSizer37->Add(m_TraceFileBox, 0, wxGROW|wxALL, 5);
    m_TraceFile = new wxTextCtrl( m_TracePanel, ID_TEXTCTRL_RG_TRACEFILE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_TraceFileBox->Add(m_TraceFile, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_TracePanel, _("Trace"));

    m_PTpanel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer44 = new wxBoxSizer(wxVERTICAL);
    m_PTpanel->SetSizer(itemBoxSizer44);

    wxFlexGridSizer* itemFlexGridSizer45 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer45->AddGrowableCol(1);
    itemBoxSizer44->Add(itemFlexGridSizer45, 0, wxGROW|wxALL, 5);
    m_labCVnrs = new wxStaticText( m_PTpanel, wxID_ANY, _("CV nrs"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer45->Add(m_labCVnrs, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_CVnrs = new wxTextCtrl( m_PTpanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer45->Add(m_CVnrs, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labTimeout = new wxStaticText( m_PTpanel, wxID_ANY, _("Timeout"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer45->Add(m_labTimeout, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Timeout = new wxTextCtrl( m_PTpanel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer45->Add(m_Timeout, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Lissy = new wxCheckBox( m_PTpanel, wxID_ANY, _("Lissy addressing"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Lissy->SetValue(false);
    itemBoxSizer44->Add(m_Lissy, 0, wxALIGN_LEFT|wxALL, 5);

    m_SVtab = new wxCheckBox( m_PTpanel, wxID_ANY, _("LocoNet SV Tab"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SVtab->SetValue(false);
    itemBoxSizer44->Add(m_SVtab, 0, wxALIGN_LEFT|wxALL, 5);

    m_Notebook->AddPage(m_PTpanel, _("PT"));

    m_SVGpanel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer53 = new wxBoxSizer(wxVERTICAL);
    m_SVGpanel->SetSizer(itemBoxSizer53);

    m_SVGPathBox = new wxStaticBox(m_SVGpanel, wxID_ANY, _("Theme 1"));
    wxStaticBoxSizer* itemStaticBoxSizer54 = new wxStaticBoxSizer(m_SVGPathBox, wxHORIZONTAL);
    itemBoxSizer53->Add(itemStaticBoxSizer54, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, 5);
    m_SVGPath = new wxTextCtrl( m_SVGpanel, wxID_ANY, _("."), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer54->Add(m_SVGPath, 1, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    m_SvgPathDlg = new wxButton( m_SVGpanel, ID_SVGPATH_BUTTON, _("..."), wxDefaultPosition, wxSize(30, 25), 0 );
    itemStaticBoxSizer54->Add(m_SvgPathDlg, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_SVGPath2Box = new wxStaticBox(m_SVGpanel, wxID_ANY, _("Theme 2"));
    wxStaticBoxSizer* itemStaticBoxSizer57 = new wxStaticBoxSizer(m_SVGPath2Box, wxHORIZONTAL);
    itemBoxSizer53->Add(itemStaticBoxSizer57, 0, wxGROW|wxLEFT|wxRIGHT, 5);
    m_SVGPath2 = new wxTextCtrl( m_SVGpanel, wxID_ANY, _("."), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer57->Add(m_SVGPath2, 1, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    m_SvgPath2Dlg = new wxButton( m_SVGpanel, ID_SVGPATH2_BUTTON, _("..."), wxDefaultPosition, wxSize(30, 25), 0 );
    itemStaticBoxSizer57->Add(m_SvgPath2Dlg, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_SVGPath3Box = new wxStaticBox(m_SVGpanel, wxID_ANY, _("Theme 3"));
    wxStaticBoxSizer* itemStaticBoxSizer60 = new wxStaticBoxSizer(m_SVGPath3Box, wxHORIZONTAL);
    itemBoxSizer53->Add(itemStaticBoxSizer60, 0, wxGROW|wxLEFT|wxRIGHT, 5);
    m_SVGPath3 = new wxTextCtrl( m_SVGpanel, wxID_ANY, _("."), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer60->Add(m_SVGPath3, 1, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    m_SvgPath3Dlg = new wxButton( m_SVGpanel, ID_SVGPATH3_BUTTON, _("..."), wxDefaultPosition, wxSize(30, 25), 0 );
    itemStaticBoxSizer60->Add(m_SvgPath3Dlg, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_SVGPath4Box = new wxStaticBox(m_SVGpanel, wxID_ANY, _("Theme 4"));
    wxStaticBoxSizer* itemStaticBoxSizer63 = new wxStaticBoxSizer(m_SVGPath4Box, wxHORIZONTAL);
    itemBoxSizer53->Add(itemStaticBoxSizer63, 0, wxGROW|wxLEFT|wxRIGHT, 5);
    m_SVGPath4 = new wxTextCtrl( m_SVGpanel, wxID_ANY, _("."), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer63->Add(m_SVGPath4, 1, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    m_SvgPath4Dlg = new wxButton( m_SVGpanel, ID_SVGPATH4_BUTTON, _("..."), wxDefaultPosition, wxSize(30, 25), 0 );
    itemStaticBoxSizer63->Add(m_SvgPath4Dlg, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_SVGPath5Box = new wxStaticBox(m_SVGpanel, wxID_ANY, _("Theme 5"));
    wxStaticBoxSizer* itemStaticBoxSizer66 = new wxStaticBoxSizer(m_SVGPath5Box, wxHORIZONTAL);
    itemBoxSizer53->Add(itemStaticBoxSizer66, 0, wxGROW|wxLEFT|wxRIGHT, 5);
    m_SVGPath5 = new wxTextCtrl( m_SVGpanel, wxID_ANY, _("."), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer66->Add(m_SVGPath5, 1, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    m_SvgPath5Dlg = new wxButton( m_SVGpanel, ID_SVGPATH5_BUTTON, _("..."), wxDefaultPosition, wxSize(30, 25), 0 );
    itemStaticBoxSizer66->Add(m_SvgPath5Dlg, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxFlexGridSizer* itemFlexGridSizer69 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer53->Add(itemFlexGridSizer69, 0, wxGROW|wxLEFT|wxRIGHT, 5);
    m_labItemIDpointsize = new wxStaticText( m_SVGpanel, wxID_ANY, _("Item ID pointsize"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer69->Add(m_labItemIDpointsize, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ItemIDpointsize = new wxSpinCtrl( m_SVGpanel, wxID_ANY, _T("7"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 5, 10, 7 );
    itemFlexGridSizer69->Add(m_ItemIDpointsize, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_SVGpanel, _("SVG"));

    m_RICpanel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer73 = new wxBoxSizer(wxVERTICAL);
    m_RICpanel->SetSizer(itemBoxSizer73);

    wxStaticBox* itemStaticBoxSizer74Static = new wxStaticBox(m_RICpanel, wxID_ANY, _("Velocity"));
    m_labVelocity = new wxStaticBoxSizer(itemStaticBoxSizer74Static, wxVERTICAL);
    itemBoxSizer73->Add(m_labVelocity, 0, wxGROW, 5);
    wxFlexGridSizer* itemFlexGridSizer75 = new wxFlexGridSizer(2, 4, 0, 0);
    m_labVelocity->Add(itemFlexGridSizer75, 0, wxGROW, 5);
    m_labY = new wxStaticText( m_RICpanel, wxID_ANY, _("V"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer75->Add(m_labY, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Velocity = new wxTextCtrl( m_RICpanel, wxID_ANY, _("5"), wxDefaultPosition, wxSize(40, -1), wxTE_CENTRE );
    itemFlexGridSizer75->Add(m_Velocity, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labRepeat = new wxStaticText( m_RICpanel, wxID_ANY, _("repeat"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer75->Add(m_labRepeat, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 5);

    m_Repeat = new wxTextCtrl( m_RICpanel, wxID_ANY, _("500"), wxDefaultPosition, wxSize(50, -1), wxTE_CENTRE );
    itemFlexGridSizer75->Add(m_Repeat, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labDir = new wxStaticText( m_RICpanel, wxID_ANY, _("dir"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer75->Add(m_labDir, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    m_Dir = new wxTextCtrl( m_RICpanel, wxID_ANY, _("4"), wxDefaultPosition, wxSize(40, -1), wxTE_CENTRE );
    itemFlexGridSizer75->Add(m_Dir, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labStop = new wxStaticText( m_RICpanel, wxID_ANY, _("stop"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer75->Add(m_labStop, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    m_Stop = new wxTextCtrl( m_RICpanel, wxID_ANY, _("6"), wxDefaultPosition, wxSize(40, -1), wxTE_CENTRE );
    itemFlexGridSizer75->Add(m_Stop, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxFlexGridSizer* itemFlexGridSizer84 = new wxFlexGridSizer(2, 2, 0, 0);
    m_labVelocity->Add(itemFlexGridSizer84, 0, wxGROW, 5);
    m_labRICStep = new wxStaticText( m_RICpanel, wxID_ANY, _("step"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer84->Add(m_labRICStep, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_RICStep = new wxSpinCtrl( m_RICpanel, wxID_ANY, _T("1"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 1, 9, 1 );
    itemFlexGridSizer84->Add(m_RICStep, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer87Static = new wxStaticBox(m_RICpanel, wxID_ANY, _("Functions"));
    m_FunctionsBox = new wxStaticBoxSizer(itemStaticBoxSizer87Static, wxVERTICAL);
    itemBoxSizer73->Add(m_FunctionsBox, 0, wxGROW, 5);
    wxFlexGridSizer* itemFlexGridSizer88 = new wxFlexGridSizer(2, 6, 0, 0);
    m_FunctionsBox->Add(itemFlexGridSizer88, 0, wxGROW, 5);
    m_labLights = new wxStaticText( m_RICpanel, wxID_ANY, _("lights"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer88->Add(m_labLights, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 5);

    m_Lights = new wxTextCtrl( m_RICpanel, wxID_ANY, _("5"), wxDefaultPosition, wxSize(40, -1), wxTE_CENTRE );
    itemFlexGridSizer88->Add(m_Lights, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labF1 = new wxStaticText( m_RICpanel, wxID_ANY, _("F1"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer88->Add(m_labF1, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 5);

    m_F1 = new wxTextCtrl( m_RICpanel, wxID_ANY, _("0"), wxDefaultPosition, wxSize(40, -1), wxTE_CENTRE );
    itemFlexGridSizer88->Add(m_F1, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labF2 = new wxStaticText( m_RICpanel, wxID_ANY, _("F2"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer88->Add(m_labF2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 5);

    m_F2 = new wxTextCtrl( m_RICpanel, wxID_ANY, _("1"), wxDefaultPosition, wxSize(40, -1), wxTE_CENTRE );
    itemFlexGridSizer88->Add(m_F2, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labF3 = new wxStaticText( m_RICpanel, wxID_ANY, _("F3"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer88->Add(m_labF3, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 5);

    m_F3 = new wxTextCtrl( m_RICpanel, wxID_ANY, _("2"), wxDefaultPosition, wxSize(40, -1), wxTE_CENTRE );
    itemFlexGridSizer88->Add(m_F3, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labF4 = new wxStaticText( m_RICpanel, wxID_ANY, _("F4"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer88->Add(m_labF4, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 5);

    m_F4 = new wxTextCtrl( m_RICpanel, wxID_ANY, _("3"), wxDefaultPosition, wxSize(40, -1), wxTE_CENTRE );
    itemFlexGridSizer88->Add(m_F4, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labF5 = new wxStaticText( m_RICpanel, wxID_ANY, _("F5"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer88->Add(m_labF5, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 5);

    m_F5 = new wxTextCtrl( m_RICpanel, wxID_ANY, _("-1"), wxDefaultPosition, wxSize(40, -1), 0 );
    itemFlexGridSizer88->Add(m_F5, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labF6 = new wxStaticText( m_RICpanel, wxID_ANY, _("F6"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer88->Add(m_labF6, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 5);

    m_F6 = new wxTextCtrl( m_RICpanel, wxID_ANY, _("-1"), wxDefaultPosition, wxSize(40, -1), 0 );
    itemFlexGridSizer88->Add(m_F6, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labF7 = new wxStaticText( m_RICpanel, wxID_ANY, _("F7"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer88->Add(m_labF7, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 5);

    m_F7 = new wxTextCtrl( m_RICpanel, wxID_ANY, _("-1"), wxDefaultPosition, wxSize(40, -1), 0 );
    itemFlexGridSizer88->Add(m_F7, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labF8 = new wxStaticText( m_RICpanel, wxID_ANY, _("F8"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer88->Add(m_labF8, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 5);

    m_F8 = new wxTextCtrl( m_RICpanel, wxID_ANY, _("-1"), wxDefaultPosition, wxSize(40, -1), 0 );
    itemFlexGridSizer88->Add(m_F8, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labF9 = new wxStaticText( m_RICpanel, wxID_ANY, _("F9"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer88->Add(m_labF9, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 5);

    m_F9 = new wxTextCtrl( m_RICpanel, wxID_ANY, _("-1"), wxDefaultPosition, wxSize(40, -1), 0 );
    itemFlexGridSizer88->Add(m_F9, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labF10 = new wxStaticText( m_RICpanel, wxID_ANY, _("F10"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer88->Add(m_labF10, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 5);

    m_F10 = new wxTextCtrl( m_RICpanel, wxID_ANY, _("-1"), wxDefaultPosition, wxSize(40, -1), 0 );
    itemFlexGridSizer88->Add(m_F10, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labF11 = new wxStaticText( m_RICpanel, wxID_ANY, _("F11"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer88->Add(m_labF11, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 5);

    m_F11 = new wxTextCtrl( m_RICpanel, wxID_ANY, _("-1"), wxDefaultPosition, wxSize(40, -1), 0 );
    itemFlexGridSizer88->Add(m_F11, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labF12 = new wxStaticText( m_RICpanel, wxID_ANY, _("F12"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer88->Add(m_labF12, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    m_F12 = new wxTextCtrl( m_RICpanel, wxID_ANY, _("-1"), wxDefaultPosition, wxSize(40, -1), 0 );
    itemFlexGridSizer88->Add(m_F12, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxStaticBox* itemStaticBoxSizer115Static = new wxStaticBox(m_RICpanel, wxID_ANY, _("Power"));
    m_PowerBox = new wxStaticBoxSizer(itemStaticBoxSizer115Static, wxVERTICAL);
    itemBoxSizer73->Add(m_PowerBox, 0, wxGROW, 5);
    wxFlexGridSizer* itemFlexGridSizer116 = new wxFlexGridSizer(2, 4, 0, 0);
    m_PowerBox->Add(itemFlexGridSizer116, 0, wxGROW, 5);
    m_labPowerOn = new wxStaticText( m_RICpanel, wxID_ANY, _("on"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer116->Add(m_labPowerOn, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    m_PowerOn = new wxTextCtrl( m_RICpanel, wxID_ANY, _("8"), wxDefaultPosition, wxSize(40, -1), wxTE_CENTRE );
    itemFlexGridSizer116->Add(m_PowerOn, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labPowerOff = new wxStaticText( m_RICpanel, wxID_ANY, _("off"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer116->Add(m_labPowerOff, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    m_PowerOff = new wxTextCtrl( m_RICpanel, wxID_ANY, _("7"), wxDefaultPosition, wxSize(40, -1), wxTE_CENTRE );
    itemFlexGridSizer116->Add(m_PowerOff, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Notebook->AddPage(m_RICpanel, _("RIC"));

    m_MICpanel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer122 = new wxBoxSizer(wxVERTICAL);
    m_MICpanel->SetSizer(itemBoxSizer122);

    wxFlexGridSizer* itemFlexGridSizer123 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer122->Add(itemFlexGridSizer123, 0, wxGROW|wxALL, 5);
    m_labMICStep = new wxStaticText( m_MICpanel, wxID_ANY, _("step"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer123->Add(m_labMICStep, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_MICStep = new wxSpinCtrl( m_MICpanel, wxID_ANY, _T("1"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 1, 9, 1 );
    itemFlexGridSizer123->Add(m_MICStep, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_MICStopOnExit = new wxCheckBox( m_MICpanel, wxID_ANY, _("stop on exit"), wxDefaultPosition, wxDefaultSize, 0 );
    m_MICStopOnExit->SetValue(true);
    itemBoxSizer122->Add(m_MICStopOnExit, 0, wxALIGN_LEFT|wxALL, 5);

    m_Notebook->AddPage(m_MICpanel, _("MIC"));

    itemBoxSizer2->Add(m_Notebook, 1, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer127 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer127, 0, wxALIGN_RIGHT|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer127->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer127->AddButton(m_Cancel);

    itemStdDialogButtonSizer127->Realize();

////@end RocguiIniDialog content construction
}

/*!
 * Should we show tooltips?
 */

bool RocguiIniDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap RocguiIniDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin RocguiIniDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end RocguiIniDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon RocguiIniDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin RocguiIniDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end RocguiIniDialog icon retrieval
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void RocguiIniDialog::OnOKClick( wxCommandEvent& event )
{
  evaluate();
  EndModal( wxID_OK );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void RocguiIniDialog::OnCANCELClick( wxCommandEvent& event )
{
  EndModal( 0 );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_ANY
 */

void RocguiIniDialog::OnSvgPath2Click( wxCommandEvent& event )
{
  wxDirDialog* dlg = new wxDirDialog( this );
  dlg->SetPath(m_SVGPath2->GetValue());
  if( dlg->ShowModal() == wxID_OK ) {
    m_SVGPath2->SetValue( dlg->GetPath() );
  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_IMAGEPATH_BUTTON
 */

void RocguiIniDialog::OnUpdatespathButtonClick( wxCommandEvent& event )
{
  wxDirDialog* dlg = new wxDirDialog( this );
  dlg->SetPath(m_UpdatesPath->GetValue());
  if( dlg->ShowModal() == wxID_OK ) {
    m_UpdatesPath->SetValue( dlg->GetPath() );
  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_IMAGEPATH_BUTTON
 */

void RocguiIniDialog::OnImagepathButtonClick( wxCommandEvent& event )
{
  wxDirDialog* dlg = new wxDirDialog( this );
  dlg->SetPath(m_ImagePath->GetValue());
  if( dlg->ShowModal() == wxID_OK ) {
    m_ImagePath->SetValue( dlg->GetPath() );
  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SVGPATH_BUTTON
 */

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SVGPATH_BUTTON
 */

void RocguiIniDialog::OnSvgPath5Click( wxCommandEvent& event )
{
  wxDirDialog* dlg = new wxDirDialog( this );
  dlg->SetPath(m_SVGPath5->GetValue());
  if( dlg->ShowModal() == wxID_OK ) {
    m_SVGPath5->SetValue( dlg->GetPath() );
  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_XSLPATH_BUTTON
 */

void RocguiIniDialog::OnXslpathButtonClick( wxCommandEvent& event )
{
  wxDirDialog* dlg = new wxDirDialog( this );
  dlg->SetPath(m_XSLPath->GetValue());
  if( dlg->ShowModal() == wxID_OK ) {
    m_XSLPath->SetValue( dlg->GetPath() );
  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SVGPATH3_BUTTON
 */

void RocguiIniDialog::OnSvgPath4Click( wxCommandEvent& event )
{
  wxDirDialog* dlg = new wxDirDialog( this );
  dlg->SetPath(m_SVGPath4->GetValue());
  if( dlg->ShowModal() == wxID_OK ) {
    m_SVGPath4->SetValue( dlg->GetPath() );
  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SVGPATH_BUTTON
 */

void RocguiIniDialog::OnSvgPath1Click( wxCommandEvent& event )
{
  wxDirDialog* dlg = new wxDirDialog( this );
  dlg->SetPath(m_SVGPath->GetValue());
  if( dlg->ShowModal() == wxID_OK ) {
    m_SVGPath->SetValue( dlg->GetPath() );
  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SVGPATH3_BUTTON
 */

void RocguiIniDialog::OnSvgPath3Click( wxCommandEvent& event )
{
  wxDirDialog* dlg = new wxDirDialog( this );
  dlg->SetPath(m_SVGPath3->GetValue());
  if( dlg->ShowModal() == wxID_OK ) {
    m_SVGPath3->SetValue( dlg->GetPath() );
  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON
 */

void RocguiIniDialog::OnWorkspaceClick( wxCommandEvent& event )
{
  wxDirDialog* dlg = new wxDirDialog( this );
  dlg->SetPath(m_Workspace->GetValue());
  if( dlg->ShowModal() == wxID_OK ) {
    m_Workspace->SetValue( dlg->GetPath() );
  }
}

