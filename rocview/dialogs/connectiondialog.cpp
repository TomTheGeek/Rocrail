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
#pragma implementation "connectiondialog.h"
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

#include "connectiondialog.h"

#include "rocview/public/guiapp.h"

#include "rocview/wrapper/public/Gui.h"
#include "rocview/wrapper/public/RRCon.h"

////@begin XPM images
////@end XPM images

/*!
 * ConnectionDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( ConnectionDialog, wxDialog )

/*!
 * ConnectionDialog event table definition
 */

BEGIN_EVENT_TABLE( ConnectionDialog, wxDialog )

////@begin ConnectionDialog event table entries
    EVT_COMBOBOX( ID_COMBOBOX_CONN_HOST, ConnectionDialog::OnComboboxConnHostSelected )
    EVT_TEXT( ID_COMBOBOX_CONN_HOST, ConnectionDialog::OnComboboxConnHostUpdated )

    EVT_BUTTON( wxID_OK, ConnectionDialog::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, ConnectionDialog::OnCancelClick )

////@end ConnectionDialog event table entries

END_EVENT_TABLE()

/*!
 * ConnectionDialog constructors
 */

ConnectionDialog::ConnectionDialog( )
{
}

ConnectionDialog::ConnectionDialog( wxWindow* parent, iONode props )
{
  m_TabAlign = wxGetApp().getTabAlign();
  Create(parent, -1, wxGetApp().getMsg( "connect" ) );
  m_Props = props;
  initLabels();
  initValues();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
  GetSizer()->Layout();
}


void ConnectionDialog::initLabels() {
  m_LabelHost->SetLabel( wxGetApp().getMsg( "host" ) );
  m_LabelPort->SetLabel( wxGetApp().getMsg( "port" ) );

  // Buttons
  m_OK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
}


void ConnectionDialog::initValues() {
  iONode rrcon = wGui.getrrcon( m_Props );

  while( rrcon != NULL ) {
    if( StrOp.equals( wGui.gethost( m_Props ), wRRCon.gethost( rrcon ) ) ) {
      char* val = StrOp.fmt( "%d", wRRCon.getport( rrcon ) );
      m_Port->SetValue( wxString(val,wxConvUTF8) ); StrOp.free( val );
    }
    m_Host->Append( wxString( wRRCon.gethost(rrcon),wxConvUTF8 ) );
    rrcon = wGui.nextrrcon( m_Props, rrcon );
  }
  m_Host->SetStringSelection( wxString( wGui.gethost(m_Props),wxConvUTF8 ) );
}

wxString ConnectionDialog::getHostname() {
  return m_Host->GetValue();
}
int ConnectionDialog::getPort() {
  return atoi( m_Port->GetValue().mb_str(wxConvUTF8) );
}

void ConnectionDialog::evaluate() {
  bool exist = false;
  char* hostname = StrOp.dup( m_Host->GetValue().mb_str(wxConvUTF8) );
  iONode rrcon = wGui.getrrcon( m_Props );

  while( rrcon != NULL ) {
    if( StrOp.equals( hostname, wRRCon.gethost( rrcon ) ) ) {
      wGui.sethost( m_Props, hostname );
      wRRCon.setport( rrcon, atoi( m_Port->GetValue().mb_str(wxConvUTF8) ) );
      exist = true;
      break;
    }
    rrcon = wGui.nextrrcon( m_Props, rrcon );
  }
  if( !exist ) {
    iONode newrrcon = NodeOp.inst( wRRCon.name(), m_Props, ELEMENT_NODE );
    wRRCon.sethost( newrrcon, hostname );
    wRRCon.setport( newrrcon, atoi( m_Port->GetValue().mb_str(wxConvUTF8) ) );
    NodeOp.addChild( m_Props, newrrcon );
    wGui.sethost( m_Props, hostname );
  }
}

/*!
 * ConnectionDialog creator
 */

bool ConnectionDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin ConnectionDialog member initialisation
    m_LabelHost = NULL;
    m_Host = NULL;
    m_LabelPort = NULL;
    m_Port = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
////@end ConnectionDialog member initialisation

////@begin ConnectionDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end ConnectionDialog creation
    return true;
}

/*!
 * Control creation for ConnectionDialog
 */

void ConnectionDialog::CreateControls()
{
////@begin ConnectionDialog content construction
    ConnectionDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxFlexGridSizer* itemFlexGridSizer3 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer2->Add(itemFlexGridSizer3, 0, wxGROW|wxALL, 5);

    m_LabelHost = new wxStaticText( itemDialog1, wxID_STATIC_CON_HOST, _("Hostname"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_LabelHost, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_HostStrings;
    m_Host = new wxComboBox( itemDialog1, ID_COMBOBOX_CONN_HOST, _("localhost"), wxDefaultPosition, wxSize(200, -1), m_HostStrings, wxCB_DROPDOWN );
    m_Host->SetStringSelection(_("localhost"));
    itemFlexGridSizer3->Add(m_Host, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelPort = new wxStaticText( itemDialog1, wxID_STATIC_CONN_PORT, _("Port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_LabelPort, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Port = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_CONN_PORT, _("8051"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_Port->SetMaxLength(5);
    itemFlexGridSizer3->Add(m_Port, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemFlexGridSizer3->AddGrowableCol(1);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer8 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer8, 0, wxALIGN_RIGHT|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer8->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer8->AddButton(m_Cancel);

    itemStdDialogButtonSizer8->Realize();

////@end ConnectionDialog content construction
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void ConnectionDialog::OnOkClick( wxCommandEvent& event )
{
  evaluate();
  EndModal( wxID_OK );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void ConnectionDialog::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}

/*!
 * Should we show tooltips?
 */

bool ConnectionDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap ConnectionDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin ConnectionDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end ConnectionDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon ConnectionDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin ConnectionDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end ConnectionDialog icon retrieval
}
/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_CONN_HOST
 */

void ConnectionDialog::OnComboboxConnHostSelected( wxCommandEvent& event )
{
  char* hostname = StrOp.dup( m_Host->GetValue().mb_str(wxConvUTF8) );
  iONode rrcon = wGui.getrrcon( m_Props );

  while( rrcon != NULL ) {
    if( StrOp.equals( hostname, wRRCon.gethost( rrcon ) ) ) {
      char* val = StrOp.fmt( "%d", wRRCon.getport( rrcon ) );
      m_Port->SetValue( wxString(val,wxConvUTF8) ); StrOp.free( val );
      break;
    }
    rrcon = wGui.nextrrcon( m_Props, rrcon );
  }
}


/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_COMBOBOX_CONN_HOST
 */

void ConnectionDialog::OnComboboxConnHostUpdated( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_TEXT_UPDATED event handler for ID_COMBOBOX_CONN_HOST in ConnectionDialog.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_TEXT_UPDATED event handler for ID_COMBOBOX_CONN_HOST in ConnectionDialog.
}


