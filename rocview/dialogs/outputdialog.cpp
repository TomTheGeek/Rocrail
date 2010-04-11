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
#pragma implementation "outputdialog.h"
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

#include "outputdialog.h"
#include "actionsctrldlg.h"

////@begin XPM images
////@end XPM images
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/OutputList.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocview/public/guiapp.h"

/*!
 * OutputDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( OutputDialog, wxDialog )

/*!
 * OutputDialog event table definition
 */

BEGIN_EVENT_TABLE( OutputDialog, wxDialog )

////@begin OutputDialog event table entries
    EVT_NOTEBOOK_PAGE_CHANGED( ID_NOTEBOOK_CO, OutputDialog::OnNotebookCoPageChanged )

    EVT_LISTBOX( ID_LISTBOX_CO, OutputDialog::OnListboxCoSelected )

    EVT_BUTTON( ID_BUTTON_CO_NEW, OutputDialog::OnButtonCoNewClick )

    EVT_BUTTON( ID_BUTTON_CO_DELETE, OutputDialog::OnButtonCoDeleteClick )

    EVT_BUTTON( ID_OUTPUT_ACTIONS, OutputDialog::OnOutputActionsClick )

    EVT_CHECKBOX( ID_OUTPUT_SWITCH, OutputDialog::OnOutputSwitchClick )

    EVT_BUTTON( wxID_CANCEL, OutputDialog::OnCancelClick )

    EVT_BUTTON( wxID_OK, OutputDialog::OnOkClick )

    EVT_BUTTON( wxID_APPLY, OutputDialog::OnApplyClick )

////@end OutputDialog event table entries
    EVT_MENU( ID_PANEL_CO_GENERAL, OutputDialog::OnSelectPage )

END_EVENT_TABLE()

/*!
 * OutputDialog constructors
 */

OutputDialog::OutputDialog( )
{
}

OutputDialog::OutputDialog( wxWindow* parent, iONode p_Props )
{
  m_TabAlign = wxGetApp().getTabAlign();
  Create(parent, -1, wxGetApp().getMsg("outputtable") );

  m_Props = p_Props;

  initLabels();

  m_IndexPanel->GetSizer()->Layout();
  m_GeneralPanel->GetSizer()->Layout();
  m_InterfacePanel->GetSizer()->Layout();
  m_Notebook->Fit();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  initIndex();

  if( m_Props != NULL ) {
    initValues();
    //m_Notebook->SetSelection( 1 );
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_PANEL_CO_GENERAL );
    wxPostEvent( this, event );
  }

}

void OutputDialog::initLabels() {
  m_Notebook->SetPageText( 0, wxGetApp().getMsg( "index" ) );
  m_Notebook->SetPageText( 1, wxGetApp().getMsg( "general" ) );
  m_Notebook->SetPageText( 2, wxGetApp().getMsg( "interface" ) );
  m_Notebook->SetPageText( 3, wxGetApp().getMsg( "position" ) );

  // Index
  m_New->SetLabel( wxGetApp().getMsg( "new" ) );
  m_Delete->SetLabel( wxGetApp().getMsg( "delete" ) );

  // General
  m_LabelID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_LabelDescription->SetLabel( wxGetApp().getMsg( "description" ) );
  m_Show->SetLabel( wxGetApp().getMsg( "visible" ) );
  m_labSVG->SetLabel( wxGetApp().getMsg( "svg" ) );
  m_Actions->SetLabel( wxGetApp().getMsg( "actions" )+_T("...") );
  m_labRouteIDs->SetLabel( wxGetApp().getMsg( "routeids" ) );
  m_labBlockID->SetLabel( wxGetApp().getMsg( "blockid" ) );
  m_TriState->SetLabel( wxGetApp().getMsg( "tristate" ) );
  m_Type->SetLabel( wxGetApp().getMsg( "type" ) );
  m_Type->SetString( 0, wxGetApp().getMsg( "toggleswitch" ) );
  m_Type->SetString( 1, wxGetApp().getMsg( "pushbutton" ) );

  // Interface
  m_Labeliid->SetLabel( wxGetApp().getMsg( "iid" ) );
  m_Label_Bus->SetLabel( wxGetApp().getMsg( "bus" ) );
  m_LabelAddress->SetLabel( wxGetApp().getMsg( "address" ) );
  m_LabelPort->SetLabel( wxGetApp().getMsg( "port" ) );
  m_labGate->SetLabel( wxGetApp().getMsg( "gate" ) );
  m_Gate->SetString( 0, wxGetApp().getMsg( "red" ) );
  m_Gate->SetString( 1, wxGetApp().getMsg( "green" ) );
  m_LabelProt->SetLabel( wxGetApp().getMsg( "protocol" ) );
  m_Prot->Clear();
  m_Prot->Append(_T("Default"));
  m_Prot->Append(_T("Motorola"));
  m_Prot->Append(_T("NMRA-DCC"));
  m_Prot->Append(_T("Dinamo MDD"));
  m_Prot->Append(_T("Dinamo OM32"));
  m_Prot->Append(_T("Dinamo DO"));
  m_Prot->Append(_T("Dinamo VO"));

  m_OptionsBox->SetLabel(wxGetApp().getMsg( "options"));

  m_Invert->SetLabel( wxGetApp().getMsg( "invert" ) );
  m_AsSwitch->SetLabel( wxGetApp().getMsg( "turnout" ) );

  // Location
  m_LabelX->SetLabel( wxGetApp().getMsg( "x" ) );
  m_LabelY->SetLabel( wxGetApp().getMsg( "y" ) );
  m_LabelZ->SetLabel( wxGetApp().getMsg( "z" ) );
  m_ori->SetLabel( wxGetApp().getMsg( "orientation" ) );
  m_ori->SetString( 0, wxGetApp().getMsg( "north" ) );
  m_ori->SetString( 1, wxGetApp().getMsg( "east" ) );
  m_ori->SetString( 2, wxGetApp().getMsg( "south" ) );
  m_ori->SetString( 3, wxGetApp().getMsg( "west" ) );

  // Buttons
  m_OK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
  m_Apply->SetLabel( wxGetApp().getMsg( "apply" ) );

  m_BlockID->Append( _T("") );
  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode bklist = wPlan.getbklist( model );
    if( bklist != NULL ) {
      int cnt = NodeOp.getChildCnt( bklist );
      for( int i = 0; i < cnt; i++ ) {
        iONode bk = NodeOp.getChild( bklist, i );
        m_BlockID->Append( wxString(wBlock.getid( bk ),wxConvUTF8), bk );
      }
    }
    iONode fblist = wPlan.getfblist( model );
    if( fblist != NULL ) {
      int cnt = NodeOp.getChildCnt( fblist );
      for( int i = 0; i < cnt; i++ ) {
        iONode fb = NodeOp.getChild( fblist, i );
        m_BlockID->Append( wxString(wFeedback.getid( fb ),wxConvUTF8), fb );
      }
    }
  }


}


void OutputDialog::OnSelectPage(wxCommandEvent& event) {
  m_Notebook->SetSelection( 1 );
}

void OutputDialog::initIndex() {
  TraceOp.trc( "app", TRCLEVEL_INFO, __LINE__, 9999, "InitIndex" );
  iONode l_Props = m_Props;
  m_List->Clear();
  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode colist = wPlan.getcolist( model );
    if( colist != NULL ) {
      int cnt = NodeOp.getChildCnt( colist );
      for( int i = 0; i < cnt; i++ ) {
        iONode co = NodeOp.getChild( colist, i );
        const char* id = wOutput.getid( co );
        if( id != NULL ) {
          m_List->Append( wxString(id,wxConvUTF8) );
        }
      }
      if( l_Props != NULL ) {
        m_List->SetStringSelection( wxString(wOutput.getid( l_Props ),wxConvUTF8) );
        m_List->SetFirstItem( wxString(wOutput.getid( l_Props ),wxConvUTF8) );
        m_Props = l_Props;
      }
    }
  }
}

void OutputDialog::initValues() {
  char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("output").mb_str(wxConvUTF8), wOutput.getid( m_Props ) );
  SetTitle( wxString(title,wxConvUTF8) );
  StrOp.free( title );

  // General
  m_ID->SetValue( wxString(wOutput.getid( m_Props ),wxConvUTF8) );
  m_Description->SetValue( wxString(wOutput.getdesc( m_Props ),wxConvUTF8) );
  m_Show->SetValue( wOutput.isshow( m_Props ) ? true:false);
  m_SVG->SetValue( wOutput.getsvgtype( m_Props ) );
  m_BlockID->SetStringSelection( wOutput.getblockid( m_Props ) == NULL ?
                                _T(""):wxString(wOutput.getblockid( m_Props ),wxConvUTF8)  );
  m_RouteIDs->SetValue( wxString(wItem.getrouteids( m_Props ),wxConvUTF8) );
  m_TriState->SetValue( wOutput.istristate( m_Props ) ? true:false);
  m_Type->SetSelection( wOutput.istoggleswitch(m_Props) ? 0:1);

  // Interface
  m_iid->SetValue( wxString(wOutput.getiid( m_Props ),wxConvUTF8) );
  char* str = StrOp.fmt( "%d", wOutput.getaddr(m_Props) );
  m_Address->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wOutput.getbus(m_Props) );
  m_Bus->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wOutput.getport(m_Props) );
  m_Port->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  m_Gate->SetSelection( wOutput.getgate(m_Props) );

  if( StrOp.equals( wOutput.prot_M, wOutput.getprot( m_Props ) ) )
    m_Prot->SetSelection( 1 );
  else if( StrOp.equals( wOutput.prot_N, wOutput.getprot( m_Props ) ) )
    m_Prot->SetSelection( 2 );
  else if( StrOp.equals( wOutput.prot_MDD, wOutput.getprot( m_Props ) ) )
    m_Prot->SetSelection( 3 );
  else if( StrOp.equals( wOutput.prot_OM32, wOutput.getprot( m_Props ) ) )
    m_Prot->SetSelection( 4 );
  else if( StrOp.equals( wOutput.prot_DO, wOutput.getprot( m_Props ) ) )
    m_Prot->SetSelection( 5 );
  else if( StrOp.equals( wOutput.prot_VO, wOutput.getprot( m_Props ) ) )
    m_Prot->SetSelection( 6 );
  else
    m_Prot->SetSelection( 0 );

  m_Invert->SetValue( wOutput.isinv( m_Props )?true:false );
  m_AsSwitch->SetValue( wOutput.isasswitch( m_Props )?true:false );
  m_Gate->Enable(!m_AsSwitch->GetValue());


  // Location
  char* val = StrOp.fmt( "%d", wOutput.getx( m_Props ) );
  m_x->SetValue( wxString(val,wxConvUTF8) ); StrOp.free( val );
  val = StrOp.fmt( "%d", wOutput.gety( m_Props ) );
  m_y->SetValue( wxString(val,wxConvUTF8) ); StrOp.free( val );
  val = StrOp.fmt( "%d", wOutput.getz( m_Props ) );
  m_z->SetValue( wxString(val,wxConvUTF8) ); StrOp.free( val );
  if( StrOp.equals( wItem.north, wOutput.getori( m_Props ) ) )
    m_ori->SetSelection( 0 );
  else if( StrOp.equals( wItem.east, wOutput.getori( m_Props ) ) )
    m_ori->SetSelection( 1 );
  else if( StrOp.equals( wItem.south, wOutput.getori( m_Props ) ) )
    m_ori->SetSelection( 2 );
  else
    m_ori->SetSelection( 3 );


}


bool OutputDialog::evaluate() {
  if( m_Props == NULL )
    return false;

  if( m_ID->GetValue().Len() == 0 ) {
    wxMessageDialog( this, wxGetApp().getMsg("invalidid"), _T("Rocrail"), wxOK | wxICON_ERROR ).ShowModal();
    m_ID->SetValue( wxString(wOutput.getid( m_Props ),wxConvUTF8) );
    return false;
  }
  // General
  wOutput.setid( m_Props, m_ID->GetValue().mb_str(wxConvUTF8) );
  wOutput.setdesc( m_Props, m_Description->GetValue().mb_str(wxConvUTF8) );
  wOutput.setshow( m_Props , m_Show->GetValue() ? True:False);
  wOutput.setsvgtype( m_Props , m_SVG->GetValue());
  wOutput.setblockid( m_Props, m_BlockID->GetStringSelection().mb_str(wxConvUTF8) );
  wItem.setrouteids( m_Props, m_RouteIDs->GetValue().mb_str(wxConvUTF8) );
  wOutput.settristate( m_Props , m_TriState->GetValue() ? True:False);
  wOutput.settoggleswitch( m_Props , m_Type->GetSelection() == 0 ? True:False);

  // Interface
  wOutput.setiid( m_Props, m_iid->GetValue().mb_str(wxConvUTF8) );
  wOutput.setbus( m_Props, atoi( m_Bus->GetValue().mb_str(wxConvUTF8) ) );
  wOutput.setaddr( m_Props, atoi( m_Address->GetValue().mb_str(wxConvUTF8) ) );
  wOutput.setport( m_Props, atoi( m_Port->GetValue().mb_str(wxConvUTF8) ) );
  wOutput.setgate( m_Props, m_Gate->GetSelection() );

  if( m_Prot->GetSelection() == 1 )
    wOutput.setprot( m_Props, wOutput.prot_M );
  else if( m_Prot->GetSelection() == 2 )
    wOutput.setprot( m_Props, wOutput.prot_N );
  else if( m_Prot->GetSelection() == 3 )
    wOutput.setprot( m_Props, wOutput.prot_MDD );
  else if( m_Prot->GetSelection() == 4 )
    wOutput.setprot( m_Props, wOutput.prot_OM32 );
  else if( m_Prot->GetSelection() == 5 )
    wOutput.setprot( m_Props, wOutput.prot_DO );
  else if( m_Prot->GetSelection() == 6 )
    wOutput.setprot( m_Props, wOutput.prot_VO );
  else
    wOutput.setprot( m_Props, wOutput.prot_DEF );

  wOutput.setinv( m_Props, m_Invert->GetValue()?True:False);
  wOutput.setasswitch( m_Props, m_AsSwitch->GetValue()?True:False);

  // Location
  wOutput.setx( m_Props, atoi( m_x->GetValue().mb_str(wxConvUTF8) ) );
  wOutput.sety( m_Props, atoi( m_y->GetValue().mb_str(wxConvUTF8) ) );
  wOutput.setz( m_Props, atoi( m_z->GetValue().mb_str(wxConvUTF8) ) );
  int ori = m_ori->GetSelection();
  if( ori == 0 )
    wOutput.setori( m_Props, wItem.north );
  else if( ori == 1 )
    wOutput.setori( m_Props, wItem.east );
  else if( ori == 2 )
    wOutput.setori( m_Props, wItem.south );
  else if( ori == 3 )
    wOutput.setori( m_Props, wItem.west );

  return true;

}




/*!
 * OutputDialog creator
 */

bool OutputDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin OutputDialog member initialisation
    m_Notebook = NULL;
    m_IndexPanel = NULL;
    m_List = NULL;
    m_New = NULL;
    m_Delete = NULL;
    m_GeneralPanel = NULL;
    m_LabelID = NULL;
    m_ID = NULL;
    m_LabelDescription = NULL;
    m_Description = NULL;
    m_labSVG = NULL;
    m_SVG = NULL;
    m_labBlockID = NULL;
    m_BlockID = NULL;
    m_labRouteIDs = NULL;
    m_RouteIDs = NULL;
    m_Show = NULL;
    m_TriState = NULL;
    m_Type = NULL;
    m_Actions = NULL;
    m_InterfacePanel = NULL;
    m_Labeliid = NULL;
    m_iid = NULL;
    m_Label_Bus = NULL;
    m_Bus = NULL;
    m_LabelProt = NULL;
    m_Prot = NULL;
    m_LabelAddress = NULL;
    m_Address = NULL;
    m_LabelPort = NULL;
    m_Port = NULL;
    m_labGate = NULL;
    m_Gate = NULL;
    m_OptionsBox = NULL;
    m_AsSwitch = NULL;
    m_Invert = NULL;
    m_LocationPanel = NULL;
    m_LabelX = NULL;
    m_x = NULL;
    m_LabelY = NULL;
    m_y = NULL;
    m_LabelZ = NULL;
    m_z = NULL;
    m_ori = NULL;
    m_Cancel = NULL;
    m_OK = NULL;
    m_Apply = NULL;
////@end OutputDialog member initialisation

////@begin OutputDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end OutputDialog creation
    return true;
}

/*!
 * Control creation for OutputDialog
 */

void OutputDialog::CreateControls()
{
////@begin OutputDialog content construction
    OutputDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_Notebook = new wxNotebook( itemDialog1, ID_NOTEBOOK_CO, wxDefaultPosition, wxDefaultSize, m_TabAlign );

    m_IndexPanel = new wxPanel( m_Notebook, ID_PANEL_CO_INDEX, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    m_IndexPanel->SetSizer(itemBoxSizer5);

    wxArrayString m_ListStrings;
    m_List = new wxListBox( m_IndexPanel, ID_LISTBOX_CO, wxDefaultPosition, wxDefaultSize, m_ListStrings, wxLB_SINGLE|wxLB_ALWAYS_SB|wxLB_SORT );
    itemBoxSizer5->Add(m_List, 1, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer7 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer5->Add(itemFlexGridSizer7, 0, wxGROW|wxALL, 5);
    m_New = new wxButton( m_IndexPanel, ID_BUTTON_CO_NEW, _("New"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_New, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Delete = new wxButton( m_IndexPanel, ID_BUTTON_CO_DELETE, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_Delete, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_IndexPanel, _("Index"));

    m_GeneralPanel = new wxPanel( m_Notebook, ID_PANEL_CO_GENERAL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer11 = new wxBoxSizer(wxVERTICAL);
    m_GeneralPanel->SetSizer(itemBoxSizer11);

    wxFlexGridSizer* itemFlexGridSizer12 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer12->AddGrowableCol(1);
    itemBoxSizer11->Add(itemFlexGridSizer12, 0, wxGROW|wxALL, 5);
    m_LabelID = new wxStaticText( m_GeneralPanel, wxID_STATIC_CO_ID, _("id"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_LabelID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ID = new wxTextCtrl( m_GeneralPanel, ID_TEXTCTRL_CO_ID, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_ID, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelDescription = new wxStaticText( m_GeneralPanel, wxID_STATIC_CO_DESC, _("desrciption"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_LabelDescription, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Description = new wxTextCtrl( m_GeneralPanel, ID_TEXTCTRL_CO_DESC, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_Description, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labSVG = new wxStaticText( m_GeneralPanel, wxID_ANY, _("SVG"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_labSVG, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SVG = new wxSpinCtrl( m_GeneralPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 99, 0 );
    itemFlexGridSizer12->Add(m_SVG, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labBlockID = new wxStaticText( m_GeneralPanel, wxID_ANY, _("Block ID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_labBlockID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_BlockIDStrings;
    m_BlockID = new wxComboBox( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_BlockIDStrings, wxCB_DROPDOWN );
    itemFlexGridSizer12->Add(m_BlockID, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labRouteIDs = new wxStaticText( m_GeneralPanel, wxID_ANY, _("Route ID's"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_labRouteIDs, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_RouteIDs = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_RouteIDs, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Show = new wxCheckBox( m_GeneralPanel, ID_CHECKBOX_CO_SHOW, _("visible"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_Show->SetValue(false);
    itemBoxSizer11->Add(m_Show, 0, wxALIGN_LEFT|wxALL, 5);

    m_TriState = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("Tri-State"), wxDefaultPosition, wxDefaultSize, 0 );
    m_TriState->SetValue(false);
    itemBoxSizer11->Add(m_TriState, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxArrayString m_TypeStrings;
    m_TypeStrings.Add(_("&Toggle switch"));
    m_TypeStrings.Add(_("&Push button"));
    m_Type = new wxRadioBox( m_GeneralPanel, wxID_ANY, _("Type"), wxDefaultPosition, wxDefaultSize, m_TypeStrings, 1, wxRA_SPECIFY_ROWS );
    m_Type->SetSelection(0);
    itemBoxSizer11->Add(m_Type, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Actions = new wxButton( m_GeneralPanel, ID_OUTPUT_ACTIONS, _("Actions..."), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer11->Add(m_Actions, 0, wxALIGN_LEFT|wxALL, 5);

    m_Notebook->AddPage(m_GeneralPanel, _("General"));

    m_InterfacePanel = new wxPanel( m_Notebook, ID_PANEL_CO_INTERFACE, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxVERTICAL);
    m_InterfacePanel->SetSizer(itemBoxSizer28);

    wxFlexGridSizer* itemFlexGridSizer29 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer29->AddGrowableCol(1);
    itemBoxSizer28->Add(itemFlexGridSizer29, 0, wxGROW|wxALL, 5);
    m_Labeliid = new wxStaticText( m_InterfacePanel, wxID_STATIC_CO_IID, _("iid"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer29->Add(m_Labeliid, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_iid = new wxTextCtrl( m_InterfacePanel, ID_TEXTCTRL_CO_IID, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer29->Add(m_iid, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Label_Bus = new wxStaticText( m_InterfacePanel, wxID_STATIC_CO_BUS, _("Bus:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer29->Add(m_Label_Bus, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Bus = new wxTextCtrl( m_InterfacePanel, ID_TEXTCTRL_CO_BUS, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer29->Add(m_Bus, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelProt = new wxStaticText( m_InterfacePanel, wxID_STATIC_CO_PROT, _("Protocol"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer29->Add(m_LabelProt, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxArrayString m_ProtStrings;
    m_ProtStrings.Add(_("Motorola"));
    m_ProtStrings.Add(_("NMRA-DCC"));
    m_Prot = new wxChoice( m_InterfacePanel, ID_CHOICE_CO_PROT, wxDefaultPosition, wxDefaultSize, m_ProtStrings, 0 );
    m_Prot->SetStringSelection(_("Motorola"));
    itemFlexGridSizer29->Add(m_Prot, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelAddress = new wxStaticText( m_InterfacePanel, wxID_STATIC_CO_ADDRESS, _("address"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer29->Add(m_LabelAddress, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Address = new wxTextCtrl( m_InterfacePanel, ID_TEXTCTRL_CO_ADDRESS, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer29->Add(m_Address, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelPort = new wxStaticText( m_InterfacePanel, wxID_STATIC_CO_PORT, _("port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer29->Add(m_LabelPort, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Port = new wxTextCtrl( m_InterfacePanel, ID_TEXTCTRL_CO_PORT, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer29->Add(m_Port, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labGate = new wxStaticText( m_InterfacePanel, wxID_STATIC_CO_GATE, _("gate"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer29->Add(m_labGate, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxArrayString m_GateStrings;
    m_GateStrings.Add(_("&Red"));
    m_GateStrings.Add(_("&Green"));
    m_Gate = new wxRadioBox( m_InterfacePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_GateStrings, 1, wxRA_SPECIFY_ROWS );
    m_Gate->SetSelection(0);
    itemFlexGridSizer29->Add(m_Gate, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_OptionsBox = new wxStaticBox(m_InterfacePanel, wxID_ANY, _("Options"));
    wxStaticBoxSizer* itemStaticBoxSizer42 = new wxStaticBoxSizer(m_OptionsBox, wxVERTICAL);
    itemBoxSizer28->Add(itemStaticBoxSizer42, 0, wxGROW|wxALL, 5);
    m_AsSwitch = new wxCheckBox( m_InterfacePanel, ID_OUTPUT_SWITCH, _("Switch"), wxDefaultPosition, wxDefaultSize, 0 );
    m_AsSwitch->SetValue(false);
    itemStaticBoxSizer42->Add(m_AsSwitch, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Invert = new wxCheckBox( m_InterfacePanel, wxID_ANY, _("Invert"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Invert->SetValue(false);
    itemStaticBoxSizer42->Add(m_Invert, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_Notebook->AddPage(m_InterfacePanel, _("Interface"));

    m_LocationPanel = new wxPanel( m_Notebook, ID_PANEL_CO_LOCATION, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer46 = new wxBoxSizer(wxHORIZONTAL);
    m_LocationPanel->SetSizer(itemBoxSizer46);

    wxFlexGridSizer* itemFlexGridSizer47 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer46->Add(itemFlexGridSizer47, 0, wxALIGN_TOP|wxALL, 5);
    m_LabelX = new wxStaticText( m_LocationPanel, wxID_STATIC_CO_X, _("x"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer47->Add(m_LabelX, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_x = new wxTextCtrl( m_LocationPanel, ID_TEXTCTRL_CO_X, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer47->Add(m_x, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelY = new wxStaticText( m_LocationPanel, wxID_STATIC_CO_Y, _("y"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer47->Add(m_LabelY, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_y = new wxTextCtrl( m_LocationPanel, ID_TEXTCTRL_CO_Y, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer47->Add(m_y, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelZ = new wxStaticText( m_LocationPanel, wxID_STATIC_CO_Z, _("z"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer47->Add(m_LabelZ, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_z = new wxTextCtrl( m_LocationPanel, ID_TEXTCTRL_CO_Z, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer47->Add(m_z, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_oriStrings;
    m_oriStrings.Add(_("&north"));
    m_oriStrings.Add(_("&east"));
    m_oriStrings.Add(_("&south"));
    m_oriStrings.Add(_("&west"));
    m_ori = new wxRadioBox( m_LocationPanel, ID_RADIOBOX_CO_ORI, _("Orientation"), wxDefaultPosition, wxDefaultSize, m_oriStrings, 1, wxRA_SPECIFY_COLS );
    m_ori->SetSelection(0);
    itemBoxSizer46->Add(m_ori, 0, wxALIGN_TOP|wxALL, 5);

    m_Notebook->AddPage(m_LocationPanel, _("Location"));

    itemBoxSizer2->Add(m_Notebook, 1, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer55 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer55, 0, wxALIGN_RIGHT|wxALL, 5);
    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer55->AddButton(m_Cancel);

    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer55->AddButton(m_OK);

    m_Apply = new wxButton( itemDialog1, wxID_APPLY, _("&Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer55->AddButton(m_Apply);

    itemStdDialogButtonSizer55->Realize();

////@end OutputDialog content construction
}

/*!
 * Should we show tooltips?
 */

bool OutputDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap OutputDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin OutputDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end OutputDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon OutputDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin OutputDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end OutputDialog icon retrieval
}
/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_CO
 */

void OutputDialog::OnListboxCoSelected( wxCommandEvent& event )
{
  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode colist = wPlan.getcolist( model );
    if( colist != NULL ) {
      int cnt = NodeOp.getChildCnt( colist );
      for( int i = 0; i < cnt; i++ ) {
        iONode co = NodeOp.getChild( colist, i );
        const char* id = wOutput.getid( co );
        if( id != NULL && StrOp.equals( id, m_List->GetStringSelection().mb_str(wxConvUTF8) ) ) {
          m_Props = co;
          initValues();
          break;
        }
      }
    }
  }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CO_NEW
 */

void OutputDialog::OnButtonCoNewClick( wxCommandEvent& event )
{
  int i = m_List->FindString( _T("NEW") );
  if( i == wxNOT_FOUND ) {
    m_List->Append( _T("NEW") );
    iONode model = wxGetApp().getModel();
    if( model != NULL ) {
      iONode colist = wPlan.getcolist( model );
      if( colist == NULL ) {
        colist = NodeOp.inst( wOutputList.name(), model, ELEMENT_NODE );
        NodeOp.addChild( model, colist );
      }
      if( colist != NULL ) {
        iONode co = NodeOp.inst( wOutput.name(), colist, ELEMENT_NODE );
        NodeOp.addChild( colist, co );
        wOutput.setid( co, "NEW" );
        m_Props = co;
        initValues();
      }
    }
  }
  m_List->SetStringSelection( _T("NEW") );
  m_List->SetFirstItem( _T("NEW") );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CO_DELETE
 */

void OutputDialog::OnButtonCoDeleteClick( wxCommandEvent& event )
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
    iONode colist = wPlan.getsglist( model );
    if( colist != NULL ) {
      NodeOp.removeChild( colist, m_Props );
      m_Props = NULL;
    }
  }

  initIndex();
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void OutputDialog::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void OutputDialog::OnOkClick( wxCommandEvent& event )
{
  OnApplyClick(event);
  EndModal( wxID_OK );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
 */

void OutputDialog::OnApplyClick( wxCommandEvent& event )
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
  initIndex();
}


/*!
 * wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED event handler for ID_NOTEBOOK_CO
 */

void OutputDialog::OnNotebookCoPageChanged( wxNotebookEvent& event )
{
////@begin wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED event handler for ID_NOTEBOOK_CO in OutputDialog.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED event handler for ID_NOTEBOOK_CO in OutputDialog.
}



/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_OUTPUT_ACTIONS
 */

void OutputDialog::OnOutputActionsClick( wxCommandEvent& event )
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
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_OUTPUT_SWITCH
 */

void OutputDialog::OnOutputSwitchClick( wxCommandEvent& event )
{
  m_Gate->Enable(!m_AsSwitch->GetValue());
}

