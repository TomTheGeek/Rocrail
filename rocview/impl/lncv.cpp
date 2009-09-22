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
#pragma implementation "lncv.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "rocs/public/trace.h"
#include "rocs/public/strtok.h"
#include "rocview/public/guiapp.h"

#include "rocview/public/lncv.h"

#include "rocrail/wrapper/public/Program.h"



LNCV::LNCV( wxScrolledWindow* parent, wxWindow* frame ) {

  m_Parent = parent;
  TraceOp.trc( "lncv", TRCLEVEL_INFO, __LINE__, 9999, "init" );
  init();
  TraceOp.trc( "lncv", TRCLEVEL_INFO, __LINE__, 9999, "Create" );
  Create();
  TraceOp.trc( "lncv", TRCLEVEL_INFO, __LINE__, 9999, "ready" );
  //m_ModuleDialog->Enable(false);
}

/*!
 * LNCV creator
 */

bool LNCV::Create()
{
  CreateControls();
  return true;
}

/*!
 * Member initialisation
 */

void LNCV::OnButton(wxCommandEvent& event)
{
  TraceOp.trc( "lncv", TRCLEVEL_DEBUG, __LINE__, 9999, "button event: %d", event.GetId() );

  if ( event.GetEventObject() == m_ModuleDialog ) {
    int modid = atoi( m_ModuleKey->GetValue().mb_str(wxConvUTF8) );
    if(modid == 6335 || modid == 6334) {
    }
  }
  else if ( event.GetEventObject() == m_ModuleDetect ) {
    /* use broadcast values: modkey=0xFFFF cv=0x0000 value=0xFFFF */
    int lnType = (m_useLNCV->IsChecked()?wProgram.lntype_cv:wProgram.lntype_sv);
    iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setlntype( cmd, lnType );
    wProgram.setcmd( cmd, wProgram.lncvget );
    wProgram.setvalue( cmd, 0xFFFF );
    wProgram.setcv( cmd, 0 );
    wProgram.setmodid( cmd, 0xFFFF );
    TraceOp.trc( "lncv", TRCLEVEL_INFO, __LINE__, 9999, "broadcast lncv program command" );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
  else if ( event.GetEventObject() == m_Start ) {
    int lnType = (m_useLNCV->IsChecked()?wProgram.lntype_cv:wProgram.lntype_sv);
    iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setlncvcmd( cmd, wProgram.lncvstart );
    wProgram.setlntype( cmd, lnType );
    wProgram.setcmd( cmd, wProgram.lncvget );
    wProgram.setaddr( cmd, atoi( m_Address->GetValue().mb_str(wxConvUTF8) ) );
    wProgram.setcv( cmd, atoi( m_CVfrom->GetValue().mb_str(wxConvUTF8) ) );
    wProgram.setmodid( cmd, atoi( m_ModuleKey->GetValue().mb_str(wxConvUTF8) ) );
    wProgram.setvalue( cmd, atoi( m_Value->GetValue().mb_str(wxConvUTF8) ) );
    TraceOp.trc( "lncv", TRCLEVEL_INFO, __LINE__, 9999, "get lncv program command for %d,%d,%d",
        wProgram.getmodid(cmd), wProgram.getaddr(cmd), wProgram.getcv(cmd) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
  else if ( event.GetEventObject() == m_Stop ) {
    int lnType = (m_useLNCV->IsChecked()?wProgram.lntype_cv:wProgram.lntype_sv);
    iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setlncvcmd( cmd, wProgram.lncvend );
    wProgram.setlntype( cmd, lnType );
    wProgram.setcmd( cmd, wProgram.lncvget );
    wProgram.setaddr( cmd, atoi( m_Address->GetValue().mb_str(wxConvUTF8) ) );
    wProgram.setcv( cmd, atoi( m_CVfrom->GetValue().mb_str(wxConvUTF8) ) );
    wProgram.setmodid( cmd, atoi( m_ModuleKey->GetValue().mb_str(wxConvUTF8) ) );
    wProgram.setvalue( cmd, atoi( m_Value->GetValue().mb_str(wxConvUTF8) ) );
    TraceOp.trc( "lncv", TRCLEVEL_INFO, __LINE__, 9999, "get lncv program command for %d,%d,%d",
        wProgram.getmodid(cmd), wProgram.getaddr(cmd), wProgram.getcv(cmd) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
  else if ( event.GetEventObject() == m_Get ) {
    int lnType = (m_useLNCV->IsChecked()?wProgram.lntype_cv:wProgram.lntype_sv);
    iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setlntype( cmd, lnType );
    wProgram.setcmd( cmd, wProgram.lncvget );
    wProgram.setaddr( cmd, atoi( m_Address->GetValue().mb_str(wxConvUTF8) ) );
    wProgram.setcv( cmd, atoi( m_CVfrom->GetValue().mb_str(wxConvUTF8) ) );
    wProgram.setmodid( cmd, atoi( m_ModuleKey->GetValue().mb_str(wxConvUTF8) ) );
    wProgram.setvalue( cmd, atoi( m_Value->GetValue().mb_str(wxConvUTF8) ) );
    TraceOp.trc( "lncv", TRCLEVEL_INFO, __LINE__, 9999, "get lncv program command for %d,%d,%d",
        wProgram.getmodid(cmd), wProgram.getaddr(cmd), wProgram.getcv(cmd) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
  else if ( event.GetEventObject() == m_Set ) {
    int lnType = (m_useLNCV->IsChecked()?wProgram.lntype_cv:wProgram.lntype_sv);
    iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setlntype( cmd, lnType );
    wProgram.setcmd( cmd, wProgram.lncvset );
    wProgram.setaddr( cmd, atoi( m_Address->GetValue().mb_str(wxConvUTF8) ) );
    wProgram.setcv( cmd, atoi( m_CVfrom->GetValue().mb_str(wxConvUTF8) ) );
    wProgram.setmodid( cmd, atoi( m_ModuleKey->GetValue().mb_str(wxConvUTF8) ) );
    wProgram.setvalue( cmd, atoi( m_Value->GetValue().mb_str(wxConvUTF8) ) );
    TraceOp.trc( "lncv", TRCLEVEL_INFO, __LINE__, 9999, "set lncv program command for %d,%d,%d,%d",
        wProgram.getmodid(cmd), wProgram.getaddr(cmd), wProgram.getcv(cmd), wProgram.getvalue(cmd) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
  else if ( event.GetEventObject() == m_useLNCV ) {
    if( m_useLNCV->IsChecked() ) {
      m_useLNCV->SetLabel( _T("CV") );
      m_ModuleDetect->Enable(true);
      m_Start->Enable(true);
      m_Stop->Enable(true);
      m_ModuleKey->Enable(true);
    }
    else {
      m_useLNCV->SetLabel( _T("SV") );
      m_ModuleDetect->Enable(false);
      m_Start->Enable(false);
      m_Stop->Enable(false);
      m_ModuleKey->Enable(false);
    }
  }
}

void LNCV::event( iONode event ) {
  int mod  = wProgram.getmodid(event);
  int addr = wProgram.getaddr(event);
  int cmd  = wProgram.getcmd(event);
  int cv   = wProgram.getcv (event);
  int val  = wProgram.getvalue(event);
  bool datarsp = false;

  if( cmd == wProgram.datarsp )
    datarsp = true;

  // update the controls with the values...
  //char* str = StrOp.fmt( "%d", val );
  //m_ModuleKey->SetValue( wxString( str,wxConvUTF8) );
  //StrOp.free(str);

  //str = StrOp.fmt( "%d", addr );
  //m_Address->SetValue( wxString( str,wxConvUTF8) );
  //StrOp.free(str);

  char* str = StrOp.fmt( "%d", cv );
  m_CVfrom->SetValue( wxString( str,wxConvUTF8) );
  StrOp.free(str);

  str = StrOp.fmt( "%d", val );
  m_Value->SetValue( wxString( str,wxConvUTF8) );
  StrOp.free(str);

}

void LNCV::init()
{
    m_ItemPanel = NULL;
    m_labModuleKey = NULL;
    m_ModuleKey = NULL;
    m_ModuleDialog = NULL;
    m_ModuleDetect = NULL;
    m_labAddress = NULL;
    m_Address = NULL;
    m_useLNCV = NULL;
    m_CVfrom = NULL;
    m_labValue = NULL;
    m_Value = NULL;
    m_Get = NULL;
    m_Set = NULL;
    m_Start = NULL;
    m_Stop = NULL;
}

/*!
 * Control creation for LNCV
 */

void LNCV::CreateControls()
{
  m_MainBox = new wxBoxSizer(wxVERTICAL);
  m_Parent->SetSizer(m_MainBox);

    m_ItemPanel = new wxPanel( m_Parent, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_MainBox->Add(m_ItemPanel, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    m_ItemPanel->SetSizer(itemBoxSizer3);

    wxStaticBox* itemStaticBoxSizer4Static = new wxStaticBox(m_ItemPanel, wxID_ANY, _("Module"));
    wxStaticBoxSizer* itemStaticBoxSizer4 = new wxStaticBoxSizer(itemStaticBoxSizer4Static, wxVERTICAL);
    itemBoxSizer3->Add(itemStaticBoxSizer4, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer4->Add(itemBoxSizer5, 0, wxALIGN_LEFT, 5);

    m_labModuleKey = new wxStaticText( m_ItemPanel, wxID_ANY, _("Key"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(m_labModuleKey, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    m_ModuleKey = new wxTextCtrl( m_ItemPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(m_ModuleKey, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ModuleDialog = new wxButton( m_ItemPanel, wxID_ANY, _("..."), wxDefaultPosition, wxSize(30, 25), 0 );
    itemBoxSizer5->Add(m_ModuleDialog, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer4->Add(itemBoxSizer9, 0, wxALIGN_LEFT, 5);

    m_labAddress = new wxStaticText( m_ItemPanel, wxID_ANY, _("Address"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(m_labAddress, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Address = new wxTextCtrl( m_ItemPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(m_Address, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ModuleDetect = new wxButton( m_ItemPanel, wxID_ANY, _("Detect"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer4->Add(m_ModuleDetect, 0, wxALIGN_LEFT|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer12 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer3->Add(itemFlexGridSizer12, 0, wxGROW|wxALL, 5);

    m_useLNCV = new wxCheckBox( m_ItemPanel, wxID_ANY, _("CV"), wxDefaultPosition, wxDefaultSize, 0 );
    m_useLNCV->SetValue(true);
    itemFlexGridSizer12->Add(m_useLNCV, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_CVfrom = new wxTextCtrl( m_ItemPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_CVfrom, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labValue = new wxStaticText( m_ItemPanel, wxID_ANY, _("Value"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_labValue, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Value = new wxTextCtrl( m_ItemPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_Value, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer17 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer17, 0, wxGROW|wxALL, 5);

    m_Get = new wxButton( m_ItemPanel, wxID_ANY, _("Get"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer17->Add(m_Get, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Set = new wxButton( m_ItemPanel, wxID_ANY, _("Set"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer17->Add(m_Set, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer18 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer18, 0, wxGROW|wxALL, 5);

    m_Start = new wxButton( m_ItemPanel, wxID_ANY, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer18->Add(m_Start, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Stop = new wxButton( m_ItemPanel, wxID_ANY, _("Stop"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer18->Add(m_Stop, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end LNCV content construction
}

