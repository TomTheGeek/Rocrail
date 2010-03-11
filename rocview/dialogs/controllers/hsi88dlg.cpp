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
#pragma implementation "hsi88dlg.h"
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

#include "hsi88dlg.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/HSI88.h"
#include "rocview/public/guiapp.h"

////@begin XPM images
////@end XPM images

/*!
 * Hsi88CntrlDlg type definition
 */

IMPLEMENT_DYNAMIC_CLASS( Hsi88CntrlDlg, wxDialog )

/*!
 * Hsi88CntrlDlg event table definition
 */

BEGIN_EVENT_TABLE( Hsi88CntrlDlg, wxDialog )

////@begin Hsi88CntrlDlg event table entries
    EVT_BUTTON( wxID_OK, Hsi88CntrlDlg::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, Hsi88CntrlDlg::OnCancelClick )

////@end Hsi88CntrlDlg event table entries

END_EVENT_TABLE()

/*!
 * Hsi88CntrlDlg constructors
 */

Hsi88CntrlDlg::Hsi88CntrlDlg( )
{
}

Hsi88CntrlDlg::Hsi88CntrlDlg( wxWindow* parent, iONode props )
{
  Create(parent, -1, wxGetApp().getMsg("hsi88"));
  m_Props = props;
  initLabels();
  initValues();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  GetSizer()->Layout();
}

void Hsi88CntrlDlg::initLabels() {
  m_labIID->SetLabel( wxGetApp().getMsg( "iid" ) );
  m_labDevice->SetLabel( wxGetApp().getMsg( "port" ) );
  m_Type->SetLabel( wxGetApp().getMsg( "type" ) );
  m_ChainBox->SetLabel( wxGetApp().getMsg( "chain" ) );
  m_labLeft->SetLabel( wxGetApp().getMsg( "left" ) );
  m_labMid->SetLabel( wxGetApp().getMsg( "middle" ) );
  m_labRight->SetLabel( wxGetApp().getMsg( "right" ) );
}

void Hsi88CntrlDlg::initValues() {
  if( m_Props == NULL )
    return;
    
  m_IID->SetValue( wxString( wDigInt.getiid( m_Props ), wxConvUTF8 ) );
  m_Device->SetValue( wxString( wDigInt.getdevice( m_Props ), wxConvUTF8 ) );

  iONode hsi88ini = wDigInt.gethsi88(m_Props);
  if( hsi88ini == NULL ) {
    hsi88ini = NodeOp.inst( wHSI88.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild(m_Props, hsi88ini );
  }

  m_Type->SetSelection( wHSI88.isusb(hsi88ini) ? 1:0 );

  char* val = StrOp.fmt( "%d", wHSI88.getfbleft( hsi88ini ) );
  m_Left->SetValue( wxString( val, wxConvUTF8 ) );
  StrOp.free( val );
  val = StrOp.fmt( "%d", wHSI88.getfbmiddle( hsi88ini ) );
  m_Mid->SetValue( wxString( val, wxConvUTF8 ) );
  StrOp.free( val );
  val = StrOp.fmt( "%d", wHSI88.getfbright( hsi88ini ) );
  m_Right->SetValue( wxString( val, wxConvUTF8 ) );
  StrOp.free( val );
}


void Hsi88CntrlDlg::evaluate() {
  if( m_Props == NULL )
    return;
  wDigInt.setiid( m_Props, m_IID->GetValue().mb_str(wxConvUTF8) );
  wDigInt.setdevice( m_Props, m_Device->GetValue().mb_str(wxConvUTF8) );

  iONode hsi88ini = wDigInt.gethsi88(m_Props);
  if( hsi88ini == NULL ) {
    hsi88ini = NodeOp.inst( wHSI88.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild(m_Props, hsi88ini );
  }

  wHSI88.setusb( hsi88ini, m_Type->GetSelection() == 1 ? True:False );

  wHSI88.setfbleft( hsi88ini, atoi( m_Left->GetValue().mb_str(wxConvUTF8) ) );
  wHSI88.setfbmiddle( hsi88ini, atoi( m_Mid->GetValue().mb_str(wxConvUTF8) ) );
  wHSI88.setfbright( hsi88ini, atoi( m_Right->GetValue().mb_str(wxConvUTF8) ) );
}


  
/*!
 * Hsi88CntrlDlg creator
 */

bool Hsi88CntrlDlg::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin Hsi88CntrlDlg member initialisation
    m_labIID = NULL;
    m_IID = NULL;
    m_labDevice = NULL;
    m_Device = NULL;
    m_Type = NULL;
    m_ChainBox = NULL;
    m_labLeft = NULL;
    m_labMid = NULL;
    m_labRight = NULL;
    m_Left = NULL;
    m_Mid = NULL;
    m_Right = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
////@end Hsi88CntrlDlg member initialisation

////@begin Hsi88CntrlDlg creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end Hsi88CntrlDlg creation
    return true;
}

/*!
 * Control creation for Hsi88CntrlDlg
 */

void Hsi88CntrlDlg::CreateControls()
{    
////@begin Hsi88CntrlDlg content construction
    Hsi88CntrlDlg* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxPanel* itemPanel3 = new wxPanel( itemDialog1, ID_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    itemBoxSizer2->Add(itemPanel3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    itemPanel3->SetSizer(itemBoxSizer4);

    wxFlexGridSizer* itemFlexGridSizer5 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer5->AddGrowableCol(1);
    itemBoxSizer4->Add(itemFlexGridSizer5, 0, wxGROW|wxALL, 5);

    m_labIID = new wxStaticText( itemPanel3, ID_STATICTEXT_HSI_IID, _("IID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_labIID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_IID = new wxTextCtrl( itemPanel3, ID_TEXTCTRL_HSI_IID, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_IID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labDevice = new wxStaticText( itemPanel3, ID_STATICTEXT_HSI_DEVICE, _("Device"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_labDevice, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Device = new wxTextCtrl( itemPanel3, ID_TEXTCTRL_HSI_DEVICE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_Device, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_TypeStrings;
    m_TypeStrings.Add(_("&RS232"));
    m_TypeStrings.Add(_("&USB"));
    m_Type = new wxRadioBox( itemPanel3, wxID_ANY, _("Type"), wxDefaultPosition, wxDefaultSize, m_TypeStrings, 1, wxRA_SPECIFY_ROWS );
    m_Type->SetSelection(0);
    itemBoxSizer4->Add(m_Type, 0, wxGROW|wxALL, 5);

    m_ChainBox = new wxStaticBox(itemPanel3, wxID_ANY, _("Chains"));
    wxStaticBoxSizer* itemStaticBoxSizer11 = new wxStaticBoxSizer(m_ChainBox, wxVERTICAL);
    itemBoxSizer4->Add(itemStaticBoxSizer11, 0, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer12 = new wxFlexGridSizer(2, 3, 0, 0);
    itemStaticBoxSizer11->Add(itemFlexGridSizer12, 0, wxGROW, 5);

    m_labLeft = new wxStaticText( itemPanel3, wxID_STATIC_HSI_LEFT, wxGetTranslation(wxString() + (wxChar) 0x013B + wxT("eft bus")), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_labLeft, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_labMid = new wxStaticText( itemPanel3, wxID_STATIC_HSI_MID, _("Middle bus"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_labMid, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_labRight = new wxStaticText( itemPanel3, wxID_STATIC_HSI_RIGHT, _("Right bus"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_labRight, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Left = new wxTextCtrl( itemPanel3, ID_TEXTCTRL_HSI_LEFT, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_Left->SetMaxLength(5);
    itemFlexGridSizer12->Add(m_Left, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Mid = new wxTextCtrl( itemPanel3, ID_TEXTCTRL_HSI_MID, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_Mid->SetMaxLength(5);
    itemFlexGridSizer12->Add(m_Mid, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Right = new wxTextCtrl( itemPanel3, ID_TEXTCTRL_HSI_RIGHT, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_Right->SetMaxLength(5);
    itemFlexGridSizer12->Add(m_Right, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer19 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer19, 0, wxALIGN_RIGHT|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer19->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer19->AddButton(m_Cancel);

    itemStdDialogButtonSizer19->Realize();

////@end Hsi88CntrlDlg content construction
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void Hsi88CntrlDlg::OnOkClick( wxCommandEvent& event )
{
  evaluate();
  EndModal( wxID_OK );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void Hsi88CntrlDlg::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}

/*!
 * Should we show tooltips?
 */

bool Hsi88CntrlDlg::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap Hsi88CntrlDlg::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin Hsi88CntrlDlg bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end Hsi88CntrlDlg bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon Hsi88CntrlDlg::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin Hsi88CntrlDlg icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end Hsi88CntrlDlg icon retrieval
}
