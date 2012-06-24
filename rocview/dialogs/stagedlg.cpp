/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include "stagedlg.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocview/public/guiapp.h"

#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Stage.h"
#include "rocrail/wrapper/public/StageSection.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Loc.h"

StageDlg::StageDlg( wxWindow* parent, iONode p_Props ):stagedlggen( parent )
{
  TraceOp.trc( "stagedlg", TRCLEVEL_INFO, __LINE__, 9999, "stagedlg" );
  m_TabAlign = wxGetApp().getTabAlign();
  m_Props = p_Props;
  m_Section = NULL;
  initLabels();

  m_General->GetSizer()->Layout();
  m_Sections->GetSizer()->Layout();

  m_Notebook->Fit();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  m_Notebook->Connect( wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( StageDlg::OnSetPage ), NULL, this );
  m_SetPage = 0;

  if( m_Props != NULL ) {
    initValues();
    m_SetPage = 0;
  }
  wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_STAGEBOOK );
  wxPostEvent( m_Notebook, event );

}


void StageDlg::OnSetPage(wxCommandEvent& event) {
  TraceOp.trc( "stagedlg", TRCLEVEL_INFO, __LINE__, 9999, "set page to %d", m_SetPage );
  m_Notebook->SetSelection( m_SetPage );
}


/* comparator for sorting by id: */
static int __sortStr(obj* _a, obj* _b)
{
    const char* a = (const char*)*_a;
    const char* b = (const char*)*_b;
    return strcmp( a, b );
}

void StageDlg::initLabels() {
  SetTitle(wxGetApp().getMsg( "stagingblock" ));
  m_Notebook->SetPageText( 0, wxGetApp().getMsg( "general" ) );
  m_Notebook->SetPageText( 1, wxGetApp().getMsg( "sections" ) );

  // General
  m_labID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_labDescription->SetLabel( wxGetApp().getMsg( "description" ) );
  m_labSectionLength->SetLabel( wxGetApp().getMsg( "section" ) + _T(" ") + wxGetApp().getMsg( "length" ) );
  m_labTrainGap->SetLabel( wxGetApp().getMsg( "train" ) + _T(" ") + wxGetApp().getMsg( "gap" ) );
  m_labEnterSensor->SetLabel( _T("Enter ") + wxGetApp().getMsg( "sensor" ) );

  // Sections
  m_labSectionID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_labSectionSensor->SetLabel( wxGetApp().getMsg( "sensor" ) );
  m_AddSection->SetLabel( wxGetApp().getMsg( "add" ) );
  m_ModifySection->SetLabel( wxGetApp().getMsg( "modify" ) );
  m_DeleteSection->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_SetLoco->SetLabel( wxGetApp().getMsg( "setloco" ) );
  m_FreeSection->SetLabel( wxGetApp().getMsg( "free" ) );

  m_EnterSensor->Append( _T("") );
  m_SectionSensor->Append( _T("") );

  iONode model = wxGetApp().getModel();
  iOList list = ListOp.inst();

  if( model != NULL ) {
    iONode fblist = wPlan.getfblist( model );
    if( fblist != NULL ) {
      int cnt = NodeOp.getChildCnt( fblist );
      for( int i = 0; i < cnt; i++ ) {
        iONode fb = NodeOp.getChild( fblist, i );
        ListOp.add(list, (obj)wFeedback.getid( fb ));
      }
    }

    ListOp.sort(list, &__sortStr);
    int cnt = ListOp.size( list );
    for( int i = 0; i < cnt; i++ ) {
      const char* id = (const char*)ListOp.get( list, i );
      m_EnterSensor->Append( wxString(id,wxConvUTF8) );
      m_SectionSensor->Append( wxString(id,wxConvUTF8) );
    }

  }

  ListOp.base.del(list);


  // init loco combo
  m_SectionLocoId->Append( _T("") );

  list = ListOp.inst();

  if( model != NULL ) {
    iONode lclist = wPlan.getlclist( model );
    if( lclist != NULL ) {
      int cnt = NodeOp.getChildCnt( lclist );
      for( int i = 0; i < cnt; i++ ) {
        iONode lc = NodeOp.getChild( lclist, i );
        ListOp.add(list, (obj)wLoc.getid( lc ));
      }
    }

    ListOp.sort(list, &__sortStr);
    int cnt = ListOp.size( list );
    for( int i = 0; i < cnt; i++ ) {
      const char* id = (const char*)ListOp.get( list, i );
      m_SectionLocoId->Append( wxString(id,wxConvUTF8) );
    }

  }

  ListOp.base.del(list);



  // Buttons
  m_stdButtonOK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_stdButtonCancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
}

bool StageDlg::evaluate() {
  if( m_ID->GetValue().Len() == 0 ) {
    wxMessageDialog( this, wxGetApp().getMsg("invalidid"), _T("Rocrail"), wxOK | wxICON_ERROR ).ShowModal();
    m_ID->SetValue( wxString(wStage.getid( m_Props ),wxConvUTF8) );
    return false;
  }
  // evaluate General
  wItem.setprev_id( m_Props, wItem.getid(m_Props) );
  wStage.setid( m_Props, m_ID->GetValue().mb_str(wxConvUTF8) );
  wStage.setdesc( m_Props, m_Description->GetValue().mb_str(wxConvUTF8) );
  wStage.setslen( m_Props, m_SectionLength->GetValue() );
  wStage.setgap( m_Props, m_TrainGap->GetValue() );
  wStage.setfbenterid( m_Props, m_EnterSensor->GetStringSelection().mb_str(wxConvUTF8) );
  return true;
}

void StageDlg::initSections() {
  // init Sections
  m_SectionList->Clear();
  iONode section = wStage.getsection(m_Props);
  while( section != NULL ) {
    const char* id = wStageSection.getid(section);
    const char* lcid = wStageSection.getlcid(section);
    if( lcid != NULL && StrOp.len(lcid) > 0 )
      m_SectionList->Append( wxString(id,wxConvUTF8) + _T(" (") + wxString(lcid,wxConvUTF8) + _T(")"), section );
    else
      m_SectionList->Append( wxString(id,wxConvUTF8) + _T(" (") + wxString("free",wxConvUTF8) + _T(")"), section );
    section = wStage.nextsection(m_Props, section);
  }
  m_ModifySection->Enable(false);
  m_DeleteSection->Enable(false);
  m_SetLoco->Enable(false);
  m_FreeSection->Enable(false);

}

void StageDlg::initValues() {
  if( m_Props == NULL ) {
    TraceOp.trc( "stagedlg", TRCLEVEL_DEBUG, __LINE__, 9999, "no stage selected" );
    return;
  }

  char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("stagingblock").mb_str(wxConvUTF8), wStage.getid( m_Props ) );
  SetTitle( wxString(title,wxConvUTF8) );
  StrOp.free( title );

  TraceOp.trc( "stagedlg", TRCLEVEL_INFO, __LINE__, 9999, "initValues for stage [%s]", wStage.getid( m_Props ) );

  // init General
  m_ID->SetValue( wxString(wStage.getid( m_Props ),wxConvUTF8) );
  m_Description->SetValue( wxString(wStage.getdesc( m_Props ),wxConvUTF8) );
  m_SectionLength->SetValue( wStage.getslen( m_Props ) );
  m_TrainGap->SetValue( wStage.getgap( m_Props ) );
  m_EnterSensor->SetStringSelection( wStage.getfbenterid( m_Props ) == NULL ?
      _T(""):wxString(wStage.getfbenterid( m_Props ),wxConvUTF8) );

  initSections();

}


void StageDlg::OnSectionList( wxCommandEvent& event )
{
  if( m_SectionList->GetSelection() != wxNOT_FOUND ) {
    m_Section = (iONode)m_SectionList->GetClientData(m_SectionList->GetSelection());
    if( m_Section != NULL ) {
      m_SectionID->SetValue( wxString(wStageSection.getid( m_Section ),wxConvUTF8) );
      m_SectionSensor->SetStringSelection( wStageSection.getfbid( m_Section ) == NULL ?
          _T(""):wxString(wStageSection.getfbid( m_Section ),wxConvUTF8) );
      m_SectionLocoId->SetStringSelection( wStageSection.getlcid( m_Section ) == NULL ?
          _T(""):wxString(wStageSection.getlcid( m_Section ),wxConvUTF8) );
      m_ModifySection->Enable(true);
      m_DeleteSection->Enable(true);
      m_SetLoco->Enable(true);
      m_FreeSection->Enable(true);
    }
    else
      TraceOp.trc( "stagedlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );
  }
}

void StageDlg::OnFreeSection( wxCommandEvent& event ) {
  m_SectionLocoId->SetSelection(0);
  OnSetLoco(event);
}

void StageDlg::OnSetLoco( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;
  TraceOp.trc( "stagedlg", TRCLEVEL_INFO, __LINE__, 9999, "free section" );

  iONode cmd = NodeOp.inst( wStage.name(), NULL, ELEMENT_NODE );
  wStage.setid( cmd, wStage.getid( m_Props ) );
  wStage.setlocid( cmd, m_SectionLocoId->GetStringSelection().mb_str(wxConvUTF8) );
  wStage.setsecid( cmd, m_SectionID->GetValue().mb_str(wxConvUTF8) );
  wStage.setcmd( cmd, wBlock.loc );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
  OnSectionModify(event);
}

void StageDlg::OnSectionAdd( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;
  TraceOp.trc( "stagedlg", TRCLEVEL_INFO, __LINE__, 9999, "add node..." );

  iONode node = NodeOp.inst(wStageSection.name(), m_Props, ELEMENT_NODE);
  wStageSection.setid( node, m_SectionID->GetValue().mb_str(wxConvUTF8) );
  wStageSection.setfbid( node, m_SectionSensor->GetStringSelection().mb_str(wxConvUTF8) );
  NodeOp.addChild( m_Props, node );
  initSections();
}

void StageDlg::OnSectionModify( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;
  TraceOp.trc( "stagedlg", TRCLEVEL_INFO, __LINE__, 9999, "modify node..." );

  if( m_SectionList->GetSelection() != wxNOT_FOUND ) {
    iONode node = (iONode)m_SectionList->GetClientData(m_SectionList->GetSelection());
    if( node != NULL ) {
      wStageSection.setid( node, m_SectionID->GetValue().mb_str(wxConvUTF8) );
      wStageSection.setfbid( node, m_SectionSensor->GetStringSelection().mb_str(wxConvUTF8) );
      wStageSection.setlcid( node, m_SectionLocoId->GetStringSelection().mb_str(wxConvUTF8) );
      initSections();
    }
    else
      TraceOp.trc( "stagedlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );
  }
}

void StageDlg::OnSectionDelete( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;
  TraceOp.trc( "stagedlg", TRCLEVEL_INFO, __LINE__, 9999, "delete node..." );

  if( m_SectionList->GetSelection() != wxNOT_FOUND ) {
    iONode node = (iONode)m_SectionList->GetClientData(m_SectionList->GetSelection());
    if( node != NULL ) {
      NodeOp.removeChild( m_Props, node );
      initSections();
    }
    else
      TraceOp.trc( "stagedlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );
  }
}

void StageDlg::OnOK( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;
  if( !evaluate() )
    return;
  if( !wxGetApp().isStayOffline() ) {
    /* Notify RocRail. */
    iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.modify );
    NodeOp.addChild( cmd, (iONode)m_Props->base.clone( m_Props ) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
  else {
    wxGetApp().setLocalModelModified(true);
  }
  EndModal( wxID_OK );
}
void StageDlg::OnCancel( wxCommandEvent& event )
{
  EndModal( 0 );
}
