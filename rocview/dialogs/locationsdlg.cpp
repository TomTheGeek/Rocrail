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
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "locationsdlg.h"
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

#include "locationsdlg.h"

#include "rocs/public/strtok.h"
#include "rocs/public/trace.h"

#include "rocview/public/guiapp.h"
#include "rocrail/wrapper/public/Location.h"
#include "rocrail/wrapper/public/LocationList.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Stage.h"

////@begin XPM images
////@end XPM images

/*!
 * LocationsDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( LocationsDialog, wxDialog )

/*!
 * LocationsDialog event table definition
 */

BEGIN_EVENT_TABLE( LocationsDialog, wxDialog )

////@begin LocationsDialog event table entries
    EVT_LISTBOX( ID_LISTBOX_LOCATIONS_LOCATIONS, LocationsDialog::OnListboxLocationsLocationsSelected )

    EVT_LISTBOX( ID_LISTBOX_LOCATIONS_BLOCKS, LocationsDialog::OnListboxLocationsBlocksSelected )

    EVT_BUTTON( ID__LOCATIONS_BLOCK_ADD, LocationsDialog::OnLocationsBlockAddClick )

    EVT_BUTTON( ID__LOCATIONS_BLOCK_DELETE, LocationsDialog::OnLocationsBlockDeleteClick )

    EVT_BUTTON( ID_BUTTON_LOCATIONS_NEW, LocationsDialog::OnButtonLocationsNewClick )

    EVT_BUTTON( ID_BUTTON_LOCATIONS_MODIFY, LocationsDialog::OnButtonLocationsModifyClick )

    EVT_BUTTON( ID_BUTTON_LOCATIONS_DELETE, LocationsDialog::OnButtonLocationsDeleteClick )

    EVT_BUTTON( wxID_OK, LocationsDialog::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, LocationsDialog::OnCancelClick )

    EVT_BUTTON( wxID_APPLY, LocationsDialog::OnApplyClick )

////@end LocationsDialog event table entries

END_EVENT_TABLE()

/*!
 * LocationsDialog constructors
 */

LocationsDialog::LocationsDialog( )
{
}

LocationsDialog::LocationsDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

LocationsDialog::LocationsDialog( wxWindow* parent, iONode p_Props )
{
  m_TabAlign = wxGetApp().getTabAlign();
  Create(parent, -1, wxGetApp().getMsg("locationtable"));
  m_Props = p_Props;
  initLabels();
  initIndex();

  // After filling the listbox we must trigger this event again.
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
  GetSizer()->Layout();
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


void LocationsDialog::initLabels() {

  // Labels
  m_LabLocations->SetLabel( wxGetApp().getMsg( "locations" ) );
  m_LabName->SetLabel( wxGetApp().getMsg( "name" ) );
  m_LabBlocks->SetLabel( wxGetApp().getMsg( "blocks" ) );

  // Buttons
  m_New->SetLabel( wxGetApp().getMsg( "new" ) );
  m_Modify->SetLabel( wxGetApp().getMsg( "modify" ) );
  m_Delete->SetLabel( wxGetApp().getMsg( "delete" ) );

  m_AddBlock->SetLabel( wxGetApp().getMsg( "add" ) );
  m_DeleteBlock->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_labMinOcc->SetLabel( wxGetApp().getMsg( "minocc" ) );

  m_OK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
  m_Apply->SetLabel( wxGetApp().getMsg( "apply" ) );

  iOList list = ListOp.inst();

  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode bklist = wPlan.getbklist( model );
    if( bklist != NULL ) {
      int cnt = NodeOp.getChildCnt( bklist );
      for( int i = 0; i < cnt; i++ ) {
        iONode bk = NodeOp.getChild( bklist, i );
        ListOp.add(list, (obj)bk);
      }
    }
  }

  ListOp.sort(list, &__sortID);
  m_BlockCombo->Clear();

  int cnt = ListOp.size( list );
  for( int i = 0; i < cnt; i++ ) {
    iONode bk = (iONode)ListOp.get( list, i );
    m_BlockCombo->Append( wxString(wItem.getid( bk ),wxConvUTF8), bk );
  }

  /* clean up the temp. list */
  ListOp.base.del(list);


}

void LocationsDialog::initIndex() {

  int selected = m_LocationList->GetSelection();
  // Listbox
  m_LocationList->Clear();
  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode locationlist = wPlan.getlocationlist( model );
    if( locationlist != NULL ) {
      int cnt = NodeOp.getChildCnt( locationlist );
      for( int i = 0; i < cnt; i++ ) {
        iONode location = NodeOp.getChild( locationlist, i );
        m_LocationList->Append( wxString(wLocation.getid( location ),wxConvUTF8), location );
      }
    }
  }

  if( selected != wxNOT_FOUND && m_LocationList->GetCount() >  0 ) {
    m_LocationList->SetSelection( selected );
    m_Props = (iONode)m_LocationList->GetClientData( selected );
  }
  initValues();

}

void LocationsDialog::initValues() {
  if( m_Props == NULL ) {
    return;
  }

  wxArrayInt ai;
  int cnt = m_BlockList->GetSelections(ai);
  for( int i = 0; i < cnt; i++ ) {
    m_BlockList->Deselect( ai.Item(i) );
  }

  m_Name->SetValue( m_LocationList->GetStringSelection() );

  m_MinOcc->SetValue(wLocation.getminocc( m_Props ));
  m_Fifo->SetValue(wLocation.isfifo( m_Props ));

  // Listbox
  m_BlockList->Clear();

  iOStrTok  blocks = StrTokOp.inst( wLocation.getblocks ( m_Props ), ',' );
  while( StrTokOp.hasMoreTokens( blocks ) ) {
    const char* tok = StrTokOp.nextToken( blocks );
    m_BlockList->Append( wxString(tok,wxConvUTF8) );
    TraceOp.trc( "location", TRCLEVEL_INFO, __LINE__, 9999, "add [%s]", tok );
  };
  StrTokOp.base.del( blocks );
}


bool LocationsDialog::evaluate() {
  if( m_Props == NULL ) {
    return false;
  }

  if( m_Name->GetValue().Len() == 0 ) {
    wxMessageDialog( this, wxGetApp().getMsg("invalidid"), _T("Rocrail"), wxOK | wxICON_ERROR ).ShowModal();
    m_Name->SetValue( wxString(wItem.getid( m_Props ),wxConvUTF8) );
    return false;
  }
  wItem.setprev_id( m_Props, wItem.getid(m_Props) );

  wLocation.setminocc( m_Props, m_MinOcc->GetValue() );
  wLocation.setfifo(m_Props, m_Fifo->IsChecked()?True:False);

  int cnt = m_BlockList->GetCount();
  if( cnt > 0 ) {
    char* ids = StrOp.dup("");
    for( int i = 0; i < cnt; i++ ) {
      char* p = ids;
      if( StrOp.len(ids) == 0 )
        ids = StrOp.fmt( "%s", (const char*)m_BlockList->GetString(i).mb_str(wxConvUTF8) );
      else
        ids = StrOp.fmt( "%s,%s", ids, (const char*)m_BlockList->GetString(i).mb_str(wxConvUTF8) );
      StrOp.free( p );
    }
    wLocation.setblocks( m_Props, ids );
    StrOp.free( ids );
  }
  else
    wLocation.setblocks( m_Props, "" );

  wLocation.setid( m_Props, m_Name->GetValue().mb_str(wxConvUTF8) );

  return true;
}


/*!
 * LocationsDialog creator
 */

bool LocationsDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin LocationsDialog member initialisation
    m_LabLocations = NULL;
    m_LocationList = NULL;
    m_LabName = NULL;
    m_Name = NULL;
    m_LabBlocks = NULL;
    m_BlockList = NULL;
    m_BlockCombo = NULL;
    m_AddBlock = NULL;
    m_DeleteBlock = NULL;
    m_OptionsBox = NULL;
    m_labMinOcc = NULL;
    m_MinOcc = NULL;
    m_Fifo = NULL;
    m_New = NULL;
    m_Modify = NULL;
    m_Delete = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
    m_Apply = NULL;
////@end LocationsDialog member initialisation

////@begin LocationsDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end LocationsDialog creation
    return true;
}

/*!
 * Control creation for LocationsDialog
 */

void LocationsDialog::CreateControls()
{
////@begin LocationsDialog content construction
    LocationsDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxGROW|wxLEFT|wxRIGHT, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer3->Add(itemBoxSizer4, 0, wxGROW|wxALL, 5);

    m_LabLocations = new wxStaticText( itemDialog1, wxID_STATIC_LOCATIONS_LOCATIONS, _("Locations"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(m_LabLocations, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    wxArrayString m_LocationListStrings;
    m_LocationList = new wxListBox( itemDialog1, ID_LISTBOX_LOCATIONS_LOCATIONS, wxDefaultPosition, wxSize(200, -1), m_LocationListStrings, wxLB_SINGLE|wxLB_ALWAYS_SB|wxLB_SORT );
    itemBoxSizer4->Add(m_LocationList, 1, wxGROW|wxALL, 5);

    m_LabName = new wxStaticText( itemDialog1, wxID_STATIC_LOCATIONS_NAME, _("Name"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(m_LabName, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_Name = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_LOCATION_NAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(m_Name, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, 5);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer3->Add(itemBoxSizer9, 1, wxGROW|wxALL, 5);

    m_LabBlocks = new wxStaticText( itemDialog1, wxID_STATIC_LOCATIONS_BLOCKS, _("Blocks"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(m_LabBlocks, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    wxArrayString m_BlockListStrings;
    m_BlockList = new wxListBox( itemDialog1, ID_LISTBOX_LOCATIONS_BLOCKS, wxDefaultPosition, wxSize(200, 120), m_BlockListStrings, wxLB_SINGLE );
    itemBoxSizer9->Add(m_BlockList, 1, wxGROW|wxALL, 5);

    wxArrayString m_BlockComboStrings;
    m_BlockCombo = new wxComboBox( itemDialog1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_BlockComboStrings, wxCB_DROPDOWN );
    itemBoxSizer9->Add(m_BlockCombo, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer13 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer9->Add(itemBoxSizer13, 0, wxGROW|wxTOP, 5);

    m_AddBlock = new wxButton( itemDialog1, ID__LOCATIONS_BLOCK_ADD, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer13->Add(m_AddBlock, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_DeleteBlock = new wxButton( itemDialog1, ID__LOCATIONS_BLOCK_DELETE, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer13->Add(m_DeleteBlock, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_OptionsBox = new wxStaticBox(itemDialog1, wxID_ANY, _("Options"));
    wxStaticBoxSizer* itemStaticBoxSizer16 = new wxStaticBoxSizer(m_OptionsBox, wxVERTICAL);
    itemBoxSizer9->Add(itemStaticBoxSizer16, 0, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer17 = new wxFlexGridSizer(0, 2, 0, 0);
    itemStaticBoxSizer16->Add(itemFlexGridSizer17, 0, wxGROW, 5);

    m_labMinOcc = new wxStaticText( itemDialog1, wxID_ANY, _("MinOcc"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer17->Add(m_labMinOcc, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_MinOcc = new wxSpinCtrl( itemDialog1, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer17->Add(m_MinOcc, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Fifo = new wxCheckBox( itemDialog1, wxID_ANY, _("Fifo"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Fifo->SetValue(false);
    itemFlexGridSizer17->Add(m_Fifo, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer21 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer21, 0, wxGROW|wxLEFT|wxRIGHT, 5);

    m_New = new wxButton( itemDialog1, ID_BUTTON_LOCATIONS_NEW, _("New"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer21->Add(m_New, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Modify = new wxButton( itemDialog1, ID_BUTTON_LOCATIONS_MODIFY, _("Modify"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer21->Add(m_Modify, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Delete = new wxButton( itemDialog1, ID_BUTTON_LOCATIONS_DELETE, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer21->Add(m_Delete, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer25 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer25, 0, wxALIGN_RIGHT|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer25->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer25->AddButton(m_Cancel);

    m_Apply = new wxButton( itemDialog1, wxID_APPLY, _("&Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer25->AddButton(m_Apply);

    itemStdDialogButtonSizer25->Realize();

////@end LocationsDialog content construction
}

/*!
 * Should we show tooltips?
 */

bool LocationsDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap LocationsDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin LocationsDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end LocationsDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon LocationsDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin LocationsDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end LocationsDialog icon retrieval
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCATIONS_NEW
 */

void LocationsDialog::OnButtonLocationsNewClick( wxCommandEvent& event )
{
  int i = wxNOT_FOUND;
  if( !m_LocationList->IsEmpty() )
    i = m_LocationList->FindString( _T("NEW") );

  if( i == wxNOT_FOUND ) {
    m_LocationList->Append( _T("NEW") );
    iONode model = wxGetApp().getModel();
    if( model != NULL ) {
      iONode locationlist = wPlan.getlocationlist( model );
      if( locationlist == NULL ) {
        locationlist = NodeOp.inst( wLocationList.name(), model, ELEMENT_NODE );
        NodeOp.addChild( model, locationlist );
      }

      if( locationlist != NULL ) {
        iONode location = NodeOp.inst( wLocation.name(), locationlist, ELEMENT_NODE );
        NodeOp.addChild( locationlist, location );
        wLocation.setid( location, "NEW" );
        m_Props = location;
      }
    }
  }
  m_LocationList->SetStringSelection( _T("NEW") );
  m_LocationList->SetFirstItem( _T("NEW") );
  initValues();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCATIONS_DELETE
 */

void LocationsDialog::OnButtonLocationsDeleteClick( wxCommandEvent& event )
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
    iONode locationlist = wPlan.getlocationlist( model );
    if( locationlist != NULL ) {
      NodeOp.removeChild( locationlist, m_Props );
      m_Props = NULL;
    }
  }

  initIndex();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void LocationsDialog::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
 */

void LocationsDialog::OnApplyClick( wxCommandEvent& event )
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


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void LocationsDialog::OnOkClick( wxCommandEvent& event )
{
  OnApplyClick(event);
  EndModal( wxID_OK );
}


/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_LOCATIONS_LOCATIONS
 */

void LocationsDialog::OnListboxLocationsLocationsSelected( wxCommandEvent& event )
{
  m_Props = (iONode)m_LocationList->GetClientData(m_LocationList->GetSelection() );
  initValues();
}


/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_LOCATIONS_BLOCKS
 */

void LocationsDialog::OnListboxLocationsBlocksSelected( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_LOCATIONS_BLOCKS in LocationsDialog.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_LOCATIONS_BLOCKS in LocationsDialog.
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCATIONS_MODIFY
 */

void LocationsDialog::OnButtonLocationsModifyClick( wxCommandEvent& event )
{
  OnApplyClick(event);
}



/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID__LOCATIONS_BLOCK_ADD
 */

void LocationsDialog::OnLocationsBlockAddClick( wxCommandEvent& event )
{
  int sel = m_BlockCombo->GetSelection();
  if( sel != wxNOT_FOUND ) {
    m_BlockList->Append(m_BlockCombo->GetStringSelection());
  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID__LOCATIONS_BLOCK_DELETE
 */

void LocationsDialog::OnLocationsBlockDeleteClick( wxCommandEvent& event )
{
  int sel = m_BlockList->GetSelection();
  if( sel != wxNOT_FOUND ) {
    m_BlockList->Delete(sel);
  }
}

