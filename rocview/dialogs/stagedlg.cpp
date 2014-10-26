/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

 


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
#include "rocrail/wrapper/public/StageList.h"
#include "rocrail/wrapper/public/StageSection.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Signal.h"

#include "actionsctrldlg.h"

static bool m_SortOrder;

StageDlg::StageDlg( wxWindow* parent, iONode p_Props ):stagedlggen( parent )
{
  TraceOp.trc( "stagedlg", TRCLEVEL_INFO, __LINE__, 9999, "stagedlg" );
  m_TabAlign = wxGetApp().getTabAlign();
  m_Props = p_Props;
  m_Section = NULL;
  m_SortCol = 0;
  m_SortOrder = true;
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
    m_SetPage = wxGetApp().getFrame()->isAutoMode()?2:1;
  }
  initIndex();

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
  m_Notebook->SetPageText( 0, wxGetApp().getMsg( "index" ) );
  m_Notebook->SetPageText( 1, wxGetApp().getMsg( "general" ) );
  m_Notebook->SetPageText( 2, wxGetApp().getMsg( "sections" ) );
  m_Notebook->SetPageText( 3, wxGetApp().getMsg( "details" ) );

  if( wxGetApp().getFrame()->isAutoMode() ) {
    m_General->Enable(false);
    m_Details->Enable(false);
    m_AddSection->Enable(false);
    m_DeleteSection->Enable(false);
    m_SectionSensor->Enable(false);
  }

  // Index
  m_StageList->InsertColumn(0, wxGetApp().getMsg( "id" ), wxLIST_FORMAT_LEFT );
  m_StageList->InsertColumn(1, wxGetApp().getMsg( "description" ), wxLIST_FORMAT_LEFT );
  m_StageList->InsertColumn(2, wxGetApp().getMsg( "sections" ), wxLIST_FORMAT_LEFT );
  m_StageList->InsertColumn(3, wxGetApp().getMsg( "length" ), wxLIST_FORMAT_LEFT );
  m_IndexNew->SetLabel( wxGetApp().getMsg( "new" ) );
  m_IndexDelete->SetLabel( wxGetApp().getMsg( "delete" ) );

  // General
  m_labID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_labDescription->SetLabel( wxGetApp().getMsg( "description" ) );
  m_labSectionLength->SetLabel( wxGetApp().getMsg( "sectionlength" ) );
  m_labTrainGap->SetLabel( wxGetApp().getMsg( "traingap" ) );
  m_labEnterSensor->SetLabel( wxGetApp().getMsg("entersensor") );
  m_labEnterSignal->SetLabel( wxGetApp().getMsg("entersignal") );
  m_labExitSignal->SetLabel( wxGetApp().getMsg("exitsignal") );
  m_labRandomRate->SetLabel( wxGetApp().getMsg( "randomrate" ) );

  // Sections
  m_labSectionID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_labSectionSensor->SetLabel( wxGetApp().getMsg( "sensor" ) );
  m_labOccSensor->SetLabel( wxGetApp().getMsg( "occupancy" ) );
  m_AddSection->SetLabel( wxGetApp().getMsg( "add" ) );
  m_ModifySection->SetLabel( wxGetApp().getMsg( "modify" ) );
  m_DeleteSection->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_SetLoco->SetLabel( wxGetApp().getMsg( "setloco" ) );
  m_FreeSection->SetLabel( wxGetApp().getMsg( "free" ) );
  m_FreeAll->SetLabel( wxGetApp().getMsg( "freeall" ) );
  m_labOwnSectionLength->SetLabel( wxGetApp().getMsg( "length" ) );

  m_EnterSensor->Append( _T("") );
  m_SectionSensor->Append( _T("") );
  m_OccSensor->Append( _T("") );

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
      m_OccSensor->Append( wxString(id,wxConvUTF8) );
    }

  }



  m_EnterSignal->Append( _T("") );
  m_ExitSignal->Append( _T("") );

  ListOp.clear(list);

  if( model != NULL ) {
    iONode sglist = wPlan.getsglist( model );
    if( sglist != NULL ) {
      int cnt = NodeOp.getChildCnt( sglist );
      for( int i = 0; i < cnt; i++ ) {
        iONode sg = NodeOp.getChild( sglist, i );
        const char* id = wSignal.getid( sg );
        if( id != NULL ) {
          ListOp.add(list, (obj)id);
    }
    }
      ListOp.sort(list, &__sortStr);
      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        const char* id = (const char*)ListOp.get( list, i );
          m_EnterSignal->Append( wxString(id,wxConvUTF8) );
          m_ExitSignal->Append( wxString(id,wxConvUTF8) );
      }
    }
  }
  /* clean up the temp. list */
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

  // Details
  m_labDepartDelay->SetLabel( wxGetApp().getMsg( "departdelay" ) );
  m_labMinOcc->SetLabel( wxGetApp().getMsg( "minocc" ) );
  m_WaitBox->GetStaticBox()->SetLabel( wxGetApp().getMsg( "wait" ) );
  m_WaitType->SetLabel( wxGetApp().getMsg( "type" ) );
  m_WaitType->SetString( 0, wxGetApp().getMsg( "random" ) );
  m_WaitType->SetString( 1, wxGetApp().getMsg( "fixed" ) );
  m_WaitType->SetString( 2, wxGetApp().getMsg( "locomotive" ) );
  m_WaitType->SetString( 3, wxGetApp().getMsg( "none" ) );
  m_labRandomMin->SetLabel( wxGetApp().getMsg( "randommin" ) );
  m_labRandomMax->SetLabel( wxGetApp().getMsg( "randommax" ) );
  m_labFixed->SetLabel( wxGetApp().getMsg( "fixed" ) );
  m_SuitsWell->SetLabel( wxGetApp().getMsg( "bestchoice" ) );
  m_InAtLen->SetLabel( wxGetApp().getMsg( "inatlen" ) );
  m_UseWD->SetLabel( wxGetApp().getMsg( "watchdog" ) );

  m_SpeedBox->GetStaticBox()->SetLabel( wxGetApp().getMsg( "speed" ) );
  m_ArriveSpeed->SetLabel( wxGetApp().getMsg( "arrive" ) );
  m_ArriveSpeed->SetString( 0, wxGetApp().getMsg( "min" ) );
  m_ArriveSpeed->SetString( 1, _T("%") );

  m_ExitSpeed->SetLabel( wxGetApp().getMsg( "departure" ) );
  m_ExitSpeed->SetString( 0, wxGetApp().getMsg( "min" ) );
  m_ExitSpeed->SetString( 1, wxGetApp().getMsg( "mid" ) );
  m_ExitSpeed->SetString( 2, wxGetApp().getMsg( "cruise" ) );
  m_ExitSpeed->SetString( 3, wxGetApp().getMsg( "max" ) );

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
  wStage.setentersignal( m_Props, m_EnterSignal->GetStringSelection().mb_str(wxConvUTF8) );
  wStage.setexitsignal( m_Props, m_ExitSignal->GetStringSelection().mb_str(wxConvUTF8) );
  wStage.setrandomrate( m_Props, m_RandomRate->GetValue() );

  // Details
  wStage.setdepartdelay( m_Props, m_DepartDelay->GetValue() );
  wStage.setminocc( m_Props, m_MinOcc->GetValue() );


  if( m_WaitType->GetSelection() == 0 )
    wStage.setwaitmode( m_Props, wBlock.wait_random );
  else if( m_WaitType->GetSelection() == 1 )
    wStage.setwaitmode( m_Props, wBlock.wait_fixed );
  else if( m_WaitType->GetSelection() == 1 )
    wStage.setwaitmode( m_Props, wBlock.wait_loc );
  else
    wStage.setwaitmode( m_Props, wBlock.wait_none );

  wStage.setminwaittime( m_Props, m_RandomMin->GetValue() );
  wStage.setmaxwaittime( m_Props, m_RandomMax->GetValue() );
  wStage.setwaittime( m_Props, m_Fixed->GetValue() );

  if( m_ExitSpeed->GetSelection() == 0 )
    wStage.setexitspeed( m_Props, wBlock.min );
  else if( m_ExitSpeed->GetSelection() == 1 )
    wStage.setexitspeed( m_Props, wBlock.mid );
  else if( m_ExitSpeed->GetSelection() == 2 )
    wStage.setexitspeed( m_Props, wBlock.cruise );
  else if( m_ExitSpeed->GetSelection() == 3 )
    wStage.setexitspeed( m_Props, wBlock.max );
  else if( m_ExitSpeed->GetSelection() == 4 )
    wStage.setexitspeed( m_Props, wBlock.percent );

  if( m_ArriveSpeed->GetSelection() == 0 )
    wStage.setstopspeed( m_Props, wBlock.min );
  else if( m_ArriveSpeed->GetSelection() == 1 ) {
    wStage.setstopspeed( m_Props, wBlock.percent );
  }
  wStage.setspeedpercent( m_Props, m_ArriveSpeedPercent->GetValue() );
  wStage.setexitspeedpercent( m_Props, m_DepartSpeedPercent->GetValue() );
  wStage.setsuitswell( m_Props, m_SuitsWell->IsChecked() ? True:False );
  wStage.setinatlen( m_Props, m_InAtLen->IsChecked() ? True:False );
  wStage.setusewd( m_Props, m_UseWD->IsChecked() ? True:False );

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

int StageDlg::getLength(iONode stage, int* nrsections) {
  // init Sections
  int cnt = 0;
  int length = 0;
  iONode section = wStage.getsection(stage);
  while( section != NULL ) {
    int sectionlength = wStageSection.getlen(section);
    if( sectionlength > 0 )
      length += sectionlength;
    else
      length += wStage.getslen(stage);
    section = wStage.nextsection(stage, section);
    cnt++;
  }
  *nrsections = cnt;
  return length;
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
  m_EnterSignal->SetStringSelection( wStage.getentersignal( m_Props ) == NULL ?
      _T(""):wxString(wStage.getentersignal( m_Props ),wxConvUTF8) );
  m_ExitSignal->SetStringSelection( wStage.getexitsignal( m_Props ) == NULL ?
      _T(""):wxString(wStage.getexitsignal( m_Props ),wxConvUTF8) );
  m_RandomRate->SetValue( wStage.getrandomrate( m_Props ) );

  // Details
  m_DepartDelay->SetValue( wStage.getdepartdelay( m_Props ) );
  m_MinOcc->SetValue( wStage.getminocc( m_Props ) );

  int wait = 0;
  if( StrOp.equals( wBlock.wait_random, wStage.getwaitmode( m_Props ) ) )
    wait = 0;
  else if( StrOp.equals( wBlock.wait_fixed, wStage.getwaitmode( m_Props ) ) )
    wait = 1;
  else if( StrOp.equals( wBlock.wait_loc, wStage.getwaitmode( m_Props ) ) )
    wait = 2;
  else if( StrOp.equals( wBlock.wait_none, wStage.getwaitmode( m_Props ) ) )
    wait = 3;
  m_WaitType->SetSelection( wait );

  m_RandomMin->SetValue( wStage.getminwaittime( m_Props ) );
  m_RandomMax->SetValue( wStage.getmaxwaittime( m_Props ) );
  m_Fixed->SetValue( wStage.getwaittime( m_Props ) );

  int speed = 0;
  if( StrOp.equals( wBlock.min, wStage.getexitspeed( m_Props ) ) )
    speed = 0;
  else if( StrOp.equals( wBlock.mid, wStage.getexitspeed( m_Props ) ) )
    speed = 1;
  else if( StrOp.equals( wBlock.cruise, wStage.getexitspeed( m_Props ) ) )
    speed = 2;
  else if( StrOp.equals( wBlock.max, wStage.getexitspeed( m_Props ) ) )
    speed = 3;
  else if( StrOp.equals( wBlock.percent, wStage.getexitspeed( m_Props ) ) )
    speed = 4;
  m_ExitSpeed->SetSelection( speed );
  m_DepartSpeedPercent->SetValue(wStage.getexitspeedpercent( m_Props ));

  speed = 0;
  if( StrOp.equals( wBlock.min, wStage.getstopspeed( m_Props ) ) )
    speed = 0;
  else if( StrOp.equals( wBlock.percent, wStage.getstopspeed( m_Props ) ) )
    speed = 1;
  m_ArriveSpeed->SetSelection( speed );
  m_ArriveSpeedPercent->SetValue( wStage.getspeedpercent(m_Props) );

  m_SuitsWell->SetValue( wStage.issuitswell( m_Props ) ? true:false );
  m_InAtLen->SetValue( wStage.isinatlen( m_Props ) ? true:false );
  m_UseWD->SetValue( wStage.isusewd( m_Props ) ? true:false );

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
      m_OccSensor->SetStringSelection( wStageSection.getfbidocc( m_Section ) == NULL ?
          _T(""):wxString(wStageSection.getfbidocc( m_Section ),wxConvUTF8) );
      m_SectionLocoId->SetStringSelection( wStageSection.getlcid( m_Section ) == NULL ?
          _T(""):wxString(wStageSection.getlcid( m_Section ),wxConvUTF8) );
      m_OwnSectionLength->SetValue( wStageSection.getlen( m_Section ) );
      m_ModifySection->Enable(true);
      m_DeleteSection->Enable(!wxGetApp().getFrame()->isAutoMode());
      m_SetLoco->Enable(true);
      m_FreeSection->Enable(true);
    }
    else
      TraceOp.trc( "stagedlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );
  }
}


void StageDlg::OnFreeAll( wxCommandEvent& event ) {
  int action = wxMessageDialog( this, wxGetApp().getMsg("freeallwarning"), _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION ).ShowModal();
  if( action == wxID_NO )
    return;

  iONode section = wStage.getsection(m_Props);
  while( section != NULL ) {
    wStageSection.setlcid(section, NULL);
    section = wStage.nextsection(m_Props, section);
  }
  initSections();

  m_SectionLocoId->SetSelection(0);
  iONode cmd = NodeOp.inst( wStage.name(), NULL, ELEMENT_NODE );
  wStage.setid( cmd, wStage.getid( m_Props ) );
  wStage.setcmd( cmd, wBlock.loc );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
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
  wStageSection.setfbidocc( node, m_OccSensor->GetStringSelection().mb_str(wxConvUTF8) );
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
      wStageSection.setfbidocc( node, m_OccSensor->GetStringSelection().mb_str(wxConvUTF8) );
      wStageSection.setlcid( node, m_SectionLocoId->GetStringSelection().mb_str(wxConvUTF8) );
      wStageSection.setlen( node, m_OwnSectionLength->GetValue() );
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

  int action = wxMessageDialog( this, wxGetApp().getMsg("removewarning"), _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION ).ShowModal();
  if( action == wxID_NO )
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


void StageDlg::OnApply( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;
  OnSectionModify(event);
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
  initIndex();
}


void StageDlg::OnOK( wxCommandEvent& event )
{
  OnApply(event);
  EndModal( wxID_OK );
}

void StageDlg::OnCancel( wxCommandEvent& event )
{
  EndModal( 0 );
}

void StageDlg::onActions( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  ActionsCtrlDlg*  dlg = new ActionsCtrlDlg(this, m_Props, "ghost,enter,occupied,reserved,free,closed,depart,exit" );

  if( wxID_OK == dlg->ShowModal() ) {
    // TODO: inform
  }
  dlg->Destroy();
}


void StageDlg::onStageColumn( wxListEvent& event ) {
  int sortcol = event.GetColumn();
  if( sortcol == m_SortCol )
    m_SortOrder = !m_SortOrder;
  else
    m_SortOrder = true;
  m_SortCol = sortcol;
  initIndex();
}


void StageDlg::onStageList( wxListEvent& event ) {
  int index = event.GetIndex();
  m_Props = (iONode)m_StageList->GetItemData(index);
  if( m_Props != NULL )
    initValues();
  else
    TraceOp.trc( "stagedlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );
}


void StageDlg::onIndexNew( wxCommandEvent& event ) {
  int i = findID("NEW");
  if( i == wxNOT_FOUND ) {
    iONode model = wxGetApp().getModel();
    if( model != NULL ) {
      iONode sblist = wPlan.getsblist( model );
      if( sblist == NULL ) {
        sblist = NodeOp.inst( wStageList.name(), model, ELEMENT_NODE );
        NodeOp.addChild( model, sblist );
      }
      if( sblist != NULL ) {
        iONode stage = NodeOp.inst( wStage.name(), sblist, ELEMENT_NODE );
        NodeOp.addChild( sblist, stage );
        wStage.setid( stage, "NEW" );
        wStage.setx( stage, 0 );
        wStage.sety( stage, 0 );
        wStage.setz( stage, 0 );
        m_Props = stage;
        initValues();
        initIndex();
        setSelection(wStage.getid( stage ));
      }
    }
  }
}


void StageDlg::onIndexDelete( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  int action = wxMessageDialog( this, wxGetApp().getMsg("removewarning"), _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION ).ShowModal();
  if( action == wxID_NO )
    return;

  wxGetApp().pushUndoItem( (iONode)NodeOp.base.clone( m_Props ) );
  /* Notify RocRail. */
  iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
  wModelCmd.setcmd( cmd, wModelCmd.remove );
  NodeOp.addChild( cmd, (iONode)m_Props->base.clone( m_Props ) );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);

  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode sblist = wPlan.getsblist( model );
    if( sblist != NULL ) {
      NodeOp.removeChild( sblist, m_Props );
      m_Props = NULL;
    }
  }

  initIndex();
}


/* comparator for sorting by id: */
static int __sortID(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = wItem.getid( a );
    const char* idB = wItem.getid( b );
    return m_SortOrder ? strcmp( idA, idB ):strcmp( idB, idA );
}

static int __sortDesc(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = wItem.getdesc( a );
    const char* idB = wItem.getdesc( b );
    return m_SortOrder ? strcmp( idA, idB ):strcmp( idB, idA );
}

static int __sortNrSections(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    int iA = wStage.gettotalsections( a );
    int iB = wStage.gettotalsections( b );
    if( iA > iB )
      return (m_SortOrder ? 1:-1);
    if( iA < iB )
      return (m_SortOrder ? -1:1);
    return 0;
}

static int __sortLength(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    int iA = wStage.gettotallength( a );
    int iB = wStage.gettotallength( b );
    if( iA > iB )
      return (m_SortOrder ? 1:-1);
    if( iA < iB )
      return (m_SortOrder ? -1:1);
    return 0;
}


void StageDlg::initIndex() {
  TraceOp.trc( "stagedlg", TRCLEVEL_INFO, __LINE__, 9999, "initIndex" );

  SetTitle(wxGetApp().getMsg( "stagetable" ));

  m_StageList->DeleteAllItems();
  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode sblist = wPlan.getsblist( model );
    if( sblist != NULL ) {
      iOList list = ListOp.inst();
      int cnt = NodeOp.getChildCnt( sblist );
      for( int i = 0; i < cnt; i++ ) {
        iONode stage = NodeOp.getChild( sblist, i );
        const char* id = wStage.getid( stage );
        if( id != NULL ) {
          ListOp.add(list, (obj)stage);
        }
      }

      if( m_SortCol == 1 ) {
        ListOp.sort(list, &__sortDesc);
      }
      else if( m_SortCol == 2 ) {
        ListOp.sort(list, &__sortNrSections);
      }
      else if( m_SortCol == 3 ) {
        ListOp.sort(list, &__sortLength);
      }
      else {
        ListOp.sort(list, &__sortID);
      }

      int idx = 0;
      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        iONode stage = (iONode)ListOp.get( list, i );
        const char* id = wStage.getid( stage );
        m_StageList->InsertItem( idx, wxString(id,wxConvUTF8) );
        m_StageList->SetItem( idx, 1, wxString(wStage.getdesc( stage ), wxConvUTF8) );

        int nrsections = 0;
        int len = getLength(stage, &nrsections);
        m_StageList->SetItem( idx, 2, wxString::Format(wxT("%d"), nrsections) );
        m_StageList->SetItem( idx, 3, wxString::Format(wxT("%d"), len) );
        wStage.settotallength(stage, len);
        wStage.settotalsections(stage, nrsections);

        m_StageList->SetItemPtrData(idx, (wxUIntPtr)stage);
        idx++;
      }

      // resize
      for( int n = 0; n < 4; n++ ) {
        m_StageList->SetColumnWidth(n, wxLIST_AUTOSIZE_USEHEADER);
        int autoheadersize = m_StageList->GetColumnWidth(n);
        m_StageList->SetColumnWidth(n, wxLIST_AUTOSIZE);
        int autosize = m_StageList->GetColumnWidth(n);
        if(autoheadersize > autosize )
          m_StageList->SetColumnWidth(n, wxLIST_AUTOSIZE_USEHEADER);
      }

      /* clean up the temp. list */
      ListOp.base.del(list);

      if( m_Props != NULL ) {
        setSelection(wStage.getid( m_Props ));
      }
      else if(m_StageList->GetItemCount() > 0 ) {
        TraceOp.trc( "stagedlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection" );
        m_StageList->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        m_Props = (iONode)m_StageList->GetItemData(0);
      }

    }
  }
}


int StageDlg::findID( const char* ID ) {
  int size = m_StageList->GetItemCount();
  for( int index = 0; index < size; index++ ) {
    iONode node = (iONode)m_StageList->GetItemData(index);
    if( StrOp.equals( ID, wStage.getid(node) ) ) {
      return index;
    }
  }
  return wxNOT_FOUND;
}


void StageDlg::setSelection(const char* ID) {
  int size = m_StageList->GetItemCount();
  for( int index = 0; index < size; index++ ) {
    iONode node = (iONode)m_StageList->GetItemData(index);
    if( StrOp.equals( ID, wStage.getid(node) ) ) {
      m_StageList->SetItemState(index, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
      break;
    }
  }

}


void StageDlg::OnHelp( wxCommandEvent& event ) {
  switch( m_Notebook->GetSelection() ) {
  case 0: wxGetApp().openLink( "stageblock:stage-index" ); break;
  case 1: wxGetApp().openLink( "stageblock:stage-gen" ); break;
  case 2: wxGetApp().openLink( "stageblock:stage-sections" ); break;
  case 3: wxGetApp().openLink( "stageblock:stage-details" ); break;
  default: wxGetApp().openLink( "stage" ); break;
  }
}

