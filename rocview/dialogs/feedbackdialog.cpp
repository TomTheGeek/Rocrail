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
#pragma implementation "feedbackdialog.h"
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

#include "feedbackdialog.h"
#include "actionsctrldlg.h"

////@begin XPM images
////@end XPM images

#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/FeedbackList.h"
#include "rocrail/wrapper/public/ActionCtrl.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Route.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocview/wrapper/public/Gui.h"
#include "rocview/public/guiapp.h"

#include "rocs/public/node.h"
#include "rocs/public/file.h"
#include "rocs/public/system.h"

/*!
 * FeedbackDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( FeedbackDialog, wxDialog )

/*!
 * FeedbackDialog event table definition
 */

BEGIN_EVENT_TABLE( FeedbackDialog, wxDialog )

////@begin FeedbackDialog event table entries
    EVT_LIST_ITEM_SELECTED( ID_LISTCTRLINDEX_FB, FeedbackDialog::OnListctrlindexFbSelected )
    EVT_LIST_COL_CLICK( ID_LISTCTRLINDEX_FB, FeedbackDialog::OnListctrlindexFbColLeftClick )

    EVT_BUTTON( ID_BUTTON_FB_NEW, FeedbackDialog::OnButtonFbNewClick )

    EVT_BUTTON( ID_BUTTON_FB_DELETE, FeedbackDialog::OnButtonFbDeleteClick )

    EVT_BUTTON( ID_BUTTON_FB_DOC, FeedbackDialog::OnButtonFbDocClick )

    EVT_BUTTON( ID_FEEDBACK_ACTIONS, FeedbackDialog::OnFeedbackActionsClick )

    EVT_RADIOBOX( ID_FB_TYPE, FeedbackDialog::OnFbTypeSelected )

    EVT_BUTTON( wxID_CANCEL, FeedbackDialog::OnCancelClick )

    EVT_BUTTON( wxID_OK, FeedbackDialog::OnOkClick )

    EVT_BUTTON( wxID_APPLY, FeedbackDialog::OnApplyClick )

////@end FeedbackDialog event table entries
    EVT_MENU( ID_PANEL_FB_GENERAL, FeedbackDialog::OnSelectPage )

END_EVENT_TABLE()

/*!
 * FeedbackDialog constructors
 */

FeedbackDialog::FeedbackDialog()
{
}

FeedbackDialog::FeedbackDialog( wxWindow* parent, iONode p_Props )
{
  m_TabAlign = wxGetApp().getTabAlign();
  Create(parent, -1, wxGetApp().getMsg("sensortable") );
  m_Props = p_Props;

  initLabels();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  m_IndexPanel->GetSizer()->Layout();
  m_General->GetSizer()->Layout();
  m_LocationPanel->GetSizer()->Layout();
  m_Interface->GetSizer()->Layout();
  m_Wiring->GetSizer()->Layout();
  m_Notebook->Fit();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  m_Delete->Enable( false ); // ToDo
  if( initIndex() ) {
    initValues();
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_PANEL_FB_GENERAL );
    wxPostEvent( this, event );
  }
}

void FeedbackDialog::OnSelectPage(wxCommandEvent& event) {
  m_Notebook->SetSelection( 1 );
}

/* comparator for sorting by id: */
static int __sortStr(obj* _a, obj* _b)
{
    const char* a = (const char*)*_a;
    const char* b = (const char*)*_b;
    return strcmp( a, b );
}


void FeedbackDialog::initLabels() {
  m_Notebook->SetPageText( 0, wxGetApp().getMsg( "index" ) );
  m_Notebook->SetPageText( 1, wxGetApp().getMsg( "general" ) );
  m_Notebook->SetPageText( 2, wxGetApp().getMsg( "position" ) );
  m_Notebook->SetPageText( 3, wxGetApp().getMsg( "interface" ) );
  m_Notebook->SetPageText( 4, wxGetApp().getMsg( "wiring" ) );

  // Index
  initList(m_List2, this);
  m_New->SetLabel( wxGetApp().getMsg( "new" ) );
  m_Delete->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_Doc->SetLabel( wxGetApp().getMsg( "doc_report" ) );

  // General
  m_LabelId->SetLabel( wxGetApp().getMsg( "id" ) );
  m_labBlockID->SetLabel( wxGetApp().getMsg( "blockid" ) );
  m_LabelDescription->SetLabel( wxGetApp().getMsg( "description" ) );
  m_labAccNr->SetLabel( wxGetApp().getMsg( "accnr" ) );
  m_State->SetLabel( wxGetApp().getMsg( "state" ) );
  m_Show->SetLabel( wxGetApp().getMsg( "show" ) );
  m_Road->SetLabel( wxGetApp().getMsg( "road" ) );
  m_Curved->SetLabel( wxGetApp().getMsg( "curve" ) );
  m_Actions->SetLabel( wxGetApp().getMsg( "actions" ) + _T("...") );
  m_labRouteIDs->SetLabel( wxGetApp().getMsg( "routeids" ) );
  m_labTimer->SetLabel( wxGetApp().getMsg( "timer" ) );

  m_BlockID->Append( _T("") );
  iONode model = wxGetApp().getModel();
  iOList list = ListOp.inst();


  if( model != NULL ) {
    iONode bklist = wPlan.getbklist( model );
    if( bklist != NULL ) {
      int cnt = NodeOp.getChildCnt( bklist );
      for( int i = 0; i < cnt; i++ ) {
        iONode bk = NodeOp.getChild( bklist, i );
        //m_BlockID->Append( wxString(wBlock.getid( bk ),wxConvUTF8), bk );
        ListOp.add(list, (obj)wBlock.getid( bk ));
      }
    }
    iONode fblist = wPlan.getfblist( model );
    if( fblist != NULL ) {
      int cnt = NodeOp.getChildCnt( fblist );
      for( int i = 0; i < cnt; i++ ) {
        iONode fb = NodeOp.getChild( fblist, i );
        //m_BlockID->Append( wxString(wFeedback.getid( fb ),wxConvUTF8), fb );
        ListOp.add(list, (obj)wFeedback.getid( fb ));
      }
    }

    ListOp.sort(list, &__sortStr);
    int cnt = ListOp.size( list );
    for( int i = 0; i < cnt; i++ ) {
      const char* id = (const char*)ListOp.get( list, i );
      m_BlockID->Append( wxString(id,wxConvUTF8) );
    }

  }
  ListOp.base.del(list);

  // Location
  m_LabelX->SetLabel( wxGetApp().getMsg( "x" ) );
  m_LabelY->SetLabel( wxGetApp().getMsg( "y" ) );
  m_LabelZ->SetLabel( wxGetApp().getMsg( "z" ) );
  m_ori->SetLabel( wxGetApp().getMsg( "orientation" ) );
  m_ori->SetString( 0, wxGetApp().getMsg( "north" ) );
  m_ori->SetString( 1, wxGetApp().getMsg( "east" ) );
  m_ori->SetString( 2, wxGetApp().getMsg( "south" ) );
  m_ori->SetString( 3, wxGetApp().getMsg( "west" ) );
  m_Show->SetLabel( wxGetApp().getMsg( "visible" ) );

  // Interface
  m_Labeliid->SetLabel( wxGetApp().getMsg( "iid" ) );
  m_Type->SetLabel( wxGetApp().getMsg( "type" ) );
  m_ActiveLow->SetLabel( wxGetApp().getMsg( "activelow" ) );
  m_AddressBox->SetLabel( wxGetApp().getMsg( "address" ) );
  m_labBusNr->SetLabel( wxGetApp().getMsg( "bus" ) );
  m_LabelAddress->SetLabel( wxGetApp().getMsg( "address" ) );
  m_CutoutBox->SetLabel( wxGetApp().getMsg( "shortcut" ) );
  m_labCutoutBus->SetLabel( wxGetApp().getMsg( "bus" ) );
  m_labCutoutAddr->SetLabel( wxGetApp().getMsg( "address" ) );
  m_OptionsBox->SetLabel( wxGetApp().getMsg( "options" ) );
  m_ResetWC->SetLabel( wxGetApp().getMsg( "reset" ) );

  // Wiring
  m_labCTCIID->SetLabel( wxGetApp().getMsg( "iid" ) );
  m_labCTCAddr->SetLabel( wxGetApp().getMsg( "address" ) );
  m_labCTCPort->SetLabel( wxGetApp().getMsg( "port" ) );
  m_labCTCGate->SetLabel( wxGetApp().getMsg( "gate" ) );
  m_CTCGate->SetString( 0, wxGetApp().getMsg( "red" ) );
  m_CTCGate->SetString( 1, wxGetApp().getMsg( "green" ) );
  m_AsSwitch->SetLabel( wxGetApp().getMsg( "turnout" ) );


  // Buttons
  m_OK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
  m_Apply->SetLabel( wxGetApp().getMsg( "apply" ) );
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


bool FeedbackDialog::initIndex() {
  TraceOp.trc( "app", TRCLEVEL_INFO, __LINE__, 9999, "InitIndex" );
  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode fblist = wPlan.getfblist( model );
    if( fblist != NULL ) {
      fillIndex(fblist);

      if( m_Props != NULL ) {
        setIDSelection(wItem.getid( m_Props ));
        return true;
      }
      else {
        m_Props = setSelection(0);
      }

    }
  }
  return false;
}


void FeedbackDialog::initValues() {
  char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("sensor").mb_str(wxConvUTF8), wFeedback.getid( m_Props ) );
  SetTitle( wxString(title,wxConvUTF8) );
  StrOp.free( title );

  // General
  m_Id->SetValue( wxString(wFeedback.getid( m_Props ),wxConvUTF8) );
  m_Description->SetValue( wxString(wFeedback.getdesc( m_Props ),wxConvUTF8) );
  m_Show->SetValue( wFeedback.isshow( m_Props )?true:false );
  m_State->SetValue( wFeedback.isstate( m_Props )?true:false );
  m_Road->SetValue( wItem.isroad( m_Props )?true:false );
  m_Curved->SetValue( wFeedback.iscurve( m_Props )?true:false );
  m_BlockID->SetStringSelection( wFeedback.getblockid( m_Props ) == NULL ?
                                _T(""):wxString(wFeedback.getblockid( m_Props ),wxConvUTF8)  );
  m_RouteIDs->SetValue( wxString(wItem.getrouteids( m_Props ),wxConvUTF8) );
  m_AccNr->SetValue(wFeedback.getaccnr( m_Props ));
  m_Timer->SetValue(wFeedback.gettimer( m_Props ));

  // Location
  char* val = StrOp.fmt( "%d", wFeedback.getx( m_Props ) );
  m_x->SetValue( wxString(val,wxConvUTF8) ); StrOp.free( val );
  val = StrOp.fmt( "%d", wFeedback.gety( m_Props ) );
  m_y->SetValue( wxString(val,wxConvUTF8) ); StrOp.free( val );
  val = StrOp.fmt( "%d", wFeedback.getz( m_Props ) );
  m_z->SetValue( wxString(val,wxConvUTF8) ); StrOp.free( val );
  if( StrOp.equals( wItem.north, wFeedback.getori( m_Props ) ) )
    m_ori->SetSelection( 0 );
  else if( StrOp.equals( wItem.east, wFeedback.getori( m_Props ) ) )
    m_ori->SetSelection( 1 );
  else if( StrOp.equals( wItem.south, wFeedback.getori( m_Props ) ) )
    m_ori->SetSelection( 2 );
  else
    m_ori->SetSelection( 3 );
  m_Show->SetValue( wFeedback.isshow( m_Props ) ? true:false);


  // Interface
  char * str;
  m_iid->SetValue( wFeedback.getiid( m_Props )==NULL?_T(""):wxString(wFeedback.getiid( m_Props ),wxConvUTF8) );

  m_Type->SetSelection( wFeedback.getfbtype(m_Props) );

  m_Address->SetValue( wFeedback.getaddr(m_Props) );
  m_BusNr->SetValue( wxString::Format( wxT("%d"), wFeedback.getbus(m_Props)) );

  m_CutoutAddr->SetValue( wFeedback.getcutoutaddr(m_Props) );
  m_CutoutBus->SetValue( wFeedback.getcutoutbus(m_Props) );
  m_ActiveLow->SetValue( wFeedback.isactivelow( m_Props ) ? true:false);
  m_ResetWC->SetValue( wFeedback.isresetwc( m_Props ) ? true:false);

  // Wiring
  m_CTCIID->SetValue( wxString(wFeedback.getctciid( m_Props ),wxConvUTF8) );
  m_CTCAddr->SetValue( wFeedback.getctcaddr(m_Props) );
  m_CTCPort->SetValue( wFeedback.getctcport(m_Props) );
  m_CTCGate->SetSelection( wFeedback.getctcgate(m_Props) );
  m_AsSwitch->SetValue( wFeedback.isctcasswitch( m_Props )?true:false );


  // Action

}

bool FeedbackDialog::evaluate() {
  if( m_Props == NULL )
    return false;

  iONode model  = wxGetApp().getModel();
  iONode fblist = wPlan.getfblist( model );

  if( existID( this, fblist, m_Props, m_Id->GetValue() ) ) {
    m_Id->SetValue( wxString(wItem.getid( m_Props ),wxConvUTF8) );
    return false;
  }
  // General
  wItem.setprev_id( m_Props, wItem.getid(m_Props) );
  wFeedback.setid( m_Props, m_Id->GetValue().mb_str(wxConvUTF8) );
  wFeedback.setdesc( m_Props, m_Description->GetValue().mb_str(wxConvUTF8) );
  wFeedback.setshow( m_Props, m_Show->GetValue()?True:False );
  wItem.setroad( m_Props, m_Road->IsChecked()?True:False );
  wFeedback.setcurve( m_Props, m_Curved->IsChecked()?True:False );
  wFeedback.setblockid( m_Props, m_BlockID->GetStringSelection().mb_str(wxConvUTF8) );
  wItem.setrouteids( m_Props, m_RouteIDs->GetValue().mb_str(wxConvUTF8) );
  wFeedback.setaccnr( m_Props, m_AccNr->GetValue() );
  wFeedback.settimer( m_Props, m_Timer->GetValue() );

  // Location
  wFeedback.setx( m_Props, atoi( m_x->GetValue().mb_str(wxConvUTF8) ) );
  wFeedback.sety( m_Props, atoi( m_y->GetValue().mb_str(wxConvUTF8) ) );
  wFeedback.setz( m_Props, atoi( m_z->GetValue().mb_str(wxConvUTF8) ) );
  int ori = m_ori->GetSelection();
  if( ori == 0 )
    wFeedback.setori( m_Props, wItem.north );
  else if( ori == 1 )
    wFeedback.setori( m_Props, wItem.east );
  else if( ori == 2 )
    wFeedback.setori( m_Props, wItem.south );
  else if( ori == 3 )
    wFeedback.setori( m_Props, wItem.west );

  wFeedback.setshow( m_Props , m_Show->GetValue() ? True:False);

  // Interface
  wFeedback.setiid( m_Props, m_iid->GetValue().mb_str(wxConvUTF8) );
  wFeedback.setbus( m_Props, atoi( m_BusNr->GetValue().mb_str(wxConvUTF8) ) );
  wFeedback.setaddr( m_Props, m_Address->GetValue() );
  wFeedback.setcutoutbus( m_Props, m_CutoutBus->GetValue() );
  wFeedback.setcutoutaddr( m_Props, m_CutoutAddr->GetValue() );

  wFeedback.setfbtype( m_Props, m_Type->GetSelection() );
  wFeedback.setactivelow( m_Props , m_ActiveLow->GetValue() ? True:False);
  wFeedback.setresetwc( m_Props , m_ResetWC->GetValue() ? True:False);

  // Wiring
  wFeedback.setctciid( m_Props, m_CTCIID->GetValue().mb_str(wxConvUTF8) );
  wFeedback.setctcaddr( m_Props, m_CTCAddr->GetValue() );
  wFeedback.setctcport( m_Props, m_CTCPort->GetValue() );
  wFeedback.setctcgate( m_Props, m_CTCGate->GetSelection() );

  wFeedback.setctcasswitch( m_Props, m_AsSwitch->GetValue()?True:False);

  // Action
  return true;
}

/*!
 * FeedbackDialog creator
 */

bool FeedbackDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin FeedbackDialog member initialisation
    m_Notebook = NULL;
    m_IndexPanel = NULL;
    m_List2 = NULL;
    m_New = NULL;
    m_Delete = NULL;
    m_Doc = NULL;
    m_General = NULL;
    m_LabelId = NULL;
    m_Id = NULL;
    m_labBlockID = NULL;
    m_BlockID = NULL;
    m_labRouteIDs = NULL;
    m_RouteIDs = NULL;
    m_LabelDescription = NULL;
    m_Description = NULL;
    m_labAccNr = NULL;
    m_AccNr = NULL;
    m_labTimer = NULL;
    m_Timer = NULL;
    m_State = NULL;
    m_Road = NULL;
    m_Show = NULL;
    m_Curved = NULL;
    m_Actions = NULL;
    m_LocationPanel = NULL;
    m_LabelX = NULL;
    m_x = NULL;
    m_LabelY = NULL;
    m_y = NULL;
    m_LabelZ = NULL;
    m_z = NULL;
    m_ori = NULL;
    m_Interface = NULL;
    m_Labeliid = NULL;
    m_iid = NULL;
    m_AddressBox = NULL;
    m_labBusNr = NULL;
    m_BusNr = NULL;
    m_LabelAddress = NULL;
    m_Address = NULL;
    m_CutoutBox = NULL;
    m_labCutoutBus = NULL;
    m_CutoutBus = NULL;
    m_labCutoutAddr = NULL;
    m_CutoutAddr = NULL;
    m_Type = NULL;
    m_OptionsBox = NULL;
    m_ActiveLow = NULL;
    m_ResetWC = NULL;
    m_Wiring = NULL;
    m_CTCBox = NULL;
    m_labCTCIID = NULL;
    m_CTCIID = NULL;
    m_labCTCAddr = NULL;
    m_CTCAddr = NULL;
    m_labCTCPort = NULL;
    m_CTCPort = NULL;
    m_labCTCGate = NULL;
    m_CTCGate = NULL;
    m_AsSwitch = NULL;
    m_Cancel = NULL;
    m_OK = NULL;
    m_Apply = NULL;
////@end FeedbackDialog member initialisation

////@begin FeedbackDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end FeedbackDialog creation
    return true;
}

/*!
 * Control creation for FeedbackDialog
 */

void FeedbackDialog::CreateControls()
{
////@begin FeedbackDialog content construction
    FeedbackDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_Notebook = new wxNotebook( itemDialog1, ID_NOTEBOOK_FEEDBACK, wxDefaultPosition, wxDefaultSize, m_TabAlign );

    m_IndexPanel = new wxPanel( m_Notebook, ID_PANEL_FB_INDEX, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    m_IndexPanel->SetSizer(itemBoxSizer5);

    m_List2 = new wxListCtrl( m_IndexPanel, ID_LISTCTRLINDEX_FB, wxDefaultPosition, wxSize(100, 100), wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_HRULES );
    itemBoxSizer5->Add(m_List2, 1, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer7 = new wxFlexGridSizer(0, 3, 0, 0);
    itemBoxSizer5->Add(itemFlexGridSizer7, 0, wxGROW|wxALL, 5);
    m_New = new wxButton( m_IndexPanel, ID_BUTTON_FB_NEW, _("New"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_New, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Delete = new wxButton( m_IndexPanel, ID_BUTTON_FB_DELETE, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_Delete, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Doc = new wxButton( m_IndexPanel, ID_BUTTON_FB_DOC, _("Doc"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_Doc, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_IndexPanel, _("Index"));

    m_General = new wxPanel( m_Notebook, ID_PANEL_FB_GENERAL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer12 = new wxBoxSizer(wxVERTICAL);
    m_General->SetSizer(itemBoxSizer12);

    wxFlexGridSizer* itemFlexGridSizer13 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer12->Add(itemFlexGridSizer13, 0, wxGROW|wxALL, 5);
    m_LabelId = new wxStaticText( m_General, wxID_STATIC_FB_ID, _("id"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer13->Add(m_LabelId, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Id = new wxTextCtrl( m_General, ID_TEXTCTRL_FB_ID, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer13->Add(m_Id, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labBlockID = new wxStaticText( m_General, wxID_ANY, _("BlockID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer13->Add(m_labBlockID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_BlockIDStrings;
    m_BlockID = new wxComboBox( m_General, ID_COMBOBOX_FB_BLOCKID, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_BlockIDStrings, wxCB_READONLY );
    itemFlexGridSizer13->Add(m_BlockID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labRouteIDs = new wxStaticText( m_General, wxID_ANY, _("Route IDs"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer13->Add(m_labRouteIDs, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_RouteIDs = new wxTextCtrl( m_General, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer13->Add(m_RouteIDs, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelDescription = new wxStaticText( m_General, wxID_STATIC_FB_DESC, _("description"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer13->Add(m_LabelDescription, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Description = new wxTextCtrl( m_General, ID_TEXTCTRL_FB_DESC, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer13->Add(m_Description, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labAccNr = new wxStaticText( m_General, wxID_ANY, _("accnr"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer13->Add(m_labAccNr, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_AccNr = new wxSpinCtrl( m_General, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 99, 0 );
    itemFlexGridSizer13->Add(m_AccNr, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labTimer = new wxStaticText( m_General, wxID_ANY, _("Timer"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer13->Add(m_labTimer, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Timer = new wxSpinCtrl( m_General, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer13->Add(m_Timer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemFlexGridSizer13->AddGrowableCol(1);

    wxFlexGridSizer* itemFlexGridSizer26 = new wxFlexGridSizer(0, 4, 0, 0);
    itemBoxSizer12->Add(itemFlexGridSizer26, 0, wxGROW|wxALL, 5);
    m_State = new wxCheckBox( m_General, ID_CHECKBOX_FB_STATE, _("state"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_State->SetValue(false);
    m_State->Enable(false);
    itemFlexGridSizer26->Add(m_State, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Road = new wxCheckBox( m_General, wxID_ANY, _("Road"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Road->SetValue(false);
    itemFlexGridSizer26->Add(m_Road, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Show = new wxCheckBox( m_General, ID_CHECKBOX_FB_SHOW, _("show"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_Show->SetValue(false);
    itemFlexGridSizer26->Add(m_Show, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Curved = new wxCheckBox( m_General, wxID_ANY, _("curved"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Curved->SetValue(false);
    itemFlexGridSizer26->Add(m_Curved, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Actions = new wxButton( m_General, ID_FEEDBACK_ACTIONS, _("Actions..."), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer12->Add(m_Actions, 0, wxALIGN_LEFT|wxALL, 5);

    m_Notebook->AddPage(m_General, _("General"));

    m_LocationPanel = new wxPanel( m_Notebook, ID_PANEL_FB_LOCATION, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer33 = new wxBoxSizer(wxHORIZONTAL);
    m_LocationPanel->SetSizer(itemBoxSizer33);

    wxFlexGridSizer* itemFlexGridSizer34 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer33->Add(itemFlexGridSizer34, 0, wxALIGN_TOP|wxALL, 5);
    m_LabelX = new wxStaticText( m_LocationPanel, wxID_STATIC_FB_X, _("x"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer34->Add(m_LabelX, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_x = new wxTextCtrl( m_LocationPanel, ID_TEXTCTRL_FB_X, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer34->Add(m_x, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelY = new wxStaticText( m_LocationPanel, wxID_STATIC_FB_Y, _("y"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer34->Add(m_LabelY, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_y = new wxTextCtrl( m_LocationPanel, ID_TEXTCTRL_FB_Y, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer34->Add(m_y, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelZ = new wxStaticText( m_LocationPanel, wxID_STATIC_FB_Z, _("z"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer34->Add(m_LabelZ, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_z = new wxTextCtrl( m_LocationPanel, ID_TEXTCTRL_FB_Z, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer34->Add(m_z, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_oriStrings;
    m_oriStrings.Add(_("&north"));
    m_oriStrings.Add(_("&east"));
    m_oriStrings.Add(_("&south"));
    m_oriStrings.Add(_("&west"));
    m_ori = new wxRadioBox( m_LocationPanel, ID_RADIOBOX_FB_ORI, _("Orientation"), wxDefaultPosition, wxDefaultSize, m_oriStrings, 1, wxRA_SPECIFY_COLS );
    m_ori->SetSelection(0);
    itemBoxSizer33->Add(m_ori, 0, wxALIGN_TOP|wxALL, 5);

    m_Notebook->AddPage(m_LocationPanel, _("Location"));

    m_Interface = new wxPanel( m_Notebook, ID_PANEL_FB_INTERFACE, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer43 = new wxBoxSizer(wxVERTICAL);
    m_Interface->SetSizer(itemBoxSizer43);

    wxFlexGridSizer* itemFlexGridSizer44 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer43->Add(itemFlexGridSizer44, 0, wxGROW|wxALL, 5);
    m_Labeliid = new wxStaticText( m_Interface, wxID_STATIC_FB_IID, _("iid"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer44->Add(m_Labeliid, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_iid = new wxTextCtrl( m_Interface, ID_TEXTCTRL_FB_IID, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer44->Add(m_iid, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemFlexGridSizer44->AddGrowableCol(1);

    m_AddressBox = new wxStaticBox(m_Interface, wxID_ANY, _("Address"));
    wxStaticBoxSizer* itemStaticBoxSizer47 = new wxStaticBoxSizer(m_AddressBox, wxVERTICAL);
    itemBoxSizer43->Add(itemStaticBoxSizer47, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer48 = new wxFlexGridSizer(0, 4, 0, 0);
    itemStaticBoxSizer47->Add(itemFlexGridSizer48, 0, wxALIGN_LEFT, 5);
    m_labBusNr = new wxStaticText( m_Interface, wxID_ANY, _("Bus"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer48->Add(m_labBusNr, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_BusNr = new wxTextCtrl( m_Interface, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(120, -1), 0 );
    itemFlexGridSizer48->Add(m_BusNr, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelAddress = new wxStaticText( m_Interface, wxID_STATIC_FB_ADDRESS1, _("Address"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer48->Add(m_LabelAddress, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Address = new wxSpinCtrl( m_Interface, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 0, 65535, 0 );
    itemFlexGridSizer48->Add(m_Address, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemFlexGridSizer48->AddGrowableCol(1);
    itemFlexGridSizer48->AddGrowableCol(3);

    m_CutoutBox = new wxStaticBox(m_Interface, wxID_ANY, _("Cutout"));
    wxStaticBoxSizer* itemStaticBoxSizer53 = new wxStaticBoxSizer(m_CutoutBox, wxVERTICAL);
    itemBoxSizer43->Add(itemStaticBoxSizer53, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer54 = new wxFlexGridSizer(0, 4, 0, 0);
    itemStaticBoxSizer53->Add(itemFlexGridSizer54, 0, wxALIGN_LEFT, 5);
    m_labCutoutBus = new wxStaticText( m_Interface, wxID_ANY, _("Bus"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer54->Add(m_labCutoutBus, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_CutoutBus = new wxSpinCtrl( m_Interface, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 0, 65535, 0 );
    itemFlexGridSizer54->Add(m_CutoutBus, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labCutoutAddr = new wxStaticText( m_Interface, wxID_ANY, _("Address"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer54->Add(m_labCutoutAddr, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_CutoutAddr = new wxSpinCtrl( m_Interface, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 0, 65535, 0 );
    itemFlexGridSizer54->Add(m_CutoutAddr, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_TypeStrings;
    m_TypeStrings.Add(_("&Sensor"));
    m_TypeStrings.Add(_("&Lissy"));
    m_TypeStrings.Add(_("&Transponding"));
    m_TypeStrings.Add(_("&Barcode"));
    m_TypeStrings.Add(_("&Railcom"));
    m_TypeStrings.Add(_("&RFID"));
    m_TypeStrings.Add(_("&Wheel counter"));
    m_Type = new wxRadioBox( m_Interface, ID_FB_TYPE, _("Type"), wxDefaultPosition, wxDefaultSize, m_TypeStrings, 2, wxRA_SPECIFY_COLS );
    m_Type->SetSelection(0);
    itemBoxSizer43->Add(m_Type, 0, wxALIGN_LEFT|wxALL, 5);

    m_OptionsBox = new wxStaticBox(m_Interface, wxID_ANY, _("Options"));
    wxStaticBoxSizer* itemStaticBoxSizer60 = new wxStaticBoxSizer(m_OptionsBox, wxHORIZONTAL);
    itemBoxSizer43->Add(itemStaticBoxSizer60, 0, wxGROW|wxALL, 5);
    m_ActiveLow = new wxCheckBox( m_Interface, wxID_ANY, _("Active low"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ActiveLow->SetValue(false);
    itemStaticBoxSizer60->Add(m_ActiveLow, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_ResetWC = new wxCheckBox( m_Interface, wxID_ANY, _("Reset"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ResetWC->SetValue(false);
    itemStaticBoxSizer60->Add(m_ResetWC, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Notebook->AddPage(m_Interface, _("Interface"));

    m_Wiring = new wxPanel( m_Notebook, ID_PANEL_FB_WIRING, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_CTCBox = new wxStaticBox(m_Wiring, wxID_ANY, _("CTC"));
    wxStaticBoxSizer* itemStaticBoxSizer64 = new wxStaticBoxSizer(m_CTCBox, wxVERTICAL);
    m_Wiring->SetSizer(itemStaticBoxSizer64);

    wxFlexGridSizer* itemFlexGridSizer65 = new wxFlexGridSizer(0, 2, 0, 0);
    itemStaticBoxSizer64->Add(itemFlexGridSizer65, 0, wxGROW|wxALL, 5);
    m_labCTCIID = new wxStaticText( m_Wiring, wxID_STATIC, _("IID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer65->Add(m_labCTCIID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_CTCIID = new wxTextCtrl( m_Wiring, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer65->Add(m_CTCIID, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labCTCAddr = new wxStaticText( m_Wiring, wxID_ANY, _("Address"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer65->Add(m_labCTCAddr, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_CTCAddr = new wxSpinCtrl( m_Wiring, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 10000, 0 );
    itemFlexGridSizer65->Add(m_CTCAddr, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labCTCPort = new wxStaticText( m_Wiring, wxID_ANY, _("Port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer65->Add(m_labCTCPort, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_CTCPort = new wxSpinCtrl( m_Wiring, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 10000, 0 );
    itemFlexGridSizer65->Add(m_CTCPort, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labCTCGate = new wxStaticText( m_Wiring, wxID_ANY, _("Gate"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer65->Add(m_labCTCGate, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_CTCGateStrings;
    m_CTCGateStrings.Add(_("&Red"));
    m_CTCGateStrings.Add(_("&Green"));
    m_CTCGate = new wxRadioBox( m_Wiring, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_CTCGateStrings, 1, wxRA_SPECIFY_ROWS );
    m_CTCGate->SetSelection(0);
    itemFlexGridSizer65->Add(m_CTCGate, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_AsSwitch = new wxCheckBox( m_Wiring, wxID_ANY, _("Switch"), wxDefaultPosition, wxDefaultSize, 0 );
    m_AsSwitch->SetValue(false);
    itemStaticBoxSizer64->Add(m_AsSwitch, 0, wxALIGN_LEFT|wxALL, 5);

    m_Notebook->AddPage(m_Wiring, _("Wiring"));

    itemBoxSizer2->Add(m_Notebook, 1, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer75 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer75, 0, wxALIGN_RIGHT|wxALL, 5);
    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer75->AddButton(m_Cancel);

    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer75->AddButton(m_OK);

    m_Apply = new wxButton( itemDialog1, wxID_APPLY, _("&Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer75->AddButton(m_Apply);

    itemStdDialogButtonSizer75->Realize();

////@end FeedbackDialog content construction
}

/*!
 * Should we show tooltips?
 */

bool FeedbackDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap FeedbackDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin FeedbackDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end FeedbackDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon FeedbackDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin FeedbackDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end FeedbackDialog icon retrieval
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
 */

void FeedbackDialog::OnOkClick( wxCommandEvent& event )
{
  OnApplyClick(event);
  EndModal( wxID_OK );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void FeedbackDialog::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}





/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SW_NEW
 */

void FeedbackDialog::OnButtonFbNewClick( wxCommandEvent& event )
{
  int i = findID("NEW");
  if( i == wxNOT_FOUND ) {
    iONode model = wxGetApp().getModel();
    if( model != NULL ) {
      iONode fblist = wPlan.getfblist( model );
      if( fblist == NULL ) {
        fblist = NodeOp.inst( wFeedbackList.name(), model, ELEMENT_NODE );
        NodeOp.addChild( model, fblist );
      }
      if( fblist != NULL ) {
        iONode fb = NodeOp.inst( wFeedback.name(), fblist, ELEMENT_NODE );
        NodeOp.addChild( fblist, fb );
        wFeedback.setid( fb, "NEW" );
        appendItem(fb);
        setIDSelection(wItem.getid(fb));
        m_Props = fb;
        initValues();
      }
    }
  }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SW_DELETE
 */

void FeedbackDialog::OnButtonFbDeleteClick( wxCommandEvent& event )
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
    iONode fblist = wPlan.getfblist( model );
    if( fblist != NULL ) {
      NodeOp.removeChild( fblist, m_Props );
      m_Props = selectPrev();
    }
  }

  initIndex();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
 */

void FeedbackDialog::OnApplyClick( wxCommandEvent& event )
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
  initIndex();
}


void FeedbackDialog::OnButtonFbDocClick( wxCommandEvent& event )
{
  doDoc( event, "feedbacks");
}



/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_FB_ACTIOINS
 */

void FeedbackDialog::OnFeedbackActionsClick( wxCommandEvent& event )
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
 * wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_FB_BUS
 */

void FeedbackDialog::OnFbTypeSelected( wxCommandEvent& event )
{
}


/*!
 * wxEVT_COMMAND_LIST_ITEM_SELECTED event handler for ID_LISTCTRLINDEX
 */

void FeedbackDialog::OnListctrlindexFbSelected( wxListEvent& event )
{
  m_Props = getSelection(event.GetIndex());
  initValues();
  m_Delete->Enable( true );
}


/*!
 * wxEVT_COMMAND_LIST_COL_CLICK event handler for ID_LISTCTRLINDEX
 */

void FeedbackDialog::OnListctrlindexFbColLeftClick( wxListEvent& event )
{
  sortOnColumn(event.GetColumn());
}

