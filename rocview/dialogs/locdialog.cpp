/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

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
#pragma implementation "locdialog.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

////@begin includes
#include "wx/imaglist.h"
////@end includes

#include "locdialog.h"
#include "fundialog.h"
#include "actionsctrldlg.h"

#include "rocview/public/guiapp.h"
#include "rocview/public/base.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/LocList.h"
#include "rocrail/wrapper/public/FunDef.h"
#include "rocview/wrapper/public/Gui.h"
#include "rocrail/wrapper/public/CVByte.h"
#include "rocrail/wrapper/public/Schedule.h"


#include "rocs/public/trace.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"

#include "rocview/xpm/nopict.xpm"

////@begin XPM images
////@end XPM images

/*!
 * LocDialog type definition
 */

/*!
 * LocDialog event table definition
 */

BEGIN_EVENT_TABLE( LocDialog, wxDialog )

////@begin LocDialog event table entries
    EVT_BUTTON( ID_BITMAPBUTTON, LocDialog::OnBitmapbuttonClick )

    EVT_LIST_ITEM_SELECTED( ID_LISTCTRLINDEX_LC, LocDialog::OnListctrlindexLcSelected )
    EVT_LIST_COL_CLICK( ID_LISTCTRLINDEX_LC, LocDialog::OnListctrlindexLcColLeftClick )

    EVT_BUTTON( ID_BUTTON_LOC_NEW, LocDialog::OnButtonLocNewClick )

    EVT_BUTTON( ID_BUTTON_DELETE, LocDialog::OnButtonDeleteClick )

    EVT_BUTTON( ID_BUTTON_LOCO_DOC, LocDialog::OnButtonLocoDocClick )

    EVT_BUTTON( ID_LOC_COPY, LocDialog::OnLocCopyClick )

    EVT_BUTTON( ID_BUTTON_IMPORTLOCOS, LocDialog::OnButtonImportlocosClick )

    EVT_CHECKBOX( wxID_ANY, LocDialog::OnRestoreFxClick )

    EVT_CHECKBOX( wxID_ANY, LocDialog::OnRestoreSpeedClick )

    EVT_CHECKBOX( wxID_ANY, LocDialog::OnShowClick )

    EVT_BUTTON( ID_BUTTON_SHOWDOC, LocDialog::OnButtonShowdocClick )

    EVT_BUTTON( ID_LC_SERVICED, LocDialog::OnLcServicedClick )

    EVT_BUTTON( ID_LOC_ACTIONS, LocDialog::OnLocActionsClick )

    EVT_TEXT( ID_TEXTCTRL_SPEEDSTEPS, LocDialog::OnTextctrlSpeedstepsTextUpdated )
    EVT_TEXT_ENTER( ID_TEXTCTRL_SPEEDSTEPS, LocDialog::OnTextctrlSpeedstepsEnter )

    EVT_CHECKBOX( ID_LOC_REGULATED, LocDialog::OnLocRegulatedClick )

    EVT_BUTTON( ID_BUTTON_F0, LocDialog::OnButtonF0Click )

    EVT_BUTTON( ID_LOC_FN_GROUP_PREV, LocDialog::OnLocFnGroupPrevClick )

    EVT_BUTTON( ID_LOC_FN_GROUP, LocDialog::OnLocFnGroupClick )

    EVT_BUTTON( ID_BUTTON_F1, LocDialog::OnButtonF1Click )

    EVT_BUTTON( ID_BUTTON_F2, LocDialog::OnButtonF2Click )

    EVT_BUTTON( ID_BUTTON_F3, LocDialog::OnButtonF3Click )

    EVT_BUTTON( ID_BUTTON_F4, LocDialog::OnButtonF4Click )

    EVT_BUTTON( ID_BUTTON_LOC_CONSIST_ADD, LocDialog::OnButtonLocConsistAddClick )

    EVT_BUTTON( ID_BUTTON_LOC_CONSIST_DELETE, LocDialog::OnButtonLocConsistDeleteClick )

    EVT_GRID_CELL_LEFT_CLICK( LocDialog::OnCVCellLeftClick )

    EVT_BUTTON( ID_BUTTON_LC_CV_DESC, LocDialog::OnButtonLcCvDescClick )

    EVT_BUTTON( wxID_CANCEL, LocDialog::OnCancelClick )

    EVT_BUTTON( wxID_APPLY, LocDialog::OnApplyClick )

    EVT_BUTTON( wxID_OK, LocDialog::OnOkClick )

////@end LocDialog event table entries
    EVT_MENU( ID_PANEL_GENERAL, LocDialog::OnSelectPage )

END_EVENT_TABLE()

/*!
 * LocDialog constructors
 */

LocDialog::LocDialog(  wxWindow* parent, iONode p_Props, bool save )
{
  m_TabAlign = wxGetApp().getTabAlign();
  Create( parent, -1, wxGetApp().getMsg( "loctable" ) );
  m_Props = p_Props;
  m_bSave = save;
  m_iSelectedCV = -1;
  m_iFunGroup = 0;

  initLabels();
  initCVDesc();

  if( InitIndex() ) {
    InitValues();
    //m_Notebook->SetSelection( 1 );
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_PANEL_GENERAL );
    wxPostEvent( this, event );
  }

  m_IndexPanel->GetSizer()->Layout();
  m_General_Panel->GetSizer()->Layout();
  m_Interface_Panel->GetSizer()->Layout();
  m_DetailsPanel->GetSizer()->Layout();
  m_FunctionPanel->GetSizer()->Layout();
  m_CVPanel->GetSizer()->Layout();

  m_Notebook->Fit();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  m_List2->SetFocus();
}


void LocDialog::SelectNext() {
  m_Props = selectNext();
  InitValues();
}


void LocDialog::SelectPrev() {
  m_Props = selectPrev();
  InitValues();
}


void LocDialog::OnSelectPage(wxCommandEvent& event) {
  m_Notebook->SetSelection( 1 );
}


/* comparator for sorting by id: */
static int __sortID(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = wItem.getid( a );
    const char* idB = wItem.getid( b );
    return strcmp( idA, idB );
}

/* comparator for sorting by id: */
static int __sortStr(obj* _a, obj* _b)
{
    const char* a = (const char*)*_a;
    const char* b = (const char*)*_b;
    return strcmp( a, b );
}


void LocDialog::initLabels() {
  m_Notebook->SetPageText( 0, wxGetApp().getMsg( "index" ) );
  m_Notebook->SetPageText( 1, wxGetApp().getMsg( "general" ) );
  m_Notebook->SetPageText( 2, wxGetApp().getMsg( "interface" ) );
  m_Notebook->SetPageText( 3, wxGetApp().getMsg( "details" ) );
  m_Notebook->SetPageText( 4, wxGetApp().getMsg( "functions" ) );
  m_Notebook->SetPageText( 5, wxGetApp().getMsg( "multipleunit" ) );

  // Index
  initList(m_List2, this, false, true, true);
  m_New->SetLabel( wxGetApp().getMsg( "new" ) );
  m_Delete->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_Doc->SetLabel( wxGetApp().getMsg( "doc_report" ) );
  m_ImportLocos->SetLabel( wxGetApp().getMenu( "import" ) );
  m_Copy->SetLabel( wxGetApp().getMsg( "copy" ) );


  // General
  m_label_ID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_labShortID->SetLabel( wxGetApp().getMsg( "shortid" ) );
  m_labRoadname->SetLabel( wxGetApp().getMsg( "roadname" ) );
  m_labNumber->SetLabel( wxGetApp().getMsg( "number" ) );
  m_Label_Description->SetLabel( wxGetApp().getMsg( "description" ) );
  m_Label_Length->SetLabel( wxGetApp().getMsg( "length" ) );
  m_Label_CatalogNr->SetLabel( wxGetApp().getMsg( "catnr" ) );
  m_Label_Purchased->SetLabel( wxGetApp().getMsg( "purchased" ) );
  m_Label_Runtime->SetLabel( wxGetApp().getMsg( "runtime" ) );
  m_labDecType->SetLabel( wxGetApp().getMsg( "decodertype" ) );
  m_labIdent->SetLabel( wxGetApp().getMsg( "identifier" ) );
  m_Ident->SetToolTip( wxGetApp().getTip( "identifier" ) );
  m_labRemark->SetLabel( wxGetApp().getMsg( "remark" ) );
  m_labImageName->SetLabel( wxGetApp().getMsg( "image" ) );
  m_Show->SetLabel( wxGetApp().getMsg( "show" ) );
  m_labMTime->SetLabel( wxGetApp().getMsg( "mtime" ) );
  m_labMInt->SetLabel( wxGetApp().getMsg( "mint" ) );
  m_MTime->SetToolTip( wxGetApp().getTip( "mtime" ) );
  m_MInt->SetToolTip( wxGetApp().getTip( "mint" ) );
  m_Serviced->SetLabel( wxGetApp().getMsg( "serviced" ) );
  m_Serviced->SetToolTip( wxGetApp().getTip( "serviced" ) );
  m_UseShortID->SetLabel( wxGetApp().getMsg( "use" ) + _T(" ") + wxGetApp().getMsg( "shortid" ) );
  m_Actions->SetLabel( wxGetApp().getMsg( "actions" )+_T("...") );
  m_labDocumentation->SetLabel( wxGetApp().getMsg( "documentation" ) );

  // Interface
  m_Label_Bus->SetLabel( wxGetApp().getMsg( "bus" ) );
  m_Label_Address->SetLabel( wxGetApp().getMsg( "address" ) );
  m_Label_IID->SetLabel( wxGetApp().getMsg( "iid" ) );
  m_Label_Protocol->SetLabel( wxGetApp().getMsg( "protocol" ) );
  m_LabelProtVersion->SetLabel( wxGetApp().getMsg( "protocol_version" ) );
  m_LabelSpeedSteps->SetLabel( wxGetApp().getMsg( "speedsteps" ) );
  m_LabelFunCnt->SetLabel( wxGetApp().getMsg( "functioncount" ) );
  m_Label_V_min->SetLabel( wxGetApp().getMsg( "v_min" ) );
  m_Label_V_mid->SetLabel( wxGetApp().getMsg( "v_mid" ) );
  m_labVcru->SetLabel( wxGetApp().getMsg( "v_cru" ) );
  m_Label_V_max->SetLabel( wxGetApp().getMsg( "v_max" ) );

  m_Label_V_Rmin->SetLabel( wxGetApp().getMsg( "v_rmin" ) );
  m_Label_V_Rmid->SetLabel( wxGetApp().getMsg( "v_rmid" ) );
  m_labVRcru->SetLabel( wxGetApp().getMsg( "v_rcru" ) );
  m_Label_V_Rmax->SetLabel( wxGetApp().getMsg( "v_rmax" ) );
  m_V_Rmin->SetToolTip( wxGetApp().getTip( "v_rmin" ) );
  m_V_Rmax->SetToolTip( wxGetApp().getTip( "v_rmax" ) );

  m_LabelV_step->SetLabel( wxGetApp().getMsg( "v_step" ) );
  m_LabelV_mode->SetLabel( wxGetApp().getMsg( "v_mode" ) );
  m_V_mode_percent->SetLabel( wxGetApp().getMsg( "percent" ) );
  m_Label_Mass->SetLabel( wxGetApp().getMsg( "mass" ) );
  m_LabelPlacing->SetLabel( wxGetApp().getMsg( "placing" ) );
  m_Placing->SetLabel( wxGetApp().getMsg( "default" ) );
  m_DirPause->SetLabel( wxGetApp().getMsg( "dirpause" ) );
  m_DirPause->SetToolTip( wxGetApp().getTip( "dirpause" ) );
  m_Regulated->SetLabel( wxGetApp().getMsg( "regulated" ) );
  m_Regulated->SetToolTip( wxGetApp().getTip( "regulated" ) );
  m_RestoreFx->SetLabel( wxGetApp().getMsg( "restorefx" ) );
  m_RestoreSpeed->SetLabel( wxGetApp().getMsg( "restorespeed" ) );
  m_Info4Throttle->SetLabel( wxGetApp().getMsg( "info4throttle" ) );

  // Details
  m_Label_Blockwait->SetLabel( wxGetApp().getMsg( "blockwait" ) );
  m_labEventTimer->SetLabel( wxGetApp().getMsg( "eventtimer" ) );
  m_labSwapTimer->SetLabel( wxGetApp().getMsg( "swaptimer" ) );
  m_labEvtCorrection->SetLabel( wxGetApp().getMsg( "evtcorrection" ) );
  m_labPriority->SetLabel( wxGetApp().getMsg( "priority" ) );
  m_Priority->SetToolTip( wxGetApp().getTip( "priority" ) );
  m_UseScheduleTime->SetLabel( wxGetApp().getMsg( "usescheduletime" ) );
  m_Commuter->SetLabel( wxGetApp().getMsg( "commuter" ) );
  m_SameDir->SetLabel( wxGetApp().getMsg( "samedir" ) );
  m_SameDir->SetToolTip( wxGetApp().getTip( "samedir" ) );
  m_OppositeDir->SetLabel( wxGetApp().getMsg( "oppositedir" ) );
  m_OppositeDir->SetToolTip( wxGetApp().getTip( "oppositedir" ) );
  m_ForceSameDir->SetLabel( wxGetApp().getMsg( "forcesamedir" ) );
  m_ForceSameDir->SetToolTip( wxGetApp().getTip( "forcesamedir" ) );
  m_EngineBox->SetLabel( wxGetApp().getMsg( "engine" ) );
  m_EngineBox->SetString( 0, wxGetApp().getMsg( "diesel" ) );
  m_EngineBox->SetString( 1, wxGetApp().getMsg( "steam" ) );
  m_EngineBox->SetString( 2, wxGetApp().getMsg( "electric" ) );
  m_EngineBox->SetString( 3, wxGetApp().getMsg( "automobile" ) );
  m_CargoBox->SetLabel( wxGetApp().getMsg( "cargo" ) );
  m_CargoBox->SetString( 0, wxGetApp().getMsg( "none" ) );
  m_CargoBox->SetString( 1, wxGetApp().getMsg( "goods" ) );
  m_CargoBox->SetString( 2, wxGetApp().getMsg( "local" ) );
  m_CargoBox->SetString( 3, wxGetApp().getMsg( "mixed" ) );
  m_CargoBox->SetString( 4, wxGetApp().getMsg( "cleaning" ) );
  m_CargoBox->SetString( 5, wxGetApp().getMsg( "ice" ) );
  m_CargoBox->SetString( 6, wxGetApp().getMsg( "post" ) );
  m_CargoBox->SetString( 7, wxGetApp().getMsg( "light" ) );
  m_CargoBox->SetString( 8, wxGetApp().getMsg( "lightgoods" ) );
  m_CargoBox->SetString( 9, wxGetApp().getMsg( "regional" ) );
  m_CargoBox->SetString( 10, wxGetApp().getMsg( "all" ) );
  m_secondNextBlock->SetLabel( wxGetApp().getMsg( "secondnextblock" ) );
  m_ShortIn->SetLabel( wxGetApp().getMsg( "useshortinevent" ) );
  m_InAtPre2In->SetLabel( wxGetApp().getMsg( "inatpre2in" ) );
  m_UseManualRoutes->SetLabel( wxGetApp().getMsg( "usemanualroutes" ) );
  m_UseOwnWaittime->SetLabel( wxGetApp().getMsg( "useownwaittime" ) );

  m_labStartupSchedule->SetLabel( wxGetApp().getMsg( "startupschedule" ) );
  m_StartupSchedule->Append( _T("") );
  iONode model = wxGetApp().getModel();
  iOList list = ListOp.inst();

  if( model != NULL ) {
    iONode sclist = wPlan.getsclist( model );
    if( sclist != NULL ) {
      int cnt = NodeOp.getChildCnt( sclist );
      for( int i = 0; i < cnt; i++ ) {
        iONode sc = NodeOp.getChild( sclist, i );
        ListOp.add(list, (obj)wSchedule.getid( sc ));
      }
    }

    ListOp.sort(list, &__sortStr);
    int cnt = ListOp.size( list );
    for( int i = 0; i < cnt; i++ ) {
      const char* id = (const char*)ListOp.get( list, i );
      m_StartupSchedule->Append( wxString(id,wxConvUTF8) );
    }

  }

  ListOp.base.del(list);

  // Functions
  m_labFnNr->SetLabel( _T("") );
  m_labFnDesc->SetLabel( wxGetApp().getMsg( "description" ) );
  m_labFnTimer->SetLabel( wxGetApp().getMsg( "timer" ) );
  m_labFnEvents->SetLabel( wxGetApp().getMsg( "events" ) );
  m_labFnAddr->SetLabel( wxGetApp().getMsg( "address" ) );
  m_labSound->SetLabel( wxGetApp().getMsg( "sound" ) );
  m_labIcon->SetLabel( wxGetApp().getMsg( "icon" ) );

  m_labF0Nr->SetLabel( _T("") );
  m_labF0Desc->SetLabel( wxGetApp().getMsg( "description" ) );
  m_labF0Timer->SetLabel( wxGetApp().getMsg( "timer" ) );
  m_labF0Events->SetLabel( wxGetApp().getMsg( "events" ) );
  m_labF0Icon->SetLabel( wxGetApp().getMsg( "icon" ) );

  // Consist
  m_labDetails->SetLabel( wxGetApp().getMsg( "details" ) );
  m_LightsOff->SetLabel( wxGetApp().getMsg( "lightsoff" ) );
  m_SyncFun->SetLabel( wxGetApp().getMsg( "sync" ) + _T(" ") + wxGetApp().getMsg( "functions" ) );
  m_labConsistList->SetLabel( wxGetApp().getMsg( "members" ) );
  m_labConsistLocID->SetLabel( wxGetApp().getMsg( "locid" ) );
  m_AddConsistLoc->SetLabel( wxGetApp().getMsg( "add" ) );
  m_DeleteConsistLoc->SetLabel( wxGetApp().getMsg( "delete" ) );

  // CV's
  m_CVList->SetRowLabelSize(0);
  m_CVList->EnableEditing(false);
  m_CVList->EnableDragGridSize(false);
  m_CVList->SetSelectionMode(wxGrid::wxGridSelectRows);
  m_CVList->SetColLabelValue(0, _T("CV"));
  m_CVList->SetColLabelValue(1, wxGetApp().getMsg( "value" ));
  m_CVList->SetColLabelValue(2, wxGetApp().getMsg( "description" ));
  m_CVList->AutoSizeColumns();
  m_CVList->AutoSizeRows();
  m_CVDescModify->SetLabel( wxGetApp().getMsg( "modify" ) );
  m_labCVDesc->SetLabel( wxGetApp().getMsg( "description" ) );
  m_CVDescModify->Enable(false);
  m_CVDescription->Enable(false);

  // Buttons
  m_OK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
  m_Apply->SetLabel( wxGetApp().getMsg( "apply" ) );
  m_Apply->Enable( m_bSave );
}

static int __sortCV(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    int nrA = wCVByte.getnr( a );
    int nrB = wCVByte.getnr( b );
    if( nrA == nrB )
      return 0;
    if( nrA > nrB )
      return 1;
    return -1;
}


bool LocDialog::InitIndex() {
  TraceOp.trc( "locdlg", TRCLEVEL_INFO, __LINE__, 9999, "InitIndex" );

  m_ConsistLocID->Clear();

  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode lclist = wPlan.getlclist( model );
    if( lclist != NULL ) {
      fillIndex(lclist);
      iOList list = ListOp.inst();
      int cnt = NodeOp.getChildCnt( lclist );
      for( int i = 0; i < cnt; i++ ) {
        iONode lc = NodeOp.getChild( lclist, i );
        const char* id = wLoc.getid( lc );
        if( id != NULL ) {
          ListOp.add(list, (obj)lc);
        }
      }

      ListOp.sort(list, &__sortID);
      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        iONode lc = (iONode)ListOp.get( list, i );
        const char* id = wLoc.getid( lc );
        m_ConsistLocID->Append( wxString(id,wxConvUTF8) );
      }
      /* clean up the temp. list */
      ListOp.base.del(list);

      if( m_Props != NULL ) {
        setIDSelection(wLoc.getid( m_Props ));
        return true;
      }
      else {
        m_Props = setSelection(0);
      }
    }

  }
  return false;
}


void LocDialog::initFunctions() {
  // flags for functions
  wxTextCtrl* l_fx[] = {m_f0,m_f1,m_f2,m_f3,m_f4};
  wxTextCtrl* l_sound[] = {NULL,m_Sound1,m_Sound2,m_Sound3,m_Sound4};
  wxTextCtrl* l_icon[] = {m_F0Icon,m_Icon_f1,m_Icon_f2,m_Icon_f3,m_Icon_f4};
  wxStaticText* l_labfx[] = {m_Label_f1,m_Label_f2,m_Label_f3,m_Label_f4};
  wxSpinCtrl* l_timer[] = {m_TimerF0,m_TimerF1,m_TimerF2,m_TimerF3,m_TimerF4};
  wxSpinCtrl* l_addr[] = {NULL,m_F1Addr,m_F2Addr,m_F3Addr,m_F4Addr};
  wxSpinCtrl* l_addrfn[] = {NULL,m_F1AddrFn,m_F2AddrFn,m_F3AddrFn,m_F4AddrFn};

  for( int i = 0; i < 5; i++ ) {
    l_fx[i]->SetValue( _T("") );
    l_timer[i]->SetValue(0);
    if( i > 0 ) {
      l_addr[i]->SetValue(0);
      l_addrfn[i]->SetValue(0);
      l_sound[i]->SetValue( _T("") );
      l_icon[i]->SetValue( _T("") );
    }
  }
  for( int i = 0; i < 4; i++ ) {
    l_labfx[i]->SetLabel( wxString::Format(_T("F%d"), i+1+m_iFunGroup*4 ) );
  }
  m_labFnGroup->SetLabel( wxString::Format(_T("%d"),1+m_iFunGroup ) );

  m_FunctionPanel->GetSizer()->Layout();

  // Init Functions
  TraceOp.trc( "locdlg", TRCLEVEL_INFO, __LINE__, 9999, "Init functions %s", wLoc.getid( m_Props ) );
  iONode fundef = wLoc.getfundef( m_Props );
  while( fundef != NULL ) {

    wxString fntxt( wFunDef.gettext( fundef ),wxConvUTF8 );
    wxString soundtxt( wFunDef.getsound( fundef ),wxConvUTF8 );
    wxString icontxt( wFunDef.geticon( fundef ),wxConvUTF8 );
    int funnr = wFunDef.getfn( fundef );
    TraceOp.trc( "locdlg", TRCLEVEL_INFO, __LINE__, 9999,
        "function[%d] name[%s]", funnr, wFunDef.gettext( fundef ) );

    if( funnr == 0 ) {
      l_fx[0]->SetValue( fntxt );
      l_timer[0]->SetValue( wFunDef.gettimer(fundef) );
      l_icon[0]->SetValue( icontxt );
    }
    else if( funnr == m_iFunGroup * 4 + 1 ) {
      l_fx[1]->SetValue( fntxt );
      l_sound[1]->SetValue( soundtxt );
      l_icon[1]->SetValue( icontxt );
      l_timer[1]->SetValue( wFunDef.gettimer(fundef) );
      l_addr[1]->SetValue( wFunDef.getaddr(fundef) );
      l_addrfn[1]->SetValue( wFunDef.getmappedfn(fundef) );
    }
    else if( funnr == m_iFunGroup * 4 + 2 ) {
      l_fx[2]->SetValue( fntxt );
      l_sound[2]->SetValue( soundtxt );
      l_icon[2]->SetValue( icontxt );
      l_timer[2]->SetValue( wFunDef.gettimer(fundef) );
      l_addr[2]->SetValue( wFunDef.getaddr(fundef) );
      l_addrfn[2]->SetValue( wFunDef.getmappedfn(fundef) );
    }
    else if( funnr == m_iFunGroup * 4 + 3 ) {
      l_fx[3]->SetValue( fntxt );
      l_sound[3]->SetValue( soundtxt );
      l_icon[3]->SetValue( icontxt );
      l_timer[3]->SetValue( wFunDef.gettimer(fundef) );
      l_addr[3]->SetValue( wFunDef.getaddr(fundef) );
      l_addrfn[3]->SetValue( wFunDef.getmappedfn(fundef) );
    }
    else if( funnr == m_iFunGroup * 4 + 4 ) {
      l_fx[4]->SetValue( fntxt );
      l_sound[4]->SetValue( soundtxt );
      l_icon[4]->SetValue( icontxt );
      l_timer[4]->SetValue( wFunDef.gettimer(fundef) );
      l_addr[4]->SetValue( wFunDef.getaddr(fundef) );
      l_addrfn[4]->SetValue( wFunDef.getmappedfn(fundef) );
    }

    fundef = wLoc.nextfundef( m_Props, fundef );
  }

}


void LocDialog::InitValues() {
  if( m_Props == NULL ) {
    return;
  }

  char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("loc").mb_str(wxConvUTF8), wLoc.getid( m_Props ) );
  SetTitle( wxString(title,wxConvUTF8) );
  StrOp.free( title );

  TraceOp.trc( "locdlg", TRCLEVEL_INFO, __LINE__, 9999, "InitValues %s", wLoc.getid( m_Props ) );
  // Init General
  if( wLoc.getimage( m_Props ) != NULL ) {
    wxBitmapType bmptype = wxBITMAP_TYPE_XPM;
    if( StrOp.endsWithi( wLoc.getimage( m_Props ), ".gif" ) )
      bmptype = wxBITMAP_TYPE_GIF;
    else if( StrOp.endsWithi( wLoc.getimage( m_Props ), ".png" ) )
      bmptype = wxBITMAP_TYPE_PNG;

    const char* imagepath = wGui.getimagepath(wxGetApp().getIni());
    const char* imagename = FileOp.ripPath( wLoc.getimage( m_Props ) );
    static char pixpath[256];
    StrOp.fmtb( pixpath, "%s%c%s", imagepath, SystemOp.getFileSeparator(), imagename );

    if( imagename != NULL && StrOp.len(imagename) > 0 && FileOp.exist(pixpath)) {
      TraceOp.trc( "locdlg", TRCLEVEL_INFO, __LINE__, 9999, "picture [%s]", pixpath );
      m_LocImage->SetBitmapLabel( wxBitmap(wxString(pixpath,wxConvUTF8), bmptype) );
    }
    else {
      TraceOp.trc( "locdlg", TRCLEVEL_WARNING, __LINE__, 9999, "picture [%s] not found", pixpath );
      m_LocImage->SetBitmapLabel( wxBitmap(nopict_xpm) );
    }
    m_LocImage->SetToolTip(wxString(wLoc.getdesc( m_Props ),wxConvUTF8));


    //m_LocImage->SetBitmapLabel( wxBitmap(wxString(wLoc.getimage( m_Props ),wxConvUTF8), bmptype) );
    //m_LocImageIndex->SetBitmapLabel( wxBitmap(wxString(wLoc.getimage( m_Props ),wxConvUTF8), bmptype) );
  }
  else {
    m_LocImage->SetBitmapLabel( wxBitmap(nopict_xpm) );
    //m_LocImageIndex->SetBitmapLabel( wxBitmap(nopict_xpm) );
  }
  m_LocImage->Refresh();
  //m_LocImageIndex->Refresh();

  m_ImageNr->SetValue(wLoc.getimagenr( m_Props ));
  m_Show->SetValue(wLoc.isshow( m_Props ));
  m_UseShortID->SetValue(wLoc.isuseshortid( m_Props ));
  m_ID->SetValue( wxString(wLoc.getid( m_Props ),wxConvUTF8) );
  m_ShortID->SetValue( wxString(wLoc.getshortid( m_Props ),wxConvUTF8) );
  m_ThrottleNr->SetValue( wLoc.getthrottlenr( m_Props ) );
  m_Roadname->SetValue( wxString(wLoc.getroadname( m_Props ),wxConvUTF8) );
  m_Number->SetValue( wxString(wLoc.getnumber( m_Props ),wxConvUTF8) );
  m_Description->SetValue( wxString(wLoc.getdesc( m_Props ),wxConvUTF8) );
  m_DecoderType->SetValue( wxString(wLoc.getdectype( m_Props ),wxConvUTF8) );
  m_Documentation->SetValue( wxString(wLoc.getdocu( m_Props ),wxConvUTF8) );
  m_ImageName->SetValue( wxString(wLoc.getimage( m_Props ),wxConvUTF8) );
  m_Remark->SetValue( wxString(wLoc.getremark( m_Props ),wxConvUTF8) );
  char* str = StrOp.fmt( "%d", wLoc.getlen( m_Props ) );
  m_Length->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  m_NrCars->SetValue( wLoc.getnrcars( m_Props ) );
  m_CatNr->SetValue( wxString(wLoc.getcatnr( m_Props ),wxConvUTF8) );
  m_Purchased->SetValue( wxString(wLoc.getpurchased( m_Props ),wxConvUTF8) );
  int runtime = wLoc.getruntime( m_Props );
  str = StrOp.fmt( "%d:%02d.%02d",
      runtime/3600,
      (runtime%3600)/60,
      (runtime%3600)%60 );
  m_RunTime->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );

  runtime = wLoc.getmtime( m_Props );
  str = StrOp.fmt( "%d:%02d.%02d",
      runtime/3600,
      (runtime%3600)/60,
      (runtime%3600)%60 );
  m_MTime->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  m_MInt->SetValue( wLoc.getmint( m_Props ) );

  if( wLoc.getmint( m_Props ) > 0 ) {
    if(wLoc.getruntime( m_Props ) - wLoc.getmtime( m_Props ) > wLoc.getmint( m_Props ) * 3600 ) {
      m_Serviced->SetBackgroundColour( Base::getRed() );
    }
    else {
      m_Serviced->SetBackgroundColour( Base::getGreen() );
    }
  }

  m_StartupSchedule->SetStringSelection( wLoc.getstartupscid( m_Props ) == NULL ?
                                _T(""):wxString(wLoc.getstartupscid( m_Props ),wxConvUTF8)  );

  m_Ident->SetValue( wxString(wLoc.getidentifier( m_Props ),wxConvUTF8) );

  // Init Interface
  str = StrOp.fmt( "%d", wLoc.getbus( m_Props ) );
  m_Bus->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wLoc.getaddr( m_Props ) );
  m_Address->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  m_IID->SetValue( wxString(wLoc.getiid( m_Props ),wxConvUTF8) );

  if( StrOp.equals( wLoc.prot_P, wLoc.getprot( m_Props ) ) )
    m_Protocol->SetSelection( 0 );
  else if( StrOp.equals( wLoc.prot_M, wLoc.getprot( m_Props ) ) )
    m_Protocol->SetSelection( 1 );
  else if( StrOp.equals( wLoc.prot_N, wLoc.getprot( m_Props ) ) )
    m_Protocol->SetSelection( 2 );
  else if( StrOp.equals( wLoc.prot_L, wLoc.getprot( m_Props ) ) )
    m_Protocol->SetSelection( 3 );
  else if( StrOp.equals( wLoc.prot_A, wLoc.getprot( m_Props ) ) )
    m_Protocol->SetSelection( 4 );
  else if( StrOp.equals( wLoc.prot_C, wLoc.getprot( m_Props ) ) )
    m_Protocol->SetSelection( 5 );
  else if( StrOp.equals( wLoc.prot_S, wLoc.getprot( m_Props ) ) )
    m_Protocol->SetSelection( 6 );
  else if( StrOp.equals( wLoc.prot_X, wLoc.getprot( m_Props ) ) )
    m_Protocol->SetSelection( 7 );

  str = StrOp.fmt( "%d", wLoc.getprotver( m_Props ) );
  m_ProtVersion->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wLoc.getfncnt( m_Props ) );
  m_FunCnt->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wLoc.getspcnt( m_Props ) );
  m_SpeedSteps->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );

  str = StrOp.fmt( "%d", wLoc.getV_min( m_Props ) );
  m_V_min->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wLoc.getV_mid( m_Props ) );
  m_V_mid->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wLoc.getV_cru( m_Props ) );
  m_V_cru->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wLoc.getV_max( m_Props ) );
  m_V_max->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wLoc.getV_Rmin( m_Props ) );
  m_V_Rmin->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wLoc.getV_Rmid( m_Props ) );
  m_V_Rmid->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wLoc.getV_Rcru( m_Props ) );
  m_V_Rcru->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wLoc.getV_Rmax( m_Props ) );
  m_V_Rmax->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wLoc.getmass( m_Props ) );
  m_Mass->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );

  m_V_mode_percent->SetValue( StrOp.equals( wLoc.V_mode_percent, wLoc.getV_mode( m_Props ) ) ? true:false );

  m_Placing->SetValue( wLoc.isplacing( m_Props ) ? true:false );
  m_Regulated->SetValue( wLoc.isregulated( m_Props ) ? true:false );
  m_RestoreFx->SetValue( wLoc.isrestorefx( m_Props ) ? true:false );
  m_RestoreSpeed->SetValue( wLoc.isrestorespeed( m_Props ) ? true:false );
  m_Info4Throttle->SetValue( wLoc.isinfo4throttle( m_Props ) ? true:false );

  str = StrOp.fmt( "%d", wLoc.getV_step( m_Props ) );
  m_Accel->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  if( wLoc.isregulated( m_Props ) || wLoc.getspcnt( m_Props ) > 14 ) {
    m_Accel->Enable( false );
  }

  str = StrOp.fmt( "%d", wLoc.getdirpause( m_Props ) );
  m_DirPause->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );

  // Init Details
  str = StrOp.fmt( "%d", wLoc.getblockwaittime( m_Props ) );
  m_Blockwait->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wLoc.getevttimer( m_Props ) );
  m_EventTimer->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  m_SwapTimer->SetValue( wLoc.getswaptimer( m_Props ) );
  m_EvtCorrection->SetValue( wLoc.getent2incorr( m_Props ) );
  m_Priority->SetValue( wLoc.getpriority( m_Props ) );
  m_UseScheduleTime->SetValue( wLoc.isusescheduletime( m_Props ) ? true:false );
  m_Commuter->SetValue( wLoc.iscommuter( m_Props ) ? true:false );
  m_SameDir->SetValue( wLoc.istrysamedir( m_Props ) ? true:false );
  m_OppositeDir->SetValue( wLoc.istryoppositedir( m_Props ) ? true:false );
  m_ForceSameDir->SetValue( wLoc.isforcesamedir( m_Props ) ? true:false );
  m_ShortIn->SetValue( wLoc.isshortin( m_Props ) ? true:false );
  m_InAtPre2In->SetValue( wLoc.isinatpre2in( m_Props ) ? true:false );
  m_UseManualRoutes->SetValue( wLoc.isusemanualroutes( m_Props ) ? true:false );
  m_UseOwnWaittime->SetValue( wLoc.isuseownwaittime( m_Props ) ? true:false );

  int engine = 0;
  if( StrOp.equals( wLoc.engine_diesel, wLoc.getengine( m_Props ) ) )
    engine = 0;
  else if( StrOp.equals( wLoc.engine_steam, wLoc.getengine( m_Props ) ) )
    engine = 1;
  else if( StrOp.equals( wLoc.engine_electric, wLoc.getengine( m_Props ) ) )
    engine = 2;
  else if( StrOp.equals( wLoc.engine_automobile, wLoc.getengine( m_Props ) ) )
    engine = 3;
  m_EngineBox->SetSelection( engine );
  int cargo = 0;
  if( StrOp.equals( wLoc.cargo_none, wLoc.getcargo( m_Props ) ) )
    cargo = 0;
  else if( StrOp.equals( wLoc.cargo_goods, wLoc.getcargo( m_Props ) ) )
    cargo = 1;
  else if( StrOp.equals( wLoc.cargo_person, wLoc.getcargo( m_Props ) ) )
    cargo = 2;
  else if( StrOp.equals( wLoc.cargo_mixed, wLoc.getcargo( m_Props ) ) )
    cargo = 3;
  else if( StrOp.equals( wLoc.cargo_cleaning, wLoc.getcargo( m_Props ) ) )
    cargo = 4;
  else if( StrOp.equals( wLoc.cargo_ice, wLoc.getcargo( m_Props ) ) )
    cargo = 5;
  else if( StrOp.equals( wLoc.cargo_post, wLoc.getcargo( m_Props ) ) )
    cargo = 6;
  else if( StrOp.equals( wLoc.cargo_light, wLoc.getcargo( m_Props ) ) )
    cargo = 7;
  else if( StrOp.equals( wLoc.cargo_lightgoods, wLoc.getcargo( m_Props ) ) )
    cargo = 8;
  else if( StrOp.equals( wLoc.cargo_regional, wLoc.getcargo( m_Props ) ) )
    cargo = 9;
  else if( StrOp.equals( wLoc.cargo_all, wLoc.getcargo( m_Props ) ) )
    cargo = 10;
  m_CargoBox->SetSelection( cargo );
  m_secondNextBlock->SetValue( wLoc.issecondnextblock( m_Props ) );


  // functions
  initFunctions();


  // Consist
  wxCheckBox* funsync[] = {m_FSync1 ,m_FSync2 ,m_FSync3 ,m_FSync4 ,m_FSync5 ,m_FSync6 ,m_FSync7 ,m_FSync8 ,m_FSync9 ,m_FSync10,m_FSync11,m_FSync12,m_FSync13,m_FSync14,
                           m_FSync15,m_FSync16,m_FSync17,m_FSync18,m_FSync19,m_FSync20,m_FSync21,m_FSync22,m_FSync23,m_FSync24,m_FSync25,m_FSync26,m_FSync27,m_FSync28};
  int fmap = wLoc.getconsist_syncfunmap( m_Props );

  for( int i = 0; i < 28; i++ ) {
    funsync[i]->SetValue( (fmap & (1 << i)) ? true:false );
  }

  m_LightsOff->SetValue( wLoc.isconsist_lightsoff( m_Props ) ? true:false );
  m_SyncFun->SetValue( wLoc.isconsist_syncfun( m_Props ) ? true:false );
  m_ConsistList->Clear();
  iOStrTok  consist = StrTokOp.inst( wLoc.getconsist ( m_Props ), ',' );
  while( StrTokOp.hasMoreTokens( consist ) ) {
    const char* tok = StrTokOp.nextToken( consist );
    m_ConsistList->Append( wxString(tok,wxConvUTF8) );
  };
  StrTokOp.base.del( consist );

  // CV's
  iOList list = ListOp.inst();
  iONode cv = wLoc.getcvbyte( m_Props );
  while( cv != NULL ) {
    if( wCVByte.getnr( cv ) < 1 || wCVByte.getnr( cv ) > 1024 )
      continue;
    ListOp.add(list, (obj)cv);
    cv = wLoc.nextcvbyte( m_Props, cv );
  };

  if( m_CVList->GetNumberRows() > 0 )
    m_CVList->DeleteRows( 0, m_CVList->GetNumberRows() );
  ListOp.sort(list, &__sortCV);
  int cnt = ListOp.size( list );
  for( int i = 0; i < cnt && cnt < 1024; i++ ) {
    iONode cv = (iONode)ListOp.get( list, i );
    char* cvnr = StrOp.fmt( "%d", wCVByte.getnr( cv ) );
    char* cvval = StrOp.fmt( "%d", wCVByte.getvalue( cv ) );
    const char* cvdesc = wCVByte.getdesc(cv);
    m_CVNodes[wCVByte.getnr( cv )] = cv;
    m_CVList->AppendRows();
    int row = m_CVList->GetNumberRows()-1;
    m_CVList->SetCellValue(row, 0, wxString(cvnr,wxConvUTF8) );
    m_CVList->SetCellValue(row, 1, wxString(cvval,wxConvUTF8) );
    if( cvdesc != NULL && StrOp.len(cvdesc) > 0 )
      m_CVList->SetCellValue(row, 2, wxString(cvdesc,wxConvUTF8) );
    else
      m_CVList->SetCellValue(row, 2, wxString(m_CVDesc[wCVByte.getnr( cv )&0xff],wxConvUTF8) );
    m_CVList->SetReadOnly( row, 0, true );
    m_CVList->SetReadOnly( row, 1, true );
    m_CVList->SetReadOnly( row, 2, true );
    StrOp.free( cvnr );
    StrOp.free( cvval );
  }
  /* clean up the temp. list */
  ListOp.base.del(list);


  m_CVList->AutoSize();


}


bool LocDialog::evaluateFunctions() {
  // evaluate Functions
  wxTextCtrl* l_fx[] = {m_f0,m_f1,m_f2,m_f3,m_f4};
  wxTextCtrl* l_sound[] = {NULL,m_Sound1,m_Sound2,m_Sound3,m_Sound4};
  wxTextCtrl* l_icon[] = {m_F0Icon,m_Icon_f1,m_Icon_f2,m_Icon_f3,m_Icon_f4};
  wxSpinCtrl* l_timer[] = {m_TimerF0,m_TimerF1,m_TimerF2,m_TimerF3,m_TimerF4};
  wxSpinCtrl* l_addr[] = {NULL,m_F1Addr,m_F2Addr,m_F3Addr,m_F4Addr};
  wxSpinCtrl* l_addrfn[] = {NULL,m_F1AddrFn,m_F2AddrFn,m_F3AddrFn,m_F4AddrFn};
  int function[] = {0,0,0,0,0};

  if( l_fx[0]->GetValue().Length() > 0 )
    function[0] = 1;

  for( int i = 1; i < 5; i++ ) {
    if( l_fx[i]->GetValue().Length() > 0 )
      function[i] = 1;
  }

  TraceOp.trc( "locdlg", TRCLEVEL_INFO, __LINE__, 9999, "evaluate function group %d", m_iFunGroup + 1);


  iONode fundef = wLoc.getfundef( m_Props );
  while( fundef != NULL ) {
    wxString fntxt( wFunDef.gettext( fundef ),wxConvUTF8 );
    int fnr = wFunDef.getfn( fundef );

    if( fnr == 0 ) {
      TraceOp.trc( "locdlg", TRCLEVEL_INFO, __LINE__, 9999, "modify function  %d", fnr );
      wFunDef.settext( fundef, m_f0->GetValue().mb_str(wxConvUTF8)  );
      wFunDef.settimer( fundef, m_TimerF0->GetValue() );
      wFunDef.seticon( fundef, m_F0Icon->GetValue().mb_str(wxConvUTF8)  );
      function[0] = 0;
    }
    else if( fnr >= 1 + m_iFunGroup*4 && fnr <= 5 + m_iFunGroup*4 ) {
      TraceOp.trc( "locdlg", TRCLEVEL_INFO, __LINE__, 9999, "modify function  %d", fnr );
      switch( fnr - m_iFunGroup*4 ) {
        case 1 :
          wFunDef.settext( fundef, m_f1->GetValue().mb_str(wxConvUTF8)  );
          wFunDef.setsound( fundef, m_Sound1->GetValue().mb_str(wxConvUTF8)  );
          wFunDef.seticon( fundef, m_Icon_f1->GetValue().mb_str(wxConvUTF8)  );
          wFunDef.settimer( fundef, m_TimerF1->GetValue() );
          wFunDef.setaddr( fundef, m_F1Addr->GetValue() );
          wFunDef.setmappedfn( fundef, m_F1AddrFn->GetValue() );
          function[fnr - m_iFunGroup*4] = 0;
          break;
        case 2 :
          wFunDef.settext( fundef, m_f2->GetValue().mb_str(wxConvUTF8)  );
          wFunDef.setsound( fundef, m_Sound2->GetValue().mb_str(wxConvUTF8)  );
          wFunDef.seticon( fundef, m_Icon_f2->GetValue().mb_str(wxConvUTF8)  );
          wFunDef.settimer( fundef, m_TimerF2->GetValue() );
          wFunDef.setaddr( fundef, m_F2Addr->GetValue() );
          wFunDef.setmappedfn( fundef, m_F2AddrFn->GetValue() );
          function[fnr - m_iFunGroup*4] = 0;
          break;
        case 3 :
          wFunDef.settext( fundef, m_f3->GetValue().mb_str(wxConvUTF8)  );
          wFunDef.setsound( fundef, m_Sound3->GetValue().mb_str(wxConvUTF8)  );
          wFunDef.seticon( fundef, m_Icon_f3->GetValue().mb_str(wxConvUTF8)  );
          wFunDef.settimer( fundef, m_TimerF3->GetValue() );
          wFunDef.setaddr( fundef, m_F3Addr->GetValue() );
          wFunDef.setmappedfn( fundef, m_F3AddrFn->GetValue() );
          function[fnr - m_iFunGroup*4] = 0;
          break;
        case 4 :
          wFunDef.settext( fundef, m_f4->GetValue().mb_str(wxConvUTF8)  );
          wFunDef.setsound( fundef, m_Sound4->GetValue().mb_str(wxConvUTF8)  );
          wFunDef.seticon( fundef, m_Icon_f4->GetValue().mb_str(wxConvUTF8)  );
          wFunDef.settimer( fundef, m_TimerF4->GetValue() );
          wFunDef.setaddr( fundef, m_F4Addr->GetValue() );
          wFunDef.setmappedfn( fundef, m_F4AddrFn->GetValue() );
          function[fnr - m_iFunGroup*4] = 0;
          break;
      }
    }
    fundef = wLoc.nextfundef( m_Props, fundef );
  }

  if( function[0] == 1 ) {
    fundef = NodeOp.inst( wFunDef.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild( m_Props, fundef );
    wFunDef.setfn( fundef, 0 );
    wFunDef.settext( fundef, l_fx[0]->GetValue().mb_str(wxConvUTF8) );
    wFunDef.settimer( fundef, m_TimerF0->GetValue() );
  }
  for( int i = 1; i < 5; i++ ) {
    if( function[i] == 1 ) {
      int f = i + m_iFunGroup*4;
      TraceOp.trc( "locdlg", TRCLEVEL_INFO, __LINE__, 9999, "adding function  %d", f );
      fundef = NodeOp.inst( wFunDef.name(), m_Props, ELEMENT_NODE );
      NodeOp.addChild( m_Props, fundef );
      wFunDef.setfn( fundef, f );
      wFunDef.settext( fundef, l_fx[i]->GetValue().mb_str(wxConvUTF8) );
      wFunDef.setsound( fundef, l_sound[i]->GetValue().mb_str(wxConvUTF8) );
      wFunDef.seticon( fundef, l_icon[i]->GetValue().mb_str(wxConvUTF8) );
      wFunDef.settimer( fundef, l_timer[i]->GetValue() );
      wFunDef.setaddr( fundef, l_addr[i]->GetValue() );
      wFunDef.setmappedfn( fundef, l_addrfn[i]->GetValue() );
    }
  }
  return true;
}

bool LocDialog::Evaluate() {
  if( m_Props == NULL )
    return false;

  TraceOp.trc( "locdlg", TRCLEVEL_INFO, __LINE__, 9999, "Evaluate %s", wLoc.getid( m_Props ) );

  if( m_ID->GetValue().Len() == 0 ) {
    wxMessageDialog( this, wxGetApp().getMsg("invalidid"), _T("Rocrail"), wxOK | wxICON_ERROR ).ShowModal();
    m_ID->SetValue( wxString(wLoc.getid( m_Props ),wxConvUTF8) );
    return false;
  }

  // evaluate General
  wItem.setprev_id( m_Props, wItem.getid(m_Props) );
  wLoc.setid( m_Props, m_ID->GetValue().mb_str(wxConvUTF8) );
  wLoc.setshortid( m_Props, m_ShortID->GetValue().mb_str(wxConvUTF8) );
  wLoc.setroadname( m_Props, m_Roadname->GetValue().mb_str(wxConvUTF8) );
  wLoc.setnumber( m_Props, m_Number->GetValue().mb_str(wxConvUTF8) );
  wLoc.setdesc( m_Props, m_Description->GetValue().mb_str(wxConvUTF8) );
  wLoc.setdectype( m_Props, m_DecoderType->GetValue().mb_str(wxConvUTF8) );
  wLoc.setdocu( m_Props, m_Documentation->GetValue().mb_str(wxConvUTF8) );
  wLoc.setimage( m_Props, m_ImageName->GetValue().mb_str(wxConvUTF8) );
  wLoc.setimagenr( m_Props, m_ImageNr->GetValue() );
  wLoc.setremark( m_Props, m_Remark->GetValue().mb_str(wxConvUTF8) );
  int val = atoi( m_Length->GetValue().mb_str(wxConvUTF8) );
  wLoc.setlen( m_Props, val );
  wLoc.setnrcars( m_Props, m_NrCars->GetValue() );
  wLoc.setcatnr( m_Props, m_CatNr->GetValue().mb_str(wxConvUTF8) );
  wLoc.setpurchased( m_Props, m_Purchased->GetValue().mb_str(wxConvUTF8) );
  wLoc.setidentifier( m_Props, m_Ident->GetValue().mb_str(wxConvUTF8) );
  wLoc.setshow( m_Props, m_Show->IsChecked()?True:False);
  wLoc.setuseshortid( m_Props, m_UseShortID->IsChecked()?True:False);
  wLoc.setmint( m_Props, m_MInt->GetValue() );
  wLoc.setthrottlenr( m_Props, m_ThrottleNr->GetValue() );

  // evaluate Interface
  val = atoi( m_Bus->GetValue().mb_str(wxConvUTF8) );
  wLoc.setbus( m_Props, val );
  val = atoi( m_Address->GetValue().mb_str(wxConvUTF8) );
  wLoc.setaddr( m_Props, val );
  wLoc.setiid( m_Props, m_IID->GetValue().mb_str(wxConvUTF8) );

  if( m_Protocol->GetSelection() == 0 )
    wLoc.setprot( m_Props, wLoc.prot_P );
  else if( m_Protocol->GetSelection() == 1 )
    wLoc.setprot( m_Props, wLoc.prot_M );
  else if( m_Protocol->GetSelection() == 2 )
    wLoc.setprot( m_Props, wLoc.prot_N );
  else if( m_Protocol->GetSelection() == 3 )
    wLoc.setprot( m_Props, wLoc.prot_L );
  else if( m_Protocol->GetSelection() == 4 )
    wLoc.setprot( m_Props, wLoc.prot_A );
  else if( m_Protocol->GetSelection() == 5 )
    wLoc.setprot( m_Props, wLoc.prot_C );
  else if( m_Protocol->GetSelection() == 6 )
    wLoc.setprot( m_Props, wLoc.prot_S );
  else if( m_Protocol->GetSelection() == 7 )
    wLoc.setprot( m_Props, wLoc.prot_X );

  val = atoi( m_ProtVersion->GetValue().mb_str(wxConvUTF8) );
  wLoc.setprotver( m_Props, val );
  val = atoi( m_SpeedSteps->GetValue().mb_str(wxConvUTF8) );
  wLoc.setspcnt( m_Props, val );
  val = atoi( m_ProtVersion->GetValue().mb_str(wxConvUTF8) );
  wLoc.setprotver( m_Props, val );

  val = atoi( m_FunCnt->GetValue().mb_str(wxConvUTF8) );
  wLoc.setfncnt( m_Props, val );

  val = atoi( m_V_min->GetValue().mb_str(wxConvUTF8) );
  wLoc.setV_min( m_Props, val );
  val = atoi( m_V_mid->GetValue().mb_str(wxConvUTF8) );
  wLoc.setV_mid( m_Props, val );
  val = atoi( m_V_cru->GetValue().mb_str(wxConvUTF8) );
  wLoc.setV_cru( m_Props, val );
  val = atoi( m_V_max->GetValue().mb_str(wxConvUTF8) );
  wLoc.setV_max( m_Props, val );
  val = atoi( m_V_Rmin->GetValue().mb_str(wxConvUTF8) );
  wLoc.setV_Rmin( m_Props, val );
  val = atoi( m_V_Rmid->GetValue().mb_str(wxConvUTF8) );
  wLoc.setV_Rmid( m_Props, val );
  val = atoi( m_V_Rcru->GetValue().mb_str(wxConvUTF8) );
  wLoc.setV_Rcru( m_Props, val );
  val = atoi( m_V_Rmax->GetValue().mb_str(wxConvUTF8) );
  wLoc.setV_Rmax( m_Props, val );
  val = atoi( m_Accel->GetValue().mb_str(wxConvUTF8) );
  wLoc.setV_step( m_Props, val );
  val = atoi( m_Mass->GetValue().mb_str(wxConvUTF8) );
  wLoc.setmass( m_Props, val );
  wLoc.setV_mode( m_Props, m_V_mode_percent->GetValue() ? wLoc.V_mode_percent:wLoc.V_mode_kmh );
  wLoc.setplacing( m_Props, m_Placing->GetValue() ? True:False );
  wLoc.setregulated( m_Props, m_Regulated->GetValue() ? True:False );
  wLoc.setrestorefx( m_Props, m_RestoreFx->GetValue() ? True:False );
  wLoc.setrestorespeed( m_Props, m_RestoreSpeed->GetValue() ? True:False );
  wLoc.setinfo4throttle( m_Props, m_Info4Throttle->GetValue() ? True:False );

  val = atoi( m_DirPause->GetValue().mb_str(wxConvUTF8) );
  wLoc.setdirpause( m_Props, val );

  // evaluate Details
  val = atoi( m_Blockwait->GetValue().mb_str(wxConvUTF8) );
  wLoc.setblockwaittime( m_Props, val );
  val = atoi( m_EventTimer->GetValue().mb_str(wxConvUTF8) );
  wLoc.setevttimer( m_Props, val );
  wLoc.setswaptimer( m_Props, m_SwapTimer->GetValue() );
  wLoc.setent2incorr( m_Props, m_EvtCorrection->GetValue() );
  wLoc.setpriority( m_Props, m_Priority->GetValue() );
  wLoc.setusescheduletime( m_Props, m_UseScheduleTime->GetValue() ? True:False );
  wLoc.setcommuter( m_Props, m_Commuter->GetValue() ? True:False );
  wLoc.settrysamedir( m_Props, m_SameDir->GetValue() ? True:False );
  wLoc.settryoppositedir( m_Props, m_OppositeDir->GetValue() ? True:False );
  wLoc.setforcesamedir( m_Props, m_ForceSameDir->GetValue() ? True:False );
  wLoc.setshortin( m_Props, m_ShortIn->GetValue() ? True:False );
  wLoc.setinatpre2in( m_Props, m_InAtPre2In->GetValue() ? True:False );
  wLoc.setusemanualroutes( m_Props, m_UseManualRoutes->GetValue() ? True:False );
  wLoc.setuseownwaittime( m_Props, m_UseOwnWaittime->GetValue() ? True:False );
  wLoc.setstartupscid( m_Props, m_StartupSchedule->GetStringSelection().mb_str(wxConvUTF8) );

  int engine = m_EngineBox->GetSelection();
  if( engine == 0 )
    wLoc.setengine( m_Props, wLoc.engine_diesel );
  else if( engine == 1 )
    wLoc.setengine( m_Props, wLoc.engine_steam );
  else if( engine == 2 )
    wLoc.setengine( m_Props, wLoc.engine_electric );
  else if( engine == 3 )
    wLoc.setengine( m_Props, wLoc.engine_automobile );

  int cargo = m_CargoBox->GetSelection();
  if( cargo == 0 )
    wLoc.setcargo( m_Props, wLoc.cargo_none );
  else if( cargo == 1 )
    wLoc.setcargo( m_Props, wLoc.cargo_goods );
  else if( cargo == 2 )
    wLoc.setcargo( m_Props, wLoc.cargo_person );
  else if( cargo == 3 )
    wLoc.setcargo( m_Props, wLoc.cargo_mixed );
  else if( cargo == 4 )
    wLoc.setcargo( m_Props, wLoc.cargo_cleaning );
  else if( cargo == 5 )
    wLoc.setcargo( m_Props, wLoc.cargo_ice );
  else if( cargo == 6 )
    wLoc.setcargo( m_Props, wLoc.cargo_post );
  else if( cargo == 7 )
    wLoc.setcargo( m_Props, wLoc.cargo_light );
  else if( cargo == 8 )
    wLoc.setcargo( m_Props, wLoc.cargo_lightgoods );
  else if( cargo == 9 )
    wLoc.setcargo( m_Props, wLoc.cargo_regional );
  else if( cargo == 10 )
    wLoc.setcargo( m_Props, wLoc.cargo_all );

  wLoc.setsecondnextblock( m_Props, m_secondNextBlock->IsChecked() ? True:False );

  // Functions
  evaluateFunctions();

  // Consist
  wxCheckBox* funsync[] = {m_FSync1 ,m_FSync2 ,m_FSync3 ,m_FSync4 ,m_FSync5 ,m_FSync6 ,m_FSync7 ,m_FSync8 ,m_FSync9 ,m_FSync10,m_FSync11,m_FSync12,m_FSync13,m_FSync14,
                           m_FSync15,m_FSync16,m_FSync17,m_FSync18,m_FSync19,m_FSync20,m_FSync21,m_FSync22,m_FSync23,m_FSync24,m_FSync25,m_FSync26,m_FSync27,m_FSync28};
  int fmap = 0;

  for( int i = 0; i < 28; i++ ) {
    if( funsync[i]->IsChecked() )
      fmap |= (1 << i);
  }
  wLoc.setconsist_syncfunmap( m_Props, fmap );


  wLoc.setconsist_lightsoff( m_Props, m_LightsOff->GetValue() ? True:False );
  wLoc.setconsist_syncfun( m_Props, m_SyncFun->GetValue() ? True:False );
  int locos = m_ConsistList->GetCount();
  char* consist = StrOp.cat( NULL, "" );
  for( int i = 0; i < locos; i++ ) {
    if( i > 0 )
      consist = StrOp.cat( consist, "," );
    consist = StrOp.cat( consist, m_ConsistList->GetString(i).mb_str(wxConvUTF8) );
  }
  wLoc.setconsist( m_Props, consist);
  TraceOp.trc( "locdlg", TRCLEVEL_INFO, __LINE__, 9999, "Consist [%s]", consist );
  StrOp.free(consist);

  return true;
}


/*!
 * LocDialog creator
 */

bool LocDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin LocDialog member initialisation
    m_Props = NULL;
    m_LocImage = NULL;
    m_Notebook = NULL;
    m_IndexPanel = NULL;
    m_List2 = NULL;
    m_New = NULL;
    m_Delete = NULL;
    m_Doc = NULL;
    m_Copy = NULL;
    m_ImportLocos = NULL;
    m_RestoreFx = NULL;
    m_RestoreSpeed = NULL;
    m_Show = NULL;
    m_General_Panel = NULL;
    m_GeneralSizer = NULL;
    m_label_ID = NULL;
    m_ID = NULL;
    m_labRoadname = NULL;
    m_Roadname = NULL;
    m_labNumber = NULL;
    m_Number = NULL;
    m_Label_Description = NULL;
    m_Description = NULL;
    m_labImageName = NULL;
    m_ImageName = NULL;
    m_ImageNr = NULL;
    m_Label_Length = NULL;
    m_Length = NULL;
    m_NrCars = NULL;
    m_Label_CatalogNr = NULL;
    m_CatNr = NULL;
    m_labDecType = NULL;
    m_DecoderType = NULL;
    m_labDocumentation = NULL;
    m_Documentation = NULL;
    m_ShowDoc = NULL;
    m_Label_Purchased = NULL;
    m_Purchased = NULL;
    m_labShortID = NULL;
    m_ShortID = NULL;
    m_labIdent = NULL;
    m_Ident = NULL;
    m_Label_Runtime = NULL;
    m_RunTime = NULL;
    m_labMTime = NULL;
    m_MTime = NULL;
    m_UseShortID = NULL;
    m_ThrottleNr = NULL;
    m_Serviced = NULL;
    m_labMInt = NULL;
    m_MInt = NULL;
    m_labRemark = NULL;
    m_Remark = NULL;
    m_Actions = NULL;
    m_Interface_Panel = NULL;
    m_Label_Address = NULL;
    m_Address = NULL;
    m_Label_IID = NULL;
    m_IID = NULL;
    m_Label_Bus = NULL;
    m_Bus = NULL;
    m_Label_Protocol = NULL;
    m_Protocol = NULL;
    m_LabelProtVersion = NULL;
    m_ProtVersion = NULL;
    m_LabelSpeedSteps = NULL;
    m_SpeedSteps = NULL;
    m_LabelFunCnt = NULL;
    m_FunCnt = NULL;
    m_Label_V_min = NULL;
    m_V_min = NULL;
    m_Label_V_Rmin = NULL;
    m_V_Rmin = NULL;
    m_Label_V_mid = NULL;
    m_V_mid = NULL;
    m_Label_V_Rmid = NULL;
    m_V_Rmid = NULL;
    m_labVcru = NULL;
    m_V_cru = NULL;
    m_labVRcru = NULL;
    m_V_Rcru = NULL;
    m_Label_V_max = NULL;
    m_V_max = NULL;
    m_Label_V_Rmax = NULL;
    m_V_Rmax = NULL;
    m_LabelV_step = NULL;
    m_Accel = NULL;
    m_LabelV_mode = NULL;
    m_V_mode_percent = NULL;
    m_Label_Mass = NULL;
    m_Mass = NULL;
    m_LabelPlacing = NULL;
    m_Placing = NULL;
    m_labDirPause = NULL;
    m_DirPause = NULL;
    m_Regulated = NULL;
    m_Info4Throttle = NULL;
    m_DetailsPanel = NULL;
    m_Label_Blockwait = NULL;
    m_Blockwait = NULL;
    m_labEventTimer = NULL;
    m_EventTimer = NULL;
    m_labEvtCorrection = NULL;
    m_EvtCorrection = NULL;
    m_labPriority = NULL;
    m_Priority = NULL;
    m_labSwapTimer = NULL;
    m_SwapTimer = NULL;
    m_labStartupSchedule = NULL;
    m_StartupSchedule = NULL;
    m_Commuter = NULL;
    m_SameDir = NULL;
    m_OppositeDir = NULL;
    m_ForceSameDir = NULL;
    m_UseScheduleTime = NULL;
    m_secondNextBlock = NULL;
    m_ShortIn = NULL;
    m_InAtPre2In = NULL;
    m_UseManualRoutes = NULL;
    m_UseOwnWaittime = NULL;
    m_EngineBox = NULL;
    m_CargoBox = NULL;
    m_FunctionPanel = NULL;
    m_labF0Nr = NULL;
    m_labF0Desc = NULL;
    m_labF0Timer = NULL;
    m_labF0Events = NULL;
    m_labF0Icon = NULL;
    m_Label_f0 = NULL;
    m_f0 = NULL;
    m_TimerF0 = NULL;
    m_Button_f0 = NULL;
    m_F0Icon = NULL;
    m_labFnGroup = NULL;
    m_FunctionGroup = NULL;
    m_labFnNr = NULL;
    m_labFnDesc = NULL;
    m_labFnTimer = NULL;
    m_labFnEvents = NULL;
    m_labSound = NULL;
    m_labIcon = NULL;
    m_labFnAddr = NULL;
    m_labAddrFn = NULL;
    m_Label_f1 = NULL;
    m_f1 = NULL;
    m_TimerF1 = NULL;
    m_Button_f1 = NULL;
    m_Sound1 = NULL;
    m_Icon_f1 = NULL;
    m_F1Addr = NULL;
    m_F1AddrFn = NULL;
    m_Label_f2 = NULL;
    m_f2 = NULL;
    m_TimerF2 = NULL;
    m_Button_f2 = NULL;
    m_Sound2 = NULL;
    m_Icon_f2 = NULL;
    m_F2Addr = NULL;
    m_F2AddrFn = NULL;
    m_Label_f3 = NULL;
    m_f3 = NULL;
    m_TimerF3 = NULL;
    m_Button_f3 = NULL;
    m_Sound3 = NULL;
    m_Icon_f3 = NULL;
    m_F3Addr = NULL;
    m_F3AddrFn = NULL;
    m_Label_f4 = NULL;
    m_f4 = NULL;
    m_TimerF4 = NULL;
    m_Button_f4 = NULL;
    m_Sound4 = NULL;
    m_Icon_f4 = NULL;
    m_F4Addr = NULL;
    m_F4AddrFn = NULL;
    m_ConsistsPanel = NULL;
    m_labDetails = NULL;
    m_LightsOff = NULL;
    m_SyncFun = NULL;
    m_FSync1 = NULL;
    m_FSync2 = NULL;
    m_FSync3 = NULL;
    m_FSync4 = NULL;
    m_FSync5 = NULL;
    m_FSync6 = NULL;
    m_FSync7 = NULL;
    m_FSync8 = NULL;
    m_FSync9 = NULL;
    m_FSync10 = NULL;
    m_FSync11 = NULL;
    m_FSync12 = NULL;
    m_FSync13 = NULL;
    m_FSync14 = NULL;
    m_FSync15 = NULL;
    m_FSync16 = NULL;
    m_FSync17 = NULL;
    m_FSync18 = NULL;
    m_FSync19 = NULL;
    m_FSync20 = NULL;
    m_FSync21 = NULL;
    m_FSync22 = NULL;
    m_FSync23 = NULL;
    m_FSync24 = NULL;
    m_FSync25 = NULL;
    m_FSync26 = NULL;
    m_FSync27 = NULL;
    m_FSync28 = NULL;
    m_labConsistList = NULL;
    m_ConsistList = NULL;
    m_labConsistLocID = NULL;
    m_ConsistLocID = NULL;
    m_AddConsistLoc = NULL;
    m_DeleteConsistLoc = NULL;
    m_CVPanel = NULL;
    m_CVList = NULL;
    m_labCVDesc = NULL;
    m_CVDescription = NULL;
    m_CVDescModify = NULL;
    m_Cancel = NULL;
    m_Apply = NULL;
    m_OK = NULL;
////@end LocDialog member initialisation

////@begin LocDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end LocDialog creation
    return true;
}

/*!
 * Control creation for LocDialog
 */

void LocDialog::CreateControls()
{
////@begin LocDialog content construction
    LocDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_LocImage = new wxBitmapButton( itemDialog1, ID_BITMAPBUTTON, wxNullBitmap, wxDefaultPosition, wxSize(-1, 88), wxBU_AUTODRAW );
    itemBoxSizer2->Add(m_LocImage, 0, wxGROW|wxALL, 5);

    m_Notebook = new wxNotebook( itemDialog1, ID_NOTEBOOK, wxDefaultPosition, wxDefaultSize, m_TabAlign );

    m_IndexPanel = new wxPanel( m_Notebook, ID_PANEL_INDEX, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxVERTICAL);
    m_IndexPanel->SetSizer(itemBoxSizer6);

    m_List2 = new wxListCtrl( m_IndexPanel, ID_LISTCTRLINDEX_LC, wxDefaultPosition, wxSize(100, 100), wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_HRULES );
    itemBoxSizer6->Add(m_List2, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer6->Add(itemBoxSizer8, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer9 = new wxFlexGridSizer(0, 3, 0, 0);
    itemBoxSizer8->Add(itemFlexGridSizer9, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    m_New = new wxButton( m_IndexPanel, ID_BUTTON_LOC_NEW, _("New"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer9->Add(m_New, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Delete = new wxButton( m_IndexPanel, ID_BUTTON_DELETE, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer9->Add(m_Delete, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Doc = new wxButton( m_IndexPanel, ID_BUTTON_LOCO_DOC, _("Doc"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer9->Add(m_Doc, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Copy = new wxButton( m_IndexPanel, ID_LOC_COPY, _("Copy"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer9->Add(m_Copy, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ImportLocos = new wxButton( m_IndexPanel, ID_BUTTON_IMPORTLOCOS, _("Import..."), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer9->Add(m_ImportLocos, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine15 = new wxStaticLine( m_IndexPanel, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer8->Add(itemStaticLine15, 0, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer16 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer8->Add(itemFlexGridSizer16, 0, wxGROW|wxALL, 5);
    m_RestoreFx = new wxCheckBox( m_IndexPanel, wxID_ANY, _("Restore functions"), wxDefaultPosition, wxDefaultSize, 0 );
    m_RestoreFx->SetValue(false);
    itemFlexGridSizer16->Add(m_RestoreFx, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_RestoreSpeed = new wxCheckBox( m_IndexPanel, wxID_ANY, _("Restore speed"), wxDefaultPosition, wxDefaultSize, 0 );
    m_RestoreSpeed->SetValue(false);
    itemFlexGridSizer16->Add(m_RestoreSpeed, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Show = new wxCheckBox( m_IndexPanel, wxID_ANY, _("Show"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Show->SetValue(false);
    itemFlexGridSizer16->Add(m_Show, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_IndexPanel, _("Index"));

    m_General_Panel = new wxPanel( m_Notebook, ID_PANEL_GENERAL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer21 = new wxBoxSizer(wxHORIZONTAL);
    m_General_Panel->SetSizer(itemBoxSizer21);

    m_GeneralSizer = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer21->Add(m_GeneralSizer, 1, wxALIGN_TOP|wxALL, 0);
    wxFlexGridSizer* itemFlexGridSizer23 = new wxFlexGridSizer(0, 2, 0, 0);
    m_GeneralSizer->Add(itemFlexGridSizer23, 1, wxGROW|wxALL, 5);
    m_label_ID = new wxStaticText( m_General_Panel, wxID_STATIC_ID, _("ID:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer23->Add(m_label_ID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ID = new wxTextCtrl( m_General_Panel, ID_TEXTCTRL_ID, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer23->Add(m_ID, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labRoadname = new wxStaticText( m_General_Panel, wxID_ANY, _("Roadname"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer23->Add(m_labRoadname, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Roadname = new wxTextCtrl( m_General_Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer23->Add(m_Roadname, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labNumber = new wxStaticText( m_General_Panel, wxID_ANY, _("Number"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer23->Add(m_labNumber, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Number = new wxTextCtrl( m_General_Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer23->Add(m_Number, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Label_Description = new wxStaticText( m_General_Panel, wxID_STATIC_DESCRIPTION, _("Description:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer23->Add(m_Label_Description, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Description = new wxTextCtrl( m_General_Panel, ID_TEXTCTRL_DESCRIPTION, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer23->Add(m_Description, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labImageName = new wxStaticText( m_General_Panel, wxID_ANY, _("Image:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer23->Add(m_labImageName, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxFlexGridSizer* itemFlexGridSizer33 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer23->Add(itemFlexGridSizer33, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5);
    m_ImageName = new wxTextCtrl( m_General_Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer33->Add(m_ImageName, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ImageNr = new wxSpinCtrl( m_General_Panel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 256, 0 );
    itemFlexGridSizer33->Add(m_ImageNr, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxRIGHT, 5);

    itemFlexGridSizer33->AddGrowableCol(0);

    m_Label_Length = new wxStaticText( m_General_Panel, wxID_STATIC_LENGTH, _("Length:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer23->Add(m_Label_Length, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxFlexGridSizer* itemFlexGridSizer37 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer23->Add(itemFlexGridSizer37, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5);
    m_Length = new wxTextCtrl( m_General_Panel, ID_TEXTCTRL_LENGTH, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer37->Add(m_Length, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_NrCars = new wxSpinCtrl( m_General_Panel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 999, 0 );
    itemFlexGridSizer37->Add(m_NrCars, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxBOTTOM, 5);

    itemFlexGridSizer37->AddGrowableCol(0);

    m_Label_CatalogNr = new wxStaticText( m_General_Panel, wxID_STATIC_CATALOGNR, _("CatalogNr:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer23->Add(m_Label_CatalogNr, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_CatNr = new wxTextCtrl( m_General_Panel, ID_TEXTCTRL_CATALOGNR, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer23->Add(m_CatNr, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labDecType = new wxStaticText( m_General_Panel, wxID_ANY, _("Decoder type:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer23->Add(m_labDecType, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_DecoderType = new wxTextCtrl( m_General_Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer23->Add(m_DecoderType, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labDocumentation = new wxStaticText( m_General_Panel, wxID_ANY, _("Documentation"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer23->Add(m_labDocumentation, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxFlexGridSizer* itemFlexGridSizer45 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer23->Add(itemFlexGridSizer45, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5);
    m_Documentation = new wxTextCtrl( m_General_Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer45->Add(m_Documentation, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_ShowDoc = new wxButton( m_General_Panel, ID_BUTTON_SHOWDOC, _("..."), wxDefaultPosition, wxSize(40, -1), 0 );
    itemFlexGridSizer45->Add(m_ShowDoc, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxBOTTOM, 5);

    itemFlexGridSizer45->AddGrowableCol(0);

    m_Label_Purchased = new wxStaticText( m_General_Panel, wxID_STATIC_PURCHASED, _("Purchased:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer23->Add(m_Label_Purchased, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Purchased = new wxTextCtrl( m_General_Panel, ID_TEXTCTRL_PURCHASED, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer23->Add(m_Purchased, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    itemFlexGridSizer23->AddGrowableCol(1);

    wxFlexGridSizer* itemFlexGridSizer50 = new wxFlexGridSizer(0, 4, 0, 0);
    m_GeneralSizer->Add(itemFlexGridSizer50, 0, wxGROW, 5);
    m_labShortID = new wxStaticText( m_General_Panel, wxID_ANY, _("Short ID:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer50->Add(m_labShortID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ShortID = new wxTextCtrl( m_General_Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer50->Add(m_ShortID, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    m_labIdent = new wxStaticText( m_General_Panel, ID_STATICTEXT, _("Identifier:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer50->Add(m_labIdent, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Ident = new wxTextCtrl( m_General_Panel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer50->Add(m_Ident, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    m_Label_Runtime = new wxStaticText( m_General_Panel, ID_STATICTEXT1, _("Runtime:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer50->Add(m_Label_Runtime, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_RunTime = new wxTextCtrl( m_General_Panel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_CENTRE );
    m_RunTime->Enable(false);
    itemFlexGridSizer50->Add(m_RunTime, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    m_labMTime = new wxStaticText( m_General_Panel, wxID_ANY, _("M-Time:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer50->Add(m_labMTime, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_MTime = new wxTextCtrl( m_General_Panel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_MTime->Enable(false);
    itemFlexGridSizer50->Add(m_MTime, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    itemFlexGridSizer50->AddGrowableCol(3);

    wxFlexGridSizer* itemFlexGridSizer59 = new wxFlexGridSizer(0, 2, 0, 0);
    m_GeneralSizer->Add(itemFlexGridSizer59, 0, wxGROW, 5);
    m_UseShortID = new wxCheckBox( m_General_Panel, wxID_ANY, _("Use short ID"), wxDefaultPosition, wxDefaultSize, 0 );
    m_UseShortID->SetValue(false);
    itemFlexGridSizer59->Add(m_UseShortID, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_ThrottleNr = new wxSpinCtrl( m_General_Panel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 255, 0 );
    itemFlexGridSizer59->Add(m_ThrottleNr, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer62 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer21->Add(itemBoxSizer62, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer63 = new wxFlexGridSizer(0, 3, 0, 0);
    itemBoxSizer62->Add(itemFlexGridSizer63, 0, wxGROW, 5);
    m_Serviced = new wxButton( m_General_Panel, ID_LC_SERVICED, _("Serviced"), wxDefaultPosition, wxSize(100, 30), 0 );
    itemFlexGridSizer63->Add(m_Serviced, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labMInt = new wxStaticText( m_General_Panel, wxID_ANY, _("M-Int.:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer63->Add(m_labMInt, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_MInt = new wxSpinCtrl( m_General_Panel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 1000, 0 );
    itemFlexGridSizer63->Add(m_MInt, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine67 = new wxStaticLine( m_General_Panel, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer62->Add(itemStaticLine67, 0, wxGROW|wxALL, 5);

    m_labRemark = new wxStaticText( m_General_Panel, wxID_ANY, _("Remark"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer62->Add(m_labRemark, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_Remark = new wxTextCtrl( m_General_Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    itemBoxSizer62->Add(m_Remark, 1, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Actions = new wxButton( m_General_Panel, ID_LOC_ACTIONS, _("Actions..."), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer62->Add(m_Actions, 0, wxALIGN_LEFT|wxALL, 5);

    m_Notebook->AddPage(m_General_Panel, _("General"));

    m_Interface_Panel = new wxPanel( m_Notebook, ID_PANEL_INTERFACE, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer72 = new wxBoxSizer(wxVERTICAL);
    m_Interface_Panel->SetSizer(itemBoxSizer72);

    wxFlexGridSizer* itemFlexGridSizer73 = new wxFlexGridSizer(0, 4, 0, 0);
    itemBoxSizer72->Add(itemFlexGridSizer73, 0, wxGROW|wxALL, 5);
    m_Label_Address = new wxStaticText( m_Interface_Panel, wxID_STATIC_ADDRESS, _("Address:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer73->Add(m_Label_Address, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_Address = new wxTextCtrl( m_Interface_Panel, ID_TEXTCTRL_ADDRESS, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer73->Add(m_Address, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_Label_IID = new wxStaticText( m_Interface_Panel, wxID_STATIC_IID, _("IID:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer73->Add(m_Label_IID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_IID = new wxTextCtrl( m_Interface_Panel, ID_TEXTCTRL_IID, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer73->Add(m_IID, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    itemFlexGridSizer73->AddGrowableCol(3);

    wxFlexGridSizer* itemFlexGridSizer78 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer72->Add(itemFlexGridSizer78, 0, wxGROW|wxALL, 5);
    m_Label_Bus = new wxStaticText( m_Interface_Panel, wxID_STATIC_LC_BUS, _("Bus:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer78->Add(m_Label_Bus, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_Bus = new wxTextCtrl( m_Interface_Panel, ID_TEXTCTRL_LC_BUS, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer78->Add(m_Bus, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_Label_Protocol = new wxStaticText( m_Interface_Panel, wxID_STATIC_PROTOCOL, _("Protocol:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer78->Add(m_Label_Protocol, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_ProtocolStrings;
    m_ProtocolStrings.Add(_("ServerDefined"));
    m_ProtocolStrings.Add(_("Motorola"));
    m_ProtocolStrings.Add(_("NMRA-DCC"));
    m_ProtocolStrings.Add(_("NMRA-DCC long"));
    m_ProtocolStrings.Add(_("Analog"));
    m_ProtocolStrings.Add(_("Car"));
    m_ProtocolStrings.Add(_("SX1"));
    m_ProtocolStrings.Add(_("SX2"));
    m_Protocol = new wxChoice( m_Interface_Panel, ID_CHOICE_PROTOCOL, wxDefaultPosition, wxDefaultSize, m_ProtocolStrings, 0 );
    m_Protocol->SetStringSelection(_("ServerDefined"));
    itemFlexGridSizer78->Add(m_Protocol, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_LabelProtVersion = new wxStaticText( m_Interface_Panel, wxID_STATIC_PROT_VER, _("ProtVersion:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer78->Add(m_LabelProtVersion, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_ProtVersion = new wxTextCtrl( m_Interface_Panel, ID_TEXTCTRL_PROTVER, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer78->Add(m_ProtVersion, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_LabelSpeedSteps = new wxStaticText( m_Interface_Panel, wxID_STATIC_SPEEDSTEPS, _("Speed steps:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer78->Add(m_LabelSpeedSteps, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_SpeedSteps = new wxTextCtrl( m_Interface_Panel, ID_TEXTCTRL_SPEEDSTEPS, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer78->Add(m_SpeedSteps, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_LabelFunCnt = new wxStaticText( m_Interface_Panel, wxID_STATIC_FUNCNT, _("Functions:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer78->Add(m_LabelFunCnt, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_FunCnt = new wxTextCtrl( m_Interface_Panel, ID_TEXTCTRL_FUNCNT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer78->Add(m_FunCnt, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxFlexGridSizer* itemFlexGridSizer89 = new wxFlexGridSizer(0, 4, 0, 0);
    itemBoxSizer72->Add(itemFlexGridSizer89, 0, wxGROW|wxALL, 5);
    m_Label_V_min = new wxStaticText( m_Interface_Panel, wxID_STATIC_V_MIN, _("V_min:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer89->Add(m_Label_V_min, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 2);

    m_V_min = new wxTextCtrl( m_Interface_Panel, ID_TEXTCTRL_V_MIN, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_V_min->SetMaxLength(3);
    itemFlexGridSizer89->Add(m_V_min, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_Label_V_Rmin = new wxStaticText( m_Interface_Panel, wxID_ANY, _("V_Rmin:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer89->Add(m_Label_V_Rmin, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_V_Rmin = new wxTextCtrl( m_Interface_Panel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer89->Add(m_V_Rmin, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Label_V_mid = new wxStaticText( m_Interface_Panel, wxID_ANY, _("V_mid:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer89->Add(m_Label_V_mid, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 2);

    m_V_mid = new wxTextCtrl( m_Interface_Panel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_V_mid->SetMaxLength(3);
    itemFlexGridSizer89->Add(m_V_mid, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 2);

    m_Label_V_Rmid = new wxStaticText( m_Interface_Panel, wxID_ANY, _("V_Rmid:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer89->Add(m_Label_V_Rmid, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 5);

    m_V_Rmid = new wxTextCtrl( m_Interface_Panel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer89->Add(m_V_Rmid, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labVcru = new wxStaticText( m_Interface_Panel, wxID_ANY, _("V_cru:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer89->Add(m_labVcru, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 2);

    m_V_cru = new wxTextCtrl( m_Interface_Panel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer89->Add(m_V_cru, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 2);

    m_labVRcru = new wxStaticText( m_Interface_Panel, wxID_STATIC, _("V_Rcru:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer89->Add(m_labVRcru, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 5);

    m_V_Rcru = new wxTextCtrl( m_Interface_Panel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer89->Add(m_V_Rcru, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Label_V_max = new wxStaticText( m_Interface_Panel, wxID_STATIC_V_MAX, _("V_max:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer89->Add(m_Label_V_max, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 2);

    m_V_max = new wxTextCtrl( m_Interface_Panel, ID_TEXTCTRL_V_MAX, _("100"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_V_max->SetMaxLength(3);
    itemFlexGridSizer89->Add(m_V_max, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 2);

    m_Label_V_Rmax = new wxStaticText( m_Interface_Panel, wxID_ANY, _("V_Rmax:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer89->Add(m_Label_V_Rmax, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 5);

    m_V_Rmax = new wxTextCtrl( m_Interface_Panel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer89->Add(m_V_Rmax, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_LabelV_step = new wxStaticText( m_Interface_Panel, wxID_STATIC_LOC_V_STEP, _("V_step:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer89->Add(m_LabelV_step, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 2);

    m_Accel = new wxTextCtrl( m_Interface_Panel, ID_TEXTCTRL_LOC_ACCEL, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer89->Add(m_Accel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 2);

    m_LabelV_mode = new wxStaticText( m_Interface_Panel, wxID_STATIC_LOC_V_MODE, _("V_mode:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer89->Add(m_LabelV_mode, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 2);

    m_V_mode_percent = new wxCheckBox( m_Interface_Panel, ID_CHECKBOX_LOC_CONTROLLED, _("percent"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_V_mode_percent->SetValue(false);
    itemFlexGridSizer89->Add(m_V_mode_percent, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 2);

    wxBoxSizer* itemBoxSizer110 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer72->Add(itemBoxSizer110, 0, wxGROW|wxLEFT|wxRIGHT, 5);
    wxFlexGridSizer* itemFlexGridSizer111 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer110->Add(itemFlexGridSizer111, 0, wxALIGN_TOP|wxALL, 5);
    m_Label_Mass = new wxStaticText( m_Interface_Panel, wxID_STATIC_MASS, _("Mass:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer111->Add(m_Label_Mass, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 2);

    m_Mass = new wxTextCtrl( m_Interface_Panel, ID_TEXTCTRL_MASS, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_Mass->SetMaxLength(3);
    itemFlexGridSizer111->Add(m_Mass, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP, 2);

    m_LabelPlacing = new wxStaticText( m_Interface_Panel, wxID_STATIC_LOC_PLACING, _("Placing:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer111->Add(m_LabelPlacing, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 2);

    m_Placing = new wxCheckBox( m_Interface_Panel, ID_CHECKBOX_LOC_PLACING, _("default"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_Placing->SetValue(true);
    itemFlexGridSizer111->Add(m_Placing, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 2);

    m_labDirPause = new wxStaticText( m_Interface_Panel, wxID_ANY, _("Dir pause:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer111->Add(m_labDirPause, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_DirPause = new wxTextCtrl( m_Interface_Panel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_DirPause->SetMaxLength(4);
    itemFlexGridSizer111->Add(m_DirPause, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 2);

    itemFlexGridSizer111->AddGrowableCol(1);

    wxStaticLine* itemStaticLine118 = new wxStaticLine( m_Interface_Panel, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer110->Add(itemStaticLine118, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer119 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer110->Add(itemBoxSizer119, 0, wxALIGN_TOP|wxLEFT|wxRIGHT, 5);
    m_Regulated = new wxCheckBox( m_Interface_Panel, ID_LOC_REGULATED, _("regulated"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Regulated->SetValue(false);
    itemBoxSizer119->Add(m_Regulated, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_Info4Throttle = new wxCheckBox( m_Interface_Panel, wxID_ANY, _("info4throttle"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Info4Throttle->SetValue(false);
    itemBoxSizer119->Add(m_Info4Throttle, 0, wxALIGN_LEFT|wxALL, 5);

    m_Notebook->AddPage(m_Interface_Panel, _("Interface"));

    m_DetailsPanel = new wxPanel( m_Notebook, ID_PANEL_DETAILS, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer123 = new wxBoxSizer(wxVERTICAL);
    m_DetailsPanel->SetSizer(itemBoxSizer123);

    wxBoxSizer* itemBoxSizer124 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer123->Add(itemBoxSizer124, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer125 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer124->Add(itemFlexGridSizer125, 0, wxALIGN_TOP, 5);
    m_Label_Blockwait = new wxStaticText( m_DetailsPanel, wxID_STATIC_BLOCKWAIT, _("Block wait"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer125->Add(m_Label_Blockwait, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Blockwait = new wxTextCtrl( m_DetailsPanel, ID_TEXTCTRL_BLOCKWAIT, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer125->Add(m_Blockwait, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labEventTimer = new wxStaticText( m_DetailsPanel, wxID_ANY, _("Event timer"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer125->Add(m_labEventTimer, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_EventTimer = new wxTextCtrl( m_DetailsPanel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer125->Add(m_EventTimer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labEvtCorrection = new wxStaticText( m_DetailsPanel, wxID_ANY, _("Timer correction"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer125->Add(m_labEvtCorrection, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_EvtCorrection = new wxSpinCtrl( m_DetailsPanel, wxID_ANY, _T("100"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 1, 1000, 100 );
    itemFlexGridSizer125->Add(m_EvtCorrection, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labPriority = new wxStaticText( m_DetailsPanel, wxID_ANY, _("Priority"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer125->Add(m_labPriority, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Priority = new wxSpinCtrl( m_DetailsPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer125->Add(m_Priority, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labSwapTimer = new wxStaticText( m_DetailsPanel, wxID_ANY, _("Swap timer"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer125->Add(m_labSwapTimer, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_SwapTimer = new wxSpinCtrl( m_DetailsPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 5000, 0 );
    itemFlexGridSizer125->Add(m_SwapTimer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labStartupSchedule = new wxStaticText( m_DetailsPanel, wxID_ANY, _("Starup schedule"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer125->Add(m_labStartupSchedule, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxArrayString m_StartupScheduleStrings;
    m_StartupSchedule = new wxComboBox( m_DetailsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(160, -1), m_StartupScheduleStrings, wxCB_READONLY );
    itemFlexGridSizer125->Add(m_StartupSchedule, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    itemFlexGridSizer125->AddGrowableCol(1);

    wxStaticLine* itemStaticLine138 = new wxStaticLine( m_DetailsPanel, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer124->Add(itemStaticLine138, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer139 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer124->Add(itemBoxSizer139, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    m_Commuter = new wxCheckBox( m_DetailsPanel, ID_CHECKBOX_COMMUTER, _("Commuter train"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_Commuter->SetValue(false);
    itemBoxSizer139->Add(m_Commuter, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_SameDir = new wxCheckBox( m_DetailsPanel, ID_CHECKBOX_LC_SAMEDIR, _("Same direction"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SameDir->SetValue(false);
    itemBoxSizer139->Add(m_SameDir, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_OppositeDir = new wxCheckBox( m_DetailsPanel, wxID_ANY, _("Opposite destination"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OppositeDir->SetValue(false);
    itemBoxSizer139->Add(m_OppositeDir, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_ForceSameDir = new wxCheckBox( m_DetailsPanel, ID_CHECKBOX_LC_FORCESAMEDIR, _("Force same direction"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ForceSameDir->SetValue(false);
    itemBoxSizer139->Add(m_ForceSameDir, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_UseScheduleTime = new wxCheckBox( m_DetailsPanel, wxID_ANY, _("Use schedule departure time"), wxDefaultPosition, wxDefaultSize, 0 );
    m_UseScheduleTime->SetValue(false);
    itemBoxSizer139->Add(m_UseScheduleTime, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_secondNextBlock = new wxCheckBox( m_DetailsPanel, wxID_ANY, _("Reserve second next block"), wxDefaultPosition, wxDefaultSize, 0 );
    m_secondNextBlock->SetValue(false);
    itemBoxSizer139->Add(m_secondNextBlock, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_ShortIn = new wxCheckBox( m_DetailsPanel, wxID_ANY, _("Use shortin event"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ShortIn->SetValue(false);
    itemBoxSizer139->Add(m_ShortIn, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_InAtPre2In = new wxCheckBox( m_DetailsPanel, wxID_ANY, _("Stop at pre2in event"), wxDefaultPosition, wxDefaultSize, 0 );
    m_InAtPre2In->SetValue(false);
    itemBoxSizer139->Add(m_InAtPre2In, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_UseManualRoutes = new wxCheckBox( m_DetailsPanel, wxID_ANY, _("Use manual routes"), wxDefaultPosition, wxDefaultSize, 0 );
    m_UseManualRoutes->SetValue(false);
    itemBoxSizer139->Add(m_UseManualRoutes, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_UseOwnWaittime = new wxCheckBox( m_DetailsPanel, wxID_ANY, _("Use own wait time"), wxDefaultPosition, wxDefaultSize, 0 );
    m_UseOwnWaittime->SetValue(false);
    itemBoxSizer139->Add(m_UseOwnWaittime, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxArrayString m_EngineBoxStrings;
    m_EngineBoxStrings.Add(_("&Diesel"));
    m_EngineBoxStrings.Add(_("&Steam"));
    m_EngineBoxStrings.Add(_("&Electric"));
    m_EngineBoxStrings.Add(_("&Automobile"));
    m_EngineBox = new wxRadioBox( m_DetailsPanel, ID_RADIOBOX_ENGINE, _("Engine"), wxDefaultPosition, wxDefaultSize, m_EngineBoxStrings, 1, wxRA_SPECIFY_ROWS );
    m_EngineBox->SetSelection(0);
    itemBoxSizer123->Add(m_EngineBox, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, 5);

    wxArrayString m_CargoBoxStrings;
    m_CargoBoxStrings.Add(_("&None"));
    m_CargoBoxStrings.Add(_("&Goods"));
    m_CargoBoxStrings.Add(_("&Passengers"));
    m_CargoBoxStrings.Add(_("&Mixed"));
    m_CargoBoxStrings.Add(_("&Cleaning"));
    m_CargoBoxStrings.Add(_("&ICE"));
    m_CargoBoxStrings.Add(_("&Post"));
    m_CargoBoxStrings.Add(_("&light"));
    m_CargoBoxStrings.Add(_("&lightgoods"));
    m_CargoBoxStrings.Add(_("&Regional"));
    m_CargoBoxStrings.Add(_("&All"));
    m_CargoBox = new wxRadioBox( m_DetailsPanel, ID_RADIOBOX_CARGO, _("Cargo"), wxDefaultPosition, wxDefaultSize, m_CargoBoxStrings, 3, wxRA_SPECIFY_COLS );
    m_CargoBox->SetSelection(0);
    itemBoxSizer123->Add(m_CargoBox, 0, wxGROW|wxALL, 5);

    m_Notebook->AddPage(m_DetailsPanel, _("Details"));

    m_FunctionPanel = new wxPanel( m_Notebook, ID_PANEL_FUNCTIONS, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer153 = new wxBoxSizer(wxVERTICAL);
    m_FunctionPanel->SetSizer(itemBoxSizer153);

    wxFlexGridSizer* itemFlexGridSizer154 = new wxFlexGridSizer(0, 5, 0, 0);
    itemBoxSizer153->Add(itemFlexGridSizer154, 0, wxGROW|wxALL, 5);
    m_labF0Nr = new wxStaticText( m_FunctionPanel, wxID_ANY, _("#"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer154->Add(m_labF0Nr, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labF0Desc = new wxStaticText( m_FunctionPanel, wxID_STATIC, _("Description"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer154->Add(m_labF0Desc, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labF0Timer = new wxStaticText( m_FunctionPanel, wxID_ANY, _("Timer"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer154->Add(m_labF0Timer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labF0Events = new wxStaticText( m_FunctionPanel, wxID_ANY, _("Events"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer154->Add(m_labF0Events, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labF0Icon = new wxStaticText( m_FunctionPanel, wxID_ANY, _("Icon"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer154->Add(m_labF0Icon, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Label_f0 = new wxStaticText( m_FunctionPanel, wxID_ANY, _("f0"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer154->Add(m_Label_f0, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_f0 = new wxTextCtrl( m_FunctionPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer154->Add(m_f0, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_TimerF0 = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer154->Add(m_TimerF0, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_Button_f0 = new wxButton( m_FunctionPanel, ID_BUTTON_F0, _("..."), wxDefaultPosition, wxSize(40, 25), 0 );
    itemFlexGridSizer154->Add(m_Button_f0, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_F0Icon = new wxTextCtrl( m_FunctionPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer154->Add(m_F0Icon, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    itemFlexGridSizer154->AddGrowableCol(1);

    wxStaticLine* itemStaticLine165 = new wxStaticLine( m_FunctionPanel, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer153->Add(itemStaticLine165, 0, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer166 = new wxFlexGridSizer(0, 3, 0, 0);
    itemBoxSizer153->Add(itemFlexGridSizer166, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxTOP, 5);
    wxButton* itemButton167 = new wxButton( m_FunctionPanel, ID_LOC_FN_GROUP_PREV, _("< Fg"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer166->Add(itemButton167, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labFnGroup = new wxStaticText( m_FunctionPanel, wxID_ANY, _("1"), wxDefaultPosition, wxDefaultSize, 0 );
    m_labFnGroup->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, false, wxT("Sans")));
    itemFlexGridSizer166->Add(m_labFnGroup, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_FunctionGroup = new wxButton( m_FunctionPanel, ID_LOC_FN_GROUP, _("Fg >"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer166->Add(m_FunctionGroup, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer170 = new wxFlexGridSizer(0, 8, 0, 0);
    itemBoxSizer153->Add(itemFlexGridSizer170, 0, wxGROW|wxALL, 5);
    m_labFnNr = new wxStaticText( m_FunctionPanel, wxID_ANY, _("#"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer170->Add(m_labFnNr, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labFnDesc = new wxStaticText( m_FunctionPanel, wxID_ANY, _("Description"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer170->Add(m_labFnDesc, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labFnTimer = new wxStaticText( m_FunctionPanel, wxID_ANY, _("Timer"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer170->Add(m_labFnTimer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labFnEvents = new wxStaticText( m_FunctionPanel, wxID_ANY, _("Events"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer170->Add(m_labFnEvents, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labSound = new wxStaticText( m_FunctionPanel, wxID_ANY, _("Sound"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer170->Add(m_labSound, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labIcon = new wxStaticText( m_FunctionPanel, wxID_ANY, _("Icon"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer170->Add(m_labIcon, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labFnAddr = new wxStaticText( m_FunctionPanel, wxID_ANY, _("Address"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer170->Add(m_labFnAddr, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labAddrFn = new wxStaticText( m_FunctionPanel, wxID_ANY, _("fx"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer170->Add(m_labAddrFn, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Label_f1 = new wxStaticText( m_FunctionPanel, wxID_STATIC_F1, _("f1"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer170->Add(m_Label_f1, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 2);

    m_f1 = new wxTextCtrl( m_FunctionPanel, ID_TEXTCTRL_F1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer170->Add(m_f1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_TimerF1 = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer170->Add(m_TimerF1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_Button_f1 = new wxButton( m_FunctionPanel, ID_BUTTON_F1, _("..."), wxDefaultPosition, wxSize(40, 25), 0 );
    itemFlexGridSizer170->Add(m_Button_f1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_Sound1 = new wxTextCtrl( m_FunctionPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer170->Add(m_Sound1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Icon_f1 = new wxTextCtrl( m_FunctionPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer170->Add(m_Icon_f1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_F1Addr = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 65535, 0 );
    itemFlexGridSizer170->Add(m_F1Addr, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_F1AddrFn = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 28, 0 );
    itemFlexGridSizer170->Add(m_F1AddrFn, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Label_f2 = new wxStaticText( m_FunctionPanel, wxID_STATIC_F2, _("f2"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer170->Add(m_Label_f2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_f2 = new wxTextCtrl( m_FunctionPanel, ID_TEXTCTRL_F2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer170->Add(m_f2, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_TimerF2 = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer170->Add(m_TimerF2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_Button_f2 = new wxButton( m_FunctionPanel, ID_BUTTON_F2, _("..."), wxDefaultPosition, wxSize(40, 25), 0 );
    itemFlexGridSizer170->Add(m_Button_f2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_Sound2 = new wxTextCtrl( m_FunctionPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer170->Add(m_Sound2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Icon_f2 = new wxTextCtrl( m_FunctionPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer170->Add(m_Icon_f2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_F2Addr = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 65535, 0 );
    itemFlexGridSizer170->Add(m_F2Addr, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_F2AddrFn = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 28, 0 );
    itemFlexGridSizer170->Add(m_F2AddrFn, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Label_f3 = new wxStaticText( m_FunctionPanel, wxID_STATIC_F3, _("f3"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer170->Add(m_Label_f3, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_f3 = new wxTextCtrl( m_FunctionPanel, ID_TEXTCTRL_F3, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer170->Add(m_f3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_TimerF3 = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer170->Add(m_TimerF3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_Button_f3 = new wxButton( m_FunctionPanel, ID_BUTTON_F3, _("..."), wxDefaultPosition, wxSize(40, 25), 0 );
    itemFlexGridSizer170->Add(m_Button_f3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_Sound3 = new wxTextCtrl( m_FunctionPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer170->Add(m_Sound3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Icon_f3 = new wxTextCtrl( m_FunctionPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer170->Add(m_Icon_f3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_F3Addr = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 65535, 0 );
    itemFlexGridSizer170->Add(m_F3Addr, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_F3AddrFn = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 28, 0 );
    itemFlexGridSizer170->Add(m_F3AddrFn, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Label_f4 = new wxStaticText( m_FunctionPanel, wxID_STATIC_F4, _("f4"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer170->Add(m_Label_f4, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_f4 = new wxTextCtrl( m_FunctionPanel, ID_TEXTCTRL_F4, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer170->Add(m_f4, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_TimerF4 = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer170->Add(m_TimerF4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_Button_f4 = new wxButton( m_FunctionPanel, ID_BUTTON_F4, _("..."), wxDefaultPosition, wxSize(40, 25), 0 );
    itemFlexGridSizer170->Add(m_Button_f4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_Sound4 = new wxTextCtrl( m_FunctionPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer170->Add(m_Sound4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Icon_f4 = new wxTextCtrl( m_FunctionPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer170->Add(m_Icon_f4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_F4Addr = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 65535, 0 );
    itemFlexGridSizer170->Add(m_F4Addr, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_F4AddrFn = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 28, 0 );
    itemFlexGridSizer170->Add(m_F4AddrFn, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemFlexGridSizer170->AddGrowableCol(1);

    m_Notebook->AddPage(m_FunctionPanel, _("Functions"));

    m_ConsistsPanel = new wxPanel( m_Notebook, ID_PANEL_LOC_CONSIST, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer212 = new wxBoxSizer(wxVERTICAL);
    m_ConsistsPanel->SetSizer(itemBoxSizer212);

    m_labDetails = new wxStaticBox(m_ConsistsPanel, wxID_ANY, _("Details"));
    wxStaticBoxSizer* itemStaticBoxSizer213 = new wxStaticBoxSizer(m_labDetails, wxVERTICAL);
    itemBoxSizer212->Add(itemStaticBoxSizer213, 0, wxGROW|wxALL, 5);
    m_LightsOff = new wxCheckBox( m_ConsistsPanel, wxID_ANY, _("Keep lights off"), wxDefaultPosition, wxDefaultSize, 0 );
    m_LightsOff->SetValue(false);
    itemStaticBoxSizer213->Add(m_LightsOff, 0, wxALIGN_LEFT|wxALL, 5);

    m_SyncFun = new wxCheckBox( m_ConsistsPanel, wxID_ANY, _("Synchronise functions"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SyncFun->SetValue(false);
    itemStaticBoxSizer213->Add(m_SyncFun, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxFlexGridSizer* itemFlexGridSizer216 = new wxFlexGridSizer(0, 14, 0, 0);
    itemStaticBoxSizer213->Add(itemFlexGridSizer216, 0, wxALIGN_LEFT|wxLEFT, 25);
    wxStaticText* itemStaticText217 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("1"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText217, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText218 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("2"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText218, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText219 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("3"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText219, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText220 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("4"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText220, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText221 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("5"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText221, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText222 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("6"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText222, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText223 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("7"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText223, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText224 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("8"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText224, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText225 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("9"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText225, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText226 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("10"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText226, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText227 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("11"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText227, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText228 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("12"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText228, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText229 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("13"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText229, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText230 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("14"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText230, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync1 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync1->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync2 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync2->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync3 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync3->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync4 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync4->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync5 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync5->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync6 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync6->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync6, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync7 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync7->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync7, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync8 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync8->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync8, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync9 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync9->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync9, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync10 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync10->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync10, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync11 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync11->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync11, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync12 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync12->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync12, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync13 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync13->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync13, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync14 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync14->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync14, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText245 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("15"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText245, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText246 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("16"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText246, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText247 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("17"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText247, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText248 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("18"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText248, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText249 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("19"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText249, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText250 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("20"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText250, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText251 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("21"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText251, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText252 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("22"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText252, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText253 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("23"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText253, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText254 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("24"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText254, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText255 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("25"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText255, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText256 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("26"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText256, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText257 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("27"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText257, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText258 = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("28"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer216->Add(itemStaticText258, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync15 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync15->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync15, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync16 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync16->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync16, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync17 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync17->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync17, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync18 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync18->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync18, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync19 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync19->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync19, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync20 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync20->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync20, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync21 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync21->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync21, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync22 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync22->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync22, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync23 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync23->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync23, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync24 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync24->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync24, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync25 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync25->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync25, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync26 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync26->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync26, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync27 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync27->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync27, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_FSync28 = new wxCheckBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_FSync28->SetValue(false);
    itemFlexGridSizer216->Add(m_FSync28, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticLine* itemStaticLine273 = new wxStaticLine( m_ConsistsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer212->Add(itemStaticLine273, 0, wxGROW|wxALL, 5);

    m_labConsistList = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("Locomotives"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer212->Add(m_labConsistList, 0, wxALIGN_LEFT|wxALL, 5);

    wxArrayString m_ConsistListStrings;
    m_ConsistList = new wxListBox( m_ConsistsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_ConsistListStrings, wxLB_SINGLE );
    itemBoxSizer212->Add(m_ConsistList, 1, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer276 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer212->Add(itemFlexGridSizer276, 0, wxGROW|wxALL, 5);
    m_labConsistLocID = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("LocID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer276->Add(m_labConsistLocID, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_ConsistLocIDStrings;
    m_ConsistLocID = new wxComboBox( m_ConsistsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_ConsistLocIDStrings, wxCB_DROPDOWN );
    itemFlexGridSizer276->Add(m_ConsistLocID, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemFlexGridSizer276->AddGrowableCol(1);

    wxBoxSizer* itemBoxSizer279 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer212->Add(itemBoxSizer279, 0, wxGROW|wxALL, 5);
    m_AddConsistLoc = new wxButton( m_ConsistsPanel, ID_BUTTON_LOC_CONSIST_ADD, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer279->Add(m_AddConsistLoc, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_DeleteConsistLoc = new wxButton( m_ConsistsPanel, ID_BUTTON_LOC_CONSIST_DELETE, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer279->Add(m_DeleteConsistLoc, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_ConsistsPanel, _("Consists"));

    m_CVPanel = new wxPanel( m_Notebook, ID_PANEL_LOC_CV, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer283 = new wxBoxSizer(wxVERTICAL);
    m_CVPanel->SetSizer(itemBoxSizer283);

    m_CVList = new wxGrid( m_CVPanel, ID_GRID_LOC_CV, wxDefaultPosition, wxSize(200, 150), wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL|wxALWAYS_SHOW_SB );
    m_CVList->SetDefaultColSize(50);
    m_CVList->SetDefaultRowSize(25);
    m_CVList->SetColLabelSize(25);
    m_CVList->SetRowLabelSize(50);
    m_CVList->CreateGrid(1, 3, wxGrid::wxGridSelectCells);
    itemBoxSizer283->Add(m_CVList, 1, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer285 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer283->Add(itemFlexGridSizer285, 0, wxGROW|wxALL, 5);
    m_labCVDesc = new wxStaticText( m_CVPanel, wxID_ANY, _("Description"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer285->Add(m_labCVDesc, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_CVDescription = new wxTextCtrl( m_CVPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer285->Add(m_CVDescription, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_CVDescModify = new wxButton( m_CVPanel, ID_BUTTON_LC_CV_DESC, _("Modify"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer285->Add(m_CVDescModify, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    itemFlexGridSizer285->AddGrowableCol(1);

    m_Notebook->AddPage(m_CVPanel, _("CV's"));

    itemBoxSizer2->Add(m_Notebook, 1, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer289 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer289, 0, wxALIGN_RIGHT|wxALL, 5);
    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer289->AddButton(m_Cancel);

    m_Apply = new wxButton( itemDialog1, wxID_APPLY, _("&Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer289->AddButton(m_Apply);

    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer289->AddButton(m_OK);

    itemStdDialogButtonSizer289->Realize();

////@end LocDialog content construction
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_CANCEL
 */

void LocDialog::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}

/*!
 * Should we show tooltips?
 */

bool LocDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_F1
 */

void LocDialog::OnButtonF1Click( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  FunctionDialog*  funDlg = new FunctionDialog(this, m_Props, 1 + m_iFunGroup*4 );
  funDlg->ShowModal();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_F2
 */

void LocDialog::OnButtonF2Click( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  FunctionDialog*  funDlg = new FunctionDialog(this, m_Props, 2 + m_iFunGroup*4 );
  funDlg->ShowModal();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_F3
 */

void LocDialog::OnButtonF3Click( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  FunctionDialog*  funDlg = new FunctionDialog(this, m_Props, 3 + m_iFunGroup*4 );
  funDlg->ShowModal();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_F4
 */

void LocDialog::OnButtonF4Click( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  FunctionDialog*  funDlg = new FunctionDialog(this, m_Props, 4 + m_iFunGroup*4 );
  funDlg->ShowModal();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
 */

void LocDialog::OnApplyClick( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  if( !Evaluate() )
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
  InitIndex();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON3
 */

void LocDialog::OnButtonF0Click( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  FunctionDialog*  funDlg = new FunctionDialog(this, m_Props, 0 );
  funDlg->ShowModal();
}


/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX
 */

void LocDialog::OnOkClick( wxCommandEvent& event )
{
  if( m_bSave )
    OnApplyClick(event);
  EndModal( wxID_OK );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOC_NEW
 */

void LocDialog::OnButtonLocNewClick( wxCommandEvent& event )
{
  int i = findID("NEW");
  if( i == wxNOT_FOUND ) {
    iONode model = wxGetApp().getModel();
    if( model != NULL ) {
      iONode lclist = wPlan.getlclist( model );
      if( lclist == NULL ) {
        lclist = NodeOp.inst( wLocList.name(), model, ELEMENT_NODE );
        NodeOp.addChild( model, lclist );
      }
      if( lclist != NULL ) {
        iONode lc = NodeOp.inst( wLoc.name(), lclist, ELEMENT_NODE );
        NodeOp.addChild( lclist, lc );
        wLoc.setid( lc, "NEW" );
        appendItem(lc);
        setIDSelection(wItem.getid(lc));
        m_Props = lc;
        InitValues();
      }
    }
  }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_DELETE
 */

void LocDialog::OnButtonDeleteClick( wxCommandEvent& event )
{
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
    iONode lclist = wPlan.getlclist( model );
    if( lclist != NULL ) {
      NodeOp.removeChild( lclist, m_Props );
      m_Props = NULL;
    }
  }

  InitIndex();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BITMAPBUTTON
 */

void LocDialog::OnBitmapbuttonClick( wxCommandEvent& event )
{
  const char* imagepath = wGui.getimagepath( wxGetApp().getIni() );
  TraceOp.trc( "locdlg", TRCLEVEL_INFO, __LINE__, 9999, "imagepath = [%s]", imagepath );
  wxFileDialog* fdlg = new wxFileDialog(this, _T("Search loc image"),
      wxString(imagepath,wxConvUTF8), _T(""),
      _T("PNG files (*.png)|*.png|GIF files (*.gif)|*.gif|XPM files (*.xpm)|*.xpm"), wxFD_OPEN);
  if( fdlg->ShowModal() == wxID_OK ) {
    TraceOp.trc( "locdlg", TRCLEVEL_INFO, __LINE__, 9999, "Loading %s...", (const char*)fdlg->GetPath().mb_str(wxConvUTF8) );
    wxBitmapType bmptype = wxBITMAP_TYPE_XPM;
    if( StrOp.endsWithi( fdlg->GetPath().mb_str(wxConvUTF8), ".gif" ) )
      bmptype = wxBITMAP_TYPE_GIF;
    else if( StrOp.endsWithi( fdlg->GetPath().mb_str(wxConvUTF8), ".png" ) )
      bmptype = wxBITMAP_TYPE_PNG;
    m_LocImage->SetBitmapLabel( wxBitmap( fdlg->GetPath(), bmptype ) );
    m_LocImage->Refresh();
    wLoc.setimage( m_Props, FileOp.ripPath(fdlg->GetPath().mb_str(wxConvUTF8)) );
    m_ImageName->SetValue( wxString(wLoc.getimage( m_Props ),wxConvUTF8) );
  }
}



/*!
 * Get bitmap resources
 */

wxBitmap LocDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin LocDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end LocDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon LocDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin LocDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end LocDialog icon retrieval
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BITMAPBUTTON_LC_INDEX
 */

void LocDialog::EditFunction( int nr, wxString txt ) {
  TraceOp.trc( "locdlg", TRCLEVEL_INFO, __LINE__, 9999, "EditFunction [%d]", nr );
  Boolean funfound = False;
  iONode fundef = wLoc.getfundef( m_Props );
  while( fundef != NULL ) {
    if( wFunDef.getfn( fundef ) == nr ) {
      wFunDef.settext( fundef, txt.mb_str(wxConvUTF8) );
      funfound = True;
      break;
    }
    fundef = wLoc.nextfundef( m_Props, fundef );
  }
  if( !funfound ) {
    fundef = NodeOp.inst( wFunDef.name(), m_Props, ELEMENT_NODE );
    wFunDef.setfn( fundef, nr );
    wFunDef.settext( fundef, txt.mb_str(wxConvUTF8) );
    NodeOp.addChild( m_Props, fundef );
  }
}



/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOC_CONSIST_DELETE
 */

void LocDialog::OnButtonLocConsistDeleteClick( wxCommandEvent& event )
{
  int sel = m_ConsistList->GetSelection();
  if( sel != wxNOT_FOUND )
    m_ConsistList->Delete(sel);

}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOC_CONSIST_ADD
 */

void LocDialog::OnButtonLocConsistAddClick( wxCommandEvent& event )
{
  m_ConsistList->Append( m_ConsistLocID->GetStringSelection() );
}

void LocDialog::initCVDesc() {
  for( int i = 0; i < 256; i++ )
    m_CVDesc[i] = "";

  m_CVDesc[  1]  = "Primary Address";
  m_CVDesc[  2]  = "Vstart";
  m_CVDesc[  3]  = "Acceleration Rate";
  m_CVDesc[  4]  = "Deceleration Rate";
  m_CVDesc[  5]  = "Vhigh";
  m_CVDesc[  6]  = "Vmid";
  m_CVDesc[  7]  = "Manufacturer Version No.";
  m_CVDesc[  8]  = "Manufactured ID";
  m_CVDesc[  9]  = "Total PWM Period";
  m_CVDesc[ 10]  = "EMF Feedback Cutout";
  m_CVDesc[ 11]  = "Packet Time-Out Value";
  m_CVDesc[ 12]  = "Power Source Conversion";
  m_CVDesc[ 13]  = "Alternate Mode Function Status F1-F8";
  m_CVDesc[ 14]  = "Alternate Mode Fnc. Status FL,F9-F12";
  m_CVDesc[ 15]  = "Decoder Lock";
  m_CVDesc[ 16]  = "Decoder Lock";
  m_CVDesc[ 17]  = "Extended Address";
  m_CVDesc[ 18]  = "Extended Address";
  m_CVDesc[ 19]  = "Consist Address";
  m_CVDesc[ 20]  = "Reserved by NMRA for future use";
  m_CVDesc[ 21]  = "Consist Addr Active for F1-F8";
  m_CVDesc[ 22]  = "Consist Addr Active for FL-F9-F12";
  m_CVDesc[ 23]  = "Acceleration Adjustment";
  m_CVDesc[ 24]  = "Deceleration Adjustment";
  m_CVDesc[ 25]  = "Speed Table/Mid-range Cab Speed Step";
  m_CVDesc[ 26]  = "Reserved by NMRA for future use";
  m_CVDesc[ 27]  = "Decoder Automatic Stopping Configuration";
  m_CVDesc[ 28]  = "Bi-Directional Communication Configuration";
  m_CVDesc[ 29]  = "Configuration Data #1";
  m_CVDesc[ 30]  = "Error Information";
  m_CVDesc[ 31]  = "Index High Byte";
  m_CVDesc[ 32]  = "Index Low Byte";

}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LC_CV_DESC
 */

void LocDialog::OnButtonLcCvDescClick( wxCommandEvent& event )
{
  if( m_iSelectedCV == -1 )
    return;

  wxString str = m_CVList->GetCellValue(m_iSelectedCV, 0 );
  long cvnr = 0;
  str.ToLong(&cvnr);
  if( cvnr < 256 ) {
    iONode cv = m_CVNodes[cvnr];
    wCVByte.setdesc( cv, m_CVDescription->GetValue().mb_str(wxConvUTF8) );
    m_CVList->SetCellValue(m_iSelectedCV, 2, m_CVDescription->GetValue() );
  }
}


/*!
 * wxEVT_GRID_CELL_LEFT_CLICK event handler for ID_GRID_LOC_CV
 */

void LocDialog::OnCVCellLeftClick( wxGridEvent& event )
{
  m_iSelectedCV = event.GetRow();
  if( m_iSelectedCV == -1 )
    return;

  wxString str = m_CVList->GetCellValue( m_iSelectedCV, 2 );
  m_CVDescription->SetValue(str);
  m_CVDescription->Enable(true);
  m_CVDescModify->Enable(true);
  event.Skip();
}




/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_F8
 */

void LocDialog::OnButtonLocoDocClick( wxCommandEvent& event )
{
  doDoc( event, "locos" );
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_LOC_REGULATED
 */

void LocDialog::OnLocRegulatedClick( wxCommandEvent& event )
{
  int val = atoi( m_SpeedSteps->GetValue().mb_str(wxConvUTF8) );

  if( m_Regulated->IsChecked() ) {
    m_Accel->Enable( false );
  }
  else {
    m_Accel->Enable( true );
  }
}


/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL_SPEEDSTEPS
 */

void LocDialog::OnTextctrlSpeedstepsTextUpdated( wxCommandEvent& event )
{
  OnLocRegulatedClick(event);
}


/*!
 * wxEVT_COMMAND_TEXT_ENTER event handler for ID_TEXTCTRL_SPEEDSTEPS
 */

void LocDialog::OnTextctrlSpeedstepsEnter( wxCommandEvent& event )
{
  OnLocRegulatedClick(event);
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_F12
 */

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_LC_SERVICED
 */

void LocDialog::OnLcServicedClick( wxCommandEvent& event )
{
  m_MTime->SetValue(m_RunTime->GetValue());
  wLoc.setmtime( m_Props, wLoc.getruntime( m_Props ) );
  m_Serviced->SetBackgroundColour( Base::getGreen() );
}





/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_IMPORTLOCOS
 */

void LocDialog::OnButtonImportlocosClick( wxCommandEvent& event )
{
  wxString ms_FileExt = wxGetApp().getMsg("planfiles");
  const char* l_openpath = wGui.getopenpath( wxGetApp().getIni() );
  wxFileDialog* fdlg = new wxFileDialog(this, wxGetApp().getMenu("openplanfile"), wxString(l_openpath,wxConvUTF8) , _T(""), ms_FileExt, wxFD_OPEN);
  if( fdlg->ShowModal() == wxID_OK ) {

    wGui.setopenpath( wxGetApp().getIni(), fdlg->GetPath().mb_str(wxConvUTF8) );
    // strip filename:
    wGui.setopenpath( wxGetApp().getIni(), FileOp.getPath(wGui.getopenpath( wxGetApp().getIni() ) ) );

    if( fdlg->GetPath().Len() > 0 && FileOp.exist(fdlg->GetPath().mb_str(wxConvUTF8)) ) {
      iOFile f = FileOp.inst( fdlg->GetPath().mb_str(wxConvUTF8), OPEN_READONLY );
      char* buffer = (char*)allocMem( FileOp.size( f ) +1 );
      FileOp.read( f, buffer, FileOp.size( f ) );
      FileOp.base.del( f );
      iODoc doc = DocOp.parse( buffer );
      if( doc != NULL ) {
        iONode plan = DocOp.getRootNode( doc );
        DocOp.base.del( doc );
        TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Plan [%s] is successfully parsed!", (const char*)fdlg->GetPath().mb_str(wxConvUTF8) );

        /* TODO: read all loco's and add them to the list */
        iONode list = NodeOp.findNode( plan, wLocList.name() );

        if( list != NULL ) {
          int i = 0;
          int cnt = NodeOp.getChildCnt( list );
          for( i = 0; i < cnt; i++ ) {
            m_Props = (iONode)NodeOp.base.clone( NodeOp.getChild( list, i ) );

            iONode model = wxGetApp().getModel();
            if( model != NULL ) {
              iONode lclist = wPlan.getlclist( model );
              if( lclist == NULL ) {
                lclist = NodeOp.inst( wLocList.name(), model, ELEMENT_NODE );
                NodeOp.addChild( model, lclist );
              }
              if( lclist != NULL ) {
                iONode lc = NodeOp.inst( wLoc.name(), lclist, ELEMENT_NODE );
                NodeOp.addChild( lclist, m_Props );
                InitValues();
                OnApplyClick(event);
              }
            }
          }
        }


        NodeOp.base.del( plan );
      }
      else {
        TraceOp.trc( "frame", TRCLEVEL_EXCEPTION, __LINE__, 9999, "Plan [%s] is not parseable!", (const char*)fdlg->GetPath().mb_str(wxConvUTF8) );
      }
    }

  }
  fdlg->Destroy();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_LOC_FN_GROUP
 */

void LocDialog::OnLocFnGroupClick( wxCommandEvent& event )
{
  evaluateFunctions();
  m_iFunGroup++;
  if( m_iFunGroup > 6 )
    m_iFunGroup = 0;

  initFunctions();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_LOC_FN_GROUP_PREV
 */

void LocDialog::OnLocFnGroupPrevClick( wxCommandEvent& event )
{
  evaluateFunctions();
  if( m_iFunGroup > 0 )
    m_iFunGroup--;

  initFunctions();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_LOC_COPY
 */

void LocDialog::OnLocCopyClick( wxCommandEvent& event )
{
  if( m_Props != NULL ) {
    iONode model = wxGetApp().getModel();
    if( model != NULL ) {
      iONode lclist = wPlan.getlclist( model );
      if( lclist == NULL ) {
        lclist = NodeOp.inst( wLocList.name(), model, ELEMENT_NODE );
        NodeOp.addChild( model, lclist );
      }

      if( lclist != NULL ) {
        iONode lccopy = (iONode)NodeOp.base.clone( m_Props );
        char* id = StrOp.fmt( "%s (copy)", wLoc.getid(lccopy));
        wLoc.setid(lccopy, id);
        wLoc.setruntime(lccopy, 0 );
        wLoc.setmtime(lccopy, 0 );
        StrOp.free(id);
        NodeOp.addChild( lclist, lccopy );
        InitIndex();
      }

    }
  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_LOC_ACTIONS
 */

void LocDialog::OnLocActionsClick( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  ActionsCtrlDlg*  dlg = new ActionsCtrlDlg(this, m_Props );

  if( wxID_OK == dlg->ShowModal() ) {
    // TODO: inform
  }

  dlg->Destroy();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SHOWDOC
 */

void LocDialog::OnButtonShowdocClick( wxCommandEvent& event )
{
  wxLaunchDefaultBrowser(m_Documentation->GetValue(), wxBROWSER_NEW_WINDOW );
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for wxID_ANY
 */

void LocDialog::OnShowClick( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  if( !Evaluate() )
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
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for wxID_ANY
 */

void LocDialog::OnRestoreSpeedClick( wxCommandEvent& event )
{
  OnShowClick(event);
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for wxID_ANY
 */

void LocDialog::OnRestoreFxClick( wxCommandEvent& event )
{
  OnShowClick(event);
}


/*!
 * wxEVT_COMMAND_LIST_ITEM_SELECTED event handler for ID_LISTCTRLINDEX
 */

void LocDialog::OnListctrlindexLcSelected( wxListEvent& event )
{
  m_Props = getSelection(event.GetIndex());
  InitValues();
}


/*!
 * wxEVT_COMMAND_LIST_COL_CLICK event handler for ID_LISTCTRLINDEX
 */

void LocDialog::OnListctrlindexLcColLeftClick( wxListEvent& event )
{
  sortOnColumn(event.GetColumn());
}

