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
////@end includes

#include "locdialog.h"
#include "fundialog.h"

#include "rocgui/public/guiapp.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/LocList.h"
#include "rocrail/wrapper/public/FunDef.h"
#include "rocgui/wrapper/public/Gui.h"
#include "rocrail/wrapper/public/CVByte.h"


#include "rocs/public/trace.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"

#include "rocgui/xpm/nopict.xpm"

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

    EVT_LISTBOX( ID_LISTBOX, LocDialog::OnListboxSelected )

    EVT_BUTTON( ID_BUTTON_LOC_NEW, LocDialog::OnButtonLocNewClick )

    EVT_BUTTON( ID_BUTTON_DELETE, LocDialog::OnButtonDeleteClick )

    EVT_BUTTON( ID_BUTTON_LOCO_DOC, LocDialog::OnButtonLocoDocClick )

    EVT_TEXT( ID_TEXTCTRL_SPEEDSTEPS, LocDialog::OnTextctrlSpeedstepsTextUpdated )
    EVT_TEXT_ENTER( ID_TEXTCTRL_SPEEDSTEPS, LocDialog::OnTextctrlSpeedstepsEnter )

    EVT_CHECKBOX( ID_LOC_REGULATED, LocDialog::OnLocRegulatedClick )

    EVT_BUTTON( ID_BUTTON_F1, LocDialog::OnButtonF1Click )

    EVT_BUTTON( ID_BUTTON_F2, LocDialog::OnButtonF2Click )

    EVT_BUTTON( ID_BUTTON_F3, LocDialog::OnButtonF3Click )

    EVT_BUTTON( ID_BUTTON_F4, LocDialog::OnButtonF4Click )

    EVT_BUTTON( ID_BUTTON_F5, LocDialog::OnButtonF5Click )

    EVT_BUTTON( ID_BUTTON_F6, LocDialog::OnButtonF6Click )

    EVT_BUTTON( ID_BUTTON_F7, LocDialog::OnButtonF7Click )

    EVT_BUTTON( ID_BUTTON_F8, LocDialog::OnButtonF8Click )

    EVT_BUTTON( ID_BUTTON_F9, LocDialog::OnButtonF9Click )

    EVT_BUTTON( ID_BUTTON_F10, LocDialog::OnButtonF10Click )

    EVT_BUTTON( ID_BUTTON_F11, LocDialog::OnButtonF11Click )

    EVT_BUTTON( ID_BUTTON_F12, LocDialog::OnButtonF12Click )

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

  initLabels();
  initCVDesc();

  InitIndex();

  if( m_Props != NULL ) {
    InitValues();
    //m_Notebook->SetSelection( 1 );
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_PANEL_GENERAL );
    wxPostEvent( this, event );
  }
  else if( m_List->GetCount() > 0 ) {
    //m_List->Select( 0 );
  }

  m_IndexPanel->GetSizer()->Layout();
  m_General_Panel->GetSizer()->Layout();
  m_Interface_Panel->GetSizer()->Layout();
  m_DetailsPanel->GetSizer()->Layout();
  m_FunctionPanel->GetSizer()->Layout();

  m_Notebook->Fit();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  m_List->SetFocus();
}


void LocDialog::SelectNext() {
  int cnt = m_List->GetCount();
  int sel = m_List->GetSelection();
  if( sel == wxNOT_FOUND )
    m_List->Select( 0 );
  else if( cnt > 0 && sel + 1 < cnt ) {
    m_List->Select( sel + 1 );
    m_List->SetFirstItem( sel + 1 );
  }

  m_Props = wxGetApp().getFrame()->findLoc( m_List->GetStringSelection().mb_str(wxConvUTF8) );
  InitValues();
}


void LocDialog::SelectPrev() {
  int cnt = m_List->GetCount();
  int sel = m_List->GetSelection();
  if( sel == wxNOT_FOUND )
    m_List->Select( 0 );
  else if( sel > 0 ) {
    m_List->Select( sel - 1 );
    m_List->SetFirstItem( sel - 1 );
  }

  m_Props = wxGetApp().getFrame()->findLoc( m_List->GetStringSelection().mb_str(wxConvUTF8) );
  InitValues();
}


void LocDialog::OnSelectPage(wxCommandEvent& event) {
  m_Notebook->SetSelection( 1 );
}


void LocDialog::initLabels() {
  m_Notebook->SetPageText( 0, wxGetApp().getMsg( "index" ) );
  m_Notebook->SetPageText( 1, wxGetApp().getMsg( "general" ) );
  m_Notebook->SetPageText( 2, wxGetApp().getMsg( "interface" ) );
  m_Notebook->SetPageText( 3, wxGetApp().getMsg( "details" ) );
  m_Notebook->SetPageText( 4, wxGetApp().getMsg( "functions" ) );
  m_Notebook->SetPageText( 5, wxGetApp().getMsg( "consist" ) );

  // Index
  m_New->SetLabel( wxGetApp().getMsg( "new" ) );
  m_Delete->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_Doc->SetLabel( wxGetApp().getMsg( "doc_report" ) );


  // General
  m_label_ID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_labShortID->SetLabel( wxGetApp().getMsg( "shortid" ) );
  m_Label_Description->SetLabel( wxGetApp().getMsg( "description" ) );
  m_Label_Length->SetLabel( wxGetApp().getMsg( "length" ) );
  m_Label_CatalogNr->SetLabel( wxGetApp().getMsg( "catnr" ) );
  m_Label_Purchased->SetLabel( wxGetApp().getMsg( "purchased" ) );
  m_Label_Runtime->SetLabel( wxGetApp().getMsg( "runtime" ) );
  m_labIdent->SetLabel( wxGetApp().getMsg( "identifier" ) );
  m_Ident->SetToolTip( wxGetApp().getTip( "identifier" ) );
  m_labRemark->SetLabel( wxGetApp().getMsg( "remark" ) );
  m_labImageName->SetLabel( wxGetApp().getMsg( "image" ) );
  m_Show->SetLabel( wxGetApp().getMsg( "show" ) );

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
  m_Label_V_max->SetLabel( wxGetApp().getMsg( "v_max" ) );

  m_Label_V_Rmin->SetLabel( wxGetApp().getMsg( "v_rmin" ) );
  m_Label_V_Rmid->SetLabel( wxGetApp().getMsg( "v_rmid" ) );
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

  // Details
  m_Label_Blockwait->SetLabel( wxGetApp().getMsg( "blockwait" ) );
  m_labEventTimer->SetLabel( wxGetApp().getMsg( "eventtimer" ) );
  m_UseScheduleTime->SetLabel( wxGetApp().getMsg( "usescheduletime" ) );
  m_Commuter->SetLabel( wxGetApp().getMsg( "commuter" ) );
  m_SameDir->SetLabel( wxGetApp().getMsg( "samedir" ) );
  m_SameDir->SetToolTip( wxGetApp().getTip( "samedir" ) );
  m_OppositeDir->SetLabel( wxGetApp().getMsg( "oppositedir" ) );
  m_OppositeDir->SetToolTip( wxGetApp().getTip( "oppositedir" ) );
  m_EngineBox->SetLabel( wxGetApp().getMsg( "engine" ) );
  m_EngineBox->SetString( 0, wxGetApp().getMsg( "diesel" ) );
  m_EngineBox->SetString( 1, wxGetApp().getMsg( "steam" ) );
  m_EngineBox->SetString( 2, wxGetApp().getMsg( "electric" ) );
  m_CargoBox->SetLabel( wxGetApp().getMsg( "cargo" ) );
  m_CargoBox->SetString( 0, wxGetApp().getMsg( "none" ) );
  m_CargoBox->SetString( 1, wxGetApp().getMsg( "goods" ) );
  m_CargoBox->SetString( 2, wxGetApp().getMsg( "local" ) );
  m_CargoBox->SetString( 3, wxGetApp().getMsg( "mixed" ) );
  m_CargoBox->SetString( 4, wxGetApp().getMsg( "cleaning" ) );
  m_CargoBox->SetString( 5, wxGetApp().getMsg( "ice" ) );
  m_secondNextBlock->SetLabel( wxGetApp().getMsg( "secondnextblock" ) );

  // Functions
  m_Lights->SetLabel( wxGetApp().getMsg( "lights" ) );

  // Consist
  m_labDetails->SetLabel( wxGetApp().getMsg( "details" ) );
  m_LightsOff->SetLabel( wxGetApp().getMsg( "lightsoff" ) );
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

/* comparator for sorting by id: */
static int __sortID(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = wItem.getid( a );
    const char* idB = wItem.getid( b );
    return strcmp( idA, idB );
}


void LocDialog::InitIndex() {
  TraceOp.trc( "locdlg", TRCLEVEL_INFO, __LINE__, 9999, "InitIndex" );
  iONode l_Props = m_Props;

  m_List->Clear();
  m_ConsistLocID->Clear();

  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode lclist = wPlan.getlclist( model );
    if( lclist != NULL ) {
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
        m_List->Append( wxString(id,wxConvUTF8), lc );
        m_ConsistLocID->Append( wxString(id,wxConvUTF8) );
      }
      /* clean up the temp. list */
      ListOp.base.del(list);

      if( l_Props != NULL ) {
        m_List->SetStringSelection( wxString(wLoc.getid( l_Props ),wxConvUTF8) );
        m_List->SetFirstItem( wxString(wLoc.getid( l_Props ),wxConvUTF8) );
        m_Props = l_Props;
      }
      else
        TraceOp.trc( "locdlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection" );
    }

  }
}


void LocDialog::InitValues() {
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
    static char pixpath[256];
    StrOp.fmtb( pixpath, "%s%c%s", imagepath, SystemOp.getFileSeparator(), FileOp.ripPath( wLoc.getimage( m_Props ) ) );

    if( FileOp.exist(pixpath)) {
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

  m_Show->SetValue(wLoc.isshow( m_Props ));
  m_ID->SetValue( wxString(wLoc.getid( m_Props ),wxConvUTF8) );
  m_ShortID->SetValue( wxString(wLoc.getshortid( m_Props ),wxConvUTF8) );
  m_Description->SetValue( wxString(wLoc.getdesc( m_Props ),wxConvUTF8) );
  m_ImageName->SetValue( wxString(wLoc.getimage( m_Props ),wxConvUTF8) );
  m_Remark->SetValue( wxString(wLoc.getremark( m_Props ),wxConvUTF8) );
  char* str = StrOp.fmt( "%d", wLoc.getlen( m_Props ) );
  m_Length->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  m_CatNr->SetValue( wxString(wLoc.getcatnr( m_Props ),wxConvUTF8) );
  m_Purchased->SetValue( wxString(wLoc.getpurchased( m_Props ),wxConvUTF8) );
  int runtime = wLoc.getruntime( m_Props );
  str = StrOp.fmt( "%d:%02d.%02d",
      runtime/3600,
      (runtime%3600)/60,
      (runtime%3600)%60 );
  m_RunTime->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wLoc.getidentifier( m_Props ) );
  m_Ident->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );

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
  str = StrOp.fmt( "%d", wLoc.getV_max( m_Props ) );
  m_V_max->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wLoc.getV_Rmin( m_Props ) );
  m_V_Rmin->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wLoc.getV_Rmid( m_Props ) );
  m_V_Rmid->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wLoc.getV_Rmax( m_Props ) );
  m_V_Rmax->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wLoc.getmass( m_Props ) );
  m_Mass->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );

  m_V_mode_percent->SetValue( StrOp.equals( wLoc.V_mode_percent, wLoc.getV_mode( m_Props ) ) ? true:false );

  m_Placing->SetValue( wLoc.isplacing( m_Props ) ? true:false );
  m_Regulated->SetValue( wLoc.isregulated( m_Props ) ? true:false );

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
  m_UseScheduleTime->SetValue( wLoc.isusescheduletime( m_Props ) ? true:false );
  m_Commuter->SetValue( wLoc.iscommuter( m_Props ) ? true:false );
  m_SameDir->SetValue( wLoc.istrysamedir( m_Props ) ? true:false );
  m_OppositeDir->SetValue( wLoc.istryoppositedir( m_Props ) ? true:false );
  int engine = 0;
  if( StrOp.equals( wLoc.engine_diesel, wLoc.getengine( m_Props ) ) )
    engine = 0;
  else if( StrOp.equals( wLoc.engine_steam, wLoc.getengine( m_Props ) ) )
    engine = 1;
  else if( StrOp.equals( wLoc.engine_electric, wLoc.getengine( m_Props ) ) )
    engine = 2;
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
  m_CargoBox->SetSelection( cargo );
  m_secondNextBlock->SetValue( wLoc.issecondnextblock( m_Props ) );

  // Init Functions
  TraceOp.trc( "locdlg", TRCLEVEL_INFO, __LINE__, 9999, "Init functions %s", wLoc.getid( m_Props ) );
  iONode fundef = wLoc.getfundef( m_Props );

  // flags for functions
  int funset[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
  wxTextCtrl* l_fx[] = {m_f1,m_f2,m_f3,m_f4,m_f5,m_f6,m_f7,m_f8,m_f9,m_f10,m_f11,m_f12};
  wxSpinCtrl* l_timer[] = {m_TimerF1,m_TimerF2,m_TimerF3,m_TimerF4,m_TimerF5,m_TimerF6,m_TimerF7,m_TimerF8,m_TimerF9,m_TimerF10,m_TimerF11,m_TimerF12};

  while( fundef != NULL ) {
    wxString fntxt( wFunDef.gettext( fundef ),wxConvUTF8 );
    int funnr = wFunDef.getfn( fundef );
    TraceOp.trc( "locdlg", TRCLEVEL_INFO, __LINE__, 9999,
        "function[%d] name[%s]", funnr, wFunDef.gettext( fundef ) );

    // flag which functions are set
    if( funnr > 0 && funnr <= 12 ) {
      funset[funnr-1] = 1;
      l_fx[funnr-1]->SetValue( fntxt );
      l_timer[funnr-1]->SetValue( wFunDef.gettimer(fundef) );
    }

    fundef = wLoc.nextfundef( m_Props, fundef );
  }

  // do not like this code...
  for( int i = 0; i < 12; i++ ) {
    if( funset[i] == 0 ) {
      l_fx[i]->SetValue( _T("") );
    }
  }

  m_Lights->SetValue( wLoc.isfn( m_Props ) ? true:false );

  // Consist
  m_LightsOff->SetValue( wLoc.isconsist_lightsoff( m_Props ) ? true:false );
  m_ConsistList->Clear();
  iOStrTok  consist = StrTokOp.inst( wLoc.getconsist ( m_Props ), ',' );
  while( StrTokOp.hasMoreTokens( consist ) ) {
    const char* tok = StrTokOp.nextToken( consist );
    m_ConsistList->Append( wxString(tok,wxConvUTF8) );
  };
  StrTokOp.base.del( consist );

  // CV's
  m_CVList->DeleteRows( 0, m_CVList->GetNumberRows() );
  iONode cv = wLoc.getcvbyte( m_Props );
  while( cv != NULL ) {
    char* cvnr = StrOp.fmt( "%d", wCVByte.getnr( cv ) );
    char* cvval = StrOp.fmt( "%d", wCVByte.getvalue( cv ) );
    const char* cvdesc = wCVByte.getdesc(cv);
    if( wCVByte.getnr( cv ) < 1 || wCVByte.getnr( cv ) > 256 )
      continue;
    m_CVNodes[wCVByte.getnr( cv )] = cv;
    m_CVList->AppendRows();
    int row = m_CVList->GetNumberRows()-1;
    m_CVList->SetCellValue(row, 0, wxString(cvnr,wxConvUTF8) );
    m_CVList->SetCellValue(row, 1, wxString(cvval,wxConvUTF8) );
    if( cvdesc != NULL && StrOp.len(cvdesc) > 0 )
      m_CVList->SetCellValue(row, 2, wxString(cvdesc,wxConvUTF8) );
    else
      m_CVList->SetCellValue(row, 2, wxString(m_CVDesc[wCVByte.getnr( cv )],wxConvUTF8) );
    m_CVList->SetReadOnly( row, 0, true );
    m_CVList->SetReadOnly( row, 1, true );
    m_CVList->SetReadOnly( row, 2, true );
    StrOp.free( cvnr );
    StrOp.free( cvval );

    cv = wLoc.nextcvbyte( m_Props, cv );
  };
  m_CVList->AutoSize();


}


void LocDialog::Evaluate() {
  if( m_Props == NULL )
    return;

  TraceOp.trc( "locdlg", TRCLEVEL_INFO, __LINE__, 9999, "Evaluate %s", wLoc.getid( m_Props ) );

  // evaluate General
  wItem.setprev_id( m_Props, wItem.getid(m_Props) );
  wLoc.setid( m_Props, m_ID->GetValue().mb_str(wxConvUTF8) );
  wLoc.setshortid( m_Props, m_ShortID->GetValue().mb_str(wxConvUTF8) );
  wLoc.setdesc( m_Props, m_Description->GetValue().mb_str(wxConvUTF8) );
  wLoc.setimage( m_Props, m_ImageName->GetValue().mb_str(wxConvUTF8) );
  wLoc.setremark( m_Props, m_Remark->GetValue().mb_str(wxConvUTF8) );
  int val = atoi( m_Length->GetValue().mb_str(wxConvUTF8) );
  wLoc.setlen( m_Props, val );
  wLoc.setcatnr( m_Props, m_CatNr->GetValue().mb_str(wxConvUTF8) );
  wLoc.setpurchased( m_Props, m_Purchased->GetValue().mb_str(wxConvUTF8) );
  val = atoi( m_RunTime->GetValue().mb_str(wxConvUTF8) );
  wLoc.setruntime( m_Props, val );
  val = atoi( m_Ident->GetValue().mb_str(wxConvUTF8) );
  wLoc.setidentifier( m_Props, val );
  wLoc.setshow( m_Props, m_Show->IsChecked()?True:False);

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
  val = atoi( m_V_max->GetValue().mb_str(wxConvUTF8) );
  wLoc.setV_max( m_Props, val );
  val = atoi( m_V_Rmin->GetValue().mb_str(wxConvUTF8) );
  wLoc.setV_Rmin( m_Props, val );
  val = atoi( m_V_Rmid->GetValue().mb_str(wxConvUTF8) );
  wLoc.setV_Rmid( m_Props, val );
  val = atoi( m_V_Rmax->GetValue().mb_str(wxConvUTF8) );
  wLoc.setV_Rmax( m_Props, val );
  val = atoi( m_Accel->GetValue().mb_str(wxConvUTF8) );
  wLoc.setV_step( m_Props, val );
  val = atoi( m_Mass->GetValue().mb_str(wxConvUTF8) );
  wLoc.setmass( m_Props, val );
  wLoc.setV_mode( m_Props, m_V_mode_percent->GetValue() ? wLoc.V_mode_percent:wLoc.V_mode_kmh );
  wLoc.setplacing( m_Props, m_Placing->GetValue() ? True:False );
  wLoc.setregulated( m_Props, m_Regulated->GetValue() ? True:False );

  val = atoi( m_DirPause->GetValue().mb_str(wxConvUTF8) );
  wLoc.setdirpause( m_Props, val );

  // evaluate Details
  val = atoi( m_Blockwait->GetValue().mb_str(wxConvUTF8) );
  wLoc.setblockwaittime( m_Props, val );
  val = atoi( m_EventTimer->GetValue().mb_str(wxConvUTF8) );
  wLoc.setevttimer( m_Props, val );
  wLoc.setusescheduletime( m_Props, m_UseScheduleTime->GetValue() ? True:False );
  wLoc.setcommuter( m_Props, m_Commuter->GetValue() ? True:False );
  wLoc.settrysamedir( m_Props, m_SameDir->GetValue() ? True:False );
  wLoc.settryoppositedir( m_Props, m_OppositeDir->GetValue() ? True:False );

  int engine = m_EngineBox->GetSelection();
  if( engine == 0 )
    wLoc.setengine( m_Props, wLoc.engine_diesel );
  else if( engine == 1 )
    wLoc.setengine( m_Props, wLoc.engine_steam );
  else if( engine == 2 )
    wLoc.setengine( m_Props, wLoc.engine_electric );

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

  wLoc.setsecondnextblock( m_Props, m_secondNextBlock->IsChecked() ? True:False );


  // evaluate Functions
  wxTextCtrl* l_fx[] = {m_f1,m_f2,m_f3,m_f4,m_f5,m_f6,m_f7,m_f8,m_f9,m_f10,m_f11,m_f12};
  wxSpinCtrl* l_timer[] = {m_TimerF1,m_TimerF2,m_TimerF3,m_TimerF4,m_TimerF5,m_TimerF6,m_TimerF7,m_TimerF8,m_TimerF9,m_TimerF10,m_TimerF11,m_TimerF12};
  int function = 0;
  for( int i = 0; i < 12; i++ ) {
    if( l_fx[i]->GetValue().Length() > 0 )
      function |= (1 << i);
  }


  iONode fundef = wLoc.getfundef( m_Props );
  while( fundef != NULL ) {
    wxString fntxt( wFunDef.gettext( fundef ),wxConvUTF8 );
    switch( wFunDef.getfn( fundef ) ) {
      case 1 :
        wFunDef.settext( fundef, m_f1->GetValue().mb_str(wxConvUTF8)  );
        wFunDef.settimer( fundef, m_TimerF1->GetValue() );
        function = (function & (~0x0001));
        break;
      case 2 :
        wFunDef.settext( fundef, m_f2->GetValue().mb_str(wxConvUTF8)  );
        wFunDef.settimer( fundef, m_TimerF2->GetValue() );
        function = (function & (~0x0002));
        break;
      case 3 :
        wFunDef.settext( fundef, m_f3->GetValue().mb_str(wxConvUTF8)  );
        wFunDef.settimer( fundef, m_TimerF3->GetValue() );
        function = (function & (~0x0004));
        break;
      case 4 :
        wFunDef.settext( fundef, m_f4->GetValue().mb_str(wxConvUTF8)  );
        wFunDef.settimer( fundef, m_TimerF4->GetValue() );
        function = (function & (~0x0008));
        break;
      case 5 :
        wFunDef.settext( fundef, m_f5->GetValue().mb_str(wxConvUTF8)  );
        wFunDef.settimer( fundef, m_TimerF5->GetValue() );
        function = (function & (~0x0010));
        break;
      case 6 :
        wFunDef.settext( fundef, m_f6->GetValue().mb_str(wxConvUTF8)  );
        wFunDef.settimer( fundef, m_TimerF6->GetValue() );
        function = (function & (~0x0020));
        break;
      case 7 :
        wFunDef.settext( fundef, m_f7->GetValue().mb_str(wxConvUTF8)  );
        wFunDef.settimer( fundef, m_TimerF7->GetValue() );
        function = (function & (~0x0040));
        break;
      case 8 :
        wFunDef.settext( fundef, m_f8->GetValue().mb_str(wxConvUTF8)  );
        wFunDef.settimer( fundef, m_TimerF8->GetValue() );
        function = (function & (~0x0080));
        break;
      case 9 :
        wFunDef.settext( fundef, m_f9->GetValue().mb_str(wxConvUTF8)  );
        wFunDef.settimer( fundef, m_TimerF9->GetValue() );
        function = (function & (~0x0100));
        break;
      case 10:
        wFunDef.settext( fundef, m_f10->GetValue().mb_str(wxConvUTF8) );
        wFunDef.settimer( fundef, m_TimerF10->GetValue() );
        function = (function & (~0x0200));
        break;
      case 11:
        wFunDef.settext( fundef, m_f11->GetValue().mb_str(wxConvUTF8) );
        wFunDef.settimer( fundef, m_TimerF11->GetValue() );
        function = (function & (~0x0400));
        break;
      case 12:
        wFunDef.settext( fundef, m_f12->GetValue().mb_str(wxConvUTF8) );
        wFunDef.settimer( fundef, m_TimerF12->GetValue() );
        function = (function & (~0x0800));
        break;
    }
    fundef = wLoc.nextfundef( m_Props, fundef );
  }

  if( function > 0 ) {
    for( int i = 0; i < 12; i++ ) {
      if( function & (0x0001 << i) ) {
        fundef = NodeOp.inst( wFunDef.name(), m_Props, ELEMENT_NODE );
        NodeOp.addChild( m_Props, fundef );
        wFunDef.setfn( fundef, i+1 );
        wFunDef.settext( fundef, l_fx[i]->GetValue().mb_str(wxConvUTF8) );
        function &= (function & (~(0x0001<<i)));
      }
    }
  }

  wLoc.setfn( m_Props, m_Lights->GetValue() ? True:False );

  // Consist
  wLoc.setconsist_lightsoff( m_Props, m_LightsOff->GetValue() ? True:False );
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
    m_List = NULL;
    m_New = NULL;
    m_Delete = NULL;
    m_Doc = NULL;
    m_General_Panel = NULL;
    m_GeneralSizer = NULL;
    m_label_ID = NULL;
    m_ID = NULL;
    m_labShortID = NULL;
    m_ShortID = NULL;
    m_Label_Description = NULL;
    m_Description = NULL;
    m_labImageName = NULL;
    m_ImageName = NULL;
    m_Label_Length = NULL;
    m_Length = NULL;
    m_Label_CatalogNr = NULL;
    m_CatNr = NULL;
    m_Label_Purchased = NULL;
    m_Purchased = NULL;
    m_Label_Runtime = NULL;
    m_RunTime = NULL;
    m_labIdent = NULL;
    m_Ident = NULL;
    m_labRemark = NULL;
    m_Remark = NULL;
    m_Show = NULL;
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
    m_Regulated = NULL;
    m_LabelPlacing = NULL;
    m_Placing = NULL;
    m_labDirPause = NULL;
    m_DirPause = NULL;
    m_DetailsPanel = NULL;
    m_Label_Blockwait = NULL;
    m_Blockwait = NULL;
    m_labEventTimer = NULL;
    m_EventTimer = NULL;
    m_Commuter = NULL;
    m_SameDir = NULL;
    m_OppositeDir = NULL;
    m_UseScheduleTime = NULL;
    m_secondNextBlock = NULL;
    m_EngineBox = NULL;
    m_CargoBox = NULL;
    m_FunctionPanel = NULL;
    m_Label_f1 = NULL;
    m_f1 = NULL;
    m_TimerF1 = NULL;
    m_Button_f1 = NULL;
    m_Label_f2 = NULL;
    m_f2 = NULL;
    m_TimerF2 = NULL;
    m_Button_f2 = NULL;
    m_Label_f3 = NULL;
    m_f3 = NULL;
    m_TimerF3 = NULL;
    m_Button_f3 = NULL;
    m_Label_f4 = NULL;
    m_f4 = NULL;
    m_TimerF4 = NULL;
    m_Button_f4 = NULL;
    m_Label_f5 = NULL;
    m_f5 = NULL;
    m_TimerF5 = NULL;
    m_Button_f5 = NULL;
    m_Label_f6 = NULL;
    m_f6 = NULL;
    m_TimerF6 = NULL;
    m_Button_f6 = NULL;
    m_Label_f7 = NULL;
    m_f7 = NULL;
    m_TimerF7 = NULL;
    m_Button_f7 = NULL;
    m_Label_f8 = NULL;
    m_f8 = NULL;
    m_TimerF8 = NULL;
    m_Button_f8 = NULL;
    m_Label_f9 = NULL;
    m_f9 = NULL;
    m_TimerF9 = NULL;
    m_Button_f9 = NULL;
    m_Label_f10 = NULL;
    m_f10 = NULL;
    m_TimerF10 = NULL;
    m_Button_f10 = NULL;
    m_Label_f11 = NULL;
    m_f11 = NULL;
    m_TimerF11 = NULL;
    m_Button_f11 = NULL;
    m_Label_f12 = NULL;
    m_f12 = NULL;
    m_TimerF12 = NULL;
    m_Button_f12 = NULL;
    m_Lights = NULL;
    m_ConsistsPanel = NULL;
    m_labDetails = NULL;
    m_LightsOff = NULL;
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

    wxArrayString m_ListStrings;
    m_List = new wxListBox( m_IndexPanel, ID_LISTBOX, wxDefaultPosition, wxDefaultSize, m_ListStrings, wxLB_SINGLE|wxLB_ALWAYS_SB|wxLB_SORT );
    itemBoxSizer6->Add(m_List, 1, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer8 = new wxFlexGridSizer(2, 3, 0, 0);
    itemBoxSizer6->Add(itemFlexGridSizer8, 0, wxGROW|wxALL, 5);
    m_New = new wxButton( m_IndexPanel, ID_BUTTON_LOC_NEW, _("New"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(m_New, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Delete = new wxButton( m_IndexPanel, ID_BUTTON_DELETE, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(m_Delete, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Doc = new wxButton( m_IndexPanel, ID_BUTTON_LOCO_DOC, _("Doc"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer8->Add(m_Doc, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_IndexPanel, _("Index"));

    m_General_Panel = new wxPanel( m_Notebook, ID_PANEL_GENERAL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_GeneralSizer = new wxBoxSizer(wxVERTICAL);
    m_General_Panel->SetSizer(m_GeneralSizer);

    wxFlexGridSizer* itemFlexGridSizer14 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer14->AddGrowableCol(1);
    m_GeneralSizer->Add(itemFlexGridSizer14, 0, wxGROW|wxALL, 5);
    m_label_ID = new wxStaticText( m_General_Panel, wxID_STATIC_ID, _("ID:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_label_ID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_ID = new wxTextCtrl( m_General_Panel, ID_TEXTCTRL_ID, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_ID, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_labShortID = new wxStaticText( m_General_Panel, wxID_ANY, _("Short ID:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_labShortID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_ShortID = new wxTextCtrl( m_General_Panel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_ShortID, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_Label_Description = new wxStaticText( m_General_Panel, wxID_STATIC_DESCRIPTION, _("Description:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_Label_Description, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_Description = new wxTextCtrl( m_General_Panel, ID_TEXTCTRL_DESCRIPTION, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_Description, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_labImageName = new wxStaticText( m_General_Panel, wxID_ANY, _("Image:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_labImageName, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_ImageName = new wxTextCtrl( m_General_Panel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_ImageName, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_Label_Length = new wxStaticText( m_General_Panel, wxID_STATIC_LENGTH, _("Length:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_Label_Length, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_Length = new wxTextCtrl( m_General_Panel, ID_TEXTCTRL_LENGTH, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer14->Add(m_Length, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_Label_CatalogNr = new wxStaticText( m_General_Panel, wxID_STATIC_CATALOGNR, _("CatalogNr:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_Label_CatalogNr, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_CatNr = new wxTextCtrl( m_General_Panel, ID_TEXTCTRL_CATALOGNR, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_CatNr, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_Label_Purchased = new wxStaticText( m_General_Panel, wxID_STATIC_PURCHASED, _("Purchased:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_Label_Purchased, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_Purchased = new wxTextCtrl( m_General_Panel, ID_TEXTCTRL_PURCHASED, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_Purchased, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_Label_Runtime = new wxStaticText( m_General_Panel, wxID_STATIC_RUNTIME, _("Runtime:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_Label_Runtime, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_RunTime = new wxTextCtrl( m_General_Panel, ID_TEXTCTRL_RUNTIME, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_CENTRE );
    m_RunTime->Enable(false);
    itemFlexGridSizer14->Add(m_RunTime, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_labIdent = new wxStaticText( m_General_Panel, wxID_STATIC_LC_IDENT, _("Identifier"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_labIdent, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_Ident = new wxTextCtrl( m_General_Panel, ID_TEXTCTRL_LC_IDENT, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer14->Add(m_Ident, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_labRemark = new wxStaticText( m_General_Panel, wxID_ANY, _("Remark"), wxDefaultPosition, wxDefaultSize, 0 );
    m_GeneralSizer->Add(m_labRemark, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_Remark = new wxTextCtrl( m_General_Panel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    m_Remark->SetMaxLength(1000);
    m_GeneralSizer->Add(m_Remark, 1, wxGROW|wxALL, 5);

    m_Show = new wxCheckBox( m_General_Panel, wxID_ANY, _("show"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Show->SetValue(true);
    m_GeneralSizer->Add(m_Show, 0, wxALIGN_LEFT|wxALL, 5);

    m_Notebook->AddPage(m_General_Panel, _("General"));

    m_Interface_Panel = new wxPanel( m_Notebook, ID_PANEL_INTERFACE, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer37 = new wxBoxSizer(wxVERTICAL);
    m_Interface_Panel->SetSizer(itemBoxSizer37);

    wxFlexGridSizer* itemFlexGridSizer38 = new wxFlexGridSizer(1, 4, 0, 0);
    itemFlexGridSizer38->AddGrowableCol(3);
    itemBoxSizer37->Add(itemFlexGridSizer38, 0, wxGROW|wxALL, 5);
    m_Label_Address = new wxStaticText( m_Interface_Panel, wxID_STATIC_ADDRESS, _("Address:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer38->Add(m_Label_Address, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 2);

    m_Address = new wxTextCtrl( m_Interface_Panel, ID_TEXTCTRL_ADDRESS, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer38->Add(m_Address, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_Label_IID = new wxStaticText( m_Interface_Panel, wxID_STATIC_IID, _("IID:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer38->Add(m_Label_IID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 2);

    m_IID = new wxTextCtrl( m_Interface_Panel, ID_TEXTCTRL_IID, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer38->Add(m_IID, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    wxFlexGridSizer* itemFlexGridSizer43 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer37->Add(itemFlexGridSizer43, 0, wxGROW|wxALL, 5);
    m_Label_Bus = new wxStaticText( m_Interface_Panel, wxID_STATIC_LC_BUS, _("Bus:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer43->Add(m_Label_Bus, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 2);

    m_Bus = new wxTextCtrl( m_Interface_Panel, ID_TEXTCTRL_LC_BUS, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer43->Add(m_Bus, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 2);

    m_Label_Protocol = new wxStaticText( m_Interface_Panel, wxID_STATIC_PROTOCOL, _("Protocol:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer43->Add(m_Label_Protocol, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 2);

    wxArrayString m_ProtocolStrings;
    m_ProtocolStrings.Add(_("ServerDefined"));
    m_ProtocolStrings.Add(_("Motorola"));
    m_ProtocolStrings.Add(_("NMRA-DCC"));
    m_ProtocolStrings.Add(_("NMRA-DCC long"));
    m_ProtocolStrings.Add(_("Analog"));
    m_ProtocolStrings.Add(_("Car"));
    m_Protocol = new wxChoice( m_Interface_Panel, ID_CHOICE_PROTOCOL, wxDefaultPosition, wxDefaultSize, m_ProtocolStrings, 0 );
    m_Protocol->SetStringSelection(_("ServerDefined"));
    itemFlexGridSizer43->Add(m_Protocol, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_LabelProtVersion = new wxStaticText( m_Interface_Panel, wxID_STATIC_PROT_VER, _("ProtVersion:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer43->Add(m_LabelProtVersion, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 2);

    m_ProtVersion = new wxTextCtrl( m_Interface_Panel, ID_TEXTCTRL_PROTVER, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer43->Add(m_ProtVersion, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_LabelSpeedSteps = new wxStaticText( m_Interface_Panel, wxID_STATIC_SPEEDSTEPS, _("Speed steps:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer43->Add(m_LabelSpeedSteps, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 2);

    m_SpeedSteps = new wxTextCtrl( m_Interface_Panel, ID_TEXTCTRL_SPEEDSTEPS, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer43->Add(m_SpeedSteps, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_LabelFunCnt = new wxStaticText( m_Interface_Panel, wxID_STATIC_FUNCNT, _("Functions:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer43->Add(m_LabelFunCnt, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM|wxADJUST_MINSIZE, 2);

    m_FunCnt = new wxTextCtrl( m_Interface_Panel, ID_TEXTCTRL_FUNCNT, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer43->Add(m_FunCnt, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 2);

    wxFlexGridSizer* itemFlexGridSizer54 = new wxFlexGridSizer(2, 4, 0, 0);
    itemBoxSizer37->Add(itemFlexGridSizer54, 0, wxGROW|wxALL, 5);
    m_Label_V_min = new wxStaticText( m_Interface_Panel, wxID_STATIC_V_MIN, _("V_min:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer54->Add(m_Label_V_min, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxADJUST_MINSIZE, 2);

    m_V_min = new wxTextCtrl( m_Interface_Panel, ID_TEXTCTRL_V_MIN, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_V_min->SetMaxLength(3);
    itemFlexGridSizer54->Add(m_V_min, 0, wxALIGN_LEFT|wxALIGN_BOTTOM|wxLEFT|wxRIGHT|wxTOP, 2);

    m_Label_V_Rmin = new wxStaticText( m_Interface_Panel, wxID_ANY, _("V_Rmin:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer54->Add(m_Label_V_Rmin, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxADJUST_MINSIZE, 5);

    m_V_Rmin = new wxTextCtrl( m_Interface_Panel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer54->Add(m_V_Rmin, 0, wxALIGN_LEFT|wxALIGN_BOTTOM|wxLEFT|wxRIGHT|wxTOP, 5);

    m_Label_V_mid = new wxStaticText( m_Interface_Panel, wxID_ANY, _("V_mid:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer54->Add(m_Label_V_mid, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxADJUST_MINSIZE, 2);

    m_V_mid = new wxTextCtrl( m_Interface_Panel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_V_mid->SetMaxLength(3);
    itemFlexGridSizer54->Add(m_V_mid, 0, wxALIGN_LEFT|wxALIGN_BOTTOM|wxLEFT|wxRIGHT, 2);

    m_Label_V_Rmid = new wxStaticText( m_Interface_Panel, wxID_ANY, _("V_Rmid:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer54->Add(m_Label_V_Rmid, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxADJUST_MINSIZE, 5);

    m_V_Rmid = new wxTextCtrl( m_Interface_Panel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer54->Add(m_V_Rmid, 0, wxALIGN_LEFT|wxALIGN_BOTTOM|wxLEFT|wxRIGHT, 5);

    m_Label_V_max = new wxStaticText( m_Interface_Panel, wxID_STATIC_V_MAX, _("V_max:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer54->Add(m_Label_V_max, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxADJUST_MINSIZE, 2);

    m_V_max = new wxTextCtrl( m_Interface_Panel, ID_TEXTCTRL_V_MAX, _("100"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_V_max->SetMaxLength(3);
    itemFlexGridSizer54->Add(m_V_max, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_Label_V_Rmax = new wxStaticText( m_Interface_Panel, wxID_ANY, _("V_Rmax:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer54->Add(m_Label_V_Rmax, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxADJUST_MINSIZE, 5);

    m_V_Rmax = new wxTextCtrl( m_Interface_Panel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer54->Add(m_V_Rmax, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_LabelV_step = new wxStaticText( m_Interface_Panel, wxID_STATIC_LOC_V_STEP, _("V_step:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer54->Add(m_LabelV_step, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM|wxADJUST_MINSIZE, 2);

    m_Accel = new wxTextCtrl( m_Interface_Panel, ID_TEXTCTRL_LOC_ACCEL, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer54->Add(m_Accel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 2);

    m_LabelV_mode = new wxStaticText( m_Interface_Panel, wxID_STATIC_LOC_V_MODE, _("V_mode:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer54->Add(m_LabelV_mode, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM|wxADJUST_MINSIZE, 2);

    m_V_mode_percent = new wxCheckBox( m_Interface_Panel, ID_CHECKBOX_LOC_CONTROLLED, _("percent"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_V_mode_percent->SetValue(false);
    itemFlexGridSizer54->Add(m_V_mode_percent, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 2);

    wxFlexGridSizer* itemFlexGridSizer71 = new wxFlexGridSizer(2, 3, 0, 0);
    itemFlexGridSizer71->AddGrowableCol(1);
    itemBoxSizer37->Add(itemFlexGridSizer71, 0, wxGROW|wxALL, 5);
    m_Label_Mass = new wxStaticText( m_Interface_Panel, wxID_STATIC_MASS, _("Mass:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer71->Add(m_Label_Mass, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 2);

    m_Mass = new wxTextCtrl( m_Interface_Panel, ID_TEXTCTRL_MASS, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_Mass->SetMaxLength(3);
    itemFlexGridSizer71->Add(m_Mass, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 2);

    m_Regulated = new wxCheckBox( m_Interface_Panel, ID_LOC_REGULATED, _("regulated"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Regulated->SetValue(false);
    itemFlexGridSizer71->Add(m_Regulated, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_LabelPlacing = new wxStaticText( m_Interface_Panel, wxID_STATIC_LOC_PLACING, _("Placing:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer71->Add(m_LabelPlacing, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM|wxADJUST_MINSIZE, 2);

    m_Placing = new wxCheckBox( m_Interface_Panel, ID_CHECKBOX_LOC_PLACING, _("default"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_Placing->SetValue(true);
    itemFlexGridSizer71->Add(m_Placing, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 2);

    wxFlexGridSizer* itemFlexGridSizer77 = new wxFlexGridSizer(1, 3, 0, 0);
    itemBoxSizer37->Add(itemFlexGridSizer77, 0, wxGROW|wxALL, 5);
    m_labDirPause = new wxStaticText( m_Interface_Panel, wxID_ANY, _("Dir pause"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer77->Add(m_labDirPause, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 5);

    m_DirPause = new wxTextCtrl( m_Interface_Panel, wxID_ANY, _("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
    m_DirPause->SetMaxLength(4);
    itemFlexGridSizer77->Add(m_DirPause, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_Notebook->AddPage(m_Interface_Panel, _("Interface"));

    m_DetailsPanel = new wxPanel( m_Notebook, ID_PANEL_DETAILS, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer81 = new wxBoxSizer(wxVERTICAL);
    m_DetailsPanel->SetSizer(itemBoxSizer81);

    wxFlexGridSizer* itemFlexGridSizer82 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer82->AddGrowableCol(1);
    itemBoxSizer81->Add(itemFlexGridSizer82, 0, wxALIGN_LEFT|wxALL, 5);
    m_Label_Blockwait = new wxStaticText( m_DetailsPanel, wxID_STATIC_BLOCKWAIT, _("Block wait"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer82->Add(m_Label_Blockwait, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Blockwait = new wxTextCtrl( m_DetailsPanel, ID_TEXTCTRL_BLOCKWAIT, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer82->Add(m_Blockwait, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labEventTimer = new wxStaticText( m_DetailsPanel, wxID_ANY, _("Event timer"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer82->Add(m_labEventTimer, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_EventTimer = new wxTextCtrl( m_DetailsPanel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer82->Add(m_EventTimer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Commuter = new wxCheckBox( m_DetailsPanel, ID_CHECKBOX_COMMUTER, _("Commuter train"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_Commuter->SetValue(false);
    itemBoxSizer81->Add(m_Commuter, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_SameDir = new wxCheckBox( m_DetailsPanel, ID_CHECKBOX_LC_SAMEDIR, _("Same direction"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SameDir->SetValue(false);
    itemBoxSizer81->Add(m_SameDir, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_OppositeDir = new wxCheckBox( m_DetailsPanel, wxID_ANY, _("Opposite destination"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OppositeDir->SetValue(false);
    itemBoxSizer81->Add(m_OppositeDir, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_UseScheduleTime = new wxCheckBox( m_DetailsPanel, wxID_ANY, _("Use schedule departure time"), wxDefaultPosition, wxDefaultSize, 0 );
    m_UseScheduleTime->SetValue(false);
    itemBoxSizer81->Add(m_UseScheduleTime, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_secondNextBlock = new wxCheckBox( m_DetailsPanel, wxID_ANY, _("Reserve second next block"), wxDefaultPosition, wxDefaultSize, 0 );
    m_secondNextBlock->SetValue(false);
    itemBoxSizer81->Add(m_secondNextBlock, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxArrayString m_EngineBoxStrings;
    m_EngineBoxStrings.Add(_("&Diesel"));
    m_EngineBoxStrings.Add(_("&Steam"));
    m_EngineBoxStrings.Add(_("&Electric"));
    m_EngineBox = new wxRadioBox( m_DetailsPanel, ID_RADIOBOX_ENGINE, _("Engine"), wxDefaultPosition, wxDefaultSize, m_EngineBoxStrings, 1, wxRA_SPECIFY_COLS );
    m_EngineBox->SetSelection(0);
    itemBoxSizer81->Add(m_EngineBox, 0, wxGROW|wxALL, 5);

    wxArrayString m_CargoBoxStrings;
    m_CargoBoxStrings.Add(_("&None"));
    m_CargoBoxStrings.Add(_("&Goods"));
    m_CargoBoxStrings.Add(_("&Passengers"));
    m_CargoBoxStrings.Add(_("&Mixed"));
    m_CargoBoxStrings.Add(_("&Cleaning"));
    m_CargoBoxStrings.Add(_("&ICE"));
    m_CargoBox = new wxRadioBox( m_DetailsPanel, ID_RADIOBOX_CARGO, _("Cargo"), wxDefaultPosition, wxDefaultSize, m_CargoBoxStrings, 2, wxRA_SPECIFY_COLS );
    m_CargoBox->SetSelection(0);
    itemBoxSizer81->Add(m_CargoBox, 0, wxGROW|wxALL, 5);

    m_Notebook->AddPage(m_DetailsPanel, _("Details"));

    m_FunctionPanel = new wxPanel( m_Notebook, ID_PANEL_FUNCTIONS, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer95 = new wxBoxSizer(wxVERTICAL);
    m_FunctionPanel->SetSizer(itemBoxSizer95);

    wxFlexGridSizer* itemFlexGridSizer96 = new wxFlexGridSizer(2, 4, 0, 0);
    itemFlexGridSizer96->AddGrowableCol(1);
    itemBoxSizer95->Add(itemFlexGridSizer96, 0, wxGROW|wxALL, 5);
    m_Label_f1 = new wxStaticText( m_FunctionPanel, wxID_STATIC_F1, _("f1"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_Label_f1, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 2);

    m_f1 = new wxTextCtrl( m_FunctionPanel, ID_TEXTCTRL_F1, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_f1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 2);

    m_TimerF1 = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer96->Add(m_TimerF1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_Button_f1 = new wxButton( m_FunctionPanel, ID_BUTTON_F1, _("..."), wxDefaultPosition, wxSize(25, 25), 0 );
    itemFlexGridSizer96->Add(m_Button_f1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 2);

    m_Label_f2 = new wxStaticText( m_FunctionPanel, wxID_STATIC_F2, _("f2"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_Label_f2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 2);

    m_f2 = new wxTextCtrl( m_FunctionPanel, ID_TEXTCTRL_F2, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_f2, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_TimerF2 = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer96->Add(m_TimerF2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_Button_f2 = new wxButton( m_FunctionPanel, ID_BUTTON_F2, _("..."), wxDefaultPosition, wxSize(25, 25), 0 );
    itemFlexGridSizer96->Add(m_Button_f2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_Label_f3 = new wxStaticText( m_FunctionPanel, wxID_STATIC_F3, _("f3"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_Label_f3, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 2);

    m_f3 = new wxTextCtrl( m_FunctionPanel, ID_TEXTCTRL_F3, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_f3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_TimerF3 = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer96->Add(m_TimerF3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_Button_f3 = new wxButton( m_FunctionPanel, ID_BUTTON_F3, _("..."), wxDefaultPosition, wxSize(25, 25), 0 );
    itemFlexGridSizer96->Add(m_Button_f3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_Label_f4 = new wxStaticText( m_FunctionPanel, wxID_STATIC_F4, _("f4"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_Label_f4, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 2);

    m_f4 = new wxTextCtrl( m_FunctionPanel, ID_TEXTCTRL_F4, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_f4, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_TimerF4 = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer96->Add(m_TimerF4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_Button_f4 = new wxButton( m_FunctionPanel, ID_BUTTON_F4, _("..."), wxDefaultPosition, wxSize(25, 25), 0 );
    itemFlexGridSizer96->Add(m_Button_f4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_Label_f5 = new wxStaticText( m_FunctionPanel, wxID_ANY, _("f5"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_Label_f5, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 2);

    m_f5 = new wxTextCtrl( m_FunctionPanel, ID_TEXTCTRL_F5, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_f5, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_TimerF5 = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer96->Add(m_TimerF5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_Button_f5 = new wxButton( m_FunctionPanel, ID_BUTTON_F5, _("..."), wxDefaultPosition, wxSize(25, 25), 0 );
    itemFlexGridSizer96->Add(m_Button_f5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_Label_f6 = new wxStaticText( m_FunctionPanel, wxID_ANY, _("f6"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_Label_f6, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 2);

    m_f6 = new wxTextCtrl( m_FunctionPanel, ID_TEXTCTRL_F6, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_f6, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_TimerF6 = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer96->Add(m_TimerF6, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_Button_f6 = new wxButton( m_FunctionPanel, ID_BUTTON_F6, _("..."), wxDefaultPosition, wxSize(25, 25), 0 );
    itemFlexGridSizer96->Add(m_Button_f6, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_Label_f7 = new wxStaticText( m_FunctionPanel, wxID_ANY, _("f7"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_Label_f7, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 2);

    m_f7 = new wxTextCtrl( m_FunctionPanel, ID_TEXTCTRL_F7, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_f7, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_TimerF7 = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer96->Add(m_TimerF7, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_Button_f7 = new wxButton( m_FunctionPanel, ID_BUTTON_F7, _("..."), wxDefaultPosition, wxSize(25, 25), 0 );
    itemFlexGridSizer96->Add(m_Button_f7, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_Label_f8 = new wxStaticText( m_FunctionPanel, wxID_ANY, _("f8"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_Label_f8, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 2);

    m_f8 = new wxTextCtrl( m_FunctionPanel, ID_TEXTCTRL_F8, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_f8, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_TimerF8 = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer96->Add(m_TimerF8, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_Button_f8 = new wxButton( m_FunctionPanel, ID_BUTTON_F8, _("..."), wxDefaultPosition, wxSize(25, 25), 0 );
    itemFlexGridSizer96->Add(m_Button_f8, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_Label_f9 = new wxStaticText( m_FunctionPanel, wxID_ANY, _("f9"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_Label_f9, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 2);

    m_f9 = new wxTextCtrl( m_FunctionPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_f9, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_TimerF9 = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer96->Add(m_TimerF9, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_Button_f9 = new wxButton( m_FunctionPanel, ID_BUTTON_F9, _("..."), wxDefaultPosition, wxSize(25, 25), 0 );
    itemFlexGridSizer96->Add(m_Button_f9, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_Label_f10 = new wxStaticText( m_FunctionPanel, wxID_ANY, _("f10"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_Label_f10, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 2);

    m_f10 = new wxTextCtrl( m_FunctionPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_f10, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_TimerF10 = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer96->Add(m_TimerF10, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_Button_f10 = new wxButton( m_FunctionPanel, ID_BUTTON_F10, _("..."), wxDefaultPosition, wxSize(25, 25), 0 );
    itemFlexGridSizer96->Add(m_Button_f10, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_Label_f11 = new wxStaticText( m_FunctionPanel, wxID_ANY, _("f11"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_Label_f11, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 2);

    m_f11 = new wxTextCtrl( m_FunctionPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_f11, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_TimerF11 = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer96->Add(m_TimerF11, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_Button_f11 = new wxButton( m_FunctionPanel, ID_BUTTON_F11, _("..."), wxDefaultPosition, wxSize(25, 25), 0 );
    itemFlexGridSizer96->Add(m_Button_f11, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_Label_f12 = new wxStaticText( m_FunctionPanel, wxID_ANY, _("f12"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_Label_f12, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 2);

    m_f12 = new wxTextCtrl( m_FunctionPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_f12, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    m_TimerF12 = new wxSpinCtrl( m_FunctionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer96->Add(m_TimerF12, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_Button_f12 = new wxButton( m_FunctionPanel, ID_BUTTON_F12, _("..."), wxDefaultPosition, wxSize(25, 25), 0 );
    itemFlexGridSizer96->Add(m_Button_f12, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 2);

    wxFlexGridSizer* itemFlexGridSizer145 = new wxFlexGridSizer(1, 2, 0, 0);
    itemBoxSizer95->Add(itemFlexGridSizer145, 0, wxGROW|wxALL, 5);
    m_Lights = new wxCheckBox( m_FunctionPanel, ID_CHECKBOX_LC_LIGHTS, _("Lights"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_Lights->SetValue(false);
    itemFlexGridSizer145->Add(m_Lights, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_FunctionPanel, _("Functions"));

    m_ConsistsPanel = new wxPanel( m_Notebook, ID_PANEL_LOC_CONSIST, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer148 = new wxBoxSizer(wxVERTICAL);
    m_ConsistsPanel->SetSizer(itemBoxSizer148);

    m_labDetails = new wxStaticBox(m_ConsistsPanel, wxID_ANY, _("Details"));
    wxStaticBoxSizer* itemStaticBoxSizer149 = new wxStaticBoxSizer(m_labDetails, wxVERTICAL);
    itemBoxSizer148->Add(itemStaticBoxSizer149, 0, wxGROW|wxALL, 5);
    m_LightsOff = new wxCheckBox( m_ConsistsPanel, wxID_ANY, _("Keep lights off"), wxDefaultPosition, wxDefaultSize, 0 );
    m_LightsOff->SetValue(false);
    itemStaticBoxSizer149->Add(m_LightsOff, 0, wxALIGN_LEFT|wxALL, 5);

    wxStaticLine* itemStaticLine151 = new wxStaticLine( m_ConsistsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemBoxSizer148->Add(itemStaticLine151, 0, wxGROW|wxALL, 5);

    m_labConsistList = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("Locomotives"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer148->Add(m_labConsistList, 0, wxALIGN_LEFT|wxALL, 5);

    wxArrayString m_ConsistListStrings;
    m_ConsistList = new wxListBox( m_ConsistsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_ConsistListStrings, wxLB_SINGLE );
    itemBoxSizer148->Add(m_ConsistList, 1, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer154 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer154->AddGrowableCol(1);
    itemBoxSizer148->Add(itemFlexGridSizer154, 0, wxGROW|wxALL, 5);
    m_labConsistLocID = new wxStaticText( m_ConsistsPanel, wxID_ANY, _("LocID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer154->Add(m_labConsistLocID, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_ConsistLocIDStrings;
    m_ConsistLocID = new wxComboBox( m_ConsistsPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, m_ConsistLocIDStrings, wxCB_DROPDOWN );
    itemFlexGridSizer154->Add(m_ConsistLocID, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer157 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer148->Add(itemBoxSizer157, 0, wxGROW|wxALL, 5);
    m_AddConsistLoc = new wxButton( m_ConsistsPanel, ID_BUTTON_LOC_CONSIST_ADD, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer157->Add(m_AddConsistLoc, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_DeleteConsistLoc = new wxButton( m_ConsistsPanel, ID_BUTTON_LOC_CONSIST_DELETE, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer157->Add(m_DeleteConsistLoc, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_ConsistsPanel, _("Consists"));

    m_CVPanel = new wxPanel( m_Notebook, ID_PANEL_LOC_CV, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer161 = new wxBoxSizer(wxVERTICAL);
    m_CVPanel->SetSizer(itemBoxSizer161);

    m_CVList = new wxGrid( m_CVPanel, ID_GRID_LOC_CV, wxDefaultPosition, wxSize(200, 150), wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL );
    m_CVList->SetDefaultColSize(50);
    m_CVList->SetDefaultRowSize(25);
    m_CVList->SetColLabelSize(25);
    m_CVList->SetRowLabelSize(50);
    m_CVList->CreateGrid(1, 3, wxGrid::wxGridSelectCells);
    itemBoxSizer161->Add(m_CVList, 1, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer163 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer163->AddGrowableCol(1);
    itemBoxSizer161->Add(itemFlexGridSizer163, 0, wxGROW|wxALL, 5);
    m_labCVDesc = new wxStaticText( m_CVPanel, wxID_ANY, _("Description"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer163->Add(m_labCVDesc, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_CVDescription = new wxTextCtrl( m_CVPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer163->Add(m_CVDescription, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_CVDescModify = new wxButton( m_CVPanel, ID_BUTTON_LC_CV_DESC, _("Modify"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer163->Add(m_CVDescModify, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Notebook->AddPage(m_CVPanel, _("CV's"));

    itemBoxSizer2->Add(m_Notebook, 1, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer167 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer167, 0, wxALIGN_RIGHT|wxALL, 5);
    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer167->AddButton(m_Cancel);

    m_Apply = new wxButton( itemDialog1, wxID_APPLY, _("&Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer167->AddButton(m_Apply);

    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer167->AddButton(m_OK);

    itemStdDialogButtonSizer167->Realize();

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

  FunctionDialog*  funDlg = new FunctionDialog(this, m_Props, 1 );
  funDlg->ShowModal();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_F2
 */

void LocDialog::OnButtonF2Click( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  FunctionDialog*  funDlg = new FunctionDialog(this, m_Props, 2 );
  funDlg->ShowModal();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_F3
 */

void LocDialog::OnButtonF3Click( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  FunctionDialog*  funDlg = new FunctionDialog(this, m_Props, 3 );
  funDlg->ShowModal();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_F4
 */

void LocDialog::OnButtonF4Click( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  FunctionDialog*  funDlg = new FunctionDialog(this, m_Props, 4 );
  funDlg->ShowModal();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
 */

void LocDialog::OnApplyClick( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  Evaluate();
  if( !wxGetApp().isStayOffline() ) {
    /* Notify RocRail. */
    iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.modify );
    NodeOp.addChild( cmd, (iONode)m_Props->base.clone( m_Props ) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
  InitIndex();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON
 */

void LocDialog::OnButtonF5Click( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  FunctionDialog*  funDlg = new FunctionDialog(this, m_Props, 5 );
  funDlg->ShowModal();
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON1
 */

void LocDialog::OnButtonF6Click( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  FunctionDialog*  funDlg = new FunctionDialog(this, m_Props, 6 );
  funDlg->ShowModal();
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON2
 */

void LocDialog::OnButtonF7Click( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  FunctionDialog*  funDlg = new FunctionDialog(this, m_Props, 7 );
  funDlg->ShowModal();
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON3
 */

void LocDialog::OnButtonF12Click( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  FunctionDialog*  funDlg = new FunctionDialog(this, m_Props, 12 );
  funDlg->ShowModal();
}


/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX
 */

void LocDialog::OnListboxSelected( wxCommandEvent& event )
{
  m_Props = wxGetApp().getFrame()->findLoc( m_List->GetStringSelection().mb_str(wxConvUTF8) );
  if( m_Props != NULL )
    InitValues();
  else
    TraceOp.trc( "locdlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
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
  int i = m_List->FindString( _T("NEW") );
  if( i == wxNOT_FOUND ) {
    m_List->Append( _T("NEW") );
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
        m_Props = lc;
        InitValues();
      }
    }
  }
  m_List->SetStringSelection( _T("NEW") );
  m_List->SetFirstItem( _T("NEW") );
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
      _T("PNG files (*.png)|*.png|GIF files (*.gif)|*.gif|XPM files (*.xpm)|*.xpm"), wxOPEN);
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
  for( int i = 0; i < 257; i++ )
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
  iONode cv = m_CVNodes[cvnr];
  wCVByte.setdesc( cv, m_CVDescription->GetValue().mb_str(wxConvUTF8) );
  m_CVList->SetCellValue(m_iSelectedCV, 2, m_CVDescription->GetValue() );
}


/*!
 * wxEVT_GRID_CELL_LEFT_CLICK event handler for ID_GRID_LOC_CV
 */

void LocDialog::OnCVCellLeftClick( wxGridEvent& event )
{
  m_iSelectedCV = event.GetRow();
  wxString str = m_CVList->GetCellValue( m_iSelectedCV, 2 );
  m_CVDescription->SetValue(str);
  m_CVDescription->Enable(true);
  m_CVDescModify->Enable(true);
  event.Skip();
}




/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_F8
 */

void LocDialog::OnButtonF8Click( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  FunctionDialog*  funDlg = new FunctionDialog(this, m_Props, 8 );
  funDlg->ShowModal();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_F9
 */

void LocDialog::OnButtonF9Click( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  FunctionDialog*  funDlg = new FunctionDialog(this, m_Props, 9 );
  funDlg->ShowModal();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_F10
 */

void LocDialog::OnButtonF10Click( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  FunctionDialog*  funDlg = new FunctionDialog(this, m_Props, 10 );
  funDlg->ShowModal();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_F11
 */

void LocDialog::OnButtonF11Click( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  FunctionDialog*  funDlg = new FunctionDialog(this, m_Props, 11 );
  funDlg->ShowModal();
}


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

  if( m_Regulated->IsChecked() || val > 14 ) {
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

